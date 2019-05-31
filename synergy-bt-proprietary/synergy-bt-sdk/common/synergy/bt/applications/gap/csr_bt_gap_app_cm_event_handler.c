/******************************************************************************
 Copyright (c) 2009-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.

 REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_exceptionhandler.h"
#include "csr_bt_gap_app_cm_event_handler.h"

typedef void (* csrBtCmAppStateHandlerType)(void * instData, void * message, CsrBtCmAppEventHandlerStructType * csrBtCmAppEventHandlerPtr);

static void csrBtCmSetLocalNameCfmHandler(void * instData, void * message, CsrBtCmAppEventHandlerStructType * csrBtCmAppEventHandlerPtr)
{
    CsrBtCmSetLocalNameCfm         * prim = (CsrBtCmSetLocalNameCfm *) message;

    if(csrBtCmAppEventHandlerPtr->csrBtCmSetLocalNameCfmFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_CM", 0, prim->type, "csrBtCmSetLocalNameCfmFunc Event function not defined\n");
    }
    else
    {
        csrBtCmAppEventHandlerPtr->csrBtCmSetLocalNameCfmFunc(instData, prim->resultSupplier, prim->resultCode);
    }
}

static void csrBtCmReadLocalDeviceAddressCfmHandler(void * instData, void * message, CsrBtCmAppEventHandlerStructType * csrBtCmAppEventHandlerPtr)
{
    CsrBtCmReadLocalBdAddrCfm     * prim = (CsrBtCmReadLocalBdAddrCfm *) message;

    if(csrBtCmAppEventHandlerPtr->csrBtCmReadLocalDeviceAddressCfmFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_CM", 0, prim->type, "csrBtCmReadLocalDeviceAddressCfmFunc Event function not defined\n");
    }
    else
    {
        csrBtCmAppEventHandlerPtr->csrBtCmReadLocalDeviceAddressCfmFunc(instData, prim->deviceAddr);
    }
}

static void csrBtCmWriteLinkSupervTimeoutCfmHandler(void * instData, void * message, CsrBtCmAppEventHandlerStructType * csrBtCmAppEventHandlerPtr)
{
    CsrBtCmWriteLinkSupervTimeoutCfm  * prim = (CsrBtCmWriteLinkSupervTimeoutCfm *) message;

    if(csrBtCmAppEventHandlerPtr->csrBtCmWriteLinkSupervTimeoutCfmFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_CM", 0, prim->type, "csrBtCmWriteLinkSupervTimeoutCfmFunc Event function not defined\n");
    }
    else
    {
        csrBtCmAppEventHandlerPtr->csrBtCmWriteLinkSupervTimeoutCfmFunc(instData, prim->resultSupplier, prim->resultCode, prim->deviceAddr);
    }
}

static void csrBtCmReadLocalNameCfmHandler(void * instData, void * message, CsrBtCmAppEventHandlerStructType * csrBtCmAppEventHandlerPtr)
{
    CsrBtCmReadLocalNameCfm            * prim = (CsrBtCmReadLocalNameCfm *) message;

    if(csrBtCmAppEventHandlerPtr->csrBtCmReadLocalNameCfmFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_CM", 0, prim->type, "csrBtCmReadLocalNameCfmFunc Event function not defined\n");
    }
    else
    {
        csrBtCmAppEventHandlerPtr->csrBtCmReadLocalNameCfmFunc(instData, prim->localName);
    }
}

static void csrBtCmReadTxPowerLevelCfmHandler(void * instData, void * message, CsrBtCmAppEventHandlerStructType * csrBtCmAppEventHandlerPtr)
{
    CsrBtCmReadTxPowerLevelCfm        * prim = (CsrBtCmReadTxPowerLevelCfm *) message;

    if(csrBtCmAppEventHandlerPtr->csrBtCmReadTxPowerLevelCfmFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_CM", 0, prim->type, "csrBtCmReadTxPowerLevelCfmFunc Event function not defined\n");
    }
    else
    {
        csrBtCmAppEventHandlerPtr->csrBtCmReadTxPowerLevelCfmFunc(instData, prim->deviceAddr, prim->resultSupplier, prim->resultCode, prim->powerLevel);
    }
}

static void csrBtCmGetLinkQualityCfmHandler(void * instData, void * message, CsrBtCmAppEventHandlerStructType * csrBtCmAppEventHandlerPtr)
{
    CsrBtCmGetLinkQualityCfm           * prim = (CsrBtCmGetLinkQualityCfm *) message;

    if(csrBtCmAppEventHandlerPtr->csrBtCmGetLinkQualityCfmFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_CM", 0, prim->type, "csrBtCmGetLinkQualityCfmFunc Event function not defined\n");
    }
    else
    {
        csrBtCmAppEventHandlerPtr->csrBtCmGetLinkQualityCfmFunc(instData, prim->deviceAddr, prim->resultSupplier, prim->resultCode, prim->linkQuality);
    }
}

static void csrBtCmReadRssiCfmHandler(void * instData, void * message, CsrBtCmAppEventHandlerStructType * csrBtCmAppEventHandlerPtr)
{
    CsrBtCmReadRssiCfm                  * prim = (CsrBtCmReadRssiCfm *) message;

    if(csrBtCmAppEventHandlerPtr->csrBtCmReadRssiCfmFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_CM", 0, prim->type, "csrBtCmReadRssiCfmFunc Event function not defined\n");
    }
    else
    {
        csrBtCmAppEventHandlerPtr->csrBtCmReadRssiCfmFunc(instData, prim->deviceAddr, prim->resultSupplier, prim->resultCode, prim->rssi);
    }
}

static void csrBtCmWriteCodCfmHandler(void * instData, void * message, CsrBtCmAppEventHandlerStructType * csrBtCmAppEventHandlerPtr)
{
    CsrBtCmWriteCodCfm                  * prim = (CsrBtCmWriteCodCfm *) message;

    if(csrBtCmAppEventHandlerPtr->csrBtCmWriteCodCfmFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_CM", 0, prim->type, "csrBtCmWriteCodCfmFunc Event function not defined\n");
    }
    else
    {
        csrBtCmAppEventHandlerPtr->csrBtCmWriteCodCfmFunc(instData, prim->resultSupplier, prim->resultCode);
    }
}

static void csrBtCmReadCodCfmHandler(void * instData, void * message, CsrBtCmAppEventHandlerStructType * csrBtCmAppEventHandlerPtr)
{
    CsrBtCmReadCodCfm                   * prim = (CsrBtCmReadCodCfm *) message;

    if(csrBtCmAppEventHandlerPtr->csrBtCmReadCodCfmFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_CM", 0, prim->type, "csrBtCmReadCodCfmFunc Event function not defined\n");
    }
    else
    {
        csrBtCmAppEventHandlerPtr->csrBtCmReadCodCfmFunc(instData, prim->resultSupplier, prim->resultCode, prim->classOfDevice);
    }
}

static void csrBtCmReadLocalVersionCfmHandler(void * instData, void * message, CsrBtCmAppEventHandlerStructType * csrBtCmAppEventHandlerPtr)
{
    CsrBtCmReadLocalVersionCfm         * prim = (CsrBtCmReadLocalVersionCfm *) message;

    if(csrBtCmAppEventHandlerPtr->csrBtCmReadLocalVersionCfmFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_CM", 0, prim->type, "csrBtCmReadLocalVersionCfmFunc Event function not defined\n");
    }
    else
    {
        csrBtCmAppEventHandlerPtr->csrBtCmReadLocalVersionCfmFunc(instData, prim->lmpVersion, prim->hciVersion, prim->hciRevision,
                                                                    prim->manufacturerName, prim->lmpSubversion);
    }
}

static void csrBtCmReadRemoteVersionCfmHandler(void * instData, void * message, CsrBtCmAppEventHandlerStructType * csrBtCmAppEventHandlerPtr)
{
    CsrBtCmReadRemoteVersionCfm        * prim = (CsrBtCmReadRemoteVersionCfm *) message;

    if(csrBtCmAppEventHandlerPtr->csrBtCmReadRemoteVersionCfmFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_CM", 0, prim->type, "csrBtCmReadRemoteVersionCfmFunc Event function not defined\n");
    }
    else
    {
        csrBtCmAppEventHandlerPtr->csrBtCmReadRemoteVersionCfmFunc(instData, prim->resultSupplier, prim->resultCode, prim->deviceAddr, prim->lmpVersion,
                                                                                            prim->manufacturerName, prim->lmpSubversion);
    }
}

static void csrBtCmReadScanEnableCfmHandler(void * instData, void * message, CsrBtCmAppEventHandlerStructType * csrBtCmAppEventHandlerPtr)
{
    CsrBtCmReadScanEnableCfm           * prim = (CsrBtCmReadScanEnableCfm *) message;

    if(csrBtCmAppEventHandlerPtr->csrBtCmReadScanEnableCfmFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_CM", 0, prim->type, "csrBtCmReadScanEnableCfmFunc Event function not defined\n");
    }
    else
    {
        csrBtCmAppEventHandlerPtr->csrBtCmReadScanEnableCfmFunc(instData, prim->resultSupplier, prim->resultCode, prim->scanEnable);
    }
}

static void csrBtCmWriteScanEnableCfmHandler(void * instData, void * message, CsrBtCmAppEventHandlerStructType * csrBtCmAppEventHandlerPtr)
{
    CsrBtCmWriteScanEnableCfm          * prim = (CsrBtCmWriteScanEnableCfm *) message;

    if(csrBtCmAppEventHandlerPtr->csrBtCmWriteScanEnableCfmFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_CM", 0, prim->type, "csrBtCmWriteScanEnableCfmFunc Event function not defined\n");
    }
    else
    {
        csrBtCmAppEventHandlerPtr->csrBtCmWriteScanEnableCfmFunc(instData, prim->resultSupplier, prim->resultCode);
    }
}

static void csrBtCmSetOwnAddressTypeCfmHandler(void * instData, void * message, CsrBtCmAppEventHandlerStructType * csrBtCmAppEventHandlerPtr)
{
    CsrBtCmLeSetOwnAddressTypeCfm *prim = (CsrBtCmLeSetOwnAddressTypeCfm *) message;

    if(csrBtCmAppEventHandlerPtr->csrBtCmSetOwnAddressTypeCfmFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_CM", 0, prim->type, "csrBtCmWriteScanEnableCfmFunc Event function not defined\n");
    }
    else
    {
        csrBtCmAppEventHandlerPtr->csrBtCmSetOwnAddressTypeCfmFunc(instData, prim->resultSupplier, prim->resultCode);
    }
}
static const csrBtCmAppStateHandlerType csrBtCmAppStateHandler[CSR_BT_CM_DM_PRIM_UPSTREAM_COUNT] =
{
    csrBtCmSetLocalNameCfmHandler,                   /* CSR_BT_CM_SET_LOCAL_NAME_CFM                */
    csrBtCmReadLocalDeviceAddressCfmHandler,         /* CSR_BT_CM_READ_LOCAL_BD_ADDR_CFM            */
    csrBtCmWriteLinkSupervTimeoutCfmHandler,         /* CSR_BT_CM_WRITE_LINK_SUPERV_TIMEOUT_CFM     */
    csrBtCmReadLocalNameCfmHandler,                  /* CSR_BT_CM_READ_LOCAL_NAME_CFM               */
    csrBtCmReadTxPowerLevelCfmHandler,               /* CSR_BT_CM_READ_TX_POWER_LEVEL_CFM           */
    csrBtCmGetLinkQualityCfmHandler,                 /* CSR_BT_CM_GET_LINK_QUALITY_CFM              */
    csrBtCmReadRssiCfmHandler,                       /* CSR_BT_CM_READ_RSSI_CFM                     */
    csrBtCmWriteCodCfmHandler,                       /* CSR_BT_CM_WRITE_COD_CFM                     */
    csrBtCmReadCodCfmHandler,                        /* CSR_BT_CM_READ_COD_CFM                      */
    csrBtCmReadLocalVersionCfmHandler,               /* CSR_BT_CM_READ_LOCAL_VERSION_CFM            */
    csrBtCmReadRemoteVersionCfmHandler,              /* CSR_BT_CM_READ_REMOTE_VERSION_CFM           */
    csrBtCmReadScanEnableCfmHandler,                 /* CSR_BT_CM_READ_SCAN_ENABLE_CFM              */
    csrBtCmWriteScanEnableCfmHandler                 /* CSR_BT_CM_WRITE_SCAN_ENABLE_CFM             */
};

void CsrBtCmAppEventHandler(void * instData, void * message, CsrBtCmAppEventHandlerStructType *csrBtCmAppEventHandlerPtr)
{
    CsrBtCmPrim * primType = (CsrBtCmPrim *) message;

    if((*primType >= CSR_BT_CM_DM_PRIM_UPSTREAM_LOWEST) && (*primType <= CSR_BT_CM_DM_PRIM_UPSTREAM_HIGHEST) && (csrBtCmAppStateHandler[(CsrUint16)(*primType - CSR_BT_CM_DM_PRIM_UPSTREAM_LOWEST)] != NULL))
    {
        csrBtCmAppStateHandler[(CsrUint16)(*primType - CSR_BT_CM_DM_PRIM_UPSTREAM_LOWEST)](instData, message, csrBtCmAppEventHandlerPtr);
    }
    else if (*primType == CSR_BT_CM_LE_SET_OWN_ADDRESS_TYPE_CFM)
    {
        csrBtCmSetOwnAddressTypeCfmHandler(instData, message, csrBtCmAppEventHandlerPtr);
    }
    else
    {
        CsrGeneralException("CSR_BT_GAP_CM", 0, *primType, "Received CSR_BT CM primitive is unknown or private\n");
    }
}

