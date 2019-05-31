/****************************************************************************

Copyright (c) 2011-2013 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"
#ifndef EXCLUDE_CSR_BT_PROX_SRV_MODULE

#include <string.h>
#include "csr_util.h"
#include "csr_pmem.h"
#include "csr_exceptionhandler.h"
#include "csr_app_lib.h"
#include "csr_bt_platform.h"
#include "csr_bt_gap_app_lib.h"
#include "csr_bt_util.h"
#include "csr_bt_cm_prim.h"
#include "csr_bt_cm_lib.h"
#include "csr_bt_gatt_lib.h"
#include "csr_bt_gatt_demo_app.h"
#include "csr_bt_gatt_demo_prox_server_app_private_prim.h"
#include "csr_bt_gatt_demo_prox_server_app_ui_sef.h"
#include "csr_bt_prox_srv_prim.h"
#include "csr_bt_prox_srv_lib.h"
#include "csr_bt_gatt_app_task.h"

/* Forward declarations for handler functions */
void CsrBtProxsAppHandleProxSrvPrim(CsrBtProxsAppInstData *proxsData);
void CsrBtProxsAppHandleCmPrim(CsrBtProxsAppInstData *proxsData);
void CsrBtProxsAppHandleSdPrim(CsrBtProxsAppInstData *proxsData);

void CsrBtProxsAppInitInstanceData(CsrBtProxsAppInstData *proxsData)
{
    CsrMemSet(proxsData,0,sizeof(CsrBtProxsAppInstData));
    proxsData->csrUiVar.hMainMenu                = CSR_UI_DEFAULTACTION;
    proxsData->csrUiVar.hCommonDialog            = CSR_UI_DEFAULTACTION;
    proxsData->leBtConnId                        = CSR_BT_CONN_ID_INVALID;
    proxsData->connectedBtConnId                 = CSR_BT_CONN_ID_INVALID;
}
/******************************************************************************
CsrBtProxsAppStartActivate:
 *****************************************************************************/
void CsrBtProxsAppStartActivate(CsrBtProxsAppInstData *proxsData)
{
    CsrBtProxSrvActivateReqSend(proxsData->phandle, NULL, 0, 0xFFFF);
    CsrBtGattAppHandleProxsCsrUiActivateDeactivate(proxsData, 
                                                   TRUE, 
                                                   FALSE);
}
/******************************************************************************
CsrBtProxsAppStartDeactivating:
 *****************************************************************************/
void CsrBtProxsAppStartDeactivate(CsrBtProxsAppInstData *proxsData)
{

    /* Deactivate current connection */
    CsrBtProxSrvDeactivateReqSend();
}


static void csrBtProxsAppSendProxsAppRegisterReq(CsrSchedQid pHandle)
{
    CsrBtProxsAppRegisterReq *prim;

    prim = (CsrBtProxsAppRegisterReq *) CsrPmemAlloc(sizeof(CsrBtProxsAppRegisterReq));
    prim->type = CSR_BT_PROXS_APP_REGISTER_REQ;
    prim->phandle = pHandle;

    CsrSchedMessagePut(CSR_BT_PROXS_APP_IFACEQUEUE, CSR_BT_PROXS_APP_PRIM,prim);
}

static void csrBtProxsAppSendProxsAppRegisterCfm(CsrSchedQid phandle, CsrSchedQid queueId, CsrUint8 instance)
{
    CsrBtProxsAppRegisterCfm *prim;

    prim = (CsrBtProxsAppRegisterCfm *) CsrPmemAlloc(sizeof(CsrBtProxsAppRegisterCfm));
    prim->type = CSR_BT_PROXS_APP_REGISTER_CFM;
    prim->queueId = queueId;
    prim->instance = instance;

    CsrSchedMessagePut(phandle, CSR_BT_PROXS_APP_PRIM,prim);
}

static char * csrBtProxsAppReturnProxsInstName(CsrBtProxsAppInstData *instance)
{
    char *buf;
    CsrUint32 bufSize = CsrStrLen("Proximity Srv")+1;
    buf = CsrPmemAlloc(bufSize);

    snprintf(buf, bufSize, "Proximity Srv");
    return buf;
}

/******************************************************************************
 * Init function called by the scheduler upon initialisation.
 * This function is used to boot the demo application.
 *****************************************************************************/
void CsrBtProxsAppInit(void **gash)
{
    CsrBtProxsAppInstData    *proxsData;

    /* allocate and initialise instance data space */
    *gash = (void *) CsrPmemAlloc(sizeof(CsrBtProxsAppInstData));
    proxsData = (CsrBtProxsAppInstData *) *gash;
    CsrBtProxsAppInitInstanceData(proxsData);

    proxsData->phandle = CsrSchedTaskQueueGet() ;

    proxsData->initialized = TRUE;
    proxsData->bredrBtConnId = CSR_BT_PROXS_BTCONN_UNSET;

    /* Start UI creation cycle */
    CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);

    csrBtProxsAppSendProxsAppRegisterReq(proxsData->phandle);
}

#ifdef ENABLE_SHUTDOWN
void CsrBtProxsAppDeinit(void **gash)
{
    CsrUint16 msg_type;
    void *msg_data;
    CsrBtProxsAppInstData *proxsData;

    proxsData = (CsrBtProxsAppInstData *) (*gash);
    /* get a message from the demoapplication message queue. The message is returned in prim
       and the event type in eventType */
    while ( CsrSchedMessageGet(&msg_type, &msg_data) )
    {
        switch (msg_type)
        {
            case CSR_BT_PROX_SRV_PRIM:
                CsrBtProxSrvFreeUpstreamMessageContents(msg_type, msg_data);
                break;
        }
        CsrPmemFree(msg_data);
    }

    CsrPmemFree(proxsData);
}
#else
void CsrBtProxsAppDeinit(void **gash)
{
    CSR_UNUSED(gash);
}
#endif

/******************************************************************************
 * This is the demo application handler function. All primitives sent
 * to the demo application will be received in here. The scheduler
 * ensures that the function is activated when a signal is put on the
 * demo application signal queue.
 *****************************************************************************/
void CsrBtProxsAppHandler(void **gash)
{
    CsrBtProxsAppInstData *proxsData;
    CsrUint16    eventType;

    /* get a message from the demoapplication message queue. The message is returned in prim
       and the event type in eventType */
    proxsData = (CsrBtProxsAppInstData *) (*gash);

    if(!proxsData->restoreFlag)
    { /* No messages on the local save queue                                                */
        CsrSchedMessageGet(&eventType , &proxsData->recvMsgP);
    }
    else
    {
        if(!CsrMessageQueuePop(&proxsData->saveQueue, &eventType , &proxsData->recvMsgP))
        { /* No more messages on the local save queue. Call CsrGetMessage to receive
             a message from the scheduler                                                   */
            proxsData->restoreFlag = FALSE;
            CsrSchedMessageGet(&eventType , &proxsData->recvMsgP);
        }
        else
        { /* A message has been restored from the local queed                               */
            ;
        }
    }

    switch (eventType)
    {
        case CSR_BT_PROX_SRV_PRIM:
            {
                CsrBtProxsAppHandleProxSrvPrim(proxsData);
                break;
            }

        case CSR_BT_PROXS_APP_PRIM:
            {
                CsrPrim *type = (CsrPrim *) proxsData->recvMsgP;

                switch(*type)
                {
                    case CSR_BT_PROXS_APP_REGISTER_REQ:
                        { /* only received by controling proxs demo app instance */
                            if(proxsData->initialized)
                            {
                                CsrBtProxsAppRegisterReq *prim = proxsData->recvMsgP;
                                CsrSchedQid queueId = CSR_SCHED_QID_INVALID;

                                proxsData->proxsAppHandle = prim->phandle;
                                queueId = proxsData->proxsProfileHandle;
                                if(queueId != CSR_SCHED_QID_INVALID)
                                {
                                    csrBtProxsAppSendProxsAppRegisterCfm(prim->phandle, queueId,1);
                                }
                                else
                                {
                                    CsrGeneralException("CSR_PROXS_DEMO_APP", 
                                                        0, 
                                                        *type,
                                                        "No CsrBtProxs profile Queue Handle Available");
                                }
                            }
                            else
                            {
                                CsrMessageQueuePush(&proxsData->saveQueue, CSR_BT_PROXS_APP_PRIM, proxsData->recvMsgP);
                                proxsData->recvMsgP = NULL;
                            }
                            break;
                        }
                    case CSR_BT_PROXS_APP_REGISTER_CFM:
                        {
                            proxsData->profileName = csrBtProxsAppReturnProxsInstName(proxsData);

                            CsrBtGattAppHandleProxsSetMainMenuHeader(proxsData);

                            proxsData->initialized = TRUE;

                            CsrAppRegisterReqSend(proxsData->phandle, TECH_BTLE, PROFILE_NAME(proxsData->profileName));
                            break;
                        }
                    default:
                        { /* Do nothing */
                            break;
                        }
                }
                break;
            }
        case CSR_BT_GAP_APP_PRIM:
            {
                CsrPrim *prim = (CsrPrim *) proxsData->recvMsgP;
                if (*prim == CSR_BT_GAP_APP_GET_SELECTED_DEVICE_CFM)
                {
                    CsrBtGapAppGetSelectedDeviceCfm *prim = proxsData->recvMsgP;
                    proxsData->selectedDeviceAddr.addr.lap = prim->deviceAddr.lap;
                    proxsData->selectedDeviceAddr.addr.nap = prim->deviceAddr.nap;
                    proxsData->selectedDeviceAddr.addr.uap = prim->deviceAddr.uap;
                    proxsData->selectedDeviceAddr.type     = (prim->deviceStatus & CSR_BT_SD_STATUS_PRIVATE_ADDR)? TBDADDR_RANDOM:TBDADDR_PUBLIC;
                    CsrAppBacklogReqSend(TECH_BTLE, 
                                         PROFILE_NAME(proxsData->profileName), 
                                         FALSE, 
                                         "Received selected DeviceAddr: %04X:%02X:%06X (%s)", 
                                         prim->deviceAddr.nap, 
                                         prim->deviceAddr.uap, 
                                         prim->deviceAddr.lap,
                                         (CsrBtAddrIsRandom(proxsData->selectedDeviceAddr))? "Random":"Public");
                    /*update UI*/
                    CsrUiUieShowReqSend(proxsData->csrUiVar.hMainMenu,
                                        CsrSchedTaskQueueGet(),
                                        CSR_UI_INPUTMODE_AUTO,
                                        CSR_BT_PROXS_APP_CSR_UI_PRI);
                }
                else
                {
                    /* unexpected primitive received */
                    CsrGeneralException("CSR_BT_PROXS_APP", 
                                        0, 
                                        *prim, 
                                        "####### Unhandled CSR_BT_GAP_APP_PRIM in CsrBtProxsAppHandler,");
                }
                break;
            }
        case CSR_APP_PRIM:
            {
                CsrPrim *prim = (CsrPrim *) proxsData->recvMsgP;

                if (*prim == CSR_APP_TAKE_CONTROL_IND)
                { /* We need to query the GAP task for the currently selected device addr */
                    CsrBtGapAppGetSelectedDeviceReqSend(CsrSchedTaskQueueGet());
                }
                else if (*prim == CSR_APP_REGISTER_CFM)
                { /* Just ignore this primitive since we only register one menu item we don't need to remember the controlId */
                    ;
                }
                else
                {
                    /* unexpected primitive received */
                    CsrGeneralException("CSR_BT_PROXS_APP", 
                                        0, 
                                        *prim, 
                                        "####### Unhandled CSR_APP_PRIM in CsrBtProxsAppHandler,");
                }
                break;
            }
        case CSR_UI_PRIM:
            {
                CsrBtGattAppHandleProxsCsrUiPrim(proxsData);
                break;
            }

        default:
            {
                /* unexpected primitive received */
                CsrGeneralException("CSR_BT_PROXS_APP",
                                    0, 
                                    *((CsrPrim *) proxsData->recvMsgP),
                                    "####### default in CsrBtProxsAppHandler,");
            }
    }
    /* free the received signal again. If the signal for some reason must be stored in the application
     * the pointer (prim) must be set to NULL in order not to free it here
     */
    CsrPmemFree(proxsData->recvMsgP);
}

/******************************************************************************
 * Function to handle all proxs-primitives.
 ******************************************************************************/
void CsrBtProxsAppHandleProxSrvPrim(CsrBtProxsAppInstData *proxsData)
{
    CsrPrim *primType;

    primType = (CsrPrim *) proxsData->recvMsgP;
    switch (*primType)
    {
        case CSR_BT_PROX_SRV_ACTIVATE_CFM:
            {
                break;
            }
        case CSR_BT_PROX_SRV_DEACTIVATE_CFM:
            {
                /*update UI*/
                CsrBtGattAppHandleProxsCsrUiActivateDeactivate(proxsData, FALSE, TRUE);

                break;
            }
        case CSR_BT_PROX_SRV_CONNECT_IND:
            {
                CsrBtProxSrvConnectInd        *ind;

                ind = (CsrBtProxSrvConnectInd *) proxsData->recvMsgP;

                if (ind->resultSupplier == CSR_BT_SUPPLIER_PROX_SRV &&
                    ind->resultCode == CSR_BT_PROX_SRV_RESULT_SUCCESS)
                {
                    CsrAppBacklogReqSend(TECH_BTLE, 
                                         PROFILE_NAME(proxsData->profileName), 
                                         TRUE, 
                                         "Successfully Connected with: %04X:%02X:%06X", 
                                         ind->deviceAddr.addr.nap, 
                                         ind->deviceAddr.addr.uap, 
                                         ind->deviceAddr.addr.lap);
                    proxsData->connectedDeviceAddr.addr.nap = ind->deviceAddr.addr.nap;
                    proxsData->connectedDeviceAddr.addr.lap = ind->deviceAddr.addr.lap;
                    proxsData->connectedDeviceAddr.addr.uap = ind->deviceAddr.addr.uap;

                    /* Figure out which one we connected */
                    if (ind->btConnId == proxsData->leBtConnId)
                    {
                        proxsData->connectedBtConnId = proxsData->leBtConnId;
                    }
                    else
                    {
                        proxsData->bredrBtConnId = ind->btConnId;
                        proxsData->connectedBtConnId = proxsData->bredrBtConnId;
                    }
                }
                else
                {
                    CsrAppBacklogReqSend(TECH_BTLE, 
                                         PROFILE_NAME(proxsData->profileName), 
                                         TRUE, 
                                         "Failed to connect with: %04X:%02X:%06X, resultCode=%d, resultSupplier=%d", 
                                         ind->deviceAddr.addr.nap, 
                                         ind->deviceAddr.addr.uap, 
                                         ind->deviceAddr.addr.lap, 
                                         ind->resultCode, 
                                         ind->resultSupplier);

                    /* Figure out which one we failed to connect */
                    if (ind->btConnId == proxsData->leBtConnId)
                    {
                        proxsData->leBtConnId = CSR_BT_CONN_ID_INVALID;
                    }
                    else
                    {
                        proxsData->bredrBtConnId = CSR_BT_PROXS_BTCONN_UNSET;
                    }
                }

                /*update UI*/
                CsrBtGattAppHandleProxsCsrUiActivateDeactivate(proxsData, 
                                                              TRUE,
                                                              (CsrBool)((ind->resultCode == CSR_BT_PROX_SRV_RESULT_SUCCESS && 
                                                                         ind->resultSupplier == CSR_BT_SUPPLIER_PROX_SRV) ? TRUE : FALSE));
                break;
            }
        case CSR_BT_PROX_SRV_DISCONNECT_IND:
            {
                CsrBtProxSrvDisconnectInd    *ind;

                ind = (CsrBtProxSrvDisconnectInd *) proxsData->recvMsgP;

                /* Determine which one got disconnected */
                if (ind->btConnId == proxsData->connectedBtConnId)
                {
                    proxsData->connectedBtConnId = CSR_BT_CONN_ID_INVALID;
                    /*update UI*/
                    /*we are still activated as this was initiated by peer and server goes back into advertising state */
                    proxsData->csrUiVar.localInitiatedAction = TRUE;
                    CsrBtGattAppHandleProxsCsrUiActivateDeactivate(proxsData, 
                                                                   TRUE,
                                                                   FALSE);
                }

                if (ind->btConnId == proxsData->leBtConnId)
                {
                    proxsData->leBtConnId = CSR_BT_CONN_ID_INVALID;
                }
                else if (ind->btConnId == proxsData->bredrBtConnId)
                {
                    proxsData->bredrBtConnId = CSR_BT_PROXS_BTCONN_UNSET;
                }


               CsrAppBacklogReqSend(TECH_BTLE, 
                                     PROFILE_NAME(proxsData->profileName), 
                                     TRUE, 
                                     "Disconnected from: %04X:%02X:%06X", 
                                     ind->deviceAddr.addr.nap, 
                                     ind->deviceAddr.addr.uap, 
                                     ind->deviceAddr.addr.lap);

                /*Stop the TIMER */
                CsrSchedTimerCancel(proxsData->txPower_timer,
                                    0,
                                    NULL);

                /*update UI*/
                CsrBtGattAppHandleProxsCsrUiActivateDeactivate(proxsData, 
                                                              TRUE,
                                                              FALSE);
                break;
            }
        case CSR_BT_PROX_SRV_WRITE_EVENT_IND:
            {

                break;
            }
        case CSR_BT_PROX_SRV_TX_POWER_CHANGED_EVENT_IND:
            {
                break;
            }
        case CSR_BT_PROX_SRV_ALERT_EVENT_IND:
            {
                CsrBtProxSrvAlertEventInd    *ind;
                ind = (CsrBtProxSrvAlertEventInd *) proxsData->recvMsgP;
                CsrBtGattAppHandleProxsCsrUiValueUpdate(proxsData, 
                                                   ind->alertType, 
                                                   ind->alertLevel);
                
                break;
            }
        default:
            {
                CsrAppBacklogReqSend(TECH_BTLE, 
                                     PROFILE_NAME(proxsData->profileName), 
                                     TRUE, 
                                     "####### default in gatt prim handler 0x%04x,",
                                     *primType);
                CsrBtProxSrvFreeUpstreamMessageContents(CSR_BT_PROX_SRV_PRIM, proxsData->recvMsgP);
                /* unexpected primitive received */
            }
    }
}
#endif
