#ifndef CSR_IP_TASK_H__
#define CSR_IP_TASK_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_sched.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Queue definition for CSR_IP */
extern CsrSchedQid CSR_IP_IFACEQUEUE;
#define CSR_IP_ETHER_IFACEQUEUE    (CSR_IP_IFACEQUEUE)
#define CSR_IP_SOCKET_IFACEQUEUE   (CSR_IP_IFACEQUEUE)
#define CSR_IP_IFCONFIG_IFACEQUEUE (CSR_IP_IFACEQUEUE)

/* Task definition for CSR_IP */
void CsrIpInit(void **gash);
void CsrIpDeinit(void **gash);
void CsrIpHandler(void **gash);

/* CSR_IP */
#define CSR_IP_INIT CsrIpInit
#define CSR_IP_HANDLER CsrIpHandler
#ifdef ENABLE_SHUTDOWN
#define CSR_IP_DEINIT CsrIpDeinit
#else
#define CSR_IP_DEINIT NULL
#endif


#ifdef __cplusplus
}
#endif

#endif
