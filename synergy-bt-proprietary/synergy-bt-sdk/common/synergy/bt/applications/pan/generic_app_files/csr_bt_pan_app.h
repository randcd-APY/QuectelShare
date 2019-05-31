#ifndef CSR_BT_PAN_APP_H__
#define CSR_BT_PAN_APP_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2014 Qualcomm Technologies International, Ltd. 
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
#include "csr_pmem.h"
#include "csr_ip_ifconfig_prim.h"
#include "csr_bt_ip_prim.h"
#include "csr_ip_ether_prim.h"
#include "csr_ip_ether_lib.h"
#include "csr_bt_util.h"
#include "csr_bt_bsl_lib.h"
#include "csr_bt_cm_lib.h"


#ifdef __cplusplus
extern "C" {
#endif
extern CsrUint16 CSR_BT_PAN_APP_IFACEQUEUE;
#define CSR_BT_PAN_APP_CSR_UI_PRI 1

#define PROFILE_PAN CsrUtf8StrDup((const CsrUtf8String *) "PAN")
#define CSR_BT_PAN_NO_ROLE                 ((CsrBtBslPanRole) (0x00) )
#define CSR_BT_PAN_NAP_ROLE                ((CsrBtBslPanRole) (0x01) )
#define CSR_BT_PAN_GN_ROLE                 ((CsrBtBslPanRole) (0x02) )
#define CSR_BT_PAN_PANU_ROLE               ((CsrBtBslPanRole) (0x04) )

#define DEFAULT_PAN_ROLE                    CSR_BT_PAN_NAP_ROLE

/* Enable the below define to send-receive data at BSL API level
  * with no Ether IP on top of it */
/* #define CSR_BT_BSL_RAW_DATA_THROUGHPUT */


/* Misc settings */
#define MAX_MULTICAST_ADDRESSES                 8
#define MAX_ETHER_FRAME_SIZE                    1522
#define MAX_RX_DATA_QUEUE                       255
#define MAX_TX_DATA_QUEUE                       (01 << 10)
#define MAX_TX_DATA_QUEUE_MASK            (MAX_TX_DATA_QUEUE - 1)


/* Default IP address */
#define DEFAULT_IP_ADDRESS_0                    0xA8C0
#define DEFAULT_IP_ADDRESS_1                    0x2088
#define DEFAULT_SUBNET_MASK_0                   0xFFFF
#define DEFAULT_SUBNET_MASK_1                   0x00FF
#define DEFAULT_GATEWAY_IP_ADDRESS_0            0xA8C0
#define DEFAULT_GATEWAY_IP_ADDRESS_1            0xFE88


typedef struct
{
    CsrUieHandle hMainMenu;
    CsrUieHandle hBackEvent;
    CsrUieHandle hBackEvent1;
    CsrUieHandle hBackEventforConnectOptions;
    CsrUieHandle hSk1Event;
    CsrUieHandle hSelectcommonDialogSk1Event;
    CsrUieHandle hSelectcommonDialogBackEvent;
    CsrUieHandle hSk2Event;
    CsrUieHandle hSk2Event1;
    CsrUieHandle hSk2EventforConnectOptions;
    CsrUieHandle hSelectRoleInstanceMenu;
    CsrUieHandle hCommonDialog;
    CsrUieHandle hCommonDialogForwaiting;
    CsrBool localInitiatedAction;
} CsrBtPanAppUiInstData;

typedef struct
{
    CsrSchedQid panAppHandle;
    CsrSchedQid panProfileHandle;
} CsrBtPanAppProfileHandleList;

/* CSR_BT_PAN instance data */
typedef struct
{
    /* Only for the controling pan demo app to assign profile queueIds to the 
    extra pan demo app instances */
    CsrUint8 numberOfPanInstances;
    CsrBtPanAppProfileHandleList *appHandles;
    CsrMessageQueueType * saveQueue;
    CsrBool restoreFlag;
    CsrBool initialized;
    CsrSchedQid phandle; /* pan demo apps own phandle */
    CsrSchedQid queueId; /* pan profile queueId that this demo app instance talks to */
    CsrBtPanAppUiInstData csrUiVar;
    void *recvMsgP;
    CsrBtDeviceAddr selectedDeviceAddr;
    CsrUint16 id;
    CsrBool connected;
    CsrBtBslPanRole role;
    CsrUint16 macAddress[3];
    CsrUint16 ipAddress[2];
    CsrUint16 subnetMask[2];
    CsrUint16 gatewayIpAddress[2];
    ETHER_ADDR multicastAddresses[MAX_MULTICAST_ADDRESSES];
    CsrUint16 multicastAddressesCount;
    CsrUint32 ifHandle;
    CsrIpEtherFrameRxFunction frameRxFunction;
    CsrIpEtherIpContext ipContext;
    CsrIpEtherEncapsulation encapsulation;
    CsrBool linkUp;
    CsrBtBslDataInd rxQ[MAX_RX_DATA_QUEUE];
    CsrUint8 rxQSize;
    #ifndef EXCLUDE_CSR_BT_BSL_FLOW_CONTROL_FEATURE
    CsrUint8 outstandingDataReq;
    #endif /* EXCLUDE_CSR_BT_BSL_FLOW_CONTROL_FEATURE */
    CsrBool  flowPaused;
    CsrBtBslDataReq txQ[MAX_TX_DATA_QUEUE];
    CsrUint16 txQRdIdx;
    CsrUint16 txQWrIdx;
    CsrBool  txQFull;
    CsrBtBslPanRole localRole;
} CsrBtPanAppGlobalInstData;

void CsrBtPanAppStartActivate(CsrBtPanAppGlobalInstData *panData);
void CsrBtPanAppStartConnecting(CsrBtPanAppGlobalInstData *panData,CsrUint16 remoteRole);
void CsrBtPanAppStartDisconnect(CsrBtPanAppGlobalInstData *panData);
void CsrBtPanAppStartSelect(CsrBtPanAppGlobalInstData *panData);
void etherAddHandler(CsrBtPanAppGlobalInstData* instanceData);
void etherRemoveHandler(CsrBtPanAppGlobalInstData* instanceData);
void etherLinkUpHandler(CsrBtPanAppGlobalInstData* instanceData);
void etherLinkDownHandler(CsrBtPanAppGlobalInstData* instanceData);
void etherMulticastAddrAddHandler(CsrBtPanAppGlobalInstData* instanceData);
void etherMulticastAddrRemoveHandler(CsrBtPanAppGlobalInstData* instanceData);
void etherMulticastAddrFlushHandler(CsrBtPanAppGlobalInstData* instanceData);
CsrResult frameTxFunction(CsrUint8 *destinationMac, CsrUint8 *sourceMac, CsrUint16 frameLength, CsrUint8 *frame,
        CsrIpEtherIfContext ifContext);
void passToEther(CsrBtPanAppGlobalInstData* instanceData, CsrBtBslDataInd *dataInd);
void releaseRxQ(CsrBtPanAppGlobalInstData* instanceData);
void CsrBtPanAppAddEherIf(CsrBtPanAppGlobalInstData* instanceData);
void bslDataHandler(CsrBtPanAppGlobalInstData* instanceData);
void bslDisconnectHandler(CsrBtPanAppGlobalInstData* instanceData);
void bslDataCfmHandler(CsrBtPanAppGlobalInstData* instanceData);
#if defined(USE_MDM_PLATFORM)|| defined(USE_IMX_PLATFORM)
void configNetwork(CsrBtPanAppGlobalInstData* instanceData, CsrBool start);
#endif

#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_PAN_APP_H__ */

