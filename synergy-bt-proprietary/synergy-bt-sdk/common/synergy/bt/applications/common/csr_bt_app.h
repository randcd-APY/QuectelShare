#ifndef _CSR_BT_APP_H__
#define _CSR_BT_APP_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2012-2013 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef __linux__
#include <unistd.h>
#endif

#include "csr_bt_platform.h"
#include "csr_sched.h"
#include "csr_pmem.h"
#include "platform/csr_pmem_init.h"
#include "platform/csr_serial_init.h"
#include "csr_bt_bootstrap_generic.h"
#include "csr_bt_bootstrap_specific.h"

#ifndef __arm
#include "platform/csr_usb_init.h"
#endif

#include "platform/csr_panic_init.h"
#include "csr_sched_init.h"
#include "platform/csr_logtransport_init.h"
#include "csr_transport.h"
#include "csr_hci_lib.h"
#include "csr_bt_util.h"
#include "csr_msgconv.h"
#include "csr_bt_use_tasks.h"
#include "csr_bt_tasks.h"
#include "csr_tm_bluecore_task.h"
#include "csr_bt_demoapp.h"
#include "csr_log_register.h"
#include "csr_log_configure.h"
#include "csr_log_btsnoop.h"
#include "csr_log_fts.h"
#include "csr_log_pcap.h"
#include "csr_formatted_io.h"

/* Defaults */
#define DEFAULT_BTADDR             {0x0001, 0x5b, 0x000001};
#define DEFAULT_BAUDRATE           "115200"
/* [QTI] Fitted with QCA chip, such as QCA6174. */
#define DEFAULT_RESET_BAUDRATE     3000000
#ifdef __linux__
#define DEFAULT_COMPORT            "/dev/ttyS0"
#define DEFAULT_USB_DEVICE         "/dev/bt_usb"
#else
#define DEFAULT_COMPORT            "COM1"
#define DEFAULT_USB_DEVICE         "\\\\.\\csr0"
#endif

#define DEFAULT_ENV_LOG_LEVEL      (CSR_LOG_LEVEL_ENVIRONMENT_BCI_ACL |\
                                    CSR_LOG_LEVEL_ENVIRONMENT_BCI_HCI |\
                                    CSR_LOG_LEVEL_ENVIRONMENT_BCI_VENDOR |\
                                    CSR_LOG_LEVEL_ENVIRONMENT_PROTO)
#define DEFAULT_TASK_LOG_LEVEL     (CSR_LOG_LEVEL_TASK_TEXT |\
                                    CSR_LOG_LEVEL_TASK_STATE |\
                                    CSR_LOG_LEVEL_TASK_STATE_NAME |\
                                    CSR_LOG_LEVEL_TASK_TASK_SWITCH |\
                                    CSR_LOG_LEVEL_TASK_MESSAGE_PUT |\
                                    CSR_LOG_LEVEL_TASK_MESSAGE_PUT_LOC |\
                                    CSR_LOG_LEVEL_TASK_MESSAGE_GET |\
                                    CSR_LOG_LEVEL_TASK_MESSAGE_QUEUE_PUSH |\
                                    CSR_LOG_LEVEL_TASK_MESSAGE_QUEUE_POP |\
                                    CSR_LOG_LEVEL_TASK_TIMER_IN |\
                                    CSR_LOG_LEVEL_TASK_TIMER_CANCEL |\
                                    CSR_LOG_LEVEL_TASK_TIMER_FIRE)

#define DEFAULT_TEXT_LOG_LEVEL     (CSR_LOG_LEVEL_TEXT_CRITICAL |\
                                    CSR_LOG_LEVEL_TEXT_ERROR |\
                                    CSR_LOG_LEVEL_TEXT_WARNING)

#ifdef __cplusplus
extern "C" {
#endif

/* Converter functions */
extern void CsrBtAppConverterInit(void);

/* Log functions */
extern void CsrBtAppLogInit(void);
extern void CsrBtAppLogDeinit(void);
extern void CsrBtAppLogFrontline(void);
extern void CsrBtAppLogWireshark(void);
extern void CsrBtAppLogApply(void);

/* Transport */
extern void *CsrBtAppTransportInit(void);
extern void CsrBtAppTransportDeinit(void *uartHdl);
extern void CsrBtAppTransportSetTask(CsrUint16 transportType);
extern void CsrBtAppTransportSetDefault(CsrUint16 transportType);

/* Commandline parser */
extern void CsrBtAppCmdlineParse(int argc, char *argv[], char *extraOptions);

#ifdef __cplusplus
}
#endif

#endif
