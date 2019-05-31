#ifndef __QAPI_UART_H__
#define __QAPI_UART_H__

/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_uart.h
 */

/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:
   qapi_UART_Close
   qapi_UART_Open
   qapi_UART_Receive
   qapi_UART_Transmit
==================================================================================================*/
/*==================================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.ioe/1.0/v2/rom/release/api/buses/uart/qapi_uart.h#6 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
02/13/17   leo     (Tech Comm) Edited/added Doxygen comments and markup.
12/10/15   VV      Initial revision.

==================================================================================================*/

/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/


/*==================================================================================================
                                             ENUMERATIONS
==================================================================================================*/

/** @addtogroup qapi_uart
@{ */

/**
 * UART port ID enumeration. 
 *  
 * Enumeration to specify which port is to be opened during the uart_open 
 * call.
 */
typedef enum
{
   QAPI_UART_DEBUG_PORT_E = 0, /**< Debug port. */
   QAPI_UART_HS_PORT_E = 1, /**< High speed port. */

   QAPI_UART_MAX_PORTS_E,
}qapi_UART_Port_Id_e;

/**
 * Enumeration to specify how many UART bits are to be used per character configuration.
 *  
 */
typedef enum
{
  QAPI_UART_5_BITS_PER_CHAR_E  = 0,  /**< 5 bits per character. */
  QAPI_UART_6_BITS_PER_CHAR_E  = 1,  /**< 6 bits per character. */
  QAPI_UART_7_BITS_PER_CHAR_E  = 2,  /**< 7 bits per character. */
  QAPI_UART_8_BITS_PER_CHAR_E  = 3,  /**< 8 bits per character. */
} qapi_UART_Bits_Per_Char_e;

/**
 * Enumeration for the UART number of stop bits configuration.  
 *   
 */
typedef enum
{
  QAPI_UART_0_5_STOP_BITS_E    = 0,  /**< 0.5 stop bits. */
  QAPI_UART_1_0_STOP_BITS_E    = 1,  /**< 1.0 stop bit. */
  QAPI_UART_1_5_STOP_BITS_E    = 2,  /**< 1.5 stop bits. */
  QAPI_UART_2_0_STOP_BITS_E    = 3,  /**< 2.0 stop bits. */
} qapi_UART_Num_Stop_Bits_e;

/**
 * Enumeration for the UART parity mode configuration.  
 *   
 */
typedef enum
{
  QAPI_UART_NO_PARITY_E        = 0,  /**< No parity. */ 
  QAPI_UART_ODD_PARITY_E       = 1,  /**< Odd parity. */
  QAPI_UART_EVEN_PARITY_E     = 2,   /**< Even parity. */
  QAPI_UART_SPACE_PARITY_E     = 3,  /**< Space parity. @newpage */
} qapi_UART_Parity_Mode_e;

/** @} */ /* end_addtogroup qapi_uart */

/** @addtogroup qapi_uart
@{ */

/**
 * UART handle that is passed to the client when a UART port is opened.
 *    
 */
typedef void* qapi_UART_Handle_t;

/**
 * Transmit and receive operation callback type.
 *    
 * @param[in] num_bytes  Number of bytes.
 * @param[out] cb_data   Pointer to the callback data.
 */
typedef void(*qapi_UART_Callback_Fn_t)(uint32_t num_bytes, void *cb_data);

/** @} */ /* end_addtogroup qapi_uart */

/** @addtogroup qapi_uart
@{ */

/** Structure for UART configuration. */
typedef struct
{
   uint32_t                     baud_Rate; /**< Supported baud rates are 115200 bps, 1 Mbps,
                                                       2 Mbps, 3 Mbps, and 4 Mbps. */
   qapi_UART_Parity_Mode_e      parity_Mode; /**< Parity mode. */
   qapi_UART_Num_Stop_Bits_e    num_Stop_Bits; /**< Number of stop bits. */
   qapi_UART_Bits_Per_Char_e    bits_Per_Char; /**< Bits per character. */
   qbool_t                      enable_Loopback; /**< Enable loopback. */
   qbool_t                      enable_Flow_Ctrl; /**< Enable flow control. */
   qapi_UART_Callback_Fn_t         tx_CB_ISR; /**< Transmit callback, called from ISR context. \n
                                                       Be sure not to violate ISR guidelines. \n
                                                       @latexonly \textbf{Note:} @endlatexonly
                                                       Do not call uart_transmit or uart_receive
                                                       APIs from this callback. */
   qapi_UART_Callback_Fn_t         rx_CB_ISR; /**< Receive callback, called from ISR context. \n
                                                       Be sure not to violate ISR guidelines. \n
                                                       @latexonly \textbf{Note:} @endlatexonly
                                                       Do not call uart_transmit or uart_receive
                                                       APIs from this callback. */
}qapi_UART_Open_Config_t;

/** @} */ /* end_addtogroup qapi_uart */


/** @addtogroup qapi_uart
@{ */

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/**
 * Closes the UART port. Not to be called from ISR context.
 *
 * Releases clock, interrupt, and GPIO handles related to this UART and
 *          cancels any pending transfers.
 * 
 * @note1hang Do not call this API from ISR context.
 * 
 * @param[in] handle      UART handle provided by qapi_UART_Open().
 * 
 * @return
 * QAPI_OK      Port close successful. \n
 * QAPI_ERROR   Port close failed.
 * 
 */

qapi_Status_t qapi_UART_Close
(  
   qapi_UART_Handle_t handle
);

/**
 * Initializes the UART port. Not to be called from ISR context.
 *
 * Opens the UART port and configures the corresponding clocks, interrupts, and GPIO.
 * 
 * @note1hang Do not call this API from ISR context.
 * 
 * @param[in] handle UART handle. 
 * @param[in] id ID of the port to be opened.
 * @param[in] config Structure that holds all configuration data.
 *  
 * @return 
 * QAPI_OK      Port open successful. \n
 * QAPI_ERROR   Port open failed.
 *  
 *  
 */

qapi_Status_t qapi_UART_Open
(
   qapi_UART_Handle_t*        handle, 
   qapi_UART_Port_Id_e        id, 
   qapi_UART_Open_Config_t*   config
);

/**
 * Queues the buffer provided for receiving the data. Not to be called from ISR context.
 *
 * This is an asynchronous call. rx_cb_isr is called when the Rx transfer completes.
 * The buffer is owned by the UART driver until rx_cb_isr is called.
 * 
 * There must always be a pending Rx. The UART hardware has a limited buffer
 * (FIFO), and if there is no software buffer available for HS-UART,
 * the flow control will de-assert the RFR line.
 * @newpage           
 * Call uart_receive immediately after uart_open to queue a buffer.
 * After every rx_cb_isr, from a different non-ISR thread, queue the next transfer.
 * 
 * There can be a maximum of 2 buffers queued at a time.
 * 
 * @note1hang Do not call this API from ISR context.
 * 
 * @param[in] handle       UART handle provided by qapi_UART_Open().
 * @param[in] buf          Buffer to be filled with data.
 * @param[in] buf_Size     Buffer size. Must be @ge 4 and a multiple of 4. 
 * @param[in] cb_Data      Callback data to be passed when rx_cb_isr is called 
 *                         during Rx completion.
 *  
 * @return 
 * QAPI_OK      Queuing of the receive buffer was successful. \n
 * QAPI_ERROR   Queuing of the receive buffer failed.
 *  
 */

qapi_Status_t qapi_UART_Receive
(
   qapi_UART_Handle_t   handle,
   char*                buf, 
   uint32_t             buf_Size, 
   void*                cb_Data
);

/**
 * Transmits data from a specified buffer. Not to be called from ISR context.
 *
 * This is an asynchronous call. The buffer is queued for Tx, and when transmit is completed,
 * tx_cb_isr is called.
 * 
 * The buffer is owned by the UART driver until tx_cb_isr is called.
 * 
 * @note1hang Do not call this API from ISR context.
 *
 * @param[in] handle         UART handle provided by qapi_UART_Open().
 * @param[in] buf            Buffer with data for transmit. 
 * @param[in] bytes_To_Tx    Bytes of data to transmit
 * @param[in] cb_Data        Callback data to be passed when tx_cb_isr is called 
 *                           during Tx completion. 
 *  
 * @return 
 * QAPI_OK      Queuing of the transmit buffer was successful. \n
 * QAPI_ERROR   Queuing of the transmit buffer failed. 
 *
 */

qapi_Status_t qapi_UART_Transmit
(
   qapi_UART_Handle_t handle, 
   char* buf, 
   uint32_t bytes_To_Tx, 
   void* cb_Data
);

/** @} */ /* end_addtogroup qapi_uart */

#endif
