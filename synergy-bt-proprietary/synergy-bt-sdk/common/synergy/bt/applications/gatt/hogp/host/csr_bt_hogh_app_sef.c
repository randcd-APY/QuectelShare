/****************************************************************************

Copyright (c) 2014-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_bt_hogh_app.h"
#include "csr_bt_hogh_app_strings.h"
#include "csr_bt_hogh_prim.h"
#include "csr_bt_hogh_lib.h"

static CsrBtHoghAppConnection *findConnection(CsrBtHoghAppInstData *appInst,
                                              CsrBtHoghConnId hoghConnId)
{
    CsrBtHoghAppConnection *conn = HOGH_APP_FIND_CONN(appInst, hoghConnId);

    CSR_LOG_TEXT_CONDITIONAL_ERROR(!conn,
                                   (appInst->logHandle,
                                    0,
                                    "Message received (%X) for unknown connection (id=%d)",
                                    *(CsrBtHoghPrim * )appInst->recvMsgP,
                                    hoghConnId));

    return (conn);
}


static void addCfmHandler(CsrBtHoghAppInstData *appInst, CsrBtHoghAddDeviceCfm *cfm)
{
    if (cfm->resultCode == CSR_BT_RESULT_CODE_HOGH_SUCCESS &&
        cfm->resultSupplier == CSR_BT_SUPPLIER_HOGH)
    {
        CsrBtHoghAppConnection *conn = HOGH_APP_ADD_CONN(appInst);

        conn->addr = cfm->addr;
        conn->connId = cfm->hoghConnId;
        conn->status = CSR_BT_HOGH_STATUS_SCANNING;
        conn->appInst = appInst;

        appInst->activeConnection = conn;
    }
    else
    {
        /* Inform user that connection attempt failed */
        CsrBtHoghAppUiPopup(appInst, "Error", "Connection attempt failed");
    }
}

static void statusIndHandler(CsrBtHoghAppInstData *appInst, CsrBtHoghStatusInd *ind)
{
    CsrBtHoghAppConnection *conn = findConnection(appInst, ind->hoghConnId);

    if (conn)
    {
        conn->status = ind->status;

        if (conn->status != CSR_BT_HOGH_STATUS_READY)
        {
            CsrUiUieHideReqSend(conn->hMenuConn);
        }
        CsrBtHoghAppUiRefreshMainMenu(appInst);
    }
}

static void deviceInfoIndHandler(CsrBtHoghAppInstData *appInst, CsrBtHoghDeviceInfoInd *ind)
{
    CsrBtHoghAppConnection *conn = findConnection(appInst, ind->hoghConnId);

    if (conn)
    {
        conn->deviceName = ind->name;
        conn->pnpId = ind->pnpId;
        conn->addr = ind->addr;
        CsrBtHoghAppUiRefreshMainMenu(appInst);
        CsrBtHoghAppUiRefreshConnMenu(conn);
    }
}

static void serviceIndHandler(CsrBtHoghAppInstData *appInst, CsrBtHoghServiceInd *ind)
{
    CsrBtHoghAppConnection *conn = findConnection(appInst, ind->hoghConnId);

    if (conn)
    {
        CsrBtHoghAppService *service = HOGH_APP_ADD_SERVICE(conn);

        service->serviceId = ind->serviceId;
        service->protocol = ind->protocol;
        service->reportMap = ind->reportMap;
        service->reportMapLength = ind->reportMapLength;
        service->hidInformation = ind->hidInformation;
        service->supportedReports = ind->supportedReports;
        service->conn = conn;
    }
}

static void setProtocolCfmHandler(CsrBtHoghAppInstData *appInst, CsrBtHoghSetProtocolCfm *cfm)
{
    CsrBtHoghAppConnection *conn = findConnection(appInst, cfm->hoghConnId);

    if (conn)
    {
        CsrBtHoghAppService *service = HOGH_APP_FIND_SERVICE(conn, cfm->serviceId);

        if (service)
        {
            if (cfm->resultCode == CSR_BT_RESULT_CODE_HOGH_SUCCESS
                && cfm->resultSupplier == CSR_BT_SUPPLIER_HOGH)
            {
                /* Do nothing */
            }
            else
            { /* Revert protocol change */
                if (service->protocol == CSR_BT_HIDS_BOOT_PROTOCOL)
                {
                    service->protocol = CSR_BT_HIDS_REPORT_PROTOCOL;
                }
                else
                {
                    service->protocol = CSR_BT_HIDS_BOOT_PROTOCOL;
                }
            }
            CsrBtHoghAppUiRefreshConnMenu(conn);
        }
        else
        {
            CSR_LOG_TEXT_ERROR((appInst->logHandle,
                                0,
                                "Message received (%X) for unknown service (id=%d) in a connection (id=%d)",
                                *(CsrBtHoghPrim * ) appInst->recvMsgP,
                                cfm->serviceId,
                                cfm->hoghConnId));
        }
    }
}

static void getReportIndHandler(CsrBtHoghAppInstData *appInst, CsrBtHoghGetReportInd *ind)
{
    CsrBtHoghAppConnection *conn = findConnection(appInst, ind->hoghConnId);

    if (conn)
    {
        CsrBtHoghAppService *service = HOGH_APP_FIND_SERVICE(conn, ind->serviceId);

        if (service)
        {
            CsrBtHoghAppReportInfo *report = HOGH_APP_GET_FIRST_REPORT(service);

            while (report)
            {
                if (report->reportId == ind->reportId
                    && report->reportType == ind->reportType)
                { /* Found matching report */
                    break;
                }
                report = (CsrBtHoghAppReportInfo *) report->cmnListInfo.next;
            }

            if (report)
            { /* Clear old report */
                CsrPmemFree(report->report);
            }
            else
            { /* New report found */
                report = HOGH_APP_ADD_REPORT(service);

                report->reportId = ind->reportId;
                report->reportType = ind->reportType;
                report->service = service;
            }

            report->report = ind->report;
            report->reportLen = ind->reportLength;

            CsrBtHoghAppUiRefreshConnMenu(conn);
        }
        else
        {
            CSR_LOG_TEXT_ERROR((appInst->logHandle,
                                0,
                                "Message received (%X) for unknown service (id=%d) in a connection (id=%d)",
                                *(CsrBtHoghPrim * ) appInst->recvMsgP,
                                ind->serviceId,
                                ind->hoghConnId));
        }
    }
}

static void setReportCfmHandler(CsrBtHoghAppInstData *appInst, CsrBtHoghSetReportCfm *cfm)
{
    if (cfm->resultCode == CSR_BT_RESULT_CODE_HOGH_SUCCESS
        && cfm->resultSupplier == CSR_BT_SUPPLIER_HOGH)
    {
        /* Do nothing */
    }
    else
    {
        CsrBtHoghAppUiPopup(appInst, "Error", "Attempt to set report failed");
    }
}

static void setStateCfmHandler(CsrBtHoghAppInstData *appInst, CsrBtHoghSetStateCfm *cfm)
{
    if (cfm->resultCode == CSR_BT_RESULT_CODE_HOGH_SUCCESS
        && cfm->resultSupplier == CSR_BT_SUPPLIER_HOGH)
    {
        /* Do nothing */
    }
    else
    { /* Revert the state change */
        if (appInst->suspendState == CSR_BT_HIDS_SUSPEND)
        {
            appInst->suspendState = CSR_BT_HIDS_EXIT_SUSPEND;
        }
        else
        {
            appInst->suspendState = CSR_BT_HIDS_SUSPEND;
        }
    }
    CsrBtHoghAppUiRefreshMainMenu(appInst);
}

static void removeDeviceCfmHandler(CsrBtHoghAppInstData *appInst, CsrBtHoghRemoveDeviceCfm *cfm)
{
    CsrBtHoghAppConnection *conn = findConnection(appInst, cfm->hoghConnId);

    if (conn)
    {
        CsrCmnListElementRemove(&appInst->connections, (CsrCmnListElm_t *) conn);

        if (cfm->resultCode == CSR_BT_RESULT_CODE_HOGH_SUCCESS
            && cfm->resultSupplier == CSR_BT_SUPPLIER_HOGH)
        {
            CsrBtHoghAppUiPopup(appInst, "Success", "Connection removed successfully");
        }
        else
        {
            CsrBtHoghAppUiPopup(appInst, "Error", "Connection removed due to some error");
        }
        CsrBtHoghAppUiRefreshMainMenu(appInst);
    }
}


/******************************************************************************
 * Function to handle all hogh-primitives.
 ******************************************************************************/
void CsrBtHoghAppPrimHandler(CsrBtHoghAppInstData *appInst)
{
    CsrBtHoghAppConnection *connection = appInst->activeConnection;

    switch (*(CsrBtHoghPrim *) appInst->recvMsgP)
    {
        case CSR_BT_HOGH_REGISTER_CFM:
            /* Do nothing */
            break;
        case CSR_BT_HOGH_ADD_DEVICE_CFM:
            addCfmHandler(appInst, appInst->recvMsgP);
            break;
        case CSR_BT_HOGH_STATUS_IND:
            statusIndHandler(appInst, appInst->recvMsgP);
            break;
        case CSR_BT_HOGH_CONN_PARAM_CFM:
            /* Do nothing */
            break;
        case CSR_BT_HOGH_TIMEOUT_CFM:
            /* Do nothing */
            break;
        case CSR_BT_HOGH_SECURITY_CFM:
            /* Do nothing */
            break;
        case CSR_BT_HOGH_DEVICE_INFO_IND:
            deviceInfoIndHandler(appInst, appInst->recvMsgP);
            break;
        case CSR_BT_HOGH_SERVICE_IND:
            serviceIndHandler(appInst, appInst->recvMsgP);
            break;
        case CSR_BT_HOGH_SET_PROTOCOL_CFM:
            setProtocolCfmHandler(appInst, appInst->recvMsgP);
            break;
        case CSR_BT_HOGH_GET_REPORT_IND:
            getReportIndHandler(appInst, appInst->recvMsgP);
            break;
        case CSR_BT_HOGH_SET_REPORT_CFM:
            setReportCfmHandler(appInst, appInst->recvMsgP);
            break;
        case CSR_BT_HOGH_SET_STATE_CFM:
            setStateCfmHandler(appInst, appInst->recvMsgP);
            break;
        case CSR_BT_HOGH_REMOVE_DEVICE_CFM:
            removeDeviceCfmHandler(appInst, appInst->recvMsgP);
            break;
        default:
        {
            /* unexpected primitive received */
            CSR_LOG_TEXT_UNHANDLED_PRIMITIVE(appInst->logHandle,
                                             0,
                                             CSR_BT_GAP_APP_PRIM,
                                             *(CsrBtHoghPrim *) appInst->recvMsgP);

            CsrBtHoghFreeUpstreamMessageContents(CSR_BT_HOGH_PRIM,
                                                 appInst->recvMsgP);
            break;
        }
    }
}
