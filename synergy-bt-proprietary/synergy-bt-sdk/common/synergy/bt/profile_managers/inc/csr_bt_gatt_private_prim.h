#ifndef CSR_BT_GATT_PRIVATE_PRIM_H__
#define CSR_BT_GATT_PRIVATE_PRIM_H__
/******************************************************************************
 Copyright (c) 2010-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.

 REVISION:      $Revision: #12 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_bt_gatt_prim.h"
#include "csr_bt_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/* GATT Central Address Resolution characteristic value */
#define CSR_BT_GATT_CAR_NOT_SUPPORTED             ((CsrUint8)(0x00))
#define CSR_BT_GATT_CAR_SUPPORTED                 ((CsrUint8)(0x01))
#define CSR_BT_GATT_CAR_VALUE_INVALID             ((CsrUint8)(0xFF))

/* GATT RPA Only characteristic value */
#define CSR_BT_GATT_RPA_ONLY_VALUE_SET            ((CsrUint8)(0x00))
#define CSR_BT_GATT_RPA_ONLY_VALUE_INVALID        ((CsrUint8)(0xFF))

/* search_string="CsrBtGattPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */
#define CSR_BT_GATT_PRIM_DOWNSTREAM_PRIVATE_LOWEST  (0x0100)

/*  Not part of interface, used internally by GATT only */
#define CSR_BT_GATT_EXCHANGE_MTU_REQ              ((CsrBtGattPrim)(0x0000 + CSR_BT_GATT_PRIM_DOWNSTREAM_PRIVATE_LOWEST))
#define CSR_BT_GATT_PRIVATE_UNREGISTER_REQ        ((CsrBtGattPrim)(0x0001 + CSR_BT_GATT_PRIM_DOWNSTREAM_PRIVATE_LOWEST))
#define CSR_BT_GATT_SDS_REQ                       ((CsrBtGattPrim)(0x0002 + CSR_BT_GATT_PRIM_DOWNSTREAM_PRIVATE_LOWEST))
#define CSR_BT_GATT_READ_DEVICE_INFO_REQ          ((CsrBtGattPrim)(0x0003 + CSR_BT_GATT_PRIM_DOWNSTREAM_PRIVATE_LOWEST))
/*  Part of interface private interface, used internally by SC only */
#define CSR_BT_GATT_READ_REMOTE_LE_NAME_REQ       ((CsrBtGattPrim)(0x0004 + CSR_BT_GATT_PRIM_DOWNSTREAM_PRIVATE_LOWEST))
#define CSR_BT_GATT_READ_REMOTE_CAR_CHAR_REQ      ((CsrBtGattPrim)(0x0005 + CSR_BT_GATT_PRIM_DOWNSTREAM_PRIVATE_LOWEST))
#define CSR_BT_GATT_READ_REMOTE_RPA_ONLY_CHAR_REQ ((CsrBtGattPrim)(0x0006 + CSR_BT_GATT_PRIM_DOWNSTREAM_PRIVATE_LOWEST))

#define CSR_BT_GATT_PRIM_UPSTREAM_PRIVATE_LOWEST   (CSR_BT_GATT_PRIM_DOWNSTREAM_PRIVATE_LOWEST + CSR_PRIM_UPSTREAM)

#define CSR_BT_GATT_READ_REMOTE_LE_NAME_CFM       ((CsrBtGattPrim)(0x0000 + CSR_BT_GATT_PRIM_UPSTREAM_PRIVATE_LOWEST))
#define CSR_BT_GATT_READ_REMOTE_CAR_CHAR_CFM      ((CsrBtGattPrim)(0x0001 + CSR_BT_GATT_PRIM_UPSTREAM_PRIVATE_LOWEST))
#define CSR_BT_GATT_READ_REMOTE_RPA_ONLY_CHAR_CFM ((CsrBtGattPrim)(0x0002 + CSR_BT_GATT_PRIM_UPSTREAM_PRIVATE_LOWEST))
#define CSR_BT_GATT_PRIVATE_PRIM (CSR_BT_GATT_PRIM)

/*******************************************************************************
 * Structures for internal use GATT only
 *******************************************************************************/
typedef struct
{
    CsrBtGattPrim       type;                   /* CSR_BT_GATT_EXCHANGE_MTU_REQ */
    CsrBtConnId         btConnId;               /* Connection identifier */
    CsrUint16           mtu;                    /* Client receive MTU size */
    CsrBtTypedAddr      address;                /* Peer Address */
} CsrBtGattExchangeMtuReq;

typedef struct
{
    CsrBtGattPrim       type;                   /* CSR_BT_GATT_PRIVATE_UNREGISTER_REQ */
    CsrBtGattId         gattId;                 /* The application handle */
    CsrBtConnId         btConnId;               /* Connection identifier */
} CsrBtGattPrivateUnregisterReq;

typedef struct
{
    CsrBtGattPrim       type;                   /* CSR_BT_GATT_EXCHANGE_MTU_REQ */
    CsrBtTypedAddr      address;                /* Peer Address */
} CsrBtGattReadDeviceInfoReq;

typedef struct
{
    CsrBtGattPrim       type;                   /* CSR_BT_GATT_SDS_REQ */
} CsrBtGattSdsReq;

/* Note this will only work if GATT is connected to address */
typedef struct
{
    CsrBtGattPrim       type;                   /* CSR_BT_GATT_READ_REMOTE_LE_NAME_REQ */
    CsrSchedQid         qid;                    /* Application handle */
    CsrBtTypedAddr      address;                /* Peer Address */
} CsrBtGattReadRemoteLeNameReq;

typedef struct
{
    CsrBtGattPrim       type;                   /* CSR_BT_GATT_READ_REMOTE_LE_NAME_CFM */
    CsrUint16           remoteNameLength;       /* The length of the remote name */
    CsrUint8            *remoteName;            /* The remote name */
} CsrBtGattReadRemoteLeNameCfm;

typedef struct
{
    CsrBtGattPrim       type;                   /* CSR_BT_GATT_READ_REMOTE_CAR_CHAR_REQ */
    CsrBtTypedAddr      address;                /* Peer Address */
} CsrBtGattReadRemoteCarCharReq;

typedef struct
{
    CsrBtGattPrim       type;                   /* CSR_BT_GATT_READ_REMOTE_RPA_ONLY_CHAR_REQ */
    CsrBtTypedAddr      address;                /* Peer Address */
} CsrBtGattReadRemoteRpaOnlyCharReq;

typedef struct
{
    CsrBtGattPrim    type;                   /* CSR_BT_GATT_READ_REMOTE_CAR_CHAR_CFM */
    CsrBtTypedAddr   address;                /* Peer Address */
    CsrUint8         carValue;               /* CAR Value */
    CsrBtResultCode  resultCode;
    CsrBtSupplier    resultSupplier;
} CsrBtGattReadRemoteCarCharCfm;

typedef struct
{
    CsrBtGattPrim    type;                   /* CSR_BT_GATT_READ_REMOTE_RPA_ONLY_CHAR_CFM */
    CsrBtTypedAddr   address;                /* Peer Address */
    CsrUint8         rpaOnlyValue;           /* RPA only Value */
    CsrBtResultCode  resultCode;
    CsrBtSupplier    resultSupplier;
} CsrBtGattReadRemoteRpaOnlyCharCfm;

void CsrBtGattPrivateFreeUpstreamMessageContents(CsrUint16 eventClass, void *message);

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_GATT_PRIVATE_PRIM_H__ */
