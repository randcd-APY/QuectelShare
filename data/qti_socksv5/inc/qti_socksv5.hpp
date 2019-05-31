#ifndef QTI_SOCKSV5_HPP_
#define QTI_SOCKSV5_HPP_

/*==========================================================================

  FILE:  qti_socksv5.hpp

  SERVICES:

  SOCKSv5 header file for main.

==========================================================================*/

/*==========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

==========================================================================*/

/*==========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        ----------------------------------------------------
  03/20/17   jt         SOCKSv5 support.
==========================================================================*/

#define likely(x) __builtin_expect((x),1)
#define unlikely(x) __builtin_expect((x),0)

#define PROXY_PORT 1080 //port 1080 for socks

#define MAX_LISTEN_CLIENTS 5 //max clients for listen() call

/* For setsockopt */
#define SO_TCP_SPLICE 8345

/* For netlink qc_socksify messages to be passed to tcp splice kernel module */
#define NETLINK_QC_SOCKSIFY 24 //but kernel module must be registered so that 24 is allowed first
#define NETLINK_MSG_LEN 112 //32 + 32 + 16 + 16 + 16

//513 is max sized pkt possible in a SOCKSv5 pkt with the username password auth taking 255 bytes
//each
#define SOCKSV5_MAX_PAYLOAD 513

//for private auth method
#define SOCKSV5_MAX_PAYLOAD_UNAME_ONLY 257

//RFC1928 allows for packet to be up to 1 + 1 + 255 bytes long for the possible methods
#define SOCKSV5_MAX_PAYLOAD_METHODS 257
//RFC1928 allows for packet to be up to 262 bytes long for the possible requests
#define SOCKSV5_MAX_PAYLOAD_REQUESTS 262

#define MAX_SOCKET_BUFFER_SIZE 2097152 //wmem_max and rmem_max divide by 2

#define MAX_NL_PAYLOAD 4096

#define TIME_TO_WAIT_FOR_CRITICAL_THREADS 10

#define MAX_SOCKS_CONN_REQ 8

#define MAX_NUM_EPOLL_EVENTS 2048

#include <iostream>
#include <atomic>
#include <mutex>
#include <utility>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include <asm/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if.h>

//struct relay_session for tcp splice
struct relay_session_tcp_splice
{
  int cli_sock; //cli fd for socket connected to proxy client
  int sock_remote; //remote fd for socket connected to remote server
};

//struct relay_session for socket splice
struct relay_session_socket_splice
{
  int cli_sock; //cli fd for socket connected to proxy client
  int sock_remote; //remote fd for socket connected to remote server
  int cli_to_req_pipe[2]; //pipe used for cli -> req direction for socket splice
  int req_to_cli_pipe[2]; //pipe used for req -> cli direction for socket splice
  bool done;
};

void printUsage(void);
void spawnLANServerThreads(QC_SOCKSv5_Proxy_Configuration* configuration,
                           pthread_t* server_thread_v4, pthread_t* server_thread_v6);
void checkLANIface(QC_SOCKSv5_Proxy_Configuration* configuration, pthread_t* server_thread_v4,
                   pthread_t* server_thread_v6);
void* listenForSocks5Clientv4(void* addr);
void* listenForSocks5Clientv6(void* addr);
void* handleSocks5Client(void* arg);
void cleanupLANThread(void* arg);
void cleanupHandleSocks5Client(void* arg);
void cleanupSocketSplice(void* arg);
void handleSigPipe(int sig);
void* relayClientToReq(void* sock_pair);
void* relayReqToClient(void* sock_pair);
void* monitorConfFile(void* conf_file);
void* monitorLANIface(void* arg);
void notifyTCPSpliceModule(int nl_sock, struct sockaddr_nl* kernel_addr, pid_t pid);

#endif
