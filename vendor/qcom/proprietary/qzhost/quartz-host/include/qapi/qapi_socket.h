/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$


/**
 * @file qapi_socket.h
 *
 * @addtogroup qapi_networking_socket
 * @{
 *
 * @details The BSD-Socket interface API is a collection of standard functions
 * that allow the application to include Internet communications capabilities.
 * In general, the BSD-Socket interface relies on Client-Server architecture,
 * and uses a Socket object for every operation.
 *
 * The interface supports Both TCP (SOCK_STREAM) and UDP (SOCK_DGRAM), Server
 * mode and Client mode, as well as IPv4 and IPv6 communication.
 * A socket can be configured with specific options (see socket options below).
 * Due to the memory constrained properties of the device, it is mandatory to
 * follow the BSD-Socket programming guidelines, and in particular, check for
 * return values of each function. There is a chance that an operation may
 * fail due to resource limitations. For example, the send function may be
 * able to send only some of the data and not all of it in a single call.
 * A consequent call with the rest of the data is required. In some other
 * cases, an application thread may need to sleep in order to allow the system
 * to clear its queues, process data, and so on.
 *
 * The zero-copy mechanism can be enabled by calling qapi_setsockopt() with the option 'level' being
 * SOL_SOCKET or IPPROTO_IP and 'optname' being SO_CALLBACK (for TCP) or SO_UDPCALLBACK (for UDP). \n
 * For example: \n
 * To enable TCP zero-copy: \n
 * @code  qapi_setsockopt(handle, IPPROTO_IP, SO_CALLBACK,
 *                        (void *)tcp_rx_callback, 0); @endcode \n
 * To enable UDP zero-copy: \n
 * @code  qapi_setsockopt(handle, IPPROTO_IP, SO_UDPCALLBACK,
 *                        (void *)udp_rx_callback, 0); @endcode \n
 * where, 'handle' is the socket handle returned from qapi_socket(). 'tcp_rx_callback' and
 * 'udp_rx_callback' are callback functions called from the stack to inform the application of
 * received data packets. \n
 * The callback functions should conform to the following prototype: \n
 * @code
 *      int32_t (*)(void *so, void *pkt, int32_t errcode) tcp_rx_callback;
 *   int32_t (*)(void *so, void *pkt, int32_t errcode, void *from,
 *               int32_t family) udp_rx_callback; @endcode
 * where \n
 *      'so' is a pointer to an internal socket object \n
 *      'pkt' is a pointer to an system buffer (qapi_Net_Buf_t) containing received data for
 *      the socket if it is not NULL. \n
 *      ((qapi_Net_Buf_t *)pkt)->nb_prot points to the start of the received data, and \n
 *      ((qapi_Net_Buf_t *)pkt)->nb_plen indicates the number of bytes of received data in the buffer. \n
 *      The amount of data in the buffer chain is available in ((qapi_Net_Buf_t *)pkt)->nb_tlen field
 *      of the first buffer in the chain. \n
 *      'errcode' is used by the stack to report some events. If it is not 0, it is a socket error
 *      indicating that an error or other event has occurred on the socket. For TCP, typical nonzero
 *      values will be ESHUTDOWN, indicating that the connected peer has closed its end of the connection
 *      and will send no more data; and ECONNRESET, indicating that the connected peer has abruptly closed
 *      its end of the connection and will neither send nor receive more data. \n
 *      'from' is a pointer to struct sockaddr_in if 'family' is AF_INET or to struct sockaddr_in6
 *      if 'family' is AF_INET6. 'from' indicates the sender of received data.
 *
 * If the callback function returns 0, it indicates that it has accepted responsibility for the system
 * buffer and will return it to the stack (via call to qapi_Net_Buf_Free(pkt, QAPI_NETBUF_SYS)) when it
 * no longer needs the buffer. If the callback function returns any nonzero value, it indicates to the
 * stack that it has not accepted responsibility for the system buffer.
 *
 * @sa qapi_netbuf.h
 *
 * @}
 */

#ifndef _QAPI_SOCKET_H_
#define _QAPI_SOCKET_H_

#include <stdint.h>
#include "qapi/qapi_status.h"   /* qapi_Status_t */
#include "qapi_addr.h"

/** @addtogroup qapi_networking_socket
@{ */

/*
 * Address families (AF) for the "domain" or "family" argument in qapi_socket().
 */
#define AF_UNSPEC       0   /**< Address family is unspecified. */
#define AF_INET         2   /**< Address family is IPv4. */
#define AF_INET6        3   /**< Address family is IPv6. */
#define AF_INET_DUAL46  4   /**< Address family is both IPv4 and IPv6. */
#define AF_PACKET       5   /**< Address family is packet. */

/** Socket stream (TCP). */
#define     SOCK_STREAM    1

/** Socket datagram (UDP). */
#define     SOCK_DGRAM     2

/** Raw socket. */
#define     SOCK_RAW       3

/**
 * @name BSD Socket Error Codes
 * @{ */

#define ENOBUFS        1    /**< No buffer space is available. */
#define ETIMEDOUT      2    /**< Operation timed out. */
#define EISCONN        3    /**< Socket is already connected. */
#define EOPNOTSUPP     4    /**< Operation is not supported. */
#define ECONNABORTED   5    /**< Software caused a connection abort. */
#define EWOULDBLOCK    6	/**< The socket is marked nonblocking and the requested operation would block. */
#define ECONNREFUSED   7    /**< Connection was refused. */
#define ECONNRESET     8    /**< Connection was reset by a peer. */
#define ENOTCONN       9    /**< Socket is not connected. */
#define EBADF          9    /**< An invalid descriptor was specified. */
#define EALREADY       10   /**< Operation is already in progress. */
#define EINVAL         11   /**< Invalid argument was passed. */
#define EMSGSIZE       12   /**< Message is too long. */
#define EPIPE          13   /**< Local end has been shut down on a connection-oriented socket. */
#define EDESTADDRREQ   14   /**< Destination address is required. */
#define ESHUTDOWN      15   /**< Cannot send after a socket shutdown. */
#define ENOPROTOOPT    16   /**< Protocol is not available. */
#define EHAVEOOB       17   /**< Out of band. */
#define ENOMEM         18   /**< No memory is available. */
#define EADDRNOTAVAIL  19   /**< Cannot assign the requested address. */
#define EADDRINUSE     20   /**< Address is already in use. */
#define EAFNOSUPPORT   21   /**< Address family is not supported by the protocol family. */
#define EINPROGRESS    22   /**< Operation is now in progress. */
#define ELOWER         23   /**< Lower layer (IP) error. */
#define ENOTSOCK       24   /**< Socket operation is on a nonsocket. */
#define EIEIO          27   /**< I/O error. */
#define ETOOMANYREFS   28   /**< Too many references. */
#define EFAULT         29   /**< Bad address. */
#define ENETUNREACH    30   /**< Network is unreachable. */
/** @} */ /* end namegroup */

/**
 * @name Socket Options
 * @{
 */

#define     SOL_SOCKET     -1          /**< For use with [gs]etsockopt() at SOL_SOCKET 'level'.*/
#define     SO_DEBUG       0x00001     /**< Turn on debugging information recording. */
#define     SO_ACCEPTCONN  0x00002     /**< Socket has had listen(). */
#define     SO_REUSEADDR   0x00004     /**< Allow local address reuse. */
#define     SO_KEEPALIVE   0x00008     /**< Keep the connections alive. */
#define     SO_DONTROUTE   0x00010     /**< Not used. */
#define     SO_BROADCAST   0x00020     /**< Not used. */
#define     SO_USELOOPBACK 0x00040     /**< Not used. */
#define     SO_LINGER      0x00080     /**< Linger on close if data is present. */
#define     SO_OOBINLINE   0x00100     /**< Leave received OOB data in line. */
#define     SO_TCPSACK     0x00200     /**< Allow TCP SACK (selective acknowledgment). */
#define     SO_WINSCALE    0x00400     /**< Set the scaling window option. */
#define     SO_TIMESTAMP   0x00800     /**< Set the TCP timestamp option. */
#define     SO_BIGCWND     0x01000     /**< Large initial TCP congenstion window. */
#define     SO_HDRINCL     0x02000     /**< User access to the IP header for SOCK_RAW. */
#define     SO_NOSLOWSTART 0x04000     /**< Suppress slow start on this socket. */
#define     SO_FULLMSS     0x08000     /**< Not used. */
#define     SO_SNDBUF      0x1001      /**< Send buffer size. */
#define     SO_RCVBUF      0x1002      /**< Receive buffer size. */
#define     SO_SNDLOWAT    0x1003      /**< Not used. */
#define     SO_RCVLOWAT    0x1004      /**< Not used. */
#define     SO_SNDTIMEO    0x1005      /**< Send timeout. */
#define     SO_RCVTIMEO    0x1006      /**< Receive timeout. */
#define     SO_ERROR       0x1007      /**< Get error status and clear. */
#define     SO_TYPE        0x1008      /**< Get socket type. */
#define     SO_HOPCNT      0x1009      /**< Not used. */
#define     SO_MAXMSG      0x1010      /**< TCP max segment size (MSS). */
#define     SO_RXDATA      0x1011      /**< Get a count of the bytes in sb_rcv. */
#define     SO_TXDATA      0x1012      /**< Get a count of the bytes in sb_snd. */
#define     SO_MYADDR      0x1013      /**< Return my IP address. */
#define     SO_NBIO        0x1014      /**< Set socket to Nonblocking mode. */
#define     SO_BIO         0x1015      /**< Set socket to Blocking mode. */
#define     SO_NONBLOCK    0x1016      /**< Set/get the blocking mode via the optval parameter. */
#define     SO_CALLBACK    0x1017      /**< Set/get the TCP zero_copy callback routine. */
#define     SO_UDPCALLBACK 0x1019      /**< Set/get the UDP zero_copy callback routine. */

#define     IPPROTO_TCP         6      /**< For use with [gs]etsockopt() at IPPROTO_TCP 'level'. */
#define     TCP_NODELAY         0x2004 /**< Disable Nagle Algorithm */

#define     IPPROTO_IP          0      /**< For use with [gs]etsockopt() at IPPROTO_IP 'level'. */
#define     IP_HDRINCL          2      /**< IP header is included with the data. */
#define     IP_ADD_MEMBERSHIP   12     /**< Add an IPv4 group membership. */
#define     IP_DROP_MEMBERSHIP  13     /**< Drop an IPv4 group membership. */
#define     IPV6_MULTICAST_IF   80     /**< Set the egress interface for multicast traffic. */
#define     IPV6_MULTICAST_HOPS 81     /**< Set the number of hops. */
#define     IPV6_MULTICAST_LOOP 82     /**< Enable/disable loopback for multicast. */
#define     IPV6_JOIN_GROUP     83     /**< Join an IPv6 MC group. */
#define     IPV6_LEAVE_GROUP    84     /**< Leave an IPv6 MC group. */

#define     IP_OPTIONS          1      /**< For use with [gs]etsockopt() at IP_OPTIONS 'level'. */
#define     IP_TOS              3      /**< IPv4 type of service and precedence. */
#define     IP_TTL_OPT          4      /**< IPv4 time to live. */
#define     IPV6_SCOPEID        14     /**< IPv6 IF scope ID. */
#define     IPV6_UNICAST_HOPS   15     /**< IPv6 hop limit. */
#define     IPV6_TCLASS         16     /**< IPv6 traffic class. */
/** @} */ /* end namegroup */

/**
 * @name Flags for recv() and send()
 * @{
 */
#define     MSG_OOB             0x1      /**< Send/receive out-of-band data. */
#define     MSG_PEEK            0x2      /**< Peek at an incoming message. */
#define     MSG_DONTROUTE       0x4      /**< Send without using routing tables. */
#define     MSG_DONTWAIT        0x20     /**< Send/receive is nonblocking. */
#define     MSG_ZEROCOPYSEND    0x1000   /**< Send with zero-copy. */
/** @} */ /* end namegroup */

/** Infinite time for the "timeout_ms" argument in qapi_select(). */
#define     QAPI_NET_WAIT_FOREVER   (0xFFFFFFFF)

/* loopback behavior (disabled or enabled) for multicast packets */
#define IPV6_MC_LPBK_DIS 0    /**< Disable loopback behavior for multicast packets. */
#define IPV6_MC_LPBK_EN 1     /**< Enable loopback behavior for multicast packets. */

/**
 * @brief TCP zero-copy application RX callback handler.
 */
typedef int32_t (*qapi_Net_TCP_Zerocopy_CB_t)(
	void *so,
	/**< Socket handle. */

	void *packet,
	/**< Pointer to a network buffer (packet). */

	int32_t errcode
	/**< BSD socket error code. */
	);

/**
 * @brief UDP zero-copy application RX callback handler
 */
typedef int32_t (*qapi_Net_UDP_Zerocopy_CB_t)(
	void *so,
	/**< Socket handle. */

	void *packet,
	/**< Pointer to a network buffer (packet). */

	int32_t errcode,
	/**< BSD socket error code. */

	void *from,
	/**< Sender address. */

	int32_t family
	/**< Family: AF_INET or AF_INET6. */
	);

/**
 * @brief Ethernet frame header.
 */
#define ETH_ALEN        6   /**< Size (in bytes) of Ethernet link-layer address. */
struct ethhdr {
    uint8_t h_dest[ETH_ALEN];
	/**< Destination address. */

    uint8_t h_source[ETH_ALEN];
	/**< Source address. */

    uint16_t h_proto;
	/**< Protocol id. */

} __attribute__((packed));

/**
 * @brief Device-independent link-layer address.
 */
struct sockaddr_ll {
	uint16_t	sll_family;
	/**< Always AF_PACKET. */

	uint16_t	sll_protocol;
	/**< Link-layer protocol id in network byte order, e.g. htons(0x888e) for EAPOL. */

	int		    sll_ifindex;
	/**< ifIndex in rfc1213-mib2 (1-based). */

	uint16_t    sll_hatype;
	/**< ARP hardware type. */

	uint8_t	    sll_pkttype;
	/**< Packet type. */

	uint8_t	    sll_halen;
	/**< Size (in bytes) of link-layer address. */

	uint8_t	    sll_addr[8];
	/**< Link-layer address. */
};

/**
 * @brief BSD style socket IPv4 Internet address.
 */
struct sockaddr_in
{
   uint16_t         sin_family;
   /**< AF_INET. */

   uint16_t         sin_port;
   /**< UDP/TCP port number in network order. */

   struct in_addr   sin_addr;
   /**< IPv4 address in network order. */

   uint8_t          sin_zero[8];
   /**< Reserved: must be zero. */
};

/**
 * @brief Argument structure for IP_ADD_MEMBERSHIP and IP_DROP_MEMBERSHIP.
 */
struct ip_mreq
{
   ip_addr imr_multiaddr;
   /**< IP multicast address of the group. */
   ip_addr imr_interface;
   /**< Local IP address of the interface. */
};

/**
 * @brief IPv6 multicast option request.
 */
struct ipv6_mreq {
   struct in6_addr ipv6mr_multiaddr;
   /**< IPv6 multicast address. */
   unsigned int    ipv6mr_interface;
   /**< Interface index. */
};

/**
 * @brief BSD style socket IPv6 Internet address.
 */
struct sockaddr_in6
{
   uint16_t         sin_family;
   /**< AF_INET6. */

   uint16_t         sin_port;
   /**< UDP/TCP port number in network order. */

   uint32_t         sin_flowinfo;
   /**< IPv6 flow information. */

   struct in6_addr  sin_addr;
   /**< IPv6 address. */

   int32_t          sin_scope_id;
   /**< Set of interfaces for a scope. */
};

/**
 * @brief Generic socket Internet address.
 */
struct sockaddr
{
    uint16_t sa_family;
    /**< Address family. */
    uint16_t sa_port;
    /**< Port number in network order. */

    union {
        uint8_t  sa_data[32];

        struct {
            struct in_addr  sin_addr;
            uint8_t         sin_zero[8];
        } sin;

        struct {
            uint32_t        sin_flowinfo;
            struct in6_addr sin_addr;
            int32_t         sin_scope_id;
        } sin6;  /**<  @newpagetable */
    } u;
    /**< Big enough for a 16-byte IPv6 address. */
};

#undef  FD_SETSIZE
#define FD_SETSIZE  16
#undef  fd_set

/**
 * @brief File descriptor sets for qapi_select().
 */
typedef struct qapi_fd_set_s
{
   uint32_t fd_count;
   /**< File descriptor count. */
   uint32_t fd_array[FD_SETSIZE];
   /**< File descriptor array. */
} qapi_fd_set_t;

#define fd_set qapi_fd_set_t

/*
 * Macros to manipulate fd_set
 */
#undef FD_ZERO
#undef FD_CLR
#undef FD_SET
#undef FD_ISSET
#define FD_ZERO(set)            qapi_fd_zero((set))             /**< Clear a set. */
#define FD_CLR(handle, set)     qapi_fd_clr((handle), (set))    /**< Remove a given file descriptor from a set. */
#define FD_SET(handle, set)     qapi_fd_set((handle), (set))    /**< Add a given file descriptor from a set. */
#define FD_ISSET(handle, set)   qapi_fd_isset((handle), (set))  /**< Check to see if a file descriptor is part of the set, after select() returns. */

#ifdef QAPI_NET_ENABLE_BSD_COMPATIBILITY

/**
 * @name Macros to Map BSD APIs to QAPIs
 * @{
 */

#define     socket(domain, type, protocol)      qapi_socket((domain), (type), (protocol))
#define     bind(handle, addr, addrlen)         qapi_bind((handle), (addr), (addrlen))
#define     listen(handle, backlog)             qapi_listen((handle), (backlog))
#define     accept(handle, cliaddr, addrlen)    qapi_accept((handle), (cliaddr), (addrlen))
#define     connect(handle, svraddr, addrlen)   qapi_connect((handle), (svraddr), (addrlen))
#define     setsockopt(handle, level, optname, optval, optlen) \
                                                qapi_setsockopt((handle), (level), (optname), (optval), (optlen))
#define     getsockopt(handle, level, optname, optval, optlen) \
                                                qapi_getsockopt((handle), (level), (optname), (optval), (optlen))
#define     getpeername(handle, addr, addrlen)  qapi_getpeername((handle), (addr), (addrlen))
#define     getsockname(handle, addr, addrlen)  qapi_getsockname((handle), (addr), (addrlen))
#define     recv(handle, buf, len, flags)       qapi_recv((handle), (buf), (len), (flags))
#define     recvfrom(handle, buf, len, flags, from, fromlen) \
                                                qapi_recvfrom((handle), (buf), (len), (flags), (from), (fromlen))
#define     send(handle, buf, len, flags)       qapi_send((handle), (buf), (len), (flags))
#define     sendto(handle, buf, len, flags, to, tolen) \
                                                qapi_sendto((handle), (buf), (len), (flags), (to), (tolen))
#define     close(handle)                       qapi_socketclose((handle))
/** @} */ /* end namegroup */

#endif /* QAPI_NET_ENABLE_BSD_COMPATIBILITY */

/**
 * @brief Creates an endpoint for communication.
 *
 * @param[in] family    Protocol family used for communication. \n
 *                      The supported families are: \n
 *                      AF_INET  -- IPv4 Internet protocols \n
 *                      AF_INET6 -- IPv6 Internet protocols \n
 *                      AF_PACKET -- Packet socket to receive and send Ethernet frames \n
 * @param[in] type      Transport mechanism used for communication. \n
 *                      The supported types are: \n
 *                      SOCK_STREAM -- TCP \n
 *                      SOCK_DGRAM  -- UDP \n
 *                      SOCK_RAW    -- RAW
 * @param[in] protocol  Must be set to 0 if type is SOCK_STREAM or SOCK_DGRAM.
 *                      Must be >= 0x600 if AF_PACKET/SOCK_RAW.
 *                      Must be in between 0 and 0xFF inclusive if AF_INET/SOCK_RAW.
 *
 * @return
 * On success, a handle for the new socket is returned; on error, -1 is returned.
 */
int32_t qapi_socket(int32_t family, int32_t type, int32_t protocol);

/**
 * @brief Assigns an address to the socket created by qapi_socket().
 *
 * @param[in] handle    Socket handle returned from qapi_socket().
 * @param[in] addr      Pointer to an address to be assigned to the socket.
 *                      The actual address structure passed for the addr argument
 *                      will depend on the address family.
 * @param[in] addrlen   Specifies the size, in bytes, of the address pointed to by addr.
 *
 * @return
 * On success, 0 is returned; on error, -1 is returned.
 */
int32_t qapi_bind(int32_t handle, struct sockaddr *addr, int32_t addrlen);

/**
 * @brief Marks the socket as a passive socket.
 *
 * @param[in] handle    Handle (returned from qapi_socket()) that refers to a SOCK_STREAM socket.
 * @param[in] backlog   Define the maximum length to which the queue of pending connections for a handle may grow.
 *
 * @return
 * On success, 0 is returned; on error, -1 is returned.
 */
int32_t qapi_listen(int32_t handle, int32_t backlog);

/**
 * @brief Accepts a connection request from the peer on a SOCK_STREAM socket.
 *
 * @details The accept() function is used with a SOCK_STREAM socket.
 *          It extracts the first connection request on the queue of pending connections
 *          for the listening socket (i.e., handle), creates a new connected socket, and returns
 *          a new socket handle referring to that socket. The newly created socket is in the
 *          Established state. The original socket (i.e., handle) is unaffected by this call.
 *          If no pending connections are present on the queue and the socket is not marked
 *          as nonblocking, qapi_accept() blocks the caller until a connection is present.
 *          If the socket is marked nonblocking and no pending connections are present on the queue,
 *          qapi_accept() fails with the errno EAGAIN or EWOULDBLOCK.
 *
 * @param[in] handle    Socket handle that has been created with qapi_socket(), bound to a
 *                      local address with qapi_bind(), and is listening for connections after a
 *                      qapi_listen().
 * @param[in] cliaddr   Pointer to a sockaddr structure. This structure is filled in with the address
 *                      of the peer socket. The exact format of the address returned (i.e., *cliaddr) is
 *                      determined by the socket's address family. When cliaddr is NULL, nothing is
 *                      filled in; in this case, addrlen should also be NULL.
 * @param[in] addrlen   A value-result argument: the caller must initialize it to contain the size
 *                      (in bytes) of the structure pointed to by cliaddr; on return it will contain
 *                      the actual size of the peer address.
 *
 * @return
 * On success, the call return a positive integer that is a handle for the accepted socket. \n
 *         On error, -1 is returned.
 */
int32_t qapi_accept(int32_t handle, struct sockaddr *cliaddr, int32_t *addrlen)  ;

/**
 * @brief Initiates a connection on a socket.
 *
 * @details If the socket is of type SOCK_DGRAM, *svraddr is the address to which datagrams are
 *          sent by default, and the only address from which datagrams are received. If the socket
 *          is of type SOCK_STREAM, this call attempts to make a connection to the socket that is
 *          bound to the address specified by *srvaddr.
 *
 * @param[in] handle    Socket handle returned from qapi_socket().
 * @param[in] srvaddr   Pointer to the peer's address to which the socket is connected.
 * @param[in] addrlen   Specify the size (in bytes) of *srvaddr.
 *
 * @return
 * On success, 0 is returned; on error, -1 is returned.
 */
int32_t qapi_connect(int32_t handle, struct sockaddr *srvaddr, int32_t addrlen);

/**
 * @brief Sets options on a socket.
 *
 * @param[in] handle    Socket handle returned from qapi_socket().
 * @param[in] level     Protocol level at which the option exists.
 * @param[in] optname   Name of the option.
 * @param[in] optval    Pointer to the option value to be set.
 * @param[in] optlen    Option length in bytes.
 *
 * @return
 * On success, 0 is returned; on error, -1 is returned.
 */
int32_t qapi_setsockopt(int32_t handle, int32_t level, int32_t optname, void *optval, int32_t optlen);

/**
 * @brief Gets options on a socket.
 *
 * @param[in] handle    Socket handle returned from qapi_socket().
 * @param[in] level     Protocol level at which the option exists.
 * @param[in] optname   Name of the option.
 * @param[in] optval    Pointer to a buffer in which the value for the requested option
 *                      is to be returned.
 * @param[in] optlen    This parameter is ignored.
 *
 * @return
 * On success, 0 is returned; on error, -1 is returned.
 */
int32_t qapi_getsockopt(int32_t handle, int32_t level, int32_t optname, void *optval, int32_t *optlen);

/**
 * @brief Gets a peer's socket address.
 *
 * @param[in] handle    Socket handle returned from qapi_socket().
 * @param[in] addr      Pointer to a sockaddr structure. This structure is filled in with the address
 *                      of the peer socket. The exact format of the address returned (i.e., *addr) is
 *                      determined by the socket's address family.
 * @param[in] addrlen   A value-result argument: the caller must initialize it to contain the size
 *                      (in bytes) of the structure pointed to by addr; on return it will contain
 *                      the actual size of the peer's socket address.
 *
 * @return
 * On success, 0 is returned; on error, -1 is returned.
 */
int32_t qapi_getpeername(int32_t handle, struct sockaddr *addr, int32_t *addrlen);

/**
 * @brief Gets the socket address to which the socket is bound.
 *
 * @param[in] handle    Socket handle returned from qapi_socket().
 * @param[in] addr      Pointer to a sockaddr structure. On return, this structure is filled in with
 *                      the current address of the socket. The exact format of the address returned
 *                      (i.e., *addr) is determined by the socket's address family.
 * @param[in] addrlen   A value-result argument: the caller must initialize it to contain the size
 *                      (in bytes) of the structure pointed to by addr; on return it will contain
 *                      the actual size of the socket address.
 *
 * @return
 * On success, 0 is returned; on error, -1 is returned.
 */
int32_t qapi_getsockname(int32_t handle, struct sockaddr *addr, int32_t *addrlen);

/**
 * @brief Closes a socke.t
 *
 * @param[in] handle    Socket handle returned from qapi_socket().
 *
 * @return
 * On success, 0 is returned; on error, -1 is returned.
 */
int32_t qapi_socketclose(int32_t handle);

/**
 * @brief Gets the last error code on a socket.
 *
 * @param[in] handle    Socket handle returned from qapi_socket().
 *
 * @return
 * Socket error code or ENOTSOCK if socket is not found.
 */
int32_t qapi_errno(int32_t handle);

/**
 * @brief Receives a message from a socket.
 *
 * @param[in] handle    Socket handle returned from qapi_socket().
 * @param[in] buf       Pointer to a buffer for the received message.
 * @param[in] len       Number of bytes to receive.
 * @param[in] flags     0, or is formed by ORing one or more of: \n
 *                      MSG_PEEK -- Causes the receive operation to return data from
 *                      the beginning of the receive queue without removing that data
 *                      from the queue. Thus, a subsequent receive call will return
 *                      the same data. \n
 *                      MSG_OOB -- Request receipt of out-of-band data that would not be
 *                      received in the normal data stream. \n
 *                      MSG_DONTWAIT -- Enable nonblocking operation; if the operation would
 *                      block, the call fails with the error EAGAIN or EWOULDBLOCK.
 * @param[in] from      If not NULL, and the underlying protocol provides the source address,
 *                      this source address is filled in. When NULL, nothing is filled in;
 *                      in this case, fromlen is not used, and should also be NULL.
 * @param[in] fromlen   A value-result argument, which the caller should initialize before
 *                      the call to the size of the buffer associated with from, and modified on
 *                      return to indicate the actual size of the source address.
 *
 * @return
 * The number of bytes received, or -1 if an error occurred.
 */
int32_t qapi_recvfrom(int32_t handle, char *buf, int32_t len, int32_t flags, struct sockaddr *from, int32_t *fromlen);

/**
 * @brief Receives a message from a socket.
 *
 * @details The qapi_recv() call is normally used only on a connected socket
 *          and is identical to qapi_recvfrom(handle, buf, len, flags, NULL, NULL).
 *          If the socket is in Zero-Copy mode (SO_CALLBACK has been used to set
 *          a receive handler), this function must not be used in the
 *          application. Received packets will be delivered directly to the
 *          registered receive handler and will be of the type #qapi_Net_Buf_t.
 *
 * @param[in] handle    Socket handle returned from qapi_socket().
 * @param[in] buf       Pointer to a buffer for the received message.
 * @param[in] len       Number of bytes to receive.
 * @param[in] flags     0, or is formed by ORing one or more of: \n
 *                      MSG_PEEK -- Cause the receive operation to return data from
 *                      the beginning of the receive queue without removing that data
 *                      from the queue. Thus, a subsequent receive call will return
 *                      the same data. \n
 *                      MSG_OOB -- Request receipt of out-of-band data that would not be
 *                      received in the normal data stream. \n
 *                      MSG_DONTWAIT -- Enable nonblocking operation; if the operation would
 *                      block, the call fails with the error EAGAIN or EWOULDBLOCK.
 *
 * @return
 * The number of bytes received, or -1 if an error occurred.
 */
int32_t qapi_recv(int32_t handle, char *buf, int32_t len, int32_t flags);

/**
 * @brief Sends a message on a socket to a target.
 *
 * @param[in] handle    Socket handle returned from qapi_socket().
 * @param[in] buf       Pointer to a buffer containing the message to be sent.
 * @param[in] len       Number of bytes to send.
 * @param[in] flags     0, or is formed by ORing one or more of: \n
 *                      MSG_OOB -- Send out-of-band data on sockets that support this
 *                      notion (e.g., of type SOCK_STREAM); the underlying protocol
 *                      must also support out-of-band data. \n
 *                      MSG_DONTWAIT -- Enable nonblocking operation; if the operation would
 *                      block, the call fails with the error EAGAIN or EWOULDBLOCK. \n
 *                      MSG_DONTROUTE -- Do not use a gateway to send the packet; only send to
 *                      hosts on directly connected networks. This is usually used only by
 *                      diagnostic or routing programs.
 * @param[in] to        Pointer to the address of the target.
 * @param[in] tolen     Size in bytes of the target address.
 *
 * @return
 * The number of bytes sent, or -1 if an error occurred and errno is set appropriately.
 */
int32_t qapi_sendto(int32_t handle, char *buf, int32_t len, int32_t flags, struct sockaddr *to, int32_t tolen);

/**
 * @brief Sends a message on a socket.
 *
 * @details This call may be used only when the socket is in a connected state
 *          (so that the intended recipient is known). It is equivalent to
 *          qapi_sendto(handle, buf, len, flags, NULL, 0).
 *
 * @param[in] handle    Socket handle returned from qapi_socket().
 * @param[in] buf       Pointer to a buffer containing the message to be sent.
 * @param[in] len       Number of bytes to send.
 * @param[in] flags     0, or is formed by ORing one or more of: \n
 *                      MSG_OOB -- Send out-of-band data on sockets that support this
 *                      notion (e.g., of type SOCK_STREAM); the underlying protocol
 *                      must also support out-of-band data. \n
 *                      MSG_DONTWAIT -- Enable nonblocking operation; if the operation would
 *                      block, the call fails with the error EAGAIN or EWOULDBLOCK. \n
 *                      MSG_DONTROUTE -- Do not use a gateway to send the packet, only send to
 *                      hosts on directly connected networks. This is usually used only by
 *                      diagnostic or routing programs.
 *
 * @return
 * The number of bytes sent, or -1 if an error occurred and errno is set appropriately.
 */
int32_t qapi_send(int32_t handle, char *buf, int32_t len, int32_t flags);

/**
 * @brief Monitors multiple socket handles, waiting until one or more of them become ready
 *        for some class of I/O operation (e.g., read, write, etc.).
 *
 * @details This call causes the calling process to block waiting for activity on any of a
 *          list of sockets. Arrays of socket handles are passed for read, write,
 *          and exception events. A timeout in milliseconds is also passed.
 *          The call only supports a read socket set, so "wr" and "ex" must be set to NULL.
 *
 * @param[in] rd            Pointer to a list of read socket handles,
 * @param[in] wr            Pointer to a list of write socket handles. Must be NULL.
 * @param[in] ex            Pointer to a list of exception socket handles Must be NULL.
 * @param[in] timeout_ms    Timeout values in milliseconds,
 *
 * @return
 * The number of sockets that had an event occur and become ready.
 */
int32_t qapi_select(qapi_fd_set_t *rd, qapi_fd_set_t *wr, qapi_fd_set_t *ex, int32_t timeout_ms);

/**
 * @brief Initializes a socket set to zero.
 *
 * @param[in] set   Pointer to a list of sockets.
 *
 * @return
 * On success, 0 is returned; on error, -1 is returned.
 */
int32_t qapi_fd_zero(qapi_fd_set_t *set);

/**
 * @brief Removes a socket from the socket set.
 *
 * @param[in] handle    Socket handle returned from qapi_socket().
 * @param[in] set       Pointer to a list of sockets.
 *
 * @return
 * On success, 0 is returned; on error, -1 is returned.
 */
int32_t qapi_fd_clr(int32_t handle, qapi_fd_set_t *set);

/**
 * @brief Adds a socket to the socket set.
 *
 * @param[in] handle    Socket handle returned from qapi_socket().
 * @param[in] set       Pointer to a list of sockets.
 *
 * @return
 * On success, 0 is returned; on error, -1 is returned.
 */
int32_t qapi_fd_set(int32_t handle, qapi_fd_set_t *set);

/**
 * @brief Checks whether the socket is a member of a socket set.
 *
 * @param[in] handle    Socket handle returned from qapi_socket().
 * @param[in] set       Pointer to a list of sockets.
 *
 * @return
 * On success, 0 is returned if the socket is not a member. \n
 * 1 is returned if the socket is a member. \n
 * On error, -1 is returned.
 */
int32_t qapi_fd_isset(int32_t handle, qapi_fd_set_t *set);

/**
 * @brief Retrieve infomation of stored socket.
 *
 * @param[out] handle         Socket handle.
 * @param[out] type           Transport mechanism used for communication.
 *                            The supported types are:
 *                            SOCK_STREAM -- TCP 
 *                            SOCK_DGRAM  -- UDP
 * @param[out] state          Reserved
 * @param[out] local_addr     Pointer to the local address assigned to the socket.
 *                            The actual address structure passed for the addr argument
 *                            will depend on the address family.
 * @param[out] local_addrlen  The size, in bytes, of the address pointed to local_addr.
 * @param[out] remote_addr    Pointer to the remote address assigned to the socket.
 * @param[out] remote_addrlen The size, in bytes, of the address pointed to remote_addr.
 *
 * @return
 * On success, 0 is returned; on error, -1 is returned.
 */
int32_t qapi_socket_retrieve(int *handle, int *type, int *state, struct sockaddr *local_addr, int *local_addrlen, struct sockaddr *remote_addr, int *remote_addrlen);

/** @} */

#endif /* _QAPI_SOCKET_H_ */
