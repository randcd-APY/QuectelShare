#ifndef CSR_BT_MCAP_PRIVATE_LIB_H__
#define CSR_BT_MCAP_PRIVATE_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_msg_transport.h"
#include "csr_bt_profiles.h"
#include "csr_bt_mcap_private_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtMcapMsgTransport(void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMcapActivateReqSend
 *
 *  DESCRIPTION
 *      Activates the MCAP profile.
 *
 *  PARAMETERS
 *      psm;
 *      phandle;
 *      classOfDevice;
 *      secLevel;
 *      mtu;
*----------------------------------------------------------------------------*/
#define CsrBtMcapActivateReqSend(_psm, _phandle, _classOfDevice, _secLevel, _mtu, _sniffTimeout, _uuid, _noOfConnections) { \
        CsrBtMcapActivateReq *msg__ = (CsrBtMcapActivateReq  *) CsrPmemAlloc(sizeof(CsrBtMcapActivateReq)); \
        msg__->type              = CSR_BT_MCAP_ACTIVATE_REQ;            \
        msg__->psm               = _psm;                                \
        msg__->phandle           = _phandle;                            \
        msg__->classOfDevice     = _classOfDevice;                      \
        msg__->secLevel          = _secLevel;                           \
        msg__->mtu               = _mtu;                                \
        msg__->sniffTimeout      = _sniffTimeout;                       \
        msg__->uuid              = _uuid;                               \
        msg__->noOfConnections   = _noOfConnections;                    \
        CsrBtMcapMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMcapDeactivateReqSend
 *
 *  DESCRIPTION
 *      Deactivates the MCAP profile.
 *
 *  PARAMETERS
 *      psm;
 *      phandle;
 *----------------------------------------------------------------------------*/
#define CsrBtMcapDeactivateReqSend(_psm, _phandle) {                    \
        CsrBtMcapDeactivateReq *msg__ = (CsrBtMcapDeactivateReq  *) CsrPmemAlloc(sizeof(CsrBtMcapDeactivateReq)); \
        msg__->type              = CSR_BT_MCAP_DEACTIVATE_REQ;          \
        msg__->psm               = _psm;                                \
        msg__->phandle           = _phandle;                            \
        CsrBtMcapMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMcapConnectMclReqSend
 *
 *  DESCRIPTION
 *      Create the MCL connection
 *
 *  PARAMETERS
 *      bd_addr,
 *      psm,
 *      phandle,
 *      secLevel;
 *      mtu;
 *----------------------------------------------------------------------------*/
void CsrBtMcapConnectMclReqSend(CsrBtDeviceAddr     bd_addr,
                                psm_t               localPsm,
                                psm_t               remotePsm,
                                CsrSchedQid         phandle,
                                dm_security_level_t secLevel,
                                CsrTime             sniffTimeout);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMcapDisconnectMclReqSend
 *
 *  DESCRIPTION
 *      Disconnect the MCL connection
 *
 *  PARAMETERS
 *      bd_addr,
 *      psm,
 *      phandle,
 *----------------------------------------------------------------------------*/
void CsrBtMcapDisconnectMclReqSend(CsrBtDeviceAddr bd_addr,
                                   psm_t           psm,
                                   CsrSchedQid     phandle);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMcapCreateMdlResSend
 *
 *  DESCRIPTION
 *      Send an MD_CREATE_MDL_RES to remote device using the communication channel.
 *
 *  PARAMETERS
 *      bd_addr,
 *      mdlId;
 *      mcapResponse;
 *      configResponse;
 *      psm;
 *      secLevel;
 *      mtu;
 *      *flow;
 *----------------------------------------------------------------------------*/
void CsrBtMcapCreateMdlResSend(CsrUint16               mclId,
                               CsrUint16               mdlId,
                               McapResponseType        mcapResponse,
                               CsrUint8                configResponse,
                               psm_t                   psm,
                               dm_security_level_t     secLevel,
                               l2ca_mtu_t              mtu,
                               L2CA_FLOW_T             *flow);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMcapReconnectMdlReqSend
 *
 *  DESCRIPTION
 *      Send an MD_RECONNECT_MDL_REQ to remote device using the communication channel.
 *
 *  PARAMETERS
 *      bd_addr,
 *      mdlId;
 *----------------------------------------------------------------------------*/
void CsrBtMcapReconnectMdlReqSend(CsrUint16        mclId,
                                  CsrUint16        mdlId,
                                  psm_t           localPsm,
                                  psm_t           remotePsm);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMcapReconnectMdlResSend
 *
 *  DESCRIPTION
 *      Send an MD_RECONNECT_MDL_RES to remote device using the communication channel.
 *
 *  PARAMETERS
 *      bd_addr,
 *      mdlId;
 *      mcapResponse;
 *      secLevel;
 *      mtu;
 *      *flow;
 *----------------------------------------------------------------------------*/
void CsrBtMcapReconnectMdlResSend(CsrUint16               mclId,
                                  CsrUint16               mdlId,
                                  McapResponseType        mcapResponse,
                                  psm_t                   psm,
                                  dm_security_level_t     secLevel,
                                  l2ca_mtu_t              mtu,
                                  L2CA_FLOW_T            *flow);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMcapConnectMdlReqSend
 *
 *  DESCRIPTION
 *      Create an MDL connection
 *
 *  PARAMETERS
 *      bd_addr,
 *      mdlId;
 *      psm;
 *      secLevel;
 *      mtu;
 *      *flow;
 *----------------------------------------------------------------------------*/
void CsrBtMcapConnectMdlReqSend(CsrUint16                mclId,
                                CsrUint16                mdlId,
                                CsrUint8                 mdepId,
                                CsrUint8                 config,
                                psm_t                    localPsm,
                                psm_t                    remotePsm,
                                dm_security_level_t      secLevel,
                                l2ca_mtu_t               mtu,
                                L2CA_FLOW_T             *flow);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMcapDisconnectMdlReqSend
 *
 *  DESCRIPTION
 *      Disconnect the MDL connection
 *
 *  PARAMETERS
 *      bd_addr,
 *      mdlId;
 *----------------------------------------------------------------------------*/
void CsrBtMcapDisconnectMdlReqSend(CsrUint16        mclId,
                                   CsrUint16        mdlId);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMcapAbortMdlResSend
 *
 *  DESCRIPTION
 *      Send an MD_ABORT_MDL_RES to remote device using the communication channel.
 *
 *  PARAMETERS
 *      bd_addr,
 *      mdlId;
 *      mcapResponse;
 *----------------------------------------------------------------------------*/
#define CsrBtMcapAbortMdlResSend(_mclId, _mdlId, _mcapResponse) {       \
        CsrBtMcapAbortMdlRes *msg__ = (CsrBtMcapAbortMdlRes  *) CsrPmemAlloc(sizeof(CsrBtMcapAbortMdlRes)); \
        msg__->type              = CSR_BT_MCAP_ABORT_MDL_RES;           \
        msg__->mclId             = _mclId;                              \
        msg__->mdlId             = _mdlId;                              \
        msg__->mcapResponse      = _mcapResponse;                       \
        CsrBtMcapMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMcapDeleteMdlReqSend
 *
 *  DESCRIPTION
 *      Send an MD_DELETE_MDL_REQ to remote device using the communication channel.
 *
 *  PARAMETERS
 *      bd_addr,
 *      mdlId;
 *----------------------------------------------------------------------------*/
void CsrBtMcapDeleteMdlReqSend(CsrUint16        mclId,
                               CsrUint16        mdlId);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMcapDeleteMdlResSend
 *
 *  DESCRIPTION
 *      Send an MD_DELETE_MDL_RES to remote device using the communication channel.
 *
 *  PARAMETERS
 *      bd_addr,
 *      mdlId;
 *      mcapResponse;
 *----------------------------------------------------------------------------*/
void CsrBtMcapDeleteMdlResSend(CsrUint16            mclId,
                               CsrUint16            mdlId,
                               McapResponseType    mcapResponse);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMcapDataSendReqSend
 *
 *  DESCRIPTION
 *      Send data on an MDL connection
 *
 *  PARAMETERS
 *      bd_addr,
 *      mdlId;
 *      length;
 *      *payload;
 *----------------------------------------------------------------------------*/
void CsrBtMcapDataSendReqSend(CsrUint16        mclId,
                              CsrUint16        mdlId,
                              CsrUint16        length,
                              void            *payload);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMcapSyncCapReqSend
 *
 *  DESCRIPTION
 *      Send an MD_SYNC_CAP_REQ to remote device using the communication channel.
 *
 *  PARAMETERS
 *      bd_addr,
 *      timeStampRequiredAccuracy;
 *----------------------------------------------------------------------------*/
#define CsrBtMcapSyncCapReqSend(_mclId, _timeStampRequiredAccuracy) {   \
        CsrBtMcapSyncCapReq *msg__       = (CsrBtMcapSyncCapReq  *) CsrPmemAlloc(sizeof(CsrBtMcapSyncCapReq)); \
        msg__->type                      = CSR_BT_MCAP_SYNC_CAP_REQ;    \
        msg__->mclId                     = _mclId;                      \
        msg__->timeStampRequiredAccuracy = _timeStampRequiredAccuracy;  \
        CsrBtMcapMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMcapSyncCapResSend
 *
 *  DESCRIPTION
 *      Send an MD_SYNC_CAP_RES to remote device using the communication channel.
 *
 *  PARAMETERS
 *      bd_addr,
 *      mcapResponse;
 *      btClkAccessRes;
 *      syncLeadTime;
 *      timeStampNativeRes;
 *      timeStampNativeAccuracy;
 *----------------------------------------------------------------------------*/
#define CsrBtMcapSyncCapResSend(_mclId, _mcapResponse, _btClkAccessRes, _syncLeadTime, _timeStampNativeRes, _timeStampNativeAccuracy) { \
        CsrBtMcapSyncCapRes *msg__       = (CsrBtMcapSyncCapRes  *) CsrPmemAlloc(sizeof(CsrBtMcapSyncCapRes)); \
        msg__->type                      = CSR_BT_MCAP_SYNC_CAP_RES;    \
        msg__->mclId                     = _mclId;                      \
        msg__->mcapResponse              = _mcapResponse;               \
        msg__->btClkAccessRes            = _btClkAccessRes;             \
        msg__->syncLeadTime              = _syncLeadTime;               \
        msg__->timeStampNativeRes        = _timeStampNativeRes;         \
        msg__->timeStampNativeAccuracy   = _timeStampNativeAccuracy;    \
        CsrBtMcapMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMcapSyncSetReqSend
 *
 *  DESCRIPTION
 *      Send an MD_SYNC_SET_REQ to remote device using the communication channel.
 *
 *  PARAMETERS
 *      bd_addr,
 *      timeStampRequiredAccuracy;
 *----------------------------------------------------------------------------*/
#define CsrBtMcapSyncSetReqSend(_mclId, _timeStampUpdateInfo, _btClkSyncTime, _timeStampSyncTimeLsb, _timeStampSyncTimeMsb) { \
        CsrBtMcapSyncSetReq *msg__       = (CsrBtMcapSyncSetReq  *) CsrPmemAlloc(sizeof(CsrBtMcapSyncSetReq)); \
        msg__->type                      = CSR_BT_MCAP_SYNC_SET_REQ;    \
        msg__->mclId                     = _mclId;                      \
        msg__->timeStampUpdateInfo       = _timeStampUpdateInfo;        \
        msg__->btClkSyncTime             = _btClkSyncTime;              \
        msg__->timeStampSyncTimeLsb      = _timeStampSyncTimeLsb;       \
        msg__->timeStampSyncTimeMsb      = _timeStampSyncTimeMsb;       \
        CsrBtMcapMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMcapSyncSetResSend
 *
 *  DESCRIPTION
 *      Send an MD_SYNC_SET_RES to remote device using the communication channel.
 *
 *  PARAMETERS
 *      bd_addr,
 *      mcapResponse;
 *      btClkAccessRes;
 *      syncLeadTime;
 *      timeStampSyncTime;
 *      timeStampSampleAccuracy;
 *----------------------------------------------------------------------------*/
#define CsrBtMcapSyncSetResSend(_mclId, _mcapResponse, _btClkSyncTime, _timeStampSyncTimeLsb, _timeStampSyncTimeMsb, _timeStampSampleAccuracy) { \
        CsrBtMcapSyncSetRes *msg__       = (CsrBtMcapSyncSetRes  *) CsrPmemAlloc(sizeof(CsrBtMcapSyncSetRes)); \
        msg__->type                      = CSR_BT_MCAP_SYNC_SET_RES;    \
        msg__->mclId                     = _mclId;                      \
        msg__->mcapResponse              = _mcapResponse;               \
        msg__->btClkSyncTime             = _btClkSyncTime;              \
        msg__->timeStampSyncTimeLsb      = _timeStampSyncTimeLsb;       \
        msg__->timeStampSyncTimeMsb      = _timeStampSyncTimeMsb;       \
        msg__->timeStampSampleAccuracy   = _timeStampSampleAccuracy;    \
        CsrBtMcapMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMcapSyncInfoReqSend
 *
 *  DESCRIPTION
 *      Send an MD_SYNC_INFO_IND to remote device using the communication channel.
 *
 *  PARAMETERS
 *      bd_addr,
 *      btClkAccessRes;
 *      syncLeadTime;
 *      timeStampSyncTime;
 *      timeStampSampleAccuracy;
 *----------------------------------------------------------------------------*/
#define CsrBtMcapSyncInfoReqSend(_mclId, _btClkSyncTime, _timeStampSyncTimeLsb, _timeStampSyncTimeMsb, _timeStampSampleAccuracy) { \
        CsrBtMcapSyncInfoReq *msg__      = (CsrBtMcapSyncInfoReq  *) CsrPmemAlloc(sizeof(CsrBtMcapSyncInfoReq)); \
        msg__->type                      = CSR_BT_MCAP_SYNC_INFO_REQ;   \
        msg__->mclId                     = _mclId;                      \
        msg__->btClkSyncTime             = _btClkSyncTime;              \
        msg__->timeStampSyncTimeLsb      = _timeStampSyncTimeLsb;       \
        msg__->timeStampSyncTimeMsb      = _timeStampSyncTimeMsb;       \
        msg__->timeStampSampleAccuracy   = _timeStampSampleAccuracy;    \
        CsrBtMcapMsgTransport(msg__);}

#ifdef __cplusplus
}
#endif

#endif

