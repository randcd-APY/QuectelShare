/****************************************************************************

Copyright (c) 2011-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_pmem.h"
#include "csr_list.h"
#include "csr_bt_gap_app_lib.h"
#include "csr_bt_util.h"
#include "csr_bt_hogh_app.h"
#include "csr_bt_hogh_prim.h"
#include "csr_bt_hogh_lib.h"
#include "csr_bt_hogh_app_strings.h"

#include <stdio.h>


static void deinitReport(CsrCmnListElm_t *elem)
{
    CsrBtHoghAppReportInfo *report = (CsrBtHoghAppReportInfo *) elem;

    CsrPmemFree(report->report);
}

static void initService(CsrCmnListElm_t *elem)
{
    CsrBtHoghAppService *service = (CsrBtHoghAppService *) elem;
    CsrCmnListInit(&service->reports,
                   0,
                   NULL,
                   deinitReport);
}

static void deinitService(CsrCmnListElm_t *elem)
{
    CsrBtHoghAppService *service = (CsrBtHoghAppService *) elem;

    CsrCmnListDeinit(&service->reports);
    CsrPmemFree(service->reportMap);
}

static void initConnection(CsrCmnListElm_t *elem)
{
    CsrBtHoghAppConnection *conn = (CsrBtHoghAppConnection *) elem;

    conn->deviceName = NULL;
    conn->hMenuConn = CSR_UI_DEFAULTACTION;
    CsrCmnListInit(&conn->services,
                   0,
                   initService,
                   deinitService);
    CsrUiUieCreateReqSend(CSR_BT_HOGH_APP_IFACEQUEUE, CSR_UI_UIETYPE_MENU);
}

static void deinitConnection(CsrCmnListElm_t *elem)
{
    CsrBtHoghAppConnection *conn = (CsrBtHoghAppConnection *) elem;

    CsrCmnListDeinit(&conn->services);
    CsrPmemFree(conn->deviceName);

    if (conn->hMenuConn != CSR_UI_DEFAULTACTION)
    {
        CsrUiUieDestroyReqSend(conn->hMenuConn);
    }
}


CsrUtf16String *CsrBtHoghAppAddrToString(BD_ADDR_T *addr)
{
    CsrUint8 i;
    CsrCharString peerDeviceAddress[ADDRESS_STRING_LEN];
    /* [QTI] Fix KW issue#267187. */
    snprintf(peerDeviceAddress, sizeof(peerDeviceAddress), "%4X:%2X:%6X", addr->nap, addr->uap, addr->lap);

    for (i = 0; i < ADDRESS_STRING_LEN; i++)
    {
        if (peerDeviceAddress[i] == ' ')
        {
            peerDeviceAddress[i] = '0';
        }
    }

    return (CsrUtf82Utf16String((CsrUtf8String *) peerDeviceAddress));
}


/******************************************************************************
 * Init function called by the scheduler upon initialisation.
 * This function is used to boot the demo application.
 *****************************************************************************/
void CsrBtHoghAppInit(void **gash)
{
    CsrBtHoghAppInstData *appInst;

    /* allocate and initialise instance data space */
    *gash = (void *) CsrPmemZalloc(sizeof(CsrBtHoghAppInstData));
    appInst = (CsrBtHoghAppInstData *) *gash;

    appInst->suspendState = CSR_BT_HIDS_EXIT_SUSPEND;
    CsrCmnListInit(&appInst->connections,
                   0,
                   initConnection,
                   deinitConnection);

    /* Start UI creation cycle */
    CsrUiUieCreateReqSend(CSR_BT_HOGH_APP_IFACEQUEUE, CSR_UI_UIETYPE_EVENT);
    CsrAppRegisterReqSend(CSR_BT_HOGH_APP_IFACEQUEUE,
                          TECH_BTLE,
                          CsrUtf8StrDup(HOGH_APP_PROFILE_NAME));
    CsrBtHoghRegisterReqSend(CSR_BT_HOGH_APP_IFACEQUEUE);
    CSR_LOG_TEXT_REGISTER(&(appInst->logHandle), "HOGH_APP", 0, NULL);
}

void CsrBtHoghAppDeinit(void **gash)
{
#ifdef ENABLE_SHUTDOWN
    CsrUint16 msg_type;
    void *msg_data;
    CsrBtHoghAppInstData *appInst;

    appInst = (CsrBtHoghAppInstData *) (*gash);

    /* get a message from the demo application message queue. The message is returned in prim
     and the event type in eventType */
    while (CsrSchedMessageGet(&msg_type, &msg_data))
    {
        switch (msg_type)
        {
            case CSR_BT_HOGH_PRIM:
                CsrBtHoghFreeUpstreamMessageContents(msg_type, msg_data);
                break;
        }
        CsrPmemFree(msg_data);
    }

    CsrCmnListDeinit(&appInst->connections);
    CsrPmemFree(appInst);
#else
    CSR_UNUSED(gash);
#endif
}

/******************************************************************************
 * This is the demo application handler function. All primitives sent
 * to the demo application will be received in here. The scheduler
 * ensures that the function is activated when a signal is put on the
 * demo application signal queue.
 *****************************************************************************/
void CsrBtHoghAppHandler(void **gash)
{
    CsrBtHoghAppInstData *appInst;
    CsrUint16 eventType;

    /* get a message from the demo application message queue. The message is returned in prim
     and the event type in eventType */
    appInst = (CsrBtHoghAppInstData *) (*gash);

    CsrSchedMessageGet(&eventType, &appInst->recvMsgP);

    switch (eventType)
    {
        case CSR_BT_HOGH_PRIM:
            CsrBtHoghAppPrimHandler(appInst);
            break;
        case CSR_BT_GAP_APP_PRIM:
            if (*(CsrBtGapAppPrim *) appInst->recvMsgP == CSR_BT_GAP_APP_GET_SELECTED_DEVICE_CFM)
            {
                CsrBtGapAppGetSelectedDeviceCfm *prim = appInst->recvMsgP;
                appInst->addr.addr.lap = prim->deviceAddr.lap;
                appInst->addr.addr.nap = prim->deviceAddr.nap;
                appInst->addr.addr.uap = prim->deviceAddr.uap;
          
                appInst->addr.type     = ((prim->deviceStatus & CSR_BT_SD_STATUS_PRIVATE_ADDR)
                                                          ? TBDADDR_RANDOM
                                                          : TBDADDR_PUBLIC);

                CsrBtHoghAppUiRefreshMainMenu(appInst);

                CsrUiInputdialogSetReqSend(appInst->hInputDialogAddress,
                                           TEXT_HOGH_APP_ADD_DEVICE_UCS2,
                                           TEXT_HOGH_APP_CONNECT_TARGET_UCS2,
                                           CSR_UI_ICON_CONNECT,
                                           CsrBtHoghAppAddrToString(&prim->deviceAddr),
                                           14,
                                           CSR_UI_KEYMAP_ALPHANUMERIC,
                                           TEXT_HOGH_APP_CONNECT_UCS2,
                                           TEXT_HOGH_APP_AUTO_CONNECT_UCS2,
                                           appInst->hEventSk1,
                                           appInst->hEventSk2,
                                           appInst->hEventBack,
                                           0);
                CsrUiUieShowReqSend(appInst->hMenuMain,
                                    CSR_BT_HOGH_APP_IFACEQUEUE,
                                    CSR_UI_INPUTMODE_AUTO,
                                    CSR_BT_HOGH_APP_CSR_UI_PRI);
            }
            else
            {
                /* unexpected primitive received */
                CSR_LOG_TEXT_UNHANDLED_PRIMITIVE(appInst->logHandle,
                                                 0,
                                                 eventType,
                                                 *(CsrPrim *) appInst->recvMsgP);
            }
            break;
        case CSR_APP_PRIM:
            if (*(CsrPrim *) appInst->recvMsgP == CSR_APP_TAKE_CONTROL_IND)
            { /* We need to query the GAP task for the currently selected device addr */
                CsrBtGapAppGetSelectedDeviceReqSend(CSR_BT_HOGH_APP_IFACEQUEUE);
            }
            else if (*(CsrPrim *) appInst->recvMsgP == CSR_APP_REGISTER_CFM)
            {
                /* Just ignore this primitive since we only register one menu
                 * item we don't need to remember the controlId */
            }
            else
            {
                /* unexpected primitive received */
                CSR_LOG_TEXT_UNHANDLED_PRIMITIVE(appInst->logHandle,
                                                 0,
                                                 eventType,
                                                 *(CsrPrim *) appInst->recvMsgP);
            }
            break;
        case CSR_UI_PRIM:
            CsrBtHoghAppUiPrimHandler(appInst);
            break;

        default:
            /* unexpected primitive received */
            CSR_LOG_TEXT_UNHANDLED_PRIMITIVE(appInst->logHandle,
                                             0,
                                             eventType,
                                             *(CsrPrim *) appInst->recvMsgP);
            break;
    }
    /* free the received signal again. If the signal for some reason must be stored in the application
     * the pointer (prim) must be set to NULL in order not to free it here
     */
    CsrPmemFree(appInst->recvMsgP);
}

