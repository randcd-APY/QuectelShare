/******************************************************************************

Copyright (c) 2014-2016 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_synergy.h"
#include <stdio.h>
#include <string.h>

#include "csr_util.h"
#include "csr_pmem.h"
#include "csr_exceptionhandler.h"

#include "csr_app_lib.h"

#include "csr_bt_platform.h"
#include "csr_bt_gap_app_lib.h"
#include "csr_bt_util.h"
#include "csr_bt_pac_prim.h"
#include "csr_bt_demoapp.h"
#include "csr_unicode.h"

#include "csr_bt_pac_lib.h"
#include "csr_bt_pac_app.h"
#include "csr_bt_pac_app_ui_sef.h"
#include "csr_bt_pac_app_task.h"
#include "csr_bt_pac_app_strings.h"
#include "csr_bt_obex.h"
#include "csr_bt_pac_app_files.h"

/******************************************************************************
 CsrBtPacAppResetInstance:
 *****************************************************************************/
static void csrBtPacAppResetInstance(CsrBtPacAppInstData_t* inst)
{
    inst->btConnId      = CSR_BT_CONN_ID_INVALID;
    inst->connecting    = FALSE;

    /* Reset peer device name */
    CsrMemSet(inst->remoteDeviceName, 0, sizeof(inst->remoteDeviceName));

    inst->recvMsgP  = NULL;
    inst->pFile     = NULL;

    /* Default filter gets everything */
    inst->filter[0] = 0xff;
    inst->filter[1] = 0xff;
    inst->filter[2] = 0xff;

    /* bit29 - X-BT-SPEEDDIALKEY, bit -30 - Logical operation of filtering */
    inst->filter[3] = 0xff;
    inst->filter[4] = 0x00;
    inst->filter[5] = 0x00;
    inst->filter[6] = 0x00;
    inst->filter[7] = 0x00;

    /* Default vCardSelector value gets everything */
    inst->vCardSelector[0] = 0x00;
    inst->vCardSelector[1] = 0x00;
    inst->vCardSelector[2] = 0x00;
    inst->vCardSelector[3] = 0x00;
    inst->vCardSelector[4] = 0x00;
    inst->vCardSelector[5] = 0x00;
    inst->vCardSelector[6] = 0x00;
    inst->vCardSelector[7] = 0x00;

    /* If set to 1, the remote server shall reset the missed call */
    inst->resetNewMissedCall    = 0x00;

    /*Reset other values defined in CsrBtPacAppInstData_t*/
    inst->vCardSelectorOperator = 0x00;
    inst->vCardFormat           = DEFAULT_VCARD_FORMAT;
    inst->order                 = CSR_BT_PAC_ORDER_ALPHABETICAL;
    inst->src                   = CSR_BT_PAC_SRC_PHONE;
    inst->enterPassword         = FALSE;
    inst->userId                = NULL;
    inst->enterUserId           = FALSE;
    inst->startBrowsing         = FALSE;
    inst->downloadedPopUp       = FALSE;
    inst->value                 = 0;
    inst->valueFilter           = 0;
    inst->nMaxLstCnt            = DEFAULT_MAX_LIST_COUNT;
    inst->maxListCount          = FALSE;
    inst->pSearch               = NULL;
    inst->browseContacts        = FALSE;
    inst->entryFolder           = 0;
    inst->entryFromListing      = FALSE;
    inst->entryKey              = 0;
    inst->entryByUid            = FALSE;
    inst->listing               = FALSE;
    inst->setEntrySim           = FALSE;
    inst->startOffsetVal        = 0;
    inst->startOffset           = FALSE;
}

static void csrBtPacAppConnectCfmHandler(CsrBtPacAppInstData_t* inst)
{
    CsrBtPacConnectCfm* prim;
    /* CsrBtPacAppUiInstData *ui = &inst->ui; */

    prim = (CsrBtPacConnectCfm*) inst->recvMsgP;

    if (((inst->state == CONNECT_STATE ) || (inst->state == AUTHENTICATE_STATE))
        && prim->resultSupplier == CSR_BT_SUPPLIER_IRDA_OBEX
        && prim->resultCode == CSR_BT_OBEX_SUCCESS_RESPONSE_CODE
        && prim->btConnId != CSR_BT_CONN_ID_INVALID)
    {
        CsrUint16 index;
        CsrUint16 currentValue;
        CsrUint32 address;

        inst->btConnId = prim->btConnId;
        /*Show the address of the remote device as a sublabel to the Disconnect
         * label after successful connection*/
        if (CsrStrLen(inst->remoteDeviceName) == 0)
        {
            CsrUint32 length;

            /*To display nap value*/
            address = inst->selectedDeviceAddr.nap;
            CsrUInt16ToHex(address, inst->remoteDeviceName);
            CsrStrLCat(inst->remoteDeviceName, "-", MAX_ADDRESS_LEN);
            length = CsrStrLen(inst->remoteDeviceName);


            /*To display uap value*/
            address = inst->selectedDeviceAddr.uap;
            for (index = 0; index < 2; index++)
            {
                currentValue = (CsrUint16)(address & 0x000F);
                address >>= 4;
                (inst->remoteDeviceName + length)[1 - index] =
                (char) (currentValue > 9 ? currentValue + 55 :currentValue+'0');
            }
            (inst->remoteDeviceName + length)[2] = '\0';
            CsrStrLCat(inst->remoteDeviceName, "-", MAX_ADDRESS_LEN);
            length = CsrStrLen(inst->remoteDeviceName);

            /*To display lap value*/
            address = inst->selectedDeviceAddr.lap;
            for (index = 0; index < 6; index++)
            {
                currentValue = (CsrUint16)(address & 0x00000F);
                address >>= 4;
                (inst->remoteDeviceName + length)[5 - index] =
                (char) (currentValue > 9 ? currentValue + 55 :currentValue+'0');
            }
            (inst->remoteDeviceName + length)[6] = '\0';
        }

        CsrBtPacAppHandleUiConnectDisconnect(inst);
    }
    else
    {
        if (inst->state == AUTHENTICATE_STATE)
        {
            CsrUiDialogSetReqSend(inst->ui.hCommonDialog,
                                  NULL,
                                  TEXT_PAC_APP_AUTHENTICATION_FAILED_UCS2,
                                  TEXT_OK_UCS2,
                                  NULL,
                                  inst->ui.hMainMenu,
                                  0,
                                  inst->ui.hBackEvent,
                                  0);

            CsrUiUieShowReqSend(inst->ui.hCommonDialog,
                                CSR_BT_PAC_APP_IFACEQUEUE,
                                CSR_UI_INPUTMODE_AUTO,
                                CSR_BT_PAC_APP_CSR_UI_PRI);

            inst->ui.popup = TRUE;
        }
        else if (inst->state == CONNECT_STATE)
        {
            CsrBtPacAppUiPopupHide(inst);
        }
    }

    inst->state = DOWNLOAD_STATE;
    inst->connecting = FALSE;
}

static void csrBtPacAppDisconnectIndHandler(CsrBtPacAppInstData_t* inst)
{
    csrBtPacAppResetInstance(inst);
    CsrBtPacAppHandleUiConnectDisconnect(inst);
    inst->state = IDLE_STATE;
}

/*Handles authentication when the remote device asks for it*/
static void handlePacAuthInd(CsrBtPacAppInstData_t* inst)
{
    CsrBtPacAuthenticateInd *pMsg = (CsrBtPacAuthenticateInd *) inst->recvMsgP;
    /* CsrBtPacAppUiInstData *ui = &inst->ui; */

    CsrBtPacAppUiPopupHide(inst);

    inst->enterUserId = TRUE;
    inst->state = AUTHENTICATE_STATE;

    CsrPmemFree(pMsg->realm);

    CsrUiUieShowReqSend(inst->ui.userId,
                        CSR_BT_PAC_APP_IFACEQUEUE,
                        CSR_UI_INPUTMODE_AUTO,
                        CSR_BT_PAC_APP_CSR_UI_PRI);

}

/*Aborts the ongoing operation*/
static void handlePacAbort(CsrBtPacAppInstData_t *inst)
{
    /* Remove UI since downloading has been aborted */
    CsrBtPacAppUiPopupHide(inst);

    if (inst->pFile)
    {
        CsrBtFclose(inst->pFile);
        inst->pFile = NULL;
    }
}

static void handleCsrBtPacPullPbInd(CsrBtPacAppInstData_t *inst)
{
    CsrBtPacPullPbInd *pMsg = inst->recvMsgP;
    CsrUint32 nWrite;
    /* CsrBtPacAppUiInstData *ui = &inst->ui; */

    if(inst->srmp == FALSE)
        CsrBtPacAppUiPopup(inst, "Downloading", "Please Wait");

    if (NULL == inst->pFile)
    {
        inst->pFile = CsrBtFopen((const CsrCharString *)inst->fileName, "wb");
        /* [QTI] Fix KW issue#835028 through adding the check "inst->pFile". */
        if (NULL == inst->pFile)
        {
            printf("File error, could not open the file named: %s\n", inst->fileName);
            
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
                         inst->pFile);

    if (nWrite != pMsg->bodyLength)
    {
        printf("File error");
    }

    inst->nObjLen += pMsg->bodyLength;

    CsrBtPacPullPbResSend(inst->srmp);

    if (NULL != pMsg->payload)
    {
        CsrPmemFree(pMsg->payload);
        pMsg->payload = NULL;
    }
}

static void handleCsrBtPacPullPbCfm(CsrBtPacAppInstData_t *inst)
{
    CsrBtPacPullPbCfm *pMsg = inst->recvMsgP;
    CsrBtPacConnectCfm* prim;
    prim = (CsrBtPacConnectCfm*) inst->recvMsgP;

    if (CSR_BT_OBEX_SUCCESS_RESPONSE_CODE == pMsg->responseCode)
    {
        CsrBtPacAppUiPopupHide(inst);
        CsrBtPacAppDownloadedUiPopup(inst);
        if (NULL == inst->pFile)
        {
            inst->pFile = CsrBtFopen((const CsrCharString *)inst->fileName, "wb");
        }
    }
    else
    {
        CsrUiDialogSetReqSend(inst->ui.hCommonDialog,
                              NULL,
                              TEXT_PAC_APP_NOT_FOUND_UCS2,
                              TEXT_OK_UCS2,
                              NULL,
                              inst->ui.hSk1Event,
                              0,
                              0,
                              0);

        CsrUiUieShowReqSend(inst->ui.hCommonDialog,
                            CSR_BT_PAC_APP_IFACEQUEUE,
                            CSR_UI_INPUTMODE_AUTO,
                            CSR_BT_PAC_APP_CSR_UI_PRI);

        printf("Error %x", pMsg->responseCode);
    }

    if (NULL != inst->pFile)
    {
        CsrBtFclose(inst->pFile);
        inst->pFile = NULL;
    }
}

static void handlePullvCardListInd(CsrBtPacAppInstData_t *pInst)
{
    CsrBtPacPullVcardListInd *pMsg = pInst->recvMsgP;
    /* CsrBtPacAppUiInstData *ui = &pInst->ui; */
    CsrUint32 nWrite;

    CsrBtPacAppUiPopup(pInst, "Listing", "Please Wait");

    /* first indication, create file */
    if (NULL == pInst->pFile)
    {
        pInst->pFile = CsrBtFopen(CSR_BT_PAC_APP_FILE_LISTING_OUTPUT, "wb");

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
    /*Save the listing into a file.*/
    nWrite = CsrBtFwrite(pMsg->bodyOffset + pMsg->payload,
                         sizeof(CsrUint8),
                         pMsg->bodyLength,
                         pInst->pFile);

    if (nWrite != pMsg->bodyLength)
    {
        printf("File error, could not write to file!\n");
    }

    pInst->nObjLen += pMsg->bodyLength;

    CsrBtPacPullVcardListResSend(pInst->srmp);

    if (NULL != pMsg->payload)
    {
        CsrPmemFree(pMsg->payload);
        pMsg->payload = NULL;
    }
}

static void handlePullvCardListCfm(CsrBtPacAppInstData_t *inst)
{
    CsrBtPacPullVcardListCfm *pMsg = inst->recvMsgP;
    CsrBtPacAppUiInstData *ui = &inst->ui;

    CsrUint32 i = 0, k = 0, size, count;
    CsrUint8 string[MAX_FILE_LENGTH], entry[MAX_ENTRIES_LENGTH][MAX_ENTRIES_LENGTH];
    CsrUint8 handleValue[MAX_CARD_HANDLE_LENGTH];
    CsrUint8 *cardHandleTypeA, *cardHandleTypeB;

    inst->entryKey = 0;

    if (CSR_BT_OBEX_SUCCESS_RESPONSE_CODE == pMsg->responseCode)
    {
        CsrBtPacAppUiPopupHide(inst);   /*Hide the listing pop up.*/

        while (i != MAX_FILE_LENGTH)
        {
            string[i] = '\0';
            i++;
        }

        inst->pFile = CsrBtFopen(CSR_BT_PAC_APP_FILE_LISTING_OUTPUT, "r");
		/* [QTI] Fix KW issue#835029 through adding the check "inst->pFile". */
        if (NULL == inst->pFile)
        {
            printf("File error, could not open the file named: %s\n", CSR_BT_PAC_APP_FILE_LISTING_OUTPUT);
            return;
        }

        CsrUiUieShowReqSend(inst->ui.hVCardList,
                            CSR_BT_PAC_APP_IFACEQUEUE,
                            CSR_UI_INPUTMODE_AUTO,
                            CSR_BT_PAC_APP_CSR_UI_PRI);

        CsrUiMenuRemoveallitemsReqSend(ui->hVCardList);

        CsrBtFseek(inst->pFile, 0, CSR_SEEK_END); /*seek to end of file*/
        size = CsrBtFtell(inst->pFile); /*get current file pointer*/
        CsrBtFseek(inst->pFile, 0, CSR_SEEK_SET); /*seek back to beginning of file*/

        while (!CsrBtIsAtEof(inst->pFile, size))
        {
            if (CsrBtFgets((char *)string, MAX_FILE_LENGTH, inst->pFile) != NULL)
            {
                count = CsrStrLen((const CsrCharString *)string) + 1; /*length of the line*/
                while(k < count)
                {
                    /*First, search for card handle*/
                    cardHandleTypeA = (CsrUint8 *) CsrStrStr((const CsrCharString *)string, "<card handle = \"");
                    cardHandleTypeB = (CsrUint8 *) CsrStrStr((const CsrCharString *)string, "<card handle=\"");

                    if (cardHandleTypeA || cardHandleTypeB)
                    {/*card handle value found*/
                        k = 0;
                        if (cardHandleTypeA)
                        {/*card handle format type A*/
                            for(i=0; i < k+16; i++)
                                cardHandleTypeA[i] = '0';

                            k = i;
                            i = 0;
                            while (cardHandleTypeA[k] != '"')
                            {
                                handleValue[i] = cardHandleTypeA[k];
                                k++;
                                i++;
                            }
                            /*Then search for corresponding name if type A.*/
                            cardHandleTypeA = CsrStrStr((const CsrCharString *)cardHandleTypeA,
                                                        "name = \"");
                        }
                        else
                        {/*card handle format type B*/
                            /* [QTI] Fix KW issue#835031 through adding the check "cardHandleTypeB". */
                            if (cardHandleTypeB)
                            {
                                for(i=0; i < k+14; i++)
                                    cardHandleTypeB[i] = '0';
                                
                                k = i;
                                i = 0;
                                while (cardHandleTypeB[k] != '"')
                                {
                                    handleValue[i] = cardHandleTypeB[k];
                                    k++;
                                    i++;
                                }
                                /*Then search for corresponding name if type B.*/
                                cardHandleTypeB = CsrStrStr(cardHandleTypeB,
                                                            "name=\"");
                            }
                        }

                        handleValue[i++] = '\0';
                        inst->entryKey++;
                        k = 0;

                        /*Store the name value in an array "entry"*/
                        if (cardHandleTypeA)
                        {
                            for(i=0; i < k+8; i++)
                                cardHandleTypeA[i] = '0';

                            k = i;
                            i = 0;
                            while (cardHandleTypeA[k] != '"')
                            {
                                entry[inst->entryKey][i] = cardHandleTypeA[k];
                                i++;
                                k++;
                            }
                        }
                        else
                        {
                            /* [QTI] Fix KW issue#835032 through adding the check "cardHandleTypeB". */
                            if (cardHandleTypeB)
                            {
                                for(i=0; i < k+6; i++)
                                    cardHandleTypeB[i] = '0';
                                
                                k = i;
                                i = 0;
                                while (cardHandleTypeB[k] != '"')
                                {
                                    entry[inst->entryKey][i] = cardHandleTypeB[k];
                                    i++;
                                    k++;
                                }
                            }
                        }

                        entry[inst->entryKey][i++] = '\0';

                        /* [TODO] Check the 'size'. */
                        CsrStrLCpy(*(inst->cardNo + inst->entryKey),
                                  handleValue,
                                  MAX_ENTRIES_LENGTH);
                        /*Add the name as an item to vCardList UI.*/
                        CsrUiMenuAdditemReqSend(ui->hVCardList,
                                                CSR_UI_LAST,
                                                CSR_BT_PAC_APP_ENTRY_BY_UID + inst->entryKey,
                                                CSR_UI_ICON_NONE,
                                                CsrUtf82Utf16String((CsrUint8 * )
                                                (*(entry + inst->entryKey))),
                                                NULL,
                                                ui->hSk1Event,
                                                ui->hSk2Event,
                                                ui->hBackEvent,
                                                ui->hDelEvent);
                    }
                    else
                        /*If no card handle value found, break out of the
                         * loop and search for it in the next line.     */
                        break;
                }
            }
            else
            {
                CsrUiUieHideReqSend(ui->hVCardList);
            }
        }
    }
    else
    {
        CsrUiDialogSetReqSend(inst->ui.hCommonDialog,
                              NULL,
                              TEXT_PAC_APP_NOT_FOUND_UCS2,
                              TEXT_OK_UCS2,
                              NULL,
                              inst->ui.hSk1Event,
                              0,
                              0,
                              0);

        CsrUiUieShowReqSend(inst->ui.hCommonDialog,
                            CSR_BT_PAC_APP_IFACEQUEUE,
                            CSR_UI_INPUTMODE_AUTO,
                            CSR_BT_PAC_APP_CSR_UI_PRI);

        printf("Error occurred inside vCardListCfm 0x%04X\n\n",
               pMsg->responseCode);
    }

    if (NULL != inst->pFile)
    {
        CsrBtFclose(inst->pFile);
        inst->pFile = NULL;
    }
    CsrBtPacSetRootFolderReqSend();
}

static void handlePullvCardEntryInd(CsrBtPacAppInstData_t *inst)
{
    CsrBtPacPullVcardEntryInd *pMsg = inst->recvMsgP;
    CsrUint32 nWrite;

    /* first indication, create file */
    if (NULL == inst->pFile)
    {
        CsrUint8 *contact;
        CsrUint32 contactSize = MAX_CARD_HANDLE_LENGTH;
        contact = CsrPmemAlloc(contactSize);
        snprintf(contact, contactSize, "%d", inst->entrySubKey);
        CsrStrLCat(contact, ".vcf", contactSize);
        inst->pFile = CsrBtFopen(contact, "wb");

        /* [QTI] Fix KW issue#835030 through adding the check "inst->pFile". */
        if (NULL == inst->pFile)
        {
            printf("File error, could not open the contact named: %s\n", contact);
            
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
                         inst->pFile);

    if (nWrite != pMsg->bodyLength)
    { /* file error */
        printf("File error, could not write to file!\n");
    }

    inst->nObjLen += pMsg->bodyLength;

    CsrBtPacPullVcardEntryResSend(inst->srmp);

    if (NULL != pMsg->payload)
    {
        CsrPmemFree(pMsg->payload);
        pMsg->payload = NULL;
    }
}

static void handlePullvCardEntryCfm(CsrBtPacAppInstData_t *inst)
{
    CsrBtPacPullVcardEntryCfm *pMsg = inst->recvMsgP;

    if (CSR_BT_OBEX_SUCCESS_RESPONSE_CODE == pMsg->responseCode)
    {
        CsrBtPacAppDownloadedUiPopup(inst);
    }
    else
    {
        printf("Error occurred inside vCardEntry CFM 0x%04X\n\n", pMsg->responseCode);
    }

    if (NULL != inst->pFile)
    {
        CsrBtFclose(inst->pFile);
        inst->pFile = NULL;
    }
    CsrBtPacSetRootFolderReqSend();
}

static void handleSetFolderCfm(CsrBtPacAppInstData_t *inst)
{
    CsrBtPacSetFolderCfm *pMsg = inst->recvMsgP;
    CsrUcs2String *folder = NULL;

    if (CSR_BT_OBEX_SUCCESS_RESPONSE_CODE == pMsg->responseCode)
    {
        if (inst->state == BROWSING_STATE && inst->src == CSR_BT_PAC_SRC_PHONE
                                                && inst->startBrowsing == TRUE)
        {
            inst->startBrowsing = FALSE;

            CsrBtPacPullVcardListReqSendEx(CsrUtf82Ucs2ByteString(inst->name),
                                           inst->order, inst->pSearch,
                                           inst->searchAtt, inst->nMaxLstCnt,
                                           inst->startOffsetVal, inst->resetNewMissedCall,
                                           inst->vCardSelector,
                                           inst->vCardSelectorOperator,
                                           inst->srmp);
        }
        else if (inst->state == BROWSING_STATE && inst->src == CSR_BT_PAC_SRC_PHONE
                                                        && inst->listing == TRUE)
        {/*set to a folder from which entry needs to be downloaded*/
            folder = CsrBtPbGetFolderStr(CSR_BT_PB_FOLDER_TELECOM_ID,
                                         inst->entryFolder,
                                         FALSE);
            inst->state = DOWNLOAD_ENTRY_STATE;
            inst->listing = FALSE;

            CsrBtPacSetFolderReqSend(folder);
        }
        else if (inst->state == DOWNLOAD_ENTRY_STATE)
        {
            inst->state = BROWSING_STATE;
            if (inst->src == CSR_BT_PAC_SRC_PHONE)
            {/*Send the vCard Entry request.*/
                CsrBtPacPullVcardEntryReqSend(CsrUtf82Ucs2ByteString((CsrUint8 *)
                                              (*(inst->cardNo+ inst->entrySubKey))),
                                              inst->filter,
                                              inst->vCardFormat,
                                              inst->srmp);
            }
            else
            {/*set to a folder from which entry needs to be downloaded inside SIM*/
                folder = CsrBtPbGetFolderStr(CSR_BT_PB_FOLDER_SIM1_TELECOM_ID,
                                             inst->entryFolder,
                                             FALSE);
                inst->setEntrySim = TRUE;
                CsrBtPacSetFolderReqSend(folder);
            }
        }
        else if (inst->setEntrySim == TRUE)
        {/*Send the vCard Entry request for SIM memory.*/
            inst->setEntrySim = FALSE;
            CsrBtPacPullVcardEntryReqSend(CsrUtf82Ucs2ByteString((CsrUint8 * )
                                         (*(inst->cardNo + inst->entrySubKey))),
                                          inst->filter,
                                          inst->vCardFormat,
                                          inst->srmp);
        }
        /*Set the folder from SIM1 to SIM1/telecom*/
        else if (inst->state == BROWSING_STATE && inst->src == CSR_BT_PAC_SRC_SIM)
        {
            if (inst->listing == TRUE)
            {
                inst->state = DOWNLOAD_ENTRY_STATE;
                inst->listing = FALSE;
            }

            folder = CsrBtPbGetFolderStr(CSR_BT_PB_FOLDER_SIM1_ID,
                                         CSR_BT_PB_FOLDER_SIM1_TELECOM_ID,
                                         FALSE);

            CsrBtPacSetFolderReqSend(folder);
        }
        else if(inst->state == BROWSING_STATE && (inst->src == CSR_BT_PAC_SRC_SIM
                                            || inst->src == CSR_BT_PAC_SRC_PHONE))
            inst->browseContacts = TRUE;
    }
    /*If Sim is not supported by Server.*/
    else if(inst->src== CSR_BT_PAC_SRC_SIM)
    {
        ;/*do nothing.*/
    }
    else
    {
        CsrBtPacSetRootFolderReqSend();
        printf("Error occurred. (SetFolder) 0x%04X\n\n", pMsg->responseCode);
    }
}

static void handleSetFolderBackCfm(CsrBtPacAppInstData_t *inst)
{
    CsrBtPacSetFolderCfm *pMsg = inst->recvMsgP;

    if (CSR_BT_OBEX_SUCCESS_RESPONSE_CODE == pMsg->responseCode)
    {
        printf("Folder set to Back successful!\n\n");
    }
    else
    {
        printf("Error occurred inside SetBackFolderCfm 0x%04X\n\n",
               pMsg->responseCode);
    }
}

static void handleSetFolderRootCfm(CsrBtPacAppInstData_t *inst)
{
    CsrBtPacSetFolderCfm *pMsg = inst->recvMsgP;
    CsrUcs2String *folder = NULL;

    if (CSR_BT_OBEX_SUCCESS_RESPONSE_CODE == pMsg->responseCode)
    {
        if (inst->state == BROWSING_STATE)
        {
            if(inst->entryFromListing == TRUE)
            {
                inst->listing = TRUE;
                inst->entryFromListing = FALSE;
            }

            if(inst->src == CSR_BT_PAC_SRC_PHONE)
            {
                folder = CsrBtPbGetFolderStr(CSR_BT_PB_FOLDER_ROOT_ID,
                                             CSR_BT_PB_FOLDER_TELECOM_ID,
                                             FALSE);
            }
            else if(inst->src == CSR_BT_PAC_SRC_SIM)
            {
                folder = CsrBtPbGetFolderStr(CSR_BT_PB_FOLDER_ROOT_ID,
                                             CSR_BT_PB_FOLDER_SIM1_ID,
                                             FALSE);
            }

            CsrBtPacSetFolderReqSend(folder);
        }
        else if (inst->state == DOWNLOAD_STATE)
        {
            CsrBtPacPullPbReqSendEx(CsrUtf82Ucs2ByteString((CsrUint8* ) inst->name),
                                    inst->src, (CsrUint8* )inst->filter,
                                    inst->vCardFormat, inst->nMaxLstCnt,
                                    inst->startOffsetVal, inst->resetNewMissedCall,
                                    inst->vCardSelector,
                                    inst->vCardSelectorOperator, inst->srmp);
        }
        else
        {
            inst->state = DOWNLOAD_STATE;
            printf("Folder is set to Root.");
        }
    }
    else
    {
        printf("Error occurred. (SetRootFolder) 0x%04X\n\n",
               pMsg->responseCode);
    }
}

static void csrBtPacAppHandlePacPrim(CsrBtPacAppInstData_t *inst)
{
    CsrPrim *primType;
    primType = (CsrPrim *) inst->recvMsgP;

    switch (*primType)
    {

        case CSR_BT_PAC_CONNECT_CFM:
        {
            csrBtPacAppConnectCfmHandler(inst);
            break;
        }

        case CSR_BT_PAC_AUTHENTICATE_IND:
        {
            handlePacAuthInd(inst);
            break;
        }

        case CSR_BT_PAC_PULL_PB_IND:
        {
            handleCsrBtPacPullPbInd(inst);
            break;
        }

        case CSR_BT_PAC_PULL_PB_CFM:
        {
            handleCsrBtPacPullPbCfm(inst);
            break;
        }

        case CSR_BT_PAC_PULL_VCARD_LIST_CFM:
        {
            handlePullvCardListCfm(inst);
            break;
        }

        case CSR_BT_PAC_PULL_VCARD_LIST_IND:
        {
            handlePullvCardListInd(inst);
            break;
        }

        case CSR_BT_PAC_PULL_VCARD_ENTRY_CFM:
        {
            handlePullvCardEntryCfm(inst);
            break;
        }

        case CSR_BT_PAC_PULL_VCARD_ENTRY_IND:
        {
            handlePullvCardEntryInd(inst);
            break;
        }

        case CSR_BT_PAC_SET_FOLDER_CFM:
        {
            handleSetFolderCfm(inst);
            break;
        }

        case CSR_BT_PAC_SET_BACK_FOLDER_CFM:
        {
            handleSetFolderBackCfm(inst);
            break;
        }

        case CSR_BT_PAC_SET_ROOT_FOLDER_CFM:
        {
            handleSetFolderRootCfm(inst);
            break;
        }

        case CSR_BT_PAC_ABORT_CFM:
        {
            handlePacAbort(inst);
            break;
        }

        case CSR_BT_PAC_DISCONNECT_IND:
        {
            csrBtPacAppDisconnectIndHandler(inst);
            break;
        }

        default:
        {
            /* unexpected primitive received */
            CsrAppBacklogReqSend(TECH_BT,
                                 PROFILE_NAME(PAC_PROFILE_NAME),
                                 TRUE,
                                 "Unknown Pac prim - 0x%04x,",
                                 *primType);
            CsrBtPacFreeUpstreamMessageContents(CSR_BT_PAC_PRIM,
                                                inst->recvMsgP);
            break;
        }

    }
}

void CsrBtPacAppInit(void **gash)
{
    CsrBtPacAppInstData_t *inst;
    *gash = CsrPmemZalloc(sizeof(CsrBtPacAppInstData_t));
    inst = (CsrBtPacAppInstData_t *) *gash;

    csrBtPacAppResetInstance(inst);

    CsrAppRegisterReqSend(CSR_BT_PAC_APP_IFACEQUEUE,
                          TECH_BT,
                          PROFILE_NAME(PAC_PROFILE_NAME));

    CsrUiUieCreateReqSend(CSR_BT_PAC_APP_IFACEQUEUE,
                          CSR_UI_UIETYPE_EVENT);

}

void CsrBtPacAppDeinit(void **gash)
{
#ifdef ENABLE_SHUTDOWN
    CsrBtPacAppInstData_t *inst;
    CsrUint16 msg_type = 0;
    void *msg_data = NULL;

    inst = (CsrBtPacAppInstData_t *) (*gash);

    while (CsrSchedMessageGet(&msg_type, &msg_data))
    {
        switch (msg_type)
        {
            case CSR_BT_PAC_PRIM:
            {
                CsrBtPacFreeUpstreamMessageContents(msg_type, msg_data);
                break;
            }
        }
        CsrPmemFree(msg_data);
    }

    CsrPmemFree(inst);
#else
    {
        CSR_UNUSED(gash);
    }
#endif
}

void CsrBtPacAppHandler(void **gash)
{
    CsrBtPacAppInstData_t *inst;
    CsrUint16 eventType = 0;

    inst = (CsrBtPacAppInstData_t *) (*gash);

    CsrSchedMessageGet(&eventType, &inst->recvMsgP);

    switch (eventType)
    {

        case CSR_BT_PAC_PRIM:
        {
            csrBtPacAppHandlePacPrim(inst);
            break;
        }

        case CSR_BT_GAP_APP_PRIM:
        {
            CsrPrim *prim = (CsrPrim *) inst->recvMsgP;
            if (*prim == CSR_BT_GAP_APP_GET_SELECTED_DEVICE_CFM)
            {
                CsrBtGapAppGetSelectedDeviceCfm *prim = inst->recvMsgP;
                inst->selectedDeviceAddr = prim->deviceAddr;

                CsrAppBacklogReqSend(TECH_BT,
                                     PROFILE_NAME(PAC_PROFILE_NAME),
                                     FALSE,
                                     "Received selected DeviceAddr: %04X:%02X:%06X",
                                     prim->deviceAddr.nap,
                                     prim->deviceAddr.uap,
                                     prim->deviceAddr.lap);
                CsrUiUieShowReqSend(inst->ui.hMainMenu,
                                    CSR_BT_PAC_APP_IFACEQUEUE,
                                    CSR_UI_INPUTMODE_AUTO,
                                    CSR_BT_PAC_APP_CSR_UI_PRI);
            }
            else
            {
                CsrGeneralException("CSR_BT_PAC_APP",
                                    0,
                                    *prim,
                                    "####### Unhandled CSR_BT_GAP_APP_PRIM in CsrBtPacAppHandler,");
            }

            break;
        }

        case CSR_APP_PRIM:
        {
            CsrPrim *prim = (CsrPrim *) inst->recvMsgP;

            if (*prim == CSR_APP_TAKE_CONTROL_IND)
            {
                CsrBtGapAppGetSelectedDeviceReqSend(CSR_BT_PAC_APP_IFACEQUEUE);
            }
            else if (*prim == CSR_APP_REGISTER_CFM)
            {
                ;
            }
            else
            {
                CsrGeneralException("CSR_BT_PAC_APP",
                                    0,
                                    *prim,
                                    "####### Unhandled CSR_APP_PRIM in CsrBtPacAppHandler,");
            }
            break;
        }

        case CSR_UI_PRIM:
        {
            CsrBtPacAppHandleUiPrim(inst);
            break;
        }

        default:
        {
            CsrGeneralException("CSR_BT_PAC_APP",
                                0,
                                *((CsrPrim * ) inst->recvMsgP),
                                "####### default in CsrBtPacAppHandler,");
            break;
        }

    }
    /* free the received signal again. If the signal for some reason must be stored in the application
     * the pointer (prim) must be set to NULL in order not to free it here
     */
    CsrPmemFree(inst->recvMsgP);
}
