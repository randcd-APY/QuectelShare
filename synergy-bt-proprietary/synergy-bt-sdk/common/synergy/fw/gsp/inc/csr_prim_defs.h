#ifndef CSR_PRIM_DEFS_H__
#define CSR_PRIM_DEFS_H__
/*****************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************************
 * Segmentation of primitives in upstream and downstream segment
 ************************************************************************************/
typedef CsrUint16 CsrPrim;
#define CSR_PRIM_UPSTREAM                   ((CsrPrim) (0x8000))

/************************************************************************************
 * Primitive definitions for Synergy framework
 ************************************************************************************/
#define CSR_SYNERGY_EVENT_CLASS_BASE        ((CsrUint16) (0x0600))

#define CSR_HCI_PRIM                        ((CsrUint16) (0x0000 | CSR_SYNERGY_EVENT_CLASS_BASE))
#define CSR_BCCMD_PRIM                      ((CsrUint16) (0x0001 | CSR_SYNERGY_EVENT_CLASS_BASE))
#define CSR_HQ_PRIM                         ((CsrUint16) (0x0002 | CSR_SYNERGY_EVENT_CLASS_BASE))
#define CSR_VM_PRIM                         ((CsrUint16) (0x0003 | CSR_SYNERGY_EVENT_CLASS_BASE))
#define CSR_TM_BLUECORE_PRIM                ((CsrUint16) (0x0004 | CSR_SYNERGY_EVENT_CLASS_BASE))
#define CSR_FP_PRIM                         ((CsrUint16) (0x0005 | CSR_SYNERGY_EVENT_CLASS_BASE))
#define CSR_IP_SOCKET_PRIM                  ((CsrUint16) (0x0006 | CSR_SYNERGY_EVENT_CLASS_BASE))
#define CSR_IP_ETHER_PRIM                   ((CsrUint16) (0x0007 | CSR_SYNERGY_EVENT_CLASS_BASE))
#define CSR_IP_IFCONFIG_PRIM                ((CsrUint16) (0x0008 | CSR_SYNERGY_EVENT_CLASS_BASE))
#define CSR_IP_INTERNAL_PRIM                ((CsrUint16) (0x0009 | CSR_SYNERGY_EVENT_CLASS_BASE))
#define CSR_FSAL_PRIM                       ((CsrUint16) (0x000A | CSR_SYNERGY_EVENT_CLASS_BASE))
#define CSR_DATA_STORE_PRIM                 ((CsrUint16) (0x000B | CSR_SYNERGY_EVENT_CLASS_BASE))
#define CSR_AM_PRIM                         ((CsrUint16) (0x000C | CSR_SYNERGY_EVENT_CLASS_BASE))
#define CSR_TLS_PRIM                        ((CsrUint16) (0x000D | CSR_SYNERGY_EVENT_CLASS_BASE))
#define CSR_DHCP_SERVER_PRIM                ((CsrUint16) (0x000E | CSR_SYNERGY_EVENT_CLASS_BASE))
#define CSR_TFTP_PRIM                       ((CsrUint16) (0x000F | CSR_SYNERGY_EVENT_CLASS_BASE))
#define CSR_DSPM_PRIM                       ((CsrUint16) (0x0010 | CSR_SYNERGY_EVENT_CLASS_BASE))
#define CSR_TLS_INTERNAL_PRIM               ((CsrUint16) (0x0011 | CSR_SYNERGY_EVENT_CLASS_BASE))
#define CSR_SSD_PRIM                        ((CsrUint16) (0x0012 | CSR_SYNERGY_EVENT_CLASS_BASE))
#define CSR_SSD_INTERNAL_PRIM               ((CsrUint16) (0x0013 | CSR_SYNERGY_EVENT_CLASS_BASE))
#define CSR_QVSC_PRIM                       ((CsrUint16) (0x0014 | CSR_SYNERGY_EVENT_CLASS_BASE))

#define CSR_SYNERGY_EVENT_CLASS_MISC_BASE   ((CsrUint16) (0x06A0))

#define CSR_UI_PRIM                         ((CsrUint16) (0x0000 | CSR_SYNERGY_EVENT_CLASS_MISC_BASE))
#define CSR_APP_PRIM                        ((CsrUint16) (0x0001 | CSR_SYNERGY_EVENT_CLASS_MISC_BASE))
#define CSR_SDIO_PROBE_PRIM                 ((CsrUint16) (0x0002 | CSR_SYNERGY_EVENT_CLASS_MISC_BASE))

#define CSR_ENV_PRIM                        ((CsrUint16) (0x00FF | CSR_SYNERGY_EVENT_CLASS_MISC_BASE))

typedef struct
{
    CsrPrim type;
} CsrPrimType;

#ifdef __cplusplus
}
#endif

#endif /* CSR_PRIM_DEFS_H__ */
