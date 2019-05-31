/*
 * Copyright (c) 201-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_SDCC_H__
#define __QAPI_SDCC_H__

/** 
    @file  qapi_master_sdcc.h
    @brief MASTER SDCC qapi
 */
// $QTI_LICENSE_QDN_C$

/** @addtogroup qapi_master_sdcc
@{ */

/*----------------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
 
/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

/** Enumeration of SDCC operation flags. */
typedef enum
{
    QAPI_SDCC_DIR_READ_E = 0,
    /**< A read operation is performed (default). */
    QAPI_SDCC_DIR_WRITE_E = (1 << 0),
    /**< A write operation is performed. */
    QAPI_SDCC_BLOCK_MODE_E = (1 << 1),
    /**< Use Block mode instead of Byte mode. */
    QAPI_SDCC_OP_INC_ADDR_E = (1 << 2),
    /**< Increase the address register after a R/W operation. */
    QAPI_SDCC_USES_DMA_E = (1 << 3),
    /**< Use DMA. */
    QAPI_SDCC_NON_BLOCKING_E = (1 << 4),
    /**< Nonblocking OP, return immediately. */
    QAPI_SDCC_DATA_XFER_E = (1 << 5),
    /**< Operation has a data transfer phase. */
    QAPI_SDCC_ABORT_CMD_TYP_E = (1 << 6),
    /**< Issue an IO Abort command. */
    QAPI_SDCC_RESUME_CMD_TYP_E = (1 << 7),
    /**< Issue a Bus Resume command. */
    QAPI_SDCC_SUSPEND_CMD_TYP_E = (1 << 8),
    /**< Issue a Bus Suspend command. */
    QAPI_SDCC_NOCRC_CMD_TYP_E = (1 << 9),
    /**< Command that does not use a CRC in the response. */
    QAPI_SDCC_NOIDX_CMD_TYP_E = (1 << 10),
    /**< Command that does not use an index in the response. */
    QAPI_SDCC_NORESP_CMD_TYP_E = (1 << 11),
    /**< Command that has no response. */
    QAPI_SDCC_READ_AFT_WRITE_E = (1 << 12),
    /**< Read after a write operation for CMD52. */
    QAPI_SDCC_USES_DDR_E = (1 << 13),
    /**< Use DDR mode. */
    QAPI_SDCC_NO_CARD_DETECT_E = (1 << 14),
    /**< No card detection. */
    QAPI_SDCC_INTR_ENABLE_E = (1 << 15),
    /**< Enable interrupt. */  
    QAPI_SDCC_INTR_DISABLE_E = (1 << 16)
    /**< Disable interrupt. */
} qapi_SDCC_Op_Flags_e;

/** SDCC configuration structure.
*/
typedef struct {
    uint32_t  adma_Descr_Items;
    /**< ADMA descriptor items, which indicate the maximum number
         of qapi_SDCC_Dma_Item items. */
    uint32_t  SDCC_Bus_Width;
    /**< SDCC bus width: \n
         1 -- 1 bit mode; 4 -- 4 bits mode. */
    uint32_t  freq;
    /**< Bus frequency; indicates the SDCC baud rate in Hz. */
    uint32_t  block_Size;
    /**< Block size; when using Block Transfer mode, this indicates
         the size of each block. */
} qapi_SDCC_Config_t;

/** SDCC DMA item structure.
*/
typedef struct {
   uint8  *data_Buf_Ptr;
   /**< Pointer to the data buffer. */
   uint32  size;
   /**< Size of the data in bytes. */
} qapi_SDCC_Dma_Item;

typedef void  *qapi_SDCC_Handle;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
/**
*  Initializes the master SDCC context.
*
*  @param[in] sdcc_Config   SDCC configuration.
*  @param[out] sdcc_Handle  Returned SDCC context pointer.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*/
qapi_Status_t
qapi_SDCCM_Init(
	qapi_SDCC_Config_t  *sdcc_Config, 
	qapi_SDCC_Handle *sdcc_Handle
);

/**
*  Deinitializes the SDCC context.
*
*  @param[in] sdcc_Handle   SDCC context pointer.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*/
qapi_Status_t
qapi_SDCCM_Deinit (
    qapi_SDCC_Handle sdcc_Handle
);

/**
*  Open a SDCC device.
*
*  Activates an SDCC device. If successful, the SDCC is then ready for data transfer.
*
*  @param[in] sdcc_Handle   SDCC context pointer.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*/
qapi_Status_t
qapi_SDCCM_Open
(
    qapi_SDCC_Handle sdcc_Handle
);

/**
*  Closes a SDCC handle.
*
*  @param[in] sdcc_Handle   Master SDCC context pointer.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*/
qapi_Status_t
qapi_SDCCM_Close
(
    qapi_SDCC_Handle sdcc_Handle
);

/**
 *  Registers a callback function.
 *
 *  @param[in] sdcc_Handle   Master SDCC context pointer.
 *  @param[in] isr_Cb   Callback function.
 *  @param[in] param    Callback function's parameter pointer.
 *
 *  @return
 *  QAPI_OK                -- Call succeeded. \n
 *  QAPI_ERROR             -- Call failed. \n
 *  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
 */
qapi_Status_t
qapi_SDCCM_Intr_Register (
    qapi_SDCC_Handle sdcc_Handle,
    void (*isr_Cb)(void *param), 
    void *param
);

/**
 *  Deregisters a callback function.
 *
 *  @param[in] sdcc_Handle   SDCC context pointer.
 *
 *  @return
 *  QAPI_OK                -- Call succeeded. \n
 *  QAPI_ERROR             -- Call failed. \n
 *  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
 */
qapi_Status_t
qapi_SDCCM_Intr_Deregister (
    qapi_SDCC_Handle sdcc_Handle
);

/**
 *  Sends data to a device via SDCC with extension command 53.
 *
 *  The posible device function numbers are: \n
 *  1. Support DMA and FIFO modes \n
 *  2. Support Block mode and Byte mode \n
 *  3. If using Block mode, data_Len_In_Bytes = n * block_size \n
 *  4. data_Buf must align to a 4-byte boundary
 *
 *  @param[in] sdcc_Handle        Master SDCC context pointer.
 *  @param[in] flags              Operation flag.
 *  @param[in] dev_Fn             Device function number.
 *  @param[in] reg_Addr           Register address.
 *  @param[in] data_Len_In_Bytes  Data length.
 *  @param[in] data_Buf           Data buffer pointer.
 *
 *  @return
 *  QAPI_OK                -- Call succeeded. \n
 *  QAPI_ERROR             -- Call failed. \n
 *  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
 */

qapi_Status_t
qapi_SDCCM_Send_Data_Ext (
    qapi_SDCC_Handle sdcc_Handle,
	qapi_SDCC_Op_Flags_e flags, 
	uint32 dev_Fn, 
	uint32 reg_Addr, 
    uint32 data_Len_In_Bytes, 
	uint8 *data_Buf
);

/**
 *  Fetch data on a device via SDCC with extension command 53.
 *
 *  The possible device function numbers are: \n
 *  1. Support DMA and FIFO modes \n
 *  2. Support Block mode and Byte mode \n
 *  3. If using Block mode, data_Len_In_Bytes = n * block_size \n
 *  4. data_Buf must align to a 4-byte boundary
 *
 *  @param[in] sdcc_Handle        Master SDCC context pointer.
 *  @param[in] flags              Operation flag.
 *  @param[in] dev_Fn             Device function number.
 *  @param[in] reg_Addr           Register address.
 *  @param[in] data_Len_In_Bytes  Data length. 
 *  @param[in] data_Buf           Data buffer pointer.
 *
 *  @return
 *  QAPI_OK                -- Call succeeded. \n
 *  QAPI_ERROR             -- Call failed. \n
 *  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
 */

qapi_Status_t
qapi_SDCCM_Receive_Data_Ext (
    qapi_SDCC_Handle sdcc_Handle,
	qapi_SDCC_Op_Flags_e flags, 
	uint32 dev_Fn, 
	uint32 reg_Addr, 
    uint32 data_Len_In_Bytes, 
	uint8 *data_Buf
);

/** 
 *  Sends/receives one byte of data into/from a device via SDCC with CMD 52.
 *
 *  @param[in] sdcc_Handle        Master SDCC context pointer.
 *  @param[in] flags              Operation flag.
 *  @param[in] dev_Fn             Device function number.
 *  @param[in] reg_Addr           Register address.
 *  @param[in] data_In            Send one byte of data.
 *  @param[in] data_Out           Receive one byte of data.
 *
 *  @return
 *  QAPI_OK                -- Call succeeded. \n
 *  QAPI_ERROR             -- Call failed. \n
 *  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
 */ 

qapi_Status_t
qapi_SDCCM_Send_Receive_Byte (
    qapi_SDCC_Handle sdcc_Handle,
	qapi_SDCC_Op_Flags_e flags, 
	uint32 dev_Fn, 
	uint32 reg_Addr, 
    uint8 data_In, 
	uint8 *data_Out
);

/** 
 *  Sends/receives data into/from a device via SDCC with CMD 53.
 *
 *  The possible device function numbers are: \n
 *  1. Only DMA mode transfer \n
 *  2. Support Block mode and Byte mode \n
 *  3. If using Block mode, data_Len_In_Bytes = n * block_size \n
 *  4. dma_Tab_Count maximum value; see #qapi_SDCC_Config_t \n
 *  5. data_Buf_Ptr in dma_Tab must align to a 4-byte boundary
 *
 *  @param[in] sdcc_Handle        Master SDCC context pointer.
 *  @param[in] flags              Operation flag.
 *  @param[in] dev_Fn             Device function number.
 *  @param[in] reg_Addr           Register address.
 *  @param[in] data_Len_In_Bytes  Data length (sum of all data segment sizes). 
 *  @param[in] dma_Tab_Count      DMA entry count; for the maximum value, see #qapi_SDCCM_Init.
 *  @param[in] dma_Tab            DMA entry array pointer.
 *
 *  @return
 *  QAPI_OK                -- Call succeeded. \n
 *  QAPI_ERROR             -- Call failed. \n
 *  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
 */ 

qapi_Status_t
qapi_SDCCM_Send_Receive_Data_Scatter_Gather (
    qapi_SDCC_Handle sdcc_Handle,
	qapi_SDCC_Op_Flags_e flags, 
	uint32 dev_Fn, 
	uint32 reg_Addr, 
    uint32 data_Len_In_Bytes, 
	uint32 dma_Tab_Count, 
    qapi_SDCC_Dma_Item *dma_Tab
);

/** 
 *  Enables/disables a SDCC interrupt.
 *
 *  @param[in] sdcc_Handle        Master SDCC context pointer.
 *  @param[in] enable             Enable/disable the interrupt.
 *
 *  @return
 *  QAPI_OK                -- Call succeeded. \n
 *  QAPI_ERROR             -- Call failed.
 */
qapi_Status_t 
qapi_SDCCM_EnableDisableIrq(
    qapi_SDCC_Handle sdcc_Handle, 
	boolean enable
);

/** @} */ /* end_addtogroup qapi_master_sdcc */

#endif // ifndef __QAPI_MASTER_SDCC_H__
