#ifndef CSR_ETHERNET_MANAGER_TASK_H__
#define CSR_ETHERNET_MANAGER_TASK_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_sched.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Queue definition for CSR_ETHERNET_MANAGER */
extern CsrSchedQid CSR_ETHERNET_MANAGER_IFACEQUEUE;

/* Task definition for CSR_EXAMPLE_NETMGR */
void CsrEthernetManagerInit(void **gash);
void CsrEthernetManagerDeinit(void **gash);
void CsrEthernetManagerHandler(void **gash);

#define CSR_ETHERNET_MANAGER_INIT      CsrEthernetManagerInit
#define CSR_ETHERNET_MANAGER_HANDLER   CsrEthernetManagerHandler
#ifdef ENABLE_SHUTDOWN
#define CSR_ETHERNET_MANAGER_DEINIT    CsrEthernetManagerDeinit
#else
#define CSR_ETHERNET_MANAGER_DEINIT    NULL
#endif

#ifdef __cplusplus
}
#endif

#endif
