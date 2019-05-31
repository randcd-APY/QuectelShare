#ifndef CSR_BT_DIS_COMMON_H_
#define CSR_BT_DIS_COMMON_H_

/*****************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
*****************************************************************************/

/* This file contains definitions for Device Information service */

#include "csr_synergy.h"
#include "csr_types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef CsrUint8 CsrBtDisVendorIdSource;
#define CSR_BT_DIS_VENDOR_ID_BLUETOOTH_SIG      ((CsrBtDisVendorIdSource) 1)    /* Bluetooth SIG assigned Company Identifier value from the Assigned Numbers document */
#define CSR_BT_DIS_VENDOR_ID_USB_FORUM          ((CsrBtDisVendorIdSource) 2)    /* USB Implementer's Forum assigned Vendor ID value */

/* The PnP_ID characteristic is a set of values used to create a device ID value
 * that is unique for this device. */
typedef struct
{
    CsrBtDisVendorIdSource  vendorIdSource; /* Identifies the source of the Vendor ID field */
    CsrUint16               vendorId;       /* Identifies the product vendor from the namespace in the Vendor ID Source */
    CsrUint16               productId;      /* Manufacturer managed identifier for this product */
    CsrUint16               productVersion; /* Manufacturer managed version for this product */
} CsrBtDisPnpId;

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_DIS_COMMON_H_ */
