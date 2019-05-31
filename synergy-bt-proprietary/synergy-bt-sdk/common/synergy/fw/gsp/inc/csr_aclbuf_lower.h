#ifndef CSR_ACLBUF_LOWER_H__
#define CSR_ACLBUF_LOWER_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#include "csr_types.h"

typedef void (*CsrAclBufCb)(void);

typedef CsrUint8 (*CsrAclBufAllocCb)(CsrUint16 chanAndFlags,
                                     CsrUint16 size,
                                     void *hdr,
                                     CsrUint16 hdrSize,
                                     void **ctx,
                                     CsrUint8 **data);

typedef CsrAclBufCb (*CsrAclBufDeliverCb)(CsrUint16 chanAndFlags,
                                          void *ctx,
                                          CsrUint16 size);

/** Initialise ACL buffer callbacks in lower API
 *
 * @param alloc A pointer to a function that is used to
 * allocate data buffers.
 *
 * @param deliver A pointer to a function that is used
 * to pass back data batches to the ACL-buffer implementation.
 * The function returns a CsrAclBufCb function pointer which
 * is to be called when enough data has been buffered and
 * processing needs to be started.
 *
 * NOTE: Return value must be either NULL or a constant
 *       function pointer.  In other words, the return
 *       value is either NULL or a fixed value;  the
 *       function not allowed to return a pointer to e.g.
 *       f1() in some cases and f2() in others.
 *
 */
void CsrAclBufLowerInit(CsrAclBufAllocCb alloc,
    CsrAclBufDeliverCb deliver);

/** Allocate/obtain data from AclBuf
 *
 * ** ONLY FOR USE BY TRANSPORTS INSIDE FRAMEWORK. NOT
 * ** BY TECHNOLOGIES OR APPLICATIONS.
 *
 * Used by transports (typeA) to retrieve memory
 * from pool in AclBuf. Returns a pointer to memory
 * if AclBuf has a registered subscription on specified
 * ACL channel. Returns error code if either, no subscriber
 * is present, or memory cannot be provided.
 *
 * @param chanAndFlags ACL channel and flags
 *
 * @param size Size of memory requested
 *
 * @param hdr Pointer to header in ACL payload data
 *
 * @param hdrSize Size of header
 *
 * @param ctx Context data that is used to identify when the
 * buffer obtained is passed back to AB using CsrAclBufDeliver().
 * It is intended to be used by the memory provider to do fast
 * buffer handling metadata lookups, but the behavior is
 * unspecified.  The contents of the ctx pointer are updated iff
 * the CsrAclBufAlloc() return value is CSR_ACLBUF_SUCCESS.
 *
 * @param data Pointer to memory buffer.  The data pointer
 * contents are updated only iff the CsrAclBufAlloc() return
 * value is CSR_ACLBUF_SUCCESS.
 *
 * @return CSR_ACLBUF_SUCCESS, if success
 *         CSR_ACLBUF_NONE, if no registration for chan
 *         CSR_ACLBUF_NOMEM, if memory cannot be provided
 */
CsrUint8 CsrAclBufAlloc(CsrUint16 chanAndFlags,
    CsrUint16 size,
    void *hdr,
    CsrUint16 hdrSize,
    void **ctx,
    CsrUint8 **data);

#define CSR_ACLBUF_SUCCESS  10
#define CSR_ACLBUF_NONE     20
#define CSR_ACLBUF_NOMEM    30

/** Deliver/signal data to AclBuf
 *
 * ** ONLY FOR USE BY TRANSPORTS INSIDE FRAMEWORK. NOT
 * ** BY TECHNOLOGIES OR APPLICATIONS.
 *
 * Used by transports (typeA) to deliver data /
 * signal data ready to AclBuf.
 *
 * @param chanAndFlags ACL channel and flags
 *
 * @param ctx Context data
 *
 * @param size Size of data delivered
 *
 */
CsrAclBufCb CsrAclBufDeliver(CsrUint16 chanAndFlags,
    void *ctx,
    CsrUint16 size);

#ifdef __cplusplus
}
#endif

#endif /* CSR_ACLBUF_LOWER_H__ */
