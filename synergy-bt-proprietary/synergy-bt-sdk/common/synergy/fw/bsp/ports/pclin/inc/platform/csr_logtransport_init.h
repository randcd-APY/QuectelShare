#ifndef CSR_LOGTRANSPORT_INIT_H__
#define CSR_LOGTRANSPORT_INIT_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_logtransport.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CSR_LOGTRANSPORT_HAVE_FILE
#define CSR_LOGTRANSPORT_HAVE_FILE
#endif

#ifndef CSR_LOGTRANSPORT_HAVE_STDOUT
#define CSR_LOGTRANSPORT_HAVE_STDOUT
#endif

typedef struct
{
    CsrSize (*write)(void *arg, CsrLogContext context, void *data, CsrSize length);
    void * (*alloc)(void *arg, CsrSize size);
    void (*free)(void *arg, void *data);
    void *arg;
} csrLogTransportHandle;


/*
 * File log transport API
 */

/* initialize internal stuff needed for transportation of log information
 * return log transport handle on success
 */
void *CsrLogTransportFileOpen(const char *fileName);
void CsrLogTransportFileClose(void *arg);

void *CsrLogTransportFileAsyncOpen(const char *fileName, CsrUint32 bufSize);
void CsrLogTransportFileAsyncClose(void *arg);

/*
 * Stdout log transport API
 */
void *CsrLogTransportStdoutOpen(void);
void CsrLogTransportStdoutClose(void *arg);

#ifdef __cplusplus
}
#endif

#endif
