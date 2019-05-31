#ifndef QURT_SYSTEM_H
#define QURT_SYSTEM_H

/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$


/**
  @file qurt_system.h 
  @brief  Prototypes of QuRT System level functionality API

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

*/

#include "qurt_types.h"

/*=============================================================================
                                 CONSTANTS AND MACROS
=============================================================================*/ 

/*=============================================================================
                                    TYPEDEFS
=============================================================================*/

/*=============================================================================
                                    FUNCTIONS
=============================================================================*/

/** @addtogroup qurt_system
@{ */

/**
  BSP hook that is to be set if Low Power mode is intended.
  
  @param[in] entrypoint Pointer to the BSP function.

  @return
  None.

  @dependencies
  None.
  
*/
void qurt_power_register_idle_hook(void (*entrypoint) (uint32));

/**
  BSP hook that must be set before qurt_init is invoked
  
  @param[in] entrypoint Pointer to the BSP initialization function.

  @return
  None.

  @dependencies
  None.
  
*/
void qurt_system_register_bsp_hook(void (*entrypoint) (void));

/**  
  Triggers a power collapse.

  This function performs the following power collapse sequence: 
  - 1. Checks for the power collapse type for Low Power mode (light or deep sleep). \n
  - 2. Saves the CPU and peripheral context. \n
  - 3. Raises a WFI.
  
  If an interrupt is pending, the API immediately returns to the caller. \n
  The API also returns immediately in the case of an error condition.

  @param[in] type   Sleep type. Values:
                    - QURT_POWER_SHUTDOWN_TYPE_LIGHTSLEEP
                    - QURT_POWER_SHUTDOWN_TYPE_DEEPSLEEP @tablebulletend
                      
  @param[in] pcontext Pointer to the region where the system context is to be 
                        saved.
  
  @param[in] context_sz  Size of context in bytes.
                        
  @return
  QURT_POWER_SHUTDOWN_EXIT_IPEND -- Power collapse failed because of a pending interrupt. \n
  QURT_POWER_SHUTDOWN_EXIT_LIGHTSLEEP -- Operation was successfully performed. 
                                         This value indicates that the processor has
                                         returned from power collapse light sleep mode. \n
  QURT_POWER_SHUTDOWN_EXIT_DEEPSLEEP  -- Operation was successfully performed. 
                                         This value indicates that the processor has
                                         returned from power collapse deep sleep mode.
  
  @dependencies
  None.
  
*/
int qurt_power_shutdown_enter (uint32 type, void *pcontext, uint32 context_sz);

/**
  Undoes state changes made in preparing for a power collapse.

  This function unmasks the global interrupts.
 
  @param[in] pcontext  Pointer to the region from which the system 
                       context was to be restored.
                        
  @return
  None.

  @dependencies
  None.
  
*/
void qurt_power_shutdown_exit (void *pcontext); 

/**
Gets the idle time duration remaining for threads created in the system.\n
 
  @param[in]  void
                        
  @return
  Number of system ticks

  @dependencies
  None.
  
*/
uint32 qurt_system_get_idle_time( void );

/** @} */ /* end_addtogroup qurt_system */
				 
#endif /* QURT_SYSTEM_H */

