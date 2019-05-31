#ifndef __QAPI_TLMM__
#define __QAPI_TLMM__

/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$


/**
 * @file qapi_tlmm.h 
 * 
 * @addtogroup qapi_pmm
 * @{
 *
 * @brief Pin Mode Mux (PMM)
 * 
 * @details Modern SoCs pack a lot of functionality but are often pin-limited
 *          owing to their shrinking size. This limitation is overcome by
 *          incorporating hardware to flexibly mux several different
 *          functionalities on a given physical pin under software control.
 *
 *          This module exposes an interface allowing its clients to manage
 *          desired functionalities on a set of physical general-purpose input
 *          output (GPIO) pins on the SoC. The most common usage of this 
 *          interface is to configure pins for discrete input or output to
 *          implement handshake with external peripherals, sensors, or actuators.
 *          The code snippet below shows an example usage of the programming
 *          interface. The module requires clients to use physical pin numbers
 *          on the SoC. Consult a hardware schematic or use a device
 *          configuration database to determine proper pin number.
 *
 *
 * @code {.c}
 *    * The code snippet below demonstrates usage of the PMM interface. The example 
 *    * below configures SoC pin-13 to be a discrete GPIO configured as
 *    * input with a pull-down. Note that drive strength is defaulted to be
 *    * QAPI_GPIO_2MA_E even though it is not applicable for pins
 *    * configured as discrete inputs.
 *
 *    qapi_GPIO_ID_t     gpio_id;
 *    qapi_TLMM_Config_t tlmm_config;
 *    qapi_Status_t      status = QAPI_OK;
 *
 *    tlmm_config.pin = 13;
 *    tlmm_config.func = 1  // Using the functionality tied to pin mux value 1
 *    tlmm_config.dir = QAPI_GPIO_INPUT_E;
 *    tlmm_config.pull = QAPI_GPIO_PULL_DOWN_E;
 *    tlmm_config.drive = QAPI_GPIO_2MA_E; // drive is for output pins,
 *                                         // specify default here
 *
 *    status = qapi_TLMM_Get_Gpio_ID( &tlmm_config, &gpio_id);
 *
 *    if (status == QAPI_OK)
 *    {
 *      status = qapi_TLMM_Config_Gpio(gpio_id, &tlmm_config);
 *
 *      if (status != QAPI_OK)
 *      {
 *        // Handle failed case here
 *      }
 *    } 
 *
 * @endcode
 *
 * @}
 */

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.ioe/1.0/v2/rom/release/api/systemdrivers/tlmm/qapi_tlmm.h#8 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/13/17   leo     (Tech Comm) Edited/added Doxygen comments and markup.
08/11/15   dcf     Initial version.
===========================================================================*/


/*==========================================================================

                       INCLUDE FILES FOR MODULE

==========================================================================*/
#include <stdint.h>
#include "qapi/qapi_status.h"

/*==========================================================================

                      PUBLIC DEFINITIONS FOR MODULE

==========================================================================*/

/*==========================================================================

                      PUBLIC GPIO CONFIGURATION MACROS

==========================================================================*/

/** @addtogroup qapi_pmm
@{ */

/** 
 * @brief Pin direction enumeration. 
 *   
 * @details Enumeration used to specify the direction while configuring
 *           a GPIO pin.
 */

typedef enum
{ 
  QAPI_GPIO_INPUT_E  = 0, /**< Specify the pin as an INPUT to the SoC. */
  QAPI_GPIO_OUTPUT_E = 1, /**< Specify the pin as an OUTPUT from the SoC. */

  QAPI_GPIO_INVALID_DIR_E = 0x7fffffff /**< Placeholder -- Do not use. */

}qapi_GPIO_Direction_t;



/** 
  @brief GPIO pin pull type.
    
  @details This enumeration specifies the type of pull (if any) to use when
           specifying the configuration for a GPIO pin.
*/

typedef enum
{
  QAPI_GPIO_NO_PULL_E    = 0x0, /**< Specify no pull. */
  QAPI_GPIO_PULL_DOWN_E  = 0x1, /**< Pull the GPIO down. */
  QAPI_GPIO_PULL_UP_E    = 0x2, /**< Pull the GPIO up. */

  QAPI_GPIO_INVALID_PULL_E = 0x7fffffff /* Placeholder - Do not use */

}qapi_GPIO_Pull_t;


/** 
  @brief GPIO pin drive strength. 
    
  @details This enumeration specifies the drive strength to use when specifying 
           the configuration of a GPIO pin.
*/

typedef enum
{
  QAPI_GPIO_1P6MA_E = 0,   /**< Specify a 1.6 mA drive. */
  QAPI_GPIO_2P7MA_E = 1,   /**< Specify a 2.7 mA drive. */
  QAPI_GPIO_4P0MA_E = 2,   /**< Specify a 4.0 mA drive. */

  QAPI_GPIO_INVALID_STRENGTH_E = 0x7fffffff /* Placeholder - Do not use */

}qapi_GPIO_Drive_t;

#define QAPI_GPIO_2MA_E QAPI_GPIO_2P7MA_E
#define QAPI_GPIO_4MA_E QAPI_GPIO_4P0MA_E
#define QAPI_GPIO_6MA_E QAPI_GPIO_4P0MA_E
#define QAPI_GPIO_8MA_E QAPI_GPIO_4P0MA_E
#define QAPI_GPIO_10MA_E QAPI_GPIO_4P0MA_E
#define QAPI_GPIO_12MA_E QAPI_GPIO_4P0MA_E
#define QAPI_GPIO_14MA_E QAPI_GPIO_4P0MA_E
#define QAPI_GPIO_16MA_E QAPI_GPIO_4P0MA_E

/**
  @brief GPIO output state specification.
    
  @details This enumeration specifies the value to write to a GPIO pin configured as 
           an OUTPUT. This functionality is also known as <i>bit banging</i>.
    
*/

typedef enum
{
  QAPI_GPIO_LOW_VALUE_E,     /**< Drive the output LOW. */
  QAPI_GPIO_HIGH_VALUE_E,    /**< Drive the output HIGH. */

  QAPI_GPIO_INVALID_VALUE_E = 0x7fffffff /* Placeholder - Do not use */

}qapi_GPIO_Value_t;


/*==========================================================================

                      PUBLIC DATA STRUCTURES

==========================================================================*/


/**
  @brief GPIO configuration.
    
  @details This structure is used to specify the configuration for a GPIO on
           the SoC. The GPIO can be configured as an Input or Output that can
           be driven High or Low by the software. The interface also allows the
           SoC pins to be configured for alternate functionality.
*/

typedef struct
{
  uint32_t              pin;    /**< Physical pin number. */ 
  uint32_t              func;   /**< Pin function select. */
  qapi_GPIO_Direction_t dir;    /**< Direction (input or output). */
  qapi_GPIO_Pull_t      pull;   /**< Pull value. */
  qapi_GPIO_Drive_t     drive;  /**< Drive strength. */

}qapi_TLMM_Config_t;

/**
* @brief SoC pin access ID.
*
* @details A unique ID provided by the module to the client. Clients must pass
*          this ID as a token with subsequent calls. Note that clients
*          should cache the ID. 
*/

typedef uint16_t qapi_GPIO_ID_t;


/* ============================================================================
**  Function : qapi_TLMM_Get_Gpio_ID
** ============================================================================*/
/**
  @brief Gets a unique access ID.
 
  @details This function provides a unique access ID for a specified GPIO. 
           It is required in order to access GPIO configuration APIs.

  @param[in] *qapi_TLMM_Config  Reference to the pin configuration data.
  @param[in] *qapi_GPIO_ID      Reference to a location to store an access ID.
    
  @return
  QAPI_OK -- Pin GPIO ID was successfully created. \n
  QAPI_ERR -- Pin GPIO is currently in use or not programmable. \n

*/

qapi_Status_t qapi_TLMM_Get_Gpio_ID
(
  qapi_TLMM_Config_t* qapi_TLMM_Config,
  qapi_GPIO_ID_t*     qapi_GPIO_ID
);


/* ============================================================================
**  Function : qapi_TLMM_Release_Gpio_ID
** ============================================================================*/
/**
  @brief Releases an SoC pin.
 
  @details This function allows a client to relinquish a lock on a GPIO pin. It
           facilitates sharing of a pin between two drivers in different system
           modes where each driver may need to reconfigure the pin. Using this
           function is not required unless such a condition dictates.

  @param[in] *qapi_TLMM_Config   Reference to the pin configuration data.
  @param[in]  qapi_GPIO_ID       Pin access ID retrieved from the
                                 qapi_TLMM_get_GPIO_ID() call.

  @return
  QAPI_OK -- Pin was released successfully. \n
  QAPI_ERR -- Pin could not be released. \n

*/

qapi_Status_t qapi_TLMM_Release_Gpio_ID
(
  qapi_TLMM_Config_t* qapi_TLMM_Config,
  qapi_GPIO_ID_t      qapi_GPIO_ID
);


/* ============================================================================
**  Function : qapi_TLMM_Config_Gpio
** ============================================================================*/
/**
  @brief Changes the SoC pin configuration.
 
  @details This function configures an SoC pin based on a set of fields 
           specified in the configuration structure reference passed in as a
           parameter.
 
  @param[in] qapi_GPIO_ID       Pin access ID retrieved from the
                                qapi_TLMM_get_GPIO_ID() call.
  @param[in] *qapi_TLMM_Config  Pin configuration to use.

  @return
  QAPI_OK -- Pin was configured successfully. \n
  QAPI_ERR -- Pin could not be configured. \n

*/

qapi_Status_t qapi_TLMM_Config_Gpio
(
  qapi_GPIO_ID_t      qapi_GPIO_ID,
  qapi_TLMM_Config_t* qapi_TLMM_Config
);


/* ============================================================================
**  Function : qapi_TLMM_Drive_Gpio
** ============================================================================*/
/**
  @brief Sets the state of an SoC pin configured as an output GPIO.
 
  @details This function drives the output of an SoC pin that has been
           configured as a generic output GPIO to a specified value.
 
  @param[in] qapi_GPIO_ID    Pin access ID retrieved from the
                             qapi_TLMM_get_GPIO_ID() call.
  @param[in] pin             SoC pin number to configure.
  @param[in] value           Output value.
    
  @return
  QAPI_OK -- Operation completed successfully. \n
  QAPI_ERR -- Operation failed. \n

*/

qapi_Status_t qapi_TLMM_Drive_Gpio
(
  qapi_GPIO_ID_t    qapi_GPIO_ID,
  uint32_t          pin,
  qapi_GPIO_Value_t value
);


/* ============================================================================
**  Function : qapi_TLMM_Read_Gpio
** ============================================================================*/
/**
  @brief Reads the state of an SoC pin configured as an input GPIO.

  @param[in] qapi_GPIO_ID    Pin access ID retrieved from the
                             qapi_TLMM_get_GPIO_ID() call.
  @param[in] pin             SoC pin number to configure.
    
  @return
  QAPI_GPIO_HIGH_VALUE -- Read value was HIGH. \n
  QAPI_GPIO_LOW_VALUE -- Read value was LOW. \n
    
*/

qapi_GPIO_Value_t qapi_TLMM_Read_Gpio
(
  qapi_GPIO_ID_t qapi_GPIO_ID,
  uint32_t       pin
);


/* ============================================================================
**  Function : qapi_TLMM_Get_Gpio_Outval
** ============================================================================*/
/**
  @brief Gets the output value of a GPIO.

  @param[in] pin             SoC pin number to get output value for

  @return
  QAPI_GPIO_HIGH_VALUE -- Value was set HIGH. \n
  QAPI_GPIO_LOW_VALUE -- Value was set LOW. \n

*/
qapi_GPIO_Value_t qapi_TLMM_Get_Gpio_Outval
(
  uint32_t          pin
);


/* ============================================================================
**  Function : qapi_TLMM_Get_Gpio_Config
** ============================================================================*/
/**
  @brief Gets the SoC pin configuration.

  @details This function gets the SoC pin configuration in a set of fields
           specified in the configuration structure reference passed in as a
           parameter.

  @param[in,out] *qapi_TLMM_Config  Pin configuration for a specified pin.

  @return
  QAPI_OK -- Pin was configured successfully. \n
  QAPI_ERR -- Pin could not be configured. \n

*/

qapi_Status_t qapi_TLMM_Get_Gpio_Config
(
  qapi_TLMM_Config_t* qapi_TLMM_Config
);


/* ============================================================================
**  Function : qapi_TLMM_Get_Total_Gpios
** ============================================================================*/
/**
  @brief Gets the total number of GPIO pins in the SoC.

  @details This function gets the total number of GPIO pins in the SoC.

  @param[out] *total_GPIOs  Output variable to hold number of GPIOs.

  @return
  QAPI_OK -- Operation successful. \n
  QAPI_ERR -- Operation failed. \n

*/
qapi_Status_t qapi_TLMM_Get_Total_Gpios
(
  uint32_t *total_GPIOs
);

/* @} */ /* group pmm */

#endif /* __QAPI_TLMM__ */

