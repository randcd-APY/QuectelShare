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
#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_sched.h"
#include "csr_ui_lib.h"
#include "csr_app_lib.h"
#include "csr_bt_gap_app_handler.h"
#include "csr_bt_gap_app_cm_event_handler.h"
#include "csr_util.h"
#include "csr_bt_cm_lib.h"

#define PROFILE_CM CsrUtf8StrDup((const CsrUtf8String *) "CM")

static void csrBtCmReadLocalDeviceAddressCfmFunc(
    void                    *instData,              /* the applications own instance data                                           */
    CsrBtDeviceAddr            localDeviceAddr      /* the local Bluetooth device address                      */
    )
{
    /* [QTI] Fix KW issue#831674 through increasing the size of buf from 33 to 256. */
    char buf[256];
    CsrBtGapInstData *inst = instData;

    inst->cmVar.localDeviceAddr = localDeviceAddr;

    snprintf(buf, sizeof(buf), "Local DeviceAddr: %04X:%02X:%06X", localDeviceAddr.nap, localDeviceAddr.uap, localDeviceAddr.lap);

    CsrAppBacklogReqSend(TECH_BT, PROFILE_CM, FALSE, buf);
}

static void csrBtCmSetLocalNameCfmFunc(void *instData, CsrBtSupplier supplier, CsrBtResultCode resultCode)
{
    if (supplier == CSR_BT_SUPPLIER_CM && resultCode == CSR_BT_RESULT_CODE_CM_SUCCESS)
    {
#ifdef CSR_BT_INSTALL_CM_READ_LOCAL_NAME
        CsrBtCmReadLocalNameReqSend(CsrSchedTaskQueueGet());
#endif
    }
    else
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_CM, FALSE, "Set local name failed");
    }
}

static void csrBtCmReadLocalNameCfmFunc(
    void                    *instData,              /* the applications own instance data                                           */
    CsrUtf8String           *localName              /* the friendly name of the local Bluetooth device in utf8                      */
                                        )
{
    /*
    This message is send to the tasks specified in the parameter "phandle"
    in the CSR_BT_CM_READ_LOCAL_NAME_REQ message.
    This is the confimation of read local name procedure (initiated by a CSR_BT_CM_READ_LOCAL_NAME_REQ).
    */
    CsrBtGapInstData *inst = instData;

    CsrStrLCpy((CsrCharString *) inst->cmVar.localName, (const CsrCharString *) localName,
                sizeof(inst->cmVar.localName));

    CsrAppBacklogReqSend(TECH_BT, PROFILE_CM, FALSE, (CsrCharString *) localName);
    CsrPmemFree(localName);
}

static void csrBtCmWriteCodCfmFunc(
    void                    *instData,              /* the applications own instance data                                           */
    CsrBtResultCode         resultCode,
    CsrBtSupplier           resultSupplier)
{
    CsrAppBacklogReqSend(TECH_BT, PROFILE_CM, FALSE, "COD set to Mobile Phone");
}

static void csrBtCmReadLocalVersionCfmFunc(
    void                    *instData,              /* the applications own instance data                                           */
    CsrUint8                 lmpVersion,             /* the local lmpVersion                      */
    CsrUint8                 hciVersion,
    CsrUint16                hciRevision,
    CsrUint16                manufacturerName,
    CsrUint16                lmpSubversion
)
{
    char buf[110];
    CsrBtGapInstData *inst = instData;

    inst->cmVar.lmpVersion = lmpVersion;
    inst->cmVar.hciRevision = hciRevision;
    inst->cmVar.hciVersion = hciVersion;
    inst->cmVar.lmpSubversion = lmpSubversion;
    inst->cmVar.manufacturerName = manufacturerName;

    snprintf(buf, sizeof(buf), "HCI Version: %03d HCI Revision: %06d Local LMP Version: %03d Manuf Name: %06d LMP Subversion: %06d",
            hciVersion, hciRevision,lmpVersion,manufacturerName,lmpSubversion);

    CsrAppBacklogReqSend(TECH_BT, PROFILE_CM, FALSE, buf);

    CsrUiStatusBluetoothSetReqSend(TRUE);
}


static void csrBtCmSetOwnAddressTypeCfmFunc(void           *instData,
                                            CsrBtSupplier   supplier,
                                            CsrBtResultCode resultCode)
{
    CsrBtGapInstData * inst = (CsrBtGapInstData *) instData;

    if (supplier == CSR_BT_SUPPLIER_CM && resultCode == CSR_BT_RESULT_CODE_CM_SUCCESS)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_CM, FALSE, "Set own address type is Success");
    }
    else
    {
        inst->cmVar.ownAddressType = 0;
        CsrAppBacklogReqSend(TECH_BT, PROFILE_CM, FALSE, "Set own address type is failed");
    }
}

/*
Setup of the functions.
The table below shall be configured for each application that can receive
messages from the CSR_BT CM.
If some of the messages will never occur in the specific application the function reference
can be set to "NULL" in the table.
*/
static const struct CsrBtCmAppEventHandlerStructType csrBtCmAppEventFunctions =
{
        csrBtCmSetLocalNameCfmFunc,             /* CSR_BT_CM_SET_LOCAL_NAME_CFM            */
        csrBtCmReadLocalDeviceAddressCfmFunc,   /* CSR_BT_CM_READ_LOCAL_BD_ADDR_CFM        */
        NULL,                                   /* CSR_BT_CM_WRITE_LINK_SUPERV_TIMEOUT_CFM */
        csrBtCmReadLocalNameCfmFunc,            /* CSR_BT_CM_READ_LOCAL_NAME_CFM           */
        NULL,                                   /* CSR_BT_CM_READ_TX_POWER_LEVEL_CFM       */
        NULL,                                   /* CSR_BT_CM_GET_LINK_QUALITY_CFM          */
        NULL,                                   /* CSR_BT_CM_READ_RSSI_CFM                 */
        csrBtCmWriteCodCfmFunc,                 /* CSR_BT_CM_WRITE_COD_CFM                 */
        NULL,                                   /* CSR_BT_CM_READ_COD_CFM                  */
        csrBtCmReadLocalVersionCfmFunc,         /* CSR_BT_CM_READ_LOCAL_VERSION_CFM        */
        NULL,                                   /* CSR_BT_CM_READ_REMOTE_VERSION_CFM       */
        NULL,                                   /* CSR_BT_CM_READ_SCAN_ENABLE_CFM          */
        NULL,                                   /* CSR_BT_CM_WRITE_SCAN_ENABLE_CFM         */
        csrBtCmSetOwnAddressTypeCfmFunc,        /* CSR_BT_CM_LE_SET_OWN_ADDRESS_TYPE_CFM   */
};

/* setup the global functional pointer that shall be overloaded to the event function */
const CsrBtCmAppEventHandlerStructType * const CsrBtGapCsrBtCmAppEventFunctionsPtr = &csrBtCmAppEventFunctions;
