#ifndef CSR_IP_IFCONFIG_LIB_H__
#define CSR_IP_IFCONFIG_LIB_H__
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
#include "csr_ip_ifconfig_prim.h"
#include "csr_ip_ifconfig_task.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      Free the allocated memory in a CSR_IP_IFCONFIG upstream message. Does not
 *      free the message itself, and can only be used for upstream messages.
 *
 *  PARAMETERS
 *      Deallocates the resources in a CSR_IP_IFCONFIG upstream message
 *----------------------------------------------------------------------------*/
void CsrIpIfconfigFreeUpstreamMessageContents(CsrUint16 eventClass, void *message);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigFreeDownstreamMessageContents
 *
 *  DESCRIPTION
 *      Free the allocated memory in a CSR_IP_IFCONFIG downstream message. Does not
 *      free the message itself, and can only be used for downstream messages.
 *
 *  PARAMETERS
 *      Deallocates the resources in a CSR_IP_IFCONFIG downstream message
 *----------------------------------------------------------------------------*/
void CsrIpIfconfigFreeDownstreamMessageContents(CsrUint16 eventClass, void *message);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigSubscribeReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpIfconfigSubscribeReq *CsrIpIfconfigSubscribeReq_struct(CsrSchedQid appHandle);
#define CsrIpIfconfigSubscribeReqSend(appHandle) \
    { \
        CsrIpIfconfigSubscribeReq *msg__ = CsrIpIfconfigSubscribeReq_struct(appHandle); \
        CsrMsgTransport(CSR_IP_IFCONFIG_IFACEQUEUE, CSR_IP_IFCONFIG_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigUnsubscribeReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpIfconfigUnsubscribeReq *CsrIpIfconfigUnsubscribeReq_struct(CsrSchedQid appHandle);
#define CsrIpIfconfigUnsubscribeReqSend(appHandle) \
    { \
        CsrIpIfconfigUnsubscribeReq *msg__ = CsrIpIfconfigUnsubscribeReq_struct(appHandle); \
        CsrMsgTransport(CSR_IP_IFCONFIG_IFACEQUEUE, CSR_IP_IFCONFIG_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigUpReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpIfconfigUpReq *CsrIpIfconfigUpReq_struct(CsrSchedQid appHandle, CsrUint32 ifHandle, CsrIpIfconfigConfigMethod configMethod, CsrUint8 *ipAddress, CsrUint8 *gatewayIpAddress, CsrUint8 *networkMask, CsrUint8 *dnsPrimaryIpAddress, CsrUint8 *dnsSecondaryIpAddress);
#define CsrIpIfconfigUpReqSend(appHandle, ifHandle, configMethod, ipAddress, gatewayIpAddress, networkMask, dnsPrimaryIpAddress, dnsSecondaryIpAddress) \
    { \
        CsrIpIfconfigUpReq *msg__ = CsrIpIfconfigUpReq_struct(appHandle, ifHandle, configMethod, ipAddress, gatewayIpAddress, networkMask, dnsPrimaryIpAddress, dnsSecondaryIpAddress); \
        CsrMsgTransport(CSR_IP_IFCONFIG_IFACEQUEUE, CSR_IP_IFCONFIG_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigDownReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpIfconfigDownReq *CsrIpIfconfigDownReq_struct(CsrSchedQid appHandle, CsrUint32 ifHandle);
#define CsrIpIfconfigDownReqSend(appHandle, ifHandle) \
    { \
        CsrIpIfconfigDownReq *msg__ = CsrIpIfconfigDownReq_struct(appHandle, ifHandle); \
        CsrMsgTransport(CSR_IP_IFCONFIG_IFACEQUEUE, CSR_IP_IFCONFIG_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigNatReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpIfconfigNatReq *CsrIpIfconfigNatReq_struct(CsrSchedQid appHandle, CsrUint32 ifHandle, CsrBool enable);
#define CsrIpIfconfigNatReqSend(appHandle, ifHandle, enable) \
    { \
        CsrIpIfconfigNatReq *msg__ = CsrIpIfconfigNatReq_struct(appHandle, ifHandle, enable); \
        CsrMsgTransport(CSR_IP_IFCONFIG_IFACEQUEUE, CSR_IP_IFCONFIG_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigForwardReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpIfconfigForwardReq *CsrIpIfconfigForwardReq_struct(CsrSchedQid appHandle, CsrBool enable);
#define CsrIpIfconfigForwardReqSend(appHandle, enable) \
    { \
        CsrIpIfconfigForwardReq *msg__ = CsrIpIfconfigForwardReq_struct(appHandle, enable); \
        CsrMsgTransport(CSR_IP_IFCONFIG_IFACEQUEUE, CSR_IP_IFCONFIG_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigArpEntryAddReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpIfconfigArpEntryAddReq *CsrIpIfconfigArpEntryAddReq_struct(CsrSchedQid appHandle, CsrUint8 mac[6], CsrUint8 ipAddress[4]);
#define CsrIpIfconfigArpEntryAddReqSend(appHandle, mac, ipAddress) \
    { \
        CsrIpIfconfigArpEntryAddReq *msg__ = CsrIpIfconfigArpEntryAddReq_struct(appHandle, mac, ipAddress); \
        CsrMsgTransport(CSR_IP_IFCONFIG_IFACEQUEUE, CSR_IP_IFCONFIG_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigSubscribeCfmSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpIfconfigSubscribeCfm *CsrIpIfconfigSubscribeCfm_struct(CsrUint16 ifDetailsCount, CsrIpIfconfigIfDetails *ifDetails);
#define CsrIpIfconfigSubscribeCfmSend(queue, ifDetailsCount, ifDetails) \
    { \
        CsrIpIfconfigSubscribeCfm *msg__ = CsrIpIfconfigSubscribeCfm_struct(ifDetailsCount, ifDetails); \
        CsrSchedMessagePut(queue, CSR_IP_IFCONFIG_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigUnsubscribeCfmSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpIfconfigUnsubscribeCfm *CsrIpIfconfigUnsubscribeCfm_struct(void);
#define CsrIpIfconfigUnsubscribeCfmSend(queue) \
    { \
        CsrIpIfconfigUnsubscribeCfm *msg__ = CsrIpIfconfigUnsubscribeCfm_struct(); \
        CsrSchedMessagePut(queue, CSR_IP_IFCONFIG_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigUpdateIndSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpIfconfigUpdateInd *CsrIpIfconfigUpdateInd_struct(CsrIpIfconfigIfDetails ifDetails);
#define CsrIpIfconfigUpdateIndSend(queue, ifDetails) \
    { \
        CsrIpIfconfigUpdateInd *msg__ = CsrIpIfconfigUpdateInd_struct(ifDetails); \
        CsrSchedMessagePut(queue, CSR_IP_IFCONFIG_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigUpCfmSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpIfconfigUpCfm *CsrIpIfconfigUpCfm_struct(CsrUint32 ifHandle, CsrResult result);
#define CsrIpIfconfigUpCfmSend(queue, ifHandle, result) \
    { \
        CsrIpIfconfigUpCfm *msg__ = CsrIpIfconfigUpCfm_struct(ifHandle, result); \
        CsrSchedMessagePut(queue, CSR_IP_IFCONFIG_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigDownCfmSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpIfconfigDownCfm *CsrIpIfconfigDownCfm_struct(CsrUint32 ifHandle, CsrResult result);
#define CsrIpIfconfigDownCfmSend(queue, ifHandle, result) \
    { \
        CsrIpIfconfigDownCfm *msg__ = CsrIpIfconfigDownCfm_struct(ifHandle, result); \
        CsrSchedMessagePut(queue, CSR_IP_IFCONFIG_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigNatCfmSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpIfconfigNatCfm *CsrIpIfconfigNatCfm_struct(CsrUint32 ifHandle, CsrResult result);
#define CsrIpIfconfigNatCfmSend(queue, ifHandle, result) \
    { \
        CsrIpIfconfigNatCfm *msg__ = CsrIpIfconfigNatCfm_struct(ifHandle, result); \
        CsrSchedMessagePut(queue, CSR_IP_IFCONFIG_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigForwardCfmSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpIfconfigForwardCfm *CsrIpIfconfigForwardCfm_struct(CsrResult result);
#define CsrIpIfconfigForwardCfmSend(queue, result) \
    { \
        CsrIpIfconfigForwardCfm *msg__ = CsrIpIfconfigForwardCfm_struct(result); \
        CsrSchedMessagePut(queue, CSR_IP_IFCONFIG_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigArpEntryAddCfmSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpIfconfigArpEntryAddCfm *CsrIpIfconfigArpEntryAddCfm_struct(CsrResult result);
#define CsrIpIfconfigArpEntryAddCfmSend(queue, result) \
    { \
        CsrIpIfconfigArpEntryAddCfm *msg__ = CsrIpIfconfigArpEntryAddCfm_struct(result); \
        CsrSchedMessagePut(queue, CSR_IP_IFCONFIG_PRIM, msg__); \
    }


#ifdef __cplusplus
}
#endif

#endif
