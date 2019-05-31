#ifndef CSR_TFTP_FILE_TASK_H__
#define CSR_TFTP_FILE_TASK_H__
/*****************************************************************************

Copyright (c) 2011-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_sched.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Queue definition for CSR_TFTP_FILE */
extern CsrSchedQid CSR_TFTP_FILE_IFACEQUEUE;

/* Task definition for CSR_TFTP_FILE */
void CsrTftpFileInit(void **gash);
void CsrTftpFileDeinit(void **gash);
void CsrTftpFileHandler(void **gash);

/* CSR_TFTP_FILE */
#define CSR_TFTP_FILE_INIT CsrTftpFileInit
#define CSR_TFTP_FILE_HANDLER CsrTftpFileHandler
#ifdef ENABLE_SHUTDOWN
#define CSR_TFTP_FILE_DEINIT CsrTftpFileDeinit
#else
#define CSR_TFTP_FILE_DEINIT NULL
#endif

#ifdef __cplusplus
}
#endif

#endif
