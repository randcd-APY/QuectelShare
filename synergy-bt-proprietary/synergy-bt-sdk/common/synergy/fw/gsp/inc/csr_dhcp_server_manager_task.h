#ifndef CSR_DHCP_SERVER_MANAGER_TASK_H__
#define CSR_DHCP_SERVER_MANAGER_TASK_H__

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

/* Queue definition for CSR_DHCP_SERVER_MANAGER */
extern CsrSchedQid CSR_DHCP_SERVER_MANAGER_IFACEQUEUE;

/* Task definition for CSR_DHCP_SERVER_MANAGER */
void CsrDhcpServerManagerInit(void **gash);
void CsrDhcpServerManagerDeinit(void **gash);
void CsrDhcpServerManagerHandler(void **gash);

#define CSR_DHCP_SERVER_MANAGER_INIT      CsrDhcpServerManagerInit
#define CSR_DHCP_SERVER_MANAGER_HANDLER   CsrDhcpServerManagerHandler
#ifdef ENABLE_SHUTDOWN
#define CSR_DHCP_SERVER_MANAGER_DEINIT    CsrDhcpServerManagerDeinit
#else
#define CSR_DHCP_SERVER_MANAGER_DEINIT    NULL
#endif

#ifdef __cplusplus
}
#endif

#endif
