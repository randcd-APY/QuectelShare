/******************************************************************************
 Copyright (c) 2007-2018 Qualcomm Technologies International, Ltd.
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

#include "csr_types.h"
#include "csr_pmem.h"

/* Profile manager include files */
#include "csr_bt_tasks.h"
#include "bluetooth.h"
#include "csr_bt_profiles.h"
#include "csr_bt_sd_lib.h"
#include "csr_bt_util.h"
#include "csr_bt_platform.h"

#include "csr_bt_sc_lib.h"
#include "csr_bt_sc_demo_app.h"

extern BD_ADDR_T defGlobalBdAddr;
extern CsrUint8  defGlobalPinLen;
extern CsrUint8  defGlobalPin[CSR_BT_PASSKEY_MAX_LEN];

typedef void (*playScMenuFunc)(ScAppInstData_t *instData);

static CsrBool hasPinCode(ScAppInstData_t *instData)
{
    if ((instData->inputLength == 0) && (defGlobalPinLen > 0))
    {
        instData->inputLength = defGlobalPinLen;
        CsrMemCpy(instData->input, defGlobalPin, defGlobalPinLen);
        return TRUE;
    }
    return FALSE;
}

static void scAppMenuActive(ScAppInstData_t *instData, CsrUint8 state, playScMenuFunc menuCb)
{
    if (instData->state != SC_APP_NA && instData->state == state)
    {
        menuCb(instData);
    }
}

static void authoriseHandler(ScAppInstData_t *instData)
{
    CsrBtKeyPress_t *key;
    key = (CsrBtKeyPress_t*)instData->recvMsgP;

    switch (CSR_TOUPPER(key->key))
    {
        case 'Y':
            printf("Y\n");
            CsrBtScAuthoriseResSend( TRUE, instData->deviceAddr);
            instData->fnFini(instData->pFiniData);
            instData->state   = SC_APP_NA;
            break;

        case 'N':
            printf("N\n");
            CsrBtScAuthoriseResSend( FALSE, instData->deviceAddr);
            instData->fnFini(instData->pFiniData);
            instData->state   = SC_APP_NA;
            break;

        default:
            break;
    }
}

static void acceptPairingHandler(ScAppInstData_t *instData)
{
    CsrBtKeyPress_t *key;
    key = (CsrBtKeyPress_t*)instData->recvMsgP;

    switch (CSR_TOUPPER(key->key))
    {
        case 'Y':
            printf("Y\n");
            instData->accept = TRUE;
            acceptPairingFunc[instData->msgType-CSR_BT_SC_PRIM_UPSTREAM_LOWEST](instData);
            instData->state = SC_APP_SEC_PAIRING;
            break;

        case 'N':
            printf("N\n");
            instData->accept = FALSE;
            rejectPairingFunc[instData->msgType-CSR_BT_SC_PRIM_UPSTREAM_LOWEST](instData);
            break;

        default:
            break;
    }
}

static void legacyPasskeyHandler(ScAppInstData_t *instData)
{
    CsrBtKeyPress_t *key;
    key = (CsrBtKeyPress_t*)instData->recvMsgP;

    if (key->key == RETURN_KEY)
    {
        if (instData->inputLength == 0)
        {
            CsrBtScPasskeyNegResSendEx(instData->deviceAddr,
                                       instData->addressType);
            printf("\nSent empty passkey.\n");
        }
        else if( instData->inputLength >= instData->minPasskeyLength)
        {
            CsrBtScPasskeyResSendEx(TRUE, instData->deviceAddr,
                             instData->inputLength,
                             instData->input,
                             TRUE, FALSE,
                             instData->addressType);
            printf("\nSent the entered passkey.\n");
        }
        else
        {
           CsrBtScPasskeyNegResSendEx(instData->deviceAddr,
                                    instData->addressType);
           printf("\nPasskey provided is too short. Requiring a minimum length of %o.\n", instData->minPasskeyLength);

        }
    }
    else if (key->key == BACKSPACE_KEY)
    {
        if (instData->inputLength > 0)
        {
            instData->inputLength--;
            printf("\b \b");
        }
    }
    else if ((key->key >= 32) && (key->key < 127))
    {
        if (instData->inputLength < 16)
        {
            instData->input[instData->inputLength++] = key->key;
            printf("%c", key->key);
        }
    }
}

#if CSR_BT_BT_VERSION >= CSR_BT_BLUETOOTH_VERSION_2P1
static void sspPasskeyHandler(ScAppInstData_t *instData)
{
    CsrBtKeyPress_t *key;
    key = (CsrBtKeyPress_t*)instData->recvMsgP;

    if (key->key == RETURN_KEY)
    {
        printf("\n");
        CsrBtScSspKeypressNotificationReqSend(HCI_NOTIFICATION_TYPE_PASSKEY_COMPLETED);

        if (instData->inputLength == 0)
        {
            CsrBtScSspPasskeyNegResSendEx(instData->deviceAddr,
                                          instData->addressType);
        }
        else
        {
            CsrUint32 numericValue;
            numericValue = CsrStrToInt((char *)instData->input);

            CsrBtScSspPasskeyResSendEx(TRUE, instData->deviceAddr,
                                numericValue, TRUE, FALSE,
                                instData->addressType);
        }
    }
    else if (key->key == BACKSPACE_KEY)
    {
        if (instData->inputLength > 0)
        {
            instData->inputLength--;
            printf("\b \b");
            CsrBtScSspKeypressNotificationReqSend(HCI_NOTIFICATION_TYPE_PASSKEY_DIGIT_ERASED);
        }
    }
    else if ((key->key >= 32) && (key->key < 127))
    {
        if (instData->inputLength < 16)
        {
            instData->input[instData->inputLength++] = key->key;
            printf("%c", key->key);

            if (instData->inputLength == 1)
            {
                CsrBtScSspKeypressNotificationReqSend(HCI_NOTIFICATION_TYPE_PASSKEY_STARTED);
            }
            else
            {
                CsrBtScSspKeypressNotificationReqSend(HCI_NOTIFICATION_TYPE_PASSKEY_DIGIT_ENTERED);
            }
        }
    }
}

static void sspCompareHandler(ScAppInstData_t *instData)
{
    CsrBtKeyPress_t *key;
    key = (CsrBtKeyPress_t*)instData->recvMsgP;

    if (CSR_TOUPPER(key->key) == 'Y')
    {
        CsrBtScSspCompareResSendEx(TRUE, instData->deviceAddr, TRUE, FALSE, instData->addressType);
        printf("Y\n");
    }
    else if (CSR_TOUPPER(key->key) == 'N')
    {
        CsrBtScSspCompareNegResSendEx(instData->deviceAddr,instData->addressType);
        printf("N\n");
    }
    else
    {
    }
}

static void sspNotificationHandler(ScAppInstData_t *instData)
{
    CsrBtKeyPress_t *key;
    key = (CsrBtKeyPress_t*)instData->recvMsgP;

    if (CSR_TOUPPER(key->key) == 'Y')
    {
        CsrBtScSspNotificationResSendEx(TRUE, instData->deviceAddr, TRUE, FALSE,instData->addressType);
        printf("Y\n");
    }
    else if (CSR_TOUPPER(key->key) == 'N')
    {
        CsrBtScSspNotificationNegResSendEx(instData->deviceAddr,instData->addressType);
        printf("N\n");
    }
    else
    {
    }
}
#endif

#ifndef EXCLUDE_CSR_BT_SC_MODULE_OPTIONAL
static const char *getBondableModeText(ScAppInstData_t *instData)
{
    if (instData->bondable == CSR_BT_SEC_MODE_PAIRABLE)
    {
        return "CSR_BT_SEC_MODE_PAIRABLE";
    }
    else if (instData->bondable == CSR_BT_SEC_MODE_NON_PAIRABLE)
    {
        return "CSR_BT_SEC_MODE_NON_PAIRABLE";
    }
    else
    {
        return "CSR_BT_SEC_MODE_NON_BONDABLE";
    }
}
#endif

static void playSecurityMenu(ScAppInstData_t *instData)
{
    system_cls();

    printf("\nSecurity controller configuration:\n");
#ifdef CSR_BT_INSTALL_SC_SECURITY_MODE
    printf("    1)    Set security mode [%o]\n", instData->securityMode);
#endif
    printf("    2)    Set security level\n");
    if (instData->securityMode == SEC_MODE4_SSP)
    {
#if CSR_BT_BT_VERSION >= CSR_BT_BLUETOOTH_VERSION_2P1
        printf("    3)    Set SSP IO capability [%o]\n", instData->ioCapability);
#endif
    }
    printf("    4)    Bond with: %04X:%02X:%06X\n",
           instData->deviceAddr.nap,
           instData->deviceAddr.uap,
           instData->deviceAddr.lap);
    printf("    5)    Debond with: %04X:%02X:%06X\n",
           instData->deviceAddr.nap,
           instData->deviceAddr.uap,
           instData->deviceAddr.lap);
#ifndef EXCLUDE_CSR_BT_SC_MODULE_OPTIONAL
    printf("    6)    Set bondable mode [%s]\n", getBondableModeText(instData));
#else
    printf("    6)    Set bondable mode (NOT enabled in build!!!)\n");
#endif
    printf("    7)    Encrypt connection to: %04X:%02X:%06X%s\n",
           instData->deviceAddr.nap,
           instData->deviceAddr.uap,
           instData->deviceAddr.lap,
           (instData->encrypted == TRUE) ? " [ON]" : " [OFF]");
    if (instData->securityMode == SEC_MODE4_SSP)
    {
#if CSR_BT_BT_VERSION >= CSR_BT_BLUETOOTH_VERSION_2P1
        printf("    8)    Set SSP Authentication Requirements [%o]\n", instData->authRequirements);
        printf("    9)    Set Event Mask [%o]\n", instData->eventMask);
        printf("    A)    OOB Data\n");
        printf("    B)    Toggle SSP Debug Mode [%s]\n", (char*)(instData->enableSspDebugMode
                                                                 ? "ON" : "OFF"));
#endif
    } /* CSR_BT_BLUETOOTH_VERSION_2P1 */
#ifdef SC_APP_INCLUDE_PRIVATE_API
    printf("    C)    Authenticate %04X:%02X:%06X\n",
           instData->deviceAddr.nap,
           instData->deviceAddr.uap,
           instData->deviceAddr.lap);
#endif
    printf("    x)    Back\n");
}

static void playSecurityMode(ScAppInstData_t *instData)
{
    system_cls();

    printf("\nSet security mode:\n");
    printf("    1) %sSet security mode 1 [0x00]\n", (instData->securityMode == SEC_MODE1_NON_SECURE) ? "-->" : "   ");
    printf("    2) %sSet security mode 2 [0x01]\n", (instData->securityMode == SEC_MODE2_SERVICE) ? "-->" : "   ");
    printf("    3) %sSet security mode 3 [0x02]\n", (instData->securityMode == SEC_MODE3_LINK) ? "-->" : "   ");
#if CSR_BT_BT_VERSION >= CSR_BT_BLUETOOTH_VERSION_2P1
    printf("    4) %sSet security mode 4 [0x03]\n", (instData->securityMode == SEC_MODE4_SSP) ? "-->" : "   ");
#endif
    printf("    x)    Back\n");
}

#if CSR_BT_BT_VERSION >= CSR_BT_BLUETOOTH_VERSION_2P1
static void playHashC(CsrUint8 *oobHashC)
{
    int i;

    if (oobHashC)
    {
        for (i=0; i < SIZE_OOB_DATA; ++i)
        {
            printf("%02X", oobHashC[i]);
        }
    }
    else
    {
        printf("UNKNOWN");
    }
    printf("\n");
}

static void playRandR(CsrUint8 *oobRandR)
{
    int i;

    if (oobRandR)
    {
        for (i=0; i < SIZE_OOB_DATA; ++i)
        {
            printf("%02X", oobRandR[i]);
        }
    }
    else
    {
        printf("UNKNOWN");
    }
    printf("\n");
}

static void playOobData(ScAppInstData_t *instData)
{
    system_cls();

    printf("\nOOB Data:\n");
    printf("    *)    Local Hash C: ");
    playHashC(instData->oobHashCLocal);
    printf("    *)    Local Rand R: ");
    playRandR(instData->oobRandRLocal);
    printf("    *)    Remote Hash C: ");
    playHashC(instData->oobHashCRemote);
    printf("    *)    Remote Rand R: ");
    playRandR(instData->oobRandRRemote);
    printf("    1)    Read Local Oob Data\n");
    printf("    2)    Set Remote Hash C\n");
    printf("    3)    Set Remote Rand R\n");
    printf("    x)    Back\n");
}
#endif

static void playSecurityLevel(ScAppInstData_t *instData)
{
    ScAppSecLvlInstData_t *secLevelInst;
    ScAppDemoSecurityReq  *req;

    secLevelInst = &instData->secLevelData[instData->secLevelIdx % instData->secLevelCnt];
    req          = &secLevelInst->req[secLevelInst->securityIncoming];

    system_cls();

    printf("\na)    Profile: %s\n", secLevelInst->profileName);
    printf("\nb)    Set security level%s\n", (secLevelInst->securityIncoming == TRUE) ? " [INCOMING]" : " [OUTGOING]");
    printf("        1)    Authorisation%s\n",   ((req->secLevel & CSR_BT_SEC_AUTHORISATION) == CSR_BT_SEC_AUTHORISATION) ? " [ENABLED]" : " [DISABLED]");
    printf("        2)    Encryption%s\n",      ((req->secLevel & CSR_BT_SEC_ENCRYPTION) == CSR_BT_SEC_ENCRYPTION) ? " [ENABLED]" : " [DISABLED]");
    printf("        3)    Authentication%s\n",  ((req->secLevel & CSR_BT_SEC_AUTHENTICATION) == CSR_BT_SEC_AUTHENTICATION) ? " [ENABLED]" : " [DISABLED]");
#if CSR_BT_BT_VERSION >= CSR_BT_BLUETOOTH_VERSION_2P1
    if (instData->securityMode == SEC_MODE4_SSP)
    {
       printf("        4)    MITM Protection%s\n", ((req->secLevel & CSR_BT_SEC_MITM) == CSR_BT_SEC_MITM) ? " [ENABLED]" : " [DISABLED]");
    }
#endif
    printf("        s)    Send security level request\n");
    printf("        x)    Back\n");
}

#if CSR_BT_BT_VERSION >= CSR_BT_BLUETOOTH_VERSION_2P1
static void playSecurityIoCaps(ScAppInstData_t *instData)
{
    system_cls();

    printf("\nSet SSP IO capability:\n");
    printf("    1) %sDisplay Only [0x00]\n",   (instData->ioCapability == HCI_IO_CAP_DISPLAY_ONLY) ? "-->" : "   ");
    printf("    2) %sDisplay Yes No [0x01]\n", (instData->ioCapability == HCI_IO_CAP_DISPLAY_YES_NO) ? "-->" : "   ");
    printf("    3) %sKeyboard Only [0x02]\n",  (instData->ioCapability == HCI_IO_CAP_KEYBOARD_ONLY) ? "-->" : "   ");
    printf("    4) %sNo IO [0x03]\n",          (instData->ioCapability == HCI_IO_CAP_NO_INPUT_NO_OUTPUT) ? "-->" : "   ");
    printf("    x)    Back\n");
}

static void playAuthRequirements(ScAppInstData_t *instData)
{
    system_cls();

    printf("\nSet SSP Authentication Requirements:\n");
    printf("    1) %sMITM_NOT_REQUIRED_NO_BONDING [0x00]\n",   (instData->authRequirements == HCI_MITM_NOT_REQUIRED_NO_BONDING) ? "-->" : "   ");
    printf("    2) %sMITM_REQUIRED_NO_BONDING [0x01]\n",   (instData->authRequirements == HCI_MITM_REQUIRED_NO_BONDING) ? "-->" : "   ");
    printf("    3) %sMITM_NOT_REQUIRED_DEDICATED_BONDING [0x02]\n",   (instData->authRequirements == HCI_MITM_NOT_REQUIRED_DEDICATED_BONDING) ? "-->" : "   ");
    printf("    4) %sMITM_REQUIRED_DEDICATED_BONDING [0x03]\n",   (instData->authRequirements == HCI_MITM_REQUIRED_DEDICATED_BONDING) ? "-->" : "   ");
    printf("    5) %sMITM_NOT_REQUIRED_GENERAL_BONDING [0x04]\n",   (instData->authRequirements == HCI_MITM_NOT_REQUIRED_GENERAL_BONDING) ? "-->" : "   ");
    printf("    6) %sMITM_REQUIRED_GENERAL_BONDING [0x05]\n",   (instData->authRequirements == HCI_MITM_REQUIRED_GENERAL_BONDING) ? "-->" : "   ");
    printf("    x)    Back\n");
}

static void playSetEventMask(ScAppInstData_t *instData)
{
    system_cls();

    printf("\nSet Event Mask:\n");
    printf("    1) %sSC_EVENT_MASK_REBOND\n", (instData->eventMask & CSR_BT_SC_EVENT_MASK_REBOND) ? "-->" : "   ");
    printf("    2) %sSC_EVENT_MASK_PAIR\n", (instData->eventMask & CSR_BT_SC_EVENT_MASK_PAIR) ? "-->" : "   ");
    printf("    3) %sSC_EVENT_MASK_AUTHORISE_EXTENDED\n", (instData->eventMask & CSR_BT_SC_EVENT_MASK_AUTHORISE_EXTENDED) ? "-->" : "   ");
    printf("    x)    Back\n");
}

static void handleScSspKeypressNotificationInd(ScAppInstData_t *instData)
{
    CsrBtScSspKeypressNotificationInd * prim;
    prim = (CsrBtScSspKeypressNotificationInd *) instData->recvMsgP;

    switch(prim->notificationType)
    {
        case HCI_NOTIFICATION_TYPE_PASSKEY_STARTED:
            /* TODO */
            break;
        case HCI_NOTIFICATION_TYPE_PASSKEY_CLEARED:
            /* TODO */
            break;
        case HCI_NOTIFICATION_TYPE_PASSKEY_COMPLETED:
            /* printf("\nRemote device has entered passkey"); */
            break;
        case HCI_NOTIFICATION_TYPE_PASSKEY_DIGIT_ENTERED:
            /* printf("*"); */
            break;
        case HCI_NOTIFICATION_TYPE_PASSKEY_DIGIT_ERASED:
            /* printf("\b \b"); */
            break;
    }
}
#endif

static void handleScBondCfm(ScAppInstData_t *instData)
{
    CsrBtScBondCfm *prim;
    prim = (CsrBtScBondCfm *)instData->recvMsgP;

    printf("\nBond confirm with result 0x%02X from supplier 0x%02X received from device: (%04X:%02X:%06X/%X)\n",
           prim->resultCode,
           prim->resultSupplier,
           prim->deviceAddr.nap,
           prim->deviceAddr.uap,
           prim->deviceAddr.lap,
           prim->cod);

    instData->state   = SC_APP_MAIN_MENU;
}

static void handleScDebondCfm(ScAppInstData_t *instData)
{
    CsrBtScDebondCfm *prim;
    prim = (CsrBtScDebondCfm *)instData->recvMsgP;

    printf("\nDebond confirm with result 0x%02X from supplier 0x%02X received from device: %04X:%02X:%06X\n",
           prim->resultCode,
           prim->resultSupplier,
           prim->deviceAddr.nap,
           prim->deviceAddr.uap,
           prim->deviceAddr.lap);

    instData->state   = SC_APP_MAIN_MENU;
}

static void handleScBondableInd(ScAppInstData_t *instData)
{
    CsrBtScModeInd *prim;
    prim = (CsrBtScModeInd *)instData->recvMsgP;

    printf("\nBondable mode changed to: %o\n", prim->mode);

    instData->state   = SC_APP_MAIN_MENU;
}

static void handleScAuthenticateCfm(ScAppInstData_t *instData)
{
    CsrBtScAuthenticateCfm *prim;
    prim = (CsrBtScAuthenticateCfm *)instData->recvMsgP;

    printf("\nAuthenticated with result 0x%02X from supplier 0x%02X for device (%04X:%02X:%06X)\n",
           prim->resultCode,
           prim->resultSupplier,
           prim->deviceAddr.nap,
           prim->deviceAddr.uap,
           prim->deviceAddr.lap);

    instData->state   = SC_APP_MAIN_MENU;
}

static void handleScEncryptCfm(ScAppInstData_t *instData)
{
    CsrBtScEncryptionCfm *prim;
    prim = (CsrBtScEncryptionCfm *)instData->recvMsgP;

    printf("\nEncryption mode changed to %o for device (%04X:%02X:%06X) with result 0x%02X from supplier 0x%02X\n",
           prim->encType,
           prim->deviceAddr.nap,
           prim->deviceAddr.uap,
           prim->deviceAddr.lap,
           prim->resultCode,
           prim->resultSupplier);

    instData->state   = SC_APP_MAIN_MENU;
}

static void scAppRestoreIfNeeded(ScAppInstData_t *instData)
{
    /* Only restore if we have actually hijacked the input */
    if (instData->state != SC_APP_NA)
    {
        instData->fnFini(instData->pFiniData);
        instData->state   = SC_APP_NA;
    }
}

static void handleScBondInd(ScAppInstData_t *instData)
{
    CsrBtScBondInd *prim;
    prim    = (CsrBtScBondInd *) instData->recvMsgP;

    scAppRestoreIfNeeded(instData);

    printf("\nBond indication with result 0x%02X (supplier 0x%02X) received from device: (%04X:%02X:%06X/%X)\n",
           prim->resultCode,
           prim->resultSupplier,
           prim->deviceAddr.nap,
           prim->deviceAddr.uap,
           prim->deviceAddr.lap,
           prim->cod);
}

static void handleSecurityDebond(ScAppInstData_t *instData)
{
    CsrBtScDebondReqSendEx(TESTQUEUE, instData->deviceAddr,
                           instData->addressType);

    playSecurityMenu(instData);

    printf("\nDebonding with: %04X:%02X:%06X",
           instData->deviceAddr.nap,
           instData->deviceAddr.uap,
           instData->deviceAddr.lap);
}

#ifdef CSR_BT_INSTALL_SC_MODE
static void handleSecurityBondable(ScAppInstData_t *instData)
{
    CsrBtScModeReqSend(CSR_BT_INFINITE_TIME, instData->bondable);

    playSecurityMenu(instData);

    printf("\nSetting bondable mode to: %s for %o (infinite) time",
           getBondableModeText(instData),
           CSR_BT_INFINITE_TIME);

    instData->bondable = (instData->bondable+1) % CSR_BT_SEC_MODE_MAX;
}
#endif

#ifdef SC_APP_INCLUDE_PRIVATE_API
static void handleSecurityAuthenticate(ScAppInstData_t *instData)
{
#ifdef CSR_BT_INSTALL_SC_AUTHENTICATE
    CsrBtScAuthenticateReqSend(TESTQUEUE, instData->deviceAddr);

    playSecurityMenu(instData);

    printf("\nAuthenticating...");

    instData->encrypted = !instData->encrypted;
#else
    playSecurityMenu(instData);

    printf("\nAuthenticate is NOT enabled in the build!!!");
#endif    
}
#endif

static void handleSecurityEncrypt(ScAppInstData_t *instData)
{
#ifdef CSR_BT_INSTALL_SC_ENCRYPTION
    CsrBtScEncryptionReqSend(TESTQUEUE, instData->deviceAddr, instData->encrypted);

    playSecurityMenu(instData);

    printf("\nSetting encryption mode to: %o", instData->encrypted);

    instData->encrypted = !instData->encrypted;
#else
    playSecurityMenu(instData);
    printf("\nPlease build with CSR_BT_INSTALL_SC_ENCRYPTION to enable encryption...", instData->encrypted);

#endif
}

static void handleSecurityBond(ScAppInstData_t *instData)
{
    CsrBtScBondReqSend(TESTQUEUE, instData->deviceAddr);

    printf("\nInitiating bonding with: %04X:%02X:%06X (Press C to cancel)",
           instData->deviceAddr.nap,
           instData->deviceAddr.uap,
           instData->deviceAddr.lap);
}

static void playSetSecurityModeCfm(ScAppInstData_t *instData)
{
    CsrBtScSetSecurityModeCfm *prim;
    prim = (CsrBtScSetSecurityModeCfm *) instData->recvMsgP;
    printf("\nSet security level confirm with result: 0x%02X from supplier 0x%02X\n", prim->resultCode, prim->resultSupplier);
}

static void handleSetSecurityModeCfm(ScAppInstData_t *instData)
{
    CsrBtScSetSecurityModeCfm *prim;
    prim = (CsrBtScSetSecurityModeCfm *) instData->recvMsgP;

    if (prim->resultCode == CSR_BT_RESULT_CODE_SC_SUCCESS &&
        prim->resultSupplier == CSR_BT_SUPPLIER_SC)
    {
        instData->securityMode = instData->newSecurityMode;
#ifdef CSR_BT_INSTALL_SC_OOB
        if (instData->securityMode == SEC_MODE4_SSP)
        {
            /* Fetch new OOB data */
            CsrBtScReadLocalOobDataReqSend(TESTQUEUE, CSR_BT_TRANSPORT_BREDR);
        }
#endif
        scAppMenuActive(instData, SC_APP_SEC_MODE_MENU, playSecurityMode);
    }

    scAppMenuActive(instData, SC_APP_SEC_MODE_MENU, playSetSecurityModeCfm);
}

static void handleSecurityCancelPairing(ScAppInstData_t *instData)
{
    CsrBtKeyPress_t *key;
    key = (CsrBtKeyPress_t*)instData->recvMsgP;

    switch (key->key)
    {
        case 'C':
        case 'c':
            CsrBtScCancelBondReqSendEx(TESTQUEUE,
                                     instData->deviceAddr,
                                     instData->addressType);
            printf("\nCancelling bonding with: %04X:%02X:%06X",
                   instData->deviceAddr.nap,
                   instData->deviceAddr.uap,
                   instData->deviceAddr.lap);
            break;

        default:
            break;
    }
}

void handleScAcceptPairing(ScAppInstData_t *instData)
{
    printf("\nAllow pairing attempt (%o) from device %s (%04X:%02X:%06X/%X) [Y/N]: ",
           instData->paired,
           (CsrCharString *) instData->deviceName,
           instData->deviceAddr.nap,
           instData->deviceAddr.uap,
           instData->deviceAddr.lap,
           instData->cod);
}

void handleScAcceptAuthorise(ScAppInstData_t *instData)
{
    printf("\nAllow connection to service %o from device %s (%04X:%02X:%06X/%X) [Y/N]: ",
           instData->serviceId,
           (CsrCharString *) instData->deviceName,
           instData->deviceAddr.nap,
           instData->deviceAddr.uap,
           instData->deviceAddr.lap,
           instData->cod);
}

void handleScAcceptAuthoriseExt(ScAppInstData_t  *instData,
                                dm_protocol_id_t protocolId,
                                CsrUint16        channel,
                                CsrBool          incoming)
{
    if (incoming)
    {
        printf("\nAllow peer ");
    }
    else
    {
        printf("\nAllow local ");
    }
    if (protocolId == CSR_BT_SC_PROTOCOL_L2CAP)
    {
        printf("initiated l2cap connection to service %o from device %s (%04X:%02X:%06X/%X) on PSM 0x%02X [Y/N]: ",
            instData->serviceId,
            (CsrCharString *) instData->deviceName,
            instData->deviceAddr.nap,
            instData->deviceAddr.uap,
            instData->deviceAddr.lap,
            instData->cod,
            channel);
    }
    else
    { /* protocolId == CSR_BT_SC_PROTOCOL_RFCOMM */
        printf("initiated rfcomm connection to service %o from device %s (%04X:%02X:%06X/%X) on local server channel 0x%02X [Y/N]: ",
           instData->serviceId,
           (CsrCharString *) instData->deviceName,
           instData->deviceAddr.nap,
           instData->deviceAddr.uap,
           instData->deviceAddr.lap,
           instData->cod,
           channel);
    }
}

#ifdef CSR_BT_INSTALL_SC_SECURITY_MODE
static void handleScSecModeMenu(ScAppInstData_t *instData)
{
    CsrBtKeyPress_t *key;
    key = (CsrBtKeyPress_t*)instData->recvMsgP;

    switch (CSR_TOUPPER(key->key))
    {
        case '1':
            instData->newSecurityMode = SEC_MODE1_NON_SECURE;
            CsrBtScSetSecurityModeReqSend(TESTQUEUE, SEC_MODE1_NON_SECURE);
            break;
        case '2':
            instData->newSecurityMode = SEC_MODE2_SERVICE;
            CsrBtScSetSecurityModeReqSend(TESTQUEUE, SEC_MODE2_SERVICE);
            break;
        case '3':
            instData->newSecurityMode = SEC_MODE3_LINK;
            CsrBtScSetSecurityModeReqSend(TESTQUEUE, SEC_MODE3_LINK);
            break;
#if CSR_BT_BT_VERSION >= CSR_BT_BLUETOOTH_VERSION_2P1
        case '4':
            instData->newSecurityMode = SEC_MODE4_SSP;
            CsrBtScSetSecurityModeReqSend(TESTQUEUE, SEC_MODE4_SSP);
            break;
#endif
        case 'X':
            instData->state = SC_APP_MAIN_MENU;
            playSecurityMenu(instData);
            break;

        default:
            playSecurityMode(instData);
            break;
    }
}
#endif
static void scAppSetSecurityLevel(ScAppInstData_t *instData)
{
    if (instData->fnSend != NULL)
    {
        instData->fnSend(instData->pSendData);
        playSecurityLevel(instData);
    }
}

static void scAppSetNextInOutSecLevel(ScAppInstData_t *instData)
{
    CsrBool newSecInLevel;
    ScAppSecLvlInstData_t *secLevelInst;

    secLevelInst = &instData->secLevelData[instData->secLevelIdx % instData->secLevelCnt];
    newSecInLevel = !secLevelInst->securityIncoming;

    if ((newSecInLevel &&
            secLevelInst->msgSecInType != SC_APP_SET_SEC_LEVEL_NA)
    ||
    (!newSecInLevel &&
            secLevelInst->msgSecOutType != SC_APP_SET_SEC_LEVEL_NA))
    {
        secLevelInst->securityIncoming = newSecInLevel;
    }
}

static void handleScLevelMenu(ScAppInstData_t *instData)
{
    CsrBtKeyPress_t        *key;
    ScAppSecLvlInstData_t *secLevelInst;
    ScAppDemoSecurityReq  *req;

    secLevelInst = &instData->secLevelData[instData->secLevelIdx % instData->secLevelCnt];
    req          = &secLevelInst->req[secLevelInst->securityIncoming];
    key          = (CsrBtKeyPress_t*)instData->recvMsgP;

    switch (CSR_TOUPPER(key->key))
    {
        case 'A':
            ++instData->secLevelIdx;
            playSecurityLevel(instData);
            break;
        case 'B':
        {
            scAppSetNextInOutSecLevel(instData);
            playSecurityLevel(instData);
            break;
        }
        case '1':
            req->secLevel ^= CSR_BT_SEC_AUTHORISATION;
            playSecurityLevel(instData);
            break;
        case '2':
            req->secLevel ^= CSR_BT_SEC_ENCRYPTION;
            playSecurityLevel(instData);
            break;
        case '3':
            req->secLevel ^= CSR_BT_SEC_AUTHENTICATION;
            playSecurityLevel(instData);
            break;
#if CSR_BT_BT_VERSION >= CSR_BT_BLUETOOTH_VERSION_2P1
        case '4':
            if (instData->securityMode == SEC_MODE4_SSP)
            {
                req->secLevel ^=CSR_BT_SEC_MITM;
                playSecurityLevel(instData);
            }
            break;
#endif
        case 'S':
            scAppSetSecurityLevel(instData);
            break;
        case 'X':
            instData->state = SC_APP_MAIN_MENU;
            playSecurityMenu(instData);
            break;

        default:
            playSecurityLevel(instData);
            break;
    }
}

#if CSR_BT_BT_VERSION >= CSR_BT_BLUETOOTH_VERSION_2P1
static void handleScSecIoMenu(ScAppInstData_t *instData)
{
    CsrBtKeyPress_t *key;
    key = (CsrBtKeyPress_t*)instData->recvMsgP;

    switch (CSR_TOUPPER(key->key))
    {
        case '1':
            instData->ioCapability= HCI_IO_CAP_DISPLAY_ONLY;
            CsrBtScSetIoCapabilityReqSend(instData->ioCapability);
            playSecurityIoCaps(instData);
            break;
        case '2':
            instData->ioCapability= HCI_IO_CAP_DISPLAY_YES_NO;
            CsrBtScSetIoCapabilityReqSend(instData->ioCapability);
            playSecurityIoCaps(instData);
            break;
        case '3':
            instData->ioCapability= HCI_IO_CAP_KEYBOARD_ONLY;
            CsrBtScSetIoCapabilityReqSend(instData->ioCapability);
            playSecurityIoCaps(instData);
            break;
        case '4':
            instData->ioCapability= HCI_IO_CAP_NO_INPUT_NO_OUTPUT;
            CsrBtScSetIoCapabilityReqSend(instData->ioCapability);
            playSecurityIoCaps(instData);
            break;
        case 'X':
            instData->state = SC_APP_MAIN_MENU;
            playSecurityMenu(instData);
            break;

        default:
            playSecurityIoCaps(instData);
            break;
    }
}

static void handleScAuthRequireMenu(ScAppInstData_t *instData)
{
    CsrBtKeyPress_t *key;
    key = (CsrBtKeyPress_t*)instData->recvMsgP;

    switch (CSR_TOUPPER(key->key))
    {
        case '1':
            instData->authRequirements = HCI_MITM_NOT_REQUIRED_NO_BONDING;
            CsrBtScSetAuthRequirementsReqSend(instData->authRequirements);
            playAuthRequirements(instData);
            break;
        case '2':
            instData->authRequirements = HCI_MITM_REQUIRED_NO_BONDING;
            CsrBtScSetAuthRequirementsReqSend(instData->authRequirements);
            playAuthRequirements(instData);
            break;
        case '3':
            instData->authRequirements = HCI_MITM_NOT_REQUIRED_DEDICATED_BONDING;
            CsrBtScSetAuthRequirementsReqSend(instData->authRequirements);
            playAuthRequirements(instData);
            break;
        case '4':
            instData->authRequirements = HCI_MITM_REQUIRED_DEDICATED_BONDING;
            CsrBtScSetAuthRequirementsReqSend(instData->authRequirements);
            playAuthRequirements(instData);
            break;
        case '5':
            instData->authRequirements = HCI_MITM_NOT_REQUIRED_GENERAL_BONDING;
            CsrBtScSetAuthRequirementsReqSend(instData->authRequirements);
            playAuthRequirements(instData);
            break;
        case '6':
            instData->authRequirements = HCI_MITM_REQUIRED_GENERAL_BONDING;
            CsrBtScSetAuthRequirementsReqSend(instData->authRequirements);
            playAuthRequirements(instData);
            break;
        case 'X':
            instData->state = SC_APP_MAIN_MENU;
            playSecurityMenu(instData);
            break;

        default:
            playAuthRequirements(instData);
            break;
    }
}

static void handleScEventMaskMenu(ScAppInstData_t *instData)
{
    CsrBtKeyPress_t *key;
    key = (CsrBtKeyPress_t*)instData->recvMsgP;

    switch (CSR_TOUPPER(key->key))
    {
        case '1':
            instData->eventMask ^= CSR_BT_SC_EVENT_MASK_REBOND;
            CsrBtScSetEventMaskReqSend(instData->eventMask);
            playSetEventMask(instData);
            break;
        case '2':
            instData->eventMask ^= CSR_BT_SC_EVENT_MASK_PAIR;
            CsrBtScSetEventMaskReqSend(instData->eventMask);
            playSetEventMask(instData);
            break;
        case '3':
            instData->eventMask ^= CSR_BT_SC_EVENT_MASK_AUTHORISE_EXTENDED;
            CsrBtScSetEventMaskReqSend(instData->eventMask);
            playSetEventMask(instData);
            break;
        case 'X':
            instData->state = SC_APP_MAIN_MENU;
            playSecurityMenu(instData);
            break;

        default:
            playSetEventMask(instData);
            break;
    }
}

static void scAppHexStrToStream(CsrUint8 * string, CsrUint8 * stream)
{
    int i;
    for (i=0; i < SIZE_OOB_DATA*2; i +=2)
    {
        CsrHexStrToUint8((char *)&string[i], &stream[i/2]);
    }
}

static void sspRemoteOob(ScAppInstData_t *instData)
{
    CsrBtKeyPress_t *key;
    key = (CsrBtKeyPress_t*)instData->recvMsgP;

    if (key->key == RETURN_KEY)
    {
        printf("\n");
        if (instData->inputLength == SIZE_OOB_DATA*2)
        {
            CsrUint8 *oobRemote = (instData->subState == SC_APP_SEC_OOB_SET_REM_HASHC_MENU)
                                    ? instData->oobHashCRemote
                                    : instData->oobRandRRemote;


            scAppHexStrToStream(instData->input, oobRemote);
#ifdef CSR_BT_INSTALL_SC_OOB
            CsrBtScAddRemoteOobDataReqSend(instData->deviceAddr,
                                           instData->addressType,
                                           CSR_BT_TRANSPORT_BREDR,
                                           CsrMemDup(instData->oobHashCRemote,
                                                     sizeof(instData->oobHashCRemote)),
                                           CsrMemDup(instData->oobRandRRemote,
                                                     sizeof(instData->oobRandRRemote)),
                                           NULL,
                                           NULL);
#else
            printf("ERROR: Please compile with CSR_BT_INSTALL_SC_OOB to enable OOB...\n");
#endif

            instData->subState = SC_APP_SEC_OOB_MAIN_MENU;
            playOobData(instData);

        }
    }
    else if (key->key == BACKSPACE_KEY)
    {
        if (instData->inputLength > 0)
        {
            instData->inputLength--;
            printf("\b \b");
        }
    }
    else if (((key->key >= 48) && (key->key < 58)) ||
             ((key->key >= 65) && (key->key < 71)) ||
             ((key->key >= 97) && (key->key < 103)))
    {
        if (instData->inputLength < SIZE_OOB_DATA*2)
        {
            instData->input[instData->inputLength++] = key->key;
            printf("%c", key->key);
        }
    }
}

static void handleOobMainMenu(ScAppInstData_t *instData)
{
    CsrBtKeyPress_t *key;
    key = (CsrBtKeyPress_t*)instData->recvMsgP;

    switch (CSR_TOUPPER(key->key))
    {
        case '1':
#ifdef CSR_BT_INSTALL_SC_OOB
            CsrBtScReadLocalOobDataReqSend(TESTQUEUE, CSR_BT_TRANSPORT_BREDR);
            playOobData(instData);
            printf("\nReading Local OOB Data...\n");
#else
            printf("\nERROR: Install OOB Data by compiling with CSR_BT_INSTALL_SC_OOB...\n");
#endif
            break;
        case '2':
            instData->subState = SC_APP_SEC_OOB_SET_REM_HASHC_MENU;
            instData->inputLength = 0;
            CsrMemSet(&instData->input, 0 , SIZE_OOB_DATA*2);
            printf("\nType remote OOB Hash C: ");
            break;
        case '3':
            instData->subState = SC_APP_SEC_OOB_SET_REM_RANDR_MENU;
            instData->inputLength = 0;
            CsrMemSet(&instData->input, 0 , SIZE_OOB_DATA*2);
            printf("\nType remote OOB Rand R: ");
            break;

        case 'X':
            instData->state = SC_APP_MAIN_MENU;
            playSecurityMenu(instData);
            break;

        default:
            playOobData(instData);
            break;
    }
}

static void handleOobMenu(ScAppInstData_t *instData)
{
    switch (instData->subState)
    {
        case SC_APP_SEC_OOB_MAIN_MENU:
            handleOobMainMenu(instData);
            break;
        case SC_APP_SEC_OOB_SET_REM_HASHC_MENU:
        case SC_APP_SEC_OOB_SET_REM_RANDR_MENU:
            sspRemoteOob(instData);
            break;

        default:
            playOobData(instData);
            break;
    }
}

static void handleSecuritySspDebugMode(ScAppInstData_t *instData)
{
    instData->enableSspDebugMode = !instData->enableSspDebugMode;

    playSecurityMenu(instData);
#ifdef CSR_BT_INSTALL_SC_DEBUG_MODE
    printf("\nSetting SSP debug mode to: %s", (char*)(instData->enableSspDebugMode
                                                      ? "ON" : "OFF"));
    CsrBtScDebugModeReqSend(TESTQUEUE, instData->enableSspDebugMode);
#else
    printf("\nERROR: Please build with CSR_BT_INSTALL_SC_DEBUG_MODE to enable debug mode\n");
#endif
}

#endif

static void handleScPairing(ScAppInstData_t *instData)
{
    switch (instData->msgType)
    {
        case CSR_BT_SC_PASSKEY_IND:
            if (instData->proximityPairing || hasPinCode(instData))
            {
                acceptPairingFunc[instData->msgType-CSR_BT_SC_PRIM_UPSTREAM_LOWEST](instData);
            }
            else
            {
                legacyPasskeyHandler(instData);
            }
            break;
        case CSR_BT_SC_AUTHORISE_IND:
        case CSR_BT_SC_AUTHORISE_EXT_IND:
            {
                if (instData->proximityPairing)
                {
                    CsrBtScAuthoriseResSend( TRUE, instData->deviceAddr);
                    instData->fnFini(instData->pFiniData);
                    instData->state   = SC_APP_NA;
                }
                else
                {
                    authoriseHandler(instData);
                }
                break;
            }
#if CSR_BT_BT_VERSION >= CSR_BT_BLUETOOTH_VERSION_2P1
        case CSR_BT_SC_SSP_PASSKEY_IND:
            if (instData->proximityPairing || hasPinCode(instData))
            {
                acceptPairingFunc[instData->msgType-CSR_BT_SC_PRIM_UPSTREAM_LOWEST](instData);
            }
            else
            {
                sspPasskeyHandler(instData);
            }
            break;
        case CSR_BT_SC_SSP_COMPARE_IND:
            {
                if (instData->proximityPairing)
                {
                    CsrBtScSspCompareResSendEx(TRUE, instData->deviceAddr, TRUE, FALSE,instData->addressType);
                }
                else
                {
                    sspCompareHandler(instData);
                }
                break;
            }
        case CSR_BT_SC_SSP_NOTIFICATION_IND:
            {
                if (instData->proximityPairing)
                {
                    CsrBtScSspNotificationResSendEx(TRUE, instData->deviceAddr, TRUE, FALSE, instData->addressType);
                }
                else
                {
                    sspNotificationHandler(instData);
                }
                break;
            }
        case CSR_BT_SC_SSP_JUSTWORKS_IND:
            /* Stay put. We don't have to type anything */
            break;
#endif
        default:
            printf("[%s:%d] Unhandled msg: %d in state: %dX\n", __FILE__, __LINE__, instData->msgType, instData->state);
            break;
    }
}

static void handleScMainMenu(ScAppInstData_t *instData)
{
    CsrBtKeyPress_t *key;
    key = (CsrBtKeyPress_t*)instData->recvMsgP;

    switch (CSR_TOUPPER(key->key))
    {
#ifdef CSR_BT_INSTALL_SC_SECURITY_MODE
        case '1':
            playSecurityMode(instData);
            instData->state = SC_APP_SEC_MODE_MENU;
            break;
#endif
        case '2':
            playSecurityLevel(instData);
            instData->state = SC_APP_SEC_LEVEL_MENU;
            break;
#if CSR_BT_BT_VERSION >= CSR_BT_BLUETOOTH_VERSION_2P1
        case '3':
            if (instData->securityMode == SEC_MODE4_SSP)
            {
                playSecurityIoCaps(instData);
                instData->state = SC_APP_SEC_IO_MENU;
            }
            break;
#endif
        case '4':
            handleSecurityBond(instData);
            instData->state = SC_APP_SEC_INIT_PAIRING;
            break;
        case '5':
            handleSecurityDebond(instData);
            break;
#ifdef CSR_BT_INSTALL_SC_MODE
        case '6':
            handleSecurityBondable(instData);
            break;
#endif
#ifdef CSR_BT_INSTALL_SC_ENCRYPTION
        case '7':
            handleSecurityEncrypt(instData);
            break;
#endif            
#if CSR_BT_BT_VERSION >= CSR_BT_BLUETOOTH_VERSION_2P1
        case '8':
            if (instData->securityMode == SEC_MODE4_SSP)
            {
                playAuthRequirements(instData);
                instData->state = SC_APP_SEC_AUTH_REQUIRE_MENU;
            }
            break;
        case '9':
            if (instData->securityMode == SEC_MODE4_SSP)
            {
                playSetEventMask(instData);
                instData->state = SC_APP_SEC_EVENT_MASK_MENU;
            }
            break;
        case 'A':
            if (instData->securityMode == SEC_MODE4_SSP)
            {
                playOobData(instData);
                instData->state = SC_APP_SEC_OOB_MENU;
                instData->subState = SC_APP_SEC_OOB_MAIN_MENU;
            }
            break;
        case 'B':
            handleSecuritySspDebugMode(instData);
            break;
#endif
#ifdef SC_APP_INCLUDE_PRIVATE_API
        case 'C':
            handleSecurityAuthenticate(instData);
            break;
#endif
        case 'X':
            instData->fnFini(instData->pFiniData);
            instData->state = SC_APP_NA;
            break;
        default:
            playSecurityMenu(instData);
            break;
    }
}

void handleScKeybPrim(ScAppInstData_t *instData)
{
    instData->recvMsgP = instData->fnMsg(instData->pMsgData);

    switch (instData->state)
    {
        case SC_APP_NA:
            playSecurityMenu(instData);
            instData->state = SC_APP_MAIN_MENU;
            instData->fnInit(instData->pInitData);
            break;
        case SC_APP_MAIN_MENU:
            handleScMainMenu(instData);
            break;
#ifdef CSR_BT_INSTALL_SC_SECURITY_MODE
        case SC_APP_SEC_MODE_MENU:
            handleScSecModeMenu(instData);
            break;
#endif
        case SC_APP_SEC_LEVEL_MENU:
            handleScLevelMenu(instData);
            break;
#if CSR_BT_BT_VERSION >= CSR_BT_BLUETOOTH_VERSION_2P1
        case SC_APP_SEC_IO_MENU:
            handleScSecIoMenu(instData);
            break;
        case SC_APP_SEC_AUTH_REQUIRE_MENU:
            handleScAuthRequireMenu(instData);
            break;
        case SC_APP_SEC_EVENT_MASK_MENU:
            handleScEventMaskMenu(instData);
            break;
        case SC_APP_SEC_OOB_MENU:
            handleOobMenu(instData);
            break;
#endif
        case SC_APP_SEC_AUTHORISE_MENU:
            authoriseHandler(instData);
            break;
        case SC_APP_SEC_INIT_PAIRING:
            handleSecurityCancelPairing(instData);
            break;
        case SC_APP_SEC_ACP_PAIRING:
            acceptPairingHandler(instData);
            break;
        case SC_APP_SEC_PAIRING:
            handleScPairing(instData);
            break;
        default:
            printf("[%s:%d] Unhandled event in state: %dX\n", __FILE__, __LINE__, instData->state);
            break;
    }

    CsrPmemFree(instData->recvMsgP);
}

static void scAppActiveIfNeeded(ScAppInstData_t *instData)
{
    if (instData->state == SC_APP_NA)
    {
        instData->fnInit(instData->pInitData);
    }
}

void handleScAuthoriseInd(ScAppInstData_t *instData)
{
    CsrPrim    *prim;

    prim = (CsrPrim *)instData->recvMsgP;

    instData->msgType       = *prim;

    scAppActiveIfNeeded(instData);

    switch (*prim)
    {
        case CSR_BT_SC_AUTHORISE_IND:
        {
            CsrBtScAuthoriseInd    *scPrim;
            scPrim = (CsrBtScAuthoriseInd *) instData->recvMsgP;

            instData->addressType = scPrim->addressType;
            instData->deviceAddr = scPrim->deviceAddr;
            instData->serviceId  = scPrim->serviceId;
            CsrUtf8StrNCpy(instData->deviceName, scPrim->deviceName, CSR_BT_MAX_FRIENDLY_NAME_LEN+1);
            CsrUtf8StrTruncate(instData->deviceName, CSR_BT_MAX_FRIENDLY_NAME_LEN);

            if (instData->proximityPairing)
            {
                CsrBtScAuthoriseResSend(TRUE, instData->deviceAddr);
                instData->fnFini(instData->pFiniData);
                instData->state   = SC_APP_NA;
            }
            else
            {
                instData->state   = SC_APP_SEC_AUTHORISE_MENU;
                handleScAcceptAuthorise(instData);
            }
            break;
        }
        case CSR_BT_SC_AUTHORISE_EXT_IND:
        {
            CsrBtScAuthoriseExtInd    *scPrim;
            scPrim = (CsrBtScAuthoriseExtInd *) instData->recvMsgP;

            instData->addressType = scPrim->addressType;
            instData->deviceAddr = scPrim->deviceAddr;
            instData->serviceId  = scPrim->serviceId;
            CsrUtf8StrNCpy(instData->deviceName, scPrim->deviceName, CSR_BT_MAX_FRIENDLY_NAME_LEN+1);
            CsrUtf8StrTruncate(instData->deviceName, CSR_BT_MAX_FRIENDLY_NAME_LEN);

            if (instData->proximityPairing)
            {
                CsrBtScAuthoriseResSend(TRUE, instData->deviceAddr);
                instData->fnFini(instData->pFiniData);
                instData->state   = SC_APP_NA;
            }
            else
            {
                instData->state   = SC_APP_SEC_AUTHORISE_MENU;
                handleScAcceptAuthoriseExt(instData, scPrim->protocolId, scPrim->channel, scPrim->incoming);
            }
            break;
        }
    }
}

void handleScPasskeyInd(ScAppInstData_t *instData)
{
    CsrPrim    *prim;

    prim = (CsrPrim *)instData->recvMsgP;

    instData->msgType       = *prim;
    instData->inputLength = 0;
    CsrMemSet(&instData->input, 0 , SIZE_LINK_KEY);

    scAppActiveIfNeeded(instData);

    switch (*prim)
    {
        case CSR_BT_SC_PASSKEY_IND:
            {
                CsrBtScPasskeyInd    *scPrim;
                scPrim = (CsrBtScPasskeyInd *) instData->recvMsgP;

                instData->addressType =  scPrim->addressType;
                instData->deviceAddr = scPrim->deviceAddr;
                instData->cod        = scPrim->cod;
                instData->paired     = scPrim->paired;
                CsrUtf8StrNCpy(instData->deviceName, scPrim->deviceName, CSR_BT_MAX_FRIENDLY_NAME_LEN+1);
                CsrUtf8StrTruncate(instData->deviceName, CSR_BT_MAX_FRIENDLY_NAME_LEN);

                if (instData->proximityPairing || hasPinCode(instData))
                {
                    instData->accept = TRUE;
                    acceptPairingFunc[instData->msgType-CSR_BT_SC_PRIM_UPSTREAM_LOWEST](instData);
                    instData->state = SC_APP_SEC_PAIRING;
                }
                else
                {
                    instData->state = SC_APP_SEC_ACP_PAIRING;
                    handleScAcceptPairing(instData);
                }
                break;
            }
    }
}

#if CSR_BT_BT_VERSION >= BLUETOOTH_VERSION_2P1
void handleScSspPassPrims(ScAppInstData_t *instData)
{
    CsrPrim    *prim;

    prim = (CsrPrim *)instData->recvMsgP;

    instData->msgType     = *prim;
    instData->inputLength = 0;
    CsrMemSet(&instData->input, 0 , SIZE_LINK_KEY);

    scAppActiveIfNeeded(instData);

    switch (*prim)
    {
        case CSR_BT_SC_SSP_NOTIFICATION_IND:
        case CSR_BT_SC_SSP_COMPARE_IND:
            {
                CsrBtScSspNotificationInd    *scPrim;
                scPrim = (CsrBtScSspNotificationInd *) instData->recvMsgP;

                instData->numericValue           = scPrim->numericValue;
                instData->addressType            = scPrim->addressType;
                instData->deviceAddr             = scPrim->deviceAddr;
                instData->cod                    = scPrim->cod;
                instData->paired                 = scPrim->paired;
                instData->remoteAuthRequirements = scPrim->authRequirements;
                CsrUtf8StrNCpy(instData->deviceName, scPrim->deviceName, CSR_BT_MAX_FRIENDLY_NAME_LEN+1);
                CsrUtf8StrTruncate(instData->deviceName, CSR_BT_MAX_FRIENDLY_NAME_LEN);

                if (hasPinCode(instData))
                {
                    instData->accept = TRUE;
                    acceptPairingFunc[instData->msgType-CSR_BT_SC_PRIM_UPSTREAM_LOWEST](instData);
                    instData->state = SC_APP_SEC_PAIRING;
                }
                else
                {
                    instData->state = SC_APP_SEC_ACP_PAIRING;
                    handleScAcceptPairing(instData);
                }
                break;
            }

        case CSR_BT_SC_SSP_JUSTWORKS_IND:
        case CSR_BT_SC_SSP_PASSKEY_IND:
            {
                CsrBtScSspJustworksInd    *scPrim;
                scPrim = (CsrBtScSspJustworksInd *) instData->recvMsgP;

                instData->addressType            = scPrim->addressType;
                instData->deviceAddr             = scPrim->deviceAddr;
                instData->cod                    = scPrim->cod;
                instData->paired                 = scPrim->paired;
                instData->remoteAuthRequirements = scPrim->authRequirements;
                CsrUtf8StrNCpy(instData->deviceName, scPrim->deviceName, CSR_BT_MAX_FRIENDLY_NAME_LEN+1);
                CsrUtf8StrTruncate(instData->deviceName, CSR_BT_MAX_FRIENDLY_NAME_LEN);

                if (hasPinCode(instData))
                {
                    instData->accept = TRUE;
                    acceptPairingFunc[instData->msgType-CSR_BT_SC_PRIM_UPSTREAM_LOWEST](instData);
                    instData->state = SC_APP_SEC_PAIRING;
                }
                else
                {
                    instData->state = SC_APP_SEC_ACP_PAIRING;
                    handleScAcceptPairing(instData);
                }
                break;
            }
    }
}

static void handleScRebondInd(ScAppInstData_t *instData)
{
    CsrBtScRebondInd *scPrim = (CsrBtScRebondInd *)instData->recvMsgP;

    instData->msgType    = scPrim->type;
    instData->addressType = scPrim->addressType;
    instData->deviceAddr = scPrim->deviceAddr;
    instData->cod         = scPrim->cod;
    CsrUtf8StrNCpy(instData->deviceName, scPrim->deviceName, CSR_BT_MAX_FRIENDLY_NAME_LEN+1);
    CsrUtf8StrTruncate(instData->deviceName, CSR_BT_MAX_FRIENDLY_NAME_LEN);

    scAppActiveIfNeeded(instData);

    if (instData->proximityPairing)
    {
        instData->accept = TRUE;
        acceptPairingFunc[instData->msgType-CSR_BT_SC_PRIM_UPSTREAM_LOWEST](instData);
        instData->state = SC_APP_SEC_PAIRING;
    }
    else
    {
        instData->state = SC_APP_SEC_ACP_PAIRING;
        printf("\nAllow rebond from device %s (%04X:%02X:%06X/%X) [Y/N]: ",
               (CsrCharString *) instData->deviceName,
               instData->deviceAddr.nap,
               instData->deviceAddr.uap,
               instData->deviceAddr.lap,
               instData->cod);
    }
}

static void handleScSspPairingInd(ScAppInstData_t *instData)
{
    CsrBtScSspPairingInd *scPrim = (CsrBtScSspPairingInd *)instData->recvMsgP;

    instData->msgType     = scPrim->type;
    instData->addressType = scPrim->addressType;
    instData->deviceAddr = scPrim->deviceAddr;
    instData->cod         = scPrim->cod;
    CsrUtf8StrNCpy(instData->deviceName, scPrim->deviceName, CSR_BT_MAX_FRIENDLY_NAME_LEN+1);
    CsrUtf8StrTruncate(instData->deviceName, CSR_BT_MAX_FRIENDLY_NAME_LEN);

    scAppActiveIfNeeded(instData);

    if (instData->proximityPairing)
    {
        instData->accept = TRUE;
        acceptPairingFunc[instData->msgType-CSR_BT_SC_PRIM_UPSTREAM_LOWEST](instData);
        instData->state = SC_APP_SEC_PAIRING;
    }
    else
    {
        instData->state = SC_APP_SEC_ACP_PAIRING;
        printf("\nAllow pairing attempt from device %s (%04X:%02X:%06X/%X) [Y/N]: ",
               (CsrCharString *) instData->deviceName,
               instData->deviceAddr.nap,
               instData->deviceAddr.uap,
               instData->deviceAddr.lap,
               instData->cod);
    }
}

static void playScReadLocalOobDataCfm(ScAppInstData_t *instData)
{
    CsrBtScReadLocalOobDataCfm *prim = (CsrBtScReadLocalOobDataCfm *)instData->recvMsgP;

    printf("\nRead local OOB data with result: 0x%02X from supplier 0x%02X for local device %s (%04X:%02X:%06X)\n",
        prim->resultCode,
        prim->resultSupplier,
        (CsrCharString *) instData->deviceName,
        instData->deviceAddr.nap,
        instData->deviceAddr.uap,
        instData->deviceAddr.lap);
}

static void handleScReadLocalOobDataCfm(ScAppInstData_t *instData)
{
    CsrBtScReadLocalOobDataCfm *prim = (CsrBtScReadLocalOobDataCfm *)instData->recvMsgP;

    if (prim->resultCode == CSR_BT_RESULT_CODE_SC_SUCCESS &&
        prim->resultSupplier == CSR_BT_SUPPLIER_SC)
    {
        CsrPmemFree(instData->oobHashCLocal);
        CsrPmemFree(instData->oobRandRLocal);
        instData->oobHashCLocal = (CsrUint8 *)prim->oobHashC192;
        instData->oobRandRLocal = (CsrUint8 *)prim->oobRandR192;
        prim->oobHashC192 = NULL;
        prim->oobRandR192 = NULL;
        scAppMenuActive(instData, SC_APP_SEC_OOB_MENU, playOobData);
    }

    scAppMenuActive(instData, SC_APP_SEC_OOB_MENU, playScReadLocalOobDataCfm);
}

static void handleScDebugModeCfm(ScAppInstData_t *instData)
{
    CsrBtScDebugModeCfm *prim;
    prim = (CsrBtScDebugModeCfm *)instData->recvMsgP;

    instData->enableSspDebugMode = prim->enabled;
    printf("\nSSP debug mode changed to: %s with result 0x%02X (supplier 0x%02X)\n",
           (char*)(prim->enabled ? "ON" : "OFF"),
           prim->resultCode,
           prim->resultSupplier);

    instData->state = SC_APP_MAIN_MENU;
}

#endif

#ifdef CSR_BT_LE_ENABLE
static void handleScLeSecurityInd(ScAppInstData_t *instData)
{
    const char *at[] = { "public", "private" };
    CsrBtScLeSecurityInd *ind;
    ind = (CsrBtScLeSecurityInd*)instData->recvMsgP;

    scAppRestoreIfNeeded(instData);

    printf("CsrBtScLeSecurityInd, %04x:%02x:%06x(%s), result 0x%04x, supplier 0x%04x (%sadded to db)\n",
           ind->address.addr.nap,
           ind->address.addr.uap,
           ind->address.addr.lap,
           at[ind->address.type],
           ind->resultCode,
           ind->resultSupplier,
           ind->addedToScDbList ? "":"Not ");
}

static void handleScOobDataExpiryInd(ScAppInstData_t *instData)
{
    printf("OOB data expired\n");
}
#endif /* CSR_BT_LE_ENABLE */

void handleScPrim(ScAppInstData_t *instData)
{
    CsrPrim    *primType;
    instData->recvMsgP = instData->fnMsg(instData->pMsgData);
    primType = (CsrPrim *)instData->recvMsgP;

    switch (*primType)
    {
        case CSR_BT_SC_AUTHORISE_EXT_IND:
        case CSR_BT_SC_AUTHORISE_IND:
            handleScAuthoriseInd(instData);
            break;
        case CSR_BT_SC_PASSKEY_IND:
            handleScPasskeyInd(instData);
            break;
#if CSR_BT_BT_VERSION >= BLUETOOTH_VERSION_2P1
        case CSR_BT_SC_SSP_JUSTWORKS_IND:
        case CSR_BT_SC_SSP_PASSKEY_IND:
        case CSR_BT_SC_SSP_COMPARE_IND:
        case CSR_BT_SC_SSP_NOTIFICATION_IND:
            handleScSspPassPrims(instData);
            break;
        case CSR_BT_SC_SSP_KEYPRESS_NOTIFICATION_IND:
            handleScSspKeypressNotificationInd(instData);
            break;
        case CSR_BT_SC_SSP_PAIRING_IND:
            handleScSspPairingInd(instData);
            break;
        case CSR_BT_SC_REBOND_IND:
            handleScRebondInd(instData);
            break;
        case CSR_BT_SC_READ_LOCAL_OOB_DATA_CFM:
            handleScReadLocalOobDataCfm(instData);
            break;
        case CSR_BT_SC_DEBUG_MODE_CFM:
            handleScDebugModeCfm(instData);
            break;
#endif
        case CSR_BT_SC_BOND_CFM:
            handleScBondCfm(instData);
            break;
        case CSR_BT_SC_BOND_IND:
            handleScBondInd(instData);
            break;
        case CSR_BT_SC_SET_SECURITY_MODE_CFM:
            handleSetSecurityModeCfm(instData);
            break;
        case CSR_BT_SC_DEBOND_CFM:
            handleScDebondCfm(instData);
            break;
        case CSR_BT_SC_MODE_IND:
            handleScBondableInd(instData);
            break;
        case CSR_BT_SC_ENCRYPTION_CFM:
            handleScEncryptCfm(instData);
            break;
        case CSR_BT_SC_AUTHENTICATE_CFM:
            handleScAuthenticateCfm(instData);
            break;
#ifdef CSR_BT_LE_ENABLE
        case CSR_BT_SC_LE_SECURITY_IND:
            handleScLeSecurityInd(instData);
            break;
        case CSR_BT_SC_LOCAL_OOB_DATA_EXPIRED_IND:
            handleScOobDataExpiryInd(instData);
            break;
        case CSR_BT_SC_LE_ADDRESS_MAPPED_IND:
            break;
#endif
        default:
            printf("[%s:%d] Unhandled event in state: %dX\n", __FILE__, __LINE__, instData->state);
            break;
    }

    CsrBtScFreeUpstreamMessageContents(CSR_BT_SC_PRIM, instData->recvMsgP);
    CsrPmemFree(instData->recvMsgP);
}

void handleScSecLevelPrim(ScAppInstData_t *instData)
{
    ScAppDemoSecurityCfm    *prim;

    instData->recvMsgP = instData->fnMsg(instData->pMsgData);

    prim = (ScAppDemoSecurityCfm *)instData->recvMsgP;

    printf("\nSecurity level set with result: 0x%04x (supplier 0x%04x)\n",
        prim->resultCode, prim->resultSupplier);
}

void scAppInitInstanceData(ScAppInstData_t *instData)
{
    int i;

    CsrMemSet(instData, 0, sizeof(ScAppInstData_t));

    instData->ioCapability       = CSR_BT_DEFAULT_IO_CAPABILITY;
    instData->securityMode       = CSR_BT_DEFAULT_SEC_MODE;
    instData->newSecurityMode    = instData->securityMode;
    instData->bondable           = CSR_BT_DEFAULT_SC_MODE;
    instData->enableSspDebugMode = FALSE;
    instData->authRequirements   = CSR_BT_DEFAULT_AUTH_REQUIREMENTS;
    instData->eventMask          = CSR_BT_SC_EVENT_MASK_REBOND;
    instData->encrypted          = FALSE;
    instData->state              = SC_APP_NA;
    instData->secLevelIdx        = 0;
    instData->secLevelCnt        = 0;
    instData->fnInit             = NULL;
    instData->fnFini             = NULL;
    instData->fnMsg              = NULL;
    instData->fnSend             = NULL;
    instData->minPasskeyLength   = 1;
    instData->proximityPairing   = FALSE;

    /* TODO: Use the in init */
    instData->autoAcceptPairingAttempts = FALSE;

    instData->addressType = CSR_BT_ADDR_PUBLIC;
    instData->deviceAddr = defGlobalBdAddr;

    for (i=0; i < SC_APP_MAX_CONNECTIONS; ++i)
    {
        instData->secLevelData[i].profileName      = "NA";
        instData->secLevelData[i].securityIncoming = TRUE;
    }

    CsrBtScSetEventMaskReqSend(instData->eventMask);
    CsrBtScSetSecurityModeReqSend(TESTQUEUE, instData->securityMode);
}

void scAppDeinitInstanceData(ScAppInstData_t *instData)
{
    CsrPmemFree(instData->oobHashCLocal);
    CsrPmemFree(instData->oobRandRLocal);
}

void scAppSetBondingTypedAddr(ScAppInstData_t *instData,
                              CsrBtAddressType addressType,
                              CsrBtDeviceAddr deviceAddr)
{
    instData->deviceAddr = deviceAddr;
    instData->addressType = addressType;
}

void scAppSetBondingAddr(ScAppInstData_t *instData,
                         CsrBtDeviceAddr    deviceAddr)
{
    scAppSetBondingTypedAddr(instData, CSR_BT_ADDR_PUBLIC, deviceAddr);
}

void scAppSetFnInit(ScAppInstData_t *instData,
                    scAppInOutCallback     fnInit,
                    void                   *pInitData)
{
    instData->fnInit    = fnInit;
    instData->pInitData = pInitData;
}

void scAppSetFnFini(ScAppInstData_t *instData,
                    scAppInOutCallback      fnFini,
                    void                    *pFiniData)
{
    instData->fnFini    = fnFini;
    instData->pFiniData = pFiniData;
}

void scAppSetFnGetMessage(ScAppInstData_t *instData,
                          scAppGetMessage         fnMsg,
                          void                    *pMsgData)
{
    instData->fnMsg    = fnMsg;
    instData->pMsgData = pMsgData;
}

void scAppSetSecLevelSend(ScAppInstData_t *instData,
                          scAppInOutCallback     fnSend,
                          void                   *pSendData)

{
    instData->fnSend    = fnSend;
    instData->pSendData = pSendData;
}

void scAppSetSecLevelPrim(ScAppInstData_t *instData,
                          CsrUint8                secLevelIdx,
                          char                   *profileName,
                          CsrPrim           primType,
                          CsrUint16               ifaceQueue,
                          CsrPrim           msgSecInType,
                          CsrPrim           msgSecOutType)
{
    ScAppSecLvlInstData_t *secLevelInst;

    if (secLevelIdx < SC_APP_MAX_CONNECTIONS)
    {
        secLevelInst = &instData->secLevelData[secLevelIdx];

        instData->secLevelIdx          = secLevelIdx;
        secLevelInst->profileName      = profileName;
        secLevelInst->secPrimType      = primType;
        secLevelInst->ifaceQueue       = ifaceQueue;
        secLevelInst->msgSecInType     = msgSecInType;
        secLevelInst->msgSecOutType    = msgSecOutType;
        secLevelInst->req[0].appHandle = TESTQUEUE;
        secLevelInst->req[1].appHandle = TESTQUEUE;

        ++instData->secLevelCnt;

        /* Make sure we end up with the correct default direction on security levels */
        scAppSetNextInOutSecLevel(instData);
        scAppSetNextInOutSecLevel(instData);
    }
}

void scAppSetSecLevelCM(ScAppInstData_t *instData,
                                  CsrUint8 secLevelIdx,
                                  char    *profileName)
{
    scAppSetSecLevelPrim(instData,
                         secLevelIdx,
                         profileName,
                         0,  /* Don't care */
                         0,  /* Don't care */
                         0,  /* Don't care */
                         0); /* Don't care */
}

static CsrUint16 scAppGetSecOut(ScAppInstData_t *instData,
                                      CsrUint8 secLevelIdx)
{
    ScAppDemoSecurityReq *req;
    CsrUint8 index;
    CsrUint16 secLevel;

    index = instData->secLevelData[secLevelIdx].securityIncoming;
    req   = &instData->secLevelData[secLevelIdx].req[index];
    secLevel = req->secLevel|CSR_BT_SEC_SPECIFY;

    return secLevel;
}

static CsrUint16 scAppGetSecIn(ScAppInstData_t *instData,
                                    CsrUint8 secLevelIdx)
{
    ScAppDemoSecurityReq *req;
    CsrUint8 index;
    CsrUint16 secLevel;

    index = instData->secLevelData[secLevelIdx].securityIncoming;
    req   = &instData->secLevelData[secLevelIdx].req[index];
    secLevel = req->secLevel|CSR_BT_SEC_SPECIFY;

    return secLevel;
}

CsrUint16 scAppGetSecLevelCM(ScAppInstData_t *instData,
                                      CsrUint8 secLevelIdx,
                                      CsrBool securityIncoming)
{
    if (securityIncoming)
    {
        return scAppGetSecIn(instData, secLevelIdx);
    }
    else
    {
        return scAppGetSecOut(instData, secLevelIdx);
    }
}

void scAppSaveState(CsrUint8 *prevState, CsrUint8 *curState, CsrUint8 newState)
{
    *prevState = *curState;
    *curState = newState;
}

void scAppRestoreState(CsrUint8 *prevState, CsrUint8 *curState)
{
    CsrUint8 tmp = *curState;
    *curState = *prevState;
    *prevState = tmp;
}

void scAppSendProfileSecurityLevelReq(void *arg)
{
    ScAppInstData_t *instData;
    ScAppDemoSecurityReq *req;
    ScAppSecLvlInstData_t *secLevelInst;
    CsrUint8 idx;

    instData = (ScAppInstData_t *)arg;
    idx = instData->secLevelIdx % instData->secLevelCnt;
    secLevelInst = &instData->secLevelData[idx];

    req            = pnew(ScAppDemoSecurityReq);
    req->appHandle = secLevelInst->req[secLevelInst->securityIncoming].appHandle;
    req->secLevel   = secLevelInst->req[secLevelInst->securityIncoming].secLevel|CSR_BT_SEC_SPECIFY;
    req->type       = secLevelInst->securityIncoming
                        ? secLevelInst->msgSecInType
                        : secLevelInst->msgSecOutType;

    CsrSchedMessagePut(secLevelInst->ifaceQueue,
                secLevelInst->secPrimType,
                req);
}

void scAppSetMinimumPasskeylength(ScAppInstData_t *instData,
                                  CsrUint8          minPasskeyLength)
{
    if(minPasskeyLength  <= CSR_BT_PASSKEY_MAX_LEN)
    {
        instData->minPasskeyLength = minPasskeyLength;
    }
    else
    {
        printf("Note: The passkey can be no longer than %d, defaulting back to %d\n",
               CSR_BT_PASSKEY_MAX_LEN, instData->minPasskeyLength);
    }
}

void scAppSetProximityPairing(ScAppInstData_t   *instData,
                              CsrBool           proximityPairingOn)
{
    instData->proximityPairing = proximityPairingOn;

    if(proximityPairingOn)
    {
        printf("NOTE: SC is auto accepting incoming pairing requests due to proximity pairing\n");
    }
}
