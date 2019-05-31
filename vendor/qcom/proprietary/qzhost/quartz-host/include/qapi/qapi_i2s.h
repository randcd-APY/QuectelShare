/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_I2S_H__
#define __QAPI_I2S_H__

/** 
    @file qapi_i2s.h
    @brief I2S qapi
 */
// $QTI_LICENSE_QDN_C$

/** @addtogroup qapi_i2s
@{ */

/*----------------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------------*/

#define __QAPI_MOD_BSP_I2S_MASTER              (15)    /**< Module number. */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/

#define __QAPI_I2S_OK                                __QAPI_ERROR(QAPI_MOD_BSP_I2S_MASTER, 0)   /**< I2S success. */
#define __QAPI_I2S_EINVAL                            __QAPI_ERROR(QAPI_MOD_BSP_I2S_MASTER, 1)   /**< Invalid. */
#define __QAPI_I2S_ERR_UNSUPPORTED_CORE_INSTANCE     __QAPI_ERROR(QAPI_MOD_BSP_I2S_MASTER, 2)   /**< Unsupported instance. */
#define __QAPI_I2S_ERR_API_INVALID_EXECUTION_LEVEL   __QAPI_ERROR(QAPI_MOD_BSP_I2S_MASTER, 3)   /**< Invalid execution level. */
#define __QAPI_I2S_ERR_HANDLE_ALLOCATION             __QAPI_ERROR(QAPI_MOD_BSP_I2S_MASTER, 4)   /**< Handle allocation failure. */
#define __QAPI_I2S_ERR_HW_INFO_ALLOCATION            __QAPI_ERROR(QAPI_MOD_BSP_I2S_MASTER, 5)   /**< Hardware allocation failure. */
#define __QAPI_I2S_ERR_BUS_NOT_IDLE                  __QAPI_ERROR(QAPI_MOD_BSP_I2S_MASTER, 6)   /**< Bus is busy. */
#define __QAPI_I2S_ERR_TRANSFER_TIMEOUT              __QAPI_ERROR(QAPI_MOD_BSP_I2S_MASTER, 7)   /**< Transfer timeout. */
#define __QAPI_I2S_ERR_INPUT_FIFO_OVER_RUN           __QAPI_ERROR(QAPI_MOD_BSP_I2S_MASTER, 8)   /**< Input FIFO overrun. */
#define __QAPI_I2S_ERR_OUTPUT_FIFO_UNDER_RUN         __QAPI_ERROR(QAPI_MOD_BSP_I2S_MASTER, 9)   /**< Output FIFO underrun. */
#define __QAPI_I2S_ERR_INPUT_FIFO_UNDER_RUN          __QAPI_ERROR(QAPI_MOD_BSP_I2S_MASTER, 10)  /**< Input FIFO underrun. */
#define __QAPI_I2S_ERR_OUTPUT_FIFO_OVER_RUN          __QAPI_ERROR(QAPI_MOD_BSP_I2S_MASTER, 11)  /**< Output FIFO overrun .*/
#define __QAPI_I2S_ERR_PLATFORM_INIT_FAIL            __QAPI_ERROR(QAPI_MOD_BSP_I2S_MASTER, 12)  /**< Platform initialization failure. */
#define __QAPI_I2S_ERR_PLATFORM_DEINIT_FAIL          __QAPI_ERROR(QAPI_MOD_BSP_I2S_MASTER, 13)  /**< Platform deinitialization failure. */
#define __QAPI_I2S_ERR_PLATFORM_REG_INTR_FAIL        __QAPI_ERROR(QAPI_MOD_BSP_I2S_MASTER, 14)  /**< Platform interrupt register failure. */
#define __QAPI_I2S_ERR_PLATFORM_DEREG_INTR_FAIL      __QAPI_ERROR(QAPI_MOD_BSP_I2S_MASTER, 15)  /**< Platform interrupt deregister failure. */
#define __QAPI_I2S_ERR_PLATFORM_CLOCK_ENABLE_FAIL    __QAPI_ERROR(QAPI_MOD_BSP_I2S_MASTER, 16)  /**< Platform clock enable failure .*/
#define __QAPI_I2S_ERR_PLATFORM_GPIO_ENABLE_FAIL     __QAPI_ERROR(QAPI_MOD_BSP_I2S_MASTER, 17)  /**< Platform GPIO enable failure. */
#define __QAPI_I2S_ERR_PLATFORM_CLOCK_DISABLE_FAIL   __QAPI_ERROR(QAPI_MOD_BSP_I2S_MASTER, 18)  /**< Platform clock disable failure. */
#define __QAPI_I2S_ERR_PLATFORM_GPIO_DISABLE_FAIL    __QAPI_ERROR(QAPI_MOD_BSP_I2S_MASTER, 19)  /**< Platform GPIO disable failure. */
#define __QAPI_I2S_DATA_RECEIVED_DONE                __QAPI_ERROR(QAPI_MOD_BSP_I2S_MASTER, 20)  /**< Receive operation completed. */

/** Enumeration of the instances. */
typedef enum
{
    QAPI_I2S_INSTANCE_001_E = 0,      /**< Instance 01. */
    QAPI_I2S_INSTANCE_002_E = 1,      /**< Instance 02 (unsupported). */
    QAPI_I2S_INSTANCE_003_E = 2,      /**< Instance 03 (unsupported). */
    QAPI_I2S_INSTANCE_004_E = 3,      /**< Instance 04. */

    //removed
} qapi_I2S_Instance_e;

#define __QAPI_I2S_DATA_WORD_SIZE_8_BITS       8   /**< PCM data word is 8 bits. */
#define __QAPI_I2S_DATA_WORD_SIZE_16_BITS      16  /**< PCM data word is 16 bits. */
#define __QAPI_I2S_DATA_WORD_SIZE_24_BITS      24  /**< PCM data word is 24 bits. */
#define __QAPI_I2S_DATA_WORD_SIZE_32_BITS      32  /**< PCM data word is 32 bits. */

#define __QAPI_I2S_WORD_SIZE_16_BITS            0  /**< I2S word is 16 bits. */
#define __QAPI_I2S_WORD_SIZE_32_BITS            1  /**< I2S word is 32 bits. */

#define __QAPI_I2S_MIC_WORD_SIZE_16_BITS        0  /**< Microphone input word is 16 bits. */
#define __QAPI_I2S_MIC_WORD_SIZE_32_BITS        1  /**< Microphone input word is 32 bits. */

#define __QAPI_I2S_MODE_MASTER              0x01   /**< Bit 0: 1 -- master; 0 -- slave. */
#define __QAPI_I2S_MODE_STEREO              0x02   /**< Bit 1: 1 -- stereo; 0 -- mono. */
#define __QAPI_I2S_MODE_DIR_OUTPUT          0x04   /**< Bit 2; 1 -- output; 0 -- input. */

/** @} */ /* end_addtogroup qapi_i2s */

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

/** @addtogroup qapi_i2s
@{ */

/** Enumeration of the clock source. */
typedef enum {	
    QAPI_I2S_CLK_SRC_PLL_E = 0,   /**< Select PLL clock source. */
    QAPI_I2S_CLK_SRC_CPU_E = 1    /**< Select CPU clock source. */
} qapi_I2S_Clk_Src_e;


/** I2S channel configuration: \n
 @code
 * Used to configure I2S channel parameters when qapi_I2S_Init is called.
 * For example
 *
 * qapi_I2S_Handle  hdI2S; 
 * qapi_I2S_Instance_e inst; 
 * qapi_I2S_Channel_Config_t i2s_port_config = {
 *  num_Rx_Desc  = 2
 *  num_Tx_Desc  = 2
 *  buf_Size = 64
 *  freq  = QAPI_I2S_FREQ_44_1_KHZ_E
 *  data_Word_Size = 16
 *  i2s_Word_Size = 16
 *  mic_Word_Size = 16
 *  mode  = __QAPI_I2S_MODE_MASTER;
 *  port  = 0 
 *  clk_Src_Cfg = QAPI_I2S_CLK_SRC_PLL_E;
 * };
 *  inst = QAPI_I2S_INSTANCE_004_E;
 *  status = qapi_I2S_Init(inst, &i2s_port_config, (void **)&hdI2S);
 @endcode
 */
typedef struct // qapi_I2S_Channel_Config_s 
{
    uint32_t  num_Rx_Desc;          /**< Receive DMA descriptors count. */
    uint32_t  num_Tx_Desc;          /**< Send DMA descriptors count. */
    uint32_t  buf_Size;             /**< Data buffer size. */
    uint32_t  freq;                 /**< Transfer frequency, e.g., 32 KHz, 44.1 KHz, etc. */
    uint8_t   data_Word_Size;       /**< PCM word size; 0-8 bits; 1-16 bits; 2-24 bits; 3-32 bits. */
    uint8_t   i2s_Word_Size;        /**< I2S word size; 0-16 bits, 1-32 bits, etc. */
    uint8_t   mic_Word_Size;        /**< Microphone input word size; 0-16 bits, 1-32 bits. */
    uint8_t   mode;                 /**< Work mode master/stereo/output. */
    uint8_t   port;                 /**< Channel number 0, 1, 2, 3. */
    qapi_I2S_Clk_Src_e clk_Src_Cfg; /**< Select clock source. */
} qapi_I2S_Channel_Config_t;


/** Enumeration of I2S frequencies. */
typedef enum {
    QAPI_I2S_FREQ_8_KHZ_E = 1,     /**< 8 KHz audio frequency. */
    QAPI_I2S_FREQ_16_KHZ_E = 2,    /**< 16 KHz audio frequency. */
    QAPI_I2S_FREQ_32_KHZ_E = 3,    /**< 32 KHz audio frequency. */
    QAPI_I2S_FREQ_44_1_KHZ_E = 4,  /**< 44.1 KHz audio frequency. */
    QAPI_I2S_FREQ_48_KHZ_E = 5,    /**< 48 KHz audio frequency. */
    QAPI_I2S_FREQ_96_KHZ_E = 6,    /**< 96 KHz audio frequency. */
    QAPI_I2S_FREQ_MAX = QAPI_I2S_FREQ_96_KHZ_E,
} qapi_I2S_Freq_e;

/** Enumeration of audio states. */
typedef enum {
    QAPI_I2S_AUDIO_START_E = 1,     /**< Start audio. */
    QAPI_I2S_AUDIO_STOP_E = 2,      /**< Stop audio. */
    QAPI_I2S_AUDIO_PAUSE_E = 3,     /**< Pause audio. */
    QAPI_I2S_AUDIO_RESUME_E = 4,    /**< Resume audio. */
} qapi_I2S_Control_e;

/** Enumeration of audio sound source. */
typedef enum {
    QAPI_I2S_AUDIO_SOUND_LEFT_E = 1,   /**< Left sound channel. */
    QAPI_I2S_AUDIO_SOUND_RIGHT_E = 2,  /**< Right sound channel. */
    QAPI_I2S_AUDIO_SOUND_BOTH_E = 3,   /**< Left and right sound channels. */
} qapi_I2S_Sound_Channel_e;

/** @} */ /* end_addtogroup qapi_i2s */

typedef void  *qapi_I2S_Handle;

/** @addtogroup qapi_i2s
@{ */

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
/**
*  Initializes I2S context. If i2s_Config is NULL, the default configuration will be used.
*  qapi_I2S_Configure() can be called to set the default configuration.
*
@code
*  default configuration value
*   num_Rx_Desc  = 2
*   num_Tx_Desc  = 2
*   buf_Size = 64
*   freq  = QAPI_I2S_FREQ_44_1_KHZ_E
*   data_Word_Size = 16
*   i2s_Word_Size = 16
*   mic_Word_Size = 16
*   mode  = __QAPI_I2S_MODE_MASTER;
*   port  = 0 
*   clk_Src_Cfg = QAPI_I2S_CLK_SRC_PLL_E;
@endcode
*
*  @param[in] inst         i2s instance.
*  @param[in] i2s_Config   i2s configuration.
*  @param[out] i2s_Handle  return I2S context pointer.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*
*/
qapi_Status_t
qapi_I2S_Init(
    qapi_I2S_Instance_e inst, 
    qapi_I2S_Channel_Config_t *i2s_Config, 
    qapi_I2S_Handle *i2s_Handle
);

/**
*  Deinitializes I2S context.
*
*  @param[in] i2s_Handle   I2S context pointer.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*/
qapi_Status_t
qapi_I2S_Deinit (
    qapi_I2S_Handle i2s_Handle
);

/**
*  Opens an I2S channel.
*
*  @param[in] i2s_Handle   I2S context pointer.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*/
qapi_Status_t
qapi_I2S_Open
(
    qapi_I2S_Handle   i2s_Handle
);

/**
*  Closes an I2S channel.
*
*  @param[in] i2s_Handle   I2S context pointer.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*/
qapi_Status_t
qapi_I2S_Close
(
    qapi_I2S_Handle i2s_Handle
);

/**
*  Configures an I2S channel default configuration.
*
*  @param[in] inst         I2S instance.
*  @param[in] i2s_Config   I2S configuration pointer.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*/
qapi_Status_t  
qapi_I2S_Configure
(
    qapi_I2S_Instance_e inst, 
    qapi_I2S_Channel_Config_t *i2s_Config
);

/**
*  Registers a callback function.
*
*  @param[in] i2s_Handle   I2S context pointer.
*  @param[in] isr_Cb       Callback function.
*  @param[in] param        Callback function's parameter pointer.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*/
qapi_Status_t
qapi_I2S_Intr_Register (
    qapi_I2S_Handle  i2s_Handle, 
    void (*isr_Cb)(qapi_I2S_Handle i2s_Handle, uint32_t status, void *param), 
    void *param
);

/**
*  Deregisters a callback function.
*
*  @param[in] i2s_Handle   I2S context pointer.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*/
qapi_Status_t
qapi_I2S_Intr_Deregister (
    qapi_I2S_Handle i2s_Handle
);

/**
*  Sends data on the I2S channel.
*
*  @param[in] i2s_Handle   I2S context pointer.
*  @param[in] data         Data buffer pointer.
*  @param[in] data_Len_In_Bytes  Data length.
*  @param[in] bytes_Sent         Actual data length that was sent.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*/
qapi_Status_t 
qapi_I2S_Send_Data(
    qapi_I2S_Handle  i2s_Handle, 
    uint8_t *data, 
    uint32_t data_Len_In_Bytes,
    uint32_t *bytes_Sent);

/**
*  Receives data on the I2S channel.
*
*  @param[in] i2s_Handle   I2S context pointer.
*  @param[in] data         Data buffer pointer.
*  @param[in] buf_Len_In_Bytes  Buffer size.
*  @param[in] rcv_Len_In_Bytes  Data length that has been received.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*/
qapi_Status_t 
qapi_I2S_Receive_Data(
    qapi_I2S_Handle  i2s_Handle, 
    uint8_t *data, 
    uint32_t buf_Len_In_Bytes, 
    uint32_t *rcv_Len_In_Bytes
);

/** 
 *  Kick-off audio input and output at the same time. 
 * 
 *  @param[in] i2s_Handle_Out        I2S sent context pointer. 
 *  @param[in] audio_Snd_Data        Sent data buffer pointer. 
 *  @param[in] data_Len_In_Bytes     Sent data length. 
 *  @param[out] bytes_Sent           Data length that was actually sent. 
 *  @param[in] i2s_Handle_In         I2S receive context pointer. 
 *  @param[in] audio_Rcv_Data        Received data buffer pointer. 
 *  @param[in] buf_Len_In_Bytes      Received data length. 
 *  @param[out] rcv_Len_In_Bytes     Received data length that was actually sent. 
 * 
 *  @return 
 *  QAPI_OK                -- Call succeeded. \n 
 *  QAPI_ERROR             -- Call failed. \n 
 *  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
 */ 
qapi_Status_t 
qapi_I2S_BiDir(
    qapi_I2S_Handle  i2s_Handle_Out, 
    uint8_t *audio_Snd_Data, 
    uint32_t data_Len_In_Bytes, 
    uint32_t *bytes_Sent,
    qapi_I2S_Handle  i2s_Handle_In, 
    uint8_t *audio_Rcv_Data, 
    uint32_t buf_Len_In_Bytes, 
    uint32_t *rcv_Len_In_Bytes
);

/**
*  Controls the send/receive function.
*
*  @param[in] i2s_Handle   I2S context pointer.
*  @param[in] control      Control action. See #qapi_I2S_Control_e.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*/
qapi_Status_t 
qapi_I2S_Audio_Control(
    qapi_I2S_Handle i2s_Handle, 
    qapi_I2S_Control_e control
);

/**
*  Sets the audio volume.
*
*  @param[in] i2s_Handle   I2S context pointer.
*  @param[in] sound_Channel  Left, right channel 
*  @param[in] volume       Volume; value range: 0 to 7.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*/
qapi_Status_t 
qapi_I2S_Audio_Volume_Set(
    qapi_I2S_Handle  i2s_Handle,
    qapi_I2S_Sound_Channel_e sound_Channel,
    int32_t  volume
);

/**
*  Gets the I2S data buffer. The buffer size is defined in the configuration.
*
*  @param[out] audio_Data_Buffer  Pointer to the buffer that contains the 
*                                 audio data. 
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. 
*/
qapi_Status_t
qapi_I2S_Get_Buffer(
    uint8_t **audio_Data_Buffer
);

/**
*  Releases the I2S data buffer.
*
*  @param[in] audio_Data_Buffer    Buffer to release. 
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. 
*/
qapi_Status_t
qapi_I2S_Release_Buffer(
	uint8_t *audio_Data_Buffer
);

/**
 *  Starts audio input and output simultaneously. 
 * 
 *  @param[in] i2s_Handle_Out        I2S sent context pointer. 
 *  @param[in] audio_Snd_Data_Array  Sent data buffer array. 
 *  @param[in] snd_Count_In_Packets  Sent data buffer count. 
 *  @param[in] i2s_Handle_In         I2S receive context pointer. 
 *  @param[in] audio_Rcv_Data_Array  Received data buffer array. 
 *  @param[in] rcv_Count_In_Packets  Received data buffer count. 
 * 
 *  @return 
 *  QAPI_OK                -- Call succeeded. \n 
 *  QAPI_ERROR             -- Call failed. \n 
 *  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. 
 */
qapi_Status_t 
qapi_I2S_Send_Receive(
	qapi_I2S_Handle  i2s_Handle_Out, 
	uint8_t *audio_Snd_Data_Array[], 
	uint32_t snd_Count_In_Packets, 
    qapi_I2S_Handle  i2s_Handle_In, 
	uint8_t *audio_Rcv_Data_Array[], 
	uint32_t rcv_Count_In_Packets
);

/** @} */ /* end_addtogroup qapi_i2s */

#endif // ifndef __QAPI_I2S_H__
