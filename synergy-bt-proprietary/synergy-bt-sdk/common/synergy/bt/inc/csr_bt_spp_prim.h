#ifndef CSR_BT_SPP_PRIM_H__
#define CSR_BT_SPP_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
****************************************************************************/

#include "csr_types.h"
#include "csr_bt_result.h"
#include "csr_bt_profiles.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtSppPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

/* ---------- Defines the SPP profile CsrBtResultCode ----------*/
#define CSR_BT_RESULT_CODE_SPP_SUCCESS                              ((CsrBtResultCode) (0x0000))
#define CSR_BT_RESULT_CODE_SPP_UNACCEPTABLE_PARAMETER               ((CsrBtResultCode) (0x0001))
#define CSR_BT_RESULT_CODE_SPP_COMMAND_DISALLOWED                   ((CsrBtResultCode) (0x0002))
#define CSR_BT_RESULT_CODE_SPP_MAX_NUM_OF_CONNECTIONS               ((CsrBtResultCode) (0x0003))
#define CSR_BT_RESULT_CODE_SPP_ALREADY_CONNECTING                   ((CsrBtResultCode) (0x0004))
#define CSR_BT_RESULT_CODE_SPP_SEARCH_FAILED                        ((CsrBtResultCode) (0x0005))
#define CSR_BT_RESULT_CODE_SPP_NOTHING_TO_CANCEL                    ((CsrBtResultCode) (0x0006))
#define CSR_BT_RESULT_CODE_SPP_CANCELLED_CONNECT_ATTEMPT            ((CsrBtResultCode) (0x0007))
#define CSR_BT_RESULT_CODE_SPP_TIMEOUT                              ((CsrBtResultCode) (0x0008))

/* The SPP can take the role of both DTE and DCE. Use this define to
 * set the desired role of the SPP when it is either a client or a
 * server. The setting can not be changed during run time*/
#define CSR_BT_SPP_CLIENT_ROLE                    (CSR_BT_DTE)
#define CSR_BT_SPP_SERVER_ROLE                    (CSR_BT_DCE)

typedef CsrPrim CsrBtSppPrim;

typedef struct
{
    CsrBtDeviceName        serviceName;
    CsrBtUuid32              serviceHandle;
} CsrBtSppServiceName;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST                     (0x0000)

#define CSR_BT_SPP_CONNECT_REQ                   ((CsrBtSppPrim) (0x0000 + CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SPP_ACTIVATE_REQ                  ((CsrBtSppPrim) (0x0001 + CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SPP_DEACTIVATE_REQ                ((CsrBtSppPrim) (0x0002 + CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SPP_DISCONNECT_REQ                ((CsrBtSppPrim) (0x0003 + CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SPP_DATA_REQ                      ((CsrBtSppPrim) (0x0004 + CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SPP_DATA_RES                      ((CsrBtSppPrim) (0x0005 + CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SPP_CONTROL_REQ                   ((CsrBtSppPrim) (0x0006 + CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SPP_PORTNEG_RES                   ((CsrBtSppPrim) (0x0007 + CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SPP_PORTNEG_REQ                   ((CsrBtSppPrim) (0x0008 + CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SPP_SERVICE_NAME_RES              ((CsrBtSppPrim) (0x0009 + CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SPP_AUDIO_CONNECT_REQ             ((CsrBtSppPrim) (0x000A + CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SPP_ACCEPT_AUDIO_REQ              ((CsrBtSppPrim) (0x000B + CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SPP_CANCEL_ACCEPT_AUDIO_REQ       ((CsrBtSppPrim) (0x000C + CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SPP_AUDIO_DISCONNECT_REQ          ((CsrBtSppPrim) (0x000D + CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SPP_EXTENDED_CONNECT_REQ          ((CsrBtSppPrim) (0x000E + CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SPP_EXTENDED_UUID_CONNECT_REQ     ((CsrBtSppPrim) (0x000F + CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SPP_MODE_CHANGE_REQ               ((CsrBtSppPrim) (0x0010 + CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SPP_EXTENDED_ACTIVATE_REQ         ((CsrBtSppPrim) (0x0011 + CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SPP_REGISTER_DATA_PATH_HANDLE_REQ ((CsrBtSppPrim) (0x0012 + CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SPP_DATA_PATH_STATUS_REQ          ((CsrBtSppPrim) (0x0013 + CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SPP_GET_INSTANCES_QID_REQ         ((CsrBtSppPrim) (0x0014 + CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SPP_REGISTER_QID_REQ              ((CsrBtSppPrim) (0x0015 + CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SPP_AUDIO_ACCEPT_CONNECT_RES      ((CsrBtSppPrim) (0x0016 + CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SPP_AUDIO_RENEGOTIATE_REQ         ((CsrBtSppPrim) (0x0017 + CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SPP_CANCEL_CONNECT_REQ            ((CsrBtSppPrim) (0x0018 + CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SPP_SECURITY_IN_REQ               ((CsrBtSppPrim) (0x0019 + CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SPP_SECURITY_OUT_REQ              ((CsrBtSppPrim) (0x001A + CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_SPP_PRIM_DOWNSTREAM_HIGHEST                    (0x001A + CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST)

/* Not part of app-interface for internal use only*/
#define CSR_BT_SPP_HOUSE_CLEANING                ((CsrBtSppPrim) (0x007F + CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST))

/*******************************************************************************/

#define CSR_BT_SPP_PRIM_UPSTREAM_LOWEST                       (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_SPP_DEACTIVATE_CFM                ((CsrBtSppPrim) (0x0000 + CSR_BT_SPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SPP_DISCONNECT_IND                ((CsrBtSppPrim) (0x0001 + CSR_BT_SPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SPP_DATA_CFM                      ((CsrBtSppPrim) (0x0002 + CSR_BT_SPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SPP_DATA_IND                      ((CsrBtSppPrim) (0x0003 + CSR_BT_SPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SPP_CONNECT_IND                   ((CsrBtSppPrim) (0x0004 + CSR_BT_SPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SPP_CONTROL_IND                   ((CsrBtSppPrim) (0x0005 + CSR_BT_SPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SPP_PORTNEG_IND                   ((CsrBtSppPrim) (0x0006 + CSR_BT_SPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SPP_PORTNEG_CFM                   ((CsrBtSppPrim) (0x0007 + CSR_BT_SPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SPP_SERVICE_NAME_IND              ((CsrBtSppPrim) (0x0008 + CSR_BT_SPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SPP_AUDIO_CONNECT_IND             ((CsrBtSppPrim) (0x0009 + CSR_BT_SPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SPP_AUDIO_CONNECT_CFM             ((CsrBtSppPrim) (0x000A + CSR_BT_SPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SPP_AUDIO_DISCONNECT_IND          ((CsrBtSppPrim) (0x000B + CSR_BT_SPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SPP_AUDIO_DISCONNECT_CFM          ((CsrBtSppPrim) (0x000C + CSR_BT_SPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SPP_ACTIVATE_CFM                  ((CsrBtSppPrim) (0x000D + CSR_BT_SPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SPP_MODE_CHANGE_IND               ((CsrBtSppPrim) (0x000E + CSR_BT_SPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SPP_STATUS_IND                    ((CsrBtSppPrim) (0x000F + CSR_BT_SPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SPP_REGISTER_DATA_PATH_HANDLE_CFM ((CsrBtSppPrim) (0x0010 + CSR_BT_SPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SPP_DATA_PATH_STATUS_IND          ((CsrBtSppPrim) (0x0011 + CSR_BT_SPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SPP_GET_INSTANCES_QID_CFM         ((CsrBtSppPrim) (0x0012 + CSR_BT_SPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SPP_AUDIO_RENEGOTIATE_CFM         ((CsrBtSppPrim) (0x0013 + CSR_BT_SPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SPP_AUDIO_RENEGOTIATE_IND         ((CsrBtSppPrim) (0x0014 + CSR_BT_SPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SPP_AUDIO_ACCEPT_CONNECT_IND      ((CsrBtSppPrim) (0x0015 + CSR_BT_SPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SPP_SECURITY_IN_CFM               ((CsrBtSppPrim) (0x0016 + CSR_BT_SPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SPP_SECURITY_OUT_CFM              ((CsrBtSppPrim) (0x0017 + CSR_BT_SPP_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_SPP_PRIM_UPSTREAM_HIGHEST                      (0x0015 + CSR_BT_SPP_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_SPP_PRIM_DOWNSTREAM_COUNT         (CSR_BT_SPP_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_SPP_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_SPP_PRIM_UPSTREAM_COUNT           (CSR_BT_SPP_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_SPP_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

typedef struct
{
    CsrBtSppPrim            type;                                 /* primitive/message identity*/
    CsrSchedQid                  phandle;                              /* control (bluetooth) protocol handle */
    CsrBtDeviceAddr         deviceAddr;                           /* BT address of device to bonded with. */
    CsrUint8                role;                                 /* is SPP DTC or CSR_BT_DCE */
    CsrBool                 validPortPar;                         /* request CM to execute port neg */
    CsrBool                 requestPortPar;                       /* this is a request or information */
    RFC_PORTNEG_VALUES_T    portPar;                              /* port negotiation parameter (relevant only if requestPortPar is true) */
} CsrBtSppConnectReq;

typedef struct
{
    CsrBtSppPrim            type;                                 /* primitive/message identity */
    CsrBtDeviceAddr         deviceAddr;                           /* BT address of device to bonded with.*/
    CsrSchedQid                  queueId;                              /* instance index for multiple SPP connections */
    CsrUint8           serverChannel;                        /* Local server channel */
    CsrUint16               profileMaxFrameSize;                  /* Maximum frame size (bytes) */
    CsrBool                 validPortPar;                         /* TRUE if CM has executed port neg */
    RFC_PORTNEG_VALUES_T    portPar;                              /* port negotiation parameter (valid only if validPortPar is true) */
    CsrBtResultCode         resultCode;                           /* indicate the status of the connection.*/
    CsrBtSupplier           resultSupplier;
    CsrBtConnId             btConnId;                             /* Global Bluetooth connection ID */
} CsrBtSppConnectInd;

typedef struct
{
    CsrBtSppPrim            type;                                 /* primitive/message identity */
    CsrSchedQid                  phandle;                              /* protocol handle */
    CsrBtCplTimer           timeout;                              /* Holds the pageScanTimeOut time */
    CsrUint8                role;                                 /* is SPP DTC or CSR_BT_DCE */
    CsrCharString               *serviceName;                         /* name of the service we are activating (ignored in phase I)*/
} CsrBtSppActivateReq;

typedef struct
{
    CsrBtSppPrim            type;                                  /* primitive/message identity */
    CsrSchedQid                  queueId;                               /* spp instance index protocol handle */
    CsrUint8           serverChannel;                         /* local server channel */
} CsrBtSppActivateCfm;

typedef struct
{
    CsrBtSppPrim            type;                                  /* primitive identity */
    CsrBtDeviceAddr         deviceAddr;                            /* address of remote bluetooth device */
    CsrSchedQid                  queueId;                               /* spp instance index protocol handle */
    CsrUint8           serverChannel;                         /* local server channel */
    CsrBool                 connect;                               /* true on new connection, false if connection is dead */
    CsrUint16               maxMsgSize;                            /* maximum message size for the profile */
} CsrBtSppStatusInd;

typedef struct
{
    CsrBtSppPrim            type;                                  /* primitive/message identity */
    CsrSchedQid                  phandle;                               /* protocol handle */
} CsrBtSppDeactivateReq;

typedef struct
{
    CsrBtSppPrim            type;                                  /* primitive/message identity */
    CsrSchedQid                  phandle;                               /* protocol handle (also temporary use for multiple instances) */
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtSppDeactivateCfm;
/* to optimise for speed this primitive must be identical to CSR_BT_CM_DATA_REQ*/

typedef struct
{
    CsrBtSppPrim            type;                                  /* primitive/message identity */
    CsrUint8           serverChannel;                         /* local server channel number */
    CsrUint16              payloadLength;                         /* Length of the payload */
    CsrUint8              *payload;                              /* Pointer to the data */
} CsrBtSppDataReq;
/* to optimise for speed this primitive must be identical to CSR_BT_CM_DATA_CFM*/

typedef struct
{
    CsrBtSppPrim            type;                                  /* primitive/message identity */
    CsrSchedQid                  queueId;                               /* spp instance index protocol handle */
    CsrUint8           serverChannel;                         /* local server channel number */
} CsrBtSppDataCfm;
/* to optimise for speed this primitive must be identical to CSR_BT_CM_DATA_IND*/

typedef struct
{
    CsrBtSppPrim            type;                                  /* primitive/message identity */
    CsrSchedQid                  queueId;                               /* spp instance index protocol handle */
    CsrUint8           serverChannel;                         /* local server channel number */
    CsrUint16              payloadLength;                         /* Length of the payload */
    CsrUint8              *payload;                              /* Pointer to the data */
} CsrBtSppDataInd;
/* to optimise for speed this primitive must be identical to CSR_BT_CM_DATA_RES*/

typedef struct
{
    CsrBtSppPrim            type;                                  /* primitive/message identity */
    CsrUint8           serverChannel;                         /* local server channel number */
} CsrBtSppDataRes;

typedef struct
{
    CsrBtSppPrim            type;                                  /* primitive/message identity */
    CsrSchedQid                  queueId;                               /* spp instance index protocol handle */
    CsrUint8           serverChannel;                         /* Local server channel */
    CsrBtDeviceAddr         deviceAddr;                            /* BT address of device to bonded with.*/
    CsrBtReasonCode         reasonCode;                            /* indicate the status of the connection.*/
    CsrBtSupplier           reasonSupplier;
    CsrBool                 localTerminated;
} CsrBtSppDisconnectInd;

typedef struct
{
    CsrBtSppPrim            type;                                  /* primitive/message identity */
    CsrUint8           serverChannel;                         /* Local server channel */
} CsrBtSppDisconnectReq;

typedef struct
{
    CsrBtSppPrim            type;                                  /* primitive/message identity */
    CsrUint8           serverChannel;                         /* Local server channel*/
    CsrUint8                modemstatus;                           /* modemstatus*/
    CsrUint8                break_signal;                          /* break signal*/
} CsrBtSppControlReq;

typedef struct
{
    CsrBtSppPrim            type;                                  /* primitive/message identity */
    CsrSchedQid                  queueId;                               /* spp instance index protocol handle */
    CsrUint8           serverChannel;                         /* Local server channel */
    CsrUint8                modemstatus;                           /* modemstatus */
    CsrUint8                break_signal;                          /* break signal */
} CsrBtSppControlInd;

typedef struct
{
    CsrBtSppPrim            type;                                  /* primitive identity */
    CsrUint8           serverChannel;                         /* local server channel */
    CsrSchedQid                  queueId;                               /* spp instance index protocol handle */
    RFC_PORTNEG_VALUES_T    portPar;                               /* serial port parameters */
    CsrBool                 request;                               /* */
} CsrBtSppPortnegInd;

typedef struct
{
    CsrBtSppPrim            type;                                  /* primitive identity */
    CsrUint8           serverChannel;                         /* local server channel */

    RFC_PORTNEG_VALUES_T    portPar;                               /* serial port parameters */
} CsrBtSppPortnegRes;

typedef struct
{
    CsrBtSppPrim            type;                                  /* primitive identity */
    CsrUint8           serverChannel;                         /* local server channel */
    RFC_PORTNEG_VALUES_T    portPar;                               /* serial port parameters */
} CsrBtSppPortnegReq;

typedef struct
{
    CsrBtSppPrim            type;                                  /* primitive identity */
    CsrUint8           serverChannel;                         /* local server channel */
    CsrSchedQid                  queueId;                               /* spp instance index protocol handle */
    RFC_PORTNEG_VALUES_T    portPar;                               /* serial port parameters */
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtSppPortnegCfm;

typedef struct
{
    CsrBtSppPrim            type;
    CsrSchedQid                  queueId;                               /* spp instance index protocol handle */
    CsrBtSppServiceName     *serviceNameList;
    CsrUint16               serviceNameListSize;                   /* Number of items in serviceNameList, not byte-size! */
} CsrBtSppServiceNameInd;

typedef struct
{
    CsrBtSppPrim            type;
    CsrBtUuid32             serviceHandle;
    CsrBool                 accept;
} CsrBtSppServiceNameRes;

typedef struct
{
    CsrBtSppPrim            type;
} CsrBtSppHouseCleaning;

typedef struct
{
    hci_pkt_type_t          packetType;    /* Specifies which SCO/eSCO packet type to use */
    CsrUint32               txBandwidth;   /* Specifies the maximum Transmission bandwidth to use */
    CsrUint32               rxBandwidth;   /* Specifies the maximum Receive bandwidth to use */
    CsrUint16               maxLatency;    /* Specifies the maximum Latency to use */
    CsrUint16               voiceSettings;
    CsrUint8                reTxEffort;    /* Specifies the Retransmission setting to use */
} CsrBtSppAudioParameterList;

typedef struct
{
    CsrBtSppPrim            type;
    CsrUint8           serverChannel;

    CsrUint8                audioParametersLength;  /* number of entries in the audioParameters pointer */
    CsrBtSppAudioParameterList *audioParameters;    /* Specifies which SCO/eSCO parameters to use in the connection establishment. If NULL the default Audio parameters (ie. the CSR_BT_ESCO_DEFAULT_CONNECT_XXX parameters) from csr_bt_usr_config.h are used */

    CsrUint8                pcmSlot;
    CsrBool                 pcmReassign;
} CsrBtSppAudioConnectReq;

typedef struct
{
    CsrBtSppPrim            type;
    CsrUint8           serverChannel;
    hci_pkt_type_t          audioQuality;
    CsrUint32               txBandwidth;
    CsrUint32               rxBandwidth;
    CsrUint16               maxLatency;
    CsrUint16               voiceSettings;
    CsrUint8                reTxEffort;
} CsrBtSppAcceptAudioReq;

typedef struct
{
    CsrBtSppPrim            type;
    CsrUint8           serverChannel;
    hci_pkt_type_t          audioQuality;
    CsrUint16               maxLatency;
    CsrUint8                reTxEffort;
    hci_connection_handle_t scoHandle;
} CsrBtSppAudioRenegotiateReq;

typedef struct
{
    CsrBtSppPrim            type;
    CsrUint8           serverChannel;
} CsrBtSppCancelAcceptAudioReq;

typedef struct
{
    CsrBtSppPrim            type;
    CsrSchedQid                  queueId;                               /* spp instance index protocol handle */
    CsrUint8           serverChannel;
    CsrUint8                linkType;
    CsrUint8                txInterval;
    CsrUint8                weSco;
    CsrUint16               rxPacketLength;
    CsrUint16               txPacketLength;
    CsrUint8                airMode;
    hci_connection_handle_t scoHandle;
    CsrUint8                pcmSlot;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtSppAudioConnectInd;

typedef CsrBtSppAudioConnectInd CsrBtSppAudioConnectCfm;

typedef struct
{
    CsrBtSppPrim            type;
    CsrSchedQid                  queueId;                               /* spp instance index protocol handle */
    CsrUint8           serverChannel;
    hci_connection_handle_t scoHandle;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtSppAudioRenegotiateInd;

typedef struct
{
    CsrBtSppPrim            type;
    CsrSchedQid                  queueId;                               /* spp instance index protocol handle */
    CsrUint8           serverChannel;
    hci_connection_handle_t scoHandle;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtSppAudioRenegotiateCfm;

typedef struct
{
    CsrBtSppPrim            type;
    CsrUint8           serverChannel;
    CsrUint16               scoHandle;
} CsrBtSppAudioDisconnectReq;

typedef struct
{
    CsrBtSppPrim            type;
    CsrSchedQid                  queueId;                               /* spp instance index protocol handle */
    CsrUint8           serverChannel;
    CsrUint16               scoHandle;
    CsrBtReasonCode         reasonCode;                            /* indicate the status of the connection.*/
    CsrBtSupplier           reasonSupplier;
} CsrBtSppAudioDisconnectInd;

typedef struct
{
    CsrBtSppPrim            type;
    CsrSchedQid                  queueId;                               /* spp instance index protocol handle */
    CsrUint8           serverChannel;
    CsrUint16               scoHandle;
    CsrBtResultCode         resultCode;                            /* indicate the status of the connection.*/
    CsrBtSupplier           resultSupplier;
} CsrBtSppAudioDisconnectCfm;

typedef struct
{
    CsrBtSppPrim            type;                                  /* primitive/message identity */
    CsrUint8           serverChannel;                         /* Local server channel */
    CsrUint8                mode;                                  /* Link mode to request (Active, Sniff or Park)*/
} CsrBtSppModeChangeReq;

typedef struct
{
    CsrBtSppPrim            type;                                  /* primitive/message identity */
    CsrSchedQid                  queueId;                               /* spp instance index protocol handle */
    CsrUint8           serverChannel;                         /* Local server channel */
    CsrUint8                mode;                                  /* Link mode to request (Active, Sniff or Park)*/
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtSppModeChangeInd;

typedef struct
{
    CsrBtSppPrim            type;                                  /* primitive/message identity */
    CsrSchedQid                  phandle;
} CsrBtSppRegisterQidReq;

typedef struct
{
    CsrBtSppPrim            type;                                  /* primitive/message identity */
    CsrSchedQid                  phandle;
} CsrBtSppGetInstancesQidReq;

typedef struct
{
    CsrBtSppPrim            type;                                  /* primitive/message identity */
    CsrUint8                phandlesListSize;                      /* number of _items_ in phandlesList, _not_ length in bytes */
    CsrSchedQid                  *phandlesList;
} CsrBtSppGetInstancesQidCfm;

typedef struct
{
    CsrBtSppPrim            type;                                  /* primitive/message identity */
    CsrSchedQid                  phandle;                               /* protocol handle */
    CsrBtDeviceAddr         deviceAddr;                            /* BT address of device to bonded with. */
    CsrUint8                role;                                  /* is SPP DTC or CSR_BT_DCE */
    CsrBool                 validPortPar;                          /* request CM to execute port neg */
    CsrBool                 requestPortPar;                        /* this is a request or information */
    RFC_PORTNEG_VALUES_T    portPar;                               /* port negotiation parameter (relevant only if requestPortPar is true) */
    uuid16_t                profileUuid;                           /* 16 bit UUIDs */
    dm_security_level_t     secLevel;                              /* Level of outgoing security to be applied */
} CsrBtSppExtendedConnectReq;

typedef struct
{
    CsrBtSppPrim            type;                                  /* primitive/message identity */
    CsrSchedQid                  phandle;                               /* protocol handle */
    CsrBtDeviceAddr         deviceAddr;                            /* BT address of device to bonded with. */
    CsrUint8                role;                                  /* is SPP DTC or CSR_BT_DCE */
    CsrBool                 validPortPar;                          /* request CM to execute port neg */
    CsrBool                 requestPortPar;                        /* this is a request or information */
    RFC_PORTNEG_VALUES_T    portPar;                               /* port negotiation parameter (relevant only if requestPortPar is true */
    CsrBtUuid128            profileUuid;                           /* 128 bit UUIDs */
    dm_security_level_t     secLevel;                              /* Level of outgoing security to be applied */
} CsrBtSppExtendedUuidConnectReq;

typedef struct
{
    CsrBtSppPrim            type;                                  /* primitive/message identity */
    CsrSchedQid                  dataAppHandle;                         /* register this handle for the data application */
} CsrBtSppRegisterDataPathHandleReq;

typedef struct
{
    CsrBtSppPrim            type;                                  /* primitive/message identity */
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtSppRegisterDataPathHandleCfm;

typedef struct
{
    CsrBtSppPrim            type;                                  /* primitive/message identity */
    CsrSchedQid                  phandle;                               /* protocol handle */
    CsrBtCplTimer           timeout;                               /* Holds the pageScanTimeOut time */
    CsrUint16               serverChannelIndex;                    /* index for the location of server channel in service record */
    CsrUint16               serviceRecordSize;                     /* Length of the service record */
    CsrUint8                *serviceRecord;                        /* Alternative service record instead of default */
    dm_security_level_t     secLevel;                              /* Level of outgoing security to be applied */
    CsrUint24               classOfDevice;                         /* The Class Of Device of the profile */
} CsrBtSppExtendedActivateReq;

typedef struct
{
    CsrBtSppPrim            type;                                  /* primitive/message identity */
    CsrUint8                status;                                /* see csr_bt_profiles.h: DATA_PATH_STATUS_xxx */
} CsrBtSppDataPathStatusReq;

typedef struct
{
    CsrBtSppPrim            type;                                  /* primitive/message identity */
    CsrSchedQid                  queueId;                               /* spp instance index protocol handle */
    CsrUint8                status;                                /* see csr_bt_profiles.h: DATA_PATH_STATUS_xxx */
} CsrBtSppDataPathStatusInd;


typedef struct
{
    CsrBtSppPrim            type;
    CsrSchedQid                  queueId;                               /* spp instance index protocol handle */
    CsrUint8           serverChannel;
    CsrUint8                linkType;                       /* specifies SCO/eSCO */
} CsrBtSppAudioAcceptConnectInd;

typedef struct
{
    hci_pkt_type_t          packetTypes;   /* Specifies which SCO/eSCO packet types to accept */
    CsrUint32               txBandwidth;   /* Specifies the maximum Transmission bandwidth to accept */
    CsrUint32               rxBandwidth;   /* Specifies the maximum Receive bandwidth to accept */
    CsrUint16               maxLatency;    /* Specifies the maximum Latency to accept */
    CsrUint16               contentFormat; /* Specifies which SCO/eSCO content format to accept */
    CsrUint8                reTxEffort;    /* Specifies the Retransmission setting(s) to accept */
} CsrBtSppAudioIncomingAcceptParameters;

typedef struct
{
    CsrBtSppPrim            type;
    CsrUint8           serverChannel;
    hci_error_t             acceptResponse;                 /* The HCI response code from profile can be one of the following:
                                                               HCI_SUCCESS, HCI_ERROR_REJ_BY_REMOTE_NO_RES, HCI_ERROR_REJ_BY_REMOTE_PERS
                                                               Note: If this is != HCI_SUCCESS then the incoming SCO/eSCO connection will be rejected */

    CsrBtSppAudioIncomingAcceptParameters *acceptParameters;/* Specifies which SCO/eSCO parameters to accept. If NULL the default ACCEPT SCO parameters given in CsrBtSppAcceptAudioReq will be used */
    CsrUint8                acceptParametersLength;         /* shall be 1 if acceptParameters != NULL otherwise 0 */

    CsrUint8                pcmSlot;
    CsrBool                 pcmReassign;
} CsrBtSppAudioAcceptConnectRes;

typedef struct
{
    CsrBtSppPrim            type;
} CsrBtSppCancelConnectReq;

typedef struct
{
    CsrBtSppPrim            type;
    CsrSchedQid                  appHandle;
    CsrUint16               secLevel;
} CsrBtSppSecurityInReq;

typedef struct
{
    CsrBtSppPrim            type;
    CsrSchedQid                  appHandle;
    CsrUint16               secLevel;
} CsrBtSppSecurityOutReq;

typedef struct
{
    CsrBtSppPrim            type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtSppSecurityInCfm;

typedef struct
{
    CsrBtSppPrim            type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtSppSecurityOutCfm;

#ifdef __cplusplus
}
#endif

#endif
