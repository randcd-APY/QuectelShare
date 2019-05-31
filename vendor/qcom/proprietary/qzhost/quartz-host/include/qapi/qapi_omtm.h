#ifndef QAPI_OMTM_H
#define QAPI_OMTM_H

/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
@file qapi_omtm.h

Operating Mode Transition Module QAPI
*/

#ifdef __cplusplus
extern "C" {
#endif


/*----------------------------------------------------------------------------
 * Defines/Enums
 * -------------------------------------------------------------------------*/

/** @addtogroup qapi_omtm
@{ */
 
/** @name Operating Mode Attributes
@{ */

/** Default attributes. */
#define QAPI_OMTM_MODE_ATTR_DEFAULT  0x00000000

/** Mode requires XIP. */
#define QAPI_OMTM_MODE_ATTR_XIP_ON   0x00000001

/** @} */ /* end_namegroup */

/** @name Operating Mode Exit Callback Priorities
@{ */

/** Mode exit callbacks that do not have a priority and just need to
    be invoked on mode exit should be registered with this value as "prio" */
#define QAPI_OMTM_MODE_EXIT_CB_PRIO_DEFAULT 0

/** @} */ /* end_namegroup */

/**
 * Enumeration to specify when a mode switch is to occur.
 */
typedef enum
{
  /**< Switch when the system goes idle. NOT SUPPORTED; treated as NOW */ 
  QAPI_OMTM_SWITCH_AT_IDLE_E, 
  
  QAPI_OMTM_SWITCH_NOW_E, /**< Switch now. */
} qapi_OMTM_Switch_At_t;

/**
 * CONSS Mode Ids. Use one of the below tospecify which Memory Mode
 * to switch CONSS to, in qapi_OMTM_Switch_ConSS_Memory_Mode.
 */
typedef enum
{
  /**< Minimal memory mode on CONSS */
  QAPI_OMTM_CONSS_MODE_MMM,
  
  /**< Full memory mode on CONSS */
  QAPI_OMTM_CONSS_MODE_FMM,
  
  /**< Limited memory mode #1 on CONSS */
  QAPI_OMTM_CONSS_MODE_LMM1,
  
} qapi_OMTM_ConSS_Mode_Id;

/** @} */ /* end_addtogroup qapi_omtm */

/** @addtogroup qapi_omtm
@{ */

/*----------------------------------------------------------------------------
 * Types
 * -------------------------------------------------------------------------*/

/**
 * Defines a memory range to load from flash.
 *
 * Only ELF segments that fit completely in one of these regions are loaded.
 * OEMs can provide an array of these against the "whitelist" field
 * in qapi_OMTM_Operating_Mode_t. OMTM compares regions loaded in the
 * current operating mode and only loads those regions that are not already
 * loaded.
 */
typedef struct
{ 
  uint32_t address; /**< Memory range start address. */
  uint32_t size; /**< Memory size. @newpagetable */
} qapi_OMTM_Whitelist_Region_t;


/** Mode entry point. */
typedef void ( *qapi_OMTM_Mode_Entry_FN_t ) ( void );

/** 
 * Mode exit callbacks are invoked before switching out of the current mode.
 * The dest_Mode_Id parameter identifies the mode to which the switch is taking place.
 *
 * @param[in] dest_mode_id Destination mode ID.
 * @param[in] user_Data Pointer to the user data.
 */
typedef void ( *qapi_OMTM_Mode_Exit_CB_t )( uint32_t dest_Mode_Id, 
                                            void *user_Data );

/** Defines an operating mode. */
typedef struct
{
  uint32_t attributes;
  /**< Attributes of the operating mode. Some of these imply an action to be
     executed (by OMTM) at mode switch. Some are only informational. */  

  qapi_OMTM_Mode_Entry_FN_t entry_Fn;
  /**< Entry point; jump to this routine to enter this mode. */

  qapi_OMTM_Whitelist_Region_t *whitelist;
  /**< Array of memory ranges to load from flash before entering this mode,
   *   sorted by address (this ordering is important).
   *
   * If NULL, no loading occurs before jumping to the entry point
   * of this mode.
   *
   * When switching into this mode, OMTM compares this list against regions
   * already loaded in the current operating mode and only passes those regions
   * that are not already loaded to the loader. The loader only loads
   * those ELF segments that fit entirely in one of these regions.
   *
   * The user must explicitly cast the provided array into type
   * qapi_OMTM_Whitelist_Region_t[].
   */  

  uint32_t num_Regions;
  /**< Number of elements in the whitelist array. */
  
} qapi_OMTM_Operating_Mode_t;

/** @} */ /* end_addtogroup qapi_omtm */

/** @addtogroup qapi_omtm
@{ */

/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/
 
/** 
 * One-time call to register operating modes with OMTM.
 * This API must be invoked before qapi_OMTM_Switch_Mode
 *
 * @param[in] modes Array of OMs defined by the OEM.
 * @param[in] num_Modes Number of OMs in the array.
 * @param[in] cur_Mode Index of the current operating mode in "modes."
 *
 * @return
 *  QAPI_OK on success, one of the other status codes on error.
 */
qapi_Status_t
qapi_OMTM_Register_Operating_Modes( qapi_OMTM_Operating_Mode_t *modes, 
                                    uint32_t num_Modes, uint32_t cur_Mode );

/** 
 * Returns a pointer to the qapi_OMTM_Operating_Mode_t structure of
 * the current operating mode.
 */
qapi_OMTM_Operating_Mode_t* qapi_OMTM_Get_Current_Operating_Mode( void );

/** 
 * Returns the index in the Operating Mode Table of the current
 * operating mode.
 */
uint32_t qapi_OMTM_Get_Current_Operating_Mode_Id( void );

/** 
 * Registers a callback to be invoked before switching out of
 * the current operating mode. The callback is invoked with the
 * mode_id of the mode that is being switched to and user_data as parameters.
 *
 * @param[in] func Pointer to the callback routine.
 * @param[in] user_Data Pointer to data that is to be passed to the callback.
 * @param[in] prio Either QAPI_OMTM_MODE_EXIT_CB_PRIO_DEFAULT or an integer
 * identifying a "priority". Callbacks with a higher priority are
 * invoked before callbacks with a lower priority. Callbacks with the same
 * priority may be invoked in any order relative to each other.
 * Callbacks with a 0 or DEFAULT priority are invoked in an undefined order,
 * after callbacks with +ve priorities.
 * Callbacks with -ve priority values are invoked after the DEFAULT/0
 * priority callbacks.
 * Callback authors must negotiate priorities between themselves.
 * In general, if to be invoked first, choose a very positive value.
 * To be invoked last, choose a very negative value.
 *
 * @note1hang There may be only one callback registered with INT32_MAX or INT32_MIN
 * priorities. The routine returns an error on an attempt to register
 * a second callback at these priorities.
 *
 * @note1hang Mode exit callback registrations only apply to the current mode and
 * are dispatched and cleared before a mode transition.
 *
 * @return
 * QAPI_OK if the callback was successfully registered, 
 * one of the other status codes on error.
 */
qapi_Status_t
qapi_OMTM_Register_Mode_Exit_Callback( qapi_OMTM_Mode_Exit_CB_t func,
                                       void *user_Data, int32_t prio );
                                       
/** 
 * Switches operating modes.
 *
 * If no entity is voting against a mode switch, OMTM immediately attempts to
 * switch to the operating mode at index "mode_id" in the Operating Mode Table.
 *
 * If an entity has a vote preventing a mode switch, this routine does nothing
 * and returns an error code.
 *
 * Mode switch occurs if all of the registered mode_switch_prepare callbacks
 * allow it; if any one of them asks to abort, mode switch is aborted and the
 * routine returns an error code.
 *
 * @note Mode switch requests are always treated NOW irrespective of the
 * value of the "when" parameter. AT_IDLE mode switches are NOT supported.
 *
 * @param[in] mode_Id Index into the Operating Mode table (array of OMs) 
 * identifying the mode to which to switch.
 * @param[in] when One of values in the qapi_OMTM_Switch_At_t enums (ignored; see note)
 *
 * @return
 * If no entity has a vote against the mode switch and none of the
 * registed prepare callbacks ask to abort the mode switch, the call does
 * not return.
 * If an entity has a vote preventing a mode switch or a registered prepare
 * callback requests to abort the mode switch, the call returns QAPI_ERROR.
 */
qapi_Status_t
qapi_OMTM_Switch_Operating_Mode( uint32_t mode_Id, qapi_OMTM_Switch_At_t when );                                       
                                         
/** 
 * Simplified qapi_OMTM_Switch_Operating_Mode for use in MOM.
 *
 * @param[in] mode_Id Index into the Operating Mode table (array of OMs). 
 *
 * @return
 * The call does not return.  
 */
qapi_Status_t qapi_OMTM_MOM_Switch_Operating_Mode( uint32_t mode_Id );                                         
        
/** 
 * Switch the CONSS to the specified Memory Mode. If mode_Id is not
 * "FMM", CONSS will be switched to the specified mode only if there are no
 * votes to prevent CONSS from switching out of FMM.
 *
 * @param[in] mode_Id: One of the qapi_OMTM_ConSS_Mode_Id enums.
 *
 * @return
 * QAPI_OK on success, one of the other status codes on error
 */
qapi_Status_t qapi_OMTM_Switch_ConSS_Memory_Mode( qapi_OMTM_ConSS_Mode_Id mode_Id );

/** 
 * Must be invoked from the entryPoint of each OM to complete any
 * remaining tasks from the mode switch, including unlocking interrupts.
 *
 * @return
 * QAPI_OK on success, one of the other status codes on error.
 */
qapi_Status_t qapi_OMTM_Complete_Mode_Switch( void );

/** @} */ /* end_addtogroup qapi_omtm */

#ifdef __cplusplus
}
#endif

#endif /* QAPI_OMTM_H */
