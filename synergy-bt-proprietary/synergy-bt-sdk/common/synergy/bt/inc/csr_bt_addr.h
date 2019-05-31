#ifndef CSR_BT_ADDR_H__
#define CSR_BT_ADDR_H__

/******************************************************************************

Copyright (c) 2010-2018 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "bluetooth.h"
#include "tbdaddr.h"
#include "hci_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Remap tbdaddr interface to Synergy compliant names.  This is to
 * allow for future changes without breaking the API */
typedef TYPED_BD_ADDR_T CsrBtTypedAddr;
typedef TP_BD_ADDR_T CsrBtTpdAddrT;
typedef CsrUint8 CsrBtAddressType;
typedef PHYSICAL_TRANSPORT_T CsrBtTransportType;
typedef CsrUint8 CsrBtOwnAddressType;

#define CSR_BT_ADDR_PUBLIC  TBDADDR_PUBLIC
#define CSR_BT_ADDR_PRIVATE TBDADDR_RANDOM
#define CSR_BT_ADDR_INVALID TBDADDR_INVALID

/* Valid CsrBtTransportType values */
#define CSR_BT_TRANSPORT_BREDR    (BREDR_ACL)
#define CSR_BT_TRANSPORT_LE       (LE_ACL)

/* Resolvable Private Address Min/Max & Invalid Timeout values */
#define CSR_BT_RPA_TIMEOUT_MIN         (0x0001)
#define CSR_BT_RPA_TIMEOUT_MAX         (0xA1B8)
#define CSR_BT_RPA_TIMEOUT_INVALID     (0xFFFF)

/* Write Application's provided static address */
#define CSR_BT_LE_WRITE_STATIC_ADDRESS ((CsrUint8)DM_SM_ADDRESS_WRITE_STATIC)
/* Generate Static address */
#define CSR_BT_LE_GEN_STATIC_ADDRESS   ((CsrUint8)DM_SM_ADDRESS_GENERATE_STATIC)
/* Generate Non-resolvable Private address */
#define CSR_BT_LE_GEN_NRPA             ((CsrUint8)DM_SM_ADDRESS_GENERATE_NON_RESOLVABLE)
/* Generate Resolvable Private address */
#define CSR_BT_LE_GEN_RPA              ((CsrUint8)DM_SM_ADDRESS_GENERATE_RESOLVABLE)

/* LE Own Address type */
#define CSR_BT_ADDR_TYPE_PUBLIC        ((CsrBtOwnAddressType)HCI_ULP_ADDRESS_PUBLIC)
#define CSR_BT_ADDR_TYPE_RANDOM        ((CsrBtOwnAddressType)HCI_ULP_ADDRESS_RANDOM)
#ifdef CSR_BT_INSTALL_LE_PRIVACY_1P2_SUPPORT
#define CSR_BT_ADDR_TYPE_RPA_OR_PUBLIC ((CsrBtOwnAddressType)HCI_ULP_ADDRESS_GENERATE_RPA_FBP)
#define CSR_BT_ADDR_TYPE_RPA_OR_RANDOM ((CsrBtOwnAddressType)HCI_ULP_ADDRESS_GENERATE_RPA_FBR)
#endif

/* Subfields */
#define CsrBtAddrType(addrt) TBDADDR_TYPE((addrt))
#define CsrBtAddrAddr(addrt) TBDADDR_ADDR((addrt))
#define CsrBtAddrLap(addrt) TBDADDR_LAP((addrt))
#define CsrBtAddrUap(addrt) TBDADDR_UAP((addrt))
#define CsrBtAddrNap(addrt) TBDADDR_NAP((addrt))
#define CsrBtAddrInvalidate(addrt) TBDADDR_INVALIDATE((addrt))
#define CsrBtAddrZero(addrt) CsrMemSet((addrt), 0, sizeof(CsrBtTypedAddr))

/* Tests */
#define CsrBtAddrIsPublic(addrt) TBDADDR_IS_PUBLIC((addrt))
#define CsrBtAddrIsRandom(addrt) TBDADDR_IS_RANDOM((addrt))
#define CsrBtAddrIsPrivate(addrt) TBDADDR_IS_PRIVATE((addrt)) 
#define CsrBtAddrIsStatic(addrt) tbdaddr_is_static((addrt))
#define CsrBtAddrIsValid(addrt) tbdaddr_is_valid((addrt))
#define CsrBtAddrIsPrivateResolvable(addrt) tbdaddr_is_private_resolvable((addrt))
#define CsrBtAddrIsPrivateNonresolvable(addrt) tbdaddr_is_private_nonresolvable((addrt))
#define CsrBtAddrIsZero(addrt) (CsrBtBdAddrEqZero((addrt.addr)) && CsrBtAddrIsPublic((*addrt)))

/* Compare */
#define CsrBtAddrEq(a,b) tbdaddr_eq((a), (b))
#define CsrBtAddrEqWithType(addrt,type,bd) tbdaddr_eq_bd_addr((addrt), (type), (bd))

/* Copy */
#define CsrBtAddrCopy(dst,src) tbdaddr_copy((dst), (src))
#define CsrBtAddrCopyWithType(dst,type,src) tbdaddr_copy_from_bd_addr((dst), (type), (src))
#define CsrBtAddrCopyFromPublic(dst,src) tbdaddr_copy_from_public_bd_addr((dst), (src))
#define CsrBtAddrCopyToPublic(dst,src) tbdaddr_copy_to_bd_addr((dst), (src))

#ifdef __cplusplus
}
#endif

#endif  /* CSR_BT_ADDR_H__ */
