/*******************************************************************************
 * Copyright (c) 2015 Qualcomm Atheros, Inc. All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 ******************************************************************************/
#ifndef DSRC_SOCK_H
#define DSRC_SOCK_H

#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h> /* the L2 protocols */
#include <unistd.h>
#include <stdint.h>

/**
 * @file
 * @brief simplified socket interfaces for use with dsrc library.
 */

#include "dsrc_ocb.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Open a new socket and initialize the given sockaddr buffer.
 *
 * @param iface_name [in] interface name to use. eg: wlanocb0
 * @param socket [out] on success returns the socket descriptor. Caller must
 *        release this socket with close()
 * @param sockaddr [out] Given struct sockaddr_ll buffer, will be initialized
 *        for subsequent use with bind() and sendto()
 *
 * @return int returns 0 on success.
 *         -EPERM  socket() creation failed, check errno for further details.
 *         -EAFNOSUPPORT if failing to find the interface
 *         -EACCES if failing to get the mac address of device
 */
int dsrc_socket_open(const char* iface_name, int *socket,
                     struct sockaddr_ll *sockaddr);

/*!
 * @brief close if the socket was open.
 *
 * @param sock_fd
 * @return int
 */
static __inline
int dsrc_socket_closeif(int* sock_fd) {
    int rc = 0;
    int sock = *sock_fd;

    *sock_fd = -1;

    if (0 <= sock) {
        rc = close(sock);
    }
    return rc;
}

/*!
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
 */
ssize_t dsrc_socket_recv_eth_frame_and_stats(int sock, uint8_t *buf, size_t len,
    int flags, struct dsrc_ocb_rx_stats_hdr** rx_stats,
    uint8_t** frame, ssize_t* frame_len);


#ifdef __cplusplus
}
#endif

#endif /* !DSRC_SOCK_H */
