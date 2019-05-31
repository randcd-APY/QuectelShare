#ifndef CSR_TM_BLUECORE_TASK_H__
#define CSR_TM_BLUECORE_TASK_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2008-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/
#include "csr_sched.h"
#include "csr_tm_bluecore_bcsp.h"
#include "csr_tm_bluecore_h4.h"
#include "csr_tm_bluecore_h4ibs.h"
#include "csr_tm_bluecore_usb.h"
#include "csr_tm_bluecore_htrans.h"
#include "csr_tm_bluecore_type_a.h"
#include "csr_tm_bluecore_hci_socket.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Tm queue definitions */
extern CsrSchedQid CSR_TM_BLUECORE_IFACEQUEUE;

/* CSR_TM_BCSP */
void CsrTmBlueCoreHandler(void **gash);
void CsrTmBlueCoreDeinit(void **gash);

#define CSR_TM_BLUECORE_BCSP_INIT   CsrTmBlueCoreBcspInit
#define CSR_TM_BLUECORE_H4_UART_INIT   CsrTmBlueCoreH4Init
#define CSR_TM_BLUECORE_H4IBS_INIT  CsrTmBlueCoreH4ibsInit
#define CSR_TM_BLUECORE_H4DS_INIT   CsrTmBlueCoreH4dsInit
#define CSR_TM_BLUECORE_H4I_INIT    CsrTmBlueCoreH4iInit
#define CSR_TM_BLUECORE_USB_INIT    CsrTmBlueCoreUsbInit
#define CSR_TM_BLUECORE_HTRANS_INIT CsrTmBlueCoreHtransInit
#define CSR_TM_BLUECORE_TYPE_A_INIT CsrTmBlueCoreTypeAInit
#define CSR_TM_BLUECORE_HCI_SOCKET_INIT  CsrTmBlueCoreHciSocketInit

#define CSR_TM_BLUECORE_HANDLER     CsrTmBlueCoreHandler
#ifdef ENABLE_SHUTDOWN
#define CSR_TM_BLUECORE_DEINIT  CsrTmBlueCoreDeinit
#else
#define CSR_TM_BLUECORE_DEINIT  NULL
#endif

#ifdef __cplusplus
}
#endif

#endif /* CSR_TM_BLUECORE_TASK_H__ */
