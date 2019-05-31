#ifndef CSR_BT_DG_PRIM_H__
#define CSR_BT_DG_PRIM_H__

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
#include "rfcomm_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtDgPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

/* ---------- Defines the AV CsrBtResultCode ----------*/
#define CSR_BT_RESULT_CODE_DG_SUCCESS                ((CsrBtResultCode) (0x0000))
#define CSR_BT_RESULT_CODE_DG_UNACCEPTABLE_PARAMETER ((CsrBtResultCode) (0x0001))
#define CSR_BT_RESULT_CODE_DG_UNSPECIFIED_ERROR      ((CsrBtResultCode) (0x0002))

typedef CsrPrim    CsrBtDgPrim;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_DG_PRIM_DOWNSTREAM_LOWEST                     (0x0000)

#define CSR_BT_DG_ACTIVATE_REQ                   ((CsrBtDgPrim) (0x0000 + CSR_BT_DG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_DG_DEACTIVATE_REQ                 ((CsrBtDgPrim) (0x0001 + CSR_BT_DG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_DG_DISCONNECT_REQ                 ((CsrBtDgPrim) (0x0002 + CSR_BT_DG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_DG_DATA_REQ                       ((CsrBtDgPrim) (0x0003 + CSR_BT_DG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_DG_DATA_RES                       ((CsrBtDgPrim) (0x0004 + CSR_BT_DG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_DG_CONTROL_REQ                    ((CsrBtDgPrim) (0x0005 + CSR_BT_DG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_DG_PORTNEG_RES                    ((CsrBtDgPrim) (0x0006 + CSR_BT_DG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_DG_PORTNEG_REQ                    ((CsrBtDgPrim) (0x0007 + CSR_BT_DG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_DG_REGISTER_DATA_PATH_HANDLE_REQ  ((CsrBtDgPrim) (0x0008 + CSR_BT_DG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_DG_DATA_PATH_STATUS_REQ           ((CsrBtDgPrim) (0x0009 + CSR_BT_DG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_DG_SECURITY_IN_REQ                ((CsrBtDgPrim) (0x000A + CSR_BT_DG_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_DG_PRIM_DOWNSTREAM_HIGHEST                    (0x000A + CSR_BT_DG_PRIM_DOWNSTREAM_LOWEST)

/* Not part of interface, used internally only */
#define CSR_BT_DG_HOUSE_CLEANING                 ((CsrBtDgPrim) (CSR_BT_DG_PRIM_DOWNSTREAM_HIGHEST + 1))

/*******************************************************************************/

#define CSR_BT_DG_PRIM_UPSTREAM_LOWEST                       (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_DG_DEACTIVATE_CFM                 ((CsrBtDgPrim) (0x0000 + CSR_BT_DG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_DG_DISCONNECT_IND                 ((CsrBtDgPrim) (0x0001 + CSR_BT_DG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_DG_DATA_CFM                       ((CsrBtDgPrim) (0x0002 + CSR_BT_DG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_DG_DATA_IND                       ((CsrBtDgPrim) (0x0003 + CSR_BT_DG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_DG_CONNECT_IND                    ((CsrBtDgPrim) (0x0004 + CSR_BT_DG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_DG_CONTROL_IND                    ((CsrBtDgPrim) (0x0005 + CSR_BT_DG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_DG_PORTNEG_IND                    ((CsrBtDgPrim) (0x0006 + CSR_BT_DG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_DG_PORTNEG_CFM                    ((CsrBtDgPrim) (0x0007 + CSR_BT_DG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_DG_STATUS_IND                     ((CsrBtDgPrim) (0x0008 + CSR_BT_DG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_DG_REGISTER_DATA_PATH_HANDLE_CFM  ((CsrBtDgPrim) (0x0009 + CSR_BT_DG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_DG_DATA_PATH_STATUS_IND           ((CsrBtDgPrim) (0x000A + CSR_BT_DG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_DG_SECURITY_IN_CFM                ((CsrBtDgPrim) (0x000B + CSR_BT_DG_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_DG_PRIM_UPSTREAM_HIGHEST                      (0x000B + CSR_BT_DG_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_DG_PRIM_DOWNSTREAM_COUNT          (CSR_BT_DG_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_DG_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_DG_PRIM_UPSTREAM_COUNT            (CSR_BT_DG_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_DG_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

typedef struct
{
    CsrBtDgPrim         type;                              /* primitive/message identity */
    CsrBtDeviceAddr     deviceAddr;                        /* BT address of device to bonded with. */
    CsrBtConnId         btConnId;                          /* Bluetooth Connection ID */
    CsrUint8            serverChannel;                     /* Local server channel */
    CsrUint16           profileMaxFrameSize;               /* Maximum frame size (bytes) */
    CsrBtResultCode     resultCode;                        /* indicate the status of the connection.*/
    CsrBtSupplier       resultSupplier;
} CsrBtDgConnectInd;

typedef struct
{
    CsrBtDgPrim         type;                               /* primitive/message identity */
    CsrSchedQid              phandle;                            /* protocol handle */
    CsrBtCplTimer       timeout;                            /* Holds the pageScanTimeOut time */
    CsrUint8            role;                               /* is DG DTC or CSR_BT_DCE (ignored in phase I - use defines) */
} CsrBtDgActivateReq;

typedef struct
{
    CsrBtDgPrim          type;                               /* primitive identity */
     CsrBtConnId         btConnId;                           /* Bluetooth Connection ID */
    CsrBtDeviceAddr      deviceAddr;                         /* BT address of device to bonded with. */
    CsrBool              connect;                            /* true on new connection, false if connection is dead */
    CsrUint16            maxMsgSize;                         /* maximum message size for the profile */
} CsrBtDgStatusInd;

typedef struct
{
    CsrBtDgPrim         type;                               /* primitive/message identity */
} CsrBtDgDeactivateReq;

typedef struct
{
    CsrBtDgPrim         type;                               /* primitive/message identity */
    CsrBtResultCode     resultCode;                         /* indicate the status of the connection.*/
    CsrBtSupplier       resultSupplier;
} CsrBtDgDeactivateCfm;

/* to optimise for speed this primitive must be identical to CSR_BT_CM_DATA_REQ */
typedef struct
{
    CsrBtDgPrim         type;                               /* primitive/message identity */
    CsrBtConnId         btConnId;                           /* Bluetooth Connection ID */
    CsrUint16           payloadLength;                      /* Length of the payload */
    CsrUint8            *payload;                           /* Pointer to the data */
} CsrBtDgDataReq;

/* to optimise for speed this primitive must be identical to CSR_BT_CM_DATA_CFM*/
typedef struct
{
    CsrBtDgPrim         type;                               /* primitive/message identity */
    CsrBtConnId         btConnId;                           /* Bluetooth Connection ID */
} CsrBtDgDataCfm;

/* to optimise for speed this primitive must be identical to CSR_BT_CM_DATA_IND */
typedef struct
{
    CsrBtDgPrim         type;                               /* primitive/message identity */
    CsrBtConnId         btConnId;                           /* Bluetooth Connection ID */
    CsrUint16           payloadLength;                      /* Length of the payload */
    CsrUint8            *payload;                           /* Pointer to the data */
} CsrBtDgDataInd;

/* to optimise for speed this primitive must be identical to CSR_BT_CM_DATA_RES */
typedef struct
{
    CsrBtDgPrim         type;                               /* primitive/message identity */
    CsrBtConnId         btConnId;                           /* Bluetooth Connection ID */
} CsrBtDgDataRes;

typedef struct
{
    CsrBtDgPrim         type;                               /* primitive/message identity */
    CsrBtConnId         btConnId;                           /* Bluetooth Connection ID */
    CsrBtDeviceAddr     deviceAddr;
    CsrBool             localTerminated;
    CsrBtReasonCode     reasonCode;                         /* indicate the status of the connection.*/
    CsrBtSupplier       reasonSupplier;
} CsrBtDgDisconnectInd;

typedef struct
{
    CsrBtDgPrim         type;                               /* primitive/message identity */
    CsrBtConnId         btConnId;                           /* Bluetooth Connection ID */
} CsrBtDgDisconnectReq;

typedef struct
{
    CsrBtDgPrim         type;                               /* primitive/message identity */
    CsrBtConnId         btConnId;                           /* Bluetooth Connection ID */
    CsrUint8            modemstatus;                        /* modemstatus */
    CsrUint8            break_signal;                       /* break signal */
} CsrBtDgControlReq;

typedef struct
{
    CsrBtDgPrim         type;                               /* primitive type */
    CsrBtConnId         btConnId;                           /* Bluetooth Connection ID */
    CsrUint8            modemstatus;                        /* modemstatus */
    CsrUint8            break_signal;                       /* break signal */
} CsrBtDgControlInd;

typedef struct
{
    CsrBtDgPrim             type;                           /* primitive identity */
    CsrBtConnId             btConnId;                       /* Bluetooth Connection ID */
    RFC_PORTNEG_VALUES_T    portPar;                        /* serial port parameters */
} CsrBtDgPortnegReq;

typedef struct
{
    CsrBtDgPrim             type;                           /* primitive identity */
    CsrBtConnId             btConnId;                       /* Bluetooth Connection ID */
    RFC_PORTNEG_VALUES_T    portPar;                        /* serial port parameters */
} CsrBtDgPortnegCfm;

typedef struct
{
    CsrBtDgPrim             type;                           /* primitive type */
    CsrBtConnId             btConnId;                       /* Bluetooth Connection ID */
    RFC_PORTNEG_VALUES_T    portPar;                        /* port parameters */
    CsrBool                 request;                        /* is portPar valid */
} CsrBtDgPortnegInd;

typedef struct
{
    CsrBtDgPrim             type;                           /* Always RFC_PORTNEG_CFM */
    CsrBtConnId             btConnId;                       /* Bluetooth Connection ID */
    RFC_PORTNEG_VALUES_T    portPar;                        /* port parameters */
} CsrBtDgPortnegRes;

typedef struct
{
    CsrBtDgPrim         type;                               /* primitive type */
} CsrBtDgHouseCleaning;


typedef struct
{
    CsrBtDgPrim         type;                               /* primitive/message identity */
    CsrSchedQid              dataAppHandle;                      /* register this handle for the data application */
} CsrBtDgRegisterDataPathHandleReq;

typedef struct
{
    CsrBtDgPrim         type;                               /* primitive/message identity */
    CsrBtResultCode     resultCode;                         /* indicate the status of the connection.*/
    CsrBtSupplier       resultSupplier;
} CsrBtDgRegisterDataPathHandleCfm;

typedef struct
{
    CsrBtDgPrim         type;                               /* primitive/message identity */
    CsrSchedQid              dgInstanceQueue;                    /* DG queue handle */
    CsrUint8            status;                             /* see csr_bt_profiles.h: DATA_PATH_STATUS_xxx */
} CsrBtDgDataPathStatusReq;

typedef struct
{
    CsrBtDgPrim         type;                               /* primitive/message identity */
    CsrSchedQid              dgInstanceQueue;                    /* DG queue handle */
    CsrUint8            status;                             /* see csr_bt_profiles.h: DATA_PATH_STATUS_xxx */
    CsrBtResultCode     resultCode;
    CsrBtSupplier       resultSupplier;
} CsrBtDgDataPathStatusInd;

typedef struct
{
    CsrBtDgPrim         type;
    CsrSchedQid              appHandle;
    CsrUint16           secLevel;
} CsrBtDgSecurityInReq;

typedef struct
{
    CsrBtDgPrim         type;
    CsrBtResultCode     resultCode;
    CsrBtSupplier       resultSupplier;
} CsrBtDgSecurityInCfm;

#ifdef __cplusplus
}
#endif

#endif
