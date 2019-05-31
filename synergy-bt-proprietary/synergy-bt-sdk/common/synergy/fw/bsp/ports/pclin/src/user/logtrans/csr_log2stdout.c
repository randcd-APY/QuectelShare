/*****************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/
#include "csr_synergy.h"

#ifdef CSR_LOG_ENABLE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_logtransport.h"

#include <pthread.h>
#include "platform/csr_logtransport_init.h"

static CsrSize logTransportStdoutWrite(void *arg, CsrLogContext context, void *data, CsrSize length)
{
    pthread_mutex_t *mutex;
    CsrSize numberOfBytesWritten;

    CSR_UNUSED(context);

    mutex = (pthread_mutex_t *) arg;

    pthread_mutex_lock(mutex);
    numberOfBytesWritten = fwrite(data, 1, length, stdout);
    pthread_mutex_unlock(mutex);

    return numberOfBytesWritten;
}

static void *logTransportStdoutAlloc(void *arg, CsrSize size)
{
    CSR_UNUSED(arg);
    return CsrPmemAlloc(size);
}

static void logTransportStdoutFree(void *arg, void *data)
{
    CSR_UNUSED(arg);
    CsrPmemFree(data);
}

void *CsrLogTransportStdoutOpen(void)
{
    csrLogTransportHandle *ltHdl;
    pthread_mutex_t *mutex;

    ltHdl = CsrPmemAlloc(sizeof(csrLogTransportHandle));
    mutex = CsrPmemAlloc(sizeof(pthread_mutex_t));

    if (pthread_mutex_init(mutex, NULL) == 0)
    {
        ltHdl->write = logTransportStdoutWrite;
        ltHdl->alloc = logTransportStdoutAlloc;
        ltHdl->free = logTransportStdoutFree;
        ltHdl->arg = mutex;
    }
    else
    {
        CsrPmemFree(mutex);
        CsrPmemFree(ltHdl);
        ltHdl = NULL;
    }

    return ltHdl;
}

void CsrLogTransportStdoutClose(void *arg)
{
    csrLogTransportHandle *ltHdl;
    pthread_mutex_t *mutex;

    ltHdl = (csrLogTransportHandle *) arg;
    mutex = (pthread_mutex_t *) ltHdl->arg;

    pthread_mutex_destroy(mutex);

    CsrPmemFree(ltHdl->arg);
    CsrPmemFree(ltHdl);
}

#endif
