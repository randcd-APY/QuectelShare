#ifndef __QAPI_PWM_H__
#define __QAPI_PWM_H__

/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/*=================================================================================
 *
 *                       PULSE WIDTH MODULATION
 *
 *================================================================================*/
 /**
 * @file qapi_pwm.h
 *
 * @addtogroup qapi_pwm
 * @{
 *
 * @brief Pulse Width Modulation (PWM)
 *
 * @details The PWM signal is used to control analog elements in the system via
 *          a digital signal. Common examples of such elements are LEDs and
 *          vibrators. The Qualcomm technologies, Inc. SoC incorporates dedicated hardware to generate an
 *          accurate PWM signal. This hardware typically has multiple channels,
 *          allowing it to generate several PWM signals simultaneously in
 *          various system modes.
 *
 *          This programming interface allows client software to manage PWM
 *          functionality on the SoC. The code snippet below shows an example usage.
 *          Note that the interface requires the client to specify the PWM channel ID.
 *          Consult the hardware schematic or device configuration database
 *          to determine the proper channel ID to use.
 *
 * @code {.c}
 *
 *   * The example below configures channel 2 for a 300 Hz PWM signal with
 *   * an 80% duty and a 20% phase shift using the sleep mode clock
 *   * (enum QAPI_PWM_SOURCE_CLK_SLEEP_MODE_E) as the PWM source clock.
 *   * The code also enables channels 3,4 and 5 simultaneously using the same
 *   * configuration as channel 2 , in order to demonstrate ganged PWM start
 *   *
 *   * For brevity, the sequence assumes that all calls succeed. Clients should
 *   * check the return status from each call.
 *
 *   qapi_Status_t pwm_result = QAPI_OK;
 *   qapi_PWM_Handle_t handleArray[3] = { NULL, NULL, NULL };
 *   qapi_PWM_Config_t config_set;
 *   qapi_PWM_Config_t config_get1, config_get2, config_get3;
 *
 *   config_set.freq = 30000;
 *   config_set.duty = 8000;
 *   config_set.phase = 2000;
 *   config_set.moduleType = 0;
 *   config_set.source_CLK = QAPI_PWM_SOURCE_CLK_SLEEP_MODE_E;
 *
 *
 *   // configure channel 2
 *   pwm_result = qapi_PWM_Channel_Open(QAPI_PWM_CHANNEL_2_E, &handleArray[0]);
 *   pwm_result = qapi_PWM_Channel_Set(handleArray[0], &config_set);
 *   pwm_result = qapi_PWM_Enable(handleArray, 1, 0x01 );
 *   pwm_result = qapi_PWM_Channel_Get(handleArray[0], &config_get1);
 *   pwm_result = qapi_PWM_Enable(handleArray, 1, 0x00);
 *   pwm_result = qapi_PWM_Channel_Close(handleArray[0]);
 *   handleArray[0] = NULL;
 *
 *   // configure channels 3, 4 and 5
 *   pwm_result = qapi_PWM_Channel_Open(QAPI_PWM_CHANNEL_3_E, &handleArray[0]);
 *   pwm_result = qapi_PWM_Channel_Open(QAPI_PWM_CHANNEL_4_E, &handleArray[1]);
 *   pwm_result = qapi_PWM_Channel_Open(QAPI_PWM_CHANNEL_5_E, &handleArray[2]);
 *   pwm_result = qapi_PWM_Channel_Set(handleArray[0], &config_set);
 *   pwm_result = qapi_PWM_Channel_Set(handleArray[1], &config_set);
 *   pwm_result = qapi_PWM_Channel_Set(handleArray[2], &config_set);
 *   pwm_result = qapi_PWM_Enable(handleArray, 3, 0x07 );
 *   pwm_result = qapi_PWM_Channel_Get(handleArray[0], &config_get1);
 *   pwm_result = qapi_PWM_Channel_Get(handleArray[1], &config_get2);
 *   pwm_result = qapi_PWM_Channel_Get(handleArray[2], &config_get3);
 *   pwm_result = qapi_PWM_Enable(handleArray, 3, 0x00);
 *   pwm_result = qapi_PWM_Channel_Close(handleArray[0]);
 *   handleArray[0] = NULL;
 *   pwm_result = qapi_PWM_Channel_Close(handleArray[1]);
 *   handleArray[1] = NULL;
 *   pwm_result = qapi_PWM_Channel_Close(handleArray[2]);
 *   handleArray[2] = NULL;
 *
 * @endcode
 *
 * @}
 */

/*==================================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.ioe/1.0/v2/rom/release/api/iodevices/pwm/qapi_pwm.h#6 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------------
05/06/16   leo     (TechComm) Edited/added Doxygen comments and markup.
01/14/16   ma      Removed qapi_PWM_Init and qapi_PWM_DeInit
12/30/15   ma      Modify qapi_PWM_Enable() to support ganged PWM start
12/14/15   ma      New return value for qapi_PWM_Init()
11/18/15   leo     (TechComm) Edited/added Doxygen comments and markup.
09/30/15   ma      Function prototypes updated
07/29/15   ma      Initial version
==================================================================================*/


/*==================================================================================

                               INCLUDE FILES

==================================================================================*/

/*==================================================================================

                                   MACROS

==================================================================================*/

/*==================================================================================

                               TYPE DEFINITIONS

==================================================================================*/

/** @addtogroup qapi_pwm
@{ */

/**
* PWM channel ID.
*
* This enumeration lists channel IDs that are to be
* used by clients of the PWM driver. This enumeration should not be changed.
*/
typedef enum
{
    QAPI_PWM_CHANNEL_0_E = 0,  /**< PWM channel 0. */
    QAPI_PWM_CHANNEL_1_E,      /**< PWM channel 1. */
    QAPI_PWM_CHANNEL_2_E,      /**< PWM channel 2. */
    QAPI_PWM_CHANNEL_3_E,      /**< PWM channel 3. */
    QAPI_PWM_CHANNEL_4_E,      /**< PWM channel 4. */
    QAPI_PWM_CHANNEL_5_E,      /**< PWM channel 5. */
    QAPI_PWM_CHANNEL_6_E,      /**< PWM channel 6. */
    QAPI_PWM_CHANNEL_7_E,      /**< PWM channel 7. */
    QAPI_PWM_CHANNEL_INVALID_E = 0x7FFFFFFF
} qapi_PWM_Channel_t;


/**
* PWM source clock enum.
*
* The PWM hardware requires an input clock to generate an output signal.
* This enumeration lists the possible options for sourcing an input clock.
* Note that the input clock selection is common to all channels.
*/
typedef enum
{
    QAPI_PWM_SOURCE_CLK_NORMAL_MODE_E = 0,
    /**< High frequency clock, which is in the order of a few tens of MHz and
     *   is only available in Normal operating mode of the system. This clock,
     *   which is the fastest available clock, should be used for applications
     *   requiring high-precision control. The performance available with this
     *   clock has a trade-off with system power.
     */

    QAPI_PWM_SOURCE_CLK_ECON_MODE_E = 1,
    /**< Medium frequency clock, which is in the order of few tens of MHz and
     *   is available in Normal and Economy operating modes of the system.
     *   Typically, this clock is slower than the NORMAL mode clock, resulting
     *   in diluted precision but offers better energy efficiency.
     */

    QAPI_PWM_SOURCE_CLK_SLEEP_MODE_E = 2,
    /**< Low frequency clock, which is of the order of few tens of kHz and
     *   is available in all system operating modes. This is the slowest
     *   available clock, providing the best energy efficiency at the expense
     *   of precision.
     *
     *   With this clock, a typical number of dimming levels will be in the order
     *   of a few hundreds if the expected PWM frequency is in the order of a few
     *   hundreds of Hz.
     *
     *   This is the only clock that allows outputting a PWM signal during
     *   Deep Sleep mode. PWM client(s) should switch to this source before
     *   the system transitions to Deep Sleep mode if the functionality is
     *   intended.
     */

    QAPI_PWM_SOURCE_CLK_INVALID_E = 0x7FFFFFFF
} qapi_PWM_Source_CLK_t;

/** @} */ /* end_addtogroup qapi_pwm */

/** @addtogroup qapi_pwm
@{ */

/**
* PWM configuration structure.
*
* Structure to hold channel configuration. Clients must use this
* structure to set and query channel configuration.
*/
typedef struct qapi_PWM_Config_s
{

   uint32_t freq;
   /**< Expected frequency of the PWM signal multiplied by 100 (i.e., 100 Hz = 10000 ). \n
    *   The default range in Normal/Economy operating mode is 1 to 100000000 (i.e., 0.01 Hz to 1 MHz). \n
    *   The default range in Deep Sleep mode is 1 to 300000 (i.e., 0.01 Hz to 3 kHz).
    */

   uint32_t duty;
   /**< Expected duty cycle (in %) of the PWM signal multiplied by 100 (i.e., 10000 = 100%). \n
    *   The default range is 0 to 10000 (i.e., 0% to 100%).
    */

   uint32_t phase;
   /**< Expected phase shift (in %) of the PWM signal multiplied by 100 (i.e., 10000 = 100%). \n
    *   The default range is 0 to 10000 (i.e., 0% to 100%).
    */

   uint32_t moduleType;
   /**< Module type should be 0 for PWM. */

   qapi_PWM_Source_CLK_t source_CLK;
   /**< PWM source clock. */
} qapi_PWM_Config_t;

/** @} */ /* end_addtogroup qapi_pwm */

/** @addtogroup qapi_pwm
@{ */

/**
* PWM client handle
*
* Handle provided by the module to the client. Clients must pass this
* handle as a token with subsequent calls. Note that the clients
* should cache the handle. Once lost, it cannot be queried back from
* the module. @newpage
*/
typedef void* qapi_PWM_Handle_t;

/** @} */ /* end_addtogroup qapi_pwm */

/*==================================================================================

                            FUNCTION DECLARATIONS

==================================================================================*/

/** @addtogroup qapi_pwm
@{ */

/*==================================================================================
  FUNCTION      qapi_PWM_Channel_Open
==================================================================================*/
/**
*  Opens a logical connection to a channel and assigns a handle
*  to the client to manage the connection.
*
*  @param[in]  channel_ID    Channel ID specified by #qapi_PWM_Channel_t.
*  @param[out] pHandle       Reference to a location in which to store the handle.
*
*  @return
*  QAPI_OK                -- Channel was successfully opened. \n
*  QAPI_ERROR             -- Chanel open failed. \n
*  QAPI_ERR_NO_RESOURCE   -- Specified channel is already in use. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
*  QAPI_ERR_NO_MEMORY     -- No memory is available to support this operation.
*/
qapi_Status_t qapi_PWM_Channel_Open
(
    qapi_PWM_Channel_t   channel_ID,
    qapi_PWM_Handle_t   *pHandle
);

/*==================================================================================
  FUNCTION      qapi_PWM_Channel_Set
==================================================================================*/
/**
*  Configures channel properties.
*
*  @note1hang
*  All channels share the same source clock for PWM. Changing the source clock
*  affects all PWM channels that are already enabled. \n
*  If the source clock needs to be changed, use the following sequence: \n
*     -# Disable all PWM channels currently enabled via qapi_PWM_Enable().
*     -# Change the source clock for the channels via qapi_PWM_Channel_Set()
*     -# Enable the intended PWM channels via qapi_PWM_Enable()
*
*  @param[in] handle      Channel handle provided by qapi_PWM_Channel_Open().
*  @param[in] pConfig     Pointer to a channel configuration structure of type
*                         #qapi_PWM_Config_t.
*
*  @return
*  QAPI_OK                -- Channel configuration was successful. \n
*  QAPI_ERROR             -- Channel configuration failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*
*/
qapi_Status_t qapi_PWM_Channel_Set
(
   qapi_PWM_Handle_t    handle,
   qapi_PWM_Config_t    *pConfig
);

/*==================================================================================
  FUNCTION      qapi_PWM_Channel_Get
==================================================================================*/
/**
*  Queries the channel configuration.
*
*  This function allows clients to retrieve the configuration settings of a
*  previously configured channel.
*
*  @param[in] handle      Channel handle provided by qapi_PWM_Channel_Open().
*  @param[in] pConfig     Pointer to a channel configuration structure of type
*                         #qapi_PWM_Config_t.
*
*  @return
*  QAPI_OK                -- Channel configuration read was successful. \n
*  QAPI_ERROR             -- Channel configuration read failed.
*/
qapi_Status_t qapi_PWM_Channel_Get
(
   qapi_PWM_Handle_t      handle,
   qapi_PWM_Config_t     *pConfig
);


/*==================================================================================
  FUNCTION      qapi_PWM_Enable
==================================================================================*/
/**
*  Enables/disables the output of PWM signals for a given set of channels. The client
*  should configure the channels properly via qapi_PWM_Channel_Set()
*  before enabling the signal outputs.
*
*  @param[in] pHandles    Pointer to the channel handle array.
*  @param[in] nHandles    Number of handles in the array.
*  @param[in] enableMask  Channel enable bit-mask. Bit 0 corresponds to handle 0 in the
*                         handle array, bit 1 corresponds to handle 1, and so on.
*                         If the bit is 1, the corresponding channel represented by
*                         the handle will be enabled, if the bit is 0, the channel will
*                         be disabled.
*  @return
*  QAPI_OK                -- Channel(s) configuration was successful. \n
*  QAPI_ERROR             -- Channel(s) configuration failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.

*/
qapi_Status_t qapi_PWM_Enable
(
   qapi_PWM_Handle_t  *pHandles,
   uint32_t            nHandles,
   uint32_t            enableMask
);


/*==================================================================================
  FUNCTION      qapi_PWM_Channel_Close
==================================================================================*/
/**
*  Closes a channel.
*
*  This function closes the logical connection to a channel. The channel handle
*  associated with the channel is invalid after the connection is closed
*
*  @param[in] handle      Channel handle provided by qapi_PWM_Channel_Open().
*
*  @return
*  QAPI_OK                -- Channel was successfully closed. \n
*  QAPI_ERROR             -- Channel cannot be closed.
*/
qapi_Status_t qapi_PWM_Channel_Close
(
    qapi_PWM_Handle_t      handle
);

/** @} */ /* end_addtogroup qapi_pwm */

#endif /* __QAPI_PWM_H__ */
