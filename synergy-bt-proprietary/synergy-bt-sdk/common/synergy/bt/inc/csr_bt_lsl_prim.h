#ifndef CSR_BT_LSL_PRIM_H__
#define CSR_BT_LSL_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_bt_profiles.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtLslPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim                             CsrBtLslPrim;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_LSL_PRIM_DOWNSTREAM_LOWEST                        (0x0000)

#define CSR_BT_LSL_DATA_REQ                       ((CsrBtLslPrim) (0x0000 + CSR_BT_LSL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_LSL_DATA_RES                       ((CsrBtLslPrim) (0x0001 + CSR_BT_LSL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_LSL_CONNECT_REQ                    ((CsrBtLslPrim) (0x0002 + CSR_BT_LSL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_LSL_DISCONNECT_REQ                 ((CsrBtLslPrim) (0x0003 + CSR_BT_LSL_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_LSL_PRIM_DOWNSTREAM_HIGHEST                       (0x0003 + CSR_BT_LSL_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_BT_LSL_PRIM_UPSTREAM_LOWEST                          (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_LSL_DATA_CFM                       ((CsrBtLslPrim) (0x0000 + CSR_BT_LSL_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_LSL_DATA_IND                       ((CsrBtLslPrim) (0x0001 + CSR_BT_LSL_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_LSL_STATUS_IND                     ((CsrBtLslPrim) (0x0002 + CSR_BT_LSL_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_LSL_PRIM_UPSTREAM_HIGHEST                         (0x0002 + CSR_BT_LSL_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_LSL_PRIM_DOWNSTREAM_COUNT          (CSR_BT_LSL_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_LSL_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_LSL_PRIM_UPSTREAM_COUNT            (CSR_BT_LSL_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_LSL_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

/* Downstream messages */
typedef struct
{
    CsrBtLslPrim         type;
    CsrUint16            length;
    CsrUint8             *payload;
} CsrBtLslDataReq;

typedef struct
{
    CsrBtLslPrim         type;
} CsrBtLslDataRes;

typedef struct
{
    CsrBtLslPrim         type;
} CsrBtLslConnectReq;

typedef struct
{
    CsrBtLslPrim         type;
} CsrBtLslDisconnectReq;

/* Upstream messages */
typedef struct
{
    CsrBtLslPrim         type;
} CsrBtLslDataCfm;

typedef struct
{
    CsrBtLslPrim         type;
    CsrUint8             *payload;
    CsrUint16            length;
} CsrBtLslDataInd;

typedef struct
{
    CsrBtLslPrim         type;
    CsrBool              connected;
    CsrUint8             reason;
    CsrUint16            maxPayloadLength;
} CsrBtLslStatusInd;

#ifdef __cplusplus
}
#endif

#endif

