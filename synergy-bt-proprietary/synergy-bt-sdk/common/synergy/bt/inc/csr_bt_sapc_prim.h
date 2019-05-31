#ifndef CSR_BT_SAPC_PRIM_H__
#define CSR_BT_SAPC_PRIM_H__

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
#include "csr_bt_sap_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtSapcPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim          CsrBtSapcPrim;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_SAPC_PRIM_DOWNSTREAM_LOWEST                           (0x0000)

#define CSR_BT_SAPC_CONNECT_REQ                        ((CsrBtSapcPrim) (0x0000 + CSR_BT_SAPC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SAPC_DISCONNECT_REQ                     ((CsrBtSapcPrim) (0x0001 + CSR_BT_SAPC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SAPC_TRANSFER_APDU_REQ                  ((CsrBtSapcPrim) (0x0002 + CSR_BT_SAPC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SAPC_TRANSFER_ATR_REQ                   ((CsrBtSapcPrim) (0x0003 + CSR_BT_SAPC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SAPC_POWER_SIM_OFF_REQ                  ((CsrBtSapcPrim) (0x0004 + CSR_BT_SAPC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SAPC_POWER_SIM_ON_REQ                   ((CsrBtSapcPrim) (0x0005 + CSR_BT_SAPC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SAPC_RESET_SIM_REQ                      ((CsrBtSapcPrim) (0x0006 + CSR_BT_SAPC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SAPC_TRANSFER_CARD_READER_STATUS_REQ    ((CsrBtSapcPrim) (0x0007 + CSR_BT_SAPC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SAPC_SET_TRANSFER_PROTOCOL_REQ          ((CsrBtSapcPrim) (0x0008 + CSR_BT_SAPC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SAPC_SECURITY_OUT_REQ                   ((CsrBtSapcPrim) (0x0009 + CSR_BT_SAPC_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_SAPC_PRIM_DOWNSTREAM_HIGHEST                          (0x0009 + CSR_BT_SAPC_PRIM_DOWNSTREAM_LOWEST)

/* Not part of interface, used internally only */
#define CSR_BT_SAPC_HOUSE_CLEANING                     ((CsrBtSapcPrim) (0x0010 + CSR_BT_SAPC_PRIM_DOWNSTREAM_LOWEST))

/*******************************************************************************/

#define CSR_BT_SAPC_PRIM_UPSTREAM_LOWEST                             (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_SAPC_CONNECT_CFM                        ((CsrBtSapcPrim) (0x0000 + CSR_BT_SAPC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SAPC_DISCONNECT_IND                     ((CsrBtSapcPrim) (0x0001 + CSR_BT_SAPC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SAPC_TRANSFER_APDU_CFM                  ((CsrBtSapcPrim) (0x0002 + CSR_BT_SAPC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SAPC_TRANSFER_ATR_CFM                   ((CsrBtSapcPrim) (0x0003 + CSR_BT_SAPC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SAPC_POWER_SIM_OFF_CFM                  ((CsrBtSapcPrim) (0x0004 + CSR_BT_SAPC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SAPC_POWER_SIM_ON_CFM                   ((CsrBtSapcPrim) (0x0005 + CSR_BT_SAPC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SAPC_RESET_SIM_CFM                      ((CsrBtSapcPrim) (0x0006 + CSR_BT_SAPC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SAPC_TRANSFER_CARD_READER_STATUS_CFM    ((CsrBtSapcPrim) (0x0007 + CSR_BT_SAPC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SAPC_SET_TRANSFER_PROTOCOL_CFM          ((CsrBtSapcPrim) (0x0008 + CSR_BT_SAPC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SAPC_STATUS_IND                         ((CsrBtSapcPrim) (0x0009 + CSR_BT_SAPC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SAPC_SECURITY_OUT_CFM                   ((CsrBtSapcPrim) (0x0010 + CSR_BT_SAPC_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_SAPC_PRIM_UPSTREAM_HIGHEST                            (0x0010 + CSR_BT_SAPC_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_SAPC_PRIM_DOWNSTREAM_COUNT              (CSR_BT_SAPC_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_SAPC_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_SAPC_PRIM_UPSTREAM_COUNT                (CSR_BT_SAPC_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_SAPC_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

/* Downstream messages */
typedef struct
{
    CsrBtSapcPrim              type;               /* primitive/message identity */
    CsrSchedQid                     appHandle;          /* application handle */
    BD_ADDR_T               bdAddr;             /* Bluetooth address of the device to connect */
    CsrUint16                maxMsgSize;         /* Max message size on the SAP link */
} CsrBtSapcConnectReq;

typedef struct
{
    CsrBtSapcPrim              type;               /* primitive/message identity */
} CsrBtSapcDisconnectReq;

typedef struct
{
    CsrBtSapcPrim              type;               /* primitive/message identity */
    CsrUint8                 *commandApdu;        /* Command APDU to send to server */
    CsrUint16                commandApduLength;   /* Length of command APDU */
    CsrBool                  apdu7816Type;        /* Used to indicate if the APDU is a 7816 type */
} CsrBtSapcTransferApduReq;

typedef struct
{
    CsrBtSapcPrim              type;               /* primitive/message identity */
} CsrBtSapcTransferAtrReq;

typedef struct
{
    CsrBtSapcPrim              type;               /* primitive/message identity */
} CsrBtSapcPowerSimOffReq;

typedef struct
{
    CsrBtSapcPrim              type;               /* primitive/message identity */
} CsrBtSapcPowerSimOnReq;

typedef struct
{
    CsrBtSapcPrim              type;               /* primitive/message identity */
} CsrBtSapcResetSimReq;

typedef struct
{
    CsrBtSapcPrim              type;               /* primitive/message identity */
} CsrBtSapcTransferCardReaderStatusReq;

typedef struct
{
    CsrBtSapcPrim              type;
    CsrUint8                 transportProtocol;
} CsrBtSapcSetTransferProtocolReq;

/* Upstream messages */
typedef struct
{
    CsrBtSapcPrim              type;               /* primitive/message identity */
    CsrUint16                maxMsgSize;         /* Max message size accepted by server */
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
    CsrBtConnId              btConnId;               /* Global Bluetooth connection ID */
} CsrBtSapcConnectCfm;

typedef struct
{
    CsrBtSapcPrim              type;               /* primitive/message identity */
    CsrBtResultCode         reasonCode;
    CsrBtSupplier     reasonSupplier;
} CsrBtSapcDisconnectInd;

typedef struct
{
    CsrBtSapcPrim              type;               /* primitive/message identity */
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
    CsrUint8                 *responseApdu;       /* APDU response from the server */
    CsrUint16                responseApduLength;  /* length of the response APDU */
} CsrBtSapcTransferApduCfm;

typedef struct
{
    CsrBtSapcPrim              type;               /* primitive/message identity */
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
    CsrUint8                 *atr;               /* the ATR */
    CsrUint16                atrLength;          /* length of ATR */
} CsrBtSapcTransferAtrCfm;

typedef struct
{
    CsrBtSapcPrim              type;               /* primitive/message identity */
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtSapcPowerSimOffCfm;

typedef struct
{
    CsrBtSapcPrim              type;               /* primitive/message identity */
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtSapcPowerSimOnCfm;

typedef struct
{
    CsrBtSapcPrim              type;               /* primitive/message identity */
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtSapcResetSimCfm;

typedef struct
{
    CsrBtSapcPrim              type;               /* primitive/message identity */
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
    CsrUint8                 cardReaderStatus;   /* the status of the card reader */
} CsrBtSapcTransferCardReaderStatusCfm;

typedef struct
{
    CsrBtSapcPrim              type;               /* primitive/message identity */
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtSapcSetTransferProtocolCfm;

typedef struct
{
    CsrBtSapcPrim              type;               /* primitive/message identity */
    CsrUint8                 statusChange;       /* new status of the SAP Server */
} CsrBtSapcStatusInd;

typedef struct
{
    CsrBtSapcPrim              type;
    CsrSchedQid                     appHandle;
    CsrUint16                secLevel;
} CsrBtSapcSecurityOutReq;

typedef struct
{
    CsrBtSapcPrim       type;
    CsrBtResultCode     resultCode;
    CsrBtSupplier resultSupplier;
} CsrBtSapcSecurityOutCfm;

typedef struct
{
    CsrBtSapcPrim              type;               /* primitive/message identity */
} CsrBtSapcHouseCleaning;

#ifdef __cplusplus
}
#endif

#endif

