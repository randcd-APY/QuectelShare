#ifndef CSR_BT_PAC_APP_H__
#define CSR_BT_PAC_APP_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2014-2015 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "csr_types.h"
#include "csr_ui_lib.h"
#include "csr_bt_profiles.h"
#include "csr_message_queue.h"
#include "csr_file.h"
#include "csr_bt_ui_strings.h"
#include "csr_bt_demoapp.h"
#include "csr_bt_platform.h"
#include "csr_bt_sc_demo_app.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define CSR_BT_PAC_APP_CSR_UI_PRI           (1)

#define MAX_QUERY_LEN                       (512)
#define MAX_ADDRESS_LEN                     (32)
#define CSR_BT_MAX_LENGTH                   20
#define DEFAULT_MAX_LIST_COUNT              0xFFFF  /* 25 */
#define DEFAULT_VCARD_FORMAT                CSR_BT_PAC_FORMAT_VCARD3_0
#define MAX_CARD_HANDLE_LENGTH              10
#define PAC_MAX_OBEX_PACKET_SIZE            (0x1f00)
#define PB_DEFAULT_FOLDER_NAME              "folder"
#define MAX_QUERY_LEN                       (512)
#define PAC_PROFILE_NAME                    ("PBAP Client")
#define MAX_SEARCH                          50
#define MAX_LINE_LENGTH                     1000
#define MAX_FILE_LENGTH                     10000
#define MAX_ENTRIES_LENGTH                  500
#define UID_TYPE                            "X-BT-UID:"
#define UNIQUE_ID_LENGTH                    32
#define UID_TYPE_LEN                        (sizeof(UID_TYPE)-1) /* "X-BT-UID:" = 9 */
#define UID_LENGTH                          (UID_TYPE_LEN+UNIQUE_ID_LENGTH+1)/* +1 for \0 */

typedef CsrUint8 appState;
#define IDLE_STATE                          (appState) 0x00
#define CONNECT_STATE                       (appState) 0x01
#define AUTHENTICATE_STATE                  (appState) 0x02
#define BROWSING_STATE                      (appState) 0x03
#define DOWNLOAD_STATE                      (appState) 0x04
#define DOWNLOAD_ENTRY_STATE                (appState) 0x05

/* [QTI] Comment to solve warning: redefinition of typedef 'CsrBtPacPrim' [-Wpedantic]. */
/* typedef CsrPrim CsrBtPacPrim; */

typedef struct
{
    CsrUieHandle    hMainMenu;
    CsrUieHandle    hSettingsMenu;
    CsrUieHandle    hPhonebookMenu;
    CsrUieHandle    hSimMenu;
    CsrUieHandle    hBackEvent;
    CsrUieHandle    hSk1Event;
    CsrUieHandle    hSk2Event;
    CsrUieHandle    hDelEvent;
    CsrUieHandle    hCommonDialog;
    CsrUieHandle    hInputDialog;
    CsrUieHandle    hInputSk1Event;
    CsrUieHandle    hVCardFormat;
    CsrUieHandle    hOrder;
    CsrUieHandle    hSearch;
    CsrUieHandle    hSearchByProperty;
    CsrUieHandle    hPrSelector;
    CsrUieHandle    hMaxListCount;
    CsrUieHandle    hStartOffset;
    CsrUieHandle    hResetNewMissedCalls;
    CsrUieHandle    hVCardFilter;
    CsrUieHandle    hSrmp;
    CsrUieHandle    hDefault;
    CsrUieHandle    hVCardSel;
    CsrUieHandle    hVCardSelOp;
    CsrUieHandle    hVCardList;
    CsrUieHandle    password;
    CsrUieHandle    userId;
    CsrUieHandle    entryByUid;
    CsrBool         popup;
} CsrBtPacAppUiInstData;

/* CSR_BT_PAC instance data */

typedef struct
{
    void                    *recvMsgP;
    void                    (*srmpHandler)(void *);
    CsrBtPacAppUiInstData   ui;
    CsrBtConnId             btConnId;
    CsrBtDeviceAddr         selectedDeviceAddr;
    CsrCharString           remoteDeviceName[MAX_ADDRESS_LEN];
    appState                state;
    CsrFileHandle           *pFile;
    CsrBtPacFormatType      vCardFormat;
    CsrBtPacSrcType         src;
    CsrBtPacOrderType       order;
    CsrBtPacSearchAtt       searchAtt;
    CsrUint32               nObjLen;
    CsrUint32               entryKey;
    CsrUint32               entrySubKey;
    CsrUint32               value;/*to store vCard Selector value*/
    CsrUint32               valueFilter;/*to store Property selector value*/
    CsrUint16               nMaxLstCnt;
    CsrUint16               startOffsetVal;
    CsrUint16               event;
    CsrUint16               entryFolder;/*to set folder to pb,mch,ich,och,cch,spd,fav*/
    CsrUint8                fileName[CSR_BT_MAX_LENGTH];
    CsrUint8                *inputtext;
    CsrUint8                name[CSR_BT_MAX_LENGTH];
    CsrUint8                search[MAX_SEARCH];
    CsrUint8                *pSearch;
    CsrUint8                filter[8];
    CsrUint8                resetNewMissedCall;
    CsrUint8                vCardSelector[8];
    CsrUint8                vCardSelectorOperator;
    CsrUint8                cardNo[MAX_ENTRIES_LENGTH][MAX_ENTRIES_LENGTH];
    CsrUint8                *userId;
    CsrUint8                UIDchar[UID_LENGTH];
    CsrBool                 connecting;
    CsrBool                 maxListCount;
    CsrBool                 srmp;
    CsrBool                 enterUserId;
    CsrBool                 enterPassword;
    CsrBool                 downloadedPopUp;/*pop up appears when download is complete*/
    CsrBool                 startBrowsing;
    CsrBool                 browseContacts;
    CsrBool                 listing;
    CsrBool                 entryFromListing;/*To download an entry from listing output*/
    CsrBool                 setEntrySim;
    CsrBool                 entryByUid; /*Download an entry by entering its UID*/
    CsrBool                 startOffset;
} CsrBtPacAppInstData_t;

#define PROFILE_NAME(name) CsrUtf8StrDup((CsrUtf8String *) name)

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_PAC_APP_H__ */
