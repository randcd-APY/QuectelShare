#ifndef CSR_BT_RSCC_APP_H__
#define CSR_BT_RSCC_APP_H__
/****************************************************************************

Copyright (c) 2012-2013 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "csr_list.h"
#include "csr_types.h"
#include "csr_ui_lib.h"
#include "csr_bt_profiles.h"
#include "csr_message_queue.h"
#include "csr_file.h"
#include "csr_bt_ui_strings.h"
#include "csr_bt_demoapp.h"
#include "csr_bt_platform.h"
#include "csr_bt_gatt_demo_db_utils.h"
#ifdef __cplusplus
extern "C" {
#endif

#define CSR_BT_RSC_TRANSPORT_LE     FALSE
#define CSR_BT_RSC_TRANSPORT_BREDR  TRUE

#define CSR_BT_RSCC_APP_CSR_UI_PRI 1

#define CSR_BT_RSCC_APP_RSSI_TIMER_INTERVAL      1000000
#define CSR_BT_RSCC_APP_MAX_INPUT_TEXT_LENGTH    11
#define CSR_BT_RSCC_POPUPMSG_LEN                 27

#define CSR_BT_RSCC_WRITE_OPCODE_RFU    0x00
#define CSR_BT_RSCC_WRITE_OPCODE_STD    0x01
#define CSR_BT_RSCC_WRITE_OPCODE_SSC    0x02
#define CSR_BT_RSCC_WRITE_OPCODE_USL    0x03
#define CSR_BT_RSCC_WRITE_OPCODE_RSSL   0x04

#define CSR_BT_RSCC_WRITE_OPCODE_RC     0x10

#define CSR_BT_RSCC_WRITE_OPCODE_LENGHT         1
#define CSR_BT_RSCC_WRITE_OPCODE_STD_LENGTH     CSR_BT_RSCC_WRITE_OPCODE_LENGHT + 4 
#define CSR_BT_RSCC_WRITE_OPCODE_SSC_LENGTH     CSR_BT_RSCC_WRITE_OPCODE_LENGHT
#define CSR_BT_RSCC_WRITE_OPCODE_USL_LENGTH     CSR_BT_RSCC_WRITE_OPCODE_LENGHT + 1 
#define CSR_BT_RSCC_WRITE_OPCODE_RSSL_LENGTH    CSR_BT_RSCC_WRITE_OPCODE_LENGHT

typedef struct
{
    CsrUieHandle hMainMenu;
    CsrUieHandle hSCControlPointMenu;
    CsrUieHandle hBackEvent;
    CsrUieHandle hSk1Event;
    CsrUieHandle hSk2Event;
    CsrUieHandle hSelectFileInputSk1Event;
    CsrUieHandle hSelectComInstanceMenu;
    CsrUieHandle hSelectFileInputDialog;
    CsrUieHandle hInputDialog;
    CsrUieHandle hInputSk1Event;
    CsrUieHandle hCommonDialog;
    CsrBool      localInitiatedAction;
} CsrBtRsccAppUiInstData;


/* CSR_BT_RSCC instance data */
typedef struct
{
    char                                *profileName;

    CsrSchedQid                         rsccAppHandle;
    CsrSchedQid                         rsccProfileHandle;
    CsrMessageQueueType                 *saveQueue;
    CsrBool                             restoreFlag;
    CsrBool                             initialized;

    CsrSchedQid                         phandle;                /* proximity client demo apps own phandle                   */
    CsrBtGattId                         gattId;                 /* gattId that this demo app identifies with                */

    CsrBtRsccAppUiInstData              csrUiVar;
    void                                *recvMsgP;
    CsrBtTypedAddr                      selectedDeviceAddr;
    CsrBtTypedAddr                      connectedDeviceAddr;
    CsrBtConnId                         btConnId;
    CsrUint32                           selectedInfo;
    CsrBool                             transport;              /* using LE (0) or BREDR (1) */
    
    /*database values and handles (local copy) */
    CsrCmnList_t                        dbPrimServices;
    CsrUint16                           dbPrimServicesCount;
    CsrBtGattAppDbPrimServiceElement    *currentPrimService;    /* used for when we treverse the database during connect */
    CsrBtGattAppDbCharacElement         *currentCharac;         /* used for when we treverse the database during connect */
    /* for values we need to write */
    CsrUint16                           scControlPointHandle;
    CsrUint8                            *writeValue;
    CsrUint16                           writeValueLength;
    CsrUint8                            rssiLevel;
    CsrBool                             rscNotif;
    CsrBool                             rscInd;
    CsrBool                             rscWriteCC;
    CsrBool                             rscScWriteReq;
    CsrBool                             traversingDb;
} CsrBtRsccAppInstData;

void CsrBtRsccAppStartRegister(CsrBtRsccAppInstData *rsccData);
void CsrBtRsccAppStartUnregister(CsrBtRsccAppInstData *rsccData);
void CsrBtRsccAppStartConnecting(CsrBtRsccAppInstData *rsccData);
void CsrBtRsccAppStartDisconnect(CsrBtRsccAppInstData *rsccData);
void CsrBtRsccAppStartDeactivate(CsrBtRsccAppInstData *rsccData);
void CsrBtRsccAppStartReadUuid(CsrBtRsccAppInstData *rsccData, CsrUint16 uuid);
void CsrBtRsccAppStartReadValue(CsrBtRsccAppInstData *rsccData, CsrUint16 pUuid, CsrUint16 cUuid);
void CsrBtRsccAppStartWriteValue(CsrBtRsccAppInstData *rsccData, CsrUint8 type, CsrUint16 valueLength, CsrUint8* value);
void CsrBtRsccAppStartWriteClientConfig(CsrBtRsccAppInstData *rsccData, CsrUint16 pUuid, CsrUint16 cUuid, CsrUint16 value);
void CsrBtRsccAppStartReadCharacUuid(CsrBtRsccAppInstData *rsccData, CsrUint16 uuid);

CsrBtUuid *CsrBtRsccAppUuid16ToUuid128(CsrUint16 uuid);

#define PROFILE_NAME(name) CsrUtf8StrDup((CsrUtf8String *) name)

#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_RSCC_APP_H__ */

