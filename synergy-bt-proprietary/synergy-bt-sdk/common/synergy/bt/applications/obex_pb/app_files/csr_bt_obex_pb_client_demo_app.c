/******************************************************************************

Copyright (c) 2006-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csr_bt_platform.h"
#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_bt_profiles.h"
#include "csr_bt_sc_lib.h"
#include "csr_bt_cm_lib.h"
#include "csr_bt_sd_lib.h"
#include "csr_bt_pac_lib.h"
#include "csr_bt_pac_prim.h"
#include "csr_bt_obex.h"
#include "csr_bt_platform.h"
#include "csr_bt_demoapp.h"
#include "csr_bt_sc_demo_app.h"
#include "csr_sched_init.h"
#include "csr_unicode.h"


#define KEYB_CHECK_TIMEOUT              (100000)

#define MAX_DISCOVERY_RESULTS_IN_LIST   (10)
#define PAC_MAX_OBEX_PACKET_SIZE        (0x1f00)
#define PB_LIST_MAX                     (0xff)
#define PB_DEFAULT_FOLDER_NAME          "folder"
#define MAX_ENTRIES                     6
#define MAX_SEARCH                      50

typedef CsrUint8 appState;
#define IDLE_STATE              (appState) 0x00
#define SEARCHING_STATE         (appState) 0x01
#define CLOSING_SEARCH_STATE    (appState) 0x02
#define CONNECT_INIT_STATE      (appState) 0x03
#define CONNECT_STATE           (appState) 0x04
#define AUTHENTICATING_STATE    (appState) 0x05
#define CONNECTED_STATE         (appState) 0x06
#define BONDING_STATE           (appState) 0x07
#define LAST_STATE              (appState) 0x08


typedef CsrUint8 kbmState;
#define NORMAL_STATE                        (kbmState) 0x00
#define PASSKEY_STATE                       (kbmState) 0x01
#define SECURITY_KEYBOARD_STATE             (kbmState) 0x02
#define ADDRESS_STATE                       (kbmState) 0x03
#define FILTER_STATE                        (kbmState) 0x04
#define PB_SOURCE_STATE                     (kbmState) 0x05
#define PB_NAME_STATE                       (kbmState) 0x06
#define PB_ENTRIES_STATE                    (kbmState) 0x07
#define FOLDER_NAME_STATE                   (kbmState) 0x08
#define VLIST_NAME_STATE                    (kbmState) 0x09
#define VLIST_ORDER_STATE                   (kbmState) 0x0A
#define VLIST_SEARCH_ATT_STATE              (kbmState) 0x0B
#define VLIST_SEARCH_VAL_STATE              (kbmState) 0x0C
#define VLIST_ENTRIES_STATE                 (kbmState) 0x0D
#define VLIST_OFFSET_STATE                  (kbmState) 0x0E
#define VCARD_NAME_STATE                    (kbmState) 0x0F
#define LAST_KBM_STATE                      (kbmState) 0x10
#define VCARDSELECTOR_STATE                 (kbmState) 0x11
#define PB_RESETNEWMISSEDCALL_STATE         (kbmState) 0x12
#define PULL_PB_ENTRY_BY_UID                (kbmState) 0x13
#define PB_VCARD_SELECTOR_OPERATOR_STATE    (kbmState) 0x14
#define SRMP_STATE                          (kbmState) 0x015

#define UID_TYPE                            "X-BT-UID:"
#define UNIQUE_ID_LENGTH                    32
#define UID_TYPE_LEN                        (sizeof(UID_TYPE)-1) /* "X-BT-UID:" = 9 */
#define UID_LENGTH                          (UID_TYPE_LEN+UNIQUE_ID_LENGTH+1)/* +1 for \0 */

#define BD_ADDR_STRING_LEN                  15
#define VCARD_PROPERTY_LEN                  8

typedef struct appInstData{
    CsrSchedQid         CsrSchedQid;
    CsrUint16           event;
    void               *pRecvMsg;
    appState            state;
    kbmState            kbm;
    kbmState            prevKbm;
    kbmState            nextKbm;
    CsrUint32           nDevice;
    CsrUint32           nDeviceInList;
    CsrBool             bFullDeviceList;
    CsrBtDeviceAddr     bdAddrList[MAX_DISCOVERY_RESULTS_IN_LIST];
    CsrUint8            passkeyLength;
    CsrUint8            passkey[CSR_BT_PASSKEY_MAX_LEN];
    char                szBdAddr[BD_ADDR_STRING_LEN];
    CsrUint32           BdAddrLen;
    CsrBtPacSrcType     src;
    char                name[CSR_BT_MAX_PATH_LENGTH];
    CsrUint32           nNameLen;
    char                szEntries[MAX_ENTRIES];
    CsrUint32           nEntriesLen;
    char                path[CSR_BT_MAX_PATH_LENGTH];
    CsrUint32           nPathLen;
    CsrBtPacOrderType   order;
    CsrBtPacSearchAtt   searchAtt;
    char                search[MAX_SEARCH];
    CsrUint32           nSearchLen;
    CsrUint16           nMaxLstCnt;
    char                szOffset[MAX_ENTRIES];
    CsrUint32           nOffsetLen;
    CsrFileHandle      *pFile;
    CsrUint32           nObjLen;
    ScAppInstData_t     scInstData;
    CsrUint8            filter[VCARD_PROPERTY_LEN];
    CsrUint8            vCardSelector[VCARD_PROPERTY_LEN];
    CsrUint8            resetNewMissedCall;
    CsrUint8            UIDchar[UID_LENGTH];
    CsrUint8            UIDcharEntryIndex;
    CsrUint8            vCardSelectorOperator;
    CsrBool             srmp;
    void                (*srmpHandler)(void *);
} appInstData;

static void playIntro(appInstData *pInst);
static void KeyMessageService(CsrUint16 pInst, void *data);

static void handleSdPrim(appInstData *pInst);

static void handlePacPrim(appInstData *pInst);
static void handleCsrBtPacConnectCfm(appInstData *pInst);
static void handlePacAuthInd(appInstData *pInst);
static void handleCsrBtPacPullPbCfm(appInstData *pInst);
static void handleCsrBtPacPullPbInd(appInstData *pInst);
static void handleSetFolderCfm(appInstData *pInst);
static void handlePullvCardListCfm(appInstData *pInst);
static void handlePullvCardListInd(appInstData *pInst);
static void handlePullvCardEntryCfm(appInstData *pInst);
static void handlePullvCardEntryInd(appInstData *pInst);
static void handlePacAbort(appInstData *pInst);
static void handlePacDisconnect(appInstData *pInst);

static void startConnect(appInstData *pInst);
static void searchStart(appInstData *pInst);
static void pullPbStart(appInstData *pInst);
static void setFolderStart(appInstData *pInst);
static void pullPbListStart(appInstData *pInst);
static void pullPbEntryStart(appInstData *pInst);
static void abortStart(appInstData *pInst);
static void disconnectPacStart(appInstData *pInst);

static void handleKeybPrim(appInstData *pInst);
static CsrBool passkeyState(appInstData *pInst, CsrUint8 cKey);
static CsrBool addressState(appInstData *pInst, CsrUint8 cKey);
static CsrBool idleState(appInstData *pInst, CsrUint8 cKey);
static CsrBool connectState(appInstData *pInst, CsrUint8 cKey);
static CsrBool connectedState(appInstData *pInst, CsrUint8 cKey);
static CsrBool searchState(appInstData *pInst, CsrUint8 cKey);

static CsrBool filterState(appInstData *pInst, CsrUint8 cKey);
static void filterMenu(void);
static CsrBool vCardSelectorState(appInstData *pInst, CsrUint8 cKey);

static char *getPathStart(char *path);


static void pbScAppSetFnFini(void *pFiniData)
{
    appInstData *instData = (appInstData *) pFiniData;
    scAppRestoreState((CsrUint8 *) &instData->prevKbm, (CsrUint8 *) &instData->kbm);
    playIntro(instData);
}

static void pbScAppSetFnInit(void *pInitData)
{
    appInstData *instData = (appInstData *) pInitData;
    scAppSaveState((CsrUint8 *) &instData->prevKbm, (CsrUint8 *) &instData->kbm, SECURITY_KEYBOARD_STATE);
}

static void* pbScAppSetFnGetMessage(void *pRecvMsgPData)
{
    appInstData *instData = (appInstData *) pRecvMsgPData;
    void *result          = instData->pRecvMsg;
    instData->pRecvMsg    = NULL;
    return result;
}

/*******************************************************************************
 *
 *   init function called by the scheduler upon initialisation. This
 *   function is used to boot the demo application by sending a
 *   request to bond with the headset. Bonding is not mandated
 *   according to the profile but is needed if encryption of the
 *   speech is required.
 *
 *******************************************************************************/
void CsrBtAppInit(void **gash)
{
    appInstData  *pInst;
    extern BD_ADDR_T defGlobalBdAddr;
    CsrBtDeviceAddr zeroAddr;

    *gash = CsrPmemAlloc(sizeof(appInstData));
    pInst = (appInstData*)*gash;
    pInst->state = IDLE_STATE;
    pInst->nDevice = 0;
    pInst->nDeviceInList = 0;
    pInst->bFullDeviceList = FALSE;
    pInst->pRecvMsg = NULL;
    pInst->pFile = NULL;
    pInst->nObjLen = 0;

    /* Authentication */
    pInst->kbm = NORMAL_STATE;
    pInst->szBdAddr[0] = 0;
    pInst->BdAddrLen = 0;
    pInst->passkey[0] = '\0';
    pInst->passkeyLength = 0;

    /* Default filter gets everything */
    pInst->filter[0] = 0xff;
    pInst->filter[1] = 0xff;
    pInst->filter[2] = 0xff;

    /* bit29 - X-BT-SPEEDDIALKEY, bit -30 - Logical operation of filtering */
    pInst->filter[3] = 0xff;
    pInst->filter[4] = 0x00;
    pInst->filter[5] = 0x00;
    pInst->filter[6] = 0x00;
    pInst->filter[7] = 0x00;

    /* Default vCardSelector value gets everything */
    pInst->vCardSelector[0] = 0x00;
    pInst->vCardSelector[1] = 0x00;
    pInst->vCardSelector[2] = 0x00;
    pInst->vCardSelector[3] = 0x00;
    pInst->vCardSelector[4] = 0x00;
    pInst->vCardSelector[5] = 0x00;
    pInst->vCardSelector[6] = 0x00;
    pInst->vCardSelector[7] = 0x00;
    /* If set to 1, the remote server shall reset the missed call */
    pInst->resetNewMissedCall = 0x00;
    /* pull pb */
    pInst->src = CSR_BT_PB_REPO_PHONE;
    pInst->nNameLen = 0;
    CsrBtBdAddrZero(&zeroAddr);
    if (!CsrBtBdAddrEq(&defGlobalBdAddr, &zeroAddr))
    {
        pInst->bdAddrList[pInst->nDevice].lap = defGlobalBdAddr.lap;
        pInst->bdAddrList[pInst->nDevice].nap = defGlobalBdAddr.nap;
        pInst->bdAddrList[pInst->nDevice].uap = defGlobalBdAddr.uap;
    }
    else
    {
        pInst->bdAddrList[pInst->nDevice].nap = 0;
        pInst->bdAddrList[pInst->nDevice].uap = 0;
        pInst->bdAddrList[pInst->nDevice].lap = 0;
    }

    playIntro(pInst);

    /* Support for usr2usr/usr2kernel splits */
    pInst->CsrSchedQid = TESTQUEUE;

    /* Security App Configuration */
    CsrBtScActivateReqSend(pInst->CsrSchedQid);
    scAppInitInstanceData(&pInst->scInstData);

    scAppSetFnFini(&pInst->scInstData, pbScAppSetFnFini, pInst);
    scAppSetFnInit(&pInst->scInstData, pbScAppSetFnInit, pInst);
    scAppSetSecLevelSend(&pInst->scInstData, scAppSendProfileSecurityLevelReq, &pInst->scInstData);

    scAppSetFnGetMessage(&pInst->scInstData, pbScAppSetFnGetMessage, pInst);

    scAppSetSecLevelPrim(&pInst->scInstData,
                         0,     /* Index 0. Single profile */
                         "PAC", CSR_BT_PAC_PRIM,
                         CSR_BT_PAC_IFACEQUEUE,
                         SC_APP_SET_SEC_LEVEL_NA, CSR_BT_PAC_SECURITY_OUT_REQ);

    /* [QTI] Add to set local name. */
#ifdef CSR_USE_MTP9X45
    CsrBtCmSetLocalNameReqSend(CsrSchedTaskQueueGet(), CsrStrDup("mdm9640"));
#endif

    CsrSchedTimerSet(KEYB_CHECK_TIMEOUT, KeyMessageService, 0, NULL);
}

#ifdef ENABLE_SHUTDOWN
void CsrBtAppDeinit(void **gash)
{
    appInstData *pInst = *gash;

    scAppDeinitInstanceData(&pInst->scInstData);

    /*  Get a message from the demoapplication message queue. The
     *  message is returned in prim and the event type in eventType */
    while(CsrSchedMessageGet(&pInst->event, &pInst->pRecvMsg) )
    {
        switch(pInst->event)
        {
            case CSR_BT_PAC_PRIM:
            {
                CsrBtPacFreeUpstreamMessageContents(pInst->event, pInst->pRecvMsg);
                break;
            }
            case CSR_BT_SC_PRIM:
            {
                CsrBtScFreeUpstreamMessageContents(pInst->event, pInst->pRecvMsg);
                break;
            }
            case CSR_BT_SD_PRIM:
            {
                CsrBtSdFreeUpstreamMessageContents(pInst->event, pInst->pRecvMsg);
                break;
            }
            default:
            { /* Just ignore    */
                break;
            }
        }
        CsrPmemFree(pInst->pRecvMsg);
    }
    CsrPmemFree(pInst);
}
#endif

/**************************************************************************************************
 *
 *   this is the demo application handler function. All primitives sent to the demo application will
 *   be received in here. The scheduler ensures that the function is activated when a signal is put
 *   on the demo application signal queue.
 *
 **************************************************************************************************/
void CsrBtAppHandler(void **gash)
{
    appInstData *pInst = *gash;

    /*  get a message from the demoapplication message queue. The message is
        returned in prim and the event type in eventType */
    if(CsrSchedMessageGet(&pInst->event, &pInst->pRecvMsg))
    {
        /*  two event types must be handled. SC for bonding and PAC for the
            connection and PB related signalling */
        switch (pInst->event)
        {
            case CSR_BT_SC_PRIM:
                handleScPrim(&pInst->scInstData);
                break;

            case CSR_BT_SD_PRIM:
                handleSdPrim(pInst);
                break;

            case CSR_BT_PAC_PRIM:
                {
                    CsrPrim *prim = (CsrPrim *) pInst->pRecvMsg;
                    switch(*prim)
                    {
                        case CSR_BT_PAC_SECURITY_OUT_CFM:
                            handleScSecLevelPrim(&pInst->scInstData);
                            break;

                        default:
                            handlePacPrim(pInst);
                            break;
                    }
                    break;
                }
            case KEY_MESSAGE:
                handleKeybPrim(pInst);
                break;

            case CSR_BT_CM_PRIM:
                {
                    CsrPrim event = *((CsrPrim *) pInst->pRecvMsg);
                    switch (event)
                    {
                        case CSR_BT_CM_SET_LOCAL_NAME_CFM:
                            printf("<%s> set local name done\n", __FUNCTION__);
                            break;

                        case CSR_BT_CM_WRITE_SCAN_ENABLE_CFM:
                            printf("<%s> write scan enable done\n", __FUNCTION__);
                            break;         

                        default:
                            printf("<%s> un-handled cm prim event 0x%04x\n", __FUNCTION__, event);
                            break;
                    }
                    break;
                }

            default:
                /*  unexpected primitive received */
                printf("####### default in testhandler %x,\n", pInst->event);
                break;
        }
        if (NULL != pInst->pRecvMsg)
        {
            /*  free the received signal again. If the signal for some reason must be stored in the application
                the pointer (prim) must be set to NULL in order not to free it here */
            CsrPmemFree(pInst->pRecvMsg);
            pInst->pRecvMsg = NULL;
        }
    }
}

static void playMenuOptions(appInstData* pInst)
{
    printf("\nOptions:\n\n");
    if (pInst->state == IDLE_STATE)
    {
        printf("  s) Search for Phonebook Access Server\n");
        printf("  c) Connect to server\n");
        printf("  t) Security Controller Configuration\n");

        printf("  b) Enter Bluetooth address\n");
    }
    else if (pInst->state == CONNECT_STATE)
    {
        printf("  d) Cancel Connection attempt (stop scanning)\n");
    }
    else
    {
        printf("  p) download phonebook\n");
        printf("  s) set phonebook folder\n");
        printf("  l) download phonebook listing\n");
        printf("  e) download phonebook entry\n");
        printf("  u) download phonebook entry by UID\n");

        printf("  a) Abort operation\n");
        printf("  d) Disconnect (close connection)\n");
        printf("  f) Enter vcard filter\n");
        printf("    g) Clear vcard filter (clear all bits)\n");
        printf("    h) Set all non-reserved vcard filter bits\n");
        printf("  v) Enter vcard selector attributes\n");
        printf("    c) Clear vcard selector attributes (clear all bits)\n");
        printf("    x) Set all non-reserved vcard selector bits\n\n");
    }
}

/* == LOCAL FUNCTIONS =============================================================================== */
static void playIntro(appInstData *pInst)
{
    system_cls();
    printf("\nCSR plc. -- Synergy BT Phonebook Access Client Demo\n\n");
    printf("Hit <ESC> to quit program!\n\n");

    playMenuOptions(pInst);
}

static void KeyMessageService(CsrUint16 m, void *data)
{
    CSR_UNUSED(m);
    CSR_UNUSED(data);
    if (_kbhit())
    {
        CsrBtKeyPress_t *key;
        key = CsrPmemAlloc(sizeof(CsrBtKeyPress_t));
        key->type = KEY_MESSAGE;
        key->key =_getch();
        CsrSchedMessagePut(TESTQUEUE, KEY_MESSAGE, key);
    }
    CsrSchedTimerSet(KEYB_CHECK_TIMEOUT, KeyMessageService, 0, NULL);
}

/*************************  SD PRIMITIVES   ***************************/
/*******************************************************************************
 *
 *   function to handle all sd-primitives.
 *
 *******************************************************************************/

static void handleSdPrim(appInstData *pInst)
{
    CsrPrim *pMsg = pInst->pRecvMsg;

    switch (*pMsg)
    {
        case CSR_BT_SD_SEARCH_RESULT_IND:
            {
                CsrBtSdSearchResultInd *prim = (CsrBtSdSearchResultInd *) pInst->pRecvMsg;
                CsrUint16 nameLen;
                CsrUint8 *name;

                pInst->bdAddrList[pInst->nDeviceInList] = prim->deviceAddr;

                printf("\n%u: ", pInst->nDeviceInList);
                nameLen = CsrBtSdDecodeFriendlyName(prim->info, prim->infoLen, &name);
                if (nameLen > 0)
                {
                    printf("%s\n   ", name);
                    CsrPmemFree(name);
                }
                printf("%04X:%02X:%06X\n   ",
                    prim->deviceAddr.nap,
                    prim->deviceAddr.uap,
                    prim->deviceAddr.lap);
                printf("%s\n",CodDecodeDeviceClass(prim->deviceClass));

                pInst->nDeviceInList++;
                if ( pInst->nDeviceInList > MAX_DISCOVERY_RESULTS_IN_LIST - 1)
                {
                    /* Reached maximum result list entries. Cancelling search */
                    CsrBtSdCancelSearchReqSend(pInst->CsrSchedQid);
                    pInst->nDeviceInList = MAX_DISCOVERY_RESULTS_IN_LIST - 1;
                    pInst->bFullDeviceList = TRUE;
                }
                CsrPmemFree(prim->info);
                break;
            }
        case CSR_BT_SD_CLOSE_SEARCH_IND:
            {
                printf("\nClosed Search\n");
                if(!pInst->bFullDeviceList)
                {
                    printf(
                        "Selecting device with addr: %04X:%02X:%06X\n",
                        pInst->bdAddrList[pInst->nDevice].nap, pInst->bdAddrList[pInst->nDevice].uap,
                        pInst->bdAddrList[pInst->nDevice].lap
                        );
                    scAppSetBondingAddr(&pInst->scInstData,
                                        pInst->bdAddrList[pInst->nDevice]);
                    pInst->state = IDLE_STATE;
                }
                break;
            }
        default:
            {
                printf("####### Unhandled sdPrim: %x received\n", *pMsg);
                CsrBtSdFreeUpstreamMessageContents(CSR_BT_SD_PRIM, pInst->pRecvMsg);
                break;
            }
    }
}

/*************************  PAC PRIMITIVES  ***************************/
static void handleCsrBtPacConnectCfm(appInstData *pInst)
{
    CsrBtPacConnectCfm *pMsg = pInst->pRecvMsg;
    if ((CONNECT_STATE == pInst->state || AUTHENTICATING_STATE == pInst->state)
        && pMsg->resultCode == CSR_BT_OBEX_SUCCESS_RESPONSE_CODE
        && pMsg->resultSupplier == CSR_BT_SUPPLIER_IRDA_OBEX)
    {
        pInst->state = CONNECTED_STATE;
        playIntro(pInst);

        printf("\tRepositories supported by server:\t");
        if (pMsg->supportedRepositories & CSR_BT_PB_REPO_PHONE)
        {
            printf(" telecom,");
        }
        if (pMsg->supportedRepositories & CSR_BT_PB_REPO_SIM)
        {
            printf(" SIM1,");
        }

        printf("\n\tFolders supported by server:\t");
        if (pMsg->supportedRepositories & CSR_BT_PB_REPO_SPD)
        {
            printf(" Speed dial contacts,");
        }
        if (pMsg->supportedRepositories & CSR_BT_PB_REPO_FAV)
        {
            printf(" Favourite contacts,");
        }

        printf("\n\tFeatures supported by server:\t");
        if (pMsg->supportedFeatures & CSR_BT_PB_FEATURE_DOWNLOAD)
        {
            printf(" Phonebook downloading,");
        }
        if (pMsg->supportedFeatures & CSR_BT_PB_FEATURE_BROWSING)
        {
            printf(" Phonebook browsing,");
        }
        if (pMsg->supportedFeatures & CSR_BT_PB_FEATURE_VCARD_SEL)
        {
            printf(" vCard selection,");
        }
        if (pMsg->supportedFeatures & CSR_BT_PB_FEATURE_MISSED_CALL)
        {
            printf(" Enhanced missed call,");
        }
        if (pMsg->supportedFeatures & CSR_BT_PB_FEATURE_DATABASE_ID)
        {
            printf(" Database identifier,");
        }
        if (pMsg->supportedFeatures & CSR_BT_PB_FEATURE_FOLDER_VER)
        {
            printf(" Folder version,");
        }
        if (pMsg->supportedFeatures & CSR_BT_PB_FEATURE_UCI)
        {
            printf(" Contact identifier,");
        }
        if (pMsg->supportedFeatures & CSR_BT_PB_FEATURE_UID)
        {
            printf(" Unique ID,");
        }
        if (pMsg->supportedFeatures & CSR_BT_PB_FEATURE_CONTACT_REF)
        {
            printf(" Contact referencing,");
        }
        if (pMsg->supportedFeatures & CSR_BT_PB_FEATURE_CONTACT_IMG)
        {
            printf(" Contact image");
        }
        printf("\n");
    }
    else
    {
        pInst->state = IDLE_STATE;

        playIntro(pInst);

        printf("Connection failed, resultCode: 0x%04X, resultSupplier: 0x%04X\n",
               pMsg->resultCode,
               pMsg->resultSupplier);
        pInst->passkeyLength = 0;
    }
}

static void handlePacAuthInd(appInstData *pInst)
{
    CsrBtPacAuthenticateInd *pMsg = (CsrBtPacAuthenticateInd *) pInst->pRecvMsg;
    CsrBtDeviceAddr slaveDeviceAddr = pInst->bdAddrList[pInst->nDevice];

    printf("Enter OBEX password for \"%04X:%02X:%06X\". Realm = %s\n", slaveDeviceAddr.nap,
           slaveDeviceAddr.uap,
           slaveDeviceAddr.lap,
           pMsg->realm);
    pInst->state = AUTHENTICATING_STATE;
    pInst->kbm = PASSKEY_STATE;
    pInst->passkeyLength = 0;
    pInst->passkey[0] = 0;
    CsrPmemFree(pMsg->realm);
}

static void printVersionInfo(CsrBtPbVersionInfo *versionInfo)
{
    CsrUint8 index;

    printf("\tPrimary Version Counter: 0x");
    for (index = 0; index < 16; index++)
    {
        printf("%x%x", versionInfo->primaryVersionCounter[15-index] >> 4, versionInfo->primaryVersionCounter[15-index] & 0x0f);
    }
    printf("\n\tSecondary Version Counter: 0x");
    for (index = 0; index < 16; index++)
    {
        printf("%x%x", versionInfo->secondaryVersionCounter[15-index] >> 4, versionInfo->secondaryVersionCounter[15-index] & 0x0f);
    }
    printf("\n\tDatabase Identifier: 0x");
    for (index = 0; index < 16; index++)
    {
        printf("%x%x", versionInfo->databaseIdentifier[15-index] >> 4, versionInfo->databaseIdentifier[15-index] & 0x0f);
    }
    printf("\n");
}

static void handleCsrBtPacPullPbCfm(appInstData *pInst)
{
    if (CONNECTED_STATE == pInst->state)
    {
        CsrBtPacPullPbCfm *pMsg = pInst->pRecvMsg;

        if (CSR_BT_OBEX_SUCCESS_RESPONSE_CODE == pMsg->responseCode)
        {
            if (NULL == pInst->pFile)
            {
                /* empty pb downloaded successful, we create file anyway */
                pInst->pFile = CsrBtFopen(pInst->name, "wb");
            }
            printf("\nPhonebook download complete!\n");
            printf("\tSize of Pb: %d entries (NB: always 0 if CSRMAX entries was different from 0)\n", pMsg->pbSize);
            printf("\tNumber of missed calls: %u\n", pMsg->newMissedCall);
            printf("\tNumber of bytes downloaded: %d\n", pInst->nObjLen);
            printf("\tDownloaded phonebook is stored in file: %s\n", pInst->name);
            printVersionInfo(&pMsg->versionInfo);
        }
        else
        {
            printf("Error occurred. OBEX response code: 0x%04x\n", pMsg->responseCode);
        }
        /*playMenuOptions(pInst);*/
    }
    if (NULL != pInst->pFile)
    {
        CsrBtFclose(pInst->pFile);
        pInst->pFile = NULL;
    }
}

static void getSrmp(appInstData *pInst, void (*func)(void *opaque))
{
    if(pInst->srmp != FALSE)
    {
        printf("\nPress (s) to suspend SRM. Press any other key to activate SRM:\t");
        pInst->kbm = SRMP_STATE;
        pInst->srmpHandler = func;
    }
    else
    {
        func(pInst);
    }
}

static void pullPbResSrmpHandler(void *opaque)
{
    appInstData *pInst = (appInstData *)opaque;
    pInst->kbm = NORMAL_STATE;
    CsrBtPacPullPbResSend(pInst->srmp);
}

static void handleCsrBtPacPullPbInd(appInstData *pInst)
{
    CsrBtPacPullPbInd *pMsg = pInst->pRecvMsg;

    if (CONNECTED_STATE == pInst->state)
    {
        CsrUint32 nWrite;

        if (NULL == pInst->pFile)
        {
            /* first indication, create file */
            pInst->pFile = CsrBtFopen(pInst->name, "wb");
        }

        nWrite = CsrBtFwrite(pMsg->bodyOffset + pMsg->payload,
                             sizeof(CsrUint8),
                             pMsg->bodyLength,
                             pInst->pFile);
        if (nWrite != pMsg->bodyLength)
        {
            /* file error */
            printf("File error, could not write to file!\n");
        }

        pInst->nObjLen += pMsg->bodyLength;
        printf("%d Bytes downloaded, total downloaded: %d Bytes\n",
               pMsg->bodyLength,
               pInst->nObjLen);

        getSrmp(pInst, pullPbResSrmpHandler);
    }
    if (NULL != pMsg->payload)
    {
        CsrPmemFree(pMsg->payload);
        pMsg->payload = NULL;
    }
}

static void handleSetFolderCfm(appInstData *pInst)
{
    CsrBtPacSetFolderCfm *pMsg = pInst->pRecvMsg;

    if (CONNECTED_STATE == pInst->state)
    {
        if (CSR_BT_OBEX_SUCCESS_RESPONSE_CODE == pMsg->responseCode)
        {
            printf("Folder set successful!\n\n");
        }
        else
        {
            printf("Error occurred. Obex response code: 0x%04X\n\n", pMsg->responseCode);
        }
    }
}

static void handlePullvCardListCfm(appInstData *pInst)
{
    CsrBtPacPullVcardListCfm *pMsg = pInst->pRecvMsg;

    if (CONNECTED_STATE == pInst->state)
    {
        if (CSR_BT_OBEX_SUCCESS_RESPONSE_CODE == pMsg->responseCode)
        {
            printf("\nV-card listing downloaded complete\n");
            printf("Phonebook size: %u\n", pMsg->pbSize);
            printf("New missed calls: %u\n", pMsg->newMissedCall);
            printf("Number of bytes downloaded: %d\n", pInst->nObjLen);
            printf("Listing stored in file: %s\n", pInst->name);
            printVersionInfo(&pMsg->versionInfo);
        }
        else
        {
            printf("Error occurred. Obex response code: 0x%04X\n\n", pMsg->responseCode);
        }
    }
    if (NULL != pInst->pFile)
    {
        CsrBtFclose(pInst->pFile);
        pInst->pFile = NULL;
    }
}

static void listResSrmpHandler(void *opaque)
{
    appInstData *pInst = (appInstData *)opaque;
    pInst->kbm = NORMAL_STATE;
    CsrBtPacPullVcardListResSend(pInst->srmp);
}

static void handlePullvCardListInd(appInstData *pInst)
{
    CsrBtPacPullVcardListInd *pMsg = pInst->pRecvMsg;

    if (CONNECTED_STATE == pInst->state)
    {
        CsrUint32 nWrite;

        /* first indication, create file */
        if (NULL == pInst->pFile)
        {
            const char *ext = ".vcf";
            const char *dflt = PB_DEFAULT_FOLDER_NAME;

            /* ensure that the name do not include a path.
             * No path must be include since PullvCardEntry uses releatib path information.
             * But just in any case, we still check and do only use the name is a path is inlcuded. */

            CsrStrLCpy(pInst->name, getPathStart(pInst->name), CSR_BT_MAX_PATH_LENGTH);

            if ('.' == pInst->name[0] || 0 == pInst->name[0])
            {
                /* create a valid file name */
                CsrStrLCpy(pInst->name, dflt, CSR_BT_MAX_PATH_LENGTH);
            }

            CsrStrLCat(pInst->name, ext, CSR_BT_MAX_PATH_LENGTH);

            pInst->pFile = CsrBtFopen(pInst->name, "wb");

            if (NULL == pInst->pFile)
            {
                printf("File error, could not open the file named: %s\n",
                       pInst->name);

                if (NULL != pMsg->payload)
                {
                    CsrPmemFree(pMsg->payload);
                    pMsg->payload = NULL;
                }
                return;
            }
        }

        nWrite = CsrBtFwrite(pMsg->bodyOffset + pMsg->payload,
                             sizeof(CsrUint8),
                             pMsg->bodyLength,
                             pInst->pFile);

        if (nWrite != pMsg->bodyLength)
        {
            printf("File error, could not write to file!\n");
        }
        pInst->nObjLen += pMsg->bodyLength;
        getSrmp(pInst, listResSrmpHandler);
    }
    if (NULL != pMsg->payload)
    {
        CsrPmemFree(pMsg->payload);
        pMsg->payload = NULL;
    }
}

static void handlePullvCardEntryCfm(appInstData *pInst)
{
    CsrBtPacPullVcardEntryCfm *pMsg = pInst->pRecvMsg;
    CsrUint8 index;

    if (CONNECTED_STATE == pInst->state)
    {
        if (CSR_BT_OBEX_SUCCESS_RESPONSE_CODE == pMsg->responseCode)
        {
            printf("V-Card Entry downloaded complete\n");
            printf("Number of bytes downloaded: %d\n", pInst->nObjLen);
            printf("Downloaded entry is stored in file: %s\n",pInst->name);
            printf("Database Identifier: 0x");
            for (index = 0; index < 16; index++)
            {
                printf("%x%x", pMsg->databaseId[15-index] >> 4, pMsg->databaseId[15-index] & 0x0f);
            }
        }
        else
        {
            printf("Error occurred. Obex response code: 0x%04X\n\n", pMsg->responseCode);
        }
        /*playMenuOptions(pInst);*/
    }
    if (NULL != pInst->pFile)
    {
        CsrBtFclose(pInst->pFile);
        pInst->pFile = NULL;
    }
}

static void handlePullvCardEntryInd(appInstData *pInst)
{
    CsrBtPacPullVcardEntryInd *pMsg = pInst->pRecvMsg;

    if (CONNECTED_STATE == pInst->state)
    {
        CsrUint32 nWrite;

        /* first indication, create file */
        if (NULL == pInst->pFile)
        {
            CsrStrLCpy(pInst->name, getPathStart(pInst->name), CSR_BT_MAX_PATH_LENGTH);

            pInst->pFile = CsrBtFopen(pInst->name, "wb");
        }
        nWrite = CsrBtFwrite(
            pMsg->bodyOffset + pMsg->payload, sizeof(CsrUint8),
            pMsg->bodyLength, pInst->pFile
            );
        if (nWrite != pMsg->bodyLength)
        {   /* file error */
            printf("File error, could not write to file!\n");
        }
        pInst->nObjLen += pMsg->bodyLength;
        printf("%d Bytes downloaded, total downloaded: %d Bytes\n", pMsg->bodyLength, pInst->nObjLen);
        CsrBtPacPullVcardEntryResSend(pInst->srmp);
    }
    if (NULL != pMsg->payload)
    {
        CsrPmemFree(pMsg->payload);
        pMsg->payload = NULL;
    }
}

static void handlePacAbort(appInstData *pInst)
{
    printf("Server confirmed abort.\n\n");
    if (pInst->pFile)
    {
        CsrBtFclose(pInst->pFile);
        pInst->pFile = NULL;
    }
    playIntro(pInst);
}

static void handlePacDisconnect(appInstData *pInst)
{
    printf("Disconnected!\n\n");
    pInst->state = IDLE_STATE;
    playIntro(pInst);
    pInst->passkeyLength = 0;
    if (pInst->pFile)
    {
        CsrBtFclose(pInst->pFile);
        pInst->pFile = NULL;
    }
}

/*******************************************************************************
 *
 *   function to handle all pac-primitives.
 *
 *******************************************************************************/
static void handlePacPrim(appInstData *pInst)
{
    CsrBtPacPrim *pMsg = pInst->pRecvMsg;

    switch (*pMsg)
    {
        case CSR_BT_PAC_CONNECT_CFM:
            handleCsrBtPacConnectCfm(pInst);
            break;

        case CSR_BT_PAC_AUTHENTICATE_IND:
            handlePacAuthInd(pInst);
            break;

        case CSR_BT_PAC_PULL_PB_CFM:
            handleCsrBtPacPullPbCfm(pInst);
            break;

        case CSR_BT_PAC_PULL_PB_IND:
            handleCsrBtPacPullPbInd(pInst);
            break;

        case CSR_BT_PAC_SET_FOLDER_CFM:
            /* Fall through */
        case CSR_BT_PAC_SET_BACK_FOLDER_CFM:
            /* Fall through */
        case CSR_BT_PAC_SET_ROOT_FOLDER_CFM:
            handleSetFolderCfm(pInst);
            break;

        case CSR_BT_PAC_PULL_VCARD_LIST_CFM:
            handlePullvCardListCfm(pInst);
            break;

        case CSR_BT_PAC_PULL_VCARD_LIST_IND:
            handlePullvCardListInd(pInst);
            break;

        case CSR_BT_PAC_PULL_VCARD_ENTRY_CFM:
            handlePullvCardEntryCfm(pInst);
            break;

        case CSR_BT_PAC_PULL_VCARD_ENTRY_IND:
            handlePullvCardEntryInd(pInst);
            break;

        case CSR_BT_PAC_ABORT_CFM:
            handlePacAbort(pInst);
            break;

        case CSR_BT_PAC_DISCONNECT_IND:
            handlePacDisconnect(pInst);
            break;

        default:
            printf("####### Unhandled pacPrim: %x received\n", *pMsg);
            CsrBtPacFreeUpstreamMessageContents(CSR_BT_PAC_PRIM, pInst->pRecvMsg);
            break;
    }
}

/*************************  KBM PRIMITIVES  ***************************/

/*******************************************************************************
 *
 *   function to handle all keyboard presses (sent as a keyboard event signal).
 *
 *******************************************************************************/
static void searchStart(appInstData *pInst)
{
    /* Find CSRMAX MAX_DISCOVERY_RESULTS_IN_LIST devices */
    pInst->nDeviceInList = 0;
    pInst->bFullDeviceList = FALSE;

    CsrBtSdSearchReqSend(pInst->CsrSchedQid,
                  CSR_BT_SD_SEARCH_SHOW_UNKNOWN_DEVICE_NAMES | CSR_BT_SD_SEARCH_CONTINUE_AFTER_RSSI_SORTING, /* searchConfiguration */
                  6000, /* RSSI buffer timeout */
                  CSR_BT_INFINITE_TIME, /* Search timeout */
                  CSR_BT_SD_RSSI_THRESHOLD_DONT_CARE, /* RSSI threshold */
                  CSR_BT_OBJECT_TRANSFER_MAJOR_SERVICE_MASK, /* Class of Device */
                  CSR_BT_OBJECT_TRANSFER_MAJOR_SERVICE_MASK, /* Class of Device mask */
                  CSR_BT_SD_ACCESS_CODE_GIAC, /* Access code */
                  0, /* Filter length */
                  NULL); /* Filter */

    printf("\nSearch in progress. Please wait .....\n");
}

static void pullPbStart(appInstData *pInst)
{
    printf("\nPress (s) to select SIM1. Press any other key to select Phone:\t");
    pInst->kbm = PB_SOURCE_STATE;
    pInst->nObjLen = 0;
    if (NULL != pInst->pFile)
    {
        CsrBtFclose(pInst->pFile);
        pInst->pFile = NULL;
    }
}

static CsrBool pbSourceState(appInstData *pInst, CsrUint8 cKey)
{
    if (cKey == 's' || cKey == 'S')
    {
        pInst->src = CSR_BT_PB_REPO_SIM;
        printf("SIM1");
    }
    else
    {
        pInst->src = CSR_BT_PB_REPO_PHONE;
        printf("Phone");
    }
    pInst->kbm = PB_NAME_STATE;
    pInst->nNameLen = 0;
    CsrMemSet(pInst->name, 0, sizeof(pInst->name));
    printf("\nSelect phonebook to download:\n"
           "\t(p)\t\"pb.vcf\"  = main phone book (default if nothing is entered)\n"
           "\t(i)\t\"ich.vcf\" = incoming calls history\n"
           "\t(o)\t\"och.vcf\" = outgoing calls history\n"
           "\t(m)\t\"mch.vcf\" = missed calls history\n"
           "\t(c)\t\"cch.vcf\" = combined calls history\n"
           "\t(f)\t\"fav.vcf\" = favourite contacts\n"
           "\t(s)\t\"spd.vcf\" = speed dial contacts"
           ":\t");
    return (TRUE);
}

static CsrBool pbResetNewMissedCallState(appInstData *pInst, CsrUint8 cKey)
{
    pInst->resetNewMissedCall = 0x00;
    if (cKey == 'r' || cKey == 'R')
    {
        pInst->resetNewMissedCall = 0x01;
        printf("reset missed calls");
    }
    else
    {
        printf("Don't reset missed calls");
    }

    pInst->kbm = pInst->nextKbm;

    if (PB_ENTRIES_STATE == pInst->kbm)
    {
        pInst->nEntriesLen = 0;
        printf("\nEnter max. number of entries you want (0 - only returns the selected PBs size) (default: 10):\t");
    }
    else if (VLIST_OFFSET_STATE == pInst->kbm)
    {
        printf("\nEnter entry start offset (default: 0):\t");
        pInst->nOffsetLen = 0;
    }

    return TRUE;
}

static CsrBool pbVcardSelectorOperatorState(appInstData *pInst, CsrUint8 cKey)
{
    pInst->vCardSelectorOperator = 0x00;
    if (cKey == 'a' || cKey == 'A')
    {
        pInst->vCardSelectorOperator = 0x01;
        printf("AND operator");
    }
    else
    {
        printf("OR operator");
    }

    pInst->kbm = PB_RESETNEWMISSEDCALL_STATE;
    printf("\nPress (r) to reset missed call counter:\t");

    return TRUE;
}

static CsrBool pbNameState(appInstData *pInst, CsrUint8 cKey)
{
    switch (cKey)
    {
        case 'i':
        case 'I':
        CsrStrLCpy(pInst->name, CSR_BT_PB_FOLDER_ICH_VCF, CSR_BT_MAX_PATH_LENGTH);
            break;
        case 'o':
        case 'O':
        CsrStrLCpy(pInst->name, CSR_BT_PB_FOLDER_OCH_VCF, CSR_BT_MAX_PATH_LENGTH);
            break;
        case 'm':
        case 'M':
        CsrStrLCpy(pInst->name, CSR_BT_PB_FOLDER_MCH_VCF, CSR_BT_MAX_PATH_LENGTH);
            break;
        case 'c':
        case 'C':
        CsrStrLCpy(pInst->name, CSR_BT_PB_FOLDER_CCH_VCF, CSR_BT_MAX_PATH_LENGTH);
            break;
        case 'f':
        case 'F':
        CsrStrLCpy(pInst->name, CSR_BT_PB_FOLDER_FAV_VCF, CSR_BT_MAX_PATH_LENGTH);
            break;
        case 's':
        case 'S':
        CsrStrLCpy(pInst->name, CSR_BT_PB_FOLDER_SPD_VCF, CSR_BT_MAX_PATH_LENGTH);
            break;
        case 'p':
        case 'P':
        default:
        CsrStrLCpy(pInst->name, CSR_BT_PB_FOLDER_PB_VCF, CSR_BT_MAX_PATH_LENGTH);
            break;
    }
    printf("%s\n", pInst->name);
    pInst->nextKbm = PB_ENTRIES_STATE;
    pInst->kbm = PB_VCARD_SELECTOR_OPERATOR_STATE;
    printf("Vcard Selector Operator: Press (a) to select AND operation (default: OR operation):\t");
    return (TRUE);
}

static void pullPbReqSrmpHandler(void *opaque)
{
    appInstData *pInst = (appInstData *) opaque;
    CsrUcs2String *pName = NULL;
    CsrUint32 nNameLen;
    CsrUint16 nMaxSize;

    if (pInst->nEntriesLen == 0)
    {
        snprintf(pInst->szEntries, sizeof(pInst->szEntries), "10");
        pInst->nEntriesLen = 2;
    }
    else
    {
        pInst->szEntries[pInst->nEntriesLen] = '\0';
    }
    pInst->kbm = NORMAL_STATE;

    nNameLen = CsrStrLen(pInst->name) * 2 + 2;
    if (nNameLen > 2)
    {
        pName = CsrUtf82Ucs2ByteString((CsrUint8*) pInst->name);
    }

    sscanf(pInst->szEntries, "%hu", &nMaxSize);

    /* If phone book name is either "mch" or "cch", the resetNewMissedCalls has
     a meaning.
     Value of 1 shall reset the missed call counter. which is NOT what
     we want. Use any non-zero value so that local client shall not
     send the resetNewMissedCalls header to remote server */
    CsrBtPacPullPbReqSendEx(pName,
                            pInst->src,
                            (CsrUint8* )pInst->filter,
                            CSR_BT_PB_FORMAT_VCARD2_1,
                            nMaxSize,
                            0,
                            pInst->resetNewMissedCall,
                            pInst->vCardSelector,
                            pInst->vCardSelectorOperator,
                            pInst->srmp);
    printf("\nDownloading phonebook. Please wait ...!\n");
}

static CsrBool pbEntriesState(appInstData *pInst, CsrUint8 cKey)
{
    CsrBool bProcessed = TRUE;

    if (RETURN_KEY == cKey)
    {
        pInst->srmp = TRUE;
        getSrmp(pInst, pullPbReqSrmpHandler);
    }
    else if (BACKSPACE_KEY == cKey)
    {
        if (pInst->nEntriesLen> 0)
        {
            pInst->nEntriesLen--;
            printf("\b \b");
        }
    }
    else if (cKey >= '0' && cKey <= '9')
    {
        if (pInst->nEntriesLen< MAX_ENTRIES)
        {
            pInst->szEntries[pInst->nEntriesLen++] = cKey;
            printf("%c", cKey);
        }
    }
    else
    {
        bProcessed = FALSE;
    }

    return bProcessed;
}

static void setFolderStart(appInstData *pInst)
{
    printf("Enter new path (["PATH_SEP"] for root (default), [..] for back):\t");
    pInst->kbm = FOLDER_NAME_STATE;
    pInst->nPathLen = 0;
    CsrMemSet(pInst->path, 0, sizeof(pInst->path));
}

static CsrBool folderNameState(appInstData *pInst, CsrUint8 cKey)
{
    CsrBool bProcessed = TRUE;

    if (RETURN_KEY == cKey)
    {
        pInst->path[pInst->nPathLen] = '\0';
        printf("\n");
        pInst->kbm = NORMAL_STATE;
        if (pInst->path[0] == *PATH_SEP || pInst->nPathLen == 0)
        {
            printf("\nRequest path set to root.\n");
            CsrBtPacSetRootFolderReqSend();
        }
        else if ('.' == pInst->path[0] && '.' == pInst->path[1])
        {
            printf("\nRequest path set one step back.\n");
            CsrBtPacSetBackFolderReqSend();
        }
        else
        {
            CsrUcs2String *pFolder = NULL;
            CsrUint32 nFolderLen;

            printf("\nRequest path set to: %s\n", pInst->path);

            nFolderLen = CsrStrLen(pInst->path)*2 + 2;
            if (nFolderLen > 2)
            {
                pFolder = CsrUtf82Ucs2ByteString((CsrUint8*)pInst->path);
            }
            CsrBtPacSetFolderReqSend(pFolder);
        }
    }
    else if (BACKSPACE_KEY == cKey)
    {
        if (pInst->nPathLen> 0)
        {
            pInst->nPathLen--;
            printf("\b \b");
        }
    }
    else if ((cKey >= 32) && (cKey < 127))
    {
        if (pInst->nPathLen < CSR_BT_MAX_PATH_LENGTH)
        {
            pInst->path[pInst->nPathLen++] = cKey;
            printf("%c", cKey);
        }
    }
    else
    {
        bProcessed = FALSE;
    }
    return bProcessed;
}

static void pullPbListStart(appInstData *pInst)
{
    printf("Select phonebook folder to list:\n"
           "\t(p)\t\"pb\" = main phone book\n"
           "\t(i)\t\"ich\" = incoming calls history\n"
           "\t(o)\t\"och\" = outgoing calls history\n"
           "\t(m)\t\"mch\" = missed calls history\n"
           "\t(c)\t\"cch\" = combined calls history\n"
           "\t(f)\t\"fav\" = favourite contacts\n"
           "\t(s)\t\"spd\" = Speed dial contacts\n"
           "\tPress any other key to list the current directory:\t");
    pInst->kbm = VLIST_NAME_STATE;
    CsrMemSet(pInst->name, 0, sizeof(pInst->name));
    pInst->nNameLen = 0;
    pInst->nObjLen = 0;
}

static CsrBool vlistNameState(appInstData *pInst, CsrUint8 cKey)
{
    switch (cKey)
    {
        case 'p':
        case 'P':
        CsrStrLCpy(pInst->name, CSR_BT_PB_FOLDER_PB_STR, CSR_BT_MAX_PATH_LENGTH);
            break;
        case 'i':
        case 'I':
        CsrStrLCpy(pInst->name, CSR_BT_PB_FOLDER_ICH_STR, CSR_BT_MAX_PATH_LENGTH);
            break;
        case 'o':
        case 'O':
        CsrStrLCpy(pInst->name, CSR_BT_PB_FOLDER_OCH_STR, CSR_BT_MAX_PATH_LENGTH);
            break;
        case 'm':
        case 'M':
        CsrStrLCpy(pInst->name, CSR_BT_PB_FOLDER_MCH_STR, CSR_BT_MAX_PATH_LENGTH);
            break;
        case 'c':
        case 'C':
        CsrStrLCpy(pInst->name, CSR_BT_PB_FOLDER_CCH_STR, CSR_BT_MAX_PATH_LENGTH);
            break;
        case 'f':
        case 'F':
        CsrStrLCpy(pInst->name, CSR_BT_PB_FOLDER_FAV_STR, CSR_BT_MAX_PATH_LENGTH);
            break;
        case 's':
        case 'S':
        CsrStrLCpy(pInst->name, CSR_BT_PB_FOLDER_SPD_STR, CSR_BT_MAX_PATH_LENGTH);
            break;
        default:
        CsrStrLCpy(pInst->name, ".", CSR_BT_MAX_PATH_LENGTH);
            break;
    }
    printf("%s", pInst->name);
    pInst->kbm = VLIST_ORDER_STATE;
    printf("\nEnter sorting order [a]lphabetic (default), [i]ndexed or [p]honetical:\t");
    return (TRUE);
}

static CsrBool vlistOrderState(appInstData *pInst, CsrUint8 cKey)
{
    if ('i' == cKey || 'I' == cKey)
    {
        pInst->order = CSR_BT_PB_ORDER_INDEXED;
    }
    else if ('p' == cKey || 'P' == cKey)
    {
        pInst->order = CSR_BT_PB_ORDER_PHONETICAL;
    }
    else
    {
        pInst->order = CSR_BT_PB_ORDER_ALPHABETICAL; /* default */
    }
    printf("%c", cKey);

    printf("\nEnter which attribute to search on ([n]ame (default), n[u]mber or [s]ound):\t");
    pInst->kbm = VLIST_SEARCH_ATT_STATE;
    return (TRUE);
}

static CsrBool vlistSearchAttState(appInstData *pInst, CsrUint8 cKey)
{
    if ('u' == cKey || 'U' == cKey)
    {
        pInst->searchAtt = CSR_BT_PB_SEARCH_ATT_NUMBER;
    }
    else if ('s' == cKey || 'S' == cKey)
    {
        pInst->searchAtt = CSR_BT_PB_SEARCH_ATT_SOUND;
    }
    else
    {
        pInst->searchAtt = CSR_BT_PB_SEARCH_ATT_NAME;
    }
    printf("%c", cKey);

    printf("\nEnter Search string to selected attribute (a \'*\' means all entries (default)):\t");
    pInst->kbm = VLIST_SEARCH_VAL_STATE;
    pInst->nSearchLen = 0;
    return (TRUE);
}

static CsrBool vlistSearchValState(appInstData *pInst, CsrUint8 cKey)
{
    CsrBool bProcessed = TRUE;

    if (RETURN_KEY == cKey)
    {
        if(pInst->nSearchLen == 0)
        {
            pInst->search[0] = '*';
            pInst->nSearchLen = 1;
        }
        else
        {
            pInst->search[pInst->nSearchLen] = '\0';
        }
        pInst->kbm = VLIST_ENTRIES_STATE;
        printf("\nEnter max. number of entries in listing (0 for size only) (default value: 10):\t");
        pInst->nEntriesLen = 0;
    }
    else if (BACKSPACE_KEY == cKey)
    {
        if (pInst->nSearchLen > 0)
        {
            pInst->nSearchLen--;
            printf("\b \b");
        }
    }
    else if ((cKey >= 32) && (cKey < 127))
    {
        if (pInst->nSearchLen < MAX_SEARCH)
        {
            pInst->search[pInst->nSearchLen++] = cKey;
            printf("%c", cKey);
        }
    }
    else
    {
        bProcessed = FALSE;
    }
    return bProcessed;
}

static CsrBool vlistEntriesState(appInstData *pInst, CsrUint8 cKey)
{
    CsrBool bProcessed = TRUE;

    if (RETURN_KEY == cKey)
    {
        if(pInst->nEntriesLen == 0)
        {
            CsrStrLCpy(pInst->szEntries, "10", MAX_ENTRIES);
            pInst->nEntriesLen = 2;
        }
        else
        {
            pInst->szEntries[pInst->nEntriesLen] = '\0';
        }
        pInst->nextKbm = VLIST_OFFSET_STATE;
        pInst->kbm = PB_VCARD_SELECTOR_OPERATOR_STATE;
        printf("\nVcard Selector Operator: Press (a) to select AND operation (default: OR operation):\t");
    }
    else if (BACKSPACE_KEY == cKey)
    {
        if (pInst->nEntriesLen> 0)
        {
            pInst->nEntriesLen--;
            printf("\b \b");
        }
    }
    else if (cKey >= '0' && cKey <= '9')
    {
        if (pInst->nEntriesLen< MAX_ENTRIES)
        {
            pInst->szEntries[pInst->nEntriesLen++] = cKey;
            printf("%c", cKey);
        }
    }
    else
    {
        bProcessed = FALSE;
    }
    return bProcessed;
}

static void listReqSrmpHandler(void *opaque)
{
    appInstData *pInst = (appInstData *) opaque;
    CsrUint32 nTmp = 0xffff;
    CsrUint16 nStartOffset;
    CsrUcs2String *pName = NULL, *pSearch;

    if (pInst->nEntriesLen == 0)
    {
        CsrStrLCpy(pInst->szOffset, "0", MAX_ENTRIES);
        pInst->nOffsetLen = 1;
    }
    else
    {
        pInst->szOffset[pInst->nOffsetLen] = '\0';
    }
    printf("\n");
    pInst->kbm = NORMAL_STATE;

    sscanf(pInst->szEntries, "%u", &nTmp);
    pInst->nMaxLstCnt = (CsrUint16) nTmp;
    pInst->nEntriesLen = 0;

    nTmp = 0;
    sscanf(pInst->szOffset, "%u", &nTmp);
    nStartOffset = (CsrUint16) nTmp;

    if (('.' == pInst->name[0] && 0 == pInst->name[1]))
    {
        pName = NULL; /* no search list all entries */
    }
    else
    {
        CsrUint32 nNameLen;

        nNameLen = CsrStrLen(pInst->name) * 2 + 2;
        if (nNameLen > 2)
        {
            pName = CsrUtf82Ucs2ByteString((CsrUint8*) pInst->name);
        }
    }

    if (0 == pInst->search[0] || '*' == pInst->search[0])
    {
        pSearch = NULL; /* don't use searchVal */
    }
    else
    {
        CsrUint16 strLen = (CsrUint16)CsrStrLen(pInst->search) + 1;
        pSearch = CsrPmemAlloc(strLen);
        CsrStrLCpy((char *) pSearch, pInst->search, strLen);
    }

    CsrBtPacPullVcardListReqSendEx(pName,
                                   pInst->order,
                                   pSearch,
                                   pInst->searchAtt,
                                   pInst->nMaxLstCnt,
                                   nStartOffset,
                                   pInst->resetNewMissedCall,
                                   pInst->vCardSelector,
                                   pInst->vCardSelectorOperator,
                                   pInst->srmp);
}

static CsrBool vlistOffsetState(appInstData *pInst, CsrUint8 cKey)
{
    CsrBool bProcessed = TRUE;

    if (RETURN_KEY == cKey)
    {
        pInst->srmp = TRUE;
        getSrmp(pInst, listReqSrmpHandler);
    }
    else if (BACKSPACE_KEY == cKey)
    {
        if (pInst->nOffsetLen > 0)
        {
            pInst->nOffsetLen--;
            printf("\b \b");
        }
    }
    else if (cKey >= '0' && cKey <= '9')
    {
        if (pInst->nOffsetLen < MAX_ENTRIES)
        {
            pInst->szOffset[pInst->nOffsetLen++] = cKey;
            printf("%c", cKey);
        }
    }
    else
    {
        bProcessed = FALSE;
    }
    return bProcessed;
}

static void pullPbEntryStart(appInstData *pInst)
{
    pInst->nObjLen = 0;
    printf("Enter entry name:\n"
        "\texamples:\n"
        "\t\"pb.vcf\" = main phone book\n"
        "\t\"0.vcf\" = owners card (only allowed in telecom/pb or sim1/telecom/pb folder!)\n"
        "\t\"xxx.vcf\" = anyother vCard entry listing obtained from the phonebook listing\n"
        ":\t");
    pInst->nNameLen = 0;
    pInst->kbm = VCARD_NAME_STATE;
}

static void pullPbEntryByUIDStart(appInstData *pInst)
{
    pInst->nObjLen = 0;
    printf("Enter entry the UID (16byte) number:\n"
        "\texamples:\n"
        "\t\"11223344556677889900aabbccddeeff\"\n"
        "\t Press Enter when you are done\n"
        "\t Press x to go back to prev menu\n"
        ":\t");
    CsrMemSet(pInst->UIDchar, '0', UID_LENGTH);
    CsrStrLCpy((char *) pInst->UIDchar, "X-BT-UID:", UID_LENGTH);
    pInst->UIDcharEntryIndex = 0;
    pInst->kbm = PULL_PB_ENTRY_BY_UID;
}

static CsrBool vcardEnterUIDState(appInstData *pInst, CsrUint8 cKey)
{
    CsrBool bProcessed = TRUE;
    CsrBool isCharValid = FALSE;
    CsrUcs2String *pName = NULL;
    CsrUint32 nNameLen;

    if ('x' == cKey || 'X' == cKey)
    {
        pInst->state = CONNECTED_STATE;
        pInst->kbm = NORMAL_STATE;
        playIntro(pInst);
        return bProcessed;
    }

    /* Operation is complete either on entery key or all 32 char are entered */
    if ((RETURN_KEY == cKey) || (pInst->UIDcharEntryIndex == 32))
    {
        CsrUint8* uid;

        pInst->kbm = NORMAL_STATE;

        while (pInst->UIDcharEntryIndex < 32)
        {
            pInst->UIDchar[UID_TYPE_LEN+pInst->UIDcharEntryIndex] = '0';
            pInst->UIDcharEntryIndex++;
        }

        /* We are done...*/
        pInst->UIDchar[UID_LENGTH - 1] = '\0';
        uid = CsrPmemAlloc(UID_LENGTH);
        CsrMemCpy(uid, pInst->UIDchar, UID_LENGTH);

        pInst->nNameLen = UNIQUE_ID_LENGTH;
        CsrMemCpy(pInst->name, pInst->UIDchar+UID_TYPE_LEN, pInst->nNameLen);
        CsrMemCpy(pInst->name+UNIQUE_ID_LENGTH, ".vcf", 4);
        pInst->nNameLen += 4;
        pInst->name[pInst->nNameLen] = '\0';
        
        printf("\n\t Thank you! You entered: \"%s\"\n", pInst->UIDchar);
        printf("\t Starting the Pull Vcard Entry by UID...please wait !");

        nNameLen = CsrUcs2ByteStrLen((CsrUcs2String*)uid);
        if (nNameLen > 0)
        {
            pName = CsrUtf82Ucs2ByteString((CsrUint8*)uid);
        }
        CsrBtPacPullVcardEntryReqSend(pName, (CsrUint8*)pInst->filter, CSR_BT_PB_FORMAT_VCARD2_1, pInst->srmp);
        CsrPmemFree(uid);
    }
    else
    {
        if (cKey >= '0' && cKey <= '9')
        {
            isCharValid = TRUE;
        }
        if (cKey >= 'a' && cKey <= 'f')
        {
            cKey = cKey - ('a' - 'A');
            isCharValid = TRUE;
        }
        if (cKey >= 'A' && cKey <= 'F')
        {
            isCharValid = TRUE;
        }
        if (BACKSPACE_KEY == cKey)
        {
            if (pInst->UIDcharEntryIndex > 0)
            {
                pInst->UIDcharEntryIndex--;
                printf("\b \b");
            }
        }
        if (isCharValid == TRUE)
        {
            pInst->UIDchar[UID_TYPE_LEN+pInst->UIDcharEntryIndex] = cKey;
            pInst->UIDcharEntryIndex++;
            printf("%c", cKey);
        }
    }

    return bProcessed; 
}

static CsrBool srmpState(appInstData *pInst, CsrUint8 cKey)
{
    if ('s' == cKey || 'S' == cKey)
    {
        pInst->srmp = TRUE;
        printf("SRM suspended");
    }
    else
    {
        pInst->srmp = FALSE;
        printf("SRM activated");
    }
    if (pInst->srmpHandler)
    {
        pInst->srmpHandler(pInst);
    }
    else
    {
        pInst->kbm = NORMAL_STATE;
    }
    return TRUE;
}

static void pullVcardReqSrmpHandler(void *opaque)
{
    appInstData *pInst = (appInstData *) opaque;
    CsrUcs2String *pName = NULL;
    CsrUint32 nNameLen;

    nNameLen = CsrUcs2ByteStrLen((CsrUcs2String*) pInst->name);
    if (nNameLen > 0)
    {
        pName = CsrUtf82Ucs2ByteString((CsrUint8*) pInst->name);
    }

    CsrBtPacPullVcardEntryReqSend(pName,
                                  (CsrUint8* )pInst->filter,
                                  CSR_BT_PB_FORMAT_VCARD2_1,
                                  pInst->srmp);
    pInst->kbm = NORMAL_STATE;
    printf("Requesting entry: %s ...\n", pInst->name);
}

static CsrBool vcardNameState(appInstData *pInst, CsrUint8 cKey)
{
    CsrBool bProcessed = TRUE;

    if (RETURN_KEY == cKey)
    {
        if (pInst->nNameLen == 0)
        {
            printf("\nInvalid selection: you must specify an entry name to download\n");
            bProcessed = TRUE;
        }
        else
        {
            pInst->name[pInst->nNameLen] = '\0';
            pInst->srmp = TRUE;
            getSrmp(pInst, pullVcardReqSrmpHandler);
        }
    }
    else if (BACKSPACE_KEY == cKey)
    {
        if (pInst->nNameLen > 0)
        {
            pInst->nNameLen--;
            printf("\b \b");
        }
    }
    else if ((cKey >= 32) && (cKey < 127))
    {
        if (pInst->nNameLen < CSR_BT_MAX_PATH_LENGTH)
        {
            pInst->name[pInst->nNameLen++] = cKey;
            printf("%c", cKey);
        }
    }
    else
    {
        bProcessed = FALSE;
    }
    return bProcessed;
}

static void abortStart(appInstData *pInst)
{
    CSR_UNUSED(pInst);
    printf("Abort all operation.\n\n");
    CsrBtPacAbortReqSend();
}

static void disconnectPacStart(appInstData *pInst)
{
    if(pInst->state != CONNECT_STATE)
    {
        CsrBtPacDisconnectReqSend(TRUE);
        printf("\nDisconnecting from server. Please wait .... \n");
    }
    else
    {
        CsrBtPacCancelConnectReqSend();
        printf("\nCancelling connection attempt. Please wait .... \n");
    }
    pInst->state = IDLE_STATE;
    if (pInst->pFile)
    {
        CsrBtFclose(pInst->pFile);
        pInst->pFile = NULL;
    }
}


/*******************************************************************************
 *
 *   a keyboard event is received.
 *   first check for selection of a found device during inquiry if state is
 *   inquiry. If not start proper action as selected.
 *
 *******************************************************************************/
static void handleKeybPrim(appInstData *pInst)
{
    CsrBtKeyPress_t *key;
    CsrBool bProcessed;
    key = (CsrBtKeyPress_t*)pInst->pRecvMsg;

    if( key->key == ESCAPE_KEY )
    {
        printf("\nUser exit...\n");
#ifdef ENABLE_SHUTDOWN
        CsrSchedStop();
#else
        exit(0);
#endif
        return;
    }

    switch (pInst->kbm)
    {
        case PASSKEY_STATE:
            bProcessed = passkeyState(pInst, key->key);
            break;

        case SECURITY_KEYBOARD_STATE:
            handleScKeybPrim(&pInst->scInstData);
            bProcessed = TRUE;
            break;

        case ADDRESS_STATE:
            bProcessed = addressState(pInst, key->key);
            break;

        case FILTER_STATE:
            bProcessed = filterState(pInst, key->key);
            break;

        case VCARDSELECTOR_STATE:
            bProcessed = vCardSelectorState(pInst, key->key);
            break;

        case PB_SOURCE_STATE:
            bProcessed = pbSourceState(pInst, key->key);
            break;

        case PB_NAME_STATE:
            bProcessed = pbNameState(pInst, key->key);
            break;

        case PB_RESETNEWMISSEDCALL_STATE:
            bProcessed = pbResetNewMissedCallState(pInst, key->key);
            break;

        case PB_VCARD_SELECTOR_OPERATOR_STATE:
            bProcessed = pbVcardSelectorOperatorState(pInst, key->key);
            break;

        case PB_ENTRIES_STATE:
            bProcessed = pbEntriesState(pInst, key->key);
            break;

        case FOLDER_NAME_STATE:
            bProcessed = folderNameState(pInst, key->key);
            break;

        case VLIST_NAME_STATE:
            bProcessed = vlistNameState(pInst, key->key);
            break;

        case VLIST_ORDER_STATE:
            bProcessed = vlistOrderState(pInst, key->key);
            break;

        case VLIST_SEARCH_ATT_STATE:
            bProcessed = vlistSearchAttState(pInst, key->key);
            break;

        case VLIST_SEARCH_VAL_STATE:
            bProcessed = vlistSearchValState(pInst, key->key);
            break;

        case VLIST_ENTRIES_STATE:
            bProcessed = vlistEntriesState(pInst, key->key);
            break;

        case VLIST_OFFSET_STATE:
            bProcessed = vlistOffsetState(pInst, key->key);
            break;

        case VCARD_NAME_STATE:
            bProcessed = vcardNameState(pInst, key->key);
            break;

        case SRMP_STATE:
            bProcessed = srmpState(pInst, key->key);
            break;

        case PULL_PB_ENTRY_BY_UID:
            bProcessed = vcardEnterUIDState(pInst, key->key);
            break;

        default:
            switch(pInst->state)
            {
                case IDLE_STATE:
                    bProcessed = idleState(pInst, key->key);
                    break;

                case CONNECT_STATE:
                    bProcessed = connectState(pInst, key->key);
                    break;

                case CONNECTED_STATE:
                    bProcessed = connectedState(pInst, key->key);
                    break;

                case SEARCHING_STATE:
                    bProcessed = searchState(pInst, key->key);
                    break;

                default:
                    bProcessed = FALSE;
                    break;
            }
            break;
    }
    /* any state */
    if (!bProcessed)
    {
        /*  unexpected primitive received */
        playIntro(pInst);
        printf("####### default in keybPrim handler %x,\n", key->key);
    }
}

static void startConnect(appInstData *pInst)
{
    CsrBtDeviceAddr slaveDeviceAddr = pInst->bdAddrList[pInst->nDevice];

    CsrBtPacConnectReqSend(pInst->CsrSchedQid,
                             PAC_MAX_OBEX_PACKET_SIZE,
                             slaveDeviceAddr,
                             0 /* windowSize */);

    pInst->state = CONNECT_STATE;
    playIntro(pInst);
    printf("Connecting to PBAP Server. Please wait .... \n");
}

static CsrBool passkeyState(appInstData *pInst, CsrUint8 cKey)
{
    CsrBool isPasskey = TRUE;

    if (RETURN_KEY == cKey)
    {
        pInst->passkey[pInst->passkeyLength] = '\0';
        printf("\n");
        pInst->kbm = NORMAL_STATE;

        switch (pInst->state)
        {
            case CONNECT_INIT_STATE:
                    startConnect(pInst);
                break;
            case AUTHENTICATING_STATE:
            case CONNECT_STATE:
            {
                CsrUint8 *key;

                key = CsrMemDup(pInst->passkey, pInst->passkeyLength);

                CsrBtPacAuthenticateResSend(key, pInst->passkeyLength, NULL);
                break;
            }

            default:
                printf("Unknown state: %d\n", pInst->state);
                break;
        }
    }
    else if (BACKSPACE_KEY == cKey)
    {
        if (pInst->passkeyLength > 0)
        {
            pInst->passkeyLength--;
            printf("\b \b");
        }
    }
    else if ((cKey >= 32) && (cKey < 127))
    {
        if (pInst->passkeyLength < CSR_BT_PASSKEY_MAX_LEN)
        {
            pInst->passkey[pInst->passkeyLength++] = cKey;
            printf("%c", cKey);
        }
    }
    else
    {
        isPasskey = FALSE;
    }
    return isPasskey;
}

static CsrBool filterState(appInstData *pInst, CsrUint8 cKey)
{
    CsrUint32    val, tmpval;
    CsrUint8     offset = 0; /* offset into pInst->filter[] */
    char        *s;

    pInst->kbm = NORMAL_STATE;

    /*
     * When you change this, REMEMBER
     * to sync with filterMenu() below.
     */

    switch (cKey) {
    case 'e':
    case 'E':
        val = CSR_BT_PB_VCARD_PROP_EMAIL;
        s = "email";
        break;

    case 'n':
    case 'N':
        val = CSR_BT_PB_VCARD_PROP_N;
        s = "name";
        break;

    case 'p':
    case 'P':
        val = CSR_BT_PB_VCARD_PROP_TEL;
        s = "phone number";
        break;

    case 'r':
    case 'R':
        val = CSR_BT_PB_VCARD_PROP_ROLE;
        s = "role";
        break;

    case 't':
    case 'T':
        val = CSR_BT_PB_VCARD_PROP_TITLE;
        s = "title";
        break;

    case 'v':
    case 'V':
        val = CSR_BT_PB_VCARD_PROP_VERSION;
        s = "VCARD version";
        break;

    case 'i':
    case 'I':
        val = CSR_BT_PB_VCARD_PROP_PHOTO;
        s = "Associated Image or Photo";
        break;

    default:
        printf("Invalid filter key.\n");
        return TRUE;
    }

    /*
     * Find offset into pInst->filter[] and flip
     * the appropriate bit in that 8 bit quantity.
     * Find offset by shifting value bitpattern 8
     * positions to the right.  While the result
     * is positive (only one bit set), the
     * pInst->filter offset is too small.
     *
     * Note that while CsrUint32 is really too small
     * to store all bits, bits 29 through 63 are
     * reserved and not used for qualification tests.
     */
    tmpval = val;
    while (tmpval >> 8) {
        tmpval >>= 8;
        offset++;
    }
    val >>= 8 * offset;

    /* Now flip/xor the bit. */
    pInst->filter[offset] ^= val;

    playIntro(pInst);
    printf("`%s' bit flipped, now %d.\n", s,
        (pInst->filter[offset] & val) ? 1 : 0);

    return TRUE;
}

static CsrBool vCardSelectorState(appInstData *pInst, CsrUint8 cKey)
{
    CsrUint32    val, tmpval;
    CsrUint8     offset = 0; /* offset into pInst->filter[] */
    char        *s;

    pInst->kbm = NORMAL_STATE;

    switch (cKey) {
    case 'e':
    case 'E':
        val = CSR_BT_PB_VCARD_PROP_EMAIL;
        s = "email";
        break;

    case 'n':
    case 'N':
        val = CSR_BT_PB_VCARD_PROP_N;
        s = "name";
        break;

    case 'p':
    case 'P':
        val = CSR_BT_PB_VCARD_PROP_TEL;
        s = "phone number";
        break;

    case 'r':
    case 'R':
        val = CSR_BT_PB_VCARD_PROP_ROLE;
        s = "role";
        break;

    case 't':
    case 'T':
        val = CSR_BT_PB_VCARD_PROP_TITLE;
        s = "title";
        break;

    case 'v':
    case 'V':
        val = CSR_BT_PB_VCARD_PROP_VERSION;
        s = "VCARD version";
        break;

    case 'l':
    case 'L':
        val = CSR_BT_PB_VCARD_PROP_X_BT_UCI;
        s = "Uniform Caller Identifier";
        break;

    case 'u':
    case 'U':
        val = CSR_BT_PB_VCARD_PROP_X_BT_UID;
        s = "Bluetooth Contact Unique Identifier";
        break;

    case 's':
    case 'S':
        val = CSR_BT_PB_VCARD_PROP_X_BT_SPEEDDIALKEY;
        s = "SpeedDial Key";
        break;

    case 'i':
    case 'I':
        val = CSR_BT_PB_VCARD_PROP_PHOTO;
        s = "Associated Image or Photo";
        break;

    default:
        printf("Invalid filter key.\n");
        return TRUE;
    }

    /*
     * Find offset into pInst->vCardSeletor[] and flip
     * the appropriate bit in that 8 bit quantity.
     * Find offset by shifting value bitpattern 8
     * positions to the right.  While the result
     * is positive (only one bit set), the
     * pInst->filter offset is too small.
     *
     * Note that while CsrUint32 is really too small
     * to store all bits, bits 29 through 63 are
     * reserved and not used for qualification tests.
     */
    tmpval = val;
    while (tmpval >> 8) {
        tmpval >>= 8;
        offset++;
    }
    val >>= 8 * offset;

    /* Now flip/xor the bit. */
    pInst->vCardSelector[offset] ^= val;

    playIntro(pInst);
    printf("`%s' bit flipped, now %d.\n", s,
        (pInst->vCardSelector[offset] & val) ? 1 : 0);

    return TRUE;
}


static void filterMenu(void)
{
    printf("VCARD filter options\n\n"
        "  e) email (EMAIL)\n"
        "  n) name (N, Structured Presentation of Name)\n"
        "  p) phone number (TEL)\n"
        "  r) organization role (ROLE)\n"
        "  s) speed dial (X-BT-SPEEDDIALKEY)\n"
        "  t) title (TITLE)\n"
        "  i) Associated Image or Photo\n"
        "  v) VCARD version no. (VERSION)\n\n");

}

static void vCardSelectorMenu(void)
{
    printf("VCARD Selector Menu options\n\n"
        "  l) Uniform Caller Identifier (bit 30)\n"
        "  u) Unique Identifier (bit 31)\n"
        "  e) email (EMAIL)\n"
        "  n) name (N, Structured Presentation of Name)\n"
        "  p) phone number (TEL)\n"
        "  s) speed dial (X-BT-SPEEDDIALKEY)\n"
        "  r) organization role (ROLE)\n"
        "  t) title (TITLE)\n"
        "  i) Associated Image or Photo\n"
        "  v) VCARD version no. (VERSION)\n\n");
}

static CsrBool addressState(appInstData *pInst, CsrUint8 cKey)
{
    CsrBool isAddr = TRUE;

    if (RETURN_KEY == cKey)
    {
        CsrUint32 n, u, l;
        pInst->szBdAddr[pInst->BdAddrLen] = '\0';
        printf("\n");
        pInst->kbm = NORMAL_STATE;
        pInst->nDevice = 0;
        sscanf(pInst->szBdAddr, "%04X:%02X:%06X", &n, &u, &l);
        pInst->bdAddrList[pInst->nDevice].nap = (CsrUint16)n;
        pInst->bdAddrList[pInst->nDevice].uap = (CsrUint8)u;
        pInst->bdAddrList[pInst->nDevice].lap = (CsrUint24)l;
    }
    else if (BACKSPACE_KEY == cKey)
    {
        if (pInst->BdAddrLen > 0)
        {
            pInst->BdAddrLen--;
            printf("\b \b");
        }
    }
    else if ((cKey >= 32) && (cKey < 127))
    {
        if (pInst->BdAddrLen < 15)
        {
            pInst->szBdAddr[pInst->BdAddrLen++] = cKey;
            printf("%c", cKey);
        }
    }
    else
    {
        isAddr = FALSE;
    }
    return isAddr;
}

static CsrBool idleState(appInstData *pInst, CsrUint8 cKey)
{
    CsrBool bProcessed = TRUE;

    switch( cKey)
    {
        case 's': /* Discover new devices */
        case 'S':
            searchStart(pInst);
            pInst->state = SEARCHING_STATE;
            break;

        case 'c': /* Connect to server */
        case 'C':
            startConnect(pInst);
            break;
        case 't': /* Security Controller Configuration */
        case 'T':
            handleScKeybPrim(&pInst->scInstData);
            break;

        case 'b':
        case 'B':
            pInst->kbm = ADDRESS_STATE;
            pInst->BdAddrLen = 0;
            printf("Enter bluetooth address (XXXX:XX:XXXXXX [nap:uap:lap]): ");
            break;



        case ' ':
        case RETURN_KEY:
            playIntro(pInst);
            break;
        default:
            printf("\nwarning: not a valid selection to press: %c, try again\n", cKey);
            break;
    }
    return bProcessed;
}

static CsrBool connectState(appInstData *pInst, CsrUint8 cKey)
{
    CsrBool bProcessed = TRUE;

    switch( cKey)
    {
        case 'd':
        case 'D':
            disconnectPacStart(pInst);
            break;
        case ' ':
        case RETURN_KEY:
            playIntro(pInst);
            break;
        default:
            printf("\nwarning: not a valid selection to press: %c, try again\n", cKey);
            break;
    }
    return bProcessed;
}

static CsrBool connectedState(appInstData *pInst, CsrUint8 cKey)
{
    CsrBool bProcessed = TRUE;

    switch( cKey)
    {
        case 'p':
        case 'P':
            pullPbStart(pInst);
            break;

        case 's':
        case 'S':
            setFolderStart(pInst);
            break;

        case 'l':
        case 'L':
            pullPbListStart(pInst);
            break;

        case 'e':
        case 'E':
            pullPbEntryStart(pInst);
            break;

        case 'u':
        case 'U':
            pullPbEntryByUIDStart(pInst);
            break;

        case 'a':
        case 'A':
            abortStart(pInst);
            break;

        case 'd':
        case 'D':
            disconnectPacStart(pInst);
            break;

        case 'f':
        case 'F':
            filterMenu();
            pInst->kbm = FILTER_STATE;
            break;

        case 'g':
        case 'G':
            pInst->filter[0] = 0x00;
            pInst->filter[1] = 0x00;
            pInst->filter[2] = 0x00;
            pInst->filter[3] = 0x00;
            pInst->filter[4] = 0x00;
            pInst->filter[5] = 0x00;
            pInst->filter[6] = 0x00;
            pInst->filter[7] = 0x00;
            printf("vcard request filter cleared.\n");
            break;

        case 'h':
        case 'H':
            /* get everything */
            pInst->filter[0] = 0xff;
            pInst->filter[1] = 0xff;
            pInst->filter[2] = 0xff;
            pInst->filter[3] = 0xff;/* | ((VCARD_X_BT_UCI | VCARD_SPEEDDIALKEY | VCARD_X_BT_UID) >> 3);*/
            pInst->filter[4] = 0x00;
            pInst->filter[5] = 0x00;
            pInst->filter[6] = 0x00;
            pInst->filter[7] = 0x00;
            printf("all non-reserved vcard request filter bits enabled.\n");
            break;

        case 'v':
        case 'V':
            vCardSelectorMenu();
            pInst->kbm = VCARDSELECTOR_STATE;
            break;

        case 'c':
        case 'C':
            pInst->vCardSelector[0] = 0x00;
            pInst->vCardSelector[1] = 0x00;
            pInst->vCardSelector[2] = 0x00;
            pInst->vCardSelector[3] = 0x00;
            pInst->vCardSelector[4] = 0x00;
            pInst->vCardSelector[5] = 0x00;
            pInst->vCardSelector[6] = 0x00;
            pInst->vCardSelector[7] = 0x00;
            printf("vcard selector attribute bits cleared.\n");
            break;

        case 'x':
        case 'X':
            /* get everything */
            pInst->vCardSelector[0] = 0xff;
            pInst->vCardSelector[1] = 0xff;
            pInst->vCardSelector[2] = 0xff;
            pInst->vCardSelector[3] = 0x7f; /* bit 30 logcial operations (= OR) aswell set.*/
            pInst->vCardSelector[4] = 0x00;
            pInst->vCardSelector[5] = 0x00;
            pInst->vCardSelector[6] = 0x00;
            pInst->vCardSelector[7] = 0x00;
            printf("all non-reserved vcard selector attributes bits enabled.\n");
            break;

        case ' ':
        case RETURN_KEY:
            playIntro(pInst);
            break;
        default:
            printf("\nwarning: not a valid selection to press: %c, try again\n", cKey);
            break;
    }
    return bProcessed;
}

static CsrBool searchState(appInstData *pInst, CsrUint8 cKey)
{
    CsrBool bProcessed = TRUE;

    if (cKey >= '0' && cKey <= '9')
    {
        if (pInst->nDeviceInList >= (CsrUint32)(cKey - '0'))
        {
            pInst->nDevice = cKey - '0';
            bProcessed = TRUE;
            if(pInst->bFullDeviceList)
            {
                printf(
                    "Selecting device with addr: %04X:%02X:%06X\n",
                    pInst->bdAddrList[pInst->nDevice].nap, pInst->bdAddrList[pInst->nDevice].uap,
                    pInst->bdAddrList[pInst->nDevice].lap
                    );
                scAppSetBondingAddr(&pInst->scInstData,
                                    pInst->bdAddrList[pInst->nDevice]);
                pInst->state = IDLE_STATE;
            }
            else
            {
                CsrBtSdCancelSearchReqSend(pInst->CsrSchedQid);
            }
        }
        else
        {
            printf("You have selected an unknown device! Please try again\n");
        }
    }
    return bProcessed;
}

static char *getPathStart(char *path)
{
    CsrUint32 i;

    for(i=CsrStrLen(path); i>0; i--)
    {
        if((char)path[i] == *"/" || (char)path[i] == *"\\" )
        {
            break;
        }
    }

    return (char*)(path + i);
}

void applicationUsage(char *col, char *des, char *opt)
{
}

char* applicationCmdLineParse(int ch, char *optionArgument)
{
    return NULL;
}

void applicationSetup(void)
{
}

char* applicationExtraOptions(void)
{
    return "";
}
