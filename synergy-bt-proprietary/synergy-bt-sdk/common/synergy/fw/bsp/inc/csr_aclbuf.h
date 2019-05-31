#ifndef CSR_ACLBUF_H__
#define CSR_ACLBUF_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Structure for bulks of ACL data */
typedef struct CsrAclBufElm
{
    struct CsrAclBufElm *next;
    CsrUint16            chanAndFlags;
    CsrUint16            size;
    void                *data;
} CsrAclBufElm;

typedef void (*CsrAclBufReady)(CsrAclBufElm *elm, void *priv);
typedef CsrInt32 (*CsrAclBufSizer)(CsrUint16 channelAndFlags, CsrUint16 size, void *data);

/* Structure for ACL buffering upper interface */
typedef struct CsrAclBuf
{
    CsrBool (*reg)(CsrUint16 chan,
        CsrBool doBuffering,
        CsrAclBufReady bufReady,
        CsrAclBufSizer aclSizer,
        void *privArg);
    void (*dereg)(CsrUint16 chan);
    void (*free)(CsrAclBufElm **elm);
    CsrAclBufElm *(*flush)(CsrUint16 chan);
    void (*deinit)(struct CsrAclBuf **acl);
} CsrAclBuf;

#ifdef __cplusplus
}
#endif

#endif /* CSR_ACLBUF_H__ */
