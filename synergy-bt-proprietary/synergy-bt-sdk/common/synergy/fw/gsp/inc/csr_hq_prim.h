#ifndef CSR_HQ_PRIM_H__
#define CSR_HQ_PRIM_H__
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

/* search_string="CsrHqPrimType" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

/* HQ result/error codes */
#define CSR_HQ_RESULT_NO_SUCH_VARID        ((CsrResult) 0x0001) /* Variable identifier not recognised */

typedef CsrUint16 CsrHqVarIdType;

/*  CSR HQ VAR Ids which are standard defines
    Notice that each module using the HQ just shall send the VarID is needs */
#define CSR_HQVARID_DELAYED_PANIC ((CsrHqVarIdType) 0x0800)
#define CSR_HQVARID_FAULT         ((CsrHqVarIdType) 0x1000)


typedef CsrPrim CsrHqPrimType;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_HQ_PRIM_DOWNSTREAM_LOWEST                          (0x0000)

#define CSR_HQ_REGISTER_REQ                   ((CsrHqPrimType) (0x0000 + CSR_HQ_PRIM_DOWNSTREAM_LOWEST))
#define CSR_HQ_DEREGISTER_REQ                 ((CsrHqPrimType) (0x0001 + CSR_HQ_PRIM_DOWNSTREAM_LOWEST))
#define CSR_HQ_MSG_RES                        ((CsrHqPrimType) (0x0002 + CSR_HQ_PRIM_DOWNSTREAM_LOWEST))

#define CSR_HQ_PRIM_DOWNSTREAM_HIGHEST                         (0x0002 + CSR_HQ_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_HQ_PRIM_UPSTREAM_LOWEST                            (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_HQ_REGISTER_CFM                   ((CsrHqPrimType) (0x0000 + CSR_HQ_PRIM_UPSTREAM_LOWEST))
#define CSR_HQ_DEREGISTER_CFM                 ((CsrHqPrimType) (0x0001 + CSR_HQ_PRIM_UPSTREAM_LOWEST))
#define CSR_HQ_MSG_IND                        ((CsrHqPrimType) (0x0002 + CSR_HQ_PRIM_UPSTREAM_LOWEST))

#define CSR_HQ_PRIM_UPSTREAM_HIGHEST                           (0x0002 + CSR_HQ_PRIM_UPSTREAM_LOWEST)

#define CSR_HQ_PRIM_DOWNSTREAM_COUNT          (CSR_HQ_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_HQ_PRIM_DOWNSTREAM_LOWEST)
#define CSR_HQ_PRIM_UPSTREAM_COUNT            (CSR_HQ_PRIM_UPSTREAM_HIGHEST + 1 - CSR_HQ_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

typedef struct
{
    CsrHqPrimType  type;      /* event identifier */
    CsrSchedQid    phandle;     /* handle to application */
    CsrHqVarIdType varId;     /* varId to request subscription for */
    CsrBool        response;    /* Set to TRUE if using CSR_HQ_MSG_RES */
} CsrHqRegisterReq;

typedef struct
{
    CsrHqPrimType  type;        /* event identifier */
    CsrHqVarIdType varId;       /* varId to request subscription for */
    CsrResult      result;      /* result of register attempt */
} CsrHqRegisterCfm;

typedef struct
{
    CsrHqPrimType  type;        /* event identifier */
    CsrSchedQid    phandle;       /* handle to application */
    CsrHqVarIdType varId;       /* varId to terminate subscription for */
} CsrHqDeregisterReq;

typedef struct
{
    CsrHqPrimType  type;        /* event identifier */
    CsrHqVarIdType varId;       /* varId to terminate subscription for */
    CsrResult      result;      /* result of deregister attempt */
} CsrHqDeregisterCfm;

typedef struct
{
    CsrHqPrimType  type;            /* event identifier */
    CsrUint16      cmdType;          /* HQ command type */
    CsrUint16      seqNo;            /* sequence number of the HQ message */
    CsrHqVarIdType varId;           /* varId of the HQ message */
    CsrUint16      payloadLength;    /* the length of the payload of the HQ message */
    CsrUint8      *payload;          /* pointer to the payload of the HQ message */
} CsrHqMsgInd;

typedef struct
{
    CsrHqPrimType  type;            /* event identifier */
    CsrUint16      cmdType;         /* HQ command type */
    CsrUint16      seqNo;           /* sequence number of the HQ message */
    CsrHqVarIdType varId;           /* varId of the HQ message */
    CsrUint16      status;          /* status field of the HQ message */
    CsrUint16      payloadLength;   /* the length of the payload of the HQ message */
    CsrUint8      *payload;         /* pointer to the payload of the HQ message */
} CsrHqMsgRes;

#ifdef __cplusplus
}
#endif

#endif /* CSR_HQ_PRIM_H__ */
