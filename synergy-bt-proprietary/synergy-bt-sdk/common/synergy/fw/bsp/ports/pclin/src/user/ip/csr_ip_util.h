#ifndef CSR_IP_UTIL_H__
#define CSR_IP_UTIL_H__
/*****************************************************************************

Copyright (c) 2010-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include <sys/select.h>

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_sched.h"
#include "csr_pmem.h"
#include "csr_prim_defs.h"

#include "csr_ip_handler.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    void       *message;
    CsrPrim     prim;
    CsrSchedQid queue;
} csrIpMessageContainer;

typedef CsrCharString CsrIpThreadCmd;
#define THREAD_CMD_TERMINATE         ((CsrIpThreadCmd) 't')
#define THREAD_CMD_RESCHEDULE        ((CsrIpThreadCmd) 'r')
#define THREAD_CMD_REAP       ((CsrIpThreadCmd) 's')
#ifdef CSR_IP_SUPPORT_IFCONFIG
#define THREAD_CMD_IFCONFIG_DEFERRED ((CsrIpThreadCmd) 'd')
#endif

void CsrIpCommand(void *arg, CsrIpThreadCmd cmd);
void CsrIpLock(void *arg);
void CsrIpUnlock(void *arg);

void CsrIpFdNew(void *, int);
void CsrIpFdAdd(void *, int, CsrBool, CsrBool);
void CsrIpFdRemove(void *, int, CsrBool, CsrBool);
void CsrIpMessageForward(CsrSchedQid, CsrPrim, void *);

#ifdef __cplusplus
}
#endif

#endif
