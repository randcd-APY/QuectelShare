#ifndef CSR_BT_HCRP_PRIM_H__
#define CSR_BT_HCRP_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_bt_result.h"
#include "csr_bt_profiles.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtHcrpPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

/* ---------- Defines the HCRP CsrBtResultCode ----------*/
#define CSR_BT_RESULT_CODE_HCRP_SUCCESS                ((CsrBtResultCode) (0x0000))
#define CSR_BT_RESULT_CODE_HCRP_UNACCEPTABLE_PARAMETER ((CsrBtResultCode) (0x0001))


typedef CsrPrim                 CsrBtHcrpPrim;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_HCRP_PRIM_DOWNSTREAM_LOWEST                           (0x0000)

#define CSR_BT_HCRP_ACTIVATE_REQ                     ((CsrBtHcrpPrim) (0x0000 + CSR_BT_HCRP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HCRP_DEACTIVATE_REQ                   ((CsrBtHcrpPrim) (0x0001 + CSR_BT_HCRP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HCRP_REGISTER_DATA_PATH_REQ           ((CsrBtHcrpPrim) (0x0002 + CSR_BT_HCRP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HCRP_GET_LPT_STATUS_RES               ((CsrBtHcrpPrim) (0x0003 + CSR_BT_HCRP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HCRP_GET_IEEE_1284_RES                ((CsrBtHcrpPrim) (0x0004 + CSR_BT_HCRP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HCRP_SECURITY_IN_REQ                  ((CsrBtHcrpPrim) (0x0005 + CSR_BT_HCRP_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_HCRP_PRIM_DOWNSTREAM_HIGHEST                          (0x0005 + CSR_BT_HCRP_PRIM_DOWNSTREAM_LOWEST)

#define CSR_BT_HCRP_HOUSE_CLEANING                   ((CsrBtHcrpPrim) CSR_BT_HCRP_PRIM_DOWNSTREAM_HIGHEST + 2)

/*******************************************************************************/

#define CSR_BT_HCRP_PRIM_UPSTREAM_LOWEST                             (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_HCRP_REGISTER_DATA_PATH_CFM           ((CsrBtHcrpPrim) (0x0000 + CSR_BT_HCRP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HCRP_L2CA_CONNECT_IND                 ((CsrBtHcrpPrim) (0x0001 + CSR_BT_HCRP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HCRP_L2CA_DISCONNECT_IND              ((CsrBtHcrpPrim) (0x0002 + CSR_BT_HCRP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HCRP_L2CA_DATA_IND                    ((CsrBtHcrpPrim) (0x0003 + CSR_BT_HCRP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HCRP_GET_LPT_STATUS_IND               ((CsrBtHcrpPrim) (0x0004 + CSR_BT_HCRP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HCRP_GET_IEEE_1284_IND                ((CsrBtHcrpPrim) (0x0005 + CSR_BT_HCRP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HCRP_STATUS_IND                       ((CsrBtHcrpPrim) (0x0006 + CSR_BT_HCRP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HCRP_SECURITY_IN_CFM                  ((CsrBtHcrpPrim) (0x0007 + CSR_BT_HCRP_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_HCRP_PRIM_UPSTREAM_HIGHEST                            (0x0007 + CSR_BT_HCRP_PRIM_DOWNSTREAM_LOWEST)

#define CSR_BT_HCRP_PRIM_DOWNSTREAM_COUNT            (CSR_BT_HCRP_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_HCRP_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_HCRP_PRIM_UPSTREAM_COUNT              (CSR_BT_HCRP_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_HCRP_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

typedef struct
{
    CsrBtHcrpPrim       type;
    CsrSchedQid         appHandle;
    CsrUtf8String       *ieee1284Id;
}  CsrBtHcrpActivateReq;

typedef struct
{
    CsrBtHcrpPrim     type;
} CsrBtHcrpDeactivateReq;

typedef struct
{
    CsrBtHcrpPrim     type;
    CsrUint16         pduId;
    CsrUint16         transId;
    CsrUint8          lptStatus;
} CsrBtHcrpGetLptStatusRes;

typedef struct
{
    CsrBtHcrpPrim     type;
    CsrSchedQid        dataHandle;
} CsrBtHcrpRegisterDataPathReq;

typedef struct
{
    CsrBtHcrpPrim     type;
    CsrUint16         pduId;
    CsrUint16         transId;
    CsrUtf8String     *ieee1284;
} CsrBtHcrpGetIeee1284Res;

typedef struct
{
    CsrBtHcrpPrim     type;
} CsrBtHcrpRegisterDataPathCfm;

typedef struct
{
    CsrBtHcrpPrim     type;
} CsrBtHcrpL2caConnectInd;

typedef struct
{
    CsrBtHcrpPrim        type;
    CsrBtReasonCode     reasonCode;
    CsrBtSupplier reasonSupplier;
} CsrBtHcrpL2caDisconnectInd;

typedef struct
{
    CsrBtHcrpPrim     type;
    CsrUint16         payloadLength;
    CsrUint8          *payload;
} CsrBtHcrpL2caDataInd;

typedef struct
{
    CsrBtHcrpPrim     type;
    CsrUint16         pduId;
    CsrUint16         transId;
} CsrBtHcrpGetLptStatusInd;

typedef struct
{
    CsrBtHcrpPrim     type;
    CsrUint16         pduId;
    CsrUint16         transId;
} CsrBtHcrpGetIeee1284Ind;

typedef struct
{
    CsrBtHcrpPrim     type;
    CsrBool           connect;
} CsrBtHcrpStatusInd;

typedef struct
{
    CsrBtHcrpPrim     type;
} CsrBtHcrpHouseCleaning;

typedef struct
{
    CsrBtHcrpPrim     type;
    CsrSchedQid        appHandle;
    CsrUint16         secLevel;
} CsrBtHcrpSecurityInReq;

typedef struct
{
    CsrBtHcrpPrim       type;
    CsrBtResultCode     resultCode;
    CsrBtSupplier resultSupplier;
} CsrBtHcrpSecurityInCfm;


#ifdef __cplusplus
}
#endif

#endif

