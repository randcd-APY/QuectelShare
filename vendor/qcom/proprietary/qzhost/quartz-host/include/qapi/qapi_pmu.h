#ifndef __QAPI_PMU_H__
#define __QAPI_PMU_H__

/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$


/*================================================================================ 
 *
 *                    PMU Driver API Header                                
 *
 *================================================================================*/

/**
 * @file qapi_pmu.h
 *
 * @addtogroup qapi_pmu
 * @{ 
 *
 * @brief API specification for the power management unit (PMU) driver.
 *
 * @details This section contains the prototypes of the functions to be used to configure the PMU module. 
 *
 * @code {.c}
 *
 *    * The code snippet below shows how to call qapi_PM_Vbatt_Level_Good() to
 *    * check the battery level.
 *
 *
 *      qapi_Status_t qapi_err_flag = QAPI_OK;
 *      qapi_PM_Vbatt_Status_Type_t  Vbatt_level_Status = QAPI_PM_VBATT_BELOW_TREHOLD_E;
 *      uint32 desired_Vbatt_Threshold = 2600;  //2.6V
 *
 *      qapi_err_flag = qapi_PM_Vbatt_Level_Good( desired_Vbatt_Threshold,  &Vbatt_level_Status);
 *      
 *      if (qapi_err_flag == QAPI_OK) 
 *      {
 *         if(Vbatt_level_Status == QAPI_PM_VBATT_ABOVE_TREHOLD_E)
 *         {
 *           //Battery level is above desired_Vbatt_Threshold
 *         }
 *         else if (Vbatt_level_Status == QAPI_PM_VBATT_BELOW_TREHOLD_E)
 *         {
 *           //Battery level is below desired_Vbatt_Threshold
 *         }
 *      }
 *      
 *      return qapi_err_flag;
 *         
 * @endcode
 *
 * @}
 *  
 */

/*=============================================================================
                            EDIT HISTORY FOR File

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.ioe/1.0/v2/rom/release/api/systemdrivers/pmu/qapi_pmu.h#5 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/12/17   leo     (Tech Comm) Edited/added Doxygen comments and markup.
01/06/16   aab     Creation
=============================================================================*/

/**
 * @addtogroup qapi_pmu 
 * @{ */

/*==========================================================================

                       INCLUDE FILES

==========================================================================*/


/*==================================================================================

                               TYPE DEFINITIONS

==================================================================================*/
/**
 * PMU Vbatt level status type.
 * 
 * Used to indicate whether the Vbatt level is above or below a given threshold.
 */
typedef enum
{
   QAPI_PM_VBATT_BELOW_TREHOLD_E   = 0,            /**< Vbatt level is below the given threshold. */ 
   QAPI_PM_VBATT_ABOVE_TREHOLD_E   = 1,            /**< Vbatt level is above the given threshold. */ 
   QAPI_PM_VBATT_TREHOLD_INVALID_E = 0x7fffffff    
}qapi_PM_Vbatt_Status_Type_t;


/*==================================================================================

                            FUNCTION DECLARATIONS

==================================================================================*/

/**
* Checks whether the battery level is above the intended level.
* 
* Programs the Vbatt threshold and checks whether the current battery level is above the programmed threshold.
* 
* @param[in] qapi_Vbatt_RefV      Intended battery level in mV. \n
*                                 Valid range: 2100 to 2800 (2.1 V to 2.8 V)
*                                
* @param[out] qapi_Vbatt_Status   Vbatt status (from qapi_PM_Vbatt_Status_Type_t): \n
*                        QAPI_PM_VBATT_ABOVE_TREHOLD_E -- Vbatt @ge qapi_Vbatt_RefV \n
*                        QAPI_PM_VBATT_BELOW_TREHOLD_E -- Vbatt < qapi_Vbatt_RefV
*                                
* @return
* qapi_Status_t
*
*/
qapi_Status_t 
qapi_PM_Vbatt_Level_Good ( uint32_t                     qapi_Vbatt_RefV,  
                           qapi_PM_Vbatt_Status_Type_t* qapi_Vbatt_Status );


/* @} */ /* end_addtogroup qapi_pmu */ 

#endif /* __QAPI_PMU_H__ */

