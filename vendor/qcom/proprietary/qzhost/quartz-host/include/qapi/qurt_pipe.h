#ifndef QURT_PIPE_H
#define QURT_PIPE_H

/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
  @file qurt_pipe.h 

  @brief   Prototypes of the pipe interface API  
   This is a pipe or message queue
   
   The QuRT pipe service is used to perform synchronized exchanges of data streams
   between threads.
   
   The size of the pipe is defined as a pipe attribute.
   
   A message to be sent over pipe is called an element. An element is of fixed size,
   defined as a pipe attribute.
   
   Multiple threads can read from or write to a single pipe.

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.
*/

#include "qurt_types.h"

/** @addtogroup pipe_types
@{ */
/*=============================================================================
                        CONSTANTS AND MACROS
=============================================================================*/

#define QURT_PIPE_ATTR_OBJ_SIZE_BYTES  16 /**< QuRT pipe attribute object size.*/

/*=============================================================================
                        TYPEDEFS
=============================================================================*/

 /** QuRT pipe types. */
typedef void * qurt_pipe_t;

/**  Represents pipe attributes. */
typedef struct qurt_pipe_attr  /* 8 byte aligned */
{
   unsigned long long _bSpace[QURT_PIPE_ATTR_OBJ_SIZE_BYTES/sizeof(unsigned long long)];
   /**< Opaque OS object accessible via attribute accessor APIs. */
}qurt_pipe_attr_t;


/*=============================================================================
                        FUNCTIONS
=============================================================================*/
/**
  Initializes the specified pipe attribute structure.

  @datatypes
  #qurt_pipe_attr_t
 
  @param[in,out] attr Pointer to the destination structure for the pipe 
                      attributes.

  @return
  None.

  @dependencies
  None.
*/
void qurt_pipe_attr_init(qurt_pipe_attr_t *attr);

/**
  Specifies the length of the memory area to be used for a pipe's data buffer. 
  
  The length is expressed in terms of the number of data elements that can be stored
  in the buffer.
  
  The buffer is allocated when the pipe is created.

  @datatypes
  #qurt_pipe_attr_t

  @param[in,out] attr Pointer to the pipe attribute structure.
  @param[in] elements Pipe length. 

  @return
  None.

  @dependencies
  None.
*/
void qurt_pipe_attr_set_elements(qurt_pipe_attr_t *attr, uint32 elements);

/**
  Specifies the size of an element in a pipe. The maximum size of an element is dependent on the 
  underlying RTOS; in some instances, the RTOS can limit the maximum size of an element.
  
  @note1hang The user thread is responsible for the data elements to be sent through a pipe.

  @datatypes
  #qurt_pipe_attr_t

  @param[in,out] attr Pointer to the pipe attribute structure.
  @param[in] element_size Size of the element.

  @return
  None.

  @dependencies
  None.
*/
void qurt_pipe_attr_set_element_size(qurt_pipe_attr_t *attr, uint32 element_size);

/**
  Creates a pipe object and its associated data buffer, and initializes the pipe object.

  @datatypes
  #qurt_pipe_t \n
  #qurt_pipe_attr_t
  
  @param[out] pipe  Pointer to the created pipe object.
  @param[in]  attr  Pointer to the attribute structure used to create the pipe.

  @return 
  QURT_EINVALID -- Invalid attribute specified for the pipe. \n
  QURT_EMEM -- Failed to allocate memory.
  
  @dependencies
  None.
 */
int qurt_pipe_create(qurt_pipe_t *pipe, qurt_pipe_attr_t *attr);

/**
  Deletes the specified pipe and deallocates the pipe object and its
  associated data buffer.

  @note1hang Pipes must be deleted when they are no longer in use. Failure to do this 
             causes resource leaks in the QuRT library.\n
  @note1hang Pipes must not be deleted while they are still in use. If this happens the
             behavior of QuRT is undefined. 

  @datatypes
  #qurt_pipe_t
  
  @param[in] pipe Pipe object to delete.

  @return 
  None.

  @dependencies
  None.
 */
void qurt_pipe_delete(qurt_pipe_t pipe);

/**
  Writes a data item to the specified pipe.

  If a thread writes to a full pipe, it is suspended on the pipe. When another thread reads
  from the pipe, the suspended thread is awakened and can then write data to the pipe.

  Pipe writes are limited to transferring a single data element per operation.

  @datatypes
  #qurt_pipe_t
  
  @param[in] pipe Pipe object to write to.
  @param[in] data Pointer to the buffer containing the data to be sent.

  @return
  None.

  @dependencies
  None.
*/
void qurt_pipe_send(qurt_pipe_t pipe, void *data);

/**
  Reads a data item from the specified pipe.

  If a thread reads from an empty pipe, it is suspended on the pipe. When another thread
  writes to the pipe, the suspended thread is awakened and can then read data from the pipe.
  Pipe data items are defined as elements.
  
  Pipe reads are limited to transferring a single element per operation.

  @datatypes
  #qurt_pipe_t
  
  @param[in] pipe Pipe object to read from.
  @param[out] data Pointer to the buffer where the data is received.

  @return
  None.

  @dependencies
  None.
*/
void qurt_pipe_receive(qurt_pipe_t pipe, void *data);

/**
  Writes a data item to the specified pipe (without suspending the thread if the pipe is full).

  If a thread writes to a full pipe, the operation returns immediately with the value QURT_EFAILED.
  Otherwise, QURT_EOK indicates a successful write operation.
  
  Pipe writes are limited to transferring a single data element per operation.

  @datatypes
  #qurt_pipe_t
  
  @param[in] pipe Pointer to the pipe object to write to.
  @param[in] data Pointer to the buffer containing the data to be sent.

  @return
  0 -- Success. \n
  -1 -- Failure (pipe full).

  @dependencies
  None.
*/ 
int qurt_pipe_try_send(qurt_pipe_t pipe, void *data);

/**
  Reads a data item from the specified pipe (without suspending the thread if the pipe is
  empty).

  If a thread reads from an empty pipe, the operation returns immediately with the value
  QURT_EFAILED. Otherwise, QURT_EOK indicates a successful read operation.

  Pipe reads are limited to transferring a single data element per operation.

  @datatypes
  #qurt_pipe_t
  
  @param[in]  pipe    Pipe object to read from.
  @param[out] data    Pointer to the buffer where the data is received.

  @return
  0 -- Success. \n
  -1 -- Failure (pipe empty).

  @dependencies
  None.
*/
int qurt_pipe_try_receive(qurt_pipe_t pipe, void *data);

/**
  Writes a data item to the specified pipe.

  If a thread writes to a full pipe, it is suspended on the pipe for the specified
  time period.
  
  A timeout occurs when no thread reads data from the pipe during the specified
  time period.

  Pipe writes are limited to transferring a single data element per operation.

  @datatypes
  #qurt_pipe_t \n
  #qurt_time_t
  
  @param[in] pipe    Pipe object to write to.
  @param[in] data    Pointer to the buffer containing the data to be sent.
  @param[in] timeout How long a thread will wait on the pipe before the send is cancelled:
                      - QURT_TIME_NO_WAIT -- Return immediately without any waiting.
                      - QURT_TIME_WAIT_FOREVER -- Equivalent to regular qurt_pipe_send().
                      - Timeout value (in system ticks) -- Return after waiting for a 
                        specified time value. @tablebulletend

  @return
  QURT_EOK -- Data item sent. \n
  QURT_EINVALID -- Invalid argument. \n
  QURT_EFAILED_TIMEOUT -- Wait ended because the timeout interval was exceeded.

  @dependencies
  None.
*/
int qurt_pipe_send_timed(qurt_pipe_t pipe, void *data, qurt_time_t timeout);

/**
  Reads a data item from the specified pipe.

  If a thread reads from an empty pipe, it is suspended on the pipe for the specified
  time period. When another thread writes to the pipe, the suspended thread is awakened
  and can then read data from the pipe.
 
  A timeout occurs when no thread writes data to the pipe during the specified time period.
  
  Pipe reads are limited to transferring a single element per operation.

  @datatypes
  #qurt_pipe_t
  
  @param[in]  pipe    Pipe object to read from.
  @param[out] data    Pointer to the buffer where the data is received.
  @param[in] timeout  How long a thread waits on the pipe before the receive is cancelled:
                      - QURT_TIME_NO_WAIT -- Return immediately without any waiting.
                      - QURT_TIME_WAIT_FOREVER -- Equivalent to regular qurt_pipe_receive().
                      - Timeout value (in system ticks) -- Return after waiting for a 
                        specified time value. @tablebulletend
  @return
  None.

  @dependencies
  None.
*/
int qurt_pipe_receive_timed(qurt_pipe_t pipe, void *data, qurt_time_t timeout);

/**
  Resets the pipe to its original empty state.

  Any pending data contained in the pipe at the time it is flushed is discarded.
 
  @datatypes
  #qurt_pipe_t
  
  @param[in]  pipe    Pipe object for the pipe being flushed.
  
  @return
  QURT_EOK -- Pipe flush succeeded. \n
  QURT_EFAILED -- Pipe flush failed.  

  @dependencies
  None.
*/
int qurt_pipe_flush(qurt_pipe_t pipe);

/** @} */ /* end_addtogroup pipe_types */

#endif  /* QURT_PIPE_H */

