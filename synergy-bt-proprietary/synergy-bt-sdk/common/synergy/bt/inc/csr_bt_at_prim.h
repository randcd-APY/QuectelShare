#ifndef CSR_BT_AT_PRIM_H__
#define CSR_BT_AT_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_bt_result.h"
#include "csr_bt_profiles.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtAtPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim             CsrBtAtPrim;

/* ---------- Defines the SPP profile CsrBtResultCode ----------*/
#define CSR_BT_RESULT_CODE_AT_SUCCESS                              ((CsrBtResultCode) (0x0000))



typedef CsrUint8                  CsrBtAtResultCode;
#define CSR_BT_NUMBER_OF_RESULTCODES    (9)

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_AT_PRIM_DOWNSTREAM_LOWEST                          (0x0000)

#define CSR_BT_AT_DG_ACTIVATE_REQ                    ((CsrBtAtPrim) (0x0000 + CSR_BT_AT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AT_DG_DEACTIVATE_REQ                  ((CsrBtAtPrim) (0x0001 + CSR_BT_AT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AT_DG_DISCONNECT_REQ                  ((CsrBtAtPrim) (0x0002 + CSR_BT_AT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AT_DG_DATA_REQ                        ((CsrBtAtPrim) (0x0003 + CSR_BT_AT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_RESULT_CODE_RES             ((CsrBtAtPrim) (0x0004 + CSR_BT_AT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AT_DG_DATA_RES                        ((CsrBtAtPrim) (0x0005 + CSR_BT_AT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_S0Q_RES                     ((CsrBtAtPrim) (0x0006 + CSR_BT_AT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_S3Q_RES                     ((CsrBtAtPrim) (0x0007 + CSR_BT_AT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_S4Q_RES                     ((CsrBtAtPrim) (0x0008 + CSR_BT_AT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_S5Q_RES                     ((CsrBtAtPrim) (0x0009 + CSR_BT_AT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_S6Q_RES                     ((CsrBtAtPrim) (0x000A + CSR_BT_AT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_S7Q_RES                     ((CsrBtAtPrim) (0x000B + CSR_BT_AT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_S8Q_RES                     ((CsrBtAtPrim) (0x000C + CSR_BT_AT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_S10Q_RES                    ((CsrBtAtPrim) (0x000D + CSR_BT_AT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_GMI_RES                     ((CsrBtAtPrim) (0x000E + CSR_BT_AT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_GMM_RES                     ((CsrBtAtPrim) (0x000F + CSR_BT_AT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_GMR_RES                     ((CsrBtAtPrim) (0x0010 + CSR_BT_AT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_GCAP_RES                    ((CsrBtAtPrim) (0x0011 + CSR_BT_AT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_CONNECT_TEXT_RES            ((CsrBtAtPrim) (0x0012 + CSR_BT_AT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_DEFAULT_SETTING_RES         ((CsrBtAtPrim) (0x0013 + CSR_BT_AT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_FACTORY_SETTING_RES         ((CsrBtAtPrim) (0x0014 + CSR_BT_AT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AT_DG_CONTROL_REQ                     ((CsrBtAtPrim) (0x0015 + CSR_BT_AT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_UNKNOWN_EXTENDED_CMD_RES    ((CsrBtAtPrim) (0x0016 + CSR_BT_AT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AT_DG_PORTNEG_RES                     ((CsrBtAtPrim) (0x0017 + CSR_BT_AT_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_AT_PRIM_DOWNSTREAM_HIGHEST                         (0x0017 + CSR_BT_AT_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_BT_AT_PRIM_UPSTREAM_LOWEST                            (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_AT_DG_ATC_AND_C_IND                   ((CsrBtAtPrim) (0x0000 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_AND_F_IND                   ((CsrBtAtPrim) (0x0001 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_AND_D_IND                   ((CsrBtAtPrim) (0x0002 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_L_IND                       ((CsrBtAtPrim) (0x0003 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_M_IND                       ((CsrBtAtPrim) (0x0004 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_O_IND                       ((CsrBtAtPrim) (0x0005 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_X_IND                       ((CsrBtAtPrim) (0x0006 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_Z_IND                       ((CsrBtAtPrim) (0x0007 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_E_IND                       ((CsrBtAtPrim) (0x0008 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_Q_IND                       ((CsrBtAtPrim) (0x0009 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_V_IND                       ((CsrBtAtPrim) (0x000A + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_D_IND                       ((CsrBtAtPrim) (0x000B + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_P_IND                       ((CsrBtAtPrim) (0x000C + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_T_IND                       ((CsrBtAtPrim) (0x000D + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_A_IND                       ((CsrBtAtPrim) (0x000E + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_H_IND                       ((CsrBtAtPrim) (0x000F + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_S0Q_IND                     ((CsrBtAtPrim) (0x0010 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_S3Q_IND                     ((CsrBtAtPrim) (0x0011 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_S4Q_IND                     ((CsrBtAtPrim) (0x0012 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_S5Q_IND                     ((CsrBtAtPrim) (0x0013 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_S6Q_IND                     ((CsrBtAtPrim) (0x0014 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_S7Q_IND                     ((CsrBtAtPrim) (0x0015 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_S8Q_IND                     ((CsrBtAtPrim) (0x0016 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_S10Q_IND                    ((CsrBtAtPrim) (0x0017 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_S0_IND                      ((CsrBtAtPrim) (0x0018 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_S3_IND                      ((CsrBtAtPrim) (0x0019 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_S4_IND                      ((CsrBtAtPrim) (0x001A + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_S5_IND                      ((CsrBtAtPrim) (0x001B + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_S6_IND                      ((CsrBtAtPrim) (0x001C + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_S7_IND                      ((CsrBtAtPrim) (0x001D + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_S8_IND                      ((CsrBtAtPrim) (0x001E + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_S10_IND                     ((CsrBtAtPrim) (0x001F + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_GMI_IND                     ((CsrBtAtPrim) (0x0020 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_GMM_IND                     ((CsrBtAtPrim) (0x0021 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_GMR_IND                     ((CsrBtAtPrim) (0x0022 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_GCAP_IND                    ((CsrBtAtPrim) (0x0023 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_DISCONNECT_IND                  ((CsrBtAtPrim) (0x0024 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_CONNECT_IND                     ((CsrBtAtPrim) (0x0025 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_DATA_IND                        ((CsrBtAtPrim) (0x0026 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_DATA_CFM                        ((CsrBtAtPrim) (0x0027 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_DEACTIVATE_CFM                  ((CsrBtAtPrim) (0x0028 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_ABORT_IND                   ((CsrBtAtPrim) (0x0029 + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_CONTROL_IND                     ((CsrBtAtPrim) (0x002A + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_ATC_UNKNOWN_EXTENDED_CMD_IND    ((CsrBtAtPrim) (0x002B + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_PORTNEG_IND                     ((CsrBtAtPrim) (0x002C + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AT_DG_DATA_PATH_STATUS_IND            ((CsrBtAtPrim) (0x002D + CSR_BT_AT_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_AT_PRIM_UPSTREAM_HIGHEST                           (0x002D + CSR_BT_AT_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_AT_PRIM_DOWNSTREAM_COUNT              (CSR_BT_AT_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_AT_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_AT_PRIM_UPSTREAM_COUNT                (CSR_BT_AT_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_AT_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

/*============================================================================*
  typedefs
  *============================================================================*/
typedef struct
{
    CsrUint8        s3Value;
    CsrUint8        qValue;
    CsrUint8        vValue;
    CsrUint8        s4Value;
    CsrUint8        eValue;
    CsrUint8        s5Value;
    CsrUint8        andDValue;
#ifndef CSR_BT_AT_SEND_S0_TOKEN_IND
    CsrUint8        s0Value;
#endif
#ifndef CSR_BT_AT_SEND_S6_TOKEN_IND
    CsrUint8        s6Value;
#endif
#ifndef CSR_BT_AT_SEND_S7_TOKEN_IND
    CsrUint8        s7Value;
#endif
#ifndef CSR_BT_AT_SEND_S8_TOKEN_IND
    CsrUint8        s8Value;
#endif
#ifndef CSR_BT_AT_SEND_S10_TOKEN_IND
    CsrUint8        s10Value;
#endif
}CsrBtAtParameterValues;

typedef struct
{
    CsrBtAtPrim    type;
    CsrUint8        value;
} CsrBtAtDgAtcAndDInd;

typedef CsrBtAtDgAtcAndDInd CsrBtAtDgAtcAndFInd;
typedef CsrBtAtDgAtcAndDInd CsrBtAtDgAtcAndCInd;
typedef CsrBtAtDgAtcAndDInd CsrBtAtDgAtcLInd;
typedef CsrBtAtDgAtcAndDInd CsrBtAtDgAtcMInd;
typedef CsrBtAtDgAtcAndDInd CsrBtAtDgAtcOInd;
typedef CsrBtAtDgAtcAndDInd CsrBtAtDgAtcXInd;
typedef CsrBtAtDgAtcAndDInd CsrBtAtDgAtcZInd;
typedef CsrBtAtDgAtcAndDInd CsrBtAtDgAtcS0Ind;
typedef CsrBtAtDgAtcAndDInd CsrBtAtDgAtcS3Ind;
typedef CsrBtAtDgAtcAndDInd CsrBtAtDgAtcS4Ind;
typedef CsrBtAtDgAtcAndDInd CsrBtAtDgAtcS5Ind;
typedef CsrBtAtDgAtcAndDInd CsrBtAtDgAtcS6Ind;
typedef CsrBtAtDgAtcAndDInd CsrBtAtDgAtcS7Ind;
typedef CsrBtAtDgAtcAndDInd CsrBtAtDgAtcS8Ind;
typedef CsrBtAtDgAtcAndDInd CsrBtAtDgAtcS10Ind;
typedef CsrBtAtDgAtcAndDInd CsrBtAtDgAtcEInd;
typedef CsrBtAtDgAtcAndDInd CsrBtAtDgAtcQInd;
typedef CsrBtAtDgAtcAndDInd CsrBtAtDgAtcVInd;
typedef CsrBtAtDgAtcAndDInd CsrBtAtDgAtcS0qRes;
typedef CsrBtAtDgAtcAndDInd CsrBtAtDgAtcS3qRes;
typedef CsrBtAtDgAtcAndDInd CsrBtAtDgAtcS4qRes;
typedef CsrBtAtDgAtcAndDInd CsrBtAtDgAtcS5qRes;
typedef CsrBtAtDgAtcAndDInd CsrBtAtDgAtcS6qRes;
typedef CsrBtAtDgAtcAndDInd CsrBtAtDgAtcS7qRes;
typedef CsrBtAtDgAtcAndDInd CsrBtAtDgAtcS8qRes;
typedef CsrBtAtDgAtcAndDInd CsrBtAtDgAtcS10qRes;

typedef struct
{
    CsrBtAtPrim    type;
} CsrBtAtDgAtcPInd;

typedef CsrBtAtDgAtcPInd CsrBtAtDgAtcHInd;
typedef CsrBtAtDgAtcPInd CsrBtAtDgAtcTInd;
typedef CsrBtAtDgAtcPInd CsrBtAtDgAtcAInd;
typedef CsrBtAtDgAtcPInd CsrBtAtDgAtcS0qInd;
typedef CsrBtAtDgAtcPInd CsrBtAtDgAtcS3qInd;
typedef CsrBtAtDgAtcPInd CsrBtAtDgAtcS4qInd;
typedef CsrBtAtDgAtcPInd CsrBtAtDgAtcS5qInd;
typedef CsrBtAtDgAtcPInd CsrBtAtDgAtcS6qInd;
typedef CsrBtAtDgAtcPInd CsrBtAtDgAtcS7qInd;
typedef CsrBtAtDgAtcPInd CsrBtAtDgAtcS8qInd;
typedef CsrBtAtDgAtcPInd CsrBtAtDgAtcS10qInd;
typedef CsrBtAtDgAtcPInd CsrBtAtDgAtcGmiInd;
typedef CsrBtAtDgAtcPInd CsrBtAtDgAtcGmmInd;
typedef CsrBtAtDgAtcPInd CsrBtAtDgAtcGmrInd;
typedef CsrBtAtDgAtcPInd CsrBtAtDgAtcGcapInd;
typedef CsrBtAtDgAtcPInd CsrBtAtDgAtcAbortInd;

typedef struct
{
    CsrBtAtPrim              type;
    CsrUint16               payloadLength;
    CsrUint8                *payload;
}CsrBtAtDgAtcGmmRes;

typedef CsrBtAtDgAtcGmmRes CsrBtAtDgAtcGcapRes;
typedef CsrBtAtDgAtcGmmRes CsrBtAtDgAtcGmrRes;
typedef CsrBtAtDgAtcGmmRes CsrBtAtDgAtcGmiRes;
typedef CsrBtAtDgAtcGmmRes CsrBtAtDgAtcUnknownExtendedCmdInd;
typedef CsrBtAtDgAtcGmmRes CsrBtAtDgAtcUnknownExtendedCmdRes;


typedef struct
{
    CsrBtAtPrim              type;
    CsrUint16               payloadLength;
    CsrUint8                *payload;
    CsrUint8                atResult;
}CsrBtAtDgAtcConnectTextRes;

typedef struct
{
    CsrBtAtPrim              type;
    CsrBtAtParameterValues      parameterSetting;
}CsrBtAtDgAtcDefaultSettingRes;

typedef CsrBtAtDgAtcDefaultSettingRes CsrBtAtDgAtcFactorySettingRes;


typedef struct
{
    CsrBtAtPrim              type;
    CsrUint16               payloadLength;
    CsrUint8                *payload;
    CsrUint8                value;
}CsrBtAtDgAtcDInd;

typedef struct
{
    CsrBtAtPrim             type;
    CsrBtAtResultCode        atResult;
} CsrBtAtDgAtcResultCodeRes;

typedef struct
{
    CsrBtAtPrim             type;
    CsrSchedQid             phandle;
    CsrBtCplTimer            timeout;
} CsrBtAtDgActivateReq;


typedef struct
{
    CsrBtAtPrim             type;
} CsrBtAtDgDeactivateReq;

typedef struct
{
    CsrBtAtPrim             type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtAtDgDeactivateCfm;

typedef struct
{
    CsrBtAtPrim             type;
    CsrBtConnId             btConnId;
} CsrBtAtDgDisconnectReq;

typedef struct
{
    CsrBtAtPrim             type;
    CsrBtConnId             btConnId;
    CsrBool                 localTerminated;
    CsrBtReasonCode         reasonCode;
    CsrBtSupplier           reasonSupplier;
} CsrBtAtDgDisconnectInd;

typedef struct
{
    CsrBtAtPrim             type;
    CsrBtConnId             btConnId;
    CsrUint8                serverChannel;
    CsrUint16               profileMaxFrameSize;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtAtDgConnectInd;

typedef struct
{
    CsrBtAtPrim             type;
    CsrBtConnId             btConnId;
    CsrUint16               payloadLength;
    CsrUint8                *payload;
} CsrBtAtDgDataInd;

typedef struct
{
    CsrBtAtPrim            type;
    CsrBtConnId            btConnId;
    CsrUint16              payloadLength;
    CsrUint8               *payload;
} CsrBtAtDgDataReq;

typedef struct
{
    CsrBtAtPrim             type;
    CsrBtConnId             btConnId;
} CsrBtAtDgDataCfm;

typedef struct
{
    CsrBtAtPrim             type;
    CsrBtConnId             btConnId;
} CsrBtAtDgDataRes;

typedef struct
{
    CsrBtAtPrim             type;
    CsrBtConnId             btConnId;
    CsrUint8                modemstatus;
    CsrUint8                break_signal;        /* break signal */

} CsrBtAtDgControlInd;

typedef struct
{
    CsrBtAtPrim             type;
    CsrBtConnId             btConnId;
    CsrUint8                modemstatus;
    CsrUint8                break_signal;        /* break signal */

} CsrBtAtDgControlReq;

typedef struct
{
    CsrBtAtPrim             type;
    CsrBtConnId             btConnId;
    RFC_PORTNEG_VALUES_T    portPar;
    CsrBool                 request;
} CsrBtAtDgPortnegInd;

typedef struct
{
    RFC_PRIM_T              type;             /* Always RFC_PORTNEG_CFM */
    CsrBtConnId             btConnId;
    RFC_PORTNEG_VALUES_T    portPar;
} CsrBtAtDgPortnegRes;

typedef struct
{
    CsrBtAtPrim             type;
    CsrSchedQid                  dgInstanceQueue;
    CsrUint8                status;
} CsrBtAtDgDataPathStatusInd;

#ifdef __cplusplus
}
#endif

#endif
