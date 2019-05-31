#ifndef __QAPI_ADC_H__
#define __QAPI_ADC_H__

/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_adc.h
 *
 * @brief Analog-to-Digital Converter (ADC)
 *
 * @addtogroup qapi_adc
 * @{
 *
 * An analog-to-digital converter (ADC) allows an analog signal to be
 * sampled and digitally represented. The SoC features an on-die
 * ADC that supports reading multiple channels. Both single-ended and
 * differential inputs can be measured. The ADC can perform single-
 * shot and recurring measurements. ADC samples are stored in an
 * internal FIFO and can be popped out with or without DMA. Callbacks
 * can be registered for amplitude thresholds and the FIFO sample count
 * threshold.
 *
 * ADC is configurable via run-time and static parameters. Please
 * see the ADC tunable board file for the statically defined
 * parameters.
 *
 * This programming interface allows client software to configure
 * channels, perform single readings, perform recurring readings,
 * set threshold, and get ADC data samples. The code snippet below
 * shows an example usage. Consult the ADC tunable board file
 * for the configurable settings that are run-time set.
 *
 * @code {.c}
 *
 *
 *  * The code snippet below demonstrates use of this interface. The example
 *  * below opens ADC to obtain a handle, gets the number of channels, reads
 *  * each ADC channel, and then closes the handle.
 *
 *   qapi_Status_t status;
 *   qapi_ADC_Handle_t handle;
 *   uint32_t num_channels;
 *   uint32_t channel;
 *   qapi_ADC_Read_Result_t result;
 *
 *   status = qapi_ADC_Open(&handle, QAPI_ADC_ATTRIBUTE_NONE);
 *   if (status != QAPI_OK) { ... }
 *
 *   status = qapi_ADC_Get_Num_Channels(handle, &num_channels);
 *   if (status != QAPI_OK) { ... }
 *
 *   for (channel = 0; channel < num_channels; channel++)
 *   {
 *      status = qapi_ADC_Read_Channel(handle, channel, &result);
 *      if (status != QAPI_OK) { ... }
 *
 *      // result.microvolts contains the reading
 *   }
 *
 *   status = qapi_ADC_Close(handle, false);
 *   if (status != QAPI_OK) { ... }
 *   handle = NULL;
 *
 * @endcode
 *
 * @}
 */

/*==================================================================================

                               EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.ioe/1.0/v2/rom/release/api/hwengines/adc/qapi_adc.h#7 $

  when        who  what, where, why
  ----------  ---  ---------------------------------------------------------------
  2017-02-02  jjo  Added support for getting buffered data in units of ADC code.
  2016-05-06  leo  (TechComm) Edited/added Doxygen comments and markup.
  2016-01-07  jjo  Updated comments.
  2015-12-11  jjo  Initial version.

==================================================================================*/

/** @addtogroup qapi_adc
@{ */

/*----------------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------------*/

/**
* ADC open attributes, which can be ORed together to specify overall attributes when
* opening a handle to ADC using qapi_ADC_Open().
*/
#define QAPI_ADC_ATTRIBUTE_NONE 0       /**< No attributes. */
#define QAPI_ADC_ATTRIBUTE_BUFFERING 1  /**< Open the ADC for buffering data/thresholds. */

/**
* ADC flags, which are ORed together to provide more details when getting data.
*/
#define QAPI_ADC_FLAG_NONE 0             /**< No flags. */
#define QAPI_ADC_FLAG_MORE_DATA 1        /**< More data is available. */
#define QAPI_ADC_FLAG_FIFO_OVERFLOWED 2  /**< The FIFO overflowed. */
#define QAPI_ADC_FLAG_DMA_ERROR 4        /**< A DMA error occurred. */

/** @} */ /* end_addtogroup qapi_adc */

/*----------------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------------*/

/** @addtogroup qapi_adc
@{ */

/**
* ADC power modes are configured for different power modes using qapi_ADC_Set_Config().
*/
typedef enum
{
   QAPI_ADC_POWER_MODE_LOW_E,     /**< Low power mode. */
   QAPI_ADC_POWER_MODE_NORMAL_E,  /**< Normal power mode. */
   QAPI_ADC_POWER_MODE_TURBO_E,   /**< Turbo power mode. */
   QAPI_ADC_POWER_MODE_INVALID_E = 0x7fffffff  /**< Reserved. */
} qapi_ADC_Power_Mode_t;

/**
* ADC amplitude threshold types that can be configured to be
*          monitored using qapi_ADC_Set_Amp_Threshold().
*/
typedef enum
{
   QAPI_ADC_AMP_THRESHOLD_LOWER_E,   /**< Lower threshold. */
   QAPI_ADC_AMP_THRESHOLD_HIGHER_E,  /**< Higher threshold. */
   QAPI_ADC_AMP_THRESHOLD_INVALID_E = 0x7fffffff  /* Force enum to 32-bit */
} qapi_ADC_Amp_Threshold_t;

/**
* ADC conversion states.
*/
typedef enum
{
   QAPI_ADC_CONV_STATE_IDLE_E,         /**< ADC is in Idle/off. */
   QAPI_ADC_CONV_STATE_SINGLE_SHOT_E,  /**< Single-shot mode. */
   QAPI_ADC_CONV_STATE_CONTINUOUS_E,   /**< Continious mode. */
   QAPI_ADC_CONV_STATE_INVALID_E = 0x7fffffff  /**< Reserved. */
} qapi_ADC_Conv_State_t;

/**
* ADC read result. This structure is returned as the result of an ADC single-shot
*          reading.
*/
typedef struct
{
   int32_t microvolts;  /**< ADC result in microvolts. */
   uint32_t code;       /**< ADC result in raw code. */
} qapi_ADC_Read_Result_t;

/**
* Structure that contains the configurable ADC parameters.
*/
typedef struct
{
   uint32_t chan_en_mask;  /**< Mask of which channels to enable. */
   uint32_t period_us;     /**< Continious mode measurement period in microseconds. */
   qapi_ADC_Power_Mode_t power_mode;  /**< Power mode. */
} qapi_ADC_Config_t;

/**
* ADC range structure.
*/
typedef struct
{
   int32_t min_uv;  /**< Minimum value in microvolts. */
   int32_t max_uv;  /**< Maximum value in microvolts. */
} qapi_ADC_Range_t;

/**
* ADC amplitude threshold result structure.
*/
typedef struct
{
   uint32_t channel;                    /**< Channel that was triggered. */
   qapi_ADC_Amp_Threshold_t threshold;  /**< Threshold that was triggered. */
} qapi_ADC_Amp_Threshold_Result_t;

/**
* Structure that contains the status bits for the different ADC wakeable
*          interrupts. Interrupts are set if a threshold event occurred while the ADC
*          was closed but left enabled, e.g., during an operating mode transition.
*/
typedef struct
{
   uint32_t fifo_int_mask;  /**< Bitmask of FIFO threshold interrupt status. */
   uint32_t low_int_mask;   /**< Per channel bitmask of low threshold interrupt status. */
   uint32_t high_int_mask;  /**< Per channel bitmask of high threshold interrupt status. */
} qapi_ADC_Wakeup_Int_Status_t;

/**
* @brief ADC data descriptor used when popping ADC data from
*          the FIFO or when using DMA. Buffers can be provided for
*          getting samples in microvolts and ADC code. One or both
*          buffers can be provided. Buffer addresses must be 4-byte
*          aligned when used in DMA mode.
*/
typedef struct
{
   int32_t *adc_sample_uv;     /**< ADC sample buffer; the samples are in microvolts. */
   uint16_t *adc_sample_code;  /**< ADC sample buffer; the samples are in ADC code. */
   uint32_t channel;           /**< Channel to be used. */
   uint32_t length;            /**< Length of the buffer. */
   uint32_t num_samples;       /**< Number of samples written to the buffer. */
} qapi_ADC_Data_Descriptor_t;

/** @} */ /* end_addtogroup qapi_adc */

/** @addtogroup qapi_adc
@{ */

/**
* ADC client handle used by clients when making calls to the ADC. Clients are
*          responsible for ensuring that they do not lose the handle and for closing
*          the handle when done with the ADC.
*/
typedef void *qapi_ADC_Handle_t;

/**
*  Callback invoked when an amplitude threshold is crossed
*
*  Once the threshold is crossed, it must be re-armed or it will
*           not trigger again.
*
*  @param[in] ctxt    Context specified when setting the threshold.
*  @param[in] result  Threshold crossing result.
*
*  @return
*  None.
*/
typedef void (*qapi_ADC_Amp_Threshold_CB_t)
(
   void *ctxt,
   const qapi_ADC_Amp_Threshold_Result_t *result
);

/**
*  Callback invoked when the FIFO threshold is crossed.
*
*  Once the threshold is crossed, it must be re-armed or it will
*           not trigger again.
*
*  @param[in] ctxt    Context specified when setting the threshold.
*
*  @return
*  None.
*/
typedef void (*qapi_ADC_Fifo_Threshold_CB_t)
(
   void *ctxt
);

/**
*  Callback invoked when an ADC transfer is completed. A descriptor with ADC
*  data samples is provded for each channel.
*
*  @param[in] ctxt         Context specified when queueing the transfer.
*  @param[in] desc         Pointer to an array of descriptors (one per channel).
*  @param[in] num_desc     Number of descriptors.
*  @param[in] transfer_id  ID for the DMA transfer.
*  @param[in] flags        Bitmask of flags with additional information.
*
*  @return
*  None.
*/
typedef void (*qapi_ADC_Transfer_Done_CB_t)
(
   void *ctxt,
   qapi_ADC_Data_Descriptor_t *desc,
   uint32_t num_desc,
   uint32_t transfer_id,
   uint32_t flags
);

/** @} */ /* end_addtogroup qapi_adc */

/*----------------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------------*/

/** @addtogroup qapi_adc
@{ */

/**
*  Opens the ADC for use by a software client.
*
*  When the ADC is opened with attribute QAPI_ADC_ATTRIBUTE_BUFFERING, its power
*  state is set to ON and its registers are configured. Furthermore,
*  no other clients can make ADC readings. Therefore, only keep the
*  ADC open for buffering use when neccesary.
*
*  @note1hang When the ADC is opened with buffering access, no other clients can make
*             ADC readings until it is closed with keep_enabled == false by the
*             buffering client.
*
*  @param[out] handle      Pointer to an ADC handle.
*  @param[in]  attributes  Mask of ADC attributes.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
*  QAPI_ERR_NO_MEMORY     -- No memory available to support this operation. \n
*  QAPI_ERR_NO_RESOURCE   -- No more handles are available.
*/
qapi_Status_t qapi_ADC_Open
(
   qapi_ADC_Handle_t *handle,
   uint32_t attributes
);

/**
*  Closes a handle to the ADC when a software client is done with it.
*
* When called with keep_enabled == false, the ADC hardware is
* powered off. Closing with keep_enabled == true keeps the
* ADC hardware on and performs sampling only if the conversion state is
* Continuous. This allows the ADC to continue sampling during
* operating mode transitions.
*
*  When entering the new operating mode,
*  the interrupt status can be checked to determine if thresholds
*  were violated during operating mode transition. Only a client that
*  has opened the ADC with buffering access can keep the ADC enabled after
*  close.
*
*  @note1hang If the ADC is closed and left enabled, the ADC is locked out
*             until a client opens a handle with buffering access and closes ADC
*             with keep_enabled == false.
*
*  @param[in] handle         Handle provided by qapi_ADC_Open().
*  @param[in] keep_enabled   Whether to keep ADC running after it is closed.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
*  QAPI_ERR_BUSY          -- ADC is busy with an internal operation, e.g.,
*                            servicing an interrupt.
*/
qapi_Status_t qapi_ADC_Close
(
   qapi_ADC_Handle_t handle,
   qbool_t keep_enabled
);

/**
*  Reads an ADC channel.
*
*  This function triggers a single-shot conversion on a selected
*  channel and returns the conversion result.
*
*  If a client has opened the ADC for buffering use, ADC readings
*  will fail and return QAPI_ERR_BUSY until the buffering client
*  closes the ADC.
*
*  Buffering clients cannot perform single-shot conversions. Either
*  a new handle should be opened without the buffering mode attribute
*  or another handle should be created for single-shot readings.
*
*  @note1hang This call first clears the ADC FIFO buffer.
*
*  @param[in]  handle   Handle provided by qapi_ADC_Open().
*  @param[in]  channel  Channel to read.
*  @param[out] result   ADC reading result structure.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
*  QAPI_ERR_BUSY          -- Busy, e.g., the ADC is in buffering mode. \n
*  QAPI_ERR_NOT_SUPPORTED -- Not supported by buffering clients.
*/
qapi_Status_t qapi_ADC_Read_Channel
(
   qapi_ADC_Handle_t handle,
   uint32_t channel,
   qapi_ADC_Read_Result_t *result
);

/**
*  Gets the number of channels.
*
*  This function is used to get the number of ADC channels in the board file.
*
*  The ADC channel index is zero-based and ranges from zero to the
*  number of channels minus one.
*
*  @param[in]  handle         Handle provided by qapi_ADC_Open().
*  @param[out] num_channels   Number of channels.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*/
qapi_Status_t qapi_ADC_Get_Num_Channels
(
   qapi_ADC_Handle_t handle,
   uint32_t *num_channels
);

/**
*  Gets the range of an ADC channel.
*
*  The range is useful when setting thresholds to make sure
*  that the desired threshold is within the channel's range.
*
*  @param[in]  handle    Handle provided by qapi_ADC_Open().
*  @param[in]  channel   Selected channel.
*  @param[out] range     Channel's range.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*/
qapi_Status_t qapi_ADC_Get_Range
(
   qapi_ADC_Handle_t handle,
   uint32_t channel,
   qapi_ADC_Range_t *range
);

/**
*  Recalibrates the ADC.
*
*  This function is used to trigger a manual recalibration of the ADC.
*
*  @param[in] handle   Handle provided by qapi_ADC_Open().
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*/
qapi_Status_t qapi_ADC_Recalibrate
(
   qapi_ADC_Handle_t handle
);

/**
*  Sets the ADC configuration specified
*  in the input configuration structure.
*
*  If this function is not called, a default configuration
*  is used if the ADC is opened while previously powered off.
*  If the ADC is opened while it has been kept enabled, the
*  configuration used in the previous mode is the default.
*
*  @note1hang This function requires buffering access.
*
*  @param[in] handle   Handle provided by qapi_ADC_Open().
*  @param[in] config   Configuration to apply.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
*  QAPI_ERR_NOT_SUPPORTED -- Client was not opened for buffering access. \n
*  QAPI_ERR_BUSY          -- ADC/DMA thresholds are enabled.
*/
qapi_Status_t qapi_ADC_Set_Config
(
   qapi_ADC_Handle_t handle,
   const qapi_ADC_Config_t *config
);

/**
*  Gets the ADC configuration.
*
*  If a configuration has not been set, a default configuration
*  is used if the ADC is opened while previously powered off.
*  If the ADC is opened while it has been kept enabled, the
*  configuration used in the previous mode is the default.
*
*  @note1hang This function requires buffering access.
*
*  @param[in]  handle   Handle provided by qapi_ADC_Open().
*  @param[out] config   ADC configuration.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
*  QAPI_ERR_NOT_SUPPORTED -- Client was not opened for buffering access.
*/
qapi_Status_t qapi_ADC_Get_Config
(
   qapi_ADC_Handle_t handle,
   qapi_ADC_Config_t *config
);

/**
*  Sets an amplitude threshold
*
*  Amplitude thresholds are armed while in Single-shot and Continuous modes.
*  This function may be called again with the same channel and
*  threshold type to update the threshold.
*
*  Lower thresholds cross when current_value < thresh.
*  Upper thresholds cross when current_value > thresh.
*
*  When monitoring thresholds in Continuous mode, the ADC buffer
*  must not be allowed to overflow. It must either be periodically
*  emptied or cleared.
*
*  Once a threshold is crossed, it must be re-armed for it to trigger
*  again.
*
*  @note1hang This function requires buffering access.
*
*  @param[in] handle       Handle provided by qapi_ADC_Open().
*  @param[in] channel      Selected channel.
*  @param[in] thresh_type  Type of amplitude threshold.
*  @param[in] thresh_uv    Threshold in microvolts.
*  @param[in] cb           Function to be called back when the threshold is crossed.
*  @param[in] ctxt         Callback function's context.
*  @param[out] thresh_set_uv  Threshold that was set in the hardware.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
*  QAPI_ERR_NOT_SUPPORTED -- Client was not opened for buffering access.
*/
qapi_Status_t qapi_ADC_Set_Amp_Threshold
(
   qapi_ADC_Handle_t handle,
   uint32_t channel,
   qapi_ADC_Amp_Threshold_t thresh_type,
   int32_t thresh_uv,
   qapi_ADC_Amp_Threshold_CB_t cb,
   void *ctxt,
   int32_t *thresh_set_uv
);

/**
*  @brief Clears an amplitude threshold on a channel.
*
*  @note1hang This function requires buffering access.
*
*  @param[in] handle   Handle provided by qapi_ADC_Open().
*  @param[in] channel  Selected channel.
*  @param[in] thresh_type  Selected threshold.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
*  QAPI_ERR_NOT_SUPPORTED -- Client was not opened for buffering access.
*/
qapi_Status_t qapi_ADC_Clear_Amp_Threshold
(
   qapi_ADC_Handle_t handle,
   uint32_t channel,
   qapi_ADC_Amp_Threshold_t thresh_type
);

/**
*  Sets the FIFO count threshold.
*
*  The threshold is triggered when FIFO count @ge threshold. This threshold is
*  only armed while the ADC is running in Continuous mode. The threshold may be
*  set at any time, however. This function may be called again to update
*  the threshold value.
*
*  Once a threshold is crossed, it must be re-armed for it to trigger again.
*
*  @note1hang This function requires buffering access.
*
*  @param[in] handle     Handle provided by qapi_ADC_Open().
*  @param[in] threshold  FIFO count threshold.
*  @param[in] cb         Function to be called back when the threshold is crossed.
*  @param[in] ctxt       Callback function's context.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
*  QAPI_ERR_NOT_SUPPORTED -- Client was not opened for buffering access.
*/
qapi_Status_t qapi_ADC_Set_Fifo_Threshold
(
   qapi_ADC_Handle_t handle,
   uint32_t threshold,
   qapi_ADC_Fifo_Threshold_CB_t cb,
   void *ctxt
);

/**
*  Clears the FIFO count threshold.
*
*  @note1hang This function requires buffering access.
*
*  @param[in] handle  Handle provided by qapi_ADC_Open().
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
*  QAPI_ERR_NOT_SUPPORTED -- Client was not opened for buffering access.
*/
qapi_Status_t qapi_ADC_Clear_Fifo_Threshold
(
   qapi_ADC_Handle_t handle
);

/**
*  Gets the wakeable interrupt status.
*
*  This is used when a client has closed the ADC and kept it running. In the new
*           operating mode, after ADC open is called, this function can be called
*           to determine if any threshold events occurred in the time between the ADC
*           being closed and re-opened.
*
*  @note1hang This function requires buffering access.
*
*  @param[in]  handle   Handle provided by qapi_ADC_Open().
*  @param[out] status  Interrupt status.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
*  QAPI_ERR_NOT_SUPPORTED -- Client was not opened for buffering access.
*/
qapi_Status_t qapi_ADC_Get_Wakeup_Int_Status
(
   qapi_ADC_Handle_t handle,
   qapi_ADC_Wakeup_Int_Status_t *status
);

/**
*  Sets the ADC conversion state
*
*  To start buffering data, set the conversion state to
*  QAPI_ADC_CONV_STATE_CONTINUOUS_E. To stop buffering data,
*  set the conversion state to QAPI_ADC_CONV_STATE_IDLE_E.
*
*  When starting Continuous mode, the FIFO is cleared first.
*
*  It is enforced that this function should only be used to
*  select between Continuous mode and Idle. For single-shot
*  conversions, the qapi_ADC_Read_Channel() function should
*  be used, otherwise, an error will be returned.
*
*  @note1hang This function requires buffering access.
*
*  @param[in] handle       Handle provided by qapi_ADC_Open().
*  @param[in] conv_state   Selected conversion state.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
*  QAPI_ERR_NOT_SUPPORTED -- Client was not opened for buffering access. \n
*  QAPI_ERR_BUSY          -- ADC conversions are already in progress.
*/
qapi_Status_t qapi_ADC_Set_Conv_State
(
   qapi_ADC_Handle_t handle,
   qapi_ADC_Conv_State_t conv_state
);

/**
*  Gets the conversion state
*
*  @note1hang This function requires buffering access.
*
*  @param[in]  handle       Handle provided by qapi_ADC_Open().
*  @param[out] conv_state   Current conversion state.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
*  QAPI_ERR_NOT_SUPPORTED -- Client was not opened for buffering access.
*/
qapi_Status_t qapi_ADC_Get_Conv_State
(
   qapi_ADC_Handle_t handle,
   qapi_ADC_Conv_State_t *conv_state
);

/**
*  Gets ADC data samples.
*
*  This function is used to get ADC data samples from the internal
*  FIFO, convert them from raw ADC code to physical values, and then
*  store them in a client's buffers. This function returns when
*  either a channel's buffer fills up or no more data is in the ADC
*  internal FIFO.
*
*  A descriptor must be provided for each enabled channel. The number of
*  descriptors specified should be equal to the number of enabled channels
*  or an error is returned.
*
*  This function returns an error if data transfers are queued
*  or in progress.
*
*  @note1hang This function requires buffering access.
*
*  @param[in]     handle       Handle provided by qapi_ADC_Open().
*  @param[in,out] desc         Pointer to an array of descriptors (one per channel).
*  @param[in]     num_desc     Number of descriptors.
*  @param[out]    flags        Bitmask of flags with additional information.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
*  QAPI_ERR_BUSY          -- Data transfers are queued. \n
*  QAPI_ERR_NOT_SUPPORTED -- Client was not opened for buffering access.
*/
qapi_Status_t qapi_ADC_Get_Data
(
   qapi_ADC_Handle_t handle,
   qapi_ADC_Data_Descriptor_t *desc,
   uint32_t num_desc,
   uint32_t *flags
);

/**
*  Queues a data transfer.
*
*  This function is used to get ADC data samples from the internal
*  FIFO, convert them from raw ADC code to physical values, and then
*  store them in a client's buffers. This is a nonblocking call
*  versus qapi_ADC_Get_Data().
*
*  When the data has been transfered to the client's buffer, a
*  callback function is called. At that time, the client can
*  extract the ADC data samples and optionally requeue the descriptor.
*
*  The buffers stop being filled when either a channel's buffer
*  fills up or the ADC conversion state has returned to Idle. If the ADC's
*  conversion state is set to Idle, any remaining transfers after
*  the FIFO has been drained are completed with zero samples. Also,
*  if the ADC configuration changes, the buffers are completed with
*  zero samples (since the descriptors may need to be updated if the enabled
*  channel changes).
*  @newpage
*  A descriptor must be provded for each enabled channel. The number of
*  descriptors specified should be equal the number of enabled channels
*  or an error is returned.
*
*  The ADC driver keeps the RAM banks with the client's buffer enabled
*  while a transfer is in progress and prevents low power modes. Therefore,
*  it is reccomended to let the FIFO fill up to a client-defined threshold
*  and then queue transfers to drain it. To make sure the FIFO does not
*  overflow, it is recommended to queue multiple transfers so the ADC can
*  fill buffers while clients are processing samples in another buffer.
*
*  @note1hang Do not let input desc go out of scope until either 1) the DMA done
*             callback is received, 2) the DMA transfer is canceled, or 3) the ADC
*             has been closed.
*
*  @note1hang This function requires buffering access.
*
*  @param[in] handle     Handle provided by qapi_ADC_Open().
*  @param[in] desc       Pointer to an array of descriptors (one per channel).
*  @param[in] num_desc   Number of descriptors.
*  @param[in] cb         Callback to notify when transfer completes.
*  @param[in] ctxt       Context to pass to the callback.
*  @param[out] transfer_id  ID corresponding to this transfer.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_NO_MEMORY     -- Insufficient memory available. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
*  QAPI_ERR_NOT_SUPPORTED -- Client was not opened for buffering access.
*/
qapi_Status_t qapi_ADC_Queue_Transfer
(
   qapi_ADC_Handle_t handle,
   qapi_ADC_Data_Descriptor_t *desc,
   uint32_t num_desc,
   qapi_ADC_Transfer_Done_CB_t cb,
   void *ctxt,
   uint32_t *transfer_id
);

/**
*  Cancels any outstanding data transfers that were queued.
*
*  @note1hang This function requires buffering access.
*
*  @param[in] handle     Handle provided by qapi_ADC_Open().
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
*  QAPI_ERR_NOT_SUPPORTED -- Client was not opened for buffering access.
*/
qapi_Status_t qapi_ADC_Cancel_Transfers
(
   qapi_ADC_Handle_t handle
);

/**
*  Clears the internal ADC FIFO buffer.
*
*  Clearing the FIFO discards all ADC samples that are
*  currently in the buffer. This call does not clear ADC
*  samples that have already been removed from the FIFO and
*  placed into clients' buffers.
*
*  @note1hang This function requires buffering access.
*
*  @param[in] handle   Handle provided by qapi_ADC_Open().
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
*  QAPI_ERR_NOT_SUPPORTED -- Client was not opened for buffering access. \n
*  QAPI_ERR_BUSY          -- DMA is in progress.
*/
qapi_Status_t qapi_ADC_Clear_Fifo
(
   qapi_ADC_Handle_t handle
);

/** @} */ /* end_addtogroup qapi_adc */

#endif /* #ifndef __QAPI_ADC_H__ */

