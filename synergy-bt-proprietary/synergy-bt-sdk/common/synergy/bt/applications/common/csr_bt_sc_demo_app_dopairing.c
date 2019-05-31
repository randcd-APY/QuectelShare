/******************************************************************************
 Copyright (c) 2008-2018 Qualcomm Technologies International, Ltd.
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

static void scAppDoPasskeyIndSend(ScAppInstData_t *instData)
{
    if ((instData->proximityPairing) && (instData->inputLength != 0))
    {
        CsrBtScPasskeyResSendEx(TRUE, instData->deviceAddr,
                             instData->inputLength,
                             instData->input,
                             TRUE, FALSE,
                             instData->addressType);
    }
    else
    {
        printf("\n[LEGACY PIN] - Enter PIN for %s (%04X:%02X:%06X/%X): ",
               (CsrCharString *) instData->deviceName,
                instData->deviceAddr.nap,
                instData->deviceAddr.uap,
                instData->deviceAddr.lap,
                instData->cod);
    }
}

#if CSR_BT_BT_VERSION >= CSR_BT_BLUETOOTH_VERSION_2P1
static void scAppDoSspPasskeyIndSend(ScAppInstData_t *instData)
{
    if ((instData->proximityPairing) && (instData->inputLength != 0))
    {
        CsrUint32 numericValue;
        numericValue = CsrStrToInt((char *)instData->input);

        CsrBtScSspPasskeyResSendEx(TRUE, instData->deviceAddr,
                            numericValue, TRUE, FALSE,
                            instData->addressType);
    }
    else
    {
        printf("\n[PASSKEY ENTRY] - Enter passkey for %s (%04X:%02X:%06X/%X): ",
               (CsrCharString *) instData->deviceName,
                instData->deviceAddr.nap,
                instData->deviceAddr.uap,
                instData->deviceAddr.lap,
                instData->cod);
    }
}

static void scAppDoSspNotificationIndSend(ScAppInstData_t *instData)
{
     printf("\n[PASSKEY NOTIFICATION] - Type %06u on %s (%04X:%02X:%06X/%X) [Y/N]: ",
           instData->numericValue,
           (CsrCharString *) instData->deviceName,
           instData->deviceAddr.nap,
           instData->deviceAddr.uap,
           instData->deviceAddr.lap,
           instData->cod);
}

static void scAppDoSspCompareIndSend(ScAppInstData_t *instData)
{
     printf("\n[NUMERIC COMPARISON] - Do the passkey %06u match with %s (%04X:%02X:%06X/%X) [Y/N]: ",
           instData->numericValue,
           (CsrCharString *) instData->deviceName,
           instData->deviceAddr.nap,
           instData->deviceAddr.uap,
           instData->deviceAddr.lap,
           instData->cod);
}

static void scAppDoSspJustWorksIndSend(ScAppInstData_t *instData)
{
    printf("\n[JUST WORKS] - Pairing with %s (%04X:%02X:%06X/%X)",
          (CsrCharString *) instData->deviceName,
          instData->deviceAddr.nap,
          instData->deviceAddr.uap,
          instData->deviceAddr.lap,
          instData->cod);

    CsrBtScSspJustWorksResSendEx(instData->accept, instData->deviceAddr, TRUE, FALSE,instData->addressType)
}

static void scAppDoRebondIndSend(ScAppInstData_t *instData)
{
    printf("\n[REBOND] - Rebonding with %s (%04X:%02X:%06X/%X)",
          (CsrCharString *) instData->deviceName,
          instData->deviceAddr.nap,
          instData->deviceAddr.uap,
          instData->deviceAddr.lap,
          instData->cod);

    CsrBtScRebondResSend(instData->deviceAddr, instData->accept);
}

static void scAppDoSspPairingIndSend(ScAppInstData_t *instData)
{
    printf("\n[PAIRING] - Pairing with %s (%04X:%02X:%06X/%X)",
          (CsrCharString *) instData->deviceName,
          instData->deviceAddr.nap,
          instData->deviceAddr.uap,
          instData->deviceAddr.lap,
          instData->cod);

    CsrBtScSspPairingResSendEx(instData->deviceAddr, instData->accept, TRUE, FALSE,
                               instData->addressType);
}
#endif

const scStateHandlerType acceptPairingFunc[CSR_BT_SC_PRIM_UPSTREAM_COUNT] =
{
    scAppDoPasskeyIndSend,                 /* CSR_BT_SC_PASSKEY_IND                    */
    NULL,                                  /* CSR_BT_SC_BOND_CFM                       */
    NULL,                                  /* CSR_BT_SC_DEBOND_CFM                     */
    NULL,                                  /* CSR_BT_SC_SET_SECURITY_MODE_CFM          */
    NULL,                                  /* CSR_BT_SC_ENCRYPTION_CFM                 */
    NULL,                                  /* CSR_BT_SC_AUTHORISE_IND                  */
    NULL,                                  /* CSR_BT_SC_BOND_IND                       */
    NULL,                                  /* CSR_BT_SC_READ_DEVICE_RECORD_IND         */
    NULL,                                  /* CSR_BT_SC_READ_DEVICE_RECORD_CFM         */
    NULL,                                  /* CSR_BT_SC_UPDATE_TRUST_LEVEL_IND         */
    NULL,                                  /* CSR_BT_SC_AUTHENTICATE_CFM               */
#if CSR_BT_BT_VERSION >= CSR_BT_BLUETOOTH_VERSION_2P1
    scAppDoSspPasskeyIndSend,              /* CSR_BT_SC_SSP_PASSKEY_IND                */
    scAppDoSspNotificationIndSend,         /* CSR_BT_SC_SSP_NOTIFICATION_IND           */
    scAppDoSspCompareIndSend,              /* CSR_BT_SC_SSP_COMPARE_IND                */
    scAppDoSspJustWorksIndSend,            /* CSR_BT_SC_SSP_JUSTWORKS_IND              */
    NULL,                                  /* CSR_BT_SC_READ_LOCAL_OOB_DATA_CFM        */
    scAppDoSspPairingIndSend,              /* CSR_BT_SC_SSP_PAIRING_IND                */
    NULL,                                  /* CSR_BT_SC_SSP_KEYPRESS_NOTIFICATION_IND */
    NULL,                                  /* CSR_BT_SC_MODE_IND                       */
    scAppDoRebondIndSend,                  /* CSR_BT_SC_REBOND_IND                     */
#else
    NULL,                                  /* CSR_BT_SC_SSP_PASSKEY_IND                */
    NULL,                                  /* CSR_BT_SC_SSP_NOTIFICATION_IND           */
    NULL,                                  /* CSR_BT_SC_SSP_COMPARE_IND                */
    NULL,                                  /* CSR_BT_SC_SSP_JUSTWORKS_IND              */
    NULL,                                  /* CSR_BT_SC_READ_LOCAL_OOB_DATA_CFM        */
    NULL,                                  /* CSR_BT_SC_SSP_PAIRING_IND                */
    NULL,                                  /* CSR_BT_SC_SSP_KEYPRESS_NOTIFICATION_IND */
    NULL,                                  /* CSR_BT_SC_MODE_IND                       */
    NULL,                                  /* CSR_BT_SC_REBOND_IND                     */
#endif
};

