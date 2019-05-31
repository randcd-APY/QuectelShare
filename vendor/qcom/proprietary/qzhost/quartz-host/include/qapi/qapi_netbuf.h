/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * 2015-2016 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_netbuf.h
 *
 * @addtogroup qapi_networking_netbuf
 * @{
 *
 * @details This file describes QAPIs for network buffer allocation and deallocation.
 * When an application wants to send data over a communication socket, it first has to allocate
 * a network buffer. There are two types of network buffers:
 * - Local (application) buffers -- These buffers are allocated from the heap using the
 * QAPI_NETBUF_APP flag. These buffers are owned and maintained by the application, and the
 * stack copies the data from it (on TX) and to it (on RX). The application can reuse the buffer
 * once it was sent or once done processing the incoming data.
 * - System buffers -- These buffers are allocated from the internal stack memory using the
 * QAPI_NETBUF_SYS flag. Using this option enables the Zero-Copy mechanism, which may improve
 * performance and reduce power due to skipping the memory copy from the application memory
 * to the stack memory. These buffers are shared between the application and the stack and some
 * restrictions apply: The application does not own the buffer once it was sent and it must
 * allocate a new one in order to transmit more data. Upon receiving a system buffer, the
 * application must free it once done processing. Note that a system buffer can be a list of
 * chained buffers.
 *
 * @code {.c}
 *
 * qapi_Net_Buf_t *my_buf;
 *
 * do {
 *   // Allocate a buffer of 1200 bytes
 *   qapi_Net_Buf_t my_buf = qapi_Net_Buf_Alloc(1200, QAPI_NETBUF_SYS);
 *
 *   if(!my_buf) {
 *   	// No memory, wait and retry
 *   	sleep(1);
 *   	continue;
 *   }
 * }
 * while(0);
 * @endcode
 *
 * @}
 */

#ifndef _QAPI_NETBUF_H_
#define _QAPI_NETBUF_H_

#include "stdint.h"
#include "qapi/qapi_status.h"

/** @addtogroup qapi_networking_netbuf
@{ */

/**
 * @brief Structure to hold nettwork buffer information.
 *
 * @details This structure is used when allocating a system buffer.
 * The network buffer can be chained and accessed via pk_prev and pk_next.
 */
typedef struct qapi_Net_Buf_s
{
   void *reserved;         /**< Reserved. */
   struct qapi_netbuf_s *pk_Prev;  /**< Previous buffer in the chain. */
   struct qapi_netbuf_s *pk_Next;  /**< Next buffer in the chain. */
   char     *nb_Buff;      /**< Beginning of the raw buffer. */
   uint32_t nb_Blen;       /**< Length of the raw buffer. */
   char     *nb_Prot;      /**< Beginning of the protocol/data. */
   uint32_t nb_Tlen;       /**< Total length of all nb_prots in the pk_next list. */
   uint32_t nb_Plen;       /**< Length of the protocol/data. */
} qapi_Net_Buf_t;

/**
 * Maximum size of the queue name.
 */
#define QAPI_NETBUF_MAX_QUEUE_NAME   6

/**
 * Maximum number of free queues.
 */
#define QAPI_NETBUF_MAX_QUEUES       3

/**
 * @brief Structure that contains information about a free queue.
 *
 * @details This structure contains the number of free packet buffers
 * and total number of buffers in a given queue.
 */
typedef struct qapi_Net_Buf_Queue_s
{
   char name[QAPI_NETBUF_MAX_QUEUE_NAME];     /**< Name of the free queue. */
   uint8_t total_Buffers;                     /**< Total number of buffers allocated. */
   uint8_t free_Buffers;                      /**< Number of free buffers available. */
} qapi_Net_Buf_Queue_t;

/**
 * @brief Structure that contains information about all free queues.
 *
 * @details This structure contains the number of free packet buffers
 * total numbers in a each queue and total number of free packet buffers 
 * in the system.
 */
typedef struct qapi_Net_Buf_Queue_Status_s
{ 
   qapi_Net_Buf_Queue_t queue[QAPI_NETBUF_MAX_QUEUES];  /**< Array of free queue objects. */ 
   uint16_t total_Free_Buffers;                         /**< Total number of free buffers available in the system (includes all free queues). */
} qapi_Net_Buf_Queue_Status_t; 

/**
 * UDP head room. MAC+IP+UDP = 24+20+8 = 52.
 */
#define QAPI_NETBUF_UDP_HEADROOM    (52)

/**
 * TCP head room. MAC+IP+TCP = 24+20+20 = 64.
 */
#define QAPI_NETBUF_TCP_HEADROOM    (64)

/**
 * UDP6 head room. 24+40+8 = 72.
 */
#define QAPI_NETBUF_UDP6_HEADROOM   (72)

/**
 * TCP6 head room. 24+40+24(TCP with option) = 88
 */
#define QAPI_NETBUF_TCP6_HEADROOM   (88)

/* For a non-chained netbuf, we can use the following MACROs to update the netbuf */

/**
 * Update the data start pointer on a system buffer.
 */
#define QAPI_NET_BUF_UPDATE_START(pkt, p) \
	(p) = ((qapi_Net_Buf_t *)(pkt))->nb_Prot

/**
 * Update 4 bytes of data in the buffer.
 */
#define QAPI_NET_BUF_UPDATE_INT32(p, val) \
	*(uint32_t *)(p) = (uint32_t)(val); (p) += sizeof(uint32_t)

/**
 * Update 2 bytes of data in the buffer.
 */
#define QAPI_NET_BUF_UPDATE_INT16(p, val) \
	*(uint16_t *)(p) = (uint16_t)(val); (p) += sizeof(uint16_t)

/**
 * Update 1 byte of data in the buffer.
 */
#define QAPI_NET_BUF_UPDATE_INT8(p, val) \
	*(uint8_t *)(p) = (uint8_t)(val); (p) += sizeof(uint8_t)

/**
 * Copy application data into a nonchained buffer.
 */
#define QAPI_NET_BUF_UPDATE_DATA(p, data, len) \
	memcpy((p), (data), (len)); (p) += (len)

/**
 * Update the buffer size on a system buffer.
 */
#define QAPI_NET_BUF_UPDATE_END(pkt, len) \
	((qapi_Net_Buf_t *)(pkt))->nb_Plen = \
    ((qapi_Net_Buf_t *)(pkt))->nb_Tlen = (len)

/* Definitions used for id in calls to qapi_Net_Buf_Alloc() and qapi_Net_Buf_Free() */

/**
 * Allocate a contiguous network buffer from the application heap.
 * If the application does not enable the Zero-copy option for a socket,
 * it should allocate memory using QAPI_NETBUF_APP for sending packets.
 * The memory will be allocated from the heap.
 */
#define QAPI_NETBUF_APP     0x0

/**
 * Allocate a network buffer from the system stack.
 * If the application enables the Zero-copy option for a socket,
 * it must allocate memory using QAPI_NETBUF_SYS for sending packets.
 * The memory will be allocated from the packet buffer pool and might
 * be made of a chain of smaller buffers.
 */
#define QAPI_NETBUF_SYS     0x1

/**
 * Allocate a contiguous network buffer from the system stack.
 * If the application enables the Zero-copy option for a socket,
 * it must allocate memory using QAPI_NETBUF_SYS for sending packets.
 * The memory will be allocated from the packet buffer pool.
 */
#define QAPI_NETBUF_SYS_CONTIG    (0x4 | QAPI_NETBUF_SYS)

/**
 * Allocate a network buffer for a secure connection.
 * If the application enables the Zero-copy option for a socket and uses a secure
 * connection over it, it must allocate memory using one of these two combinations of flags:
 * - (QAPI_NETBUF_APP | QAPI_NETBUF_SSL)
 * - (QAPI_NETBUF_SYS | QAPI_NETBUF_SSL)
 */
#define QAPI_NETBUF_SSL     0x2

/**
 * @brief Allocates a network buffer.
 *
 * @details Call this function to allocate a network buffer.
 * For QAPI_NETBUF_APP requests, the buffer is allocated from the application
 * heap (4-byte aligned plain buffer), and the application owns it (the system
 * will have its own copy).
 *
 * For QAPI_NETBUF_SYS requests, the buffer is allocated from the system
 * network buffer pool and is shared with the networking stack. The buffer is
 * of the type #qapi_Net_Buf_t, and macros and helper functions must be used in
 * order to read or write data. These buffers can be chained and noncontiguous.
 * Additionally, after a successful send, the application cannot use the same
 * buffer and must allocate a new one for additional sends. On receive, once
 * the receive handler is called, the application must free the network buffer
 * before returning.
 *
 * @param[in] size    Size, in bytes, of the buffer.
 * @param[in] id      See QAPI_NETBUF_APP, QAPI_NETBUF_SYS, and QAPI_NETBUF_SSL.
 *
 * @return
 * A pointer to the network buffer, or NULL in case no memory is available.	\n
 * For QAPI_NETBUF_SYS requests, a NULL result is usually not a fatal error;
 * the application can sleep and retry. However, for QAPI_NETBUF_APP requests,
 * this means that there is not enough free heap memory.
 *
 */
void *qapi_Net_Buf_Alloc(uint32_t size, uint32_t id);

/**
 * @brief Frees a network buffer.
 *
 * @details Call this function to free a network buffer.
 *
 * @param[in] buf     Pointer to the buffer to be freed.
 * @param[in] id      ID; see QAPI_NETBUF_APP, QAPI_NETBUF_SYS, and QAPI_NETBUF_SSL.
 *                    The same flag that was used to allocate it must be applied.
 *
 * @return
 * 0 if operation succeeded, -1 otherwise.
 *
 */
int32_t qapi_Net_Buf_Free(void *buf, uint32_t id);

/**
 * @brief Updates data in a network buffer.
 *
 * @details Call this function to seamlessly update data in a network buffer, even in chained ones.
 *
 * @param[in] netbuf  Pointer to the buffer to be updated.
 * @param[in] offset  Data offset.
 * @param[in] srcbuf  Pointer to the data source.
 * @param[in] len     Data length.
 * @param[in] id      ID; ee QAPI_NETBUF_APP, QAPI_NETBUF_SYS and QAPI_NETBUF_SSL.
 *
 * @return
 * 0 if operation succeeded, -1 otherwise. *
 */
int32_t qapi_Net_Buf_Update(void *netbuf, uint32_t offset, void *srcbuf, uint32_t len, uint32_t id);

/**
 * Gets the number of free packet buffers in all queues.
 *
 * @param[in] arg  Pointer to the #qapi_Net_Buf_Queue_Status_t data structure.
 *
 * @return
 * QAPI_OK                -- On success. \n
 * QAPI_ERR_INVALID_PARAM -- Invalid input parameter.
 *
 */
qapi_Status_t qapi_Net_Buf_Free_Queue_Status(qapi_Net_Buf_Queue_Status_t *arg);
/** @} */

#endif /* _QAPI_NETBUF_H_ */
