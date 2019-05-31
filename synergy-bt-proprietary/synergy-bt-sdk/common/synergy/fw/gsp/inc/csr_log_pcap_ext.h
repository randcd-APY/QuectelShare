#ifndef CSR_LOG_PCAP_EXT_H__
#define CSR_LOG_PCAP_EXT_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2009-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_log_formats.h"

#ifdef __cplusplus
extern "C" {
#endif

void CsrLogPcapAddExtensions(CsrLog *log);
void CsrLogPcapDestroyExtensions(CsrLog *l);

#ifdef __cplusplus
}
#endif

#endif /* CSR_LOG_PCAP_EXT_H__ */
