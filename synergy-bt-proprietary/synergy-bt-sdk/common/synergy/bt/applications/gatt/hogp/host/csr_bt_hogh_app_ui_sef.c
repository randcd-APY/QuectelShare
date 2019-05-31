/****************************************************************************

Copyright (c) 2011-2016 Qualcomm Technologies International, Ltd.
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
#include "csr_bt_hogh_app.h"
#include "csr_bt_hogh_lib.h"
#include "csr_bt_hogh_app_strings.h"
#include <stdio.h>


const static CsrCharString *hoghStatusString[] =
{
  "SCANNING",
  "CONNECTING",
  "SERVICE_DISCOVERY",
  "CONFIGURING",
  "READY",
  "DISCONNECTED",
  "RECONNECTING",
  "SERVICE_CHANGED",
};

static const CsrCharString *reportTypeString[] =
{ "Input Report",
  "Output Report",
  "Feature Report",
  "Boot Mouse Input",
  "Boot Keyboard Input",
  "Boot Keyboard Output",
  "Battery Level",
};

#define HOGH_APP_REPORT_TYPE_STRING(_reportType)                    \
    (reportTypeString[(_reportType) - CSR_BT_HOGH_REPORT_TYPE_INPUT])

#define HoghConnStatusString(_conn)                                 \
    (CONVERT_TEXT_STRING_2_UCS2(hoghStatusString[(_conn)->status]))


static CsrUtf16String *utf16StringHexStream(CsrUint8 *value, CsrUint16 length)
{
    CsrUtf16String *hexString;
    CsrUint16 i, len = 0;
    CsrCharString *tmp = (CsrCharString *) CsrPmemZalloc(sizeof(*tmp)
                                                         * 8
                                                         * length);
    for (i = 0; i < length; i++)
    {
        CsrUint16 number = value[i];

        len = (CsrUint16) CsrStrLen(tmp);
        CsrUInt16ToHex(number, tmp + len);

        tmp[len] = '0';
        tmp[len + 1] = 'x';
        tmp[len + 4] = ',';
        tmp[len + 5] = '\0';
    }
    tmp[len + 4] = 0;
    hexString = CsrUtf82Utf16String((CsrUtf8String *) tmp);
    CsrPmemFree(tmp);

    return (hexString);
}

static CsrBool convertStringToAddr(CsrUtf16String *utf16String, BD_ADDR_T *addr)
{
    CsrBool parsed = FALSE;
    CsrUint32 nap = 0, uap = 0, lap = 0;
    CsrCharString *charString = CsrUtf16String2Utf8(utf16String);

    if (sscanf(charString, "%X:%X:%X", &nap, &uap, &lap) == 3)
    {
        addr->nap = (CsrUint16) nap;
        addr->uap = (CsrUint8) uap;
        addr->lap = (CsrUint24) lap;
        parsed = TRUE;
    }

    CsrPmemFree(charString);

    return (parsed);
}

static void initializeConnUi(CsrBtHoghAppInstData *appInst,
                             CsrUiUieCreateCfm *cfm)
{
    CsrBtHoghAppConnection *conn = appInst->activeConnection;

    if (conn && cfm->elementType == CSR_UI_UIETYPE_MENU && !conn->hMenuConn)
    { /* Connection menu */
        conn->hMenuConn = cfm->handle;
    }
    else
    {
        /* Raise exception */
        CSR_LOG_TEXT_CONDITIONAL_ERROR(conn,
                                       (conn->appInst->logHandle,
                                        0,
                                        "Trying to initialize unknown connection with display handle = %X",
                                        cfm->handle));
    }
}

static void addReportMenuItems(CsrCmnListElm_t *elem, void *data)
{
    CsrBtHoghAppReportInfo *reportInfo = (CsrBtHoghAppReportInfo *) elem;
    CsrBtHoghAppService *service = (CsrBtHoghAppService *) data;

    if (service->protocol == CSR_BT_HIDS_BOOT_PROTOCOL
        && (reportInfo->reportType == CSR_BT_HOGH_REPORT_TYPE_INPUT
            || reportInfo->reportType == CSR_BT_HOGH_REPORT_TYPE_OUTPUT
            || reportInfo->reportType == CSR_BT_HOGH_REPORT_TYPE_FEATURE))
    {
        /* Ignore input/output/feature reports in Boot protocol */
    }
    else if (service->protocol == CSR_BT_HIDS_REPORT_PROTOCOL
             && (reportInfo->reportType == CSR_BT_HOGH_REPORT_TYPE_BOOT_MOUSE_INPUT
                 || reportInfo->reportType == CSR_BT_HOGH_REPORT_TYPE_BOOT_KEYBOARD_INPUT
                 || reportInfo->reportType == CSR_BT_HOGH_REPORT_TYPE_BOOT_KEYBOARD_OUTPUT))
    {
        /* Ignore Boot-Mouse-Input/Boot-Keyboard-Input/Boot-Keyboard-Output
         * reports in Report protocol */
    }
    else
    {
        CsrUtf16String *reportStr = NULL;
        CsrBtHoghAppConnection *conn = service->conn;
        CsrBtHoghAppInstData *appInst = conn->appInst;
        CsrUtf16String *reportTypeStr = CsrUtf82Utf16String(HOGH_APP_REPORT_TYPE_STRING(reportInfo->reportType));
        CsrUint16 key = CSR_BT_HOGH_APP_REPORT_KEY(conn->connId,
                                                   service->serviceId,
                                                   reportInfo->reportId,
                                                   reportInfo->reportType);

        if (reportInfo->report && reportInfo->reportLen)
        {
            reportStr = utf16StringHexStream(reportInfo->report,
                                             reportInfo->reportLen);
        }

        CsrUiMenuAdditemReqSend(conn->hMenuConn,
                                CSR_UI_LAST,
                                key,
                                CSR_UI_ICON_NONE,
                                reportTypeStr,
                                reportStr,
                                appInst->hEventSk1,
                                appInst->hEventSk2,
                                appInst->hEventBack,
                                appInst->hEventDel);
    }
}

static void addServiceMenuItems(CsrCmnListElm_t *elem, void *data)
{
    CsrBtHoghAppService *service = (CsrBtHoghAppService *) elem;

    CsrCmnListIterate(&service->reports,
                      addReportMenuItems,
                      service);
}

static void addConnMenuItems(CsrCmnListElm_t *elem, void *data)
{
    CsrUtf16String *deviceName;
    CsrBtHoghAppConnection *conn = (CsrBtHoghAppConnection *) elem;
    CsrBtHoghAppInstData *appInst = data;
    CsrUieHandle sk1, sk2;

    if (conn->deviceName)
    {
        deviceName = CsrUtf82Utf16String((CsrUtf8String *) conn->deviceName);
    }
    else
    {
        deviceName = CsrBtHoghAppAddrToString(&(conn->addr.addr));
    }

    if (conn->status == CSR_BT_HOGH_STATUS_READY)
    {
        sk1 = conn->hMenuConn;
        sk2 = appInst->hEventSk2;
    }
    else
    {
        sk1 = CSR_UI_DEFAULTACTION;
        sk2 = CSR_UI_DEFAULTACTION;
    }

    CsrUiMenuAdditemReqSend(appInst->hMenuMain,
                            CSR_UI_LAST,
                            CSR_BT_HOGH_APP_CONN_KEY(conn->connId),
                            CSR_UI_ICON_DEVICE,
                            deviceName,
                            HoghConnStatusString(conn),
                            sk1,
                            sk2,
                            appInst->hEventBack,
                            appInst->hEventDel);
}



static void mainUiEventHandler(CsrBtHoghAppInstData *appInst,
                               CsrUiEventInd *ind)
{
    if (ind->event == appInst->hEventSk2)
    {
        if (appInst->suspendState == CSR_BT_HIDS_SUSPEND)
        {
            appInst->suspendState = CSR_BT_HIDS_EXIT_SUSPEND;
        }
        else
        {
            appInst->suspendState = CSR_BT_HIDS_SUSPEND;
        }
        CsrBtHoghSetStateReqSend(appInst->suspendState);
    }
    else if (ind->event == appInst->hEventDel)
    {
        CsrBtHoghRemoveDeviceReqSend(CSR_BT_HOGH_APP_CONN_ID_FROM_KEY(ind->key));
    }
    else
    {
        /* Raise exception */
        CSR_LOG_TEXT_ERROR((appInst->logHandle,
                            0,
                            "UI event indication (key=%4X) received on main display",
                            ind->key));
    }
}

static void connUiEventHandler(CsrBtHoghAppConnection *conn,
                               CsrUiEventInd *ind)
{
    CsrUint8 serviceId = CSR_BT_HOGH_APP_SERV_ID_FROM_KEY(ind->key);
    CsrBtHoghAppService *service = HOGH_APP_FIND_SERVICE(conn, serviceId);

    if (service)
    {
        CsrUint8 reportId = CSR_BT_HOGH_APP_REPORT_ID_FROM_KEY(ind->key);
        CsrBtHoghReportType reportType = CSR_BT_HOGH_APP_REPORT_TYPE_FROM_KEY(ind->key);
        CsrBtHoghAppReportInfo *reportInfo = HOGH_APP_GET_FIRST_REPORT(service);

        while (reportInfo)
        {
            if (reportInfo->reportId == reportId
                && reportInfo->reportType == reportType)
            {
                break;
            }
            reportInfo = (CsrBtHoghAppReportInfo *) reportInfo->cmnListInfo.next;
        }

        if (reportInfo)
        {
            CsrBtHoghAppInstData *appInst = conn->appInst;

            if (ind->event == appInst->hEventDel)
            {
                if (service->protocol == CSR_BT_HIDS_BOOT_PROTOCOL)
                {
                    service->protocol = CSR_BT_HIDS_REPORT_PROTOCOL;
                }
                else
                {
                    service->protocol = CSR_BT_HIDS_BOOT_PROTOCOL;
                }

                CsrBtHoghSetProtocolReqSend(conn->connId,
                                            service->serviceId,
                                            service->protocol);

            }
            else if (ind->event == appInst->hEventSk2)
            {
                CsrUiUieShowReqSend(appInst->hInputDialogReport,
                                    CSR_BT_HOGH_APP_IFACEQUEUE,
                                    CSR_UI_INPUTMODE_AUTOPASS,
                                    CSR_BT_HOGH_APP_CSR_UI_PRI);
                appInst->activeReport = reportInfo;
            }
            else if (ind->event == appInst->hEventSk1)
            {
                CsrBtHoghGetReportReqSend(conn->connId,
                                          service->serviceId,
                                          reportType,
                                          reportId);
            }
            else
            {
                /* Raise exception */
                CSR_LOG_TEXT_ERROR((conn->appInst->logHandle,
                                    0,
                                    "Unknown UI event indication (key=%4X) received on connection display",
                                    ind->key));
            }
        }
        else
        {
            /* Raise exception */
            CSR_LOG_TEXT_ERROR((conn->appInst->logHandle,
                                0,
                                "UI event indication (key=%4X) received for unknown report",
                                ind->key));
        }
    }
    else
    {
        /* Raise exception */
        CSR_LOG_TEXT_ERROR((conn->appInst->logHandle,
                            0,
                            "UI event indication (key=%4X) received for unknown service",
                            ind->key));
    }
}

static void handleEventInd(CsrBtHoghAppInstData *appInst,
                           CsrUiEventInd *ind)
{
    if (ind->event == appInst->hEventBack)
    {
        /* hide current shown display element */
        CsrUiUieHideReqSend(ind->displayElement);
    }
    else if (ind->displayElement == appInst->hDialog)
    {
        CsrUiUieHideReqSend(ind->displayElement);
    }
    else if (ind->displayElement == appInst->hInputDialogAddress)
    {
        CsrUiUieHideReqSend(ind->displayElement);
        if (ind->event == appInst->hEventSk1)
        {
            CsrUiInputdialogGetReqSend(CSR_BT_HOGH_APP_IFACEQUEUE,
                                       ind->displayElement);
        }
        else
        {
            CsrBtHoghAddAnyDeviceReqSend(CSR_BT_HOGH_APP_PREFERRED_MTU,
                                         CSR_BT_LE_DEFAULT_CONN_ATTEMPT_TIMEOUT);
        }
    }
    else if (ind->displayElement == appInst->hInputDialogReport)
    {
        CsrUiUieHideReqSend(ind->displayElement);
        CsrUiInputdialogGetReqSend(CSR_BT_HOGH_APP_IFACEQUEUE,
                                   ind->displayElement);
    }
    else if (ind->displayElement == appInst->hMenuMain)
    {
        mainUiEventHandler(appInst, ind);
    }
    else
    {
        CsrBtHoghAppConnection *conn = HOGH_APP_FIND_CONN_BY_DISPLAY(appInst,
                                                                     ind->displayElement);

        if (conn)
        {
            connUiEventHandler(conn, ind);
        }
        else
        {
            /* Raise exception */
            CSR_LOG_TEXT_ERROR((appInst->logHandle,
                                0,
                                "Unknown UI event indication (key=%4X) received on display=%X",
                                ind->key,
                                ind->displayElement));
        }
    }
}

static void handleCreateCfm(CsrBtHoghAppInstData *appInst,
                            CsrUiUieCreateCfm *cfm)
{
    if (cfm->elementType == CSR_UI_UIETYPE_EVENT)
    {
        if (appInst->hEventBack == CSR_UI_DEFAULTACTION)
        {
            appInst->hEventBack = cfm->handle;
            CsrUiEventSetReqSend(cfm->handle,
                                 CSR_UI_INPUTMODE_AUTO,
                                 CSR_BT_HOGH_APP_IFACEQUEUE);
            CsrUiUieCreateReqSend(CSR_BT_HOGH_APP_IFACEQUEUE,
                                  CSR_UI_UIETYPE_EVENT);
        }
        else if (appInst->hEventSk1 == CSR_UI_DEFAULTACTION)
        {
            appInst->hEventSk1 = cfm->handle;
            CsrUiEventSetReqSend(cfm->handle,
                                 CSR_UI_INPUTMODE_AUTO,
                                 CSR_BT_HOGH_APP_IFACEQUEUE);
            CsrUiUieCreateReqSend(CSR_BT_HOGH_APP_IFACEQUEUE,
                                  CSR_UI_UIETYPE_EVENT);
        }
        else if (appInst->hEventSk2 == CSR_UI_DEFAULTACTION)
        {
            appInst->hEventSk2 = cfm->handle;
            CsrUiEventSetReqSend(cfm->handle,
                                 CSR_UI_INPUTMODE_AUTO,
                                 CSR_BT_HOGH_APP_IFACEQUEUE);
            CsrUiUieCreateReqSend(CSR_BT_HOGH_APP_IFACEQUEUE,
                                  CSR_UI_UIETYPE_EVENT);
        }
        else if (appInst->hEventDel == CSR_UI_DEFAULTACTION)
        {
            appInst->hEventDel = cfm->handle;
            CsrUiEventSetReqSend(cfm->handle,
                                 CSR_UI_INPUTMODE_AUTO,
                                 CSR_BT_HOGH_APP_IFACEQUEUE);
            CsrUiUieCreateReqSend(CSR_BT_HOGH_APP_IFACEQUEUE,
                                  CSR_UI_UIETYPE_DIALOG);
        }
    }
    else if (cfm->elementType == CSR_UI_UIETYPE_DIALOG)
    {
        appInst->hDialog = cfm->handle;
        CsrUiUieCreateReqSend(CSR_BT_HOGH_APP_IFACEQUEUE,
                              CSR_UI_UIETYPE_INPUTDIALOG);
    }
    else if (cfm->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        if (appInst->hInputDialogAddress == CSR_UI_DEFAULTACTION)
        {
            appInst->hInputDialogAddress = cfm->handle;
            CsrUiUieCreateReqSend(CSR_BT_HOGH_APP_IFACEQUEUE,
                                  CSR_UI_UIETYPE_INPUTDIALOG);
        }
        else
        {
            appInst->hInputDialogReport = cfm->handle;
            CsrUiInputdialogSetReqSend(appInst->hInputDialogReport,
                                       TEXT_HOGH_APP_SET_REPORT_UCS2,
                                       TEXT_HOGH_APP_SET_REPORT_LABEL_UCS2,
                                       CSR_UI_ICON_NONE,
                                       NULL,
                                       CSR_BT_HOGH_APP_MAX_REPORT_LEN,
                                       CSR_UI_KEYMAP_ALPHANUMERIC,
                                       TEXT_OK_UCS2,
                                       NULL,
                                       appInst->hEventSk1,
                                       CSR_UI_DEFAULTACTION,
                                       appInst->hEventBack,
                                       CSR_UI_DEFAULTACTION);
            CsrUiUieCreateReqSend(CSR_BT_HOGH_APP_IFACEQUEUE,
                                  CSR_UI_UIETYPE_MENU);
        }
    }
    else if (cfm->elementType == CSR_UI_UIETYPE_MENU
             && appInst->hMenuMain == CSR_UI_DEFAULTACTION)
    {
        appInst->hMenuMain = cfm->handle;
        CsrBtHoghAppUiRefreshMainMenu(appInst);
    }
    else
    { /* Initialise connection UI */
        initializeConnUi(appInst, cfm);
    }
}

static void handleInputDialogGetCfm(CsrBtHoghAppInstData *appInst, CsrUiInputdialogGetCfm *cfm)
{
    if (cfm->handle == appInst->hInputDialogAddress)
    {
        CsrBtTypedAddr addr;
        
        /* Need to get addr.type from select cfm 
         *  rather than assign to public type 
        addr.type = CSR_BT_ADDR_PUBLIC;
        convertStringToAddr(cfm->text, &addr.addr);
         */ 
        CsrBtHoghAddDeviceReqSend(appInst->addr,
                                  CSR_BT_HOGH_APP_PREFERRED_MTU,
                                  CSR_BT_LE_DEFAULT_CONN_ATTEMPT_TIMEOUT);
    }
    else if (cfm->handle == appInst->hInputDialogReport)
    {
        CsrBtHoghAppReportInfo *reportInfo = appInst->activeReport;

        if (reportInfo)
        {
            CsrBtHoghAppService *service = reportInfo->service;
            CsrBtHoghAppConnection *conn = service->conn;
            CsrUint16 reportLen = (CsrUtf16StrLen(cfm->text) + 1) / 2;
            CsrUint32 report = CsrUtf16StringToUint32(cfm->text);
            CsrUint8 *reportData = CsrPmemAlloc(sizeof(CsrUint32));

            CSR_COPY_UINT32_TO_LITTLE_ENDIAN(report, reportData);

            CsrBtHoghSetReportReqSend(conn->connId,
                                      service->serviceId,
                                      reportInfo->reportType,
                                      reportInfo->reportId,
                                      reportLen,
                                      reportData);
        }
        else
        {
            CSR_LOG_TEXT_ERROR((appInst->logHandle,
                                0,
                                "Input dialog get confirm received for unknown report"));
        }
    }
    else
    {
        CSR_LOG_TEXT_ERROR((appInst->logHandle,
                            0,
                            "Unknown Input dialog get confirm (handle=%X) received",
                            cfm->handle));
    }
}

void CsrBtHoghAppUiPrimHandler(CsrBtHoghAppInstData *appInst)
{
    switch (*(CsrUiPrim *) appInst->recvMsgP)
    {
        case CSR_UI_UIE_CREATE_CFM:
            handleCreateCfm(appInst, appInst->recvMsgP);
            break;
        case CSR_UI_EVENT_IND:
            handleEventInd(appInst, appInst->recvMsgP);
            break;
        case CSR_UI_INPUTDIALOG_GET_CFM:
            handleInputDialogGetCfm(appInst, appInst->recvMsgP);
            break;
        default:
            /* unexpected primitive received */
            CSR_LOG_TEXT_UNHANDLED_PRIMITIVE(appInst->logHandle,
                                             0,
                                             CSR_UI_PRIM,
                                             *(CsrUiPrim *) appInst->recvMsgP);

            CsrUiFreeUpstreamMessageContents(CSR_UI_PRIM,
                                             appInst->recvMsgP);
            break;
    }
}

void CsrBtHoghAppUiRefreshConnMenu(CsrBtHoghAppConnection *conn)
{
    CsrUiMenuRemoveallitemsReqSend(conn->hMenuConn);

    CsrUiMenuSetReqSend(conn->hMenuConn,
                        CsrUtf82Utf16String(conn->deviceName),
                        TEXT_HOGH_APP_GET_REPORT_UCS2,
                        TEXT_HOGH_APP_SET_REPORT_UCS2);

    CsrCmnListIterate(&conn->services,
                      addServiceMenuItems,
                      &conn);
}

void CsrBtHoghAppUiRefreshMainMenu(CsrBtHoghAppInstData *appInst)
{
    CsrUtf16String *sk2Text;

    CsrUiMenuRemoveallitemsReqSend(appInst->hMenuMain);

    if (appInst->suspendState == CSR_BT_HIDS_EXIT_SUSPEND)
    {
        sk2Text = TEXT_HOGH_APP_SUSPEND_UCS2;
    }
    else
    {
        sk2Text = TEXT_HOGH_APP_EXIT_SUSPEND_UCS2;
    }

    CsrUiMenuSetReqSend(appInst->hMenuMain,
                        CsrUtf82Utf16String(HOGH_APP_PROFILE_NAME),
                        TEXT_SELECT_UCS2,
                        sk2Text);

    CsrCmnListIterate(&appInst->connections,
                      addConnMenuItems,
                      appInst);

    CsrUiMenuAdditemReqSend(appInst->hMenuMain,
                            CSR_UI_FIRST,
                            CSR_BT_HOGH_APP_ADD_DEVICE_KEY,
                            CSR_UI_ICON_CONNECT,
                            TEXT_HOGH_APP_CONNECT_UCS2,
                            NULL,
                            appInst->hInputDialogAddress, /* Opens address input dialog box */
                            appInst->hEventSk2,
                            appInst->hEventBack,
                            CSR_UI_DEFAULTACTION);
}

void CsrBtHoghAppUiPopup(CsrBtHoghAppInstData *appInst,
                         const CsrCharString *dialogHeading,
                         const CsrCharString *dialogText)
{
    CsrUiDialogSetReqSend(appInst->hDialog,
                          CONVERT_TEXT_STRING_2_UCS2(dialogHeading),
                          CONVERT_TEXT_STRING_2_UCS2(dialogText),
                          TEXT_OK_UCS2,
                          NULL,
                          appInst->hEventBack,
                          CSR_UI_DEFAULTACTION,
                          appInst->hEventBack,
                          CSR_UI_DEFAULTACTION);
    CsrUiUieShowReqSend(appInst->hDialog,
                        CSR_BT_HOGH_APP_IFACEQUEUE,
                        CSR_UI_INPUTMODE_AUTO,
                        CSR_BT_HOGH_APP_CSR_UI_PRI);
}

