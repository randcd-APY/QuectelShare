/******************************************************************************

                          ADPL _ C M D Q . H

******************************************************************************/

/******************************************************************************

  @file    adpl_cmdq.h
  @brief   Accelerated Data Path Logging module command queues header file

  DESCRIPTION
  Header file for Accelerated Data Path Logging module command queues.

  ---------------------------------------------------------------------------
  Copyright (c) 2012-2014,2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

******************************************************************************/


/******************************************************************************

                      EDIT HISTORY FOR FILE

  $Id:$

when       who        what, where, why
--------   ---        -------------------------------------------------------
04/25/18   vv         Initial version

******************************************************************************/

#ifndef __ADPL_CMDQ_H__
#define __ADPL_CMDQ_H__

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include "ds_cmdq.h"
#include "adpl.h"


/*===========================================================================
                      DEFINITIONS AND DECLARATIONS
===========================================================================*/

#define ADPL_CMDQ_SUCCESS (0)

/*---------------------------------------------------------------------------
   Type of a Executive event data
---------------------------------------------------------------------------*/
typedef struct adpl_cmdq_cmd_data_s {
  adpl_event_e event;
} adpl_cmdq_cmd_data_t;

/*---------------------------------------------------------------------------
   Type of a Executive command
---------------------------------------------------------------------------*/
typedef struct adpl_cmdq_cmd_s {
  ds_cmd_t               cmd;                /* Command object         */
  adpl_cmdq_cmd_data_t    data;               /* Command data           */
  int                    tracker;            /* 1 if alloc, else 0     */
} adpl_cmdq_cmd_t;

/*---------------------------------------------------------------------------
   Type representing collection of state information for module
---------------------------------------------------------------------------*/
struct adpl_cmdq_state_s {
  struct ds_cmdq_info_s       cmdq;  /* Command queue for async processing */
};


/*===========================================================================
                            GLOBAL FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  adpl_cmdq_get_cmd
===========================================================================*/
/*!
@brief
  Function to get a command buffer for asynchronous processing

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - Allocated heap memory
*/
/*=========================================================================*/
adpl_cmdq_cmd_t * adpl_cmdq_get_cmd ( void );

/*===========================================================================
  FUNCTION  adpl_cmdq_release_cmd
===========================================================================*/
/*!
@brief
  Function to release a command buffer

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - Returns memory to heap
*/
/*=========================================================================*/
void adpl_cmdq_release_cmd ( adpl_cmdq_cmd_t * );

/*===========================================================================
  FUNCTION  adpl_cmdq_put_cmd
===========================================================================*/
/*!
@brief
  Function to post a command buffer

@return
  int - ADPL_CMDQ_SUCCESS on successful operation, ADPL_CMDQ__FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int adpl_cmdq_put_cmd ( const adpl_cmdq_cmd_t * cmdbuf );

/*===========================================================================
  FUNCTION  adpl_cmdq_wait
===========================================================================*/
/*!
@brief
  Forces calling thread to wait on exit of command queue thread.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - Calling thread is blocked indefinitely
*/
/*=========================================================================*/
void adpl_cmdq_wait ( void );

/*===========================================================================
  FUNCTION  adpl_cmdq_init
===========================================================================*/
/*!
@brief
 Main initialization routine of the executive control module.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
extern void adpl_cmdq_init (void);

/*===========================================================================
  FUNCTION adpl_get_cmdq_length
===========================================================================*/
/*!
@brief
 Function returns number of outstanding messages in command queue.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int adpl_get_cmdq_length( void );

#endif /* __ADPL_CMDQ_H__ */

