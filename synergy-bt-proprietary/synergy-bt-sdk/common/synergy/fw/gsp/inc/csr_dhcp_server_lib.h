#ifndef CSR_DHCP_SERVER_LIB_H__
#define CSR_DHCP_SERVER_LIB_H__
/*****************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_dhcp_server_prim.h"
#include "csr_dhcp_server_task.h"
#include "csr_msg_transport.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrDhcpServerCreateReqSend
 *
 *  DESCRIPTION
 *
 *
 *
 *  PARAMETERS
 *      qid           :
 *      ipAddress     :
 *      network       :
 *      prefixLength  :
 *      gateway       :
 *      dns1          :
 *      dns2          :
 *      leaseTime     :
 *----------------------------------------------------------------------------*/
CsrDhcpServerCreateReq *CsrDhcpServerCreateReq_struct(CsrSchedQid qid,
    CsrUint8 ipAddress[4],
    CsrUint8 network[4],
    CsrUint8 networkMask[4],
    CsrUint8 gateway[4],
    CsrUint8 dns1[4],
    CsrUint8 dns2[4],
    CsrUint8 leaseTime);

#define CsrDhcpServerCreateReqSend(_qid, _ip, _nw, _nwm, _gw, _dns1, _dns2, _time){ \
        CsrDhcpServerCreateReq *msg__; \
        msg__ = CsrDhcpServerCreateReq_struct(_qid, _ip, _nw, _nwm, _gw, _dns1, _dns2, _time); \
        CsrMsgTransport(CSR_DHCP_SERVER_IFACEQUEUE, CSR_DHCP_SERVER_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrDhcpServerDestroyReqSend
 *
 *  DESCRIPTION
 *
 *
 *
 *  PARAMETERS
 *      serverHandle  :
 *----------------------------------------------------------------------------*/
CsrDhcpServerDestroyReq *CsrDhcpServerDestroyReq_struct(CsrDhcpServerHandle serverHandle);

#define CsrDhcpServerDestroyReqSend(_sh){      \
        CsrDhcpServerDestroyReq *msg__; \
        msg__ = CsrDhcpServerDestroyReq_struct(_sh);                        \
        CsrMsgTransport(CSR_DHCP_SERVER_IFACEQUEUE, CSR_DHCP_SERVER_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrDhcpServerStartReqSend
 *
 *  DESCRIPTION
 *
 *
 *
 *  PARAMETERS
 *      serverHandle  :
 *----------------------------------------------------------------------------*/
CsrDhcpServerStartReq *CsrDhcpServerStartReq_struct(CsrDhcpServerHandle serverHandle);

#define CsrDhcpServerStartReqSend(_sh){ \
        CsrDhcpServerStartReq *msg__; \
        msg__ = CsrDhcpServerStartReq_struct(_sh); \
        CsrMsgTransport(CSR_DHCP_SERVER_IFACEQUEUE, CSR_DHCP_SERVER_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrDhcpServerStopReqSend
 *
 *  DESCRIPTION
 *
 *
 *
 *  PARAMETERS
 *      serverHandle  :
 *----------------------------------------------------------------------------*/
CsrDhcpServerStopReq *CsrDhcpServerStopReq_struct(CsrDhcpServerHandle serverHandle);

#define CsrDhcpServerStopReqSend(_sh){ \
        CsrDhcpServerStopReq *msg__; \
        msg__ = CsrDhcpServerStopReq_struct(_sh); \
        CsrMsgTransport(CSR_DHCP_SERVER_IFACEQUEUE, CSR_DHCP_SERVER_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrDhcpServerLeasesGetReqSend
 *
 *  DESCRIPTION
 *
 *
 *
 *  PARAMETERS
 *      serverHandle  :
 *----------------------------------------------------------------------------*/
CsrDhcpServerLeasesGetReq *CsrDhcpServerLeasesGetReq_struct(CsrDhcpServerHandle serverHandle);

#define CsrDhcpServerLeasesGetReqSend(_sh){ \
        CsrDhcpServerLeasesGetReq *msg__; \
        msg__ = CsrDhcpServerLeasesGetReq_struct(_sh); \
        CsrMsgTransport(CSR_DHCP_SERVER_IFACEQUEUE, CSR_DHCP_SERVER_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrDhcpServerLeasesSetReqSend
 *
 *  DESCRIPTION
 *
 *
 *
 *  PARAMETERS
 *      serverHandle  :
 *      leasesCount   :
 *      leases        :
 *----------------------------------------------------------------------------*/
CsrDhcpServerLeasesSetReq *CsrDhcpServerLeasesSetReq_struct(CsrDhcpServerHandle serverHandle,
    CsrUint16 leasesCount,
    CsrDhcpServerLease *leases);

#define CsrDhcpServerLeasesSetReqSend(_sh, _co, _le){  \
        CsrDhcpServerLeasesSetReq *msg__; \
        msg__ = CsrDhcpServerLeasesSetReq_struct(_sh, _co, _le);            \
        CsrMsgTransport(CSR_DHCP_SERVER_IFACEQUEUE, CSR_DHCP_SERVER_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrDhcpServerFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      Deallocates the payload in the CSR DHCP SERVER upstream messages
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_DHCP_SERVER_PRIM,
 *      msg:          The message received from CSR AM
 *----------------------------------------------------------------------------*/
void CsrDhcpServerFreeUpstreamMessageContents(CsrUint16 eventClass, void *message);

#ifdef __cplusplus
}
#endif

#endif
