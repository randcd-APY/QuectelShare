#ifndef __QAPI_DSR_H__
#define __QAPI_DSR_H__

/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/*==================================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

 $Header: //components/rel/core.ioe/1.0/v2/rom/release/api/platform/qapi_dsr.h#4 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------------
==================================================================================*/


/** @file qapi_dsr.h

  Function and data structure declarations for DSRs (Deferred Service
  Routines).
*/

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "com_dtypes.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef enum
{
  QAPI_DSR_PRI_LOW,
  QAPI_DSR_PRI_MED,
  QAPI_DSR_PRI_HIGH,
  QAPI_DSR_NUM_PRIO
} qapi_dsr_priority_t;

//typedef struct dsr_obj_s dsr_obj_t;
typedef struct qapi_dsr_obj_s qapi_dsr_obj_t;
typedef void (*qapi_dsr_hdlr_t)(qapi_dsr_obj_t *dsr_obj, void *ctxt, void *payload);

/*
struct qapi_dsr_obj_s
{
  struct qapi_dsr_obj *next;
  qapi_dsr_hdlr_t cb;
  void *ctxt;
  void *payload;
  qapi_dsr_priority_t priority;
  boolean pending;
};
*/

/** @addtogroup qapi_dsr
@{ */

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/

/**
 * @brief Creates a DSR object.
 *
 * @details This function creates a DSR object that can be enqueued later to
 *          defer work to thread context.
 *
 * @param[out] dsr_obj    DSR object.
 * @param[in]  cb         Callback function invoked in DSR thread context.
 * @param[in]  ctxt       Optional callback context; can be NULL.
 * @param[in]  priority   DSR priority.
 *
 * @return TRUE if successfully created; otherwise, FALSE.
 */
qapi_Status_t qapi_dsr_create(qapi_dsr_obj_t **dsr_obj, qapi_dsr_hdlr_t cb, void *ctxt, qapi_dsr_priority_t priority);

/**
 * @brief Destroys a DSR object.
 *
 * @details If the DSR is currently
 *          queued, it will be removed from the queue (callback function
 *          will not get invoked).
 *
 * @param[in] dsr_obj    DSR object.
 *
 * @return None.
 */
qapi_Status_t qapi_dsr_destroy(qapi_dsr_obj_t *dsr_obj);

/**
 * @brief Enqueues a DSR object into the DSR pending list.
 *
 * @details Worker threads process the pending list based on DSR priority.
 *
 *          Once a DSR is enqueued, it cannot be enqueued again (an object
 *          can only be inserted into a list once). In cases where drivers
 *          may attempt to enqueue a DSR that is already in the pending list, e.g.,
 *          handling interrupts, the DSR context should be maintained between
 *          the ISR and DSR.
 *
 *          Do not make blocking calls or wait-on events in DSR context.
 *
 *          If this function returns FALSE, it was not enqueued. If
 *          the DSR is already in the pending list, this function will
 *          return FALSE and the payload will not be updated.
 *
 * @param[in] dsr_obj    DSR object.
 * @param[in] payload    Optional payload context; can be NULL.
 *
 * @return TRUE if successfully enqueued; otherwise, FALSE.
 */
qapi_Status_t qapi_dsr_enqueue(qapi_dsr_obj_t *dsr_obj, void *payload);

#endif /* #ifndef __QAPI_DSR_H__ */

/** @} */ /* end_addtogroup qapi_dsr */
