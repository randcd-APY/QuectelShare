#ifndef CSR_IP_ETHER_SEF_H__
#define CSR_IP_ETHER_SEF_H__
/*****************************************************************************

Copyright (c) 2011-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_ip_handler.h"

#ifdef __cplusplus
extern "C" {
#endif

void csrIpEtherHandleEthers(CsrIpInstanceData *, int *, fd_set *);
void csrIpEtherRemove(CsrIpInstanceData *, CsrUint32);
void csrIpEtherFree(CsrIpInstanceData *, CsrUint32);

void CsrIpEtherIfAddReqHandler(CsrIpInstanceData *instanceData);
void CsrIpEtherIfRemoveReqHandler(CsrIpInstanceData *instanceData);
void CsrIpEtherIfLinkUpReqHandler(CsrIpInstanceData *instanceData);
void CsrIpEtherIfLinkDownReqHandler(CsrIpInstanceData *instanceData);
void CsrIpEtherIfFlowControlPauseReqHandler(CsrIpInstanceData *instanceData);
void CsrIpEtherIfFlowControlResumeReqHandler(CsrIpInstanceData *instanceData);

#ifdef __cplusplus
}
#endif

#endif
