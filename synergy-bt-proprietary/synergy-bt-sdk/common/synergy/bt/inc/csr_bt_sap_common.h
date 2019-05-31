#ifndef CSR_BT_SAP_COMMON_H__
#define CSR_BT_SAP_COMMON_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/* ---------- Defines the SAP CsrBtResultCode ----------*/
#define CSR_BT_RESULT_CODE_SAP_SUCCESS                                 ((CsrBtResultCode) (0x0))
#define CSR_BT_RESULT_CODE_SAP_UNSPECIFICED_ERROR                      ((CsrBtResultCode) (0x1))
#define CSR_BT_RESULT_CODE_SAP_UNACCEPTABLE_PARAMETER                  ((CsrBtResultCode) (0x2))

#define CSR_BT_RESULT_CODE_SAP_CONNECTION_SUCCESS                      ((CsrBtResultCode) (0x3))
#define CSR_BT_RESULT_CODE_SAP_CONNECTION_OK_ONGOING_CALL              ((CsrBtResultCode) (0x4))
#define CSR_BT_RESULT_CODE_SAP_CONNECTION_ATTEMPT_FAILED               ((CsrBtResultCode) (0x5))
#define CSR_BT_RESULT_CODE_SAP_CONNECTION_MAX_MSG_SIZE_NOT_SUPPORTED   ((CsrBtResultCode) (0x6))
#define CSR_BT_RESULT_CODE_SAP_CONNECTION_MSG_SIZE_TO_SMALL            ((CsrBtResultCode) (0x7))

#define CSR_BT_RESULT_CODE_SAP_IMMEDIATE_CONNECTION_TERM_BY_PEER       ((CsrBtResultCode) (0x8))
#define CSR_BT_RESULT_CODE_SAP_GRACEFUL_CONNECTION_TERM_BY_PEER        ((CsrBtResultCode) (0x9))
#define CSR_BT_RESULT_CODE_SAP_CONNECTION_TERM_BY_SELF                 ((CsrBtResultCode) (0xa))

#define CSR_BT_RESULT_CODE_SAP_PROTOCOL_SUCCESS                        ((CsrBtResultCode) (0xb))
#define CSR_BT_RESULT_CODE_SAP_PROTOCOL_UNSPECIFICED_ERROR             ((CsrBtResultCode) (0xc))
#define CSR_BT_RESULT_CODE_SAP_PROTOCOL_CARD_NOT_ACC                   ((CsrBtResultCode) (0xd))
#define CSR_BT_RESULT_CODE_SAP_PROTOCOL_CARD_POW_OFF                   ((CsrBtResultCode) (0xe))
#define CSR_BT_RESULT_CODE_SAP_PROTOCOL_CARD_REMOVED                   ((CsrBtResultCode) (0xf))
#define CSR_BT_RESULT_CODE_SAP_PROTOCOL_CARD_POW_ON                    ((CsrBtResultCode) (0x10))
#define CSR_BT_RESULT_CODE_SAP_PROTOCOL_DATA_NOT_AVAIL                 ((CsrBtResultCode) (0x11))
#define CSR_BT_RESULT_CODE_SAP_PROTOCOL_NOT_SUPPORTED                  ((CsrBtResultCode) (0x12))
#define CSR_BT_RESULT_CODE_SAP_PROTOCOL_INVALID_MSG                    ((CsrBtResultCode) (0x13))

/* Defines for cardStatus */
#define CSR_BT_UNKNOWN_ERROR                           0x00
#define CSR_BT_CARD_RESET                              0x01
#define CSR_BT_CARD_NOT_ACCESSIBLE                     0x02
#define CSR_BT_CARD_REMOVED                            0x03
#define CSR_BT_CARD_INSERTED                           0x04
#define CSR_BT_CARD_RECOVERED                          0x05

/* Defines for disconnectType */
#define CSR_BT_GRACEFUL_DISCONNECT                     0x00
#define CSR_BT_IMMEDIATE_DISCONNECT                    0x01

/* Defines for transport */
#define CSR_BT_TRANSPORT_T_0                           0x00
#define CSR_BT_TRANSPORT_T_1                           0x01

#ifdef __cplusplus
}
#endif

#endif
