/****************************************************************************

Copyright (c) 2014-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_pmem.h"
#include "csr_list.h"
#include "csr_exceptionhandler.h"

#include "csr_app_lib.h"
#include "csr_bt_platform.h"
#include "csr_bt_gap_app_lib.h"
#include "csr_bt_util.h"

#include "csr_bt_cm_prim.h"
#include "csr_bt_cm_lib.h"

#include "csr_bt_gatt_lib.h"
#include "csr_bt_gatt_demo_app.h"
#include "csr_bt_gatt_demo_defines.h"

#include "csr_bt_gatt_app_task.h"
#include "csr_bt_gatt_demo_db_utils.h"
#include "csr_bt_hogd_app.h"
#include "csr_bt_hogd_app_db.h"
#include "csr_bt_hogd_app_strings.h"

void CsrBtHogdAppResetConnection(CsrCmnListElm_t *elem)
{
    CsrBtHogdAppConnection *connection = (CsrBtHogdAppConnection *) elem;
    CsrMemSet(connection, 0, sizeof(CsrBtHogdAppConnection));
}

void CsrBtHogdAppActivate(CsrBtHogdAppInstData *inst)
{
    CsrBtGattRegisterReqSend(CSR_BT_HOGD_APP_IFACEQUEUE,
                             CSR_BT_CM_CONTEXT_UNUSED);
}

void CsrBtHogdAppDeactivate(CsrBtHogdAppInstData *inst)
{
    CsrBtGattUnregisterReqSend(inst->gattId);
    inst->gattId = CSR_BT_GATT_INVALID_GATT_ID;
}

void CsrBtHogdAppDisconnect(CsrBtHogdAppInstData *inst)
{
    CsrBtGattDisconnectReqSend(inst->gattId, inst->btConnId);
}

void CsrBtHogdAppStartPeripheralRole(CsrBtHogdAppInstData *inst)
{
    CsrBtGattConnFlags flags = CSR_BT_GATT_FLAGS_UNDIRECTED;
    CsrUint16 advIntervMin, advIntervMax;
    CsrUint8 *advData = CsrPmemZalloc((CsrUint8) CSR_BT_HOGD_APP_ADVERT_DATA_LEN);
    CsrBtTypedAddr addr;

    advData[0] = CSR_BT_HOGD_APP_ADVERT_DATA_LEN - 1; /* length */
    advData[1] = 0x02; /* AD type: Complete list of 16-bit UUIDs available */
    advData[2] = CSR_BT_GATT_UUID_HUMAN_INTERFACE_DEVICE_SERVICE & 0xFF; /* UUID: Thermometer */
    advData[3] = CSR_BT_GATT_UUID_HUMAN_INTERFACE_DEVICE_SERVICE >> 8;

    CsrMemSet(&addr, 0, sizeof(addr));

    if (inst->whiteListConnect == TRUE)
    {
        flags |= CSR_BT_GATT_FLAGS_WHITELIST;
    }

    if (inst->profileData.scanParameters.scanInterval != 0
        && inst->profileData.scanParameters.scanWindow != 0)
    {
        advIntervMax = inst->profileData.scanParameters.scanWindow;
        advIntervMin = inst->profileData.scanParameters.scanWindow / 2;
    }
    else
    {
        advIntervMin = CSR_BT_HOGD_APP_ADVERTISING_INTERVAL_MIN;
        advIntervMax = CSR_BT_HOGD_APP_ADVERTISING_INTERVAL_MAX;
    }
    CsrBtGattParamConnectionReqSend(inst->gattId,
                                    CSR_BT_LE_DEFAULT_SCAN_INTERVAL,
                                    CSR_BT_LE_DEFAULT_SCAN_WINDOW,
                                    CSR_BT_LE_DEFAULT_CONN_INTERVAL_MIN,
                                    CSR_BT_LE_DEFAULT_CONN_INTERVAL_MAX,
                                    CSR_BT_LE_DEFAULT_CONN_LATENCY,
                                    CSR_BT_LE_DEFAULT_CONN_SUPERVISION_TIMEOUT,
                                    CSR_BT_LE_DEFAULT_CONN_ATTEMPT_TIMEOUT,
                                    advIntervMin,
                                    advIntervMax,
                                    CSR_BT_LE_DEFAULT_CONN_LATENCY_MAX,
                                    CSR_BT_LE_DEFAULT_CONN_SUPERVISION_TIMEOUT_MIN,
                                    CSR_BT_LE_DEFAULT_CONN_SUPERVISION_TIMEOUT_MAX);

    CsrBtGattPeripheralReqDataSend(inst->gattId,
                                   addr,
                                   flags,
                                   CSR_BT_HOGD_APP_PREFFERED_MTU,
                                   (CsrUint8 ) CSR_BT_HOGD_APP_ADVERT_DATA_LEN,
                                   advData,
                                   0,
                                   NULL);
}

CsrBool CsrBtHogdAppSearchConnByAddr(CsrCmnListElm_t *elem, void *value)
{
    CsrBtHogdAppConnection *connection = (CsrBtHogdAppConnection *) elem;
    CsrBtTypedAddr *addr = (CsrBtTypedAddr *) value;
    return (CsrBtBdAddrEq(&connection->connectedDeviceAddr.addr, &addr->addr));
}

CsrBtHogdAppMouseBootReport *CsrBtHogdAppGetMouseBootInputReport(CsrBtHogdAppMouseData *mouseData)
{
    CsrBtHogdAppMouseBootReport *bootInputReport =
                    (CsrBtHogdAppMouseBootReport *) CsrPmemZalloc(sizeof(*bootInputReport));
    bootInputReport->buttons = mouseData->buttons
                               & (MOUSE_LEFT_BUTTON
                                  | MOUSE_MIDDLE_BUTTON
                                  | MOUSE_RIGHT_BUTTON);
    bootInputReport->positionX = mouseData->positionX;
    bootInputReport->positionY = mouseData->positionY;
    return (bootInputReport);
}

void CsrBtHogdAppRequestScanParams(CsrBtHogdAppInstData *inst)
{
    CsrUint8 *scanRefresh = CsrPmemZalloc(sizeof(*scanRefresh));

    *scanRefresh = CSR_BT_SCPS_SCAN_PARAM_REFRESH;
    CsrBtGattNotificationEventReqSend(inst->gattId,
                                      inst->btConnId,
                                      inst->db.hStart + SCP_SCAN_REFRESH_HANDLE_OFFSET - 1,
                                      sizeof(*scanRefresh),
                                      scanRefresh);
}

void CsrBtHogdAppUpdateMouseData(CsrBtHogdAppInstData *inst, CsrBool boot)
{
    if (inst->profileData.protocol == CSR_BT_HIDS_BOOT_PROTOCOL)
    {
        if (inst->activeConnection->mouseBootInputReportNotify != FALSE
            && boot != FALSE)
        {
            CsrUint16 size = sizeof(CsrBtHogdAppMouseBootReport);
            CsrUint8 *value =
                            (CsrUint8 *) CsrBtHogdAppGetMouseBootInputReport(&inst->profileData.mouseData);
            CsrBtGattNotificationEventReqSend(inst->gattId,
                                              inst->btConnId,
                                              inst->db.hStart + HIDS_BOOT_MOUSE_INPUT_HANDLE_OFFSET - 1,
                                              size,
                                              value);
        }
    }
    else if (inst->profileData.protocol == CSR_BT_HIDS_REPORT_PROTOCOL)
    {
        if (inst->activeConnection->mouseInputReportNotify != FALSE)
        {
            CsrUint16 size = 5;
            CsrUint8 *value = CsrMemDup(&inst->profileData.mouseData, size);
            CsrBtGattNotificationEventReqSend(inst->gattId,
                                              inst->btConnId,
                                              inst->db.hStart + HIDS_INPUT_REPORT_HANDLE_OFFSET - 1,
                                              size,
                                              value);
        }
    }
    inst->profileData.mouseData.positionX = 0;
    inst->profileData.mouseData.positionY = 0;
    inst->profileData.mouseData.wheelVertical = 0;
    inst->profileData.mouseData.wheelHorizontal = 0;
}

void CsrBtHogdAppUpdateBatteryData(CsrBtHogdAppInstData *inst)
{
    if (inst->activeConnection->batteryLevelNotify != FALSE)
    {
        CsrUint16 size = 1;
        CsrUint8 *value = CsrMemDup(&inst->profileData.batteryPercentage, size);
        CsrBtGattNotificationEventReqSend(inst->gattId,
                                          inst->btConnId,
                                          inst->db.hStart + BAS_BATTERY_LEVEL_HANDLE_OFFSET - 1,
                                          size,
                                          value);
    }
}

/******************************************************************************
 * Init function called by the scheduler upon initialisation.
 * This function is used to boot the demo application.
 *****************************************************************************/
void CsrBtHogdAppInit(void **gash)
{
    CsrBtHogdAppInstData *inst;

    /* allocate and initialise instance data space */
    *gash = (void *) CsrPmemZalloc(sizeof(CsrBtHogdAppInstData));
    inst = (CsrBtHogdAppInstData *) *gash;
    inst->connections = (CsrCmnList_t *) CsrPmemZalloc(sizeof(CsrCmnList_t));

    inst->profileData.batteryPercentage = 100;
    inst->profileData.protocol = CSR_BT_HIDS_REPORT_PROTOCOL;

    CsrCmnListInit(inst->connections, 0, CsrBtHogdAppResetConnection, NULL);

    /* Start UI creation cycle */
    CsrUiUieCreateReqSend(CSR_BT_HOGD_APP_IFACEQUEUE, CSR_UI_UIETYPE_EVENT);
    CsrAppRegisterReqSend(CSR_BT_HOGD_APP_IFACEQUEUE,
                          TECH_BTLE,
                          PROFILE_NAME(HOGD_APP_PROFILE_NAME));
}

void CsrBtHogdAppDeinit(void **gash)
{
#ifdef ENABLE_SHUTDOWN
    CsrUint16 msg_type;
    void *msg_data;
    CsrBtHogdAppInstData *inst;

    inst = (CsrBtHogdAppInstData *) (*gash);

    /* get a message from the demo application message queue. The message is returned in prim
     and the event type in eventType */
    while (CsrSchedMessageGet(&msg_type, &msg_data))
    {
        switch (msg_type)
        {
            case CSR_BT_GATT_PRIM:
            CsrBtGattFreeUpstreamMessageContents(msg_type, msg_data);
            break;
        }
        CsrPmemFree(msg_data);
    }
    CsrCmnListDeinit(inst->connections);
    CsrPmemFree(inst->connections);
    CsrPmemFree(inst);
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
void CsrBtHogdAppHandler(void **gash)
{
    CsrBtHogdAppInstData *inst;
    CsrUint16 eventType;

    /* get a message from the demoapplication message queue. The message is returned in prim
     and the event type in eventType */
    inst = (CsrBtHogdAppInstData *) (*gash);

    CsrSchedMessageGet(&eventType, &inst->recvMsgP);

    switch (eventType)
    {
        case CSR_BT_GATT_PRIM:
        {
            CsrBtHogdAppGattPrimHandler(inst);
            break;
        }

        case CSR_APP_PRIM:
        {
            CsrPrim *prim = (CsrPrim *) inst->recvMsgP;

            if (*prim == CSR_APP_TAKE_CONTROL_IND)
            { /* We need to query the GAP task for the currently selected device addr */
                CsrUiUieShowReqSend(inst->csrUiVar.hMenuMain,
                                    CSR_BT_HOGD_APP_IFACEQUEUE,
                                    CSR_UI_INPUTMODE_AUTO,
                                    CSR_BT_HOGD_APP_CSR_UI_PRI);
            }
            else if (*prim == CSR_APP_REGISTER_CFM)
            { /* Just ignore this primitive since we only register one menu item we don't need to remember the controlId */
                ;
            }
            else
            {
                /* unexpected primitive received */
                CsrGeneralException("CSR_BT_HOGD_APP",
                                    0,
                                    *prim,
                                    "####### Unhandled CSR_APP_PRIM in CsrBtHogdAppHandler,");
            }
            break;
        }
        case CSR_UI_PRIM:
        {
            CsrBtHogdAppHandleUiPrim(inst);
            break;
        }
        default:
        {
            /* unexpected primitive received */
            CsrGeneralException("CSR_BT_HOGD_APP",
                                0,
                                *((CsrPrim * ) inst->recvMsgP),
                                "####### default in CsrBtHogdAppHandler,");
            break;
        }
    }
    /* free the received signal again. If the signal for some reason must be stored in the application
     * the pointer (prim) must be set to NULL in order not to free it here
     */
    CsrPmemFree(inst->recvMsgP);
}
