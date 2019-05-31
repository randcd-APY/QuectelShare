/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_PCM_H__
#define __QAPI_PCM_H__
/** 
    @file  qapi_pcm.h
    @brief PCM APIs
 */
// $QTI_LICENSE_QDN_C$


/*=============================================================================

                              EDIT HISTORY 

  $Header: 

when         who     what, where, why
----------   ---     ---------------------------------------------------------- 
2017-02-12   leo     (Tech Comm) Edited/added Doxygen comments and markup.
2016-10-27   mb      Initial creation


======================================================================*/

#include    "stdint.h"
#include    "qapi_i2s.h"

/** @addtogroup qapi_pcm
@{ */

/*
 *  16 16-bits slots, or 8-bits slot1~ slot16
 */
#define __QAPI_PCM_SLOT_00             0x00000001    /**< PCM slot 1 mask. */
#define __QAPI_PCM_SLOT_01             0x00000002    /**< PCM slot 2 mask. */
#define __QAPI_PCM_SLOT_02             0x00000004    /**< PCM slot 3 mask. */
#define __QAPI_PCM_SLOT_03             0x00000008    /**< PCM slot 4 mask. */
#define __QAPI_PCM_SLOT_04             0x00000010    /**< PCM slot 5 mask. */
#define __QAPI_PCM_SLOT_05             0x00000020    /**< PCM slot 6 mask. */
#define __QAPI_PCM_SLOT_06             0x00000040    /**< PCM slot 7 mask. */
#define __QAPI_PCM_SLOT_07             0x00000080    /**< PCM slot 8 mask. */
#define __QAPI_PCM_SLOT_08             0x00000100    /**< PCM slot 9 mask. */
#define __QAPI_PCM_SLOT_09             0x00000200    /**< PCM slot 10 mask. */
#define __QAPI_PCM_SLOT_10             0x00000400    /**< PCM slot 11 mask. */
#define __QAPI_PCM_SLOT_11             0x00000800    /**< PCM slot 12 mask. */
#define __QAPI_PCM_SLOT_12             0x00001000    /**< PCM slot 13 mask. */
#define __QAPI_PCM_SLOT_13             0x00002000    /**< PCM slot 14 mask. */
#define __QAPI_PCM_SLOT_14             0x00004000    /**< PCM slot 15 mask. */
#define __QAPI_PCM_SLOT_15             0x00008000    /**< PCM slot 16 mask. */
/*
 * slot mode = 0, only 8-bits slot17~slot32
 */
#define __QAPI_PCM_SLOT_16             0x00010000    /**< PCM slot 17 mask. */
#define __QAPI_PCM_SLOT_17             0x00020000    /**< PCM slot 18 mask. */
#define __QAPI_PCM_SLOT_18             0x00040000    /**< PCM slot 19 mask. */
#define __QAPI_PCM_SLOT_19             0x00080000    /**< PCM slot 20 mask. */
#define __QAPI_PCM_SLOT_20             0x00100000    /**< PCM slot 21 mask. */
#define __QAPI_PCM_SLOT_21             0x00200000    /**< PCM slot 22 mask. */
#define __QAPI_PCM_SLOT_22             0x00400000    /**< PCM slot 23 mask. */
#define __QAPI_PCM_SLOT_23             0x00800000    /**< PCM slot 24 mask. */
#define __QAPI_PCM_SLOT_24             0x01000000    /**< PCM slot 25 mask. */
#define __QAPI_PCM_SLOT_25             0x02000000    /**< PCM slot 26 mask. */
#define __QAPI_PCM_SLOT_26             0x04000000    /**< PCM slot 27 mask. */
#define __QAPI_PCM_SLOT_27             0x08000000    /**< PCM slot 28 mask. */
#define __QAPI_PCM_SLOT_28             0x10000000    /**< PCM slot 29 mask. */
#define __QAPI_PCM_SLOT_29             0x20000000    /**< PCM slot 30 mask. */
#define __QAPI_PCM_SLOT_30             0x40000000    /**< PCM slot 31 mask. */
#define __QAPI_PCM_SLOT_31             0x80000000    /**< PCM slot 32 mask. */

#define __QAPI_PCM_MODE_DIR_OUTPUT         __QAPI_I2S_MODE_DIR_OUTPUT

/**
 * RX to TX or TX to RX loop enable/disable.
 */
typedef enum {
 QAPI_PCM_LOOP_NO_E = 0,                  /**< PCM loop disable. */
 QAPI_PCM_LOOP_YES_E = 1,                 /**< PCM loop enable. */
} qapi_PCM_Loop_e;

/** PCM mode. */
typedef enum {
 QAPI_PCM_MODE_DMA_E = 0,                 /**< PCM DMA mode. */
 QAPI_PCM_MODE_CPU_E = 1,                 /**< PCM CPU mode. */
} qapi_PCM_Mode_e;

/** PCM gate clock on/off. */
typedef enum {
 QAPI_PCM_GATE_CLK_OFF_E = 0,             /**< PCM gate clock is off. */
 QAPI_PCM_GATE_CLK_ON_E = 1,              /**< PCM gate clock is on. */
} qapi_PCM_Gate_Clk_e;

/** PCM frame synchronous length. */
typedef enum {
 QAPI_PCM_FRAME_SYNC_ONE_CLK_E = 0,       /**< PCM frame synchronous length is one PCM clock cycle. */
 QAPI_PCM_FRAME_SYNC_ONE_SLOT_E = 1,      /**< PCM frame synchronous length is one PCM slot cycle. */
} qapi_PCM_Frame_Sync_e;

/** PCM clock mode. */
typedef enum {
 QAPI_PCM_CLK_MODE_SINGLE_E = 0,          /**< PCM clock is in Single Clock mode. */
 QAPI_PCM_CLK_MODE_DOUBLE_E = 1,          /**< PCM clock is in Double Clock mode. */
} qapi_PCM_Clk_Mode_e;

/** PCM slot mode. */
typedef enum {
 QAPI_PCM_SLOT_MODE_8BITS_E = 0,          /**< Each PCM slot is 8-bit data. */
 QAPI_PCM_SLOT_MODE_16BITS_E = 1,         /**< Each PCM slot is 16-bit data. */
} qapi_PCM_Slot_Mode_e;

/** Clock in which to sample data. */
typedef enum {
 QAPI_PCM_DCLK_SAMPLE_FIRST_E = 0,        /**< Sample data in the first clock. */
 QAPI_PCM_DCLK_SAMPLE_SECOND_E = 1,       /**< Sample data in the second clock. */
} qapi_PCM_Dclk_Sample_e;

/** PCM Tx/Rx phase. */
typedef enum {
 QAPI_PCM_TXRX_PHASE_NEGATIVE_E     = 0,  /**< PCM module transmit/receive data at negative edge of PCM clock. */
 QAPI_PCM_TXRX_PHASE_NEGATIVE_90_E  = 1,  /**< PCM module transmit/receive data at 90 degree phase shift negative edge of PCM clock. */
 QAPI_PCM_TXRX_PHASE_POSITIVE_E     = 2,  /**< PCM module transmit/receive data at positive edge of PCM clock. */
 QAPI_PCM_TXRX_PHASE_POSITIVE_90_E  = 3,  /**< PCM module transmit/receive data at 90 degree phase shift positive edge of PCM clock. */
} qapi_PCM_Txrx_Phase_e;

/**
 * Used to configure PCM channel parameters when qapi_PCM_Init is called. \n
 * For example:
 *
 @code
 * qapi_I2S_Handle  pcm_Handle;
 * qapi_PCM_Config_t pcm_config = {
 *   __QAPI_PCM_MODE_DIR_OUTPUT,
 *   PCM_FREQUENCY,
 *   __QAPI_PCM_SLOT_00,
 *   QAPI_PCM_MODE_DMA_E,
 *   QAPI_PCM_CLK_MODE_SINGLE_E,
 *   QAPI_PCM_SLOT_MODE_16BITS_E,
 *   QAPI_PCM_FRAME_SYNC_ONE_CLK_E,
 *   QAPI_PCM_GATE_CLK_ON_E,
 *   QAPI_PCM_TXRX_PHASE_POSITIVE_E,
 *   QAPI_PCM_TXRX_PHASE_POSITIVE_E,
 *   0,
 *   0,
 *   160,
 *   160,
 *   0,
 *   0,
 *   6,
 *   0,
 *   64,
 * };
 * status = qapi_PCM_Init(&pcm_Config, &pcm_Handle);
 *
 @endcode
 */
typedef  struct // qapi_PCM_Config_s 
{
  uint32_t   mode;                      /**< Operating mode. */
  uint32_t   freq;                      /**< Transfer frequency (8 KHz, 16 KHz, 32 KHz, etc.). */
  uint32_t   slots;                     /**< PCM slots to be used. */
  qapi_PCM_Mode_e  cpu_Mode;            /**< PCM work mode. */
  qapi_PCM_Clk_Mode_e  clk_Mode;        /**< PCM clock mode. */
  qapi_PCM_Slot_Mode_e  slot_Mode;      /**< PCM slot mode. */
  qapi_PCM_Frame_Sync_e frame_Sync_Len; /**< PCM frame synchronous signal length. */
  qapi_PCM_Gate_Clk_e  gate_Clk_En;     /**< PCM gate clock enable/disable. */	
  qapi_PCM_Txrx_Phase_e   rx_Phase;     /**< PCM receive clock phase. */
  qapi_PCM_Txrx_Phase_e   tx_Phase;     /**< PCM transmit clock phase. */
  qapi_PCM_Loop_e   loop_Rx2tx;         /**< PCM loop from RX to TX. */
  qapi_PCM_Loop_e   loop_Tx2rx;         /**< PCM loop from TX to RX. */
  uint16_t   rx_Threshold;   /**< PCM receive data FIFO threshold. \n Maximum size is 64 bytes, which is used for CPU mode. */
  uint16_t   tx_Threshold;   /**< PCM transmit data FIFO threshold. \n Maximum size is 64 bytes, which is used for CPU mode. */
  uint16_t   pcm_Input_Offset;   /**< PCM input offset. */
  uint16_t   pcm_Output_Offset;  /**< PCM output offset. */
  uint16_t   num_Tx_Desc;   /**< PCM Tx descriptor number: \n num_Tx_Desc * pcm_Buf_Size = buffer bytes that are used in PCM transmit. */
  uint16_t   num_Rx_Desc;   /**< PCM Rx descriptor number: \n num_Rx_Desc * pcm_Buf_Size = buffer bytes that are used in PCM receive. */
    uint16_t   pcm_Buf_Size;   /**< PCM data buffer size: \n pcm_Buf_Size must align in a 4-byte boundary (e.g., 64, 128, 256). */
} qapi_PCM_Config_t;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
/**
*  Initializes the PCM context.
*
*  @param[in] pcm_Config   PCM configuration.
*  @param[out] pcm_Handle  Returned PCM context pointer.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
*/
 
qapi_Status_t
qapi_PCM_Init(
    qapi_PCM_Config_t *pcm_Config, 
	qapi_I2S_Handle *pcm_Handle
);

/**
*  Opens a PCM channel.
*
*  @param[in] pcm_Handle   PCM context pointer.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
*/
qapi_Status_t
qapi_PCM_Open(
	qapi_I2S_Handle pcm_Handle
);

#define	qapi_PCM_Deinit(handle) qapi_I2S_Deinit(handle)
  /**< Defines the qapi_PCM_Deinit macro. */
#define	qapi_PCM_Close(handle) qapi_I2S_Close(handle)
  /**< Defines the qapi_PCM_Close macro. */
#define	qapi_PCM_Intr_Register(handle,isr_cb,param) qapi_I2S_Intr_Register((handle),(isr_cb),(param))
  /**< Defines the qapi_PCM_Intr_Register macro. */
#define	qapi_PCM_Intr_Deregister(Handle) qapi_I2S_Intr_Deregister (Handle)
  /**< Defines the qapi_PCM_Intr_Deregister macro. */
#define	qapi_PCM_Send_Data(handle,data,data_len,len) qapi_I2S_Send_Data((handle),(data),(data_len),(len))
  /**< Defines the qapi_PCM_Send_Data macro. */
#define	qapi_PCM_Receive_Data(Handle,data,buf_Len_In_Bytes,rcv_Len_In_Bytes) \
                    qapi_I2S_Receive_Data((Handle),(data),(buf_Len_In_Bytes),(rcv_Len_In_Bytes))
  /**< Defines the qapi_PCM_Receive_Data macro. */
#define	qapi_PCM_BiDir(Handle_Out,audio_Snd_Data,data_Len_In_Bytes,bytes_Sent,Handle_In,audio_Rcv_Data,buf_Len_In_Bytes,rcv_Len_In_Bytes) \
                    qapi_I2S_BiDir(Handle_Out,audio_Snd_Data,data_Len_In_Bytes, bytes_Sent,Handle_In, audio_Rcv_Data, buf_Len_In_Bytes,rcv_Len_In_Bytes)
  /**< Defines the qapi_PCM_BiDir macro. */
#define	qapi_PCM_Send_Receive(i2s_Handle_Out, audio_Snd_Data_Array, snd_Count_In_Packets,i2s_Handle_In,audio_Rcv_Data_Array,rcv_Count_In_Packets) \
            qapi_I2S_Send_Receive(i2s_Handle_Out, audio_Snd_Data_Array, snd_Count_In_Packets,i2s_Handle_In,audio_Rcv_Data_Array,rcv_Count_In_Packets)
  /**< Defines the qapi_PCM_Send_Receive macro. */
/** @} */ /** end_addtogroup qapi_pcm */

#endif
