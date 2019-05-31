#ifndef CSR_BT_HIDS_COMMON_H_
#define CSR_BT_HIDS_COMMON_H_

/*****************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
*****************************************************************************/

/* This file contains definitions for HID service */

#include "csr_synergy.h"
#include "csr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef CsrUint8 CsrBtHidsProtocol;
#define CSR_BT_HIDS_BOOT_PROTOCOL           ((CsrBtHidsProtocol) 0x00)
#define CSR_BT_HIDS_REPORT_PROTOCOL         ((CsrBtHidsProtocol) 0x01)

/* HID report type values defined by HID service specification */
#define CSR_BT_HIDS_REPORT_TYPE_INPUT                   (0x01)
#define CSR_BT_HIDS_REPORT_TYPE_OUTPUT                  (0x02)
#define CSR_BT_HIDS_REPORT_TYPE_FEATURE                 (0x03)

typedef CsrUint8 CsrBtHidsControlPoint;
#define CSR_BT_HIDS_SUSPEND         ((CsrBtHidsControlPoint) 0x00)
#define CSR_BT_HIDS_EXIT_SUSPEND    ((CsrBtHidsControlPoint) 0x01)

typedef CsrUint8 CsrBtHidsInformationFlags;
#define CSR_BT_HIDS_INFO_FLAGS_REMOTE_WAKE          ((CsrBtHidsInformationFlags) 0x01) /* The device is designed to be capable of providing wake-up signal to a HID host */
#define CSR_BT_HIDS_INFO_FLAGS_NORMALLY_CONNECTABLE ((CsrBtHidsInformationFlags) 0x02) /* The device is normally connectable */

typedef struct
{
    CsrUint16 bccHid;       /* Version number of base USB HID Specification implemented by HID Device */
    CsrUint8 bCountryCode;  /* Identifies which country the hardware is localized for.
                               Most hardware is not localized and thus this value would be zero (0).
                               Refer USB Device Class Definition for Human Interface Devices in USB HID spec */
    CsrBtHidsInformationFlags flags;
} CsrBtHidsInformation;


#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_HIDS_COMMON_H_ */
