#ifndef CSR_BT_SC_PRIVATE_PRIM_H__
#define CSR_BT_SC_PRIVATE_PRIM_H__
/******************************************************************************
 Copyright (c) 2010-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.

 REVISION:      $Revision: #7 $
******************************************************************************/


#include "csr_synergy.h"
#include "csr_bt_sc_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Refresh the encryption. See dm_prim.h */
#define CSR_BT_SC_LE_REPAIR                        ((CsrUint16) DM_SM_SECURITY_REFRESH_ENCRYPTION)

/* search_string="CsrBtScPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

/* Downstream */
#define CSR_BT_SC_LE_SECURITY_REQ                  ((CsrBtScPrim) (0x0000 + CSR_BT_SC_PRIM_DOWNSTREAM_COUNT))
#define CSR_BT_SC_LE_ADD_SERVICE_CHANGED_REQ       ((CsrBtScPrim) (0x0001 + CSR_BT_SC_PRIM_DOWNSTREAM_COUNT))
#define CSR_BT_SC_LE_READ_CAR_CHAR_VALUE_REQ       ((CsrBtScPrim) (0x0002 + CSR_BT_SC_PRIM_DOWNSTREAM_COUNT))

/* Upstream */
#define CSR_BT_SC_LE_SECURITY_CFM                  ((CsrBtScPrim) ((0x0000 + CSR_BT_SC_PRIM_UPSTREAM_COUNT) | CSR_BT_SC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SC_LE_READ_CAR_VALUE_CFM            ((CsrBtScPrim) ((0x0001 + CSR_BT_SC_PRIM_UPSTREAM_COUNT) | CSR_BT_SC_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_SC_ALL_DOWNSTREAM_COUNT              (CSR_BT_SC_LE_READ_CAR_CHAR_VALUE_REQ + 1)

#define CSR_BT_SC_PRIVATE_PRIM (CSR_BT_SC_PRIM)

/* ---------- End of Primitives ----------*/
typedef struct
{
    CsrBtScPrim             type;                       /* Primitive/message identity */
    CsrSchedQid             phandle;                    /* Application handle */
    CsrUint16               securityRequirements;       /* LE security requirements. Note that 'key distribution' may
                                                         * automatically turn 'bonding' on or off */
    CsrBtTypedAddr          address;                    /* Address of device to bond with. */
    CsrUint16               l2caConFlags;               /* L2CAP connection flags */
} CsrBtScLeSecurityReq;

typedef CsrBtScLeSecurityInd CsrBtScLeSecurityCfm;

typedef struct
{
    CsrBtScPrim             type;                       /* Primitive/message identity */
    CsrBtDeviceAddr         addr;
    CsrBtAddressType        addressType;
    CsrUint32               serviceChangedHandle;
    CsrUint16               clientConfigValue;
} CsrBtScLeAddServiceChangedReq;

typedef struct
{
    CsrBtScPrim             type;                       /* Primitive/message identity */
    CsrSchedQid             phandle;                    /* Application handle */
    CsrBtTypedAddr          address;                    /* Peer device address whose CAR value to be read */
} CsrBtScLeReadCarCharValueReq;

typedef struct
{
    CsrBtScPrim             type;                       /* Primitive/message identity */
    CsrBtTypedAddr          address;                    /* Peer device address whose CAR value asked for */
    CsrUint8                value;                      /* CAR value */
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtScLeReadCarCharValueCfm;

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_SC_PRIVATE_PRIM_H__ */

