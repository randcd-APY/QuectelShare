/******************************************************************************

                         ADPL _ C M D Q . C

******************************************************************************/


/******************************************************************************

  @file    adpl_cmdq.c
  @brief   Accelerated Data Path Logging module command queues

  DESCRIPTION
  Implementation of Accelerated Data Path Logging module command queues.

  ---------------------------------------------------------------------------
  Copyright (c) 2012-2015,2018 Qualcomm Technologies, Inc.
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

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>

#include "stm2.h"
#include "ds_list.h"
#include "ds_cmdq.h"


#include "adpl_cmdq.h"



/*===========================================================================
                     LOCAL DEFINITIONS AND DECLARATIONS
===========================================================================*/


/*---------------------------------------------------------------------------
   Constant representing maximum number of command buffers used by this
   module
---------------------------------------------------------------------------*/
#define ADPL_CMDQ_MAX_CMDS 2

/*---------------------------------------------------------------------------
   Executive control state information
---------------------------------------------------------------------------*/
LOCAL struct adpl_cmdq_state_s  adpl_cmdq_state_info;

/*===========================================================================
                            FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  adpl_cmdq_cmd_free
===========================================================================*/
/*!
@brief
 Virtual function registered with the Command Thread to free a
 command buffer, after execution of the command is complete.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
adpl_cmdq_cmd_free( ds_cmd_t * cmd, void * data )
{
  adpl_cmdq_cmd_t * cmd_buf;

  (void) cmd;

  /* Get qmi cmd ptr from user data ptr */
  cmd_buf = (adpl_cmdq_cmd_t *)data;

  /* Unset tracker for debug purposes */
  cmd_buf->tracker = 0;

  /* Release dynamic memory */
  free( cmd_buf );

  LOG_MSG_INFO1("adpl_cmdq: free one commmand data",0,0,0);

  return;
}

/*===========================================================================
  FUNCTION  adpl_cmdq_cmd_process
===========================================================================*/
/*!
@brief
 Virtual function registered with the Command Thread to process a
 command buffer.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
adpl_cmdq_cmd_process (ds_cmd_t * cmd, void * data)
{
  adpl_cmdq_cmd_t * cmd_buf;
  int sockfd, len, numBytes;
  struct sockaddr_un sta_qmi_ip;
  char buffer;

  (void) cmd;

  /* Get qmi cmd ptr from user data ptr */
  cmd_buf = (adpl_cmdq_cmd_t *)data;

  if (cmd_buf->data.event == ADPL_DPM_MODEM_NOT_IN_SERVICE_EVENT )
  {
    adpl_modem_not_in_service();
  }
  else if (cmd_buf->data.event == ADPL_DPM_INIT_EVENT )
  {
    if (dpm_client_init() != ADPL_SUCCESS )
    {
      LOG_MSG_ERROR("Unable to init DPM client!!",0, 0, 0);
    }
  }
  return;
}

/*===========================================================================
  FUNCTION  adpl_cmdq_cleanup
===========================================================================*/
/*!
@brief
  Performs cleanup of executive module.  Invoked at process termination.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
adpl_cmdq_cleanup
(
  void
)
{
  /* Purge command queue to release heap memory */
  (void)ds_cmdq_deinit( &adpl_cmdq_state_info.cmdq );
}

/*===========================================================================
                            GLOBAL FUNCTION DEFINITIONS
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
adpl_cmdq_cmd_t * adpl_cmdq_get_cmd ( void )
{
  adpl_cmdq_cmd_t * cmd_buf = NULL;

  /* Allocate command buffer */
  if((cmd_buf = malloc(sizeof(adpl_cmdq_cmd_t))) == NULL )
  {
    LOG_MSG_INFO1("adpl_cmdq: malloc failed\n",0,0,0);
    return NULL;
  }

  /* Assign self-reference in DS cmd payload */
  cmd_buf->cmd.data      = (void*)cmd_buf;
  cmd_buf->tracker       = 1;

  /* Asssign default execution and free handlers */
  cmd_buf->cmd.execute_f = adpl_cmdq_cmd_process;
  cmd_buf->cmd.free_f    = adpl_cmdq_cmd_free;

  return cmd_buf;
}

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
void adpl_cmdq_release_cmd ( adpl_cmdq_cmd_t * cmd_buf )
{
  if( cmd_buf->cmd.free_f )
  {
    cmd_buf->cmd.free_f( &cmd_buf->cmd, cmd_buf->cmd.data );
  }
  else
  {
    LOG_MSG_INFO1("adpl_cmdq: Specified buffer not valid, ignoring\n",0,0,0);
  }
  return;
}

/*===========================================================================
  FUNCTION  adpl_cmdq_put_cmd
===========================================================================*/
/*!
@brief
  Function to post a command buffer for asynchronous processing

@return
  int - 0 on successful operation, -1 otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int adpl_cmdq_put_cmd ( const adpl_cmdq_cmd_t * cmdbuf )
{
  /* Append command buffer to the command queue */
  int result = ds_cmdq_enq( &adpl_cmdq_state_info.cmdq, &cmdbuf->cmd );

  return result;
}

/*===========================================================================
  FUNCTION  adpl_cmdq_wait
===========================================================================*/
/*!
@brief
  Forces calling thread to wait on exit of command processing thread.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - Calling thread is blocked indefinitely
*/
/*=========================================================================*/
void adpl_cmdq_wait ( void )
{
  ds_cmdq_join_thread( &adpl_cmdq_state_info.cmdq );
}


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
void adpl_cmdq_init ( void )
{
 /* Register process termination cleanup handler */
  atexit( adpl_cmdq_cleanup );

  /*-------------------------------------------------------------------------
    Initialize command queue for asynch processing
  -------------------------------------------------------------------------*/
  ds_cmdq_init( &adpl_cmdq_state_info.cmdq, ADPL_CMDQ_MAX_CMDS );

  return;
}


/*===========================================================================
  FUNCTION  adpl_get_cmdq_length
===========================================================================*/
/*!
@brief
 Function returns the number of outstanding messages in command queue.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int adpl_get_cmdq_length( void )
{
  return adpl_cmdq_state_info.cmdq.nel;
}
