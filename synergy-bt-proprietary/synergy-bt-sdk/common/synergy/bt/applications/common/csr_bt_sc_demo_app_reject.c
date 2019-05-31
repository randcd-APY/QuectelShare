/******************************************************************************
 Copyright (c) 2009-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.

 REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"

/* Standard include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* Profile manager include files */
#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_bt_sc_prim.h"
#include "csr_bt_sc_lib.h"
#include "csr_bt_sc_demo_app.h"

static void scAppRejectPasskeyIndSend(ScAppInstData_t *instData)
{
    CsrBtScPasskeyNegResSendEx(instData->deviceAddr,instData->addressType);
}

#if CSR_BT_BT_VERSION >= CSR_BT_BLUETOOTH_VERSION_2P1
static void scAppRejectSspPasskeyIndSend(ScAppInstData_t *instData)
{
    CsrBtScSspPasskeyNegResSendEx(instData->deviceAddr,instData->addressType);
}

static void scAppRejectSspNotificationIndSend(ScAppInstData_t *instData)
{
    CsrBtScSspNotificationNegResSendEx(instData->deviceAddr,instData->addressType);
}

static void scAppRejectSspCompareIndSend(ScAppInstData_t *instData)
{
    CsrBtScSspCompareNegResSendEx(instData->deviceAddr,instData->addressType);
}

static void scAppRejectSspJustWorksIndSend(ScAppInstData_t *instData)
{
    CsrBtScSspJustWorksNegResSendEx(instData->deviceAddr,instData->addressType);
}

static void scAppRejectRebondIndSend(ScAppInstData_t *instData)
{
    CsrBtScRebondNegResSend(instData->deviceAddr);
}

static void scAppRejectSspPairingIndSend(ScAppInstData_t *instData)
{
    CsrBtScSspPairingNegResSendEx(instData->deviceAddr,
                                  instData->addressType,
                                  CSR_BT_TRASNPORT_BREDR);
}

#endif

const scStateHandlerType rejectPairingFunc[CSR_BT_SC_PRIM_UPSTREAM_COUNT] =
{
    scAppRejectPasskeyIndSend,              /* CSR_BT_SC_PASSKEY_IND                           */
    NULL,                                   /* CSR_BT_SC_BOND_CFM                              */
    NULL,                                   /* CSR_BT_SC_DEBOND_CFM                            */
    NULL,                                   /* CSR_BT_SC_SET_SECURITY_MODE_CFM                 */
    NULL,                                   /* CSR_BT_SC_ENCRYPTION_CFM                        */
    NULL,                                   /* CSR_BT_SC_AUTHORISE_IND                         */
    NULL,                                   /* CSR_BT_SC_BOND_IND                              */
    NULL,                                   /* CSR_BT_SC_READ_DEVICE_RECORD_IND                */
    NULL,                                   /* CSR_BT_SC_READ_DEVICE_RECORD_CFM                */
    NULL,                                   /* CSR_BT_SC_UPDATE_TRUST_LEVEL_IND                */
    NULL,                                   /* CSR_BT_SC_AUTHENTICATE_CFM                      */
#if CSR_BT_BT_VERSION >= CSR_BT_BLUETOOTH_VERSION_2P1
    scAppRejectSspPasskeyIndSend,           /* CSR_BT_SC_SSP_PASSKEY_IND                       */
    scAppRejectSspNotificationIndSend,      /* CSR_BT_SC_SSP_NOTIFICATION_IND                  */
    scAppRejectSspCompareIndSend,           /* CSR_BT_SC_SSP_COMPARE_IND                       */
    scAppRejectSspJustWorksIndSend,         /* CSR_BT_SC_SSP_JUSTWORKS_IND                     */
    NULL,                                   /* CSR_BT_SC_READ_LOCAL_OOB_DATA_CFM               */
    scAppRejectSspPairingIndSend,           /* CSR_BT_SC_SSP_PAIRING_IND                       */
    NULL,                                   /* CSR_BT_SC_SSP_KEYPRESS_NOTIFICATION_IND         */
    NULL,                                   /* CSR_BT_SC_MODE_IND                              */
    scAppRejectRebondIndSend,               /* CSR_BT_SC_REBOND_IND                            */
#else
    NULL,                                   /* CSR_BT_SC_READ_LOCAL_OOB_DATA_CFM               */
    NULL,                                   /* CSR_BT_SC_SSP_PAIRING_IND                       */
    NULL,                                   /* CSR_BT_SC_SSP_KEYPRESS_NOTIFICATION_IND         */
    NULL,                                   /* CSR_BT_SC_MODE_IND                              */
    NULL,                                   /* CSR_BT_SC_REBOND_IND                            */
#endif
};

