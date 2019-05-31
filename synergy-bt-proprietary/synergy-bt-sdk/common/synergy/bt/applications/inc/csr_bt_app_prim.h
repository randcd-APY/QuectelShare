#ifndef CSR_BT_APP_PRIM_H__
#define CSR_BT_APP_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2012-2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************************
 * Primitive definitions for Synergy BT Applications
 ************************************************************************************/
#define CSR_SYNERGY_BT_APP_EVENT_CLASS_BASE        ((CsrUint16)(0x01A0))

#define CSR_BT_GAP_APP_PRIM                 ((CsrUint16) (0x0000 | CSR_SYNERGY_BT_APP_EVENT_CLASS_BASE))
#define CSR_BT_SPP_APP_PRIM                 ((CsrUint16) (0x0001 | CSR_SYNERGY_BT_APP_EVENT_CLASS_BASE))
#define CSR_BT_AV_APP_PRIM                  ((CsrUint16) (0x0002 | CSR_SYNERGY_BT_APP_EVENT_CLASS_BASE))
#define CSR_BT_AVRCP_APP_PRIM               ((CsrUint16) (0x0003 | CSR_SYNERGY_BT_APP_EVENT_CLASS_BASE))
#define CSR_BT_HFG_APP_PRIM                 ((CsrUint16) (0x0004 | CSR_SYNERGY_BT_APP_EVENT_CLASS_BASE))
#define CSR_BT_HF_APP_PRIM                  ((CsrUint16) (0x0005 | CSR_SYNERGY_BT_APP_EVENT_CLASS_BASE))
#define CSR_BT_FTS_APP_PRIM                 ((CsrUint16) (0x0006 | CSR_SYNERGY_BT_APP_EVENT_CLASS_BASE))
#define CSR_BT_FTC_APP_PRIM                 ((CsrUint16) (0x0007 | CSR_SYNERGY_BT_APP_EVENT_CLASS_BASE))
#define CSR_BT_PHDC_AG_APP_PRIM             ((CsrUint16) (0x0008 | CSR_SYNERGY_BT_APP_EVENT_CLASS_BASE))
#define CSR_BT_PHDC_MGR_APP_PRIM            ((CsrUint16) (0x0009 | CSR_SYNERGY_BT_APP_EVENT_CLASS_BASE))
#define CSR_BT_AMPWIFI_APP_PRIM             ((CsrUint16) (0x000a | CSR_SYNERGY_BT_APP_EVENT_CLASS_BASE))
#define CSR_BT_GATT_APP_PRIM                ((CsrUint16) (0x000b | CSR_SYNERGY_BT_APP_EVENT_CLASS_BASE))
#define CSR_BT_PROXC_APP_PRIM               ((CsrUint16) (0x000c | CSR_SYNERGY_BT_APP_EVENT_CLASS_BASE))
#define CSR_BT_PROXS_APP_PRIM               ((CsrUint16) (0x000d | CSR_SYNERGY_BT_APP_EVENT_CLASS_BASE))
#define CSR_BT_THERMC_APP_PRIM              ((CsrUint16) (0x000e | CSR_SYNERGY_BT_APP_EVENT_CLASS_BASE))
#define CSR_BT_THERMS_APP_PRIM              ((CsrUint16) (0x000f | CSR_SYNERGY_BT_APP_EVENT_CLASS_BASE))
#define CSR_BT_HIDC_APP_PRIM                ((CsrUint16) (0x0010 | CSR_SYNERGY_BT_APP_EVENT_CLASS_BASE))
#define CSR_BT_LE_BROWSER_APP_PRIM          ((CsrUint16) (0x0011 | CSR_SYNERGY_BT_APP_EVENT_CLASS_BASE))
#define CSR_BT_GENERIC_SERVER_APP_PRIM      ((CsrUint16) (0x0012 | CSR_SYNERGY_BT_APP_EVENT_CLASS_BASE))
#define CSR_BT_RSCC_APP_PRIM                ((CsrUint16) (0x0013 | CSR_SYNERGY_BT_APP_EVENT_CLASS_BASE))
#define CSR_BT_RSCS_APP_PRIM                ((CsrUint16) (0x0014 | CSR_SYNERGY_BT_APP_EVENT_CLASS_BASE))
#define CSR_BT_PAN_APP_PRIM                 ((CsrUint16) (0x0015 | CSR_SYNERGY_BT_APP_EVENT_CLASS_BASE))
#define CSR_BT_MPAA_APP_PRIM                ((CsrUint16) (0x0016 | CSR_SYNERGY_BT_APP_EVENT_CLASS_BASE))
#define CSR_BT_AUDIO_APP_PRIM               ((CsrUint16) (0x0017 | CSR_SYNERGY_BT_APP_EVENT_CLASS_BASE))


#define NUMBER_OF_CSR_BT_APP_EVENTS         ((CsrUint16) (0x0017 + 1)

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_APP_PRIM_H__ */

