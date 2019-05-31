#ifndef CSR_BT_SAPS_PRIM_H__
#define CSR_BT_SAPS_PRIM_H__

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

/* search_string="CsrBtSapsPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

/* ---------- Defines the SAPS CsrBtResultCode ----------*/

typedef CsrPrim          CsrBtSapsPrim;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_SAPS_PRIM_DOWNSTREAM_LOWEST                              (0x0000)

#define CSR_BT_SAPS_ACTIVATE_REQ                          ((CsrBtSapsPrim) (0x0000 + CSR_BT_SAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SAPS_DEACTIVATE_REQ                        ((CsrBtSapsPrim) (0x0001 + CSR_BT_SAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SAPS_CONNECT_RES                           ((CsrBtSapsPrim) (0x0002 + CSR_BT_SAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SAPS_DISCONNECT_REQ                        ((CsrBtSapsPrim) (0x0003 + CSR_BT_SAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SAPS_TRANSFER_APDU_RES                     ((CsrBtSapsPrim) (0x0004 + CSR_BT_SAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SAPS_TRANSFER_ATR_RES                      ((CsrBtSapsPrim) (0x0005 + CSR_BT_SAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SAPS_POWER_SIM_OFF_RES                     ((CsrBtSapsPrim) (0x0006 + CSR_BT_SAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SAPS_POWER_SIM_ON_RES                      ((CsrBtSapsPrim) (0x0007 + CSR_BT_SAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SAPS_RESET_SIM_RES                         ((CsrBtSapsPrim) (0x0008 + CSR_BT_SAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SAPS_TRANSFER_CARD_READER_STATUS_RES       ((CsrBtSapsPrim) (0x0009 + CSR_BT_SAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SAPS_SET_TRANSFER_PROTOCOL_RES             ((CsrBtSapsPrim) (0x000A + CSR_BT_SAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SAPS_SEND_STATUS_REQ                       ((CsrBtSapsPrim) (0x000B + CSR_BT_SAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SAPS_SECURITY_IN_REQ                       ((CsrBtSapsPrim) (0x000C + CSR_BT_SAPS_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_SAPS_PRIM_DOWNSTREAM_HIGHEST                             (0x000C + CSR_BT_SAPS_PRIM_DOWNSTREAM_LOWEST)

/* Not part of interface, used internally only */
#define CSR_BT_SAPS_HOUSE_CLEANING                        ((CsrBtSapsPrim) (0x000D + CSR_BT_SAPS_PRIM_DOWNSTREAM_LOWEST))

/*******************************************************************************/

#define CSR_BT_SAPS_PRIM_UPSTREAM_LOWEST                                (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_SAPS_ACTIVATE_CFM                          ((CsrBtSapsPrim) (0x0000 + CSR_BT_SAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SAPS_DEACTIVATE_CFM                        ((CsrBtSapsPrim) (0x0001 + CSR_BT_SAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SAPS_CONNECT_IND                           ((CsrBtSapsPrim) (0x0002 + CSR_BT_SAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SAPS_DISCONNECT_IND                        ((CsrBtSapsPrim) (0x0003 + CSR_BT_SAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SAPS_TRANSFER_APDU_IND                     ((CsrBtSapsPrim) (0x0004 + CSR_BT_SAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SAPS_TRANSFER_ATR_IND                      ((CsrBtSapsPrim) (0x0005 + CSR_BT_SAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SAPS_POWER_SIM_OFF_IND                     ((CsrBtSapsPrim) (0x0006 + CSR_BT_SAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SAPS_POWER_SIM_ON_IND                      ((CsrBtSapsPrim) (0x0007 + CSR_BT_SAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SAPS_RESET_SIM_IND                         ((CsrBtSapsPrim) (0x0008 + CSR_BT_SAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SAPS_TRANSFER_CARD_READER_STATUS_IND       ((CsrBtSapsPrim) (0x0009 + CSR_BT_SAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SAPS_SET_TRANSFER_PROTOCOL_IND             ((CsrBtSapsPrim) (0x000A + CSR_BT_SAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SAPS_SECURITY_IN_CFM                       ((CsrBtSapsPrim) (0x000B + CSR_BT_SAPS_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_SAPS_PRIM_UPSTREAM_HIGHEST                               (0x000B + CSR_BT_SAPS_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_SAPS_PRIM_DOWNSTREAM_COUNT                 (CSR_BT_SAPS_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_SAPS_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_SAPS_PRIM_UPSTREAM_COUNT                   (CSR_BT_SAPS_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_SAPS_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

/* Downstream messages */
typedef struct
{
    CsrBtSapsPrim               type;                 /* primitive/message identity */
    CsrSchedQid                      phandle;
} CsrBtSapsActivateReq;

typedef struct
{
    CsrBtSapsPrim               type;                 /* primitive/message identity */
} CsrBtSapsDeactivateReq;

typedef struct
{
    CsrBtSapsPrim            type;                 /* primitive/message identity */
    CsrBtResultCode          resultCode;           /* the connection result */
    CsrUint16                 maxMsgSize;           /* the CSRMAX message size accepted by server */
    CsrUint8                  cardStatus;           /* the status of the SIM card after reset */
} CsrBtSapsConnectRes;

typedef struct
{
    CsrBtSapsPrim               type;                 /* primitive/message identity */
    CsrUint8                  disconnectType;       /* the type of disconnect */
} CsrBtSapsDisconnectReq;

typedef struct
{
    CsrBtSapsPrim               type;                 /* primitive/message identity */
    CsrBtResultCode          resultCode;           /* the result of the ATR request */
    CsrUint8                  *atrResponse;         /* the ATR */
    CsrUint16                 atrResponseLength;    /* the length of the ATR */
} CsrBtSapsTransferAtrRes;

typedef struct
{
    CsrBtSapsPrim               type;                 /* primitive/message identity */
    CsrBtResultCode          resultCode;           /* the result of the ATR request */
    CsrUint8                  *apduResponse;        /* the response APDU to be send to client */
    CsrUint16                 apduResponseLength;   /* length of the response APDU */
} CsrBtSapsTransferApduRes;

typedef struct
{
    CsrBtSapsPrim               type;                 /* primitive/message identity */
    CsrBtResultCode          resultCode;           /* the result of the ATR request */
} CsrBtSapsPowerSimOffRes;

typedef struct
{
    CsrBtSapsPrim               type;                 /* primitive/message identity */
    CsrBtResultCode          resultCode;           /* the result of the ATR request */
} CsrBtSapsPowerSimOnRes;

typedef struct
{
    CsrBtSapsPrim               type;                 /* primitive/message identity */
    CsrBtResultCode          resultCode;           /* the result of the ATR request */
} CsrBtSapsResetSimRes;

typedef struct
{
    CsrBtSapsPrim               type;                 /* primitive/message identity */
    CsrBtResultCode          resultCode;           /* the result of the ATR request */
    CsrUint8                  cardReaderStatus;     /* the card reader status */
} CsrBtSapsTransferCardReaderStatusRes;

typedef struct
{
    CsrBtSapsPrim               type;
    CsrBtResultCode          resultCode;           /* the result of the ATR request */
} CsrBtSapsSetTransferProtocolRes;

typedef struct
{
    CsrBtSapsPrim               type;                 /* primitive/message identity */
    CsrUint8                  statusChange;         /* the status */
} CsrBtSapsSendStatusReq;

/* Upstream messages */
typedef struct
{
    CsrBtSapsPrim               type;                 /* primitive/message identity */
} CsrBtSapsActivateCfm;

typedef struct
{
    CsrBtSapsPrim               type;                 /* primitive/message identity */
} CsrBtSapsDeactivateCfm;

typedef struct
{
    CsrBtSapsPrim               type;                 /* primitive/message identity */
    CsrUint16                 maxMsgSize;           /* the CSRMAX message size suggested by client */
    CsrBtDeviceAddr             deviceAddr;
    CsrBtConnId                btConnId;             /* Global Bluetooth connection ID */
} CsrBtSapsConnectInd;

typedef struct
{
    CsrBtSapsPrim               type;                 /* primitive/message identity */
    CsrBtResultCode          reasonCode;
    CsrBtSupplier      reasonSupplier;
} CsrBtSapsDisconnectInd;

typedef struct
{
    CsrBtSapsPrim               type;                 /* primitive/message identity */
} CsrBtSapsTransferAtrInd;

typedef struct
{
    CsrBtSapsPrim               type;                 /* primitive/message identity */
    CsrUint8                  *apduCommand;           /* command APDU from client */
    CsrUint16                 apduCommandLength;      /* length of the command APDU */
    CsrBool                   isApdu7816Type;
} CsrBtSapsTransferApduInd;

typedef struct
{
    CsrBtSapsPrim               type;                 /* primitive/message identity */
} CsrBtSapsPowerSimOffInd;

typedef struct
{
    CsrBtSapsPrim               type;                 /* primitive/message identity */
} CsrBtSapsPowerSimOnInd;

typedef struct
{
    CsrBtSapsPrim               type;                 /* primitive/message identity */
} CsrBtSapsResetSimInd;

typedef struct
{
    CsrBtSapsPrim               type;                 /* primitive/message identity */
} CsrBtSapsTransferCardReaderStatusInd;

typedef struct
{
    CsrBtSapsPrim               type;
    CsrUint8                  protocol;
} CsrBtSapsSetTransferProtocolInd;

typedef struct
{
    CsrBtSapsPrim               type;
    CsrSchedQid                      appHandle;
    CsrUint16                 secLevel;
} CsrBtSapsSecurityInReq;

typedef struct
{
    CsrBtSapsPrim            type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtSapsSecurityInCfm;

typedef struct
{
    CsrBtSapsPrim               type;                 /* primitive/message identity */
} CsrBtSapsHouseCleaning;

#ifdef __cplusplus
}
#endif

#endif
