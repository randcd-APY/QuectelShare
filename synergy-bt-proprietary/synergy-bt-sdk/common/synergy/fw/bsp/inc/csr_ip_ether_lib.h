#ifndef CSR_IP_ETHER_LIB_H__
#define CSR_IP_ETHER_LIB_H__
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_sched.h"
#include "csr_pmem.h"
#include "csr_util.h"
#include "csr_types.h"
#include "csr_msg_transport.h"
#include "csr_ip_ether_prim.h"
#include "csr_ip_ether_task.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      Free the allocated memory in a CSR_IP_ETHER upstream message. Does not
 *      free the message itself, and can only be used for upstream messages.
 *
 *  PARAMETERS
 *      Deallocates the resources in a CSR_IP_ETHER upstream message
 *----------------------------------------------------------------------------*/
void CsrIpEtherFreeUpstreamMessageContents(CsrUint16 eventClass, void *message);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherFreeDownstreamMessageContents
 *
 *  DESCRIPTION
 *      Free the allocated memory in a CSR_IP_ETHER upstream message. Does not
 *      free the message itself, and can only be used for downstream messages.
 *
 *  PARAMETERS
 *      Deallocates the resources in a CSR_IP_ETHER upstream message
 *----------------------------------------------------------------------------*/
void CsrIpEtherFreeDownstreamMessageContents(CsrUint16 eventClass, void *message);


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfAddReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpEtherIfAddReq *CsrIpEtherIfAddReq_struct(CsrSchedQid appHandle, CsrIpEtherIfType ifType, CsrUint8 *mac, CsrUint8 encapsulation, CsrUint16 maxTxUnit, CsrIpEtherFrameTxFunction frameTxFunction, CsrIpEtherIfContext ifContext);
#define CsrIpEtherIfAddReqSend(appHandle, ifType, mac, encapsulation, maxTxUnit, frameTxFunction, ifContext) \
    { \
        CsrIpEtherIfAddReq *msg__ = CsrIpEtherIfAddReq_struct(appHandle, ifType, mac, encapsulation, maxTxUnit, frameTxFunction, ifContext); \
        CsrMsgTransport(CSR_IP_ETHER_IFACEQUEUE, CSR_IP_ETHER_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfAddCfmSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpEtherIfAddCfm *CsrIpEtherIfAddCfm_struct(CsrUint32 ifHandle, CsrResult result, CsrIpEtherEncapsulation encapsulation, CsrIpEtherFrameRxFunction frameRxFunction, CsrIpEtherIpContext ipContext);
#define CsrIpEtherIfAddCfmSend(queue, ifHandle, result, encapsulation, frameRxFunction, ipContext) \
    { \
        CsrIpEtherIfAddCfm *msg__ = CsrIpEtherIfAddCfm_struct(ifHandle, result, encapsulation, frameRxFunction, ipContext); \
        CsrSchedMessagePut(queue, CSR_IP_ETHER_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfRemoveReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpEtherIfRemoveReq *CsrIpEtherIfRemoveReq_struct(CsrSchedQid appHandle, CsrUint32 ifHandle);
#define CsrIpEtherIfRemoveReqSend(appHandle, ifHandle) \
    { \
        CsrIpEtherIfRemoveReq *msg__ = CsrIpEtherIfRemoveReq_struct(appHandle, ifHandle); \
        CsrMsgTransport(CSR_IP_ETHER_IFACEQUEUE, CSR_IP_ETHER_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfRemoveCfmSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpEtherIfRemoveCfm *CsrIpEtherIfRemoveCfm_struct(CsrUint32 ifHandle, CsrResult result);
#define CsrIpEtherIfRemoveCfmSend(queue, ifHandle, result) \
    { \
        CsrIpEtherIfRemoveCfm *msg__ = CsrIpEtherIfRemoveCfm_struct(ifHandle, result); \
        CsrSchedMessagePut(queue, CSR_IP_ETHER_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfLinkUpReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpEtherIfLinkUpReq *CsrIpEtherIfLinkUpReq_struct(CsrSchedQid appHandle, CsrUint32 ifHandle);
#define CsrIpEtherIfLinkUpReqSend(appHandle, ifHandle) \
    { \
        CsrIpEtherIfLinkUpReq *msg__ = CsrIpEtherIfLinkUpReq_struct(appHandle, ifHandle); \
        CsrMsgTransport(CSR_IP_ETHER_IFACEQUEUE, CSR_IP_ETHER_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfLinkUpCfmSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpEtherIfLinkUpCfm *CsrIpEtherIfLinkUpCfm_struct(CsrUint32 ifHandle, CsrResult result);
#define CsrIpEtherIfLinkUpCfmSend(queue, ifHandle, result) \
    { \
        CsrIpEtherIfLinkUpCfm *msg__ = CsrIpEtherIfLinkUpCfm_struct(ifHandle, result); \
        CsrSchedMessagePut(queue, CSR_IP_ETHER_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfLinkDownReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpEtherIfLinkDownReq *CsrIpEtherIfLinkDownReq_struct(CsrSchedQid appHandle, CsrUint32 ifHandle);
#define CsrIpEtherIfLinkDownReqSend(appHandle, ifHandle) \
    { \
        CsrIpEtherIfLinkDownReq *msg__ = CsrIpEtherIfLinkDownReq_struct(appHandle, ifHandle); \
        CsrMsgTransport(CSR_IP_ETHER_IFACEQUEUE, CSR_IP_ETHER_PRIM, msg__); \
    }
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfLinkDownCfmSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpEtherIfLinkDownCfm *CsrIpEtherIfLinkDownCfm_struct(CsrUint32 ifHandle, CsrResult result);
#define CsrIpEtherIfLinkDownCfmSend(queue, ifHandle, result) \
    { \
        CsrIpEtherIfLinkDownCfm *msg__ = CsrIpEtherIfLinkDownCfm_struct(ifHandle, result); \
        CsrSchedMessagePut(queue, CSR_IP_ETHER_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfMulticastAddrAddResSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpEtherIfMulticastAddrAddRes *CsrIpEtherIfMulticastAddrAddRes_struct(CsrUint32 ifHandle, CsrResult result);
#define CsrIpEtherIfMulticastAddrAddResSend(ifHandle, result) \
    { \
        CsrIpEtherIfMulticastAddrAddRes *msg__ = CsrIpEtherIfMulticastAddrAddRes_struct(ifHandle, result); \
        CsrMsgTransport(CSR_IP_ETHER_IFACEQUEUE, CSR_IP_ETHER_PRIM, msg__); \
    }
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfMulticastAddrAddIndSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpEtherIfMulticastAddrAddInd *CsrIpEtherIfMulticastAddrAddInd_struct(CsrUint32 ifHandle, CsrUint8 *multicastMacAddr);
#define CsrIpEtherIfMulticastAddrAddIndSend(queue, ifHandle, multicastMacAddr) \
    { \
        CsrIpEtherIfMulticastAddrAddInd *msg__ = CsrIpEtherIfMulticastAddrAddInd_struct(ifHandle, multicastMacAddr); \
        CsrSchedMessagePut(queue, CSR_IP_ETHER_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfMulticastAddrRemoveResSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpEtherIfMulticastAddrRemoveRes *CsrIpEtherIfMulticastAddrRemoveRes_struct(CsrUint32 ifHandle, CsrResult result);
#define CsrIpEtherIfMulticastAddrRemoveResSend(ifHandle, result) \
    { \
        CsrIpEtherIfMulticastAddrRemoveRes *msg__ = CsrIpEtherIfMulticastAddrRemoveRes_struct(ifHandle, result); \
        CsrMsgTransport(CSR_IP_ETHER_IFACEQUEUE, CSR_IP_ETHER_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfMulticastAddrRemoveIndSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpEtherIfMulticastAddrRemoveInd *CsrIpEtherIfMulticastAddrRemoveInd_struct(CsrUint32 ifHandle, CsrUint8 *multicastMacAddr);
#define CsrIpEtherIfMulticastAddrRemoveIndSend(queue, ifHandle, multicastMacAddr) \
    { \
        CsrIpEtherIfMulticastAddrRemoveInd *msg__ = CsrIpEtherIfMulticastAddrRemoveInd_struct(ifHandle, multicastMacAddr); \
        CsrSchedMessagePut(queue, CSR_IP_ETHER_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfMulticastAddrFlushResSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpEtherIfMulticastAddrFlushRes *CsrIpEtherIfMulticastAddrFlushRes_struct(CsrUint32 ifHandle, CsrResult result);
#define CsrIpEtherIfMulticastAddrFlushResSend(ifHandle, result) \
    { \
        CsrIpEtherIfMulticastAddrFlushRes *msg__ = CsrIpEtherIfMulticastAddrFlushRes_struct(ifHandle, result); \
        CsrMsgTransport(CSR_IP_ETHER_IFACEQUEUE, CSR_IP_ETHER_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfMulticastAddrFlushIndSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpEtherIfMulticastAddrFlushInd *CsrIpEtherIfMulticastAddrFlushInd_struct(CsrUint32 ifHandle);
#define CsrIpEtherIfMulticastAddrFlushIndSend(queue, ifHandle) \
    { \
        CsrIpEtherIfMulticastAddrFlushInd *msg__ = CsrIpEtherIfMulticastAddrFlushInd_struct(ifHandle); \
        CsrSchedMessagePut(queue, CSR_IP_ETHER_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfFlowControlPauseReq
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpEtherIfFlowControlPauseReq *CsrIpEtherIfFlowControlPauseReq_struct(CsrUint32 ifHandle, CsrIpEtherPriority priority);
#define CsrIpEtherIfFlowControlPauseReqSend(appHandle, ifHandle, priority) \
    { \
        CsrIpEtherIfFlowControlPauseReq *msg__ = CsrIpEtherIfFlowControlPauseReq_struct(ifHandle, priority); \
        CsrSchedMessagePut(CSR_IP_IFACEQUEUE, CSR_IP_ETHER_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfFlowControlResumeReq
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpEtherIfFlowControlResumeReq *CsrIpEtherIfFlowControlResumeReq_struct(CsrUint32 ifHandle, CsrIpEtherPriority priority);
#define CsrIpEtherIfFlowControlResumeReqSend(appHandle, ifHandle, priority) \
    { \
        CsrIpEtherIfFlowControlResumeReq *msg__ = CsrIpEtherIfFlowControlResumeReq_struct(ifHandle, priority); \
        CsrSchedMessagePut(CSR_IP_IFACEQUEUE, CSR_IP_ETHER_PRIM, msg__); \
    }

#ifdef __cplusplus
}
#endif

#endif
