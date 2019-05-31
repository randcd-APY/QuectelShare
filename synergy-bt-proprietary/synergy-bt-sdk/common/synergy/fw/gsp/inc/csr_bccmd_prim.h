#ifndef CSR_BCCMD_PRIM_H__
#define CSR_BCCMD_PRIM_H__
/*****************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/
#include "csr_types.h"
#include "csr_prim_defs.h"
#include "csr_sched.h"
#include "csr_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBccmdPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim CsrBccmdPrim;
typedef CsrUint16 CsrBccmdStoresType;

/* BCCMD result/error codes */
#define CSR_BCCMD_RESULT_NO_SUCH_VARID          ((CsrResult) 0x0001)
#define CSR_BCCMD_RESULT_TOO_BIG                ((CsrResult) 0x0002)
#define CSR_BCCMD_RESULT_NO_VALUE               ((CsrResult) 0x0003)
#define CSR_BCCMD_RESULT_BAD_REQ                ((CsrResult) 0x0004)
#define CSR_BCCMD_RESULT_NO_ACCESS              ((CsrResult) 0x0005)
#define CSR_BCCMD_RESULT_READ_ONLY              ((CsrResult) 0x0006)
#define CSR_BCCMD_RESULT_WRITE_ONLY             ((CsrResult) 0x0007)
#define CSR_BCCMD_RESULT_ERROR                  ((CsrResult) 0x0008)
#define CSR_BCCMD_RESULT_PERMISSION_DENIED      ((CsrResult) 0x0009)

/* BCCMD stores */
#define CSR_BCCMD_STORES_DEFAULT                ((CsrBccmdStoresType) 0x0000)
#define CSR_BCCMD_STORES_PSRAM                  ((CsrBccmdStoresType) 0x0008)
#define CSR_BCCMD_STORES_PSI                    ((CsrBccmdStoresType) 0x0001)
#define CSR_BCCMD_STORES_PSF                    ((CsrBccmdStoresType) 0x0002)
#define CSR_BCCMD_STORES_PSROM                  ((CsrBccmdStoresType) 0x0004)
#define CSR_BCCMD_STORES_PSI_PSF                ((CsrBccmdStoresType) 0x0003)
#define CSR_BCCMD_STORES_PSI_PSF_PSROM          ((CsrBccmdStoresType) 0x0007)
#define CSR_BCCMD_STORES_PSRAM_PSI              ((CsrBccmdStoresType) 0x0009)
#define CSR_BCCMD_STORES_PSRAM_PSI_PSF          ((CsrBccmdStoresType) 0x000B)
#define CSR_BCCMD_STORES_PSRAM_PSI_PSF_PSROM    ((CsrBccmdStoresType) 0x000F)

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BCCMD_PRIM_DOWNSTREAM_LOWEST                    (0x0000)

#define CSR_BCCMD_REQ                       ((CsrBccmdPrim) (0x0000 + CSR_BCCMD_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BCCMD_FLUSH_REQ                 ((CsrBccmdPrim) (0x0001 + CSR_BCCMD_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BCCMD_PRIM_DOWNSTREAM_HIGHEST                   (0x0001 + CSR_BCCMD_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_BCCMD_PRIM_UPSTREAM_LOWEST                      (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BCCMD_CFM                       ((CsrBccmdPrim) (0x0000 + CSR_BCCMD_PRIM_UPSTREAM_LOWEST))
#define CSR_BCCMD_FLUSH_CFM                 ((CsrBccmdPrim) (0x0001 + CSR_BCCMD_PRIM_UPSTREAM_LOWEST))

#define CSR_BCCMD_PRIM_UPSTREAM_HIGHEST                     (0x0001 + CSR_BCCMD_PRIM_UPSTREAM_LOWEST)

#define CSR_BCCMD_PRIM_DOWNSTREAM_COUNT     (CSR_BCCMD_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BCCMD_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BCCMD_PRIM_UPSTREAM_COUNT       (CSR_BCCMD_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BCCMD_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

typedef struct
{
    CsrBccmdPrim type;                      /* Event identifier                                 */
    CsrSchedQid  phandle;                     /* Handle to application                            */
    CsrUint16    payloadLength;              /* The length of the BCCMD message                  */
    CsrUint8    *payload;                    /* Pointer to the BCCMD message                     */
} CsrBccmdReq;

typedef struct
{
    CsrBccmdPrim type;                      /* Event identifier                                 */
    CsrUint16    cmdType;                    /* BCCMD command type                               */
    CsrUint16    seqNo;                      /* Sequence number of the BCCMD message             */
    CsrUint16    varId;                      /* VarId of the BCCMD message                       */
    CsrResult    status;                     /* Status information about the BCCMD transaction   */
    CsrUint16    payloadLength;              /* The length of the payload of the BCCMD message   */
    CsrUint8    *payload;                    /* Pointer to the payload of the BCCMD message      */
} CsrBccmdCfm;

typedef struct
{
    CsrBccmdPrim type;
    CsrSchedQid  phandle;
} CsrBccmdFlushReq;

typedef struct
{
    CsrBccmdPrim type;
    CsrResult    result;
} CsrBccmdFlushCfm;

#ifdef __cplusplus
}
#endif

#endif /* CSR_BCCMD_PRIM_H__ */
