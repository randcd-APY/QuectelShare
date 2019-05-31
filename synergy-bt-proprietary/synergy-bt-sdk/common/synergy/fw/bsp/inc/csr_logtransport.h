#ifndef CSR_LOGTRANSPORT_H__
#define CSR_LOGTRANSPORT_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* log entry types */
/* header data needs to be accumulated if the transport flushes */
typedef enum
{
    LOG_HEADER,
    LOG_NORMAL
} CsrLogContext;

/* Write the data on the used log interface.
 * Parameters: log transport handle, entry type, pointer to data,
 * and size of data to write.
 * Returns the number of bytes written.
 *
 * Note that `data' must previously be allocated using
 * CsrLogTransportAlloc(), and that freeing the pointer
 * is the responsibility CsrLogTransportWrite().
 */
CsrSize CsrLogTransportWrite(void *ltHdl, CsrLogContext context,
    void *data, CsrSize size);

/* Allocate buffer for the log transport. */
void *CsrLogTransportAlloc(void *ltHdl, CsrSize sz);

#ifdef __cplusplus
}
#endif

#endif
