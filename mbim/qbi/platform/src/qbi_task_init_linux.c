/*!
  @file
  qbi_task_linux.c

  @brief
  Platform-specific task initialization functionality for the Linux environment.
  Spawns a main QBI thread and a reader thread.
*/

/*=============================================================================

  Copyright (c) 2012-2013, 2018 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
04/02/18  ha   Modified mhi uevent parser
03/19/18  ha   Add support for mhi uevent and deprecate pcie_enabled NV
01/30/18  ha   Add support to monitor mhi_ctrl
04/01/13  bd   Move platform-specific device service init calls to task_init
02/07/12  bd   Added module
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"
#include "qbi_hc.h"
#include "qbi_hc_linux.h"
#include "qbi_mem.h"
#include "qbi_os.h"
#include "qbi_svc.h"
#include "qbi_svc_ext_qmux.h"
#include "qbi_svc_qdu.h"
#include "qbi_svc_qmbe.h"
#include "qbi_task.h"

#include "diag_lsm.h"

#include <pthread.h>
#include <stdio.h>

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/*=============================================================================

  Private Typedefs

=============================================================================*/

/*=============================================================================

  Private Variables

=============================================================================*/

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static void qbi_task_init_linux_init_svcs
(
  void
);

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_task_init_linux_init_svcs
===========================================================================*/
/*!
    @brief Initialize device services with a platform-specific component or
    those that may not be supported on all platforms

    @details
*/
/*=========================================================================*/
static void qbi_task_init_linux_init_svcs
(
  void
)
{
/*-------------------------------------------------------------------------*/
  qbi_svc_ext_qmux_init();
  qbi_svc_qdu_init();
  qbi_svc_qmbe_init();
} /* qbi_task_init_linux_init_svcs() */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: main
===========================================================================*/
/*!
    @brief QBI process entry function

    @details
    Initializes QBI, spawns the reader thread, and enters the main task
    loop.

    @return int
*/
/*=========================================================================*/
int main
(
  void
)
{
  int rc = 0;
  int pthread_err;
/*-------------------------------------------------------------------------*/
  qbi_os_log_event(QBI_OS_LOG_EVENT_QBI_INIT);
  if (!Diag_LSM_Init(NULL))
  {
    printf("Couldn't initialize Diag!");
    rc = -2;
  }
  else
  {
    QBI_LOG_STR_2("Build date: %s %s", __DATE__, __TIME__);

    /* Prepare QBI modules for handling commands, etc */
    qbi_os_init();
    qbi_mem_init();
    qbi_task_init();
    qbi_hc_init();
    qbi_svc_init();
    qbi_task_init_linux_init_svcs();

    if (!qbi_hc_linux_dev_init())
    {
      QBI_LOG_E_0("Couldn't open device file; bailing out");
      rc = -1;
    }
    else
    {
      /* Spawn the host communications reader thread, which will block on read()
         and pass incoming commands to the main thread */
      pthread_err = qbi_hc_linux_start_reader_thread();
      if (pthread_err != 0)
      {
        QBI_LOG_E_1("Error %d while creating reader thread!", pthread_err);
      }
      else
      {
        /* Check whether in previous session mbim closed unexpectedly
           If so triggers mbim recovery mechanism */
        qbi_hc_linux_crash_recovery_check_session_file();
        qbi_task_run();
      }
    }
  }

  return rc;
} /* main() */

