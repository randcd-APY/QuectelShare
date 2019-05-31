#ifndef CSR_SSD_LIB_H__
#define CSR_SSD_LIB_H__
/*****************************************************************************

Copyright (c) 2014-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/
#include "csr_synergy.h"
#include "csr_ssd_prim.h"
#include "csr_ssd_task.h"
#include "csr_msg_transport.h"

#ifdef __cplusplus
extern "C" {
#endif

extern CsrSchedQid CSR_SSD_IFACEQUEUE;

CsrSsdBtssShutdownReq *CsrSsdBtssShutdownReq_struct();

#define CsrSsdBtssShutdownReqSend() { \
    CsrSsdBtssShutdownReq *_msg; \
    _msg = CsrSsdBtssShutdownReq_struct(); \
    CsrMsgTransport(CSR_SSD_IFACEQUEUE, CSR_SSD_PRIM, _msg); }


#ifdef __cplusplus
}
#endif

#endif
