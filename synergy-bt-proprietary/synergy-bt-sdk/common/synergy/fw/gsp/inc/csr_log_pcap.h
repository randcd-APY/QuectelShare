#ifndef CSR_LOG_PCAP_H__
#define CSR_LOG_PCAP_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_log_formats.h"

#ifdef __cplusplus
extern "C" {
#endif

/* HCI packet types */
#define HCI_H4_TYPE_SYNERGY_V1      0x20
#define HCI_H4_TYPE_SYNERGY_V2      0x21
#define HCI_H4_TYPE_SYNERGY_V3      0x22

CsrLog *CsrLogPcapCreate(void *logHandle);
void CsrLogPcapDestroy(CsrLog *l);

#ifdef __cplusplus
}
#endif

#endif
