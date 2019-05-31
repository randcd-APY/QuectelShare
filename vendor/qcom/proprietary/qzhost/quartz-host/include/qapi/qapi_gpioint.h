#ifndef __QAPI_GPIOINT_H__
#define __QAPI_GPIOINT_H__

/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/*=================================================================================
 *
 *                       GPIO INTERRUPT CONTROLLER
 *
 *================================================================================*/
 /** @file qapi_gpioint.h
 *
 * @addtogroup qapi_gpioint
 * @{
 *
 * @brief GPIO Interrupt Controller (GPIOInt)
 *
 * @details Provides an interface to register for interrupts given a GPIO.
 *          These are generally used for customer specific use-cases in which
 *          an entity external to the chip needs to communicate with the chip.
 *          This can be done by configuring a GPIO as an input, and toggling it
 *          externally to the chip. In doing so, this will cause a GPIO interrupt
 *          to fire, and software will be invoked in order to handle it. Additional
 *          The registration API will allow clients to register their callback, and the
 *          driver will internally configure the hardware in order to handle the
 *          given trigger type. Clients may additionally force-trigger the interrupt
 *          by using the trigger API, as well as check if an interrupt is pending by
 *          using the Is_Interrupt_Pending API. The GPIO interrupt may be enabled
 *          or disabled at any time using the Enable or Disable API. This will
 *          assure that the callback is not removed from the handler, but the
 *          interrupt will be unmasked/masked accordingly.
 *
 * @code {.c}
 *    * The code snippet below demonstates usage of this interface. The example
 *    * below includes the qapi_gpioint.h header file. This example will
 *    * register a callback with the GPIO Interrupt driver, and will manually
 *    * trigger the interrupt. Although this is a manual trigger use-case,
 *    * in practice the GPIO is usually triggered externally to the chip.
 *    * After triggering the interrupt, it will loop 1000 times and deregister
 *    * the callback from the driver.
 *    *
 *    * This code snippet will register for GPIO 10 specifically. It will
 *    * register the callback that will be defined as type qapi_GPIOINT_CB_t
 *    * and register medium priority. It will be level high trigger
 *    * given the input parameter GPIOINT_TRIGGER_HIGH_LEVEL, meaning that
 *    * when the external signal is high, it will jump to the handler if
 *    * enabled.
 * 
 *
 * qapi_Status_t          nStatus;
 * qapi_Instance_Handle_t pH;
 * uint32_t               nLoopCounter = 0;
 *
 * nStatus = qapi_GPIOINT_Register_Interrupt(&pH,         // Pass in a pointer
 *                                                        // to the handle
 *                                           10,          // GPIO 10 is used
 *                                           pfnCallback, // Callback fn pointer
 *                                           NULL,
 *                                             // NULL callback data
 *                                           GPIOINT_TRIGGER_HIGH_LEVEL,
 *                                             // Level high trigger
 *                                           QAPI_GPIOINT_PRIO_MEDIUM_E,
 *                                             // Priority of interrupt
 *                                           false );
 *                                             // Maskable interrupt
 * if ( nStatus != QAPI_OK )
 * {
 *   // Error!
 * }
 * 
 * // Trigger interrupt for GPIO 10
 * nStatus = qapi_GPIOINT_Trigger_Interrupt( &pH, 10 );
 * if ( nStatus != QAPI_OK )
 * {
 *   // Error!
 * }
 *
 * while ( nLoopCounter++ < 1000 )
 * {
 * }
 * 
 * // Deregister the GPIO Interrupt
 * nRet = qapi_GPIOINT_Deregister_Interrupt( &pH, 10 );
 * if ( nRet != GPIOINT_SUCCESS )
 * {
 *   // Error!
 * }
 *
 * @endcode
 *
 * @} 
 */

/*==================================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.ioe/1.0/v2/rom/release/api/systemdrivers/gpioint/qapi_gpioint.h#7 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------------
02/10/17   leo     (Tech Comm) Edited/added Doxygen comments and markup.
01/05/16   cpaulo  Updated documentation.
12/07/15   cpaulo  Initial revision.
==================================================================================*/

/** @addtogroup qapi_gpioint
 @{ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/



/** 
 * GPIO interrupt trigger type enumeration for supported triggers.
 */
typedef enum {
  QAPI_GPIOINT_TRIGGER_LEVEL_HIGH_E,    /**< Level triggered active high. */
  QAPI_GPIOINT_TRIGGER_LEVEL_LOW_E,     /**< Level triggered active low. */
  QAPI_GPIOINT_TRIGGER_EDGE_RISING_E,   /**< Rising edge triggered. */
  QAPI_GPIOINT_TRIGGER_EDGE_FALLING_E,  /**< Falling edge triggered. */
  QAPI_GPIOINT_TRIGGER_EDGE_DUAL_E,     /**< Dual edge triggered. */
  QAPI_GPIOINT_TRIGGER_MAX_E
} qapi_GPIOINT_Trigger_e;

/** 
 * GPIO interrupt priority selection. The priority may determine how the
 * interrupt is configured internally. 
 */
typedef enum {
  QAPI_GPIOINT_PRIO_HIGHEST_E,  /**< Highest priority. */
  QAPI_GPIOINT_PRIO_HIGH_E,     /**< Medium-high priority. */
  QAPI_GPIOINT_PRIO_MEDIUM_E,   /**< Medium priority. */
  QAPI_GPIOINT_PRIO_LOW_E,      /**< Medium-low priority. */
  QAPI_GPIOINT_PRIO_LOWEST_E,   /**< Highest priority. @newpage */
  QAPI_GPIOINT_PRIO_MAX_E
} qapi_GPIOINT_Priority_e;



/** 
 * GPIO interrupt callback data type.
 *
 * This is the data type of the 
 * argument passed into the callback that is registered
 * with the GPIO interrupt module. The value to pass
 * will be given by the client at registration time.
 */
typedef uint32_t qapi_GPIOINT_Callback_Data_t;

/** 
 * GPIO interrupt callback function definition.
 *
 * GPIO interrupt clients will pass a function pointer of
 * this format into the registration API.
 */
typedef void ( *qapi_GPIOINT_CB_t )( qapi_GPIOINT_Callback_Data_t );

/** 
 * GPIO interrupt handle definition.
 */
typedef void* qapi_Instance_Handle_t;



/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/

/** 
 * Registers a callback for a GPIO interrupt.
 *
 * Registers a callback function with the GPIO 
 * interrupt controller and enables the interrupt.
 * This function configures and routes the interrupt
 * accordingly, as well as enabling it in the underlying
 * layers.
 *
 * @param[in]  pH           Input handle to the client context.
 * @param[in]  nGpio        GPIO number to configure for an 
 *                          interrupt.
 * @param[in]  pfnCallback  Callback function pointer.
 * @param[in]  nData        Callback data.
 * @param[in]  eTrigger     Trigger type for the interrupt.
 * @param[in]  ePriority    Priority enumeration to determine
 *                          the configuration of the GPIO interrupt.
 * @param[in]  bNmi         Boolean value to select whether or not the
 *                          GPIO interrupt is nonmaskable to the CPU.
 * @return
 * QAPI_ERR_INVALID_PARAM -- There is an issue with one of the input parameters \n
 * QAPI_ERROR             -- Error in internal registration; this may also be returned
 *                           if there is an invalid handle or if an incorrect or invalid GPIO is being used \n
 * QAPI_OK                -- Successfully registered
 * 
 */
qapi_Status_t qapi_GPIOINT_Register_Interrupt
(
  qapi_Instance_Handle_t       *pH,
  uint32_t                      nGpio,
  qapi_GPIOINT_CB_t             pfnCallback,
  qapi_GPIOINT_Callback_Data_t  nData,
  qapi_GPIOINT_Trigger_e        eTrigger,
  qapi_GPIOINT_Priority_e       ePriority,
  qbool_t                       bNmi
);

/** 
 * Deregisters a callback function from the GPIO 
 * interrupt controller and disables the interrupt.
 * This function deconfigures the interrupt
 * accordingly, as well as disabling it in the underlying
 * layers.
 *
 * @param[in]  pH           Input handle to the client context.
 * @param[in]  nGpio        GPIO number to deconfigure.
 * 
 * @return 
 * QAPI_ERROR -- Error in internal deregistration; this may also be returned
 *               if there is an invalid handle or if an incorrect or invalid GPIO is being used \n
 * QAPI_OK    -- Successfully deregistered
 *
 */
qapi_Status_t qapi_GPIOINT_Deregister_Interrupt
(
  qapi_Instance_Handle_t *pH,
  uint32_t                nGpio
);

/** 
 * Dynamically sets the trigger type of a registered GPIO
 * interrupt.
 *
 * Configures the underlying layer to capture an 
 * interrupt with a given trigger type. This API is
 * only to be used on a currently registered GPIO
 * interrupt and will change the trigger at runtime.
 *
 * @param[in]  pH         Input handle to the client context.
 * @param[in]  nGpio      GPIO number in which to set the trigger.
 * @param[in]  eTrigger   Trigger type to configure.
 * 
 * @return
 * QAPI_ERR_INVALID_PARAM -- eTrigger parameter is invalid \n
 * QAPI_ERROR             -- Internal error in setting the trigger; this may also be returned
 *                           if there is an invalid handle or if an incorrect or invalid GPIO is being used \n
 * QAPI_OK                -- Successfully set the trigger
 *
 */
qapi_Status_t qapi_GPIOINT_Set_Trigger
(
  qapi_Instance_Handle_t *pH,
  uint32_t                nGpio,
  qapi_GPIOINT_Trigger_e  eTrigger
);

/** 
 * Enables a currently disabled and registered GPIO 
 * interrupt.
 *
 * This is used primarily to unmask interrupts.
 *
 * @param[in]  pH      Input handle to the client context.
 * @param[in]  nGpio   GPIO number to enable.
 * 
 * @return
 * QAPI_ERROR -- Internal error in enabling the interrupt; this may also be returned
 *               if there is an invalid handle or if an incorrect or invalid GPIO is being used \n
 * QAPI_OK    -- Successfully enabled the interrupt
 *
 */
qapi_Status_t qapi_GPIOINT_Enable_Interrupt
(
  qapi_Instance_Handle_t *pH,
  uint32_t                nGpio
);

/** 
 * Disables a currently enabled and registered GPIO 
 * interrupt.
 *
 * This is used primarily to mask interrupts, still being able to capture them,
 * but not have the callback called.
 *
 * @param[in]  pH      Input handle to the client context.
 * @param[in]  nGpio   GPIO number to disable.
 * 
 * @return
 * QAPI_ERROR -- Internal error in disabling the interrupt; this may also be returned
 *               if there is an invalid handle or if an incorrect or invalid GPIO is being used \n
 * QAPI_OK    -- Successfully disabled the interrupt 
 *
 */
qapi_Status_t qapi_GPIOINT_Disable_Interrupt
(
  qapi_Instance_Handle_t *pH,
  uint32_t                nGpio
);

/** 
 * Manually triggers a GPIO interrupt by writing to the appropriate register.
 *
 * @param[in]  pH       Input handle to the client context.
 * @param[in]  nGpio    GPIO number to trigger.
 * 
 * @return
 * QAPI_ERROR -- Internal error in triggering the interrupt; this may also be returned
 *               if there is an invalid handle or if an incorrect or invalid GPIO is being used \n
 * QAPI_OK    -- Successfully triggered the interrupt 
 *
 */
qapi_Status_t qapi_GPIOINT_Trigger_Interrupt
(
  qapi_Instance_Handle_t *pH,
  uint32_t                nGpio
);

/** 
 * Queries to see if an interrupt is pending in the hardware by reading the
 * appropriate register.
 *
 * @param[in]  pH           Input handle to the client context.
 * @param[in]  nGpio        GPIO number to trigger.
 * @param[out] pbIsPending  Boolean value for whether or not the 
 *                          interrupt is pending in the hardware.
 * 
 * @return
 * QAPI_ERR_INVALID_PARAM -- pbIsPending pointer is NULL \n
 * QAPI_ERROR             -- Internal error in checking pending; this may also be returned
 *                           if there is an invalid handle or if an incorrect or invalid GPIO is being used \n
 * QAPI_OK                -- Successfully checked the pending status
 *
 */
qapi_Status_t qapi_GPIOINT_Is_Interrupt_Pending
(
  qapi_Instance_Handle_t *pH,
  uint32_t                nGpio,
  qbool_t                *pbIsPending
);

/** @} */ /* end_addtogroup qapi_gpioint */

#endif // ] #ifndef __QAPI_GPIOINT_H__

