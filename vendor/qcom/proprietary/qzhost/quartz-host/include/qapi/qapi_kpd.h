#ifndef __QAPI_KPD_H__
#define __QAPI_KPD_H__

/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/*=================================================================================
 *
 *                       KEYPAD CONTROLLER
 *
 *================================================================================*/

 /** @file qapi_kpd.h
 *
 * @brief KEYPAD CONTROLLER (KPD)
 *
 * @addtogroup qapi_kpd
 * @{
 *
 * @details Qualcomm Technologies, Inc. SOC incorporate dedicated hardware to support an external
 *          8x8 keypad using a set of physical GPIO pins. This hardware has
 *          support to generate interrupts for Key Press, Long Key Press,
 *          Extended Key Press, and Key Release events.
 *
 *          This programming interface allows client software to manage the keypad
 *          controller module on the SoC. This programming interface assumes that
 *          the external 8x8 keypad layout is as shown in the table below. The
 *          columns (C7-C0) are the keypad's sense lines and the rows (R7-R0) are
 *          the drive lines.
 @verbatim
       -----------------------------------------------------------------
       | C7/R0 | C6/R0 | C5/R0 | C4/R0 | C3/R0 | C2/R0 | C1/R0 | C0/R0 |
       -----------------------------------------------------------------
       | C7/R1 | C6/R1 | C5/R1 | C4/R1 | C3/R1 | C2/R1 | C1/R1 | C0/R1 |
       -----------------------------------------------------------------
       | C7/R2 | C6/R2 | C5/R2 | C4/R2 | C3/R2 | C2/R2 | C1/R2 | C0/R2 |
       -----------------------------------------------------------------
       | C7/R3 | C6/R3 | C5/R3 | C4/R3 | C3/R3 | C2/R3 | C1/R3 | C0/R3 |
       -----------------------------------------------------------------
       | C7/R4 | C6/R4 | C5/R4 | C4/R4 | C3/R4 | C2/R4 | C1/R4 | C0/R4 |
       -----------------------------------------------------------------
       | C7/R5 | C6/R5 | C5/R5 | C4/R5 | C3/R5 | C2/R5 | C1/R5 | C0/R5 |
       -----------------------------------------------------------------
       | C7/R6 | C6/R6 | C5/R6 | C4/R6 | C3/R6 | C2/R6 | C1/R6 | C0/R6 |
       -----------------------------------------------------------------
       | C7/R7 | C6/R7 | C5/R7 | C4/R7 | C3/R7 | C2/R7 | C1/R7 | C0/R7 |
       -----------------------------------------------------------------
 @endverbatim
 *          The code snippet below shows an example usage.
 *          Note that the interface requires the client to specify the GPIO set to be
 *          used to interface with the external keypad. Consult the hardware schematic
 *          or device configuration database to determine the proper GPIO set number.
 *
 * @code {.c}
 *
 *    * The following example enables a reduced size keypad of four rows and
 *    * four columns. Here, kpd_CB() is the callback function that gets invoked
 *    * whenever there is a keypad interrupt.
 *    * The client can use qapi_KPD_State_Get() to query the real-time state
 *    * of the keypad if it is not using a callback function for notification.
 *    *
 *    * For brevity, the sequence assumes that all calls succeed. Clients should
 *    * check the return status from each call.
 *
 *    void kpd_CB ( const qapi_KPD_Interrupt_Status_t *kpd_Int_Status,
 *                  const qapi_KPD_KeyPress_t *kpd_KeyPress_State,
 *                  void *ctxt
 *                )
 *    {
 *
 *       // Use interrupt status and keypad state information here
 *
 *       return;
 *    }
 *
 *   qapi_Status_t kpd_result = QAPI_OK;
 *   qapi_KPD_Handle_t kpd_handle = NULL;
 *   qapi_KPD_Config_t kpd_config_set;
 *
 *
 *   kpd_config_set.kpdMatrix.row_Mask = 0x0f; // 4 rows enabled (R3-R0)
 *   kpd_config_set.kpdMatrix.col_Mask = 0x0f; // 4 columns enabled (C3-C0)
 *   kpd_config_set.kpdMatrix.gpio_Set = 0;
 *
 *   kpd_config_set.kpdDebounce.firstPress = 20;   // 20ms debounce
 *   kpd_config_set.kpdDebounce.longPress = 1000;  // 1s debounce
 *   kpd_config_set.kpdDebounce.repeatPress = 500; // 0.5s debounce
 *
 *   kpd_config_set.kpdInterrupt.firstPress = 1;   // enable all interrupts
 *   kpd_config_set.kpdInterrupt.longPress = 1;
 *   kpd_config_set.kpdInterrupt.repeatPress = 1;
 *   kpd_config_set.kpdInterrupt.keyRelease = 1;
 *
 *   kpd_result = qapi_KPD_Init(&kpd_handle);
 *   kpd_result = qapi_KPD_Set_Config(kpd_handle,
 *                                    &kpd_config_set,
 *                                    kpd_CB,
 *                                    NULL
 *                                   );
 *   kpd_result = qapi_KPD_Enable(kpd_handle, true);  // enable the keypad
 *
 *   kpd_result = qapi_KPD_Enable(kpd_handle, false); // disable the keypad
 *   kpd_result = qapi_KPD_DeInit(kpd_handle);
 *   kpd_handle = NULL;
 *
 * @endcode
 * @}
 */

/*==================================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.ioe/1.0/api/iodevices/qapi_kpd.h#7 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------------
02/10/17   leo     (Tech Comm) Edited/added Doxygen comments and markup.
05/06/16   leo     (Tech Comm) Edited/added Doxygen comments and markup.
12/04/15   leo     (Tech Comm) Edited/added Doxygen comments and markup.
10/29/15   ma      Initial version
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

/** @addtogroup qapi_kpd
 @{ */

/**
* Structure to hold real-time state of the keypad matrix.
*/
typedef struct qapi_KPD_KeyPress_s
{
    uint32_t  keyMatrix_Hi;
    /**< Real-time status of the top four rows of the keypad.
     *   The MSB holds the status of row 7 and the LSB holds
     *   the status of row 4.
     *   A set bit means the corresponding key was pressed.
     */

    uint32_t  keyMatrix_Lo;
    /**< Real-time status of the bottom four rows of the keypad.
    *    The MSB holds the status of row 3 and the LSB holds
    *    the status of row 0.
    *    A set bit means the corresponding key was pressed.
    */
} qapi_KPD_KeyPress_t;


/**
* Structure to hold keypad interrupt status information.
*/
typedef struct qapi_KPD_Interrupt_Status_s
{
   uint32_t firstPress;   /**< First press interrupt status. */

   uint32_t longPress;    /**< Long press interrupt status. */

   uint32_t repeatPress;  /**< Repeated press interrupt status. */

   uint32_t keyRelease;   /**< Key release interrupt status. */
} qapi_KPD_Interrupt_Status_t;


/**
* Structure to hold keypad debounce configuration data.
*/
typedef struct qapi_KPD_Debounce_Config_s
{
   uint32_t firstPress;
   /**< First press debounce time in ms (default range: 4 to 60).
    *   The typical value for first press debounce is 20 ms.
    */

   uint32_t longPress;
   /**< Long press debounce time in ms (default range: 63 to 3937).
    *   The long press timer starts after the initial debounce has completed.
    *   The typical value for long press debounce is 1000 ms.
    */

   uint32_t repeatPress;
   /**< Repeated press debounce time in ms (default range: 63 to 3937).
    *   Repeated press debounce starts after long press debounce has completed.
    *   The typical value for repeated press debounce is 500 ms.
    */
} qapi_KPD_Debounce_Config_t;


/**
* Structure that defines the keypad matrix configuration.
*
* This defines the rows and columns (of an 8x8 keypad) that need to be enabled.
* Using this structure, the client can select a subset of rows and columns
* in the case of a reduced-size keypad.
*
* @note1hang The external keypad module is interfaced to the keypad controller
*       using GPIOs. In the case of a reduced size keypad, the unused sense
*       lines (i.e., columns) should have a weak pull-up on their GPIOs for
*       key detection to work properly.
*/
typedef struct qapi_KPD_Matrix_Config_s
{
   uint32_t row_Mask;
   /**< Bitmap of rows that need to be enabled
    *  (e.g., 0x03 = Rows 0 and 1 are enabled).
    */

   uint32_t col_Mask;
   /**< Bitmap of columns that need to be enabled
    * (e.g., 0x03 = Columns 0 and 1 are enabled ).
    */

   uint32_t gpio_Set;
   /**< GPIO set to be used. The platform supports mutiple sets of GPIOs
     *  to interface with an external keypad. Consult the hardware schematic
     *  configuration database for the GPIO set to be used.
     *  0 = set 0 is selected; 1 = set 1 is selected.
     */
} qapi_KPD_Matrix_Config_t;


/**
* Structure that defines interrupts that need to be enabled.
*/
typedef struct qapi_KPD_Int_Enable_s
{

   uint32_t firstPress;   /**< First press interrupt enable. */

   uint32_t longPress;    /**< Long press interrupt enable. */

   uint32_t repeatPress;  /**< Repeated press interrupt enable. */

   uint32_t keyRelease;   /**< Key release interrupt enable. @newpagetable */

} qapi_KPD_Int_Enable_t;


/**
* Structure that defines the keypad configuration. The client must use this
* structure to configure keypad properties. @vertspace{-4}
*/
typedef struct qapi_KPD_Config_s
{
   qapi_KPD_Matrix_Config_t   kpdMatrix;
   /**< Keypad matrix configuration. */

   qapi_KPD_Debounce_Config_t kpdDebounce;
   /**< Keypad debounce configuration. */

   qapi_KPD_Int_Enable_t      kpdInterrupt;
   /**< Keypad interrupt configuration. */
} qapi_KPD_Config_t;

/** @} */ /* end_addtogroup qapi_kpd */

/** @addtogroup qapi_kpd
@{ */

/**
* Client-defined keypad interrupt callback function.
*/
typedef void (*qapi_KPD_Interrupt_CB_t) ( const qapi_KPD_Interrupt_Status_t *pKpd_IntStatus,
                                          const qapi_KPD_KeyPress_t *pKpd_KeyPressState,
                                          void *callback_Ctxt
                                        );

/**
* Handle provided by the module to the client. The client must pass this
* handle as a token with subsequent calls. The client is advised to
* cache the handle because, once lost, it cannot be queried back from
* the module.
*/
typedef void* qapi_KPD_Handle_t;

/** @} */ /* end_addtogroup qapi_kpd */


/*=============================================================================

                      FUNCTION DECLARATIONS

=============================================================================*/

/*=============================================================================
  FUNCTION      qapi_KPD_Init
=============================================================================*/

/** @addtogroup qapi_kpd
@{ */

/**
*  Initializes the keypad module.
*
*  This function initializes internal data structures along with associated
*  static data, and it assigns a handle to the client. This function should
*  be called before calling any other function.
*
*  @param[out] pHandle Reference to the location in which to store the handle.
*
*  @return
*  QAPI_OK    --  Module was initialized successfully. \n
*  QAPI_ERROR --  Module initialization failed.
*/
qapi_Status_t qapi_KPD_Init
(
   qapi_KPD_Handle_t       *pHandle
);


/*=============================================================================
  FUNCTION      qapi_KPD_Set_Config
=============================================================================*/
/**
*  Configures keypad properties.
*
*  @param[in]  handle       Module handle provided by qapi_KPD_Init().
*  @param[in]  pKpd_Config  Pointer to the keypad configuration structure
*                           #qapi_KPD_Config_t.
*  @param[in]  c_Fn         Keypad interrupt callback function #qapi_KPD_Interrupt_CB_t.
*  @param[in]  pCtxt        Pointer to a client-allocated buffer containing context to be
*                           passed to the callback routine.
*
*  @return
*  QAPI_OK                --  Keypad configuration was successful. \n
*  QAPI_ERROR             --  Keypad configuration failed. \n
*  QAPI_ERR_INVALID_PARAM --  Invalid parameters were specified.
*/
qapi_Status_t qapi_KPD_Set_Config
(
   qapi_KPD_Handle_t        handle,
   qapi_KPD_Config_t       *pKpd_Config,
   qapi_KPD_Interrupt_CB_t  c_Fn,
   void                    *pCtxt
);

/*=============================================================================
  FUNCTION      qapi_KPD_Get_State
=============================================================================*/
/**
*  Enables clients to query the keypad real-time state.
*
*  @note1hang
*  Keypad state information will be provided to the client if the client
*  has registered a callback.
*
*  @param[in] handle              Module handle provided by qapi_KPD_Init().
*  @param[in] pKpd_KeyPressState  Pointer to the keypad state structure
*                                 #qapi_KPD_KeyPress_t.
*
*  @return
*  QAPI_OK    -- Keypad status read was successful. \n
*  QAPI_ERROR -- Keypad status read failed.
*
*/
qapi_Status_t qapi_KPD_Get_State
(
   qapi_KPD_Handle_t      handle,
   qapi_KPD_KeyPress_t   *pKpd_KeyPressState
);

/*=============================================================================
  FUNCTION      qapi_KPD_Enable
=============================================================================*/
/**
* Enables/disables the keypad scanner module.
*
*  @param[in] handle  Module handle provided by qapi_KPD_Init().
*  @param[in] enable  TRUE to enable the keypad, FALSE to disable it.
*
*  @return
*  QAPI_OK                -- Keypad enable/disable was successful. \n
*  QAPI_ERROR             -- Keypad enable/disable failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*/
qapi_Status_t qapi_KPD_Enable
(
   qapi_KPD_Handle_t   handle,
   qbool_t             enable
);

/*=============================================================================
  FUNCTION      qapi_KPD_DeInit
==============================================================================*/
/**
*  Deinitializes the keypad module and frees all memory used.
*
*  The handle is invalid after the module is deinitialized.
*
*  @param[in] handle  Handle provided by qapi_KPD_Init().
*
*  @return
*  QAPI_OK    -- Module deinitialized successfully. \n
*  QAPI_ERROR -- Module deinitialization failed.
*/
qapi_Status_t qapi_KPD_DeInit
(
    qapi_KPD_Handle_t   handle
);

/** @} */ /* end_addtogroup qapi_kpd */

#endif /* __QAPI_KPD_H__ */
