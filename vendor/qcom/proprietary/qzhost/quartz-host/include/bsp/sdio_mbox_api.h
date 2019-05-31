#ifndef SDIO_MBOX_API_H
#define SDIO_MBOX_API_H_

/*
 * Copyright (c) 2017,2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/*********************************************************************
 *
 * @file  sdio_mbox_api.h
 * @brief mailbox driver api
*/

/*==================================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     -----------------------------------------------------------------
26/07/17   mmtd    Initial version
==================================================================================*/

/*
 * Mailbox hardware API
 *
 *   The API implements a simple driver interface to the mailbox DMA hardware to
 *   faciliate host communications.
 *
 *   By convention APIs with "send" in the names refers to the "TARGET->HOST" direction, while
 *   "recv" denotes the "HOST-TARGET" direction.
 *
 *   The mailbox driver provides an interface for 4 mailbox DMA queues.  The addresses supplied
 *   in the HTC buffer structures must be 4-byte aligned.
 *
 *   The driver implements the interrupt service routine to service each queue and implements
 *   a callback mechanism for the user of this driver to be notified of pending messages (recv) or
 *   buffer completions (send).
 *
 *   The mailbox driver uses the common HTC buffer structures for managing application
 *   buffers that are queued/dequeued onto this driver.
 *
 *      In the send direction, the HTC buffer must have ->actual_length set and the ->buffer
 *      pointing to the message to send.  The mailbox driver does not append headers, it
 *      merely sends the raw buffer.  The caller can chain HTC buffers that comprise a
 *      full message. The mailbox driver pads the length (not the data) of buffers so that
 *      host interface layer block size requirements are met.  This is only applied in the
 *      send direction as DMA fetches in this direction are "harmless".  In the recv direction
 *      it is assumed that the caller supplied buffers that can accommodate block-sized messages
 *      from the host.  The stripping of block padding in the recv direction is left to the user.
 *      Block size padding (send-only) can be set in the driver through an API call (see below).
 *
 *      In the recv direction, the driver sets the ->actual_length field of each HTC buffer
 *      that is filled with a message.  When the caller dequeues a message (see below), each
 *      call to dequeue removes a complete HTC buffer chain that spans a full message.
 *
 *
 *   The caller provides 2 callbacks:
 *
 *   send_done :
 *       A callback when any mailbox send DMA queue has completed buffers.
 *       The caller must dequeue completed buffers until the queues are drained.
 *       This function executes in the context of the mailbox ISR.
 *
 *   recv_message_available :
 *       A callback when any mailbox has a complete message from the host.  The caller
 *       is responsible for dequeuing each message.  This function executes in the context
 *       of the mailbox ISR.
 *
 */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/

#define SDIO_SLAVE_MAX_GPIOS               30


/* typedefs */
typedef void (* MBOX_HOST_INT_HANDLER)(uint32 Instance);
typedef void (* MBOX_IO_COMPLETE_CALLBACK)(uint32 Instance, void *pContext, int mbox);

/* API functions (macros to call APIs are implemented in system infrastructure headers) */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Initialize Mbox data structures
  @function name: mbox_init
  @prototype: void* mbox_init(int InstanceId, int mbox_used,
                                uint8 uDMABufferCnt)
  @input:   InstanceId - Instance indentifier
            mbox_used - MBOX count
            uDMABufferCnt - Total number of DMA buffers reserved for transfers
                            per mbox.
  @output:
  @return:
  @notes: This API initializes internal state structures. DMA engines are not
          initialized and no DMA resources are allocated. But the corresponding
          CPU owned sentinel descriptor as well DMA owned descriptors are
          allocated.
          This API should only be called once at start up.

  @example:
  @see also: mbox_add_dma_resources
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void* mbox_init(int InstanceId, int mbox_used, uint8 uDMABufferCnt);


/* API functions (macros to call APIs are implemented in system infrastructure headers) */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Initialize Mbox data structures
  @function name: mbox_deinit
  @prototype: void mbox_deinit(void)
  @input:   
  @output:
  @return:
  @notes: This API de-initializes internal state structures.

  @example:
  @see also: mbox_add_dma_resources
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void mbox_deinit(int InstanceId, int mbox_used);


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Start Mailbox hardware
  @function name: mbox_hw_start
  @prototype: void mbox_hw_start(void)
  @input:
  @output:
  @return:
  @notes:  Start DMA queues and enable interrupts.  This should only be called
           after initialization and after the caller indicated the number of
           DMA resources required.
  @example:
  @see also: mbox_init, mbox_add_dma_resources
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void mbox_hw_start(int InstanceId, int mbox);


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Stop Mailbox hardware
  @function name: mbox_hw_stop
  @prototype: void mbox_hw_stop(int mbox)
  @input:
  @output:
  @return:
  @notes:  Stops DMA queues and disable interrupts.
  @example:
  @see also: mbox_init, mbox_add_dma_resources
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void mbox_hw_stop(int InstanceId, int mbox);


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Add mailbox DMA resources
  @function name: mbox_add_dma_resources
  @prototype: void mbox_add_dma_resources(int Buffers)
  @input:   Buffers - expected number of buffers to supply resources for
  @output:
  @return:
  @notes: This API should be called after initialization to create DMA resources for
          any HTC buffers that may be queues (send or recv).  The user of this
          module is responsible to allocate DMA resources for each HTC buffer it expects
          to enqueue.
  @example:
  @see also:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void mbox_add_dma_resources(int InstanceId, int mbox, int buffers);

void mbox_destroy_dma_resources(int InstanceId, int mbox);

int mbox_get_avail_dma_desc_cnt(int InstanceId, int mbox);


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Dequeue a list of completed send buffers
  @function name: mbox_send_buflist_dequeue
  @prototype: int mbox_send_buflist_dequeue(int mbox, HTC_BUFFER **pbufinfo)
  @input:  mbox - mailbox number
  @output: pbufinfo - HTC buffer chain of completed buffers
  @return: number of buffers that were retreived
  @notes: This API should be called within the send_done callback to remove
          completed buffers.  The caller should not assume that the buffers
          span a complete message.  Buffers are indicated as they are completed
          the DMA controller.

  @example:
  @see also:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int mbox_send_buflist_dequeue(int InstanceId, int mbox, HTC_BUFFER **pbufinfo);


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Enqueue a list of HTC buffers comprising a send message
  @function name: mbox_send_buflist_enqueue
  @prototype: int mbox_send_buflist_enqueue(int mbox, HTC_BUFFER *bufs)
  @input:   mbox - mailbox number
            bufs - buffer chain describing the message
  @output:
  @return:  number of buffers queued
  @notes:  The caller must set the ->buffer and ->actual_length members for each
           buffer in the chain.  The driver queues each buffer onto the DMA hardware
           and the send_done callback is called when any one buffer is completed.
  @example:
  @see also:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int mbox_send_buflist_enqueue(int InstanceId, int mbox, HTC_BUFFER *bufinfo);



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Dequeue a buffer list that spans a full message
  @function name: mbox_recv_buflist_dequeue
  @prototype: int mbox_recv_buflist_dequeue(int mbox, HTC_BUFFER **pbufinfo)
  @input:   mbox - mailbox
  @output:  pbufinfo - HTC buffer chain that spans the full message
  @return:  number of buffers dequeued
  @notes: The user must call this function within the recv_message_available callback
          to dequeue all completed messages.
  @example:
  @see also:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int mbox_recv_buflist_dequeue(int InstanceId, int mbox, HTC_BUFFER **pbufinfo);
    

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Enqueue a list of HTC buffers for message reception.
  @function name: mbox_recv_buflist_enqueue
  @prototype: int mbox_recv_buflist_enqueue(int mbox, HTC_BUFFER *bufs)
  @input:   mbox - mailbox number
            bufs - linked list of buffers (of the same size)
  @output:
  @return: number of buffers queued
  @notes:
           Each HTC buffer must have the ->buffer member set to the start of the buffer.
           The length of each buffer does not need to be set. The mailbox interface
           assumes that any buffer queued onto a mailbox is equal to the length that
           was set in the call to mbox_set_recv_bufsize().
  @example:
  @see also: mbox_recv_buflist_dequeue, mbox_set_recv_bufsize
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int mbox_recv_buflist_enqueue(int InstanceId, int mbox, HTC_BUFFER *bufinfo);


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Set the size of each receive buffer for a mailbox
  @function name: mbox_set_recv_bufsize
  @prototype: void mbox_set_recv_bufsize(int mbox, int bufsz)
  @input:   mbox - mailbox number
            bufzs - buffer size for each receive buffer
  @output:
  @return:
  @notes: The caller must indicate to the driver the expected size of each
          recv buffer that will be queued.  This is an optimization so that the
          caller is not required to reset the ->actual_length field when recycling
          buffers.  It also keeps the size of the HTC buffer structure small.
  @example:
  @see also:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void mbox_set_recv_bufsize(int InstanceId, int mbox, int bufsz);


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Set block mask for padding buffer transfers (send only)
  @function name: mbox_set_block_mask
  @prototype: void mbox_set_block_mask(int mbox, int blockmask)
  @input:   mbox - the mailbox to set the padding parameter
            blockmask - mask used to determine if the transfer requires padding
  @output:
  @return:
  @notes: The caller must obtain the desired block padding (if any) that the
          host requires.  The block mask is derived from the block size which
          must be a power of 2.  The caller assembles the block mask from the
          block size by subtracting 1.

  @example:
              block_mask = block_size - 1;
              mbox_set_block_mask(0,block_mask);
  @see also:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void mbox_set_block_mask(int InstanceId, int mbox, int mask);


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Pause a mailbox receive queue
  @function name: mbox_pause_recv
  @prototype: void mbox_pause_recv(int mbox)
  @input:   mbox - mailbox number
  @output:
  @return:
  @notes:  Pausing the mailbox masks the interrupt processing for the mailbox.
           It does not, however stop the DMA hardware.  The caller can pause a mailbox
           and later resume the mailbox.  The caller however is responsible for checking
           the DMA queue by calling mbox_recv_buflist_dequeue() to remove any
           messages that are stuck in the mailbox DMA queue that did not get processed
           since the interrupts were masked.
  @example:
  @see also: mbox_resume_recv , mbox_recv_buflist_dequeue
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void mbox_pause_recv(int InstanceId, int mbox);


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Resume the mailbox recieve queue
  @function name: mbox_resume_recv
  @prototype: void mbox_resume_recv(int mbox)
  @input:   mbox - mailbox number
  @output:
  @return:
  @notes:   This API allows mailbox interrupt processing to continue.  The caller
            should unpause the mailbox and subsequently dequeue messages.

  @example:
  @see also: mbox_pause_recv , mbox_recv_buflist_dequeue
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void mbox_resume_recv(int InstanceId, int mbox);


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Register the host interrupt handler API
  @function name: mbox_register_host_int_handler
  @prototype: void mbox_register_host_int_handler(int mbox, handler)
  @input:   mbox - mailbox number
  @output:
  @return:
  @notes:   This API allows to register host interrupt handler with the mbox driver.
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void mbox_register_host_int_handler(int InstanceId, int num, MBOX_HOST_INT_HANDLER handler);


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Enable the host interrupts
  @function name: mbox_arm_host_int
  @prototype: void mbox_arm_host_int(int mbox)
  @input:   mbox - mailbox number
  @output:
  @return:
  @notes:   This API enables the host interrupts against each mailbox.
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void mbox_arm_host_int(int InstanceId, int num);


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Register the IO complete callback functions with Mbox driver
  @function name: mbox_register_io_complete_callbacks
  @prototype: void mbox_register_io_complete_callbacks((int)mbox, 
                                              senddone_cbfn,
                                              recvdone_cbfn,
                                              context);
  @input:   mbox - mailbox number, send and recv call back functions, and context
  @output:
  @return:
  @notes:   This API allows to register DMA complete callback functions with the mbox driver.
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void mbox_register_io_complete_callbacks(int InstanceId, int mbox,
                                         MBOX_IO_COMPLETE_CALLBACK pfn_send_complete,
                                         MBOX_IO_COMPLETE_CALLBACK pfn_recv_complete,
                                         void* pContext);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Increments the Host Tx credit counter registers
  @function name: mbox_tx_credit_inc
  @prototype: void mbox_tx_credit_inc(int mbox)
  @input:   mbox - mailbox number
  @output:
  @return:
  @notes:   This API allows to increment TX credit counter register.
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/ 
void mbox_tx_credit_inc(int InstanceId, int mbox);

    
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Increments the Host Rx credit counter registers
  @function name: mbox_rx_credit_inc
  @prototype: void mbox_rx_credit_inc(int mbox)
  @input:   mbox - mailbox number
  @output:
  @return:
  @notes:   This API allows to increment RX credit counter register.
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/    
void mbox_rx_credit_inc(int InstanceId, int mbox);


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: API to write into the scratch register
  @function name: mbox_write_scratchbyte
  @prototype: void mbox_write_scratchbyte(int mbox, uint8 byte)
  @input:   index - id of the scratch register
  @output:
  @return:
  @notes:   This API allows to write into scratch register.
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void mbox_write_scratchbyte(int InstanceId, int index, uint8 byte);


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: API to read from the scratch Register
  @function name: mbox_read_scratchbyte
  @prototype: uint8 mbox_read_scratchbyte(int mbox)
  @input:   index - id of scratch register
  @output:
  @return:
  @notes:   This API allows to read from scratch register.
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
uint8 mbox_read_scratchbyte(int InstanceId, int index);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: API to write INT_HOST Register
  @function name: mbox_write_int_host
  @prototype: void mbox_write_int_host(int InstanceId, uint8 byte)
  @input:   byte - value to be written to INT_HOST register
  @output:
  @return:
  @notes:   This API allows to read from scratch register.
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void mbox_write_int_host(int InstanceId, uint8 byte);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: API to read from the scratch Register
  @function name: mbox_read_scratchbyte
  @prototype: uint8 mbox_read_int_host(int InstanceId);
  @input:   
  @output: Value of INT_HOST register
  @return:
  @notes:   This API allows to read from scratch register.
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
uint8 mbox_read_int_host(int InstanceId);



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: API to pop a byte from mbox fifos.
  @function name: mbox_byte_pop
  @prototype: void mbox_byte_pop(int mbox, uint8 *pbyte)
  @input:   mbox - mailbox number
  @output:
  @return:
  @notes:   This API allows to pop a byte from mbox fifos.
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void mbox_byte_pop(int InstanceId, int mbox, uint8 *pbyte);

    
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: API to push a byte into mbox fifos.
  @function name: mbox_byte_push
  @prototype: void mbox_byte_push(int mbox, uint8 byte)
  @input:   mbox - mailbox number
  @output:
  @return:
  @notes:   This API allows to push a byte into mbox fifos.
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/  
void mbox_byte_push(int InstanceId, int mbox, uint8 byte);


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: API to pop a word from mbox fifos.
  @function name: mbox_word_pop
  @prototype: void mbox_word_pop(int mbox, uint32 *pword)
  @input:   mbox - mailbox number, pointer to word variable.
  @output:
  @return:
  @notes:   This API allows to pop a word from mbox fifos.
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/ 
void mbox_word_pop(int InstanceId, int mbox, uint32 *pword);


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: API to push a word into mbox fifos.
  @function name: mbox_word_push
  @prototype: void mbox_word_push(int mbox, uint32 word)
  @input:   mbox - mailbox number, word to push into fifo.
  @output:
  @return:
  @notes:   This API allows to push a word into mbox fifos.
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/ 
void mbox_word_push(int InstanceId, int mbox, uint32 data);


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: API to dump the received mbox buffers list
  @function name: mbox_dump_recv_queued_buffers
  @prototype: void mbox_dump_recv_queued_buffers(int mbox)
  @input:   mbox - mailbox number
  @output:
  @return:
  @notes:   This API allows to dump the chain of received buffers addresses list.
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void mbox_dump_recv_queued_buffers(int InstanceId, int mbox);


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: API to dump the sent mbox buffers list
  @function name: mbox_dump_send_queued_buffers
  @prototype: void mbox_dump_send_queued_buffers(int mbox)
  @input:   mbox - mailbox number
  @output:
  @return:
  @notes:   This API allows to dump the chain of sent buffers addresses list.
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void mbox_dump_send_queued_buffers(int InstanceId, int mbox);


#endif /*SDIO_MBOX_API_H_*/
