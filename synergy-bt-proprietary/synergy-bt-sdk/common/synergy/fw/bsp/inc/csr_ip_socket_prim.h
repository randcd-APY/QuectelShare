#ifndef CSR_IP_SOCKET_PRIM_H__
#define CSR_IP_SOCKET_PRIM_H__
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_prim_defs.h"
#include "csr_sched.h"
#include "csr_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrIpSocketPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim CsrIpSocketPrim;

typedef CsrUint8 CsrIpSocketProtocol;
#define CSR_IP_SOCKET_PROTOCOL_IP                         ((CsrIpSocketProtocol) 0)
#define CSR_IP_SOCKET_PROTOCOL_ICMP                       ((CsrIpSocketProtocol) 1)
#define CSR_IP_SOCKET_PROTOCOL_TCP                        ((CsrIpSocketProtocol) 6)
#define CSR_IP_SOCKET_PROTOCOL_UDP                        ((CsrIpSocketProtocol) 17)

/* IP socket specific error codes. */
#define CSR_IP_SOCKET_RESULT_NO_MORE_SOCKETS              ((CsrResult) 0x01)
#define CSR_IP_SOCKET_RESULT_PORT_IN_USE                  ((CsrResult) 0x02)
#define CSR_IP_SOCKET_RESULT_IP6_NOT_SUPPORTED            ((CsrResult) 0x03)

/* Socket protocol family */
typedef CsrUint8 CsrIpSocketFamily;
#define CSR_IP_SOCKET_FAMILY_IP4    4
#define CSR_IP_SOCKET_FAMILY_IP6    6


typedef CsrUint8 CsrIpSocketOption;
#define CSR_IP_SOCKET_OPTION_NAGLE      ((CsrIpSocketOption) (1 << 0))
#define CSR_IP_SOCKET_OPTION_KEEPALIVE  ((CsrIpSocketOption) (1 << 1))
#define CSR_IP_SOCKET_OPTION_BROADCAST  ((CsrIpSocketOption) (1 << 2))
#define CSR_IP_SOCKET_OPTION_DSCP       ((CsrIpSocketOption) (1 << 3))
#define CSR_IP_SOCKET_OPTION_RXWIN      ((CsrIpSocketOption) (1 << 4))
#define CSR_IP_SOCKET_OPTION_TXWIN      ((CsrIpSocketOption) (1 << 5))

/* Mask of CsrIpSocketOption values */
typedef CsrIpSocketOption CsrIpSocketOptionMask;

typedef CsrUint8 CsrIpSocketPriority;
#define CSR_IP_SOCKET_PRIORITY_DEFAULT  ((CsrIpSocketPriority) 0)
#define CSR_IP_SOCKET_PRIORITY_BE_0     ((CsrIpSocketPriority) 0 << 3)
#define CSR_IP_SOCKET_PRIORITY_BK_1     ((CsrIpSocketPriority) 1 << 3)
#define CSR_IP_SOCKET_PRIORITY_BK_2     ((CsrIpSocketPriority) 2 << 3)
#define CSR_IP_SOCKET_PRIORITY_BE_3     ((CsrIpSocketPriority) 3 << 3)
#define CSR_IP_SOCKET_PRIORITY_VI_4     ((CsrIpSocketPriority) 4 << 3)
#define CSR_IP_SOCKET_PRIORITY_VI_5     ((CsrIpSocketPriority) 5 << 3)
#define CSR_IP_SOCKET_PRIORITY_VO_6     ((CsrIpSocketPriority) 6 << 3)
#define CSR_IP_SOCKET_PRIORITY_VO_7     ((CsrIpSocketPriority) 7 << 3)

/* Downstream */
#define CSR_IP_SOCKET_PRIM_DOWNSTREAM_LOWEST              (0x0000)

#define CSR_IP_SOCKET_DNS_RESOLVE_NAME_REQ                ((CsrIpSocketPrim) (0x0000 + CSR_IP_SOCKET_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_SOCKET_UDP_NEW_REQ                         ((CsrIpSocketPrim) (0x0001 + CSR_IP_SOCKET_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_SOCKET_UDP_BIND_REQ                        ((CsrIpSocketPrim) (0x0002 + CSR_IP_SOCKET_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_SOCKET_UDP_DATA_REQ                        ((CsrIpSocketPrim) (0x0003 + CSR_IP_SOCKET_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_SOCKET_UDP_CLOSE_REQ                       ((CsrIpSocketPrim) (0x0004 + CSR_IP_SOCKET_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_SOCKET_UDP_MULTICAST_SUBSCRIBE_REQ         ((CsrIpSocketPrim) (0x0005 + CSR_IP_SOCKET_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_SOCKET_UDP_MULTICAST_UNSUBSCRIBE_REQ       ((CsrIpSocketPrim) (0x0006 + CSR_IP_SOCKET_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_SOCKET_UDP_MULTICAST_INTERFACE_REQ         ((CsrIpSocketPrim) (0x0007 + CSR_IP_SOCKET_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_SOCKET_TCP_NEW_REQ                         ((CsrIpSocketPrim) (0x0008 + CSR_IP_SOCKET_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_SOCKET_TCP_BIND_REQ                        ((CsrIpSocketPrim) (0x0009 + CSR_IP_SOCKET_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_SOCKET_TCP_LISTEN_REQ                      ((CsrIpSocketPrim) (0x000a + CSR_IP_SOCKET_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_SOCKET_TCP_CONNECT_REQ                     ((CsrIpSocketPrim) (0x000b + CSR_IP_SOCKET_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_SOCKET_TCP_DATA_REQ                        ((CsrIpSocketPrim) (0x000c + CSR_IP_SOCKET_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_SOCKET_TCP_DATA_RES                        ((CsrIpSocketPrim) (0x000d + CSR_IP_SOCKET_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_SOCKET_TCP_CLOSE_REQ                       ((CsrIpSocketPrim) (0x000e + CSR_IP_SOCKET_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_SOCKET_TCP_ABORT_REQ                       ((CsrIpSocketPrim) (0x000f + CSR_IP_SOCKET_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_SOCKET_RAW_NEW_REQ                         ((CsrIpSocketPrim) (0x0010 + CSR_IP_SOCKET_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_SOCKET_RAW_BIND_REQ                        ((CsrIpSocketPrim) (0x0011 + CSR_IP_SOCKET_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_SOCKET_RAW_DATA_REQ                        ((CsrIpSocketPrim) (0x0012 + CSR_IP_SOCKET_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_SOCKET_RAW_CLOSE_REQ                       ((CsrIpSocketPrim) (0x0013 + CSR_IP_SOCKET_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_SOCKET_OPTIONS_REQ                         ((CsrIpSocketPrim) (0x0014 + CSR_IP_SOCKET_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_SOCKET_PRIM_DOWNSTREAM_HIGHEST             (0x0014 + CSR_IP_SOCKET_PRIM_DOWNSTREAM_LOWEST)

/* Upstream */
#define CSR_IP_SOCKET_PRIM_UPSTREAM_LOWEST                (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_IP_SOCKET_DNS_RESOLVE_NAME_CFM                ((CsrIpSocketPrim) (0x0000 + CSR_IP_SOCKET_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_SOCKET_UDP_NEW_CFM                         ((CsrIpSocketPrim) (0x0001 + CSR_IP_SOCKET_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_SOCKET_UDP_BIND_CFM                        ((CsrIpSocketPrim) (0x0002 + CSR_IP_SOCKET_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_SOCKET_UDP_DATA_IND                        ((CsrIpSocketPrim) (0x0003 + CSR_IP_SOCKET_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_SOCKET_UDP_DATA_CFM                        ((CsrIpSocketPrim) (0x0004 + CSR_IP_SOCKET_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_SOCKET_UDP_MULTICAST_SUBSCRIBE_CFM         ((CsrIpSocketPrim) (0x0005 + CSR_IP_SOCKET_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_SOCKET_UDP_MULTICAST_UNSUBSCRIBE_CFM       ((CsrIpSocketPrim) (0x0006 + CSR_IP_SOCKET_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_SOCKET_UDP_MULTICAST_INTERFACE_CFM         ((CsrIpSocketPrim) (0x0007 + CSR_IP_SOCKET_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_SOCKET_TCP_NEW_CFM                         ((CsrIpSocketPrim) (0x0008 + CSR_IP_SOCKET_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_SOCKET_TCP_BIND_CFM                        ((CsrIpSocketPrim) (0x0009 + CSR_IP_SOCKET_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_SOCKET_TCP_LISTEN_CFM                      ((CsrIpSocketPrim) (0x000a + CSR_IP_SOCKET_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_SOCKET_TCP_CONNECT_CFM                     ((CsrIpSocketPrim) (0x000b + CSR_IP_SOCKET_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_SOCKET_TCP_ACCEPT_IND                      ((CsrIpSocketPrim) (0x000c + CSR_IP_SOCKET_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_SOCKET_TCP_DATA_CFM                        ((CsrIpSocketPrim) (0x000d + CSR_IP_SOCKET_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_SOCKET_TCP_DATA_IND                        ((CsrIpSocketPrim) (0x000e + CSR_IP_SOCKET_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_SOCKET_TCP_CLOSE_IND                       ((CsrIpSocketPrim) (0x000f + CSR_IP_SOCKET_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_SOCKET_RAW_NEW_CFM                         ((CsrIpSocketPrim) (0x0010 + CSR_IP_SOCKET_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_SOCKET_RAW_BIND_CFM                        ((CsrIpSocketPrim) (0x0011 + CSR_IP_SOCKET_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_SOCKET_RAW_DATA_IND                        ((CsrIpSocketPrim) (0x0012 + CSR_IP_SOCKET_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_SOCKET_RAW_DATA_CFM                        ((CsrIpSocketPrim) (0x0013 + CSR_IP_SOCKET_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_SOCKET_OPTIONS_CFM                         ((CsrIpSocketPrim) (0x0014 + CSR_IP_SOCKET_PRIM_UPSTREAM_LOWEST))

#define CSR_IP_SOCKET_PRIM_UPSTREAM_HIGHEST               (0x0014 + CSR_IP_SOCKET_PRIM_UPSTREAM_LOWEST)

#define CSR_IP_SOCKET_PRIM_DOWNSTREAM_COUNT               (CSR_IP_SOCKET_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_IP_SOCKET_PRIM_DOWNSTREAM_LOWEST)
#define CSR_IP_SOCKET_PRIM_UPSTREAM_COUNT                 (CSR_IP_SOCKET_PRIM_UPSTREAM_HIGHEST + 1 - CSR_IP_SOCKET_PRIM_UPSTREAM_LOWEST)

typedef struct
{
    CsrIpSocketPrim   type;
    CsrSchedQid       appHandle;
    CsrCharString    *name;
    CsrIpSocketFamily socketFamilyMax;
} CsrIpSocketDnsResolveNameReq;

typedef struct
{
    CsrIpSocketPrim   type;
    CsrCharString    *name;
    CsrIpSocketFamily socketFamily;
    CsrUint8          ipAddress[16];
    CsrResult         result;
} CsrIpSocketDnsResolveNameCfm;

typedef struct
{
    CsrIpSocketPrim   type;
    CsrSchedQid       appHandle;
    CsrIpSocketFamily socketFamily;
} CsrIpSocketUdpNewReq;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
    CsrResult       result;
} CsrIpSocketUdpNewCfm;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
    CsrUint8        ipAddress[16];
    CsrUint16       port;
} CsrIpSocketUdpBindReq;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
    CsrResult       result;
    CsrUint16       port;
} CsrIpSocketUdpBindCfm;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
    CsrUint8        ipAddress[16];
    CsrUint16       port;
    CsrUint16       dataLength;
    CsrUint8       *data;
} CsrIpSocketUdpDataReq;

typedef struct
{
    CsrIpSocketPrim   type;
    CsrUint16         socketHandle;
    CsrIpSocketFamily socketFamily;
    CsrUint8          ipAddress[16];
    CsrUint16         port;
    CsrUint16         dataLength;
    CsrUint8         *data;
} CsrIpSocketUdpDataInd;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
} CsrIpSocketUdpCloseReq;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
} CsrIpSocketUdpDataCfm;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
    CsrUint8        interfaceIp[16];
    CsrUint8        group[16];
} CsrIpSocketUdpMulticastSubscribeReq;

typedef struct
{
    CsrIpSocketPrim   type;
    CsrUint16         socketHandle;
    CsrIpSocketFamily socketFamily;
    CsrUint8          interfaceIp[16];
    CsrUint8          group[16];
    CsrResult         result;
} CsrIpSocketUdpMulticastSubscribeCfm;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
    CsrUint8        interfaceIp[16];
    CsrUint8        group[16];
} CsrIpSocketUdpMulticastUnsubscribeReq;

typedef struct
{
    CsrIpSocketPrim   type;
    CsrUint16         socketHandle;
    CsrIpSocketFamily socketFamily;
    CsrUint8          interfaceIp[16];
    CsrUint8          group[16];
    CsrResult         result;
} CsrIpSocketUdpMulticastUnsubscribeCfm;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
    CsrUint8        interfaceIp[16];
} CsrIpSocketUdpMulticastInterfaceReq;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
    CsrResult       result;
} CsrIpSocketUdpMulticastInterfaceCfm;

typedef struct
{
    CsrIpSocketPrim   type;
    CsrSchedQid       appHandle;
    CsrIpSocketFamily socketFamily;
} CsrIpSocketTcpNewReq;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
    CsrResult       result;
} CsrIpSocketTcpNewCfm;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
    CsrUint8        ipAddress[16];
    CsrUint16       port;
} CsrIpSocketTcpBindReq;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
    CsrResult       result;
    CsrUint16       port;
} CsrIpSocketTcpBindCfm;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
} CsrIpSocketTcpListenReq;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
    CsrResult       result;
} CsrIpSocketTcpListenCfm;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
    CsrUint8        ipAddress[16];
    CsrUint16       port;
} CsrIpSocketTcpConnectReq;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
    CsrResult       result;
} CsrIpSocketTcpConnectCfm;

typedef struct
{
    CsrIpSocketPrim   type;
    CsrUint16         socketHandle;
    CsrIpSocketFamily socketFamily;
    CsrUint8          ipAddress[16];
    CsrUint16         port;
    CsrUint16         listenSocketHandle;
} CsrIpSocketTcpAcceptInd;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
    CsrUint16       dataLength;
    CsrUint8       *data;
} CsrIpSocketTcpDataReq;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
    CsrUint16       dataLength;
    CsrUint8       *data;
} CsrIpSocketTcpDataInd;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
} CsrIpSocketTcpDataRes;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
    CsrResult       result;
} CsrIpSocketTcpDataCfm;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
} CsrIpSocketTcpCloseReq;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
} CsrIpSocketTcpCloseInd;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
} CsrIpSocketTcpAbortReq;

typedef struct
{
    CsrIpSocketPrim     type;
    CsrSchedQid         appHandle;
    CsrIpSocketFamily   socketFamily;
    CsrIpSocketProtocol protocolNumber;
} CsrIpSocketRawNewReq;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
    CsrResult       result;
} CsrIpSocketRawNewCfm;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
    CsrUint8        ipAddress[16];
} CsrIpSocketRawBindReq;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
    CsrResult       result;
} CsrIpSocketRawBindCfm;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
    CsrUint8        ipAddress[16];
    CsrUint16       dataLength;
    CsrUint8       *data;
} CsrIpSocketRawDataReq;

typedef struct
{
    CsrIpSocketPrim   type;
    CsrUint16         socketHandle;
    CsrIpSocketFamily socketFamily;
    CsrUint8          ipAddress[16];
    CsrUint16         dataLength;
    CsrUint8         *data;
} CsrIpSocketRawDataInd;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
} CsrIpSocketRawCloseReq;

typedef struct
{
    CsrIpSocketPrim type;
    CsrUint16       socketHandle;
} CsrIpSocketRawDataCfm;

typedef struct
{
    CsrIpSocketPrim       type;
    CsrUint32             txWindow;
    CsrUint32             rxWindow;
    CsrUint16             socketHandle;
    CsrBool               nagle;
    CsrBool               keepAlive;
    CsrBool               broadcast;
    CsrIpSocketPriority   dscp;
    CsrIpSocketOptionMask validOptions;           /* Mask of CSR_IP_SOCKET_OPTION_ */
} CsrIpSocketOptionsReq;

typedef struct
{
    CsrIpSocketPrim       type;
    CsrUint16             socketHandle;
    CsrUint32             txWindow;
    CsrUint32             rxWindow;
    CsrBool               nagle;
    CsrBool               keepAlive;
    CsrBool               broadcast;
    CsrIpSocketPriority   dscp;
    CsrIpSocketOptionMask validOptions;           /* Mask of CSR_IP_SOCKET_OPTION_ */
} CsrIpSocketOptionsCfm;

#ifdef __cplusplus
}
#endif

#endif /* CSR_IP_SOCKET_PRIM_H__ */
