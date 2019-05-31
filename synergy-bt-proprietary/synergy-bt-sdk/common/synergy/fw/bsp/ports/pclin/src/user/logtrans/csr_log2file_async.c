/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/
#include "csr_synergy.h"

#define DEBUGLEVEL 0
#if DEBUGLEVEL > 0
#define DPRINTF(x) printf x
#else
#define DPRINTF(x)
#endif /* DEBUGLEVEL > 0 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_file.h"
#include "csr_logtransport.h"

#include "platform/csr_logtransport_init.h"

typedef struct
{
    CsrUint8       *buf;
    CsrUint32       bufMaxSize;
    CsrUint32       bufWrap;
    CsrUint32       bufIn;
    CsrUint32       bufOut;
    pthread_mutex_t bufReadWriteMutex;
    pthread_mutex_t bufWriteMutex;
    pthread_cond_t  bufWriteOrCloseEvent;
    pthread_t       bufFileThread;
    CsrFileHandle  *fileHdl;
    CsrBool         isOpen;
} csrLogTransportFileAsync;

#if DEBUGLEVEL > 0
static CsrUint32 bufSize = 0;
#endif

static void *csrLogTransportFileAsyncAlloc(void *arg, CsrSize size)
{
    csrLogTransportFileAsync *fHdl;
    CsrUint8 *ptr;
    CsrUint32 bufOut, bufWrap, bufIn, freeSize;

    fHdl = (csrLogTransportFileAsync *) arg;

    pthread_mutex_lock(&fHdl->bufWriteMutex);

    pthread_mutex_lock(&fHdl->bufReadWriteMutex);
    bufOut = fHdl->bufOut;
    bufIn = fHdl->bufIn;
    bufWrap = fHdl->bufWrap;
    pthread_mutex_unlock(&fHdl->bufReadWriteMutex);

    if (bufIn >= bufOut)
    {
        freeSize = bufWrap - bufIn;
    }
    else
    {
        freeSize = bufOut - bufIn - 1;
    }

    if (size <= freeSize)
    {
        ptr = &fHdl->buf[bufIn];
    }
    else if ((bufIn >= bufOut) && (size < bufOut))
    {
        ptr = &fHdl->buf[0];
        pthread_mutex_lock(&fHdl->bufReadWriteMutex);

        if (fHdl->bufOut != bufIn)
        {
            fHdl->bufWrap = bufIn;
        }
        else
        {
            fHdl->bufOut = 0;
        }
        fHdl->bufIn = 0;
        pthread_mutex_unlock(&fHdl->bufReadWriteMutex);
    }
    else
    {
        DPRINTF(("Out of buffers\n"));
        pthread_mutex_unlock(&fHdl->bufWriteMutex);
        ptr = NULL;
    }

    return ptr;
}

static CsrSize csrLogTransportFileAsyncWrite(void *arg, CsrLogContext context, void *data, CsrSize len)
{
    csrLogTransportFileAsync *fHdl;

    CSR_UNUSED(context);

    fHdl = (csrLogTransportFileAsync *) arg;

    pthread_mutex_lock(&fHdl->bufReadWriteMutex);
#if DEBUGLEVEL > 0
    bufSize += len;
    DPRINTF(("Write: bufSize =%d\n", bufSize));
#endif
    fHdl->bufIn += len;
    pthread_cond_signal(&fHdl->bufWriteOrCloseEvent);
    pthread_mutex_unlock(&fHdl->bufReadWriteMutex);

    pthread_mutex_unlock(&fHdl->bufWriteMutex);

    return len;
}

static void *csrLogTransportFileAsyncThreadFunc(void *arg)
{
    csrLogTransportFileAsync *fHdl;
    CsrUint32 bufOut, bufWrap, bufIn, size;
    CsrSize written;

    fHdl = (csrLogTransportFileAsync *) arg;

    while (fHdl->isOpen)
    {
        DPRINTF(("Begin wait bufSize=%d\n", bufSize));
        pthread_mutex_lock(&fHdl->bufReadWriteMutex);
        while ((fHdl->bufOut == fHdl->bufIn) && (fHdl->isOpen))
        {
            pthread_cond_wait(&fHdl->bufWriteOrCloseEvent, &fHdl->bufReadWriteMutex);
        }
        DPRINTF(("End wait   bufSize=%d\n", bufSize));
        bufOut = fHdl->bufOut;
        bufIn = fHdl->bufIn;
        bufWrap = fHdl->bufWrap;
        pthread_mutex_unlock(&fHdl->bufReadWriteMutex);

        if (bufIn != bufOut)
        {
            if (bufIn >= bufOut)
            {
                size = bufIn - bufOut;
            }
            else
            {
                size = bufWrap - bufOut;
            }

            DPRINTF(("Begin write bufSize=%d size=%d\n", bufSize, size));
            CsrFileWrite(&fHdl->buf[bufOut], size, fHdl->fileHdl, &written);
            DPRINTF(("End   write bufSize=%d, written=%d\n", bufSize, written));

            if (written > 0)
            {
                pthread_mutex_lock(&fHdl->bufReadWriteMutex);
#if DEBUGLEVEL > 0
                bufSize -= written;
                DPRINTF(("Read: bufSize =%d\n", bufSize));
#endif
                fHdl->bufOut += written;
                if (fHdl->bufOut >= fHdl->bufWrap)
                {
                    fHdl->bufOut = 0;
                    fHdl->bufWrap = fHdl->bufMaxSize;
                }
                pthread_mutex_unlock(&fHdl->bufReadWriteMutex);
            }
        }
    }

    return NULL;
}

void *CsrLogTransportFileAsyncOpen(const char *fileName, CsrUint32 bufSize)
{
    csrLogTransportHandle *ltHdl;
    csrLogTransportFileAsync *fHdl;

    ltHdl = CsrPmemAlloc(sizeof(*ltHdl));

    fHdl = CsrPmemAlloc(sizeof(*fHdl));
    fHdl->buf = CsrPmemAlloc(bufSize * sizeof(*fHdl->buf));
    fHdl->bufMaxSize = bufSize;
    fHdl->bufWrap = bufSize;
    fHdl->bufIn = 0;
    fHdl->bufOut = 0;

    if (pthread_mutex_init(&fHdl->bufReadWriteMutex, NULL) != 0)
    {
        CsrPmemFree(fHdl->buf);
        CsrPmemFree(fHdl);
        CsrPmemFree(ltHdl);
        return NULL;
    }

    if (pthread_mutex_init(&fHdl->bufWriteMutex, NULL) != 0)
    {
        pthread_mutex_destroy(&fHdl->bufReadWriteMutex);
        CsrPmemFree(fHdl->buf);
        CsrPmemFree(fHdl);
        CsrPmemFree(ltHdl);
        return NULL;
    }

    if (pthread_cond_init(&fHdl->bufWriteOrCloseEvent, NULL) != 0)
    {
        pthread_mutex_destroy(&fHdl->bufWriteMutex);
        pthread_mutex_destroy(&fHdl->bufReadWriteMutex);
        CsrPmemFree(fHdl->buf);
        CsrPmemFree(fHdl);
        CsrPmemFree(ltHdl);
        return NULL;
    }

    (void) CsrFileOpen(&fHdl->fileHdl, (CsrUtf8String *) fileName,
        CSR_FILE_OPEN_FLAGS_CREATE | CSR_FILE_OPEN_FLAGS_READ_WRITE | CSR_FILE_OPEN_FLAGS_TRUNCATE,
        CSR_FILE_PERMS_USER_READ | CSR_FILE_PERMS_USER_WRITE);
    fHdl->isOpen = TRUE;
    if (pthread_create(&fHdl->bufFileThread, NULL,
            csrLogTransportFileAsyncThreadFunc, fHdl) != 0)
    {
        pthread_cond_destroy(&fHdl->bufWriteOrCloseEvent);
        pthread_mutex_destroy(&fHdl->bufWriteMutex);
        pthread_mutex_destroy(&fHdl->bufReadWriteMutex);
        CsrPmemFree(fHdl->buf);
        CsrPmemFree(fHdl);
        CsrPmemFree(ltHdl);
        return NULL;
    }

    ltHdl->arg = fHdl;
    ltHdl->write = csrLogTransportFileAsyncWrite;
    ltHdl->alloc = csrLogTransportFileAsyncAlloc;
    ltHdl->free = NULL;
    return ltHdl;
}

void CsrLogTransportFileAsyncClose(void *arg)
{
    csrLogTransportHandle *ltHdl;
    csrLogTransportFileAsync *fHdl;
    int ret;
    void *thread_res;

    ltHdl = (csrLogTransportHandle *) arg;
    fHdl = (csrLogTransportFileAsync *) ltHdl->arg;

    fHdl->isOpen = FALSE;

    if (fHdl->bufFileThread != 0)
    {
        pthread_mutex_lock(&fHdl->bufReadWriteMutex);
        pthread_cond_signal(&fHdl->bufWriteOrCloseEvent);
        pthread_mutex_unlock(&fHdl->bufReadWriteMutex);

        ret = pthread_join(fHdl->bufFileThread, &thread_res);
        if (ret == 0)
        {
            fHdl->bufFileThread = 0;
        }
        else
        {
            DPRINTF(("CsrLogTransportFileAsyncClose: FileThread join error\n"));
        }
    }
    else
    {
        DPRINTF(("CsrLogTransportFileAsyncClose: FileThread not running\n"));
    }

    pthread_cond_destroy(&fHdl->bufWriteOrCloseEvent);
    pthread_mutex_destroy(&fHdl->bufReadWriteMutex);
    pthread_mutex_destroy(&fHdl->bufWriteMutex);

    if (fHdl->fileHdl != CSR_FILE_INVALID_HANDLE)
    {
        CsrFileFlush(fHdl->fileHdl);
        CsrFileClose(fHdl->fileHdl);
    }

    CsrPmemFree(fHdl->buf);
    CsrPmemFree(fHdl);

    CsrPmemFree(ltHdl);
}
