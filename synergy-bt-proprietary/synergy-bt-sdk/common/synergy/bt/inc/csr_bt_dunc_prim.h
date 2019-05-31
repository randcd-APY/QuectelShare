#ifndef CSR_BT_DUNC_PRIM_H__
#define CSR_BT_DUNC_PRIM_H__

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

/* search_string="CsrBtDuncPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim        CsrBtDuncPrim;



/* ---------- Defines the DUNC CsrBtResultCode ----------*/
#define CSR_BT_RESULT_CODE_DUNC_SUCCESS                ((CsrBtResultCode) (0x0000))
#define CSR_BT_RESULT_CODE_DUNC_UNACCEPTABLE_PARAMETER ((CsrBtResultCode) (0x0001))
#define CSR_BT_RESULT_CODE_DUNC_UNSPECIFIED_ERROR      ((CsrBtResultCode) (0x0002))


/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_DUNC_PRIM_DOWNSTREAM_LOWEST                             (0x0000)

#define CSR_BT_DUNC_CONNECT_REQ                        ((CsrBtDuncPrim) (0x0000 + CSR_BT_DUNC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_DUNC_CANCEL_CONNECT_REQ                 ((CsrBtDuncPrim) (0x0001 + CSR_BT_DUNC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_DUNC_DATA_REQ                           ((CsrBtDuncPrim) (0x0002 + CSR_BT_DUNC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_DUNC_DATA_RES                           ((CsrBtDuncPrim) (0x0003 + CSR_BT_DUNC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_DUNC_CONTROL_REQ                        ((CsrBtDuncPrim) (0x0004 + CSR_BT_DUNC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_DUNC_PORTNEG_RES                        ((CsrBtDuncPrim) (0x0005 + CSR_BT_DUNC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_DUNC_PORTNEG_REQ                        ((CsrBtDuncPrim) (0x0006 + CSR_BT_DUNC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_DUNC_DISCONNECT_REQ                     ((CsrBtDuncPrim) (0x0007 + CSR_BT_DUNC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_DUNC_REGISTER_DATA_PATH_HANDLE_REQ      ((CsrBtDuncPrim) (0x0008 + CSR_BT_DUNC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_DUNC_CHANGE_DATA_PATH_STATUS_REQ        ((CsrBtDuncPrim) (0x0009 + CSR_BT_DUNC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_DUNC_SECURITY_OUT_REQ                   ((CsrBtDuncPrim) (0x000A + CSR_BT_DUNC_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_DUNC_PRIM_DOWNSTREAM_HIGHEST                            (0x000A + CSR_BT_DUNC_PRIM_DOWNSTREAM_LOWEST)

#define CSR_BT_DUNC_HOUSE_CLEANING                     ((CsrBtDuncPrim)  0x0100)

/*******************************************************************************/

#define CSR_BT_DUNC_PRIM_UPSTREAM_LOWEST                               (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_DUNC_CONNECT_CFM                        ((CsrBtDuncPrim) (0x0000 + CSR_BT_DUNC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_DUNC_DATA_CFM                           ((CsrBtDuncPrim) (0x0001 + CSR_BT_DUNC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_DUNC_DATA_IND                           ((CsrBtDuncPrim) (0x0002 + CSR_BT_DUNC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_DUNC_CONTROL_IND                        ((CsrBtDuncPrim) (0x0003 + CSR_BT_DUNC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_DUNC_PORTNEG_IND                        ((CsrBtDuncPrim) (0x0004 + CSR_BT_DUNC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_DUNC_PORTNEG_CFM                        ((CsrBtDuncPrim) (0x0005 + CSR_BT_DUNC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_DUNC_DISCONNECT_IND                     ((CsrBtDuncPrim) (0x0006 + CSR_BT_DUNC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_DUNC_STATUS_IND                         ((CsrBtDuncPrim) (0x0007 + CSR_BT_DUNC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_DUNC_REGISTER_DATA_PATH_HANDLE_CFM      ((CsrBtDuncPrim) (0x0008 + CSR_BT_DUNC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_DUNC_DATA_PATH_STATUS_IND               ((CsrBtDuncPrim) (0x0009 + CSR_BT_DUNC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_DUNC_SECURITY_OUT_CFM                   ((CsrBtDuncPrim) (0x000A + CSR_BT_DUNC_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_DUNC_PRIM_UPSTREAM_HIGHEST                              (0x000A + CSR_BT_DUNC_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_DUNC_PRIM_DOWNSTREAM_COUNT              (CSR_BT_DUNC_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_DUNC_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_DUNC_PRIM_UPSTREAM_COUNT                (CSR_BT_DUNC_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_DUNC_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

/* Downstream messages */
typedef struct
{
    CsrBtDuncPrim            type;
} CsrBtDuncHouseCleaning;

typedef struct
{
    CsrBtDuncPrim            type;                   /* primitive identifier */
    CsrSchedQid               ctrlHandle;             /* the app handle of the Bluetooth controlling application */
    CsrBtDeviceAddr            bdAddr;                 /* the address of the device to connect to */
    CsrBool                  lowPowerSupport;        /* TRUE if low power mode should be supported (SNIFF), FALSE if only ACTIVE mode should */
} CsrBtDuncConnectReq;

typedef struct
{
    CsrBtDuncPrim            type;                   /* primitive identifier */
} CsrBtDuncCancelConnectReq;

typedef struct
{
    CsrBtDuncPrim            type;                   /* primitive identifier */
    CsrUint16                dataLength;             /* the length of the data field */
    CsrUint8                 *data;                  /* the data to be send - must be less or equal to macMsgSize in CSR_BT_DUNC_STATUS_IND */
} CsrBtDuncDataReq;

typedef struct
{
    CsrBtDuncPrim            type;                   /* primitive identifier */
} CsrBtDuncDataRes;

typedef struct
{
    CsrBtDuncPrim            type;                   /* primitive identifier */
    CsrUint8                 modemStatus;            /* Holds the Modem status flag(DCD DSR CTS RI)*/
    CsrUint8                 breakSignal;
} CsrBtDuncControlReq;

typedef struct
{
    CsrBtDuncPrim            type;                   /* primitive identifier */
    RFC_PORTNEG_VALUES_T     portPar;                /* port negotiation parameter */
} CsrBtDuncPortnegRes;

typedef struct
{
    CsrBtDuncPrim            type;                   /* primitive identifier */
    RFC_PORTNEG_VALUES_T     portPar;                /* port negotiation parameter */
    CsrBool                  request;
} CsrBtDuncPortnegReq;

typedef struct
{
    CsrBtDuncPrim            type;                   /* primitive identifier */
} CsrBtDuncDisconnectReq;

typedef struct
{
    CsrBtDuncPrim            type;                   /* primitive identifier */
    CsrSchedQid               dataAppHandle;          /* the app handle of the application to handle the data */
} CsrBtDuncRegisterDataPathHandleReq;

typedef struct
{
    CsrBtDuncPrim            type;                   /* primitive identifier */
    CsrSchedQid               duncInstanceId;         /* the handle of the DUNC profile instance, to send the message to */
    CsrUint8                 status;                 /* the status of the data path */
} CsrBtDuncChangeDataPathStatusReq;

typedef struct
{
    CsrBtDuncPrim            type;
    CsrSchedQid               appHandle;
    CsrUint16                secLevel;
} CsrBtDuncSecurityOutReq;

/* Upstream messages */
typedef struct
{
    CsrBtDuncPrim            type;                   /* primitive identifier */
    CsrSchedQid                   duncInstanceId;         /* the handle of the DUNC profile instance, that send the message */
    CsrUint16                maxMsgSize;             /* the maximum message size to send to the application in CSR_BT_DUNC_DATA_REQ */
    CsrBtResultCode          resultCode;             /* the result of the connection establishment */
    CsrBtSupplier            resultSupplier;
    CsrBtConnId              btConnId;               /* Global Bluetooth connection ID */
} CsrBtDuncConnectCfm;

typedef struct
{
    CsrBtDuncPrim            type;                   /* primitive identifier */
    CsrSchedQid               duncInstanceId;         /* the handle of the DUNC profile instance, that send the message */
} CsrBtDuncDataCfm;

typedef struct
{
    CsrBtDuncPrim            type;                   /* primitive identifier */
    CsrSchedQid               duncInstanceId;         /* the handle of the DUNC profile instance, that send the message */
    CsrUint16                dataLength;             /* the length of the data field */
    CsrUint8                 *data;                  /* the received data */
} CsrBtDuncDataInd;

typedef struct
{
    CsrBtDuncPrim            type;                   /* primitive identifier */
    CsrSchedQid               duncInstanceId;         /* the handle of the DUNC profile instance, that send the message */
    CsrUint8                 modemStatus;            /* Holds the Modem status flag(DCD DSR CTS RI)*/
    CsrUint8                 breakSignal;
} CsrBtDuncControlInd;

typedef struct
{
    CsrBtDuncPrim            type;                   /* primitive identifier */
    CsrSchedQid                   duncInstanceId;         /* the handle of the DUNC profile instance, that send the message */
    RFC_PORTNEG_VALUES_T     portPar;                /* port negotiation parameter */
    CsrBool                  request;
} CsrBtDuncPortnegInd;

typedef struct
{
    CsrBtDuncPrim           type;                   /* primitive identifier */
    CsrSchedQid                  duncInstanceId;         /* the handle of the DUNC profile instance, that send the message */
    RFC_PORTNEG_VALUES_T    portPar;                /* port negotiation parameter */
} CsrBtDuncPortnegCfm;

typedef struct
{
    CsrBtDuncPrim           type;               /* primitive identifier */
    CsrSchedQid               duncInstanceId;           /* the handle of the DUNC profile instance, that send the message */
    CsrBool                  localTerminated;    /* Connection terminated due to local request? */
    CsrBtReasonCode         reasonCode;
    CsrBtSupplier     reasonSupplier;
} CsrBtDuncDisconnectInd;

typedef struct
{
    CsrBtDuncPrim            type;                   /* primitive identifier */
    CsrSchedQid               duncInstanceId;         /* the handle of the DUNC profile instance, that send the message */
    CsrBtDeviceAddr            deviceAddr;             /* address of remote bluetooth device */
    CsrBool                  connected;              /* indicates if status is connected (TRUE) or disconnected (FALSE) */
    CsrUint16                maxMsgSize;             /* the maximum message size that must be send in a CSR_BT_DUNC_DATA_REQ message */
} CsrBtDuncStatusInd;

typedef struct
{
    CsrBtDuncPrim            type;                   /* primitive identifier */
    CsrSchedQid               duncInstanceId;         /* the handle of the DUNC profile instance, that send the message */
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtDuncRegisterDataPathHandleCfm;

typedef struct
{
    CsrBtDuncPrim            type;                   /* primitive identifier */
    CsrSchedQid               duncInstanceId;         /* the handle of the DUNC profile instance, that send the message */
    CsrUint8                 status;                 /* the status of the data path */
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtDuncDataPathStatusInd;

typedef struct
{
    CsrBtDuncPrim            type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtDuncSecurityOutCfm;


#ifdef __cplusplus
}
#endif

#endif
