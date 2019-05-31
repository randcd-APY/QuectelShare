#ifndef CSR_FP_PRIM_H__
#define CSR_FP_PRIM_H__
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_sched.h"
#include "csr_prim_defs.h"
#include "csr_mblk.h"
#include "csr_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrFpPrimType" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim CsrFpPrimType;
typedef CsrUint8 CsrFpHandleType;

/* Error messages used by the controller */
#define CSR_FP_RESULT_CANNOT_BE_ENABLED             ((CsrResult) 0x0001)
#define CSR_FP_RESULT_ALREADY_ENABLED               ((CsrResult) 0x0002)
#define CSR_FP_RESULT_NOT_ENABLED                   ((CsrResult) 0x0003)
#define CSR_FP_RESULT_CREDIT_PIPE_NOT_CREATED       ((CsrResult) 0x0004)
#define CSR_FP_RESULT_ID_IN_USE                     ((CsrResult) 0x0005)
#define CSR_FP_RESULT_TX_CAPACITY_REFUSED           ((CsrResult) 0x0006)
#define CSR_FP_RESULT_RX_CAPACITY_REFUSED           ((CsrResult) 0x0007)
#define CSR_FP_RESULT_TOO_MANY_PIPES_ACTIVE         ((CsrResult) 0x0008)
#define CSR_FP_RESULT_WOULD_EXCEED_CONTROLLER_LIMIT ((CsrResult) 0x0009)
#define CSR_FP_RESULT_INVALID_PARAMETERS            ((CsrResult) 0x000A)
#define CSR_FP_RESULT_INVALID_PIPE_ID               ((CsrResult) 0x000B)
#define CSR_FP_RESULT_IS_CREDIT_PIPE                ((CsrResult) 0x000C)

/* Error messages only used by the host */
#define CSR_FP_RESULT_NOT_SUPPORTED                 ((CsrResult) 0x000D)
#define CSR_FP_RESULT_NOT_IMPLEMENTED               ((CsrResult) 0x000E)     /* for things not implemented yet  */
#define CSR_FP_RESULT_ERROR                         ((CsrResult) 0x000F)     /*  */
#define CSR_FP_RESULT_NOT_READY                     ((CsrResult) 0x0010)     /* No FP have been created yet */
#define CSR_FP_RESULT_PIPE_COULD_NOT_BE_CREATED     ((CsrResult) 0x0011)
#define CSR_FP_RESULT_PIPE_COULD_NOT_BE_DESTROYED   ((CsrResult) 0x0012)

#define CSR_FP_RESULT_NOF_RESULT                    ((CsrResult) 0x0013)  /* must be equal to last entry + 1 */

#define CSR_FP_HANDLE_NONE      ((CsrFpHandleType) 0xFF)

#define CSR_FP_READ_FLAG        (0x1)
#define CSR_FP_WRITE_FLAG       (0x2)

#define CSR_FP_HCI_ACL_HDR_SIZE (4) /* handle+flags=2, length=2 */


/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_FP_PRIM_DOWNSTREAM_LOWEST                            (0x0000)

#define CSR_FP_CREATE_REQ                       ((CsrFpPrimType) (0x0000 + CSR_FP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_FP_WRITE_REQ                        ((CsrFpPrimType) (0x0001 + CSR_FP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_FP_CLEAR_REQ                        ((CsrFpPrimType) (0x0002 + CSR_FP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_FP_DESTROY_REQ                      ((CsrFpPrimType) (0x0003 + CSR_FP_PRIM_DOWNSTREAM_LOWEST))

#define CSR_FP_PRIM_DOWNSTREAM_HIGHEST                           (0x0003 + CSR_FP_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_FP_PRIM_UPSTREAM_LOWEST                              (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_FP_CREATE_CFM                       ((CsrFpPrimType) (0x0000 + CSR_FP_PRIM_UPSTREAM_LOWEST))
#define CSR_FP_WRITE_CFM                        ((CsrFpPrimType) (0x0001 + CSR_FP_PRIM_UPSTREAM_LOWEST))
#define CSR_FP_READ_IND                         ((CsrFpPrimType) (0x0002 + CSR_FP_PRIM_UPSTREAM_LOWEST))
#define CSR_FP_CLEAR_CFM                        ((CsrFpPrimType) (0x0003 + CSR_FP_PRIM_UPSTREAM_LOWEST))
#define CSR_FP_DESTROY_CFM                      ((CsrFpPrimType) (0x0004 + CSR_FP_PRIM_UPSTREAM_LOWEST))

#define CSR_FP_PRIM_UPSTREAM_HIGHEST                             (0x0004 + CSR_FP_PRIM_UPSTREAM_LOWEST)

#define CSR_FP_PRIM_DOWNSTREAM_COUNT     (CSR_FP_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_FP_PRIM_DOWNSTREAM_LOWEST)
#define CSR_FP_PRIM_UPSTREAM_COUNT       (CSR_FP_PRIM_UPSTREAM_HIGHEST + 1 - CSR_FP_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

/* primitive structures */
typedef struct
{
    CsrFpPrimType type;                         /* Identity  */
    CsrSchedQid   appHandle;                         /* Application handle which all data associated with this pipe should be returned to */
    CsrUint32     overheadHost;                 /* Pipe overhead on the host */
    CsrUint32     capacityRxHost;               /* Capacity of receive buffer on the host */
    CsrUint32     requiredTxController;         /* Required capacity of tx buffer on controller */
    CsrUint32     desiredTxController;          /* Desired capacity of tx buffer on controller */
    CsrUint32     requiredRxController;         /* Required capacity of rx buffer on controller */
    CsrUint32     desiredRxController;          /* Desired capacity of rx buffer on controller */
} CsrFpCreateReq;

typedef struct
{
    CsrFpPrimType   type;                       /* Identity */
    CsrFpHandleType fpHandle;                   /* fastpipe handle in the range 1..15; 0 is used for credit channel; values in the range 16..255 are reserved */
    CsrResult       result;                     /* Indicates success or specific reason for failure */
    CsrUint32       overheadController;         /* Pipe overhead on the controller */
    CsrUint32       capacityTxController;       /* Capacity of transmit buffer on controller */
    CsrUint32       capacityRxController;       /* Capacity of receive buffer on controller */
} CsrFpCreateCfm;

typedef struct
{
    CsrFpPrimType   type;                /* Identity */
    CsrFpHandleType fpHandle;            /* fastpipe handle in the range 1..15; 0 is used for credit channel; values in the range 16..255 are reserved */
    CsrMblk        *data;
} CsrFpWriteReq;

typedef struct
{
    CsrFpPrimType   type;                /* Identity */
    CsrFpHandleType fpHandle;            /* fastpipe handle in the range 1..15; 0 is used for credit channel; values in the range 16..255 are reserved */
    CsrResult       result;
} CsrFpWriteCfm;

typedef struct
{
    CsrFpPrimType   type;                /* Identity */
    CsrFpHandleType fpHandle;            /* fastpipe handle in the range 1..15; 0 is used for credit channel; values in the range 16..255 are reserved */
    CsrMblk        *data;
} CsrFpReadInd;

typedef struct
{
    CsrFpPrimType   type;                /* Identity */
    CsrFpHandleType fpHandle;            /* fastpipe handle in the range 1..15; 0 is used for credit channel; values in the range 16..255 are reserved */
} CsrFpClearReq;

typedef struct
{
    CsrFpPrimType   type;                /* Identity */
    CsrFpHandleType fpHandle;            /* fastpipe handle in the range 1..15; 0 is used for credit channel; values in the range 16..255 are reserved */
    CsrResult       result;
} CsrFpClearCfm;

typedef struct
{
    CsrFpPrimType type;                  /* Identity */
    CsrUint8      fpHandle;              /* fastpipe handle in the range 1..15; 0 is used for credit channel; values in the range 16..255 are reserved */
} CsrFpDestroyReq;

typedef struct
{
    CsrFpPrimType   type;                /* Identity */
    CsrFpHandleType fpHandle;            /* fastpipe handle in the range 1..15; 0 is used for credit channel; values in the range 16..255 are reserved */
    CsrResult       result;
} CsrFpDestroyCfm;

#ifdef __cplusplus
}
#endif

#endif
