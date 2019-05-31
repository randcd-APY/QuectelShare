/*
 * Copyright (c) 2014-2015 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

#include "dsrc_sock.h"

#include <sys/ioctl.h>
#include <linux/if.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#ifndef _ANDROID_
size_t
strlcpy(char *dst, const char *src, size_t siz);
#endif

/**
 * Custom ethernet packet type id
 */
#define LOCAL_ETH_P_QCOM_TX         (0x8151)
#define LOCAL_ETH_P_QCOM_RX         (0x8152)

/*******************************************************************************
 * Open a new socket and initialize the given sockaddr buffer.
 *
 * @param interface [in] interface name to use. eg: wlanocb0
 * @param socket [out] on success returns the socket descriptor. Caller must
 *        release this socket with close()
 * @param sockaddr [out] Given struct sockaddr_ll buffer, will be initialized
 *        for subsequent use with bind(), recvfrom() and sendto()
 *
 * @return int returns 0 on success.
 *         -EPERM  socket() creation failed, check errno for further details.
 *         -EAFNOSUPPORT if failing to find the interface
 *         -EACCES if failing to get the mac address of device
 ******************************************************************************/
int dsrc_socket_open(const char* interface, int *sock,
                    struct sockaddr_ll *sockaddr)
{
    int raw_socket;       /* socket */
    int rc = 0;           /* return code */
    struct ifreq if_idx;
    struct ifreq if_mac;

    /* Open a raw socket */
    raw_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (raw_socket < 0) {
        rc = -EPERM;
        goto bail;
    }

    /* Get interface index */
    memset(&if_idx, 0, sizeof(if_idx));
    strlcpy(if_idx.ifr_name, interface, IFNAMSIZ-1);
    if (ioctl(raw_socket, SIOCGIFINDEX, &if_idx) < 0) {
        rc = -EAFNOSUPPORT;
        goto bail;
    }

    /* Get source MAC Address */
    memset(&if_mac, 0, sizeof(if_mac));
    strlcpy(if_mac.ifr_name, interface, IFNAMSIZ-1);
    if (ioctl(raw_socket, SIOCGIFHWADDR, &if_mac) < 0) {
        rc = -EACCES;
        goto bail;
    }

    /* Populate sockaddr */
    memset(sockaddr, 0, sizeof(struct sockaddr_ll));
    sockaddr->sll_family = AF_PACKET;
    sockaddr->sll_protocol = htons(ETH_P_ALL);
    sockaddr->sll_ifindex = if_idx.ifr_ifindex;
    sockaddr->sll_hatype = 0; /* ARPHRD_IEEE80211 */
    sockaddr->sll_pkttype = PACKET_BROADCAST; /* PACKET_BROADCAST */
    sockaddr->sll_halen = IFHWADDRLEN;
    sockaddr->sll_addr[0] = if_mac.ifr_hwaddr.sa_data[0];
    sockaddr->sll_addr[1] = if_mac.ifr_hwaddr.sa_data[1];
    sockaddr->sll_addr[2] = if_mac.ifr_hwaddr.sa_data[2];
    sockaddr->sll_addr[3] = if_mac.ifr_hwaddr.sa_data[3];
    sockaddr->sll_addr[4] = if_mac.ifr_hwaddr.sa_data[4];
    sockaddr->sll_addr[5] = if_mac.ifr_hwaddr.sa_data[5];

  bail:

    if (rc < 0 && 0 <= raw_socket) {  /* error occured closeif(raw_socket)*/
        close(raw_socket); raw_socket = -1;
    }

    *sock = raw_socket;

    return rc;
}

/*******************************************************************************
 * Receive a full ethernet packet. Filter the local echoed broadcast packets.
 *
 * @param sock : raw socket
 * @param buf : a buffer
 * @param len : of atleast ETH_FRAME_LEN
 * @param flags
 *
 * @return ssize_t
 ******************************************************************************/
static __inline
ssize_t dsrc_recv_eth_frame(int sock, uint8_t *buf, size_t len, int flags)
{
    struct sockaddr_ll src_addr;
    socklen_t addrlen;
    int rc = 0;

    assert(ETH_FRAME_LEN <= len); /* ethernet packet */

    while (1) {
        addrlen = sizeof(src_addr);

        rc = recvfrom(sock, buf, len, flags, (struct sockaddr*)&src_addr,
                      &addrlen);

        if (rc < 0) { /* we must receive at least the full struct ethhdr */
            printf("Error receiving message!\n");
            goto bail;
        }

        if (rc < (int)sizeof(struct ethhdr)) { /* we must receive at least the full struct ethhdr */
            printf("Incomplete eth pkt. Len: %d \n", rc);
            rc = -1;
            goto bail;
        }

      /* For raw sockets, an outgoing packet will get echo'd back in, but
         the type of the socket will still be outgoing.  */
        if (src_addr.sll_pkttype == PACKET_OUTGOING) {
            /* This is an outgoing packet */
            continue;
        }
        printf("A frame received. Iface:%d type:%d Len = %d\n",
               src_addr.sll_ifindex, src_addr.sll_pkttype, rc);
        break;
    }

  bail:
    return rc;
}

/*******************************************************************************
 * recieve an ethernet frame and optionally provide the per packet
 * header if present.
 *
 * @param sock : socket to recieve from
 * @param buf : a buffer for collecting the frame.
 * @param len : length of the buffer, it must atleast be 0x600
 * @param flags :
 * @param rx_stats : pointer to rx stats in the buf
 * @param frame : pointer to 802.3 frame in the buf
 * @param frame_len : length of the frame.
 *
 * @return ssize_t -ve value on error. on success total bytes recieved in the buf
 ******************************************************************************/
ssize_t dsrc_socket_recv_eth_frame_and_stats(int sock, uint8_t *buf, size_t len,
    int flags, struct dsrc_ocb_rx_stats_hdr** rx_stats,
    uint8_t** frame, ssize_t* frame_len)
{
    ssize_t bytes_recd = dsrc_recv_eth_frame(sock, buf, len, flags);
    ssize_t num_bytes  = bytes_recd;

    /* assume none to begin with */
    *rx_stats  = 0;
    *frame     = 0;
    *frame_len = 0;

    if (0 < bytes_recd) {
        struct ethhdr *eh = (struct ethhdr *)buf;

        /* Check the header to see if there's an RX stats header indicated by
           an ether type of 0x8152. */
        if (ntohs(eh->h_proto) == LOCAL_ETH_P_QCOM_RX) {
            int stats_len = 0;

            buf += sizeof(struct ethhdr);
            num_bytes -= sizeof(struct ethhdr);

            /* stats */
            *rx_stats = (struct dsrc_ocb_rx_stats_hdr*)buf;
            stats_len = (*rx_stats)->length;

            buf += stats_len;  /* actual eth frame is embedded */
            num_bytes -= stats_len;

            assert ((sizeof(struct ethhdr)
                     + stats_len
                     + sizeof(struct ethhdr)) <= bytes_recd);
        }

        *frame = buf;
        *frame_len = num_bytes;
    }
    return bytes_recd;
}

