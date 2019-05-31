#ifndef CSR_TFTP_PRIM_H__
#define CSR_TFTP_PRIM_H__
/*****************************************************************************

Copyright (c) 2011-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_prim_defs.h"
#include "csr_sched.h"
#include "csr_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrTftpPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim CsrTftpPrim;

/* Opcodes */
typedef CsrUint16 CsrTftpOpcode;
#define CSR_TFTP_OPCODE_READ                             ((CsrTftpOpcode) 1)
#define CSR_TFTP_OPCODE_WRITE                            ((CsrTftpOpcode) 2)

/* Protocol family */
typedef CsrUint8 CsrTftpFamily;
#define CSR_TFTP_FAMILY_IP4                              ((CsrTftpFamily) 4)
#define CSR_TFTP_FAMILY_IP6                              ((CsrTftpFamily) 6)

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_TFTP_PRIM_DOWNSTREAM_LOWEST                  (0x0000)

#define CSR_TFTP_ACTIVATE_REQ                            ((CsrTftpPrim) (0x0000 + CSR_TFTP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_TFTP_DEACTIVATE_REQ                          ((CsrTftpPrim) (0x0001 + CSR_TFTP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_TFTP_CONNECT_RES                             ((CsrTftpPrim) (0x0002 + CSR_TFTP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_TFTP_READ_RES                                ((CsrTftpPrim) (0x0003 + CSR_TFTP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_TFTP_WRITE_RES                               ((CsrTftpPrim) (0x0004 + CSR_TFTP_PRIM_DOWNSTREAM_LOWEST))

#define CSR_TFTP_PRIM_DOWNSTREAM_HIGHEST                 (0x0004 + CSR_TFTP_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_TFTP_PRIM_UPSTREAM_LOWEST                    (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_TFTP_ACTIVATE_CFM                            ((CsrTftpPrim) (0x0000 + CSR_TFTP_PRIM_UPSTREAM_LOWEST))
#define CSR_TFTP_DEACTIVATE_CFM                          ((CsrTftpPrim) (0x0001 + CSR_TFTP_PRIM_UPSTREAM_LOWEST))
#define CSR_TFTP_CONNECT_IND                             ((CsrTftpPrim) (0x0002 + CSR_TFTP_PRIM_UPSTREAM_LOWEST))
#define CSR_TFTP_READ_IND                                ((CsrTftpPrim) (0x0003 + CSR_TFTP_PRIM_UPSTREAM_LOWEST))
#define CSR_TFTP_WRITE_IND                               ((CsrTftpPrim) (0x0004 + CSR_TFTP_PRIM_UPSTREAM_LOWEST))
#define CSR_TFTP_DISCONNECT_IND                          ((CsrTftpPrim) (0x0005 + CSR_TFTP_PRIM_UPSTREAM_LOWEST))

#define CSR_TFTP_PRIM_UPSTREAM_HIGHEST                   (0x0005 + CSR_TFTP_PRIM_UPSTREAM_LOWEST)

#define CSR_TFTP_PRIM_DOWNSTREAM_COUNT                   (CSR_TFTP_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_TFTP_PRIM_DOWNSTREAM_LOWEST)
#define CSR_TFTP_PRIM_UPSTREAM_COUNT                     (CSR_TFTP_PRIM_UPSTREAM_HIGHEST + 1 - CSR_TFTP_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

typedef struct
{
    CsrTftpPrim   type;
    CsrSchedQid   qid;
    CsrTftpFamily family;
    CsrUint8      ipAddress[16];
} CsrTftpActivateReq;

typedef struct
{
    CsrTftpPrim type;
    CsrResult   result;
} CsrTftpActivateCfm;

typedef struct
{
    CsrTftpPrim type;
    CsrSchedQid qid;
} CsrTftpDeactivateReq;

typedef struct
{
    CsrTftpPrim type;
} CsrTftpDeactivateCfm;

typedef struct
{
    CsrTftpPrim    type;
    CsrTftpFamily  family;
    CsrUint8       ipAddress[16];
    CsrUint16      port;
    CsrTftpOpcode  opcode;
    CsrCharString *fileName;
} CsrTftpConnectInd;

typedef struct
{
    CsrTftpPrim type;
    CsrResult   result;
} CsrTftpConnectRes;

typedef struct
{
    CsrTftpPrim type;
} CsrTftpReadInd;

typedef struct
{
    CsrTftpPrim type;
    CsrUint16   dataLength;
    CsrUint8   *data;
    CsrResult   result;
} CsrTftpReadRes;

typedef struct
{
    CsrTftpPrim type;
    CsrUint16   dataLength;
    CsrUint8   *data;
} CsrTftpWriteInd;

typedef struct
{
    CsrTftpPrim type;
    CsrResult   result;
} CsrTftpWriteRes;

typedef struct
{
    CsrTftpPrim type;
} CsrTftpDisconnectInd;

#ifdef __cplusplus
}
#endif

#endif /* CSR_TFTP_PRIM_H__ */
