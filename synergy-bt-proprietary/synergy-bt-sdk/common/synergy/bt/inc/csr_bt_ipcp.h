#ifndef CSR_BT_IPCP_H__
#define CSR_BT_IPCP_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_pmem.h"
#include "csr_sched.h"
#include "csr_bt_util.h"
#include "csr_bt_ppp_lib.h"
#include "csr_bt_internet_primitives.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CSR_BT_NCP_STATE_INITIAL            0
#define CSR_BT_NCP_STATE_STARTING            1
#define CSR_BT_NCP_STATE_STOPPING            2
#define CSR_BT_NCP_STATE_REQ_SENT            3
#define CSR_BT_NCP_STATE_ACK_RCVD            4
#define CSR_BT_NCP_STATE_ACK_SENT            5
#define CSR_BT_NCP_STATE_OPENED            6

/* protocol ids */
#define CSR_BT_IPCP_NCP_PROTOCOL            0x8021
#define CSR_BT_IPCP_IP_PROTOCOL            0x0021

#define CSR_BT_MAX_KEYBOARD_PING_ADDRESS_BUFFER_SIZE    15


typedef struct
{
    CsrUint8 currentIdentifier;
    CsrUint16 maxPayloadLength;
    CsrBool configureIpAddress;
    CsrUint8 assignedIpAddress[4];
    CsrUint8 ncpState;
    CsrSchedTid ncpTimer;
    CsrUint8 peerIpAddress[4];
    void * receivedMessage;
    CsrUint16 macAddress[3];
    void * demoAppInstanceData;
} CsrBtNcpInstanceData;

extern void CsrBtHandleNcpInd(CsrBtNcpInstanceData * instanceData);
extern void CsrBtStartIpCp(CsrBtNcpInstanceData * instanceData);
extern void CsrBtNcpHandleDisconnectInd(CsrBtNcpInstanceData * instanceData);

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_IPCP_H__ */
