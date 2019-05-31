/******************************************************************************

Copyright (c) 2014-2017 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_sched.h"
#include "csr_ui_lib.h"
#include "csr_app_lib.h"
#include "csr_util.h"
#include "csr_bt_pac_lib.h"
#include "csr_bt_pac_prim.h"
#include "csr_bt_pac_app.h"
#include "csr_bt_pac_app_strings.h"
#include "csr_bt_pac_app_task.h"
#include "csr_bt_pac_app_files.h"

/*Display a popup*/
void CsrBtPacAppUiPopup(CsrBtPacAppInstData_t *inst,
                        char *dialogHeading,
                        char *dialogText)
{
    CsrUiDialogSetReqSend(inst->ui.hCommonDialog,
                          CONVERT_TEXT_STRING_2_UCS2(dialogHeading),
                          CONVERT_TEXT_STRING_2_UCS2(dialogText),
                          NULL,
                          NULL,
                          0,
                          0,
                          inst->ui.hBackEvent,
                          0);

    CsrUiUieShowReqSend(inst->ui.hCommonDialog,
                        CSR_BT_PAC_APP_IFACEQUEUE,
                        CSR_UI_INPUTMODE_AUTO,
                        CSR_BT_PAC_APP_CSR_UI_PRI);

    inst->ui.popup = TRUE;
}

/*Display pop up when download is completed*/
void CsrBtPacAppDownloadedUiPopup(CsrBtPacAppInstData_t *inst)
{
    inst->downloadedPopUp = TRUE;

    CsrUiDialogSetReqSend(inst->ui.hCommonDialog,
                          NULL,
                          TEXT_PAC_APP_DOWNLOAD_COMPLETE_UCS2,
                          TEXT_OK_UCS2,
                          NULL,
                          inst->ui.hBackEvent,
                          0,
                          inst->ui.hSk1Event,
                          0);

    CsrUiUieShowReqSend(inst->ui.hCommonDialog,
                        CSR_BT_PAC_APP_IFACEQUEUE,
                        CSR_UI_INPUTMODE_AUTO,
                        CSR_BT_PAC_APP_CSR_UI_PRI);

    inst->ui.popup = TRUE;
}

/*To hide a popup*/
void CsrBtPacAppUiPopupHide(CsrBtPacAppInstData_t *inst)
{
    if (inst->ui.popup != FALSE)
    {
        CsrUiUieHideReqSend(inst->ui.hCommonDialog);
        inst->ui.popup = FALSE;
    }
}

void CsrBtPacAppHandleUiConnectDisconnect(CsrBtPacAppInstData_t *inst)
{
    CsrBtPacAppUiInstData *ui = &inst->ui;

    if (inst->btConnId != CSR_BT_CONN_ID_INVALID)
    {
        /* Update main menu so it is no longer possible to connect and activate
         * but just to disconnect, option to access phonebook and Settings option*/
        CsrUiMenuRemoveallitemsReqSend(ui->hMainMenu);

        CsrUiMenuAdditemReqSend(ui->hMainMenu,
                                CSR_UI_FIRST,
                                CSR_BT_PAC_APP_DISCONNECT_KEY,
                                CSR_UI_ICON_DISCONNECT,
                                TEXT_PAC_APP_DISCONNECT_UCS2,
                                CsrUtf82Utf16String((CsrUtf8String * )inst->remoteDeviceName),
                                ui->hSk1Event,
                                ui->hSk2Event,
                                ui->hBackEvent,
                                ui->hDelEvent);

        CsrUiMenuAdditemReqSend(ui->hMainMenu,
                                CSR_UI_LAST,
                                CSR_BT_PAC_APP_PHONEBOOK_KEY,
                                CSR_UI_ICON_BOOK,
                                TEXT_PAC_APP_PHONEBOOK_UCS2,
                                NULL,
                                ui->hSk1Event,
                                ui->hSk2Event,
                                ui->hBackEvent,
                                ui->hDelEvent);

        CsrUiMenuAdditemReqSend(ui->hMainMenu,
                                CSR_UI_LAST,
                                CSR_BT_PAC_APP_SETTINGS_KEY,
                                CSR_UI_ICON_TOOLS,
                                TEXT_PAC_APP_SETTINGS_UCS2,
                                NULL,
                                ui->hSk1Event,
                                ui->hSk2Event,
                                ui->hBackEvent,
                                ui->hDelEvent);

    }
    else
    {
        /* Update main menu so it is no longer possible to disconnect but just
         * to connect and activate and also to access the Settings menu. */
        CsrUiMenuRemoveallitemsReqSend(ui->hMainMenu);

        CsrUiMenuAdditemReqSend(ui->hMainMenu,
                                CSR_UI_FIRST,
                                CSR_BT_PAC_APP_CONNECT_KEY,
                                CSR_UI_ICON_CONNECT,
                                TEXT_PAC_APP_CONNECT_UCS2,
                                NULL,
                                ui->hSk1Event,
                                ui->hSk2Event,
                                ui->hBackEvent,
                                ui->hDelEvent);

        CsrUiMenuAdditemReqSend(ui->hMainMenu,
                                CSR_UI_LAST,
                                CSR_BT_PAC_APP_SETTINGS_KEY,
                                CSR_UI_ICON_TOOLS,
                                TEXT_PAC_APP_SETTINGS_UCS2,
                                NULL,
                                ui->hSk1Event,
                                ui->hSk2Event,
                                ui->hBackEvent,
                                ui->hDelEvent);
    }

    /* Remove popup */
    CsrBtPacAppUiPopupHide(inst);

    CsrUiDisplaySetinputmodeReqSend(ui->hMainMenu, CSR_UI_INPUTMODE_AUTO);
}

static void handleCsrUiInputDialogGetCfm(CsrBtPacAppInstData_t *inst,
                                         CsrUiInputdialogGetCfm *confirm)
{
    CsrBtPacAppUiInstData *ui = &inst->ui;

    /*Enter the user ID.*/
    if(ui->userId && inst->enterUserId == TRUE)
    {
        if (inst->state == AUTHENTICATE_STATE || inst->state == CONNECT_STATE)
        {
            inst->enterPassword     = TRUE;
            inst->enterUserId       = FALSE;

            inst->inputtext = (CsrUint8 *) CsrUtf16String2Utf8(confirm->text);
            inst->userId = CsrStrDup(inst->inputtext);

            CsrUiUieHideReqSend(ui->userId);

            CsrUiUieShowReqSend(inst->ui.password,
                                CSR_BT_PAC_APP_IFACEQUEUE,
                                CSR_UI_INPUTMODE_AUTO,
                                CSR_BT_PAC_APP_CSR_UI_PRI);
        }
    }
    /*Enter the password.*/
    else if (ui->password && inst->enterPassword == TRUE)
    {
        CsrUint8 *key;
        inst->inputtext = (CsrUint8 *) CsrUtf16String2Utf8(confirm->text);
        key = CsrStrDup(inst->inputtext);

        if (inst->userId != NULL)
            CsrBtPacAuthenticateResSend(key,
                                        (CsrUint16 )CsrStrLen(key),
                                        inst->userId);
        else
            CsrBtPacAuthenticateResSend(key,
                                        (CsrUint16 )CsrStrLen(key),
                                        NULL);

        CsrBtPacAppUiPopup(inst, "Connecting", "Please Wait");
        inst->connecting = TRUE;

        CsrUiUieHideReqSend(ui->password);
        inst->enterPassword = FALSE;
    }
    else if (ui->hStartOffset && inst->startOffset == TRUE)
    {
        CsrUint32 key;

        inst->inputtext = (CsrUint8 *) CsrUtf16String2Utf8(confirm->text);
        key = CsrStrToInt(inst->inputtext);
        inst->startOffsetVal = key;

        CsrUiUieHideReqSend(ui->hStartOffset);
        inst->startOffset = FALSE;
    }
    /*Enter the max list count.*/
    else if (ui->hMaxListCount && inst->maxListCount == TRUE)
    {
        CsrUint32 keyA;

        inst->inputtext = (CsrUint8 *) CsrUtf16String2Utf8(confirm->text);
        keyA = CsrStrToInt(inst->inputtext);
        inst->nMaxLstCnt = keyA;

        CsrUiUieHideReqSend(ui->hMaxListCount);
        inst->maxListCount = FALSE;
    }
    /*Enter the UID value.*/
    else if (ui->entryByUid && inst->entryByUid == TRUE)
    {
        CsrUint8 *keyEntry;
        CsrUint8 *uid;

        inst->state = DOWNLOAD_ENTRY_STATE;
        inst->entryByUid = FALSE;
        inst->inputtext = (CsrUint8 *) CsrUtf16String2Utf8(confirm->text);
        keyEntry = CsrStrDup(inst->inputtext);
        uid = CsrPmemAlloc(UID_LENGTH + 10);

        CsrMemSet(inst->UIDchar, '0', UID_LENGTH);
        CsrStrLCpy((char *) inst->UIDchar, "X-BT-UID:", UID_LENGTH);

        uid = CsrPmemAlloc(UID_LENGTH);

        CsrMemCpy(uid, inst->UIDchar, UID_LENGTH);
        CsrStrLCat(uid, keyEntry, UID_LENGTH);

        CsrBtPacPullVcardEntryReqSend(CsrUtf82Ucs2ByteString((CsrUint8* )uid),
                                      inst->filter,
                                      inst->vCardFormat,
                                      inst->srmp);

        CsrUiUieHideReqSend(ui->entryByUid);
    }
    /*For any other case.*/
    else
    {
        CsrUint32 i = 0;

        inst->inputtext = (CsrUint8 *) CsrUtf16String2Utf8(confirm->text);
        inst->pSearch = CsrStrDup(inst->inputtext);

        CsrUiUieHideReqSend(ui->hInputDialog);
    }

    CsrPmemFree(confirm->text);
}

static void addMainMenuOptionsUi(CsrBtPacAppInstData_t *inst)
{
    CsrBtPacAppUiInstData *ui = &inst->ui;

    CsrUiMenuSetReqSend(ui->hMainMenu,
                        CONVERT_TEXT_STRING_2_UCS2(PAC_PROFILE_NAME),
                        TEXT_SELECT_UCS2,
                        NULL);

    CsrUiMenuAdditemReqSend(ui->hMainMenu,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_CONNECT_KEY,
                            CSR_UI_ICON_CONNECT,
                            TEXT_PAC_APP_CONNECT_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hMainMenu,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_SETTINGS_KEY,
                            CSR_UI_ICON_TOOLS,
                            TEXT_PAC_APP_SETTINGS_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);
}

static void addSettingsMenuOptionsUi(CsrBtPacAppInstData_t *inst)
{
    CsrBtPacAppUiInstData *ui = &inst->ui;

    CsrUiMenuSetReqSend(ui->hSettingsMenu,
                        TEXT_PAC_APP_SETTINGS_UCS2,
                        TEXT_SELECT_UCS2,
                        NULL);

    CsrUiUieCreateReqSend(CSR_BT_PAC_APP_IFACEQUEUE, CSR_UI_UIETYPE_MENU);

    CsrUiMenuAdditemReqSend(ui->hSettingsMenu,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_DOWNLOAD_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_DOWNLOAD_PHONEBOOK_UCS2,
                            TEXT_PAC_APP_ON_UCS2,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hSettingsMenu,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_BROWSE_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_BROWSE_PHONEBOOK_UCS2,
                            TEXT_PAC_APP_OFF_UCS2,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hSettingsMenu,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_VCARD_FORMAT_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_VCARD_FORMAT_UCS2,
                            TEXT_PAC_FORMAT_VCARD2_1_UCS2,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hSettingsMenu,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_ORDER_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_ORDER_UCS2,
                            TEXT_PAC_APP_ORDER_ALPHABETICAL_UCS2,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hSettingsMenu,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_SEARCH_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_SEARCH_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hSettingsMenu,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_PROPERTY_SELECTOR_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_PROPERTY_SELECTOR_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hSettingsMenu,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_START_OFFSET_VALUE_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_START_OFFSET_VALUE_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hSettingsMenu,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_MAX_LIST_COUNT_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_MAX_LIST_COUNT_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hSettingsMenu,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_RESET_NEW_MISSED_CALLS_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_RESET_NEW_MISSED_CALLS_UCS2,
                            TEXT_PAC_APP_OFF_UCS2,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hSettingsMenu,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_VCARD_FILTER_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_VCARD_FILTER_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hSettingsMenu,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_SRMP_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_SRMP_UCS2,
                            TEXT_PAC_APP_DISABLED_UCS2,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hSettingsMenu,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_SET_TO_DEFAULT_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_SET_TO_DEFAULT_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);
}

static void addPhonebookMenuOptionsUi(CsrBtPacAppInstData_t *inst)
{
    CsrBtPacAppUiInstData *ui = &inst->ui;

    CsrUiMenuSetReqSend(inst->ui.hPhonebookMenu,
                        TEXT_PAC_APP_PHONEBOOK_UCS2,
                        TEXT_SELECT_UCS2,
                        TEXT_PAC_APP_SIM1_UCS2);

    CsrUiUieCreateReqSend(CSR_BT_PAC_APP_IFACEQUEUE, CSR_UI_UIETYPE_MENU);

    CsrUiMenuAdditemReqSend(ui->hPhonebookMenu,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_ALL_CONTACTS_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_ALL_CONTACTS_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hPhonebookMenu,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_INCOMING_CALLS_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_INCOMING_CALLS_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hPhonebookMenu,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_OUTGOING_CALLS_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_OUTGOING_CALLS_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hPhonebookMenu,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_MISSED_CALLS_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_MISSED_CALLS_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hPhonebookMenu,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_COMBINED_CALLS_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_COMBINED_CALLS_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hPhonebookMenu,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_SPEED_DIAL_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_SPEED_DIAL_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hPhonebookMenu,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_FAVORITES_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_FAVORITES_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hPhonebookMenu,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_ENTRY_BY_UID,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_ENTRY_BY_UID_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);
}

static void addSimMenuOptionsUi(CsrBtPacAppInstData_t *inst)
{
    CsrBtPacAppUiInstData *ui = &inst->ui;

    CsrUiMenuSetReqSend(inst->ui.hSimMenu,
                        TEXT_PAC_APP_SIM1_UCS2,
                        TEXT_SELECT_UCS2,
                        TEXT_PAC_APP_PHONEBOOK_UCS2);

    CsrUiUieCreateReqSend(CSR_BT_PAC_APP_IFACEQUEUE, CSR_UI_UIETYPE_MENU);

    CsrUiMenuAdditemReqSend(ui->hSimMenu,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_SIM1_ALL_CONTACTS_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_ALL_CONTACTS_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hSimMenu,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_SIM1_INCOMING_CALLS_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_INCOMING_CALLS_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hSimMenu,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_SIM1_OUTGOING_CALLS_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_OUTGOING_CALLS_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hSimMenu,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_SIM1_MISSED_CALLS_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_MISSED_CALLS_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hSimMenu,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_SIM1_COMBINED_CALLS_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_COMBINED_CALLS_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);
}

static void addSearchMenuOptionsUi(CsrBtPacAppInstData_t *inst)
{
    CsrBtPacAppUiInstData *ui = &inst->ui;

    CsrUiMenuSetReqSend(ui->hSearch,
                        TEXT_PAC_APP_SEARCH_UCS2,
                        TEXT_SELECT_UCS2,
                        NULL);

    CsrUiUieCreateReqSend(CSR_BT_PAC_APP_IFACEQUEUE, CSR_UI_UIETYPE_MENU);

    CsrUiMenuAdditemReqSend(ui->hSearch,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_PROPERTY_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_PROPERTY_UCS2,
                            TEXT_PAC_APP_NAME_UCS2,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hSearch,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_VALUE_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_VALUE_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);
}

static void addPrSelectorMenuOptions(CsrBtPacAppInstData_t *inst)
{
    CsrBtPacAppUiInstData *ui = &inst->ui;

    CsrUiMenuSetReqSend(ui->hPrSelector,
                        TEXT_PAC_APP_PROPERTY_SELECTOR_UCS2,
                        TEXT_SELECT_UCS2,
                        NULL);

    CsrUiUieCreateReqSend(CSR_BT_PAC_APP_IFACEQUEUE, CSR_UI_UIETYPE_MENU);

    CsrUiMenuAdditemReqSend(ui->hPrSelector,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_N_FILTER_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_NAME_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hPrSelector,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_VERSION_FILTER_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_VERSION_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hPrSelector,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_TELNO_FILTER_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_TELNO_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hPrSelector,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_EMAIL_FILTER_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_EMAIL_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hPrSelector,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_TITLE_FILTER_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_TITLE_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hPrSelector,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_ROLE_FILTER_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_ROLE_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hPrSelector,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_ORG_FILTER_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_ORG_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hPrSelector,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_NICKNAME_FILTER_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_NICKNAME_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hPrSelector,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_IMAGE_FILTER_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_IMAGE_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hPrSelector,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_SPEEDDIAL_FILTER_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_SPEEDDIAL_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hPrSelector,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_UID_FILTER_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_UID_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hPrSelector,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_UCI_FILTER_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_UCI_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);
}

static void addVCardFilterMenuOptionsUi(CsrBtPacAppInstData_t *inst)
{
    CsrBtPacAppUiInstData *ui = &inst->ui;

    CsrUiMenuSetReqSend(ui->hVCardFilter,
                        TEXT_PAC_APP_VCARD_FILTER_UCS2,
                        TEXT_SELECT_UCS2,
                        NULL);

    CsrUiUieCreateReqSend(CSR_BT_PAC_APP_IFACEQUEUE, CSR_UI_UIETYPE_MENU);

    CsrUiMenuAdditemReqSend(ui->hVCardFilter,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_VCARD_SELECTOR_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_VCARD_SELECTOR_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hVCardFilter,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_VCARD_SELECTOR_OPERATOR_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_VCARD_SELECTOR_OPERATOR_UCS2,
                            TEXT_PAC_APP_OR_UCS2,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);
}

static void addVCardSelMenuOptionsUi(CsrBtPacAppInstData_t *inst)
{
    CsrBtPacAppUiInstData *ui = &inst->ui;

    CsrUiMenuSetReqSend(ui->hVCardSel,
                        TEXT_PAC_APP_VCARD_SELECTOR_UCS2,
                        TEXT_SELECT_UCS2,
                        NULL);

    CsrUiUieCreateReqSend(CSR_BT_PAC_APP_IFACEQUEUE, CSR_UI_UIETYPE_MENU);

    CsrUiMenuAdditemReqSend(ui->hVCardSel,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_N_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_NAME_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hVCardSel,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_VERSION_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_VERSION_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hVCardSel,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_IMAGE_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_IMAGE_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hVCardSel,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_TELNO_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_TELNO_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hVCardSel,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_EMAIL_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_EMAIL_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hVCardSel,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_TITLE_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_TITLE_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hVCardSel,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_ROLE_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_ROLE_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hVCardSel,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_ORG_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_ORG_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hVCardSel,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_NICKNAME_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_NICKNAME_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hVCardSel,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_SPEEDDIALKEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_SPEEDDIAL_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hVCardSel,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_UID_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_UID_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuAdditemReqSend(ui->hVCardSel,
                            CSR_UI_LAST,
                            CSR_BT_PAC_APP_UCI_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_UCI_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);
}

static void showVCardListUi(CsrBtPacAppInstData_t *inst)
{
    CsrBtPacAppUiInstData *ui = &inst->ui;

    CsrUiUieCreateReqSend(CSR_BT_PAC_APP_IFACEQUEUE, CSR_UI_UIETYPE_MENU);
}

/* handler functions */
static void handleCsrUiUieCreateCfm(CsrBtPacAppInstData_t *inst,
                                    CsrUiUieCreateCfm *confirm)
{
    CsrBtPacAppUiInstData *ui = &inst->ui;

    switch (confirm->elementType)
    {
        case CSR_UI_UIETYPE_EVENT:
        {
            if (!ui->hBackEvent)
            {
                ui->hBackEvent = confirm->handle;

                CsrUiEventSetReqSend(ui->hBackEvent,
                                     CSR_UI_INPUTMODE_AUTO,
                                     CSR_BT_PAC_APP_IFACEQUEUE);

                CsrUiUieCreateReqSend(CSR_BT_PAC_APP_IFACEQUEUE,
                                      CSR_UI_UIETYPE_EVENT);
            }
            else if (!ui->hSk1Event)
            {
                ui->hSk1Event = confirm->handle;

                CsrUiEventSetReqSend(ui->hSk1Event,
                                     CSR_UI_INPUTMODE_AUTO,
                                     CSR_BT_PAC_APP_IFACEQUEUE);

                CsrUiUieCreateReqSend(CSR_BT_PAC_APP_IFACEQUEUE,
                                      CSR_UI_UIETYPE_EVENT);
            }
            else if (!ui->hSk2Event)
            {
                ui->hSk2Event = confirm->handle;

                CsrUiEventSetReqSend(ui->hSk2Event,
                                     CSR_UI_INPUTMODE_AUTO,
                                     CSR_BT_PAC_APP_IFACEQUEUE);

                CsrUiUieCreateReqSend(CSR_BT_PAC_APP_IFACEQUEUE,
                                      CSR_UI_UIETYPE_EVENT);

            }
            else if (!ui->hDelEvent)
            {
                ui->hDelEvent = confirm->handle;

                CsrUiEventSetReqSend(ui->hDelEvent,
                                     CSR_UI_INPUTMODE_AUTO,
                                     CSR_BT_PAC_APP_IFACEQUEUE);

                CsrUiUieCreateReqSend(CSR_BT_PAC_APP_IFACEQUEUE,
                                      CSR_UI_UIETYPE_EVENT);
            }
            else if (!ui->hInputSk1Event)
            {
                ui->hInputSk1Event = confirm->handle;

                CsrUiEventSetReqSend(ui->hInputSk1Event,
                                     CSR_UI_INPUTMODE_AUTO,
                                     CSR_BT_PAC_APP_IFACEQUEUE);

                CsrUiUieCreateReqSend(CSR_BT_PAC_APP_IFACEQUEUE,
                                      CSR_UI_UIETYPE_INPUTDIALOG);
            }
            break;
        }

        case CSR_UI_UIETYPE_INPUTDIALOG:
        {
            if (!ui->hInputDialog)
            {
                ui->hInputDialog = confirm->handle;

                CsrUiInputdialogSetReqSend(ui->hInputDialog,
                                           TEXT_PAC_APP_ENTER_VALUE_UCS2,
                                           TEXT_PAC_APP_VALUE_UCS2,
                                           CSR_UI_ICON_NONE,
                                           NULL,
                                           MAX_QUERY_LEN,
                                           CSR_UI_KEYMAP_ALPHANUMERIC,
                                           TEXT_OK_UCS2,
                                           NULL,
                                           ui->hInputSk1Event,
                                           0,
                                           ui->hBackEvent,
                                           0);

                CsrUiUieCreateReqSend(CSR_BT_PAC_APP_IFACEQUEUE,
                                      CSR_UI_UIETYPE_INPUTDIALOG);
            }
            else if (!ui->hMaxListCount)
            {
                ui->hMaxListCount = confirm->handle;

                CsrUiInputdialogSetReqSend(ui->hMaxListCount,
                                           TEXT_PAC_APP_ENTER_COUNT_UCS2,
                                           TEXT_PAC_APP_MAX_LIST_COUNT_UCS2,
                                           CSR_UI_ICON_NONE,
                                           NULL,
                                           MAX_QUERY_LEN,
                                           CSR_UI_KEYMAP_ALPHANUMERIC,
                                           TEXT_OK_UCS2,
                                           NULL,
                                           ui->hInputSk1Event,
                                           0,
                                           ui->hBackEvent,
                                           0);

                CsrUiUieCreateReqSend(CSR_BT_PAC_APP_IFACEQUEUE,
                                      CSR_UI_UIETYPE_INPUTDIALOG);
            }
            else if (!ui->hStartOffset)
            {
                ui->hStartOffset = confirm->handle;

                CsrUiInputdialogSetReqSend(ui->hStartOffset,
                                           TEXT_PAC_APP_ENTER_COUNT_UCS2,
                                           TEXT_PAC_APP_MAX_LIST_COUNT_UCS2,
                                           CSR_UI_ICON_NONE,
                                           NULL,
                                           MAX_QUERY_LEN,
                                           CSR_UI_KEYMAP_ALPHANUMERIC,
                                           TEXT_OK_UCS2,
                                           NULL,
                                           ui->hInputSk1Event,
                                           0,
                                           ui->hBackEvent,
                                           0);

                CsrUiUieCreateReqSend(CSR_BT_PAC_APP_IFACEQUEUE,
                                      CSR_UI_UIETYPE_INPUTDIALOG);
            }
            else if (!ui->userId)
            {
                ui->userId = confirm->handle;

                CsrUiInputdialogSetReqSend(ui->userId,
                                           TEXT_PAC_APP_ENTER_USERID_UCS2,
                                           NULL,
                                           CSR_UI_ICON_NONE,
                                           NULL,
                                           MAX_QUERY_LEN,
                                           CSR_UI_KEYMAP_ALPHANUMERIC,
                                           TEXT_OK_UCS2,
                                           NULL,
                                           ui->hInputSk1Event,
                                           0,
                                           ui->hBackEvent,
                                           0);

                CsrUiUieCreateReqSend(CSR_BT_PAC_APP_IFACEQUEUE,
                                      CSR_UI_UIETYPE_INPUTDIALOG);
            }
            else if (!ui->password)
            {
                ui->password = confirm->handle;

                CsrUiInputdialogSetReqSend(ui->password,
                                           TEXT_PAC_APP_ENTER_PASSWORD_UCS2,
                                           NULL,
                                           CSR_UI_ICON_NONE,
                                           NULL,
                                           MAX_QUERY_LEN,
                                           CSR_UI_KEYMAP_ALPHANUMERIC,
                                           TEXT_OK_UCS2,
                                           NULL,
                                           ui->hInputSk1Event,
                                           0,
                                           ui->hBackEvent,
                                           0);

                CsrUiUieCreateReqSend(CSR_BT_PAC_APP_IFACEQUEUE,
                                      CSR_UI_UIETYPE_INPUTDIALOG);
            }
            else if (!ui->entryByUid)
            {
                ui->entryByUid = confirm->handle;

                CsrUiInputdialogSetReqSend(ui->entryByUid,
                                           TEXT_PAC_APP_ENTER_UID_UCS2,
                                           NULL,
                                           CSR_UI_ICON_NONE,
                                           NULL,
                                           MAX_QUERY_LEN,
                                           CSR_UI_KEYMAP_ALPHANUMERIC,
                                           TEXT_OK_UCS2,
                                           NULL,
                                           ui->hInputSk1Event,
                                           0,
                                           ui->hBackEvent,
                                           0);

                CsrUiUieCreateReqSend(CSR_BT_PAC_APP_IFACEQUEUE,
                                      CSR_UI_UIETYPE_DIALOG);
            }
            break;
        }

        case CSR_UI_UIETYPE_DIALOG:
        {
            if (!ui->hCommonDialog)
            {
                ui->hCommonDialog = confirm->handle;

                CsrUiUieCreateReqSend(CSR_BT_PAC_APP_IFACEQUEUE,
                                      CSR_UI_UIETYPE_MENU);
            }
            break;
        }

        case CSR_UI_UIETYPE_MENU:
        {
            if (!ui->hSettingsMenu)
            {
                ui->hSettingsMenu = confirm->handle;

                addSettingsMenuOptionsUi(inst);
            }
            else if (!ui->hPhonebookMenu)
            {
                ui->hPhonebookMenu = confirm->handle;

                addPhonebookMenuOptionsUi(inst);
            }
            else if (!ui->hSimMenu)
            {
                ui->hSimMenu = confirm->handle;

                addSimMenuOptionsUi(inst);
            }
            else if (!ui->hSearch)
            {
                ui->hSearch = confirm->handle;

                addSearchMenuOptionsUi(inst);
            }
            else if (!ui->hPrSelector)
            {
                ui->hPrSelector = confirm->handle;

                addPrSelectorMenuOptions(inst);
            }
            else if (!ui->hVCardFilter)
            {
                ui->hVCardFilter = confirm->handle;

                addVCardFilterMenuOptionsUi(inst);
            }
            else if (!ui->hVCardSel)
            {
                ui->hVCardSel = confirm->handle;

                addVCardSelMenuOptionsUi(inst);
            }
            else if (!ui->hVCardList)
            {
                ui->hVCardList = confirm->handle;

                showVCardListUi(inst);
            }
            else if (!ui->hMainMenu)
            {
                ui->hMainMenu = confirm->handle;

                addMainMenuOptionsUi(inst);
            }
            break;
        }
    }
}

static void setFolder(CsrBtPacAppInstData_t *inst)
{
    /*Sets the folder to root and further setting of folders or other
     * requests are carried out in handleSetFolderRootCfm.*/
    CsrBtPacSetRootFolderReqSend();
}

/*Send request to get the listing output.*/
static void listReqHandler(CsrBtPacAppInstData_t *inst)
{
    CsrUcs2String *pName = NULL;

    pName = CsrUtf82Ucs2ByteString((CsrUint8*) inst->name);

    if (inst->browseContacts == TRUE)
    {
        CsrBtPacPullVcardListReqSendEx(pName, inst->order,
                                       inst->pSearch, inst->searchAtt,
                                       inst->nMaxLstCnt, inst->startOffsetVal,
                                       inst->resetNewMissedCall,
                                       inst->vCardSelector,
                                       inst->vCardSelectorOperator, inst->srmp);
    }
    else
    {
        inst->startBrowsing = TRUE;
        setFolder(inst);
    }
}

/*Set all the options to its default value.*/
static void defaultSettingsMenuUi(CsrBtPacAppInstData_t *inst)
{
    CsrBtPacAppUiInstData *ui = &inst->ui;

    inst->state         = DOWNLOAD_STATE;
    inst->nMaxLstCnt    = DEFAULT_MAX_LIST_COUNT;
    inst->startOffsetVal= 0;
    inst->srmp          = FALSE;
    inst->vCardFormat   = DEFAULT_VCARD_FORMAT;
    inst->order         = CSR_BT_PAC_ORDER_INDEXED;
    inst->searchAtt     = CSR_BT_PAC_SEARCH_ATT_NAME;
    inst->resetNewMissedCall    = 0x00;
    inst->vCardSelectorOperator = 0x00;

    CsrUiMenuSetitemReqSend(ui->hSettingsMenu,
                            CSR_BT_PAC_APP_SRMP_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_SRMP_UCS2,
                            TEXT_PAC_APP_DISABLED_UCS2,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuSetitemReqSend(ui->hSettingsMenu,
                            CSR_BT_PAC_APP_VCARD_FORMAT_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_VCARD_FORMAT_UCS2,
                            TEXT_PAC_FORMAT_VCARD2_1_UCS2,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuSetitemReqSend(ui->hSettingsMenu,
                            CSR_BT_PAC_APP_ORDER_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_ORDER_UCS2,
                            TEXT_PAC_APP_ORDER_INDEXED_UCS2,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuSetitemReqSend(ui->hSettingsMenu,
                            CSR_BT_PAC_APP_RESET_NEW_MISSED_CALLS_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_RESET_NEW_MISSED_CALLS_UCS2,
                            TEXT_PAC_APP_OFF_UCS2,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuSetitemReqSend(ui->hSettingsMenu,
                            CSR_BT_PAC_APP_DOWNLOAD_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_DOWNLOAD_PHONEBOOK_UCS2,
                            TEXT_PAC_APP_ON_UCS2,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuSetitemReqSend(ui->hSettingsMenu,
                            CSR_BT_PAC_APP_BROWSE_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_BROWSE_PHONEBOOK_UCS2,
                            TEXT_PAC_APP_OFF_UCS2,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuSetitemReqSend(ui->hVCardFilter,
                            CSR_BT_PAC_APP_VCARD_SELECTOR_OPERATOR_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_VCARD_SELECTOR_OPERATOR_UCS2,
                            TEXT_PAC_APP_OR_UCS2,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    /*Set vCardSelector Options to its default value*/
    inst->value = 0;
    inst->vCardSelector[0] = 0x00;
    inst->vCardSelector[1] = 0x00;
    inst->vCardSelector[2] = 0x00;
    inst->vCardSelector[3] = 0x00;
    inst->vCardSelector[4] = 0x00;
    inst->vCardSelector[5] = 0x00;
    inst->vCardSelector[6] = 0x00;
    inst->vCardSelector[7] = 0x00;

    CsrUiMenuSetitemReqSend(ui->hVCardSel,
                            CSR_BT_PAC_APP_IMAGE_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_IMAGE_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuSetitemReqSend(ui->hVCardSel,
                            CSR_BT_PAC_APP_TELNO_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_TELNO_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuSetitemReqSend(ui->hVCardSel,
                            CSR_BT_PAC_APP_EMAIL_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_EMAIL_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuSetitemReqSend(ui->hVCardSel,
                            CSR_BT_PAC_APP_TITLE_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_TITLE_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuSetitemReqSend(ui->hVCardSel,
                            CSR_BT_PAC_APP_ROLE_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_ROLE_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuSetitemReqSend(ui->hVCardSel,
                            CSR_BT_PAC_APP_ORG_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_ORG_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuSetitemReqSend(ui->hVCardSel,
                            CSR_BT_PAC_APP_NICKNAME_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_NICKNAME_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuSetitemReqSend(ui->hVCardSel,
                            CSR_BT_PAC_APP_SPEEDDIALKEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_SPEEDDIAL_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuSetitemReqSend(ui->hVCardSel,
                            CSR_BT_PAC_APP_UID_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_UID_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuSetitemReqSend(ui->hVCardSel,
                            CSR_BT_PAC_APP_UCI_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_UCI_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    /*Set Property Selector Options to its default value*/
    inst->valueFilter = 0;
    inst->filter[0] = 0xff;
    inst->filter[1] = 0xff;
    inst->filter[2] = 0xff;
    inst->filter[3] = 0xff;
    inst->filter[4] = 0x00;
    inst->filter[5] = 0x00;
    inst->filter[6] = 0x00;
    inst->filter[7] = 0x00;

    CsrUiMenuSetitemReqSend(ui->hPrSelector,
                            CSR_BT_PAC_APP_IMAGE_FILTER_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_IMAGE_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuSetitemReqSend(ui->hPrSelector,
                            CSR_BT_PAC_APP_TELNO_FILTER_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_TELNO_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuSetitemReqSend(ui->hPrSelector,
                            CSR_BT_PAC_APP_EMAIL_FILTER_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_EMAIL_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuSetitemReqSend(ui->hPrSelector,
                            CSR_BT_PAC_APP_TITLE_FILTER_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_TITLE_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuSetitemReqSend(ui->hPrSelector,
                            CSR_BT_PAC_APP_ROLE_FILTER_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_ROLE_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuSetitemReqSend(ui->hPrSelector,
                            CSR_BT_PAC_APP_ORG_FILTER_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_ORG_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuSetitemReqSend(ui->hPrSelector,
                            CSR_BT_PAC_APP_NICKNAME_FILTER_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_NICKNAME_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuSetitemReqSend(ui->hPrSelector,
                            CSR_BT_PAC_APP_SPEEDDIAL_FILTER_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_SPEEDDIAL_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuSetitemReqSend(ui->hPrSelector,
                            CSR_BT_PAC_APP_UID_FILTER_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_UID_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);

    CsrUiMenuSetitemReqSend(ui->hPrSelector,
                            CSR_BT_PAC_APP_UCI_FILTER_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_PAC_APP_UCI_UCS2,
                            NULL,
                            ui->hSk1Event,
                            ui->hSk2Event,
                            ui->hBackEvent,
                            ui->hDelEvent);
}

/*Toggle between the order i.e., Alphabetical, Indexed or Phonetical*/
static void handleOrderValue(CsrBtPacAppInstData_t *inst)
{
    CsrBtPacAppUiInstData *ui = &inst->ui;

    if (inst->order == CSR_BT_PAC_ORDER_ALPHABETICAL)
    {
        CsrUiMenuSetitemReqSend(ui->hSettingsMenu,
                                CSR_BT_PAC_APP_ORDER_KEY,
                                CSR_UI_ICON_NONE,
                                TEXT_PAC_APP_ORDER_UCS2,
                                TEXT_PAC_APP_ORDER_INDEXED_UCS2,
                                ui->hSk1Event,
                                ui->hSk2Event,
                                ui->hBackEvent,
                                ui->hDelEvent);

        inst->order = CSR_BT_PAC_ORDER_INDEXED;
    }
    else if (inst->order == CSR_BT_PAC_ORDER_INDEXED)
    {
        CsrUiMenuSetitemReqSend(ui->hSettingsMenu,
                                CSR_BT_PAC_APP_ORDER_KEY,
                                CSR_UI_ICON_NONE,
                                TEXT_PAC_APP_ORDER_UCS2,
                                TEXT_PAC_APP_ORDER_PHONETICAL_UCS2,
                                ui->hSk1Event,
                                ui->hSk2Event,
                                ui->hBackEvent,
                                ui->hDelEvent);

        inst->order = CSR_BT_PAC_ORDER_PHONETICAL;
    }
    else
    {
        CsrUiMenuSetitemReqSend(ui->hSettingsMenu,
                                CSR_BT_PAC_APP_ORDER_KEY,
                                CSR_UI_ICON_NONE,
                                TEXT_PAC_APP_ORDER_UCS2,
                                TEXT_PAC_APP_ORDER_ALPHABETICAL_UCS2,
                                ui->hSk1Event,
                                ui->hSk2Event,
                                ui->hBackEvent,
                                ui->hDelEvent);

        inst->order = CSR_BT_PAC_ORDER_ALPHABETICAL;
    }
}

/*On/Off toggle for Reset new missed calls option*/
static void handleResetNewMissedCalls(CsrBtPacAppInstData_t *inst)
{
    CsrBtPacAppUiInstData *ui = &inst->ui;
    if (inst->resetNewMissedCall == 0x01)
    {
        CsrUiMenuSetitemReqSend(ui->hSettingsMenu,
                                CSR_BT_PAC_APP_RESET_NEW_MISSED_CALLS_KEY,
                                CSR_UI_ICON_NONE,
                                TEXT_PAC_APP_RESET_NEW_MISSED_CALLS_UCS2,
                                TEXT_PAC_APP_OFF_UCS2,
                                ui->hSk1Event,
                                ui->hSk2Event,
                                ui->hBackEvent,
                                ui->hDelEvent);

        inst->resetNewMissedCall = 0x00;
    }
    else
    {
        CsrUiMenuSetitemReqSend(ui->hSettingsMenu,
                                CSR_BT_PAC_APP_RESET_NEW_MISSED_CALLS_KEY,
                                CSR_UI_ICON_NONE,
                                TEXT_PAC_APP_RESET_NEW_MISSED_CALLS_UCS2,
                                TEXT_PAC_APP_ON_UCS2,
                                ui->hSk1Event,
                                ui->hSk2Event,
                                ui->hBackEvent,
                                ui->hDelEvent);

        inst->resetNewMissedCall = 0x01;
    }
}

/*Toggle between the vCard formats i.e., either 2.1 or 3.0*/
static void handleVCardFormatValue(CsrBtPacAppInstData_t *inst)
{
    CsrBtPacAppUiInstData *ui = &inst->ui;

    if (inst->vCardFormat == CSR_BT_PAC_FORMAT_VCARD2_1)
    {
        inst->vCardFormat = CSR_BT_PAC_FORMAT_VCARD3_0;

        CsrUiMenuSetitemReqSend(ui->hSettingsMenu,
                                CSR_BT_PAC_APP_VCARD_FORMAT_KEY,
                                CSR_UI_ICON_NONE,
                                TEXT_PAC_APP_VCARD_FORMAT_UCS2,
                                TEXT_PAC_FORMAT_VCARD3_0_UCS2,
                                ui->hSk1Event,
                                ui->hSk2Event,
                                ui->hBackEvent,
                                ui->hDelEvent);
    }
    else
    {
        inst->vCardFormat = CSR_BT_PAC_FORMAT_VCARD2_1;

        CsrUiMenuSetitemReqSend(ui->hSettingsMenu,
                                CSR_BT_PAC_APP_VCARD_FORMAT_KEY,
                                CSR_UI_ICON_NONE,
                                TEXT_PAC_APP_VCARD_FORMAT_UCS2,
                                TEXT_PAC_FORMAT_VCARD2_1_UCS2,
                                ui->hSk1Event,
                                ui->hSk2Event,
                                ui->hBackEvent,
                                ui->hDelEvent);
    }
}

/*Toggle between Name / Number/ Sound for the property key*/
static void handleSearchAttValue(CsrBtPacAppInstData_t *inst)
{
    CsrBtPacAppUiInstData *ui = &inst->ui;

    if (inst->searchAtt == CSR_BT_PAC_SEARCH_ATT_NAME)
    {
        CsrUiMenuSetitemReqSend(ui->hSearch,
                                CSR_BT_PAC_APP_PROPERTY_KEY,
                                CSR_UI_ICON_NONE,
                                TEXT_PAC_APP_PROPERTY_UCS2,
                                TEXT_PAC_APP_NUMBER_UCS2,
                                ui->hSk1Event,
                                ui->hSk2Event,
                                ui->hBackEvent,
                                ui->hDelEvent);

        inst->searchAtt = CSR_BT_PAC_SEARCH_ATT_NUMBER;
    }
    else if (inst->searchAtt == CSR_BT_PAC_SEARCH_ATT_NUMBER)
    {
        CsrUiMenuSetitemReqSend(ui->hSearch,
                                CSR_BT_PAC_APP_PROPERTY_KEY,
                                CSR_UI_ICON_NONE,
                                TEXT_PAC_APP_PROPERTY_UCS2,
                                TEXT_PAC_APP_SOUND_UCS2,
                                ui->hSk1Event,
                                ui->hSk2Event,
                                ui->hBackEvent,
                                ui->hDelEvent);

        inst->searchAtt = CSR_BT_PAC_SEARCH_ATT_SOUND;
    }
    else if (inst->searchAtt == CSR_BT_PAC_SEARCH_ATT_SOUND)
    {
        CsrUiMenuSetitemReqSend(ui->hSearch,
                                CSR_BT_PAC_APP_PROPERTY_KEY,
                                CSR_UI_ICON_NONE,
                                TEXT_PAC_APP_PROPERTY_UCS2,
                                TEXT_PAC_APP_NAME_UCS2,
                                ui->hSk1Event,
                                ui->hSk2Event,
                                ui->hBackEvent,
                                ui->hDelEvent);

        inst->searchAtt = CSR_BT_PAC_SEARCH_ATT_NAME;
    }
}

/*Toggle between enabled or disabled for SRMP*/
static void handleSrmpValue(CsrBtPacAppInstData_t *inst)
{
    CsrBtPacAppUiInstData *ui = &inst->ui;

    if (inst->srmp == FALSE)
    {
        inst->srmp = TRUE;
        CsrUiMenuSetitemReqSend(ui->hSettingsMenu,
                                CSR_BT_PAC_APP_SRMP_KEY,
                                CSR_UI_ICON_NONE,
                                TEXT_PAC_APP_SRMP_UCS2,
                                TEXT_PAC_APP_ENABLED_UCS2,
                                ui->hSk1Event,
                                ui->hSk2Event,
                                ui->hBackEvent,
                                ui->hDelEvent);

    }
    else
    {
        inst->srmp = FALSE;
        CsrUiMenuSetitemReqSend(ui->hSettingsMenu,
                                CSR_BT_PAC_APP_SRMP_KEY,
                                CSR_UI_ICON_NONE,
                                TEXT_PAC_APP_SRMP_UCS2,
                                TEXT_PAC_APP_DISABLED_UCS2,
                                ui->hSk1Event,
                                ui->hSk2Event,
                                ui->hBackEvent,
                                ui->hDelEvent);

    }
}

/*Toggle between AND and OR for vCard Selector Operator*/
static void handleVCardSelectorOperator(CsrBtPacAppInstData_t *inst)
{
    CsrBtPacAppUiInstData *ui = &inst->ui;

    if (inst->vCardSelectorOperator == 0x00)
    {
        CsrUiMenuSetitemReqSend(ui->hVCardFilter,
                                CSR_BT_PAC_APP_VCARD_SELECTOR_OPERATOR_KEY,
                                CSR_UI_ICON_NONE,
                                TEXT_PAC_APP_VCARD_SELECTOR_OPERATOR_UCS2,
                                TEXT_PAC_APP_AND_UCS2,
                                ui->hSk1Event,
                                ui->hSk2Event,
                                ui->hBackEvent,
                                ui->hDelEvent);

        inst->vCardSelectorOperator = 0x01;
    }
    else
    {
        CsrUiMenuSetitemReqSend(ui->hVCardFilter,
                                CSR_BT_PAC_APP_VCARD_SELECTOR_OPERATOR_KEY,
                                CSR_UI_ICON_NONE,
                                TEXT_PAC_APP_VCARD_SELECTOR_OPERATOR_UCS2,
                                TEXT_PAC_APP_OR_UCS2,
                                ui->hSk1Event,
                                ui->hSk2Event,
                                ui->hBackEvent,
                                ui->hDelEvent);

        inst->vCardSelectorOperator = 0x00;
    }
}

/*Toggle between Downloading and Browsing*/
static void handleDownloadBrowseKeyValue(CsrBtPacAppInstData_t *inst)
{
    CsrBtPacAppUiInstData *ui = &inst->ui;

    if (inst->state == DOWNLOAD_STATE)
    {
        inst->state = BROWSING_STATE;

        CsrUiMenuSetitemReqSend(ui->hSettingsMenu,
                                CSR_BT_PAC_APP_DOWNLOAD_KEY,
                                CSR_UI_ICON_NONE,
                                TEXT_PAC_APP_DOWNLOAD_PHONEBOOK_UCS2,
                                TEXT_PAC_APP_OFF_UCS2,
                                ui->hSk1Event,
                                ui->hSk2Event,
                                ui->hBackEvent,
                                ui->hDelEvent);

        CsrUiMenuSetitemReqSend(ui->hSettingsMenu,
                                CSR_BT_PAC_APP_BROWSE_KEY,
                                CSR_UI_ICON_NONE,
                                TEXT_PAC_APP_BROWSE_PHONEBOOK_UCS2,
                                TEXT_PAC_APP_ON_UCS2,
                                ui->hSk1Event,
                                ui->hSk2Event,
                                ui->hBackEvent,
                                ui->hDelEvent);
    }
    else
    {
        inst->state = DOWNLOAD_STATE;

        CsrUiMenuSetitemReqSend(ui->hSettingsMenu,
                                CSR_BT_PAC_APP_DOWNLOAD_KEY,
                                CSR_UI_ICON_NONE,
                                TEXT_PAC_APP_DOWNLOAD_PHONEBOOK_UCS2,
                                TEXT_PAC_APP_ON_UCS2,
                                ui->hSk1Event,
                                ui->hSk2Event,
                                ui->hBackEvent,
                                ui->hDelEvent);

        CsrUiMenuSetitemReqSend(ui->hSettingsMenu,
                                CSR_BT_PAC_APP_BROWSE_KEY,
                                CSR_UI_ICON_NONE,
                                TEXT_PAC_APP_BROWSE_PHONEBOOK_UCS2,
                                TEXT_PAC_APP_OFF_UCS2,
                                ui->hSk1Event,
                                ui->hSk2Event,
                                ui->hBackEvent,
                                ui->hDelEvent);
    }
}

/*Handle Property Selector values.*/
static void handlePropertySelector(CsrBtPacAppInstData_t *inst,
                                CsrUiEventInd *indication)
{
    CsrBtPacAppUiInstData *ui = &inst->ui;
    CsrUint32 tmpval;
    CsrUint8 offset = 0; /* offset into inst->filter[] */

    switch (indication->key)
    {

        case CSR_BT_PAC_APP_N_FILTER_KEY:
        {
            inst->valueFilter = CSR_BT_PB_VCARD_PROP_N;

            CsrUiMenuSetitemReqSend(ui->hPrSelector,
                                    CSR_BT_PAC_APP_N_FILTER_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_PAC_APP_NAME_UCS2,
                                    TEXT_PAC_APP_ON_UCS2,
                                    ui->hSk1Event,
                                    ui->hSk2Event,
                                    ui->hBackEvent,
                                    ui->hDelEvent);
            break;
        }

        case CSR_BT_PAC_APP_VERSION_FILTER_KEY:
        {
            inst->valueFilter = CSR_BT_PB_VCARD_PROP_VERSION;

            CsrUiMenuSetitemReqSend(ui->hPrSelector,
                                    CSR_BT_PAC_APP_VERSION_FILTER_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_PAC_APP_VERSION_UCS2,
                                    TEXT_PAC_APP_ON_UCS2,
                                    ui->hSk1Event,
                                    ui->hSk2Event,
                                    ui->hBackEvent,
                                    ui->hDelEvent);
            break;
        }

        case CSR_BT_PAC_APP_IMAGE_FILTER_KEY:
        {
            inst->valueFilter = CSR_BT_PB_VCARD_PROP_PHOTO;

            CsrUiMenuSetitemReqSend(ui->hPrSelector,
                                    CSR_BT_PAC_APP_IMAGE_FILTER_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_PAC_APP_IMAGE_UCS2,
                                    TEXT_PAC_APP_ON_UCS2,
                                    ui->hSk1Event,
                                    ui->hSk2Event,
                                    ui->hBackEvent,
                                    ui->hDelEvent);
            break;
        }

        case CSR_BT_PAC_APP_TELNO_FILTER_KEY:
        {
            inst->valueFilter = CSR_BT_PB_VCARD_PROP_TEL;

            CsrUiMenuSetitemReqSend(ui->hPrSelector,
                                    CSR_BT_PAC_APP_TELNO_FILTER_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_PAC_APP_TELNO_UCS2,
                                    TEXT_PAC_APP_ON_UCS2,
                                    ui->hSk1Event,
                                    ui->hSk2Event,
                                    ui->hBackEvent,
                                    ui->hDelEvent);
            break;
        }

        case CSR_BT_PAC_APP_EMAIL_FILTER_KEY:
        {
            inst->valueFilter = CSR_BT_PB_VCARD_PROP_EMAIL;

            CsrUiMenuSetitemReqSend(ui->hPrSelector,
                                    CSR_BT_PAC_APP_EMAIL_FILTER_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_PAC_APP_EMAIL_UCS2,
                                    TEXT_PAC_APP_ON_UCS2,
                                    ui->hSk1Event,
                                    ui->hSk2Event,
                                    ui->hBackEvent,
                                    ui->hDelEvent);
            break;
        }

        case CSR_BT_PAC_APP_TITLE_FILTER_KEY:
        {
            inst->valueFilter = CSR_BT_PB_VCARD_PROP_TITLE;

            CsrUiMenuSetitemReqSend(ui->hPrSelector,
                                    CSR_BT_PAC_APP_TITLE_FILTER_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_PAC_APP_TITLE_UCS2,
                                    TEXT_PAC_APP_ON_UCS2,
                                    ui->hSk1Event,
                                    ui->hSk2Event,
                                    ui->hBackEvent,
                                    ui->hDelEvent);
            break;
        }

        case CSR_BT_PAC_APP_ROLE_FILTER_KEY:
        {
            inst->valueFilter = CSR_BT_PB_VCARD_PROP_ROLE;

            CsrUiMenuSetitemReqSend(ui->hPrSelector,
                                    CSR_BT_PAC_APP_ROLE_FILTER_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_PAC_APP_ROLE_UCS2,
                                    TEXT_PAC_APP_ON_UCS2,
                                    ui->hSk1Event,
                                    ui->hSk2Event,
                                    ui->hBackEvent,
                                    ui->hDelEvent);
            break;
        }

        case CSR_BT_PAC_APP_ORG_FILTER_KEY:
        {
            inst->valueFilter = CSR_BT_PB_VCARD_PROP_ORG;

            CsrUiMenuSetitemReqSend(ui->hPrSelector,
                                    CSR_BT_PAC_APP_ORG_FILTER_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_PAC_APP_ORG_UCS2,
                                    TEXT_PAC_APP_ON_UCS2,
                                    ui->hSk1Event,
                                    ui->hSk2Event,
                                    ui->hBackEvent,
                                    ui->hDelEvent);
            break;
        }

        case CSR_BT_PAC_APP_NICKNAME_FILTER_KEY:
        {
            inst->valueFilter = CSR_BT_PB_VCARD_PROP_NICKNAME;

            CsrUiMenuSetitemReqSend(ui->hPrSelector,
                                    CSR_BT_PAC_APP_NICKNAME_FILTER_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_PAC_APP_NICKNAME_UCS2,
                                    TEXT_PAC_APP_ON_UCS2,
                                    ui->hSk1Event,
                                    ui->hSk2Event,
                                    ui->hBackEvent,
                                    ui->hDelEvent);
            break;
        }

        case CSR_BT_PAC_APP_SPEEDDIAL_FILTER_KEY:
        {
            inst->valueFilter = CSR_BT_PB_VCARD_PROP_X_BT_SPEEDDIALKEY;

            CsrUiMenuSetitemReqSend(ui->hPrSelector,
                                    CSR_BT_PAC_APP_SPEEDDIAL_FILTER_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_PAC_APP_SPEEDDIAL_UCS2,
                                    TEXT_PAC_APP_ON_UCS2,
                                    ui->hSk1Event,
                                    ui->hSk2Event,
                                    ui->hBackEvent,
                                    ui->hDelEvent);
            break;
        }

        case CSR_BT_PAC_APP_UID_FILTER_KEY:
        {
            inst->valueFilter = CSR_BT_PB_VCARD_PROP_X_BT_UID;

            CsrUiMenuSetitemReqSend(ui->hPrSelector,
                                    CSR_BT_PAC_APP_UID_FILTER_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_PAC_APP_UID_UCS2,
                                    TEXT_PAC_APP_ON_UCS2,
                                    ui->hSk1Event,
                                    ui->hSk2Event,
                                    ui->hBackEvent,
                                    ui->hDelEvent);
            break;
        }

        case CSR_BT_PAC_APP_UCI_FILTER_KEY:
        {
            inst->valueFilter = CSR_BT_PB_VCARD_PROP_X_BT_UCI;

            CsrUiMenuSetitemReqSend(ui->hPrSelector,
                                    CSR_BT_PAC_APP_UCI_FILTER_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_PAC_APP_UCI_UCS2,
                                    TEXT_PAC_APP_ON_UCS2,
                                    ui->hSk1Event,
                                    ui->hSk2Event,
                                    ui->hBackEvent,
                                    ui->hDelEvent);
            break;
        }
    }

    tmpval = inst->valueFilter;

    while (tmpval >> 8)
    {
        tmpval >>= 8;
        offset++;
    }

    inst->valueFilter >>= 8 * offset;
    /* Now flip/xor the bit.*/
    inst->filter[offset] ^= inst->valueFilter;
}

/*Handle vCard Selector values*/
static void handleVCardSelector(CsrBtPacAppInstData_t *inst,
                                CsrUiEventInd *indication)
{
    CsrBtPacAppUiInstData *ui = &inst->ui;
    CsrUint32 tmpval;
    CsrUint8 offset = 0;

    switch (indication->key)
    {
        case CSR_BT_PAC_APP_N_KEY:
        {
            inst->value = CSR_BT_PB_VCARD_PROP_N;

            CsrUiMenuSetitemReqSend(ui->hVCardSel,
                                    CSR_BT_PAC_APP_N_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_PAC_APP_NAME_UCS2,
                                    TEXT_PAC_APP_ON_UCS2,
                                    ui->hSk1Event,
                                    ui->hSk2Event,
                                    ui->hBackEvent,
                                    ui->hDelEvent);
            break;
        }

        case CSR_BT_PAC_APP_VERSION_KEY:
        {
            inst->value = CSR_BT_PB_VCARD_PROP_VERSION;

            CsrUiMenuSetitemReqSend(ui->hVCardSel,
                                    CSR_BT_PAC_APP_VERSION_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_PAC_APP_VERSION_UCS2,
                                    TEXT_PAC_APP_ON_UCS2,
                                    ui->hSk1Event,
                                    ui->hSk2Event,
                                    ui->hBackEvent,
                                    ui->hDelEvent);
            break;
        }

        case CSR_BT_PAC_APP_IMAGE_KEY:
        {
            inst->value = CSR_BT_PB_VCARD_PROP_PHOTO;

            CsrUiMenuSetitemReqSend(ui->hVCardSel,
                                    CSR_BT_PAC_APP_IMAGE_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_PAC_APP_IMAGE_UCS2,
                                    TEXT_PAC_APP_ON_UCS2,
                                    ui->hSk1Event,
                                    ui->hSk2Event,
                                    ui->hBackEvent,
                                    ui->hDelEvent);
            break;
        }

        case CSR_BT_PAC_APP_TELNO_KEY:
        {
            inst->value = CSR_BT_PB_VCARD_PROP_TEL;

            CsrUiMenuSetitemReqSend(ui->hVCardSel,
                                    CSR_BT_PAC_APP_TELNO_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_PAC_APP_TELNO_UCS2,
                                    TEXT_PAC_APP_ON_UCS2,
                                    ui->hSk1Event,
                                    ui->hSk2Event,
                                    ui->hBackEvent,
                                    ui->hDelEvent);
            break;
        }

        case CSR_BT_PAC_APP_EMAIL_KEY:
        {
            inst->value = CSR_BT_PB_VCARD_PROP_EMAIL;

            CsrUiMenuSetitemReqSend(ui->hVCardSel,
                                    CSR_BT_PAC_APP_EMAIL_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_PAC_APP_EMAIL_UCS2,
                                    TEXT_PAC_APP_ON_UCS2,
                                    ui->hSk1Event,
                                    ui->hSk2Event,
                                    ui->hBackEvent,
                                    ui->hDelEvent);
            break;
        }

        case CSR_BT_PAC_APP_TITLE_KEY:
        {
            inst->value = CSR_BT_PB_VCARD_PROP_TITLE;

            CsrUiMenuSetitemReqSend(ui->hVCardSel,
                                    CSR_BT_PAC_APP_TITLE_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_PAC_APP_TITLE_UCS2,
                                    TEXT_PAC_APP_ON_UCS2,
                                    ui->hSk1Event,
                                    ui->hSk2Event,
                                    ui->hBackEvent,
                                    ui->hDelEvent);
            break;
        }

        case CSR_BT_PAC_APP_ROLE_KEY:
        {
            inst->value = CSR_BT_PB_VCARD_PROP_ROLE;

            CsrUiMenuSetitemReqSend(ui->hVCardSel,
                                    CSR_BT_PAC_APP_ROLE_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_PAC_APP_ROLE_UCS2,
                                    TEXT_PAC_APP_ON_UCS2,
                                    ui->hSk1Event,
                                    ui->hSk2Event,
                                    ui->hBackEvent,
                                    ui->hDelEvent);
            break;
        }

        case CSR_BT_PAC_APP_ORG_KEY:
        {
            inst->value = CSR_BT_PB_VCARD_PROP_ORG;

            CsrUiMenuSetitemReqSend(ui->hVCardSel,
                                    CSR_BT_PAC_APP_ORG_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_PAC_APP_ORG_UCS2,
                                    TEXT_PAC_APP_ON_UCS2,
                                    ui->hSk1Event,
                                    ui->hSk2Event,
                                    ui->hBackEvent,
                                    ui->hDelEvent);
            break;
        }

        case CSR_BT_PAC_APP_NICKNAME_KEY:
        {
            inst->value = CSR_BT_PB_VCARD_PROP_NICKNAME;

            CsrUiMenuSetitemReqSend(ui->hVCardSel,
                                    CSR_BT_PAC_APP_NICKNAME_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_PAC_APP_NICKNAME_UCS2,
                                    TEXT_PAC_APP_ON_UCS2,
                                    ui->hSk1Event,
                                    ui->hSk2Event,
                                    ui->hBackEvent,
                                    ui->hDelEvent);
            break;
        }

        case CSR_BT_PAC_APP_SPEEDDIALKEY:
        {
            inst->value = CSR_BT_PB_VCARD_PROP_X_BT_SPEEDDIALKEY;

            CsrUiMenuSetitemReqSend(ui->hVCardSel,
                                    CSR_BT_PAC_APP_SPEEDDIALKEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_PAC_APP_SPEEDDIAL_UCS2,
                                    TEXT_PAC_APP_ON_UCS2,
                                    ui->hSk1Event,
                                    ui->hSk2Event,
                                    ui->hBackEvent,
                                    ui->hDelEvent);
            break;
        }

        case CSR_BT_PAC_APP_UID_KEY:
        {
            inst->value = CSR_BT_PB_VCARD_PROP_X_BT_UID;

            CsrUiMenuSetitemReqSend(ui->hVCardSel,
                                    CSR_BT_PAC_APP_UID_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_PAC_APP_UID_UCS2,
                                    TEXT_PAC_APP_ON_UCS2,
                                    ui->hSk1Event,
                                    ui->hSk2Event,
                                    ui->hBackEvent,
                                    ui->hDelEvent);
            break;
        }

        case CSR_BT_PAC_APP_UCI_KEY:
        {
            inst->value = CSR_BT_PB_VCARD_PROP_X_BT_UCI;

            CsrUiMenuSetitemReqSend(ui->hVCardSel,
                                    CSR_BT_PAC_APP_UCI_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_PAC_APP_UCI_UCS2,
                                    TEXT_PAC_APP_ON_UCS2,
                                    ui->hSk1Event,
                                    ui->hSk2Event,
                                    ui->hBackEvent,
                                    ui->hDelEvent);
            break;
        }

    }

    tmpval = inst->value;

    while (tmpval >> 8)
    {
        tmpval >>= 8;
        offset++;
    }
    inst->value >>= 8 * offset;
    /* Now flip/xor the bit. */
    inst->vCardSelector[offset] ^= inst->value;
}

/*To download entire phonebook*/
static void handleDownloadState(CsrBtPacAppInstData_t *inst,
                                CsrUiEventInd *indication)
{
    CsrUint32 i = 0;

    while (i != CSR_BT_MAX_LENGTH)
    {
        inst->name[i] = '\0';
        inst->fileName[i] = '\0';
        i++;
    }

    /*To download an entry by entering UID value*/
    if(indication->key == CSR_BT_PAC_APP_ENTRY_BY_UID)
    {
        CsrUiUieShowReqSend(inst->ui.entryByUid,
                            CSR_BT_PAC_APP_IFACEQUEUE,
                            CSR_UI_INPUTMODE_AUTO,
                            CSR_BT_PAC_APP_CSR_UI_PRI);
    }
    else
    {
        switch (indication->key)
        {

            case CSR_BT_PAC_APP_ALL_CONTACTS_KEY:
            {
                CsrStrLCpy(inst->name, CSR_BT_PB_FOLDER_PB_VCF,
                           CSR_BT_MAX_LENGTH);

                CsrStrLCpy(inst->fileName, CSR_BT_PB_FOLDER_PB_VCF,
                           CSR_BT_MAX_LENGTH);
                break;
            }

            case CSR_BT_PAC_APP_SIM1_ALL_CONTACTS_KEY:
            {
                CsrStrLCpy(inst->name, CSR_BT_PB_FOLDER_PB_VCF,
                           CSR_BT_MAX_LENGTH);

                CsrStrLCpy(inst->fileName, CSR_BT_PAC_APP_FILE_SIM_PB,
                           CSR_BT_MAX_LENGTH);
                break;
            }

            case CSR_BT_PAC_APP_MISSED_CALLS_KEY:
            {
                CsrStrLCpy(inst->name, CSR_BT_PB_FOLDER_MCH_VCF,
                           CSR_BT_MAX_LENGTH);

                CsrStrLCpy(inst->fileName, CSR_BT_PB_FOLDER_MCH_VCF,
                           CSR_BT_MAX_LENGTH);
                break;
            }

            case CSR_BT_PAC_APP_SIM1_MISSED_CALLS_KEY:
            {
                CsrStrLCpy(inst->name, CSR_BT_PB_FOLDER_MCH_VCF,
                           CSR_BT_MAX_LENGTH);

                CsrStrLCpy(inst->fileName, CSR_BT_PAC_APP_FILE_SIM_MCH,
                           CSR_BT_MAX_LENGTH);
                break;
            }

            case CSR_BT_PAC_APP_INCOMING_CALLS_KEY:
            {
                CsrStrLCpy(inst->name, CSR_BT_PB_FOLDER_ICH_VCF,
                           CSR_BT_MAX_LENGTH);

                CsrStrLCpy(inst->fileName, CSR_BT_PB_FOLDER_ICH_VCF,
                           CSR_BT_MAX_LENGTH);
                break;
            }

            case CSR_BT_PAC_APP_SIM1_INCOMING_CALLS_KEY:
            {
                CsrStrLCpy(inst->name, CSR_BT_PB_FOLDER_ICH_VCF,
                           CSR_BT_MAX_LENGTH);

                CsrStrLCpy(inst->fileName, CSR_BT_PAC_APP_FILE_SIM_ICH,
                           CSR_BT_MAX_LENGTH);
                break;
            }

            case CSR_BT_PAC_APP_OUTGOING_CALLS_KEY:
            {
                CsrStrLCpy(inst->name, CSR_BT_PB_FOLDER_OCH_VCF,
                           CSR_BT_MAX_LENGTH);

                CsrStrLCpy(inst->fileName, CSR_BT_PB_FOLDER_OCH_VCF,
                           CSR_BT_MAX_LENGTH);
                break;
            }

            case CSR_BT_PAC_APP_SIM1_OUTGOING_CALLS_KEY:
            {
                CsrStrLCpy(inst->name, CSR_BT_PB_FOLDER_OCH_VCF,
                           CSR_BT_MAX_LENGTH);

                CsrStrLCpy(inst->fileName, CSR_BT_PAC_APP_FILE_SIM_OCH,
                           CSR_BT_MAX_LENGTH);
                break;
            }

            case CSR_BT_PAC_APP_COMBINED_CALLS_KEY:
            {
                CsrStrLCpy(inst->name, CSR_BT_PB_FOLDER_CCH_VCF,
                           CSR_BT_MAX_LENGTH);

                CsrStrLCpy(inst->fileName, CSR_BT_PB_FOLDER_CCH_VCF,
                           CSR_BT_MAX_LENGTH);
                break;
            }

            case CSR_BT_PAC_APP_SIM1_COMBINED_CALLS_KEY:
            {
                CsrStrLCpy(inst->name, CSR_BT_PB_FOLDER_CCH_VCF,
                           CSR_BT_MAX_LENGTH);

                CsrStrLCpy(inst->fileName, CSR_BT_PAC_APP_FILE_SIM_CCH,
                           CSR_BT_MAX_LENGTH);
                break;
            }

            case CSR_BT_PAC_APP_SPEED_DIAL_KEY:
            {
                CsrStrLCpy(inst->name, CSR_BT_PB_FOLDER_SPD_VCF,
                           CSR_BT_MAX_LENGTH);

                CsrStrLCpy(inst->fileName, CSR_BT_PB_FOLDER_SPD_VCF,
                           CSR_BT_MAX_LENGTH);
                break;
            }

            case CSR_BT_PAC_APP_FAVORITES_KEY:
            {
                CsrStrLCpy(inst->name, CSR_BT_PB_FOLDER_FAV_VCF,
                           CSR_BT_MAX_LENGTH);

                CsrStrLCpy(inst->fileName, CSR_BT_PB_FOLDER_FAV_VCF,
                           CSR_BT_MAX_LENGTH);
                break;
            }
        }

        setFolder(inst);
    }
}

/*To display vCard listing*/
static void handleBrowsingState(CsrBtPacAppInstData_t *inst,
                                CsrUiEventInd *indication)
{
    CsrBtPacAppUiInstData *ui = &inst->ui;
    CsrUint32 i = 0;

    while (i != CSR_BT_MAX_LENGTH)
    {
        inst->name[i] = '\0';
        inst->fileName[i] = '\0';
        i++;
    }
    switch (indication->key)
    {

        case CSR_BT_PAC_APP_ALL_CONTACTS_KEY:
        {
            CsrUiMenuSetReqSend(ui->hVCardList,
                                TEXT_PAC_APP_ALL_CONTACTS_UCS2,
                                TEXT_SELECT_UCS2,
                                NULL);

            CsrStrLCpy(inst->name,CSR_BT_PB_FOLDER_PB_STR,CSR_BT_MAX_LENGTH);

            inst->entryFolder = CSR_BT_PB_FOLDER_PHONE_PB_ID;

            break;
        }

        case CSR_BT_PAC_APP_SIM1_ALL_CONTACTS_KEY:
        {
            CsrUiMenuSetReqSend(ui->hVCardList,
                                TEXT_PAC_APP_ALL_CONTACTS_UCS2,
                                TEXT_SELECT_UCS2,
                                NULL);

            CsrStrLCpy(inst->name,CSR_BT_PB_FOLDER_PB_STR,CSR_BT_MAX_LENGTH);

            inst->entryFolder = CSR_BT_PB_FOLDER_SIM_PB_ID;
            break;
        }

        case CSR_BT_PAC_APP_MISSED_CALLS_KEY:
        {
            CsrUiMenuSetReqSend(ui->hVCardList,
                                TEXT_PAC_APP_MISSED_CALLS_UCS2,
                                TEXT_SELECT_UCS2,
                                NULL);

            CsrStrLCpy(inst->name,CSR_BT_PB_FOLDER_MCH_STR,CSR_BT_MAX_LENGTH);

            inst->entryFolder = CSR_BT_PB_FOLDER_PHONE_MCH_ID;
            break;
        }

        case CSR_BT_PAC_APP_SIM1_MISSED_CALLS_KEY:
        {
            CsrUiMenuSetReqSend(ui->hVCardList,
                                TEXT_PAC_APP_MISSED_CALLS_UCS2,
                                TEXT_SELECT_UCS2,
                                NULL);

            CsrStrLCpy(inst->name,CSR_BT_PB_FOLDER_MCH_STR,CSR_BT_MAX_LENGTH);

            inst->entryFolder = CSR_BT_PB_FOLDER_SIM_MCH_ID;
            break;
        }

        case CSR_BT_PAC_APP_INCOMING_CALLS_KEY:
        {
            CsrUiMenuSetReqSend(ui->hVCardList,
                                TEXT_PAC_APP_INCOMING_CALLS_UCS2,
                                TEXT_SELECT_UCS2,
                                NULL);

            CsrStrLCpy(inst->name,CSR_BT_PB_FOLDER_ICH_STR,CSR_BT_MAX_LENGTH);

            inst->entryFolder = CSR_BT_PB_FOLDER_PHONE_ICH_ID;
            break;
        }

        case CSR_BT_PAC_APP_SIM1_INCOMING_CALLS_KEY:
        {
            CsrUiMenuSetReqSend(ui->hVCardList,
                                TEXT_PAC_APP_INCOMING_CALLS_UCS2,
                                TEXT_SELECT_UCS2,
                                NULL);

            CsrStrLCpy(inst->name,CSR_BT_PB_FOLDER_ICH_STR,CSR_BT_MAX_LENGTH);

            inst->entryFolder = CSR_BT_PB_FOLDER_SIM_ICH_ID;
            break;
        }

        case CSR_BT_PAC_APP_OUTGOING_CALLS_KEY:
        {
            CsrUiMenuSetReqSend(ui->hVCardList,
                                TEXT_PAC_APP_OUTGOING_CALLS_UCS2,
                                TEXT_SELECT_UCS2,
                                NULL);

            CsrStrLCpy(inst->name,CSR_BT_PB_FOLDER_OCH_STR,CSR_BT_MAX_LENGTH);

            inst->entryFolder = CSR_BT_PB_FOLDER_PHONE_OCH_ID;
            break;
        }

        case CSR_BT_PAC_APP_SIM1_OUTGOING_CALLS_KEY:
        {
            CsrUiMenuSetReqSend(ui->hVCardList,
                                TEXT_PAC_APP_OUTGOING_CALLS_UCS2,
                                TEXT_SELECT_UCS2,
                                NULL);

            CsrStrLCpy(inst->name,CSR_BT_PB_FOLDER_OCH_STR,CSR_BT_MAX_LENGTH);

            inst->entryFolder = CSR_BT_PB_FOLDER_SIM_OCH_ID;
            break;
        }

        case CSR_BT_PAC_APP_COMBINED_CALLS_KEY:
        {
            CsrUiMenuSetReqSend(ui->hVCardList,
                                TEXT_PAC_APP_COMBINED_CALLS_UCS2,
                                TEXT_SELECT_UCS2,
                                NULL);

            CsrStrLCpy(inst->name,CSR_BT_PB_FOLDER_CCH_STR,CSR_BT_MAX_LENGTH);

            inst->entryFolder = CSR_BT_PB_FOLDER_PHONE_CCH_ID;
            break;
        }

        case CSR_BT_PAC_APP_SIM1_COMBINED_CALLS_KEY:
        {
            CsrUiMenuSetReqSend(ui->hVCardList,
                                TEXT_PAC_APP_COMBINED_CALLS_UCS2,
                                TEXT_SELECT_UCS2,
                                NULL);

            CsrStrLCpy(inst->name,CSR_BT_PB_FOLDER_CCH_STR,CSR_BT_MAX_LENGTH);

            inst->entryFolder = CSR_BT_PB_FOLDER_SIM_CCH_ID;
            break;
        }

        case CSR_BT_PAC_APP_SPEED_DIAL_KEY:
        {
            CsrUiMenuSetReqSend(ui->hVCardList,
                                TEXT_PAC_APP_SPEED_DIAL_UCS2,
                                TEXT_SELECT_UCS2,
                                NULL);

            CsrStrLCpy(inst->name,CSR_BT_PB_FOLDER_SPD_STR,CSR_BT_MAX_LENGTH);

            inst->entryFolder = CSR_BT_PB_FOLDER_PHONE_SPD_ID;
            break;
        }

        case CSR_BT_PAC_APP_FAVORITES_KEY:
        {
            CsrUiMenuSetReqSend(ui->hVCardList,
                                TEXT_PAC_APP_FAVORITES_UCS2,
                                TEXT_SELECT_UCS2,
                                NULL);

            CsrStrLCpy(inst->name,CSR_BT_PB_FOLDER_FAV_STR,CSR_BT_MAX_LENGTH);
            inst->entryFolder = CSR_BT_PB_FOLDER_PHONE_FAV_ID;
            break;
        }

        default:
            break;
    }

    listReqHandler(inst);
}

/*handle various event indications*/
static void handleCsrUiUieEventInd(CsrBtPacAppInstData_t *inst,
                                   CsrUiEventInd *indication)
{
    CsrBtPacAppUiInstData *ui = &inst->ui;

    if (indication->event == ui->hBackEvent)
    {
        if (inst->connecting == TRUE)
        { /* We had initiated the connection; cancel it. */
            CsrBtPacCancelConnectReqSend();
            inst->connecting = FALSE;

            CsrBtPacAppUiPopupHide(inst);
        }
        else if(indication->displayElement == ui->hSimMenu)
        {
            inst->src = CSR_BT_PAC_SRC_PHONE;
            CsrUiUieHideReqSend(indication->displayElement);
        }
        else if (indication->displayElement == ui->hCommonDialog
                                && inst->downloadedPopUp == TRUE)
        {
            CsrBtPacAppUiPopupHide(inst);
            inst->downloadedPopUp = FALSE;
        }
        else if (indication->displayElement == ui->hCommonDialog)
        {
            CSR_UNUSED(inst);
            CsrBtPacAbortReqSend();
        }
        else
        {
            /* Hide whatever was on display. */
            CsrUiUieHideReqSend(indication->displayElement);
        }
    }

    /*Toggle between Phonebook and Sim1 on Soft Key 2 Event*/
    else if (indication->event == ui->hSk2Event)
    {
        if (indication->displayElement == ui->hPhonebookMenu && inst->state
                                                          == BROWSING_STATE)
        {
            inst->src = CSR_BT_PAC_SRC_SIM;

            setFolder(inst);

            CsrUiUieShowReqSend(inst->ui.hSimMenu,
                                CSR_BT_PAC_APP_IFACEQUEUE,
                                CSR_UI_INPUTMODE_AUTO,
                                CSR_BT_PAC_APP_CSR_UI_PRI);
        }
        else if (indication->displayElement == ui->hSimMenu && inst->state
                                                         == BROWSING_STATE)
        {
            inst->src = CSR_BT_PAC_SRC_PHONE;
            setFolder(inst);

            CsrUiUieShowReqSend(inst->ui.hPhonebookMenu,
                                CSR_BT_PAC_APP_IFACEQUEUE,
                                CSR_UI_INPUTMODE_AUTO,
                                CSR_BT_PAC_APP_CSR_UI_PRI);
        }
        else if (indication->displayElement == ui->hPhonebookMenu &&
                        inst->state == DOWNLOAD_STATE)
        {
            inst->src = CSR_BT_PAC_SRC_SIM;

            CsrUiUieShowReqSend(inst->ui.hSimMenu,
                                CSR_BT_PAC_APP_IFACEQUEUE,
                                CSR_UI_INPUTMODE_AUTO,
                                CSR_BT_PAC_APP_CSR_UI_PRI);
        }
        else if (indication->displayElement == ui->hSimMenu && inst->state
                                                         == DOWNLOAD_STATE)
        {
            inst->src = CSR_BT_PAC_SRC_PHONE;

            CsrUiUieShowReqSend(inst->ui.hPhonebookMenu,
                                CSR_BT_PAC_APP_IFACEQUEUE,
                                CSR_UI_INPUTMODE_AUTO,
                                CSR_BT_PAC_APP_CSR_UI_PRI);
        }
    }
    /*Access the Settings Menu from anywhere by clicking on Del button*/
    else if (indication->event == ui->hDelEvent)
    {
        CsrUiUieShowReqSend(inst->ui.hSettingsMenu,
                            CSR_BT_PAC_APP_IFACEQUEUE,
                            CSR_UI_INPUTMODE_AUTO,
                            CSR_BT_PAC_APP_CSR_UI_PRI);
    }
    /*Handle input dialogs*/
    else if (indication->event == ui->hInputSk1Event)
    {
        if (indication->displayElement == ui->userId)
        {
            CsrUiInputdialogGetReqSend(CSR_BT_PAC_APP_IFACEQUEUE,
                                       ui->userId);
        }
        else if (indication->displayElement == ui->password)
        {
            CsrUiInputdialogGetReqSend(CSR_BT_PAC_APP_IFACEQUEUE,
                                       ui->password);
        }
        else if (indication->displayElement == ui->hMaxListCount)
        {
            inst->maxListCount = TRUE;

            CsrUiInputdialogGetReqSend(CSR_BT_PAC_APP_IFACEQUEUE,
                                       ui->hMaxListCount);
        }
        else if (indication->displayElement == ui->hStartOffset)
        {
            inst->startOffset = TRUE;

            CsrUiInputdialogGetReqSend(CSR_BT_PAC_APP_IFACEQUEUE,
                                       ui->hStartOffset);
        }
        else if (indication->displayElement == ui->entryByUid)
        {
            inst->entryByUid = TRUE;

            CsrUiInputdialogGetReqSend(CSR_BT_PAC_APP_IFACEQUEUE,
                                       ui->entryByUid);
        }
        else if (indication->displayElement == ui->hInputDialog)
        {
            CsrUiInputdialogGetReqSend(CSR_BT_PAC_APP_IFACEQUEUE,
                                       ui->hInputDialog);
        }
    }

    else if(indication->displayElement == ui->hCommonDialog &&
                    (indication->event == ui->hSk1Event))
    {
        CsrUiUieHideReqSend(indication->displayElement);
    }

    /*handle Soft Key 1 event*/
    else if ((indication->displayElement == ui->hMainMenu) &&
                    (indication->event == ui->hSk1Event))
    {
        switch (indication->key)
        {
            /*Connect to the remote device.*/
            case CSR_BT_PAC_APP_CONNECT_KEY:
            {
                inst->state = CONNECT_STATE;

                CsrBtPacConnectReqSend(CSR_BT_PAC_APP_IFACEQUEUE,
                                         PAC_MAX_OBEX_PACKET_SIZE,
                                         inst->selectedDeviceAddr,
                                         0);

                CsrBtPacAppUiPopup(inst, "Connecting", "Please Wait");
                break;
            }

            /*Disconnect from the remote device.*/
            case CSR_BT_PAC_APP_DISCONNECT_KEY:
            {
                CsrBtPacAppUiPopup(inst, "Disconnecting", "Please wait");

                CsrBtPacDisconnectReqSend(TRUE);
                break;
            }

            /*Show the Settings Menu UI created*/
            case CSR_BT_PAC_APP_SETTINGS_KEY:
            {
                CsrUiUieShowReqSend(inst->ui.hSettingsMenu,
                                    CSR_BT_PAC_APP_IFACEQUEUE,
                                    CSR_UI_INPUTMODE_AUTO,
                                    CSR_BT_PAC_APP_CSR_UI_PRI);
                break;
            }

            case CSR_BT_PAC_APP_PHONEBOOK_KEY:
            {
                inst->src = CSR_BT_PAC_SRC_PHONE;

                CsrUiUieShowReqSend(inst->ui.hPhonebookMenu,
                                    CSR_BT_PAC_APP_IFACEQUEUE,
                                    CSR_UI_INPUTMODE_AUTO,
                                    CSR_BT_PAC_APP_CSR_UI_PRI);
                break;
            }

            default:
                break;
        }
    }
    /*For all items in the Settings menu*/
    else if ((indication->displayElement == ui->hSettingsMenu) &&
                              (indication->event = ui->hSk1Event))
    {
        switch (indication->key)
        {
            case CSR_BT_PAC_APP_DOWNLOAD_KEY:
            {
                handleDownloadBrowseKeyValue(inst);
                break;
            }

            case CSR_BT_PAC_APP_BROWSE_KEY:
            {
                handleDownloadBrowseKeyValue(inst);
                break;
            }

            case CSR_BT_PAC_APP_VCARD_FORMAT_KEY:
            {
                handleVCardFormatValue(inst);
                break;
            }

            case CSR_BT_PAC_APP_ORDER_KEY:
            {
                handleOrderValue(inst);
                break;
            }

            case CSR_BT_PAC_APP_SEARCH_KEY:
            {
                CsrUiUieShowReqSend(inst->ui.hSearch,
                                    CSR_BT_PAC_APP_IFACEQUEUE,
                                    CSR_UI_INPUTMODE_AUTO,
                                    CSR_BT_PAC_APP_CSR_UI_PRI);
                break;
            }

            case CSR_BT_PAC_APP_PROPERTY_SELECTOR_KEY:
            {
                CsrUiUieShowReqSend(inst->ui.hPrSelector,
                                    CSR_BT_PAC_APP_IFACEQUEUE,
                                    CSR_UI_INPUTMODE_AUTO,
                                    CSR_BT_PAC_APP_CSR_UI_PRI);
                break;
            }

            case CSR_BT_PAC_APP_MAX_LIST_COUNT_KEY:
            {
                CsrUiUieShowReqSend(ui->hMaxListCount,
                                    CSR_BT_PAC_APP_IFACEQUEUE,
                                    CSR_UI_INPUTMODE_AUTO,
                                    CSR_BT_PAC_APP_CSR_UI_PRI);
                break;
            }

            case CSR_BT_PAC_APP_START_OFFSET_VALUE_KEY:
            {
                CsrUiUieShowReqSend(ui->hStartOffset,
                                    CSR_BT_PAC_APP_IFACEQUEUE,
                                    CSR_UI_INPUTMODE_AUTO,
                                    CSR_BT_PAC_APP_CSR_UI_PRI);
                break;
            }

            case CSR_BT_PAC_APP_RESET_NEW_MISSED_CALLS_KEY:
            {
                handleResetNewMissedCalls(inst);
                break;
            }

            case CSR_BT_PAC_APP_VCARD_FILTER_KEY:
            {
                CsrUiUieShowReqSend(inst->ui.hVCardFilter,
                                    CSR_BT_PAC_APP_IFACEQUEUE,
                                    CSR_UI_INPUTMODE_AUTO,
                                    CSR_BT_PAC_APP_CSR_UI_PRI);
                break;
            }

            case CSR_BT_PAC_APP_SRMP_KEY:
            {
                handleSrmpValue(inst);
                break;
            }

            case CSR_BT_PAC_APP_SET_TO_DEFAULT_KEY:
            {
                defaultSettingsMenuUi(inst);
                break;
            }

        }

    }
    /*Search by Property and by value*/
    else if (indication->displayElement == ui->hSearch &&
                    (indication->event = ui->hSk1Event))
    {
        switch (indication->key)
        {
            case CSR_BT_PAC_APP_PROPERTY_KEY:
            {
                handleSearchAttValue(inst);
                break;
            }

            case CSR_BT_PAC_APP_VALUE_KEY:
            {
                CsrUiUieShowReqSend(ui->hInputDialog,
                                    CSR_BT_PAC_APP_IFACEQUEUE,
                                    CSR_UI_INPUTMODE_AUTO,
                                    CSR_BT_PAC_APP_CSR_UI_PRI);
                break;
            }

            default:
            {
                break;
            }

        }

    }
    else if (indication->displayElement == ui->hVCardFilter &&
                    (indication->event = ui->hSk1Event))
    {
        switch (indication->key)
        {

            case CSR_BT_PAC_APP_VCARD_SELECTOR_KEY:
            {
                CsrUiUieShowReqSend(ui->hVCardSel,
                                    CSR_BT_PAC_APP_IFACEQUEUE,
                                    CSR_UI_INPUTMODE_AUTO,
                                    CSR_BT_PAC_APP_CSR_UI_PRI);
                break;
            }

            case CSR_BT_PAC_APP_VCARD_SELECTOR_OPERATOR_KEY:
            {
                handleVCardSelectorOperator(inst);
                break;
            }

        }
    }
    else if (indication->displayElement == ui->hPrSelector &&
                    (indication->event = ui->hSk1Event))
    {
        handlePropertySelector(inst, inst->recvMsgP);
    }
    else if (indication->displayElement == ui->hVCardSel &&
                    (indication->event = ui->hSk1Event))
    {
        handleVCardSelector(inst, inst->recvMsgP);
    }
    /*To download a particular entry from the listing*/
    else if ((indication->displayElement == ui->hVCardList) &&
                    (indication->event == ui->hSk1Event))
    {
        CsrUint32 i = 0;

        for (i = 0; i < inst->nMaxLstCnt; i++)
        {
            if (indication->key == CSR_BT_PAC_APP_ENTRY_BY_UID + i)
            {
                inst->entrySubKey = i;
                inst->entryFromListing = TRUE;
                CsrBtPacSetRootFolderReqSend();
            }
        }
    }
    /*If user wants to download a phonebook or browse it*/
    else if (((indication->displayElement == ui->hPhonebookMenu) ||
                    (indication->displayElement == ui->hSimMenu))
                    && (indication->event = ui->hSk1Event))
    {
        if (inst->state == DOWNLOAD_STATE)
        {
            handleDownloadState(inst, inst->recvMsgP);
        }
        else if (inst->state == BROWSING_STATE)
        {
            handleBrowsingState(inst, inst->recvMsgP);
        }
    }
}

void CsrBtPacAppHandleUiPrim(CsrBtPacAppInstData_t *inst)
{
   CsrUint16 msg = *((CsrUint16 *) inst->recvMsgP);

   switch (msg)
   {
        case CSR_UI_UIE_CREATE_CFM:
        handleCsrUiUieCreateCfm(inst, inst->recvMsgP);
            break;

        case CSR_UI_INPUTDIALOG_GET_CFM:
        handleCsrUiInputDialogGetCfm(inst, inst->recvMsgP);
            break;

        case CSR_UI_KEYDOWN_IND:
            break;

        case CSR_UI_EVENT_IND:
        handleCsrUiUieEventInd(inst, inst->recvMsgP);
            break;

        default:
            break;
    }
}
