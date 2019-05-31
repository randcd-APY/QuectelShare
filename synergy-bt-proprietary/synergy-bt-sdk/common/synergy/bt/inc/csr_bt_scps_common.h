#ifndef CSR_BT_SCPS_COMMON_H_
#define CSR_BT_SCPS_COMMON_H_

/*****************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
*****************************************************************************/

/* This file contains definitions for Scan Parameter service */

#include "csr_synergy.h"
#include "csr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    CsrUint16 scanInterval;
    CsrUint16 scanWindow;
} CsrBtScanParamIntervalWindow;

/* Scan refresh characteristic value to be sent in notification */
#define CSR_BT_SCPS_SCAN_PARAM_REFRESH              ((CsrUint8) 0x00)

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_SCPS_COMMON_H_ */
