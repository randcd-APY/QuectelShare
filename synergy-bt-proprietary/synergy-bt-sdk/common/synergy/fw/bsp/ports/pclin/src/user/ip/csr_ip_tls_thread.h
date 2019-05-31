#ifndef CSR_IP_TLS_THREAD_H__
#define CSR_IP_TLS_THREAD_H__

#include "csr_synergy.h"

/*****************************************************************************

Copyright (c) 2011-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_sched.h"

#include "csr_ip_tls_handler.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CSR_TLS_TYPE_TCP_CONNECT      0x0000
#define CSR_TLS_TYPE_TCP_SSL_CONNECT  0x0001
#define CSR_TLS_TYPE_TCP_DATA         0x0002

#ifdef __cplusplus
}
#endif

#endif
