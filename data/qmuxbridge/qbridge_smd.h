/******************************************************************************

                        QBRIDGE_SMD.H

******************************************************************************/

/******************************************************************************

  @file    qbridge_smd.h
  @brief   Handles initialization and interaction with SMD Interface

  ---------------------------------------------------------------------------
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------


******************************************************************************/


/******************************************************************************

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when       who        what, where, why
--------   ---        -------------------------------------------------------
1/15/18    rv         Initial version

******************************************************************************/

/*=============================================================================

  Constants and Macros

=============================================================================*/

#define SMD_BLOCKING_WRITE_TRUE  (1)
#define SMD_BLOCKING_WRITE_FALSE (0)

#define SMD_TRUE                 (1)
#define SMD_FALSE                (0)

#define DEFAULT_FD               (-1)
#define SMD_DATA_BUF_LEN         (4096)
#define SMD_DATA_TEMP_BUF_LEN    (10240)

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/


/*===========================================================================
  FUNCTION: qbridge_qmux_smd_init
===========================================================================*/
/*!
    @brief Opens QMUX SMD port and starts receiver thread for QMUX msg IO.

    @details
    Open QMUX SMD device file, create rx thread then send sync msg.

    @param rx_cb_f Callback function executed for each received QMUX msg

    @return boolean
*/
/*=========================================================================*/
boolean qbridge_qmux_smd_init
(
  qbridge_rmnet_param  *qbridge_config_param
);

/*===========================================================================
  FUNCTION: qbridge_qmux_smd_wait()
===========================================================================*/
/*!
    @brief 

    @details Wait for QMUX thread to exit

    @param

    @return void
*/
/*=========================================================================*/
void qbridge_qmux_smd_wait
(
  qbridge_rmnet_param  *qbridge_config_param
);

/*===========================================================================
  FUNCTION: qbridge_qmux_smd_close
===========================================================================*/
/*!
    @brief Cancel receiver thread and close QMUX SMD port.

    @details
    Cancel rx thread and close SMD port

    @param

    @return void
*/
/*=========================================================================*/
void qbridge_qmux_smd_close
(
  qbridge_rmnet_param  *qbridge_config_param
);