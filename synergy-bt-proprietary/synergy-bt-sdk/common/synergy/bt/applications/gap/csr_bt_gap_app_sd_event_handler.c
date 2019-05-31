/******************************************************************************
 Copyright (c) 2009-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.

 REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_exceptionhandler.h"
#include "csr_bt_gap_app_sd_event_handler.h"

typedef void (* csrBtSdAppStateHandlerType)(void * instData, void * message, const CsrBtSdAppEventHandlerStructType * csrBtSdAppEventHandlerPtr);

static void csrBtSdReadDeviceInfoCfmHandler(void * instData, void * message, const CsrBtSdAppEventHandlerStructType * csrBtSdAppEventHandlerPtr)
{
    CsrBtSdReadDeviceInfoCfm           * prim = (CsrBtSdReadDeviceInfoCfm *) message;

    if(csrBtSdAppEventHandlerPtr->csrBtSdReadDeviceInfoCfmFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_SD", 0, prim->type, "csrBtSdReadDeviceInfoCfmFunc Event function not defined\n");
    }
    else
    {
        csrBtSdAppEventHandlerPtr->csrBtSdReadDeviceInfoCfmFunc(instData, prim->resultSupplier, prim->resultCode, prim->deviceAddr, prim->deviceClass,
                                                                prim->infoLen, prim->info, prim->deviceStatus);
    }
}

static void csrBtSdSearchResultIndHandler(void * instData, void * message, const CsrBtSdAppEventHandlerStructType * csrBtSdAppEventHandlerPtr)
{
    CsrBtSdSearchResultInd                   * prim = (CsrBtSdSearchResultInd *) message;

    if(csrBtSdAppEventHandlerPtr->csrBtSdSearchResultIndFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_SD", 0, prim->type, "csrBtSdSearchResultIndFunc Event function not defined\n");
    }
    else
    {
        csrBtSdAppEventHandlerPtr->csrBtSdSearchResultIndFunc(instData, prim->deviceAddr, prim->addressType,
                                                              prim->deviceClass, prim->rssi, prim->infoLen,
                                                              prim->info, prim->deviceStatus);
    }
}

static void csrBtSdCloseSearchIndHandler(void * instData, void * message, const CsrBtSdAppEventHandlerStructType * csrBtSdAppEventHandlerPtr)
{
    CsrBtSdCloseSearchInd                    * prim = (CsrBtSdCloseSearchInd *) message;

    if(csrBtSdAppEventHandlerPtr->csrBtSdCloseSearchIndFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_SD", 0, prim->type, "csrBtSdCloseSearchIndFunc Event function not defined\n");
    }
    else
    {
        csrBtSdAppEventHandlerPtr->csrBtSdCloseSearchIndFunc(instData, prim->resultSupplier, prim->resultCode);
    }
}

static void csrBtSdReadAvailableServicesCfmHandler(void * instData, void * message, const CsrBtSdAppEventHandlerStructType * csrBtSdAppEventHandlerPtr)
{
    CsrBtSdReadAvailableServicesCfm          * prim = (CsrBtSdReadAvailableServicesCfm *) message;

    if(csrBtSdAppEventHandlerPtr->csrBtSdReadAvailableServicesCfmFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_SD", 0, prim->type, "csrBtSdReadAvailableServicesCfmFunc Event function not defined\n");
    }
    else
    {
        csrBtSdAppEventHandlerPtr->csrBtSdReadAvailableServicesCfmFunc(instData, prim->resultSupplier, prim->resultCode, prim->deviceAddr, prim->deviceClass,
                                                                prim->infoLen, prim->info, prim->deviceStatus);
    }
}

static void csrBtSdCancelReadAvailableServicesCfmHandler(void * instData, void * message, const CsrBtSdAppEventHandlerStructType * csrBtSdAppEventHandlerPtr)
{
    if(csrBtSdAppEventHandlerPtr->csrBtSdCancelReadAvailableServicesCfmFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_SD", 0, ((CsrBtSdCancelReadAvailableServicesCfm *) message)->type, "csrBtSdCancelReadAvailableServicesCfmFunc Event function not defined\n");
    }
    else
    {
        csrBtSdAppEventHandlerPtr->csrBtSdCancelReadAvailableServicesCfmFunc(instData);
    }
}

static void csrBtSdReadDeviceListIndHandler(void * instData, void * message, const CsrBtSdAppEventHandlerStructType * csrBtSdAppEventHandlerPtr)
{
    CsrBtSdReadDeviceListInd             * prim = (CsrBtSdReadDeviceListInd *) message;

    if(csrBtSdAppEventHandlerPtr->csrBtSdReadDeviceListIndFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_SD", 0, prim->type, "csrBtSdReadDeviceListIndFunc Event function not defined\n");
    }
    else
    {
        csrBtSdAppEventHandlerPtr->csrBtSdReadDeviceListIndFunc(instData, prim->deviceInfoCount, prim->deviceInfo);
    }
}

static void csrBtSdReadDeviceListCfmHandler(void * instData, void * message, const CsrBtSdAppEventHandlerStructType * csrBtSdAppEventHandlerPtr)
{
    CsrBtSdReadDeviceListCfm             * prim = (CsrBtSdReadDeviceListCfm *) message;

    if(csrBtSdAppEventHandlerPtr->csrBtSdReadDeviceListCfmFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_SD", 0, prim->type, "csrBtSdReadDeviceListCfmFunc Event function not defined\n");
    }
    else
    {
        csrBtSdAppEventHandlerPtr->csrBtSdReadDeviceListCfmFunc(instData, prim->deviceInfoCount, prim->totalNumOfDevices, prim->deviceInfo);
    }
}

static const csrBtSdAppStateHandlerType csrBtSdAppStateHandler[CSR_BT_SD_PRIM_UPSTREAM_COUNT] =
{
    csrBtSdReadDeviceInfoCfmHandler,                 /* CSR_BT_SD_READ_DEVICE_INFO_CFM              */
    csrBtSdSearchResultIndHandler,                   /* CSR_BT_SD_SEARCH_RESULT_IND                 */
    csrBtSdCloseSearchIndHandler,                    /* CSR_BT_SD_CLOSE_SEARCH_IND                  */
    csrBtSdReadAvailableServicesCfmHandler,          /* CSR_BT_SD_READ_AVAILABLE_SERVICES_CFM       */
    csrBtSdCancelReadAvailableServicesCfmHandler,    /* CSR_BT_SD_CANCEL_READ_AVAILABLE_SERVICES_CFM*/
    csrBtSdReadDeviceListIndHandler,                 /* CSR_BT_SD_READ_DEVICE_LIST_IND              */
    csrBtSdReadDeviceListCfmHandler,                 /* CSR_BT_SD_READ_DEVICE_LIST_CFM              */
    NULL,                                            /* CSR_BT_SD_READ_SERVICE_RECORD_IND           */
    NULL,                                            /* CSR_BT_SD_READ_SERVICE_RECORD_CFM           */
    NULL,                                            /* CSR_BT_SD_REGISTER_SERVICE_RECORD_CFM       */
    NULL,                                            /* CSR_BT_SD_UNREGISTER_SERVICE_RECORD_CFM     */
    NULL,                                            /* CSR_BT_SD_IMMEDIATE_SEARCH_RESULT_IND       */
    NULL,                                            /* CSR_BT_SD_MEMORY_CONFIG_CFM                 */
    NULL,                                            /* CSR_BT_SD_SEARCH_CONFIG_CFM                 */
    NULL                                             /* CSR_BT_SD_CLEANUP_CFM                       */
};

void CsrBtSdAppEventHandler(void * instData, void * message, const CsrBtSdAppEventHandlerStructType *csrBtSdAppEventHandlerPtr)
{
    CsrBtSdPrim * primType = (CsrBtSdPrim *) message;
    CsrUint16    index     = (CsrUint16)(*primType - CSR_BT_SD_PRIM_UPSTREAM_LOWEST);

    if(index < CSR_BT_SD_PRIM_UPSTREAM_COUNT && csrBtSdAppStateHandler[index] != NULL)
    {
        csrBtSdAppStateHandler[index](instData, message, csrBtSdAppEventHandlerPtr);
    }
    else
    {
        CsrGeneralException("CSR_BT_GAP_SD", 0, *primType, "Received CSR_BT SD primitive is unknown or private\n");
    }
}

