#ifndef CSR_BT_PPP_LIB_H__
#define CSR_BT_PPP_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_bt_ppp_prim.h"
#include "csr_pmem.h"
#include "csr_bt_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void PppConnectReqSend(CsrSchedQid applicationQueue, CsrBtDeviceAddr deviceAddr);
extern void PppRegisterNetworkProtocolReqSend(CsrSchedQid responseQueue, CsrUint16 ncpProtocol, CsrSchedQid dataQueue, CsrUint16 dataProtocol);
extern void PppNcpReqSend(CsrUint16 ncpProtocol, CsrUint16 length, CsrUint8 * payload);
extern void PppDisconnectReqSend(void);
extern void PppAuthenticateResSend(CsrUint8 * loginName, CsrUint8 * password);
#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_PPP_LIB_H__ */
