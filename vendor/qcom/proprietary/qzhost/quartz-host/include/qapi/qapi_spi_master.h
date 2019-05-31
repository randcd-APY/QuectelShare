#ifndef __QAPI_SPI_MASTER_H__
#define __QAPI_SPI_MASTER_H__

/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$


/*=================================================================================
 *
 *     SERIAL PERIPHERAL INTERFACE MASTER
 *
 *===============================================================================*/
 /** @file qapi_spi_master.h
 */
 /*
 * Serial Peripheral Interface (SPI)
 *
 * SPI is a full duplex communication bus to interface peripherals in several 
 * communication modes as configured by the client software. The SPI driver API
 * provides a high level interface to expose the capabilities of the SPI master.
 *
 * Typical usage:
 * qapi_SPIM_Open() - get a handle to an SPI instance
 * qapi_SPIM_Enable() - turn on all resources required for a succesful SPI transaction.
 * qapi_SPIM_Full_Duplex() - Generic transfer API to perform a transfer on the SPI bus.
 * qapi_SPIM_Disable() - turn off all resources set by qapi_SPIM_Enable()
 * qapi_SPIM_Close() - Destroy all objects created by the SPI Handle
 * 
 * Note about SPI power:
 * Calling  qapi_SPIM_Open() and leaving it open does not drain any power. 
 * If the client is expecting to do several back-to-back SPI transfers, the recommended
 * approach is to call Enable, perform all transfers, then call Disable. Calling Enable/Disable
 * for every transfer will affect throughput and increase bus idle period.
 *
 * SPI transfers:
 * SPI transfers use BAM (DMA mode), so we expect buffers passed by the client to
 * be uncached RAM addresses. There is no address or length alignment requirement.
 * SPI Modes: The SPI master supports all 4 SPI modes, and this can be changed per transfer.
 * See \c qapi_SPIM_Config_t for configuration specification details.
 * The driver supports parallel transfers on different SPI instances.
 * 
 * Note about SPI Modes:
 * The following note is meant as a guide, but always check the meaning of SPI modes in your 
 * SPI Slave Specifications. Some manufacturers use different mode meanings.
 * SPI Mode 0: CPOL = 0, and CPHA = 0
 * SPI Mode 1: CPOL = 0, and CPHA = 1
 * SPI Mode 2: CPOL = 1, and CPHA = 0
 * SPI Mode 3: CPOL = 1, and CPHA = 1
 * For CPOL = 1, set SPIM_Clk_Polarity to QAPI_SPIM_CLK_IDLE_HIGH_E
 * For CPOL = 0, set SPIM_Clk_Polarity to QAPI_SPIM_CLK_IDLE_LOW_E
 * For CPHA = 1, set SPIM_Shift_Mode to QAPI_SPIM_INPUT_FIRST_MODE_E
 * For CPHA = 0, set SPIM_Shift_Mode to QAPI_SPIM_OUTPUT_FIRST_MODE_E
 */
 
/*==================================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.ioe/1.0/v2/rom/release/api/buses/spi/qapi_spi_master.h#5 $

when         who     what, where, why
----------   ---     -----------------------------------------------------------------
02/12/17     leo     (Tech Comm) Edited/added Doxygent comments and markup.
12/14/2015   vmk     Initial version
==================================================================================*/

/*==================================================================================

                               INCLUDE FILES

==================================================================================*/

/*==================================================================================

                                   MACROS

==================================================================================*/

/** @addtogroup qapi_spi_master
@{ */

/** 
 * @name Error Codes Returned by the SPI Master API
 * @{
 */
#define QAPI_SPIM_ERROR                     __QAPI_ERROR(QAPI_MOD_BSP_SPI_MASTER, 1)
#define QAPI_SPIM_ERROR_INVALID_PARAM       __QAPI_ERROR(QAPI_MOD_BSP_SPI_MASTER, 2)
#define QAPI_SPIM_ERROR_MEM_ALLOC           __QAPI_ERROR(QAPI_MOD_BSP_SPI_MASTER, 3)
#define QAPI_SPIM_ERROR_TRANSFER_TIMEOUT    __QAPI_ERROR(QAPI_MOD_BSP_SPI_MASTER, 4)
#define QAPI_SPIM_ERROR_CLK_ENABLE_FAIL     __QAPI_ERROR(QAPI_MOD_BSP_SPI_MASTER, 5)
#define QAPI_SPIM_ERROR_GPIO_ENABLE_FAIL    __QAPI_ERROR(QAPI_MOD_BSP_SPI_MASTER, 6)
#define QAPI_SPIM_ERROR_CLK_DISABLE_FAIL    __QAPI_ERROR(QAPI_MOD_BSP_SPI_MASTER, 7)
#define QAPI_SPIM_ERROR_GPIO_DISABLE_FAIL   __QAPI_ERROR(QAPI_MOD_BSP_SPI_MASTER, 8)
#define QAPI_SPIM_ERROR_CLK_SET_FREQ_FAIL   __QAPI_ERROR(QAPI_MOD_BSP_SPI_MASTER, 9) 
#define QAPI_SPIM_ERROR_QUP_STATE_INVALID   __QAPI_ERROR(QAPI_MOD_BSP_SPI_MASTER, 10)
#define QAPI_SPIM_ERR_INTERRUPT_REGISTER    __QAPI_ERROR(QAPI_MOD_BSP_SPI_MASTER, 11)
/** @} */ /* end namegroup */

/*==================================================================================

                               TYPE DEFINITIONS

==================================================================================*/
/**
* SPI callback function type.
*
* This type is used by the client to register its callback notification function. The 
* callback_Ctxt is the context object that will be passed untouched by the SPI Master
* driver to help the client identify which transfer completion instance is being signaled.
*/
typedef void (*qapi_SPIM_Callback_Fn_t) (uint32_t status, void *callback_Ctxt);

/**
* SPI instance enumeration.
*
* This enumeration lists the possible SPI instance indicating which hardware SPI master 
* is to be used for the current SPI transaction.
*/
typedef enum
{
    QAPI_SPIM_INSTANCE_1_E = 1, /**< SPIM instance 1. */
    QAPI_SPIM_INSTANCE_2_E,     /**< SPIM instance 2. */
    QAPI_SPIM_INSTANCE_3_E,     /**< SPIM instance 3. */
    QAPI_SPIM_INSTANCE_4_E,     /**< SPIM instance 4. */
    QAPI_SPIM_INSTANCE_MAX_E,
} qapi_SPIM_Instance_t;

/**
* SPI clock mode type.
*
* This type defines the clock mode that the client wishes to set in the 
* SPI configuration.
*/
typedef enum 
{
   QAPI_SPIM_CLK_NORMAL_E,      /**< Turns off the SPI clock during the Idle state. */
   QAPI_SPIM_CLK_ALWAYS_ON_E,   /**< Runs the SPI clock during the Idle state. */
   QAPI_SPIM_CLK_INVALID_E = 0x7FFFFFFF
}qapi_SPIM_Clk_Mode_t;

/**
* SPI clock polarity type.
*
* This type defines the clock polarity that the client wishes to set in the 
* SPI configuration.
*/
typedef enum 
{
   QAPI_SPIM_CLK_IDLE_LOW_E,    /**< Clock signal is low when in Idle. */
   QAPI_SPIM_CLK_IDLE_HIGH_E,   /**< Clock signal is high when in Idle. */
   QAPI_SPIM_CLK_IDLE_INVALID_E = 0x7FFFFFFF
}qapi_SPIM_Clk_Polarity_t;

/**
* SPI phase type.
*
* This type defines the clock phase that the client wishes to set in the 
* SPI configuration. 
*/
typedef enum 
{
   QAPI_SPIM_INPUT_FIRST_MODE_E,    /**< In both master and slave, the input bit is shifted in first. */
   QAPI_SPIM_OUTPUT_FIRST_MODE_E,   /**< In both master and slave, the output bit is shifted in first. */
   QAPI_SPIM_SHIFT_INVALID_E = 0x7FFFFFFF
}qapi_SPIM_Shift_Mode_t;
 
 /**
* SPI chip select polarity type.
*
* This type defines the polarity of the chip select line.
*/
typedef enum 
{
   QAPI_SPIM_CS_ACTIVE_LOW_E,    /**< During the Idle state, the CS line is held low. */
   QAPI_SPIM_CS_ACTIVE_HIGH_E,   /**< During the Idle state, the CS line is held high. */
   QAPI_SPIM_CS_ACTIVE_INVALID_E = 0x7FFFFFFF
}qapi_SPIM_CS_Polarity_t;

/**
* SPI chip select (CS) assertion type.
*
* This type defines how the chip select line is configured between N word cycles.
*/
typedef enum 
{
   QAPI_SPIM_CS_DEASSERT_E,        /**< CS is deasserted after transferring data for N clock cycles. */
   QAPI_SPIM_CS_KEEP_ASSERTED_E,   /**< CS is asserted as long as the core is in Run state. */
   QAPI_SPIM_CS_MODE_INVALID_E = 0x7FFFFFFF
}qapi_SPIM_CS_Mode_t;

/**
* SPI master configuration.
*
* The SPI master configuration is the collection of settings specified for each SPI
* transfer call to select the various possible SPI transfer parameters.
*/
typedef struct
{
   // Add note to timing diagram here.
   qapi_SPIM_Clk_Polarity_t SPIM_Clk_Polarity;
   /**< Clock polarity type to be used for the SPI core. */
   qapi_SPIM_Shift_Mode_t SPIM_Shift_Mode;
   /**< Shift mode (CPHA) type to be used for SPI core. */
   qapi_SPIM_CS_Polarity_t SPIM_CS_Polarity;
   /**< CS polarity type to be used for the SPI core. */
   qapi_SPIM_CS_Mode_t SPIM_CS_Mode;
   /**< CS mode to be used for the SPI core. */
   qapi_SPIM_Clk_Mode_t SPIM_Clk_Always_On;
   /**< Clock mode type to be used for the SPI core. */
   uint8_t SPIM_Bits_Per_Word;
   /**< SPI bits per word, any value from 3 to 31. */

   uint8_t SPIM_Slave_Index;
   /**< Slave index, beginning at 0, if mulitple SPI devices are connected to the same master. */
   
   uint32_t min_Slave_Freq_Hz;
   /**< Minimum slave frequency in Hertz. */

   uint32_t max_Slave_Freq_Hz;
   /**< Host will set the SPI clock frequesncy to max_slave_freq_hz or to the nearest
        supported frequency greater than min_slave_freq_hz. */

   uint32_t deassertion_Time_Ns;
   /**< Minimum delay between two word (N-bit) transfers. */

   qbool_t loopback_Mode;
   /**< Loopback mode (normally 0). If set, the SPI controller enables Loopback mode; used primarily for testing. */
   
   qbool_t hs_Mode;
   /**< HS mode (normally 0). If set, it forces the HS bit in QUP, irrespective of the actual frequency. */
} qapi_SPIM_Config_t;

/**
* SPI transfer type.
*
* This type specifies the address and length of the buffer for an SPI transaction.
*/
typedef struct
{
   void* buf_phys_addr;
   /**< Buffer address for read or write data. */
   uint16_t buf_len; 
   /**< Size of the buffer in bytes. No alignment requirements, arbitrary lengths of data can be transferred. @newpagetable */
}qapi_SPIM_Transfer_t;
/** @} */ /* end_addtogroup qapi_spi_master */
/*==================================================================================

                            FUNCTION DECLARATIONS

==================================================================================*/

/** @addtogroup qapi_spi_master
@{ */

/*==================================================================================
  FUNCTION      qapi_SPIM_Open
==================================================================================*/
/**
*  Initializes the SPI Master.
*
*  This function initializes internal data structures along with associated
*  static data. In any operating mode, this function should be called before
*  calling any other SPI Master API.
*
*  @param[in]  instance    SPI instance specified by #qapi_SPIM_Instance_t.
*  @param[out] spi_Handle  Reference to a location to store the driver handle.
*
*  @return
*  QAPI_OK                          --  Module was initialized successfully. \n
*  QAPI_SPIM_ERROR_INVALID_PARAM    --  Invalid instance or handle parameter. \n
*  QAPI_SPIM_ERROR_MEM_ALLOC        --  Could not allocate space for driver structures. \n
*  QAPI_SPIM_ERR_INTERRUPT_REGISTER --  Could not register for an interrupt.
*/
qapi_Status_t qapi_SPIM_Open (qapi_SPIM_Instance_t instance, void **spi_Handle);

/*==================================================================================
  FUNCTION      qapi_SPIM_Enable
==================================================================================*/
/**
*  Enables the SPI hardware resources for an SPI transaction.
*
*  This function enables all resources required for a successful SPI transaction. This 
*  includes clocks, power resources, and pin multiplex functions. This function should 
*  be called before a transfer or a batch of SPI transfers.
* 
*  @param[in] spi_Handle  Driver handle returned by qapi_SPIM_Open().
*
*  @return
*  QAPI_OK                          --  SPI master was enabled successfully. \n
*  QAPI_SPIM_ERROR_INVALID_PARAM    --  Invalid handle parameter. \n
*  QAPI_SPIM_ERROR_CLK_ENABLE_FAIL  --  Could not enable clocks or NPA. \n
*  QAPI_SPIM_ERROR_GPIO_ENABLE_FAIL --  Could not enable GPIOs.
*/
qapi_Status_t qapi_SPIM_Enable (void *spi_Handle);

/*==================================================================================
  FUNCTION      qapi_SPIM_Disable
==================================================================================*/
/**
*  Disables the SPI Hardware resources for an SPI transaction.
*
*  This function turns off all resources used by the SPI master. This includes clocks, 
*  power resources, and GPIOs. This function should be called to put the SPI master
*  in its lowest possible power state.
* 
*  @param[in] spi_Handle  Driver handle returned by qapi_SPIM_Open().
*
*  @return
*  QAPI_OK                             --  SPI master was disabled successfully. \n
*  QAPI_SPIM_ERROR_INVALID_PARAM       --  Invalid handle parameter. \n
*  QAPI_SPIM_ERROR_CLK_DISABLE_FAIL    --  Could not disable clocks or NPA. \n
*  QAPI_SPIM_ERROR_GPIO_DISABLE_FAIL   --  Could not disable GPIOs.
*/
qapi_Status_t qapi_SPIM_Disable (void *spi_Handle);

/*==================================================================================
  FUNCTION      qapi_SPIM_Full_Duplex
==================================================================================*/
/**
*  Performs a data transfer over the SPI bus.
*
*  This function performs an asynchronous transfer over the SPI bus. Transfers can be 
*  one-directional or bi-directional. A callback is generated upon transfer completion.
* 
*  @param[in] spi_Handle  Driver handle returned by qapi_SPIM_Open().
*  @param[in] config      Pointer to the SPI configuration structure described by
                          #qapi_SPIM_Config_t.
*  @param[in] tx_Info     Transmit (write from master to slave) descriptor. Set this
                          descriptor to NULL for a read-only transfer.
*  @param[in] rx_Info     Receive (read from slave to master) descriptor. Set this
                          descriptor to NULL for a write-only transfer.
*  @param[in] c_Fn        Callback function to be invoked when the SPI transfer 
                          completes succesfully or with an error.
*  @param[in] c_Ctxt      Pointer to a client object that will be returned as an
                          argument to c_Fn.
*
*  @return
*  QAPI_OK                           --  SPI master was enabled successfully. \n
*  QAPI_SPIM_ERROR_INVALID_PARAM     --  One ore more parameters are invalid. \n
*  QAPI_SPIM_ERROR_QUP_STATE_INVALID --  SPI or BAM hardware is in a bad state. \n
*  QAPI_SPIM_ERROR_TRANSFER_TIMEOUT  --  Transfer timed out.
*/
qapi_Status_t qapi_SPIM_Full_Duplex (void *spi_Handle, qapi_SPIM_Config_t *config, 
                                     qapi_SPIM_Transfer_t *tx_Info, qapi_SPIM_Transfer_t *rx_Info, 
									 qapi_SPIM_Callback_Fn_t c_Fn, void *c_Ctxt);

/*==================================================================================
  FUNCTION      qapi_SPIM_Close
==================================================================================*/
/**
*  Closes the SPI master.
*
*  This function frees all internal data structures and closes the SPI master interface.
*  The handle returned by qapi_SPIM_Open() is rendered invalid..
*
*  @param[in] spi_handle  Driver handle returned by qapi_SPIM_Open().
*
*  @return
*  QAPI_OK  --  SPI driver was closed successfully.
*/
qapi_Status_t qapi_SPIM_Close (void *spi_handle);

/** @} */ /* end_addtogroup qapi_spi_master */

#endif	
