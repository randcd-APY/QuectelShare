/*****************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/
#include "csr_synergy.h"

#include <stdio.h>

#include "csr_panic.h"
#include "csr_util.h"
#include "csr_prim_defs.h"

#include "csr_log.h"
#include "csr_formatted_io.h"

#include "csr_exceptionhandler.h"

#ifndef EXCLUDE_CSR_EXCEPTION_HANDLER_MODULE
#define LOGBUFSZ 1024

void CsrStateEventExceptionWithInfo(const char *theTask,
    CsrUint16 theEventClass,
    CsrUint16 theType,
    CsrUint16 theState,
    const char *theFile,
    CsrUint32 theLine)
{
    char buf[LOGBUFSZ];

    theFile = CsrGetBaseName(theFile);
    theTask = theTask != NULL ? theTask : "UNKNOWN";

    CsrSnprintf(buf, sizeof(buf), "[%s:%u] State Event Exception: Task=%s, State=0x%x, EventClass=0x%x, Type=0x%x",
                theFile, theLine, theTask, theState, theEventClass, theType);

#ifdef CSR_LOG_ENABLE
    CsrLogExceptionStateEvent(theEventClass, theType, theState, theLine, theFile);
#endif

    printf("%s\n", buf);

#ifdef EXCEPTION_PANIC
    CsrPanic(CSR_TECH_FW, CSR_PANIC_FW_EXCEPTION, buf);
#endif
}

void CsrGeneralExceptionWithInfo(const char *theTask,
    CsrUint16 theState,
    CsrPrim theprimType,
    const char *theText,
    const char *theFile,
    CsrUint32 theLine)
{
    char buf[LOGBUFSZ];

    theFile = CsrGetBaseName(theFile);
    theTask = theTask != NULL ? theTask : "UNKNOWN";

    CsrSnprintf(buf, sizeof(buf), "[%s:%u] General Exception: Task=%s, State=0x%x, PrimType=0x%x, [%s]",
                theFile, theLine, theTask, theState, theprimType, theText);

#ifdef CSR_LOG_ENABLE
    CsrLogExceptionGeneral(theprimType, theState, theText, theLine, theFile);
#endif

    printf("%s\n", buf);

#ifdef EXCEPTION_PANIC
    CsrPanic(CSR_TECH_FW, CSR_PANIC_FW_EXCEPTION, buf);
#endif
}

void CsrGeneralWarningWithInfo(const char *theTask,
    CsrUint16 theState,
    CsrPrim theprimType,
    const char *theText,
    const char *theFile,
    CsrUint32 theLine)
{
    char buf[LOGBUFSZ];

    theFile = CsrGetBaseName(theFile);
    theTask = theTask != NULL ? theTask : "UNKNOWN";

    CsrSnprintf(buf, sizeof(buf), "[%s:%u] General Warning: Task=%s, State=0x%x, PrimType=0x%x, [%s]",
                theFile, theLine, theTask, theState, theprimType, theText);

#ifdef CSR_LOG_ENABLE
    CsrLogExceptionWarning(theprimType, theState, theText, theLine, theFile);
#endif

    printf("%s\n", buf);
}

#endif
