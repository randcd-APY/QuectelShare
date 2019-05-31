#ifndef CSR_BT_RESULT_H__
#define CSR_BT_RESULT_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
****************************************************************************/

#include "csr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef CsrUint16 CsrBtResultCode;
typedef CsrUint16 CsrBtReasonCode;
typedef CsrUint16 CsrBtSupplier;

/*************************************************************************************
               Denotes which supplier the CsrBtResultCode originated from
************************************************************************************/
#define CSR_BT_SUPPLIER_HCI                  ((CsrBtSupplier) (0x0000))
#define CSR_BT_SUPPLIER_DM                   ((CsrBtSupplier) (0x0001))
#define CSR_BT_SUPPLIER_L2CAP_CONNECT        ((CsrBtSupplier) (0x0002))
#define CSR_BT_SUPPLIER_L2CAP_MOVE           ((CsrBtSupplier) (0x0003))
#define CSR_BT_SUPPLIER_L2CAP_DISCONNECT     ((CsrBtSupplier) (0x0004))
#define CSR_BT_SUPPLIER_L2CAP_DATA           ((CsrBtSupplier) (0x0005))
#define CSR_BT_SUPPLIER_L2CAP_MISC           ((CsrBtSupplier) (0x0006))
#define CSR_BT_SUPPLIER_SDP_SDS              ((CsrBtSupplier) (0x0007))
#define CSR_BT_SUPPLIER_SDP_SDC              ((CsrBtSupplier) (0x0008))
#define CSR_BT_SUPPLIER_SDP_SDC_OPEN_SEARCH  ((CsrBtSupplier) (0x0009))
#define CSR_BT_SUPPLIER_RFCOMM               ((CsrBtSupplier) (0x000A))
#define CSR_BT_SUPPLIER_BCCMD                ((CsrBtSupplier) (0x000B))
#define CSR_BT_SUPPLIER_CM                   ((CsrBtSupplier) (0x000C))
#define CSR_BT_SUPPLIER_IRDA_OBEX            ((CsrBtSupplier) (0x000D))
#define CSR_BT_SUPPLIER_SPP                  ((CsrBtSupplier) (0x000E))
#define CSR_BT_SUPPLIER_SD                   ((CsrBtSupplier) (0x000F))
#define CSR_BT_SUPPLIER_HF                   ((CsrBtSupplier) (0x0010))
#define CSR_BT_SUPPLIER_AVRCP                ((CsrBtSupplier) (0x0011)) /* CSR AVRCP API error codes */
#define CSR_BT_SUPPLIER_AV                   ((CsrBtSupplier) (0x0014)) /* CSR AV API error codes */
#define CSR_BT_SUPPLIER_BSL                  ((CsrBtSupplier) (0x0015))
#define CSR_BT_SUPPLIER_DUNC                 ((CsrBtSupplier) (0x0016))
#define CSR_BT_SUPPLIER_HDP                  ((CsrBtSupplier) (0x0017))
#define CSR_BT_SUPPLIER_HFG                  ((CsrBtSupplier) (0x0018))
#define CSR_BT_SUPPLIER_HIDD                 ((CsrBtSupplier) (0x0019))
#define CSR_BT_SUPPLIER_HIDH                 ((CsrBtSupplier) (0x001A))
#define CSR_BT_SUPPLIER_SAPC                 ((CsrBtSupplier) (0x001B))
#define CSR_BT_SUPPLIER_SAPS                 ((CsrBtSupplier) (0x001C))
#define CSR_BT_SUPPLIER_DG                   ((CsrBtSupplier) (0x001D))
#define CSR_BT_SUPPLIER_HCRP                 ((CsrBtSupplier) (0x001E))
#define CSR_BT_SUPPLIER_BNEP                 ((CsrBtSupplier) (0x001F))
#define CSR_BT_SUPPLIER_BNEP_CONNECT_CFM     ((CsrBtSupplier) (0x0020))
#define CSR_BT_SUPPLIER_SC                   ((CsrBtSupplier) (0x0021))
#define CSR_BT_SUPPLIER_MCAP                 ((CsrBtSupplier) (0x0022))
#define CSR_BT_SUPPLIER_MCAP_PROTOCOL        ((CsrBtSupplier) (0x0023))
#define CSR_BT_SUPPLIER_OBEX_PROFILES        ((CsrBtSupplier) (0x0024))
#define CSR_BT_SUPPLIER_JSR82                ((CsrBtSupplier) (0x0025))
#define CSR_BT_SUPPLIER_AT                   ((CsrBtSupplier) (0x0026))
#define CSR_BT_SUPPLIER_A2DP                 ((CsrBtSupplier) (0x0027)) /* GAVDP, AVDTP, and A2DP spec error codes */
#define CSR_BT_SUPPLIER_AVCTP                ((CsrBtSupplier) (0x0028)) /* AVCTP/AVRCP protocol spec error codes */
#define CSR_BT_SUPPLIER_AVC                  ((CsrBtSupplier) (0x0029))
#define CSR_BT_SUPPLIER_AMPM                 ((CsrBtSupplier) (0x002A))
#define CSR_BT_SUPPLIER_PHDC                 ((CsrBtSupplier) (0x002B))
#define CSR_BT_SUPPLIER_PHDC_AG              ((CsrBtSupplier) (0x002C))
#define CSR_BT_SUPPLIER_ATT                  ((CsrBtSupplier) (0x002D))
#define CSR_BT_SUPPLIER_GATT                 ((CsrBtSupplier) (0x002E))
#define CSR_BT_SUPPLIER_PROX_SRV             ((CsrBtSupplier) (0x002F))
#define CSR_BT_SUPPLIER_THERM_SRV            ((CsrBtSupplier) (0x0030))
#define CSR_BT_SUPPLIER_GNSS_CLIENT          ((CsrBtSupplier) (0x0031))
#define CSR_BT_SUPPLIER_GNSS_SERVER          ((CsrBtSupplier) (0x0032))
#define CSR_BT_SUPPLIER_AVRCP_IMAGING        ((CsrBtSupplier) (0x0033))
#define CSR_BT_SUPPLIER_HOGH                 ((CsrBtSupplier) (0x0034))


#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_RESULT_H__ */
