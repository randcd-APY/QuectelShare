/******************************************************************************

                        QBRIDGE_MAIN.C

******************************************************************************/

/******************************************************************************

  @file    qbridge_main.c
  @brief   Handle initialization of basic routine for communication with
           modem and USB Layer

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
05/02/18    rv        Initial version

******************************************************************************/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#include "qbridge_log_qxdm.h"
#include "qbridge_qmux.h"
#include "qbridge_main.h"
#include "qbridge_smd.h"
#include "qbridge_dpm.h"
#include "data_port_mapper_v01.h"
#include "diag_lsm.h"

qbridge_rmnet_param  qbridge_config_param;

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbridge_qmux_rx_cb
===========================================================================*/
/*!
    @brief Processes QMUX data from the modem

    @details
    Runs in the context of the Modem task. This function must eventually free
    the buffer containing the QMUX message received from the modem.

    @param rx_msg
    @param rx_msg_len
*/
/*=========================================================================*/
static void qbridge_qmux_rx_cb
(
  unsigned char  *rx_msg,
  int  rx_msg_len
)
{
  qbridge_qmux_msg_s *qmux_msg = NULL;
/*-------------------------------------------------------------------------*/
  QBRIDGE_CHECK_NULL_PTR_RET(rx_msg);

  qmux_msg = (qbridge_qmux_msg_s *) rx_msg;

  QBRIDGE_LOG_D_1("QMUXBRIDGE :: Received QMUX Len %d ,Going to "
                  "write.",rx_msg_len);
  qbridge_send_msg_to_host((qbridge_qmux_msg_s *)qmux_msg,
                            rx_msg_len);

  return;
} /* qbridge_qmux_rx_cb () */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbridge_set_state
===========================================================================*/
/*!
    @brief Set the qmuxbridge internal state machine

    @details Set the qmuxbridge internal state machine

    @param state to be set to

    @return void
*/
/*=========================================================================*/
void qbridge_set_state(qbridge_state_e state)
{
/*--------------------------------------------------------------------------*/
  qbridge_config_param.state = state;
  QBRIDGE_LOG_D_1("QMUXBRIDGE :: Setting state to %d",
                  qbridge_config_param.state);

  return;
}/* qbridge_set_state */

/*===========================================================================
  FUNCTION: qbridge_query_state
===========================================================================*/
/*!
    @brief Queries the current qmuxbridge internal state machine

    @details Queries the qmuxbridge internal state machine

    @param none

    @return qbridge_state_e
*/
/*=========================================================================*/
qbridge_state_e qbridge_query_state()
{
  qbridge_state_e state;
/*--------------------------------------------------------------------------*/
  state = qbridge_config_param.state;
  QBRIDGE_LOG_D_1("QMUXBRIDGE :: Current state %d",
                  qbridge_config_param.state);

  return state;
}/* qbridge_query_state */

/*===========================================================================
  FUNCTION: qbridge_initialize_modem_interface
===========================================================================*/
/*!
    @brief Initializes path towards modem

    @details
    Opens DPM ports,smd ports and initializes QMI client to
    handle SSR events

    @param None

    @return int
*/
/*=========================================================================*/
int qbridge_initialize_modem_interface()
{
  int ret_val = 0;
/*--------------------------------------------------------------------------*/

  /* Open DPM Ports */
  ret_val =  qbridge_dpm_open(&qbridge_config_param);
  if (ret_val != QBRIDGE_TRUE)
  {
    QBRIDGE_LOG_E_0("QMUXBRIDGE :: Error initializing DPM.Aborting Init.");
    return QBRIDGE_FAILURE;
  }
  else
  {
    QBRIDGE_LOG_I_0("QMUXBRIDGE :: Initialized DPM.");
  }

  /* Initialize QMUX Interface and Register for Callbacks */
  ret_val = qbridge_qmux_smd_init(&qbridge_config_param);
  if (ret_val != QBRIDGE_SUCCESS)
  {
    QBRIDGE_LOG_E_0("QMUXBRIDGE :: Error initializing QMUX SMD.");
    return QBRIDGE_FAILURE;
  }
  else
  {
    QBRIDGE_LOG_I_0("QMUXBRIDGE :: Initialized SMD Interface.");
  }

  ret_val = qbridge_qmi_init_client(&qbridge_config_param);
  if (ret_val != QBRIDGE_TRUE)
  {
    QBRIDGE_LOG_E_0("QMUXBRIDGE :: Error Registering QMI for Error Callbacks.");
    return QBRIDGE_FAILURE;
  }
  else
  {
    QBRIDGE_LOG_I_0("QMUXBRIDGE :: Registered QMI for Error Callbacks.");
  }

  /* We can safely assume here that now QMUXBRIDGE is open */
  qbridge_set_state(QBRIDGE_STATE_OPENED);

  return ret_val;
}/* qbridge_initialize_modem_interface */

/*===========================================================================
  FUNCTION: qbridge_close_interface_towards_modem
===========================================================================*/
/*!
    @brief Close the device file which is used for interfacing with
           modem

    @details

    @param None

    @return void
*/
/*=========================================================================*/
void qbridge_close_interface_towards_modem
(
  void
)
{
  int ret_val = 0;
/*-------------------------------------------------------------------------*/
  ret_val = qbridge_dpm_close(&qbridge_config_param);
  if (ret_val != QBRIDGE_TRUE)
  {
    QBRIDGE_LOG_E_0("QMUXBRIDGE :: Error Closing DPM Ports.");
  }
  else
  {
    QBRIDGE_LOG_D_0("QMUXBRIDGE :: DPM Port Closed.");
  }

  qbridge_qmux_smd_close(&qbridge_config_param);

  QBRIDGE_LOG_D_1("QMUXBRIDGE :: DPM/SMD Close Complete. SSR "
                  "flag %d",qbridge_config_param.ssr_in_progress);

  qbridge_set_state(QBRIDGE_STATE_CLOSED);

  if (TRUE == qbridge_config_param.ssr_in_progress)
  {
    /* SSR Occured.Need to reopen interface towards modem */
    ret_val = qbridge_initialize_modem_interface();
    qbridge_config_param.ssr_in_progress = FALSE;
  }

  return;
}/* qbridge_close_interface_towards_modem() */

/*===========================================================================
  FUNCTION: qbridge_qmux_smd_write_msg
===========================================================================*/
/*!
    @brief Write QMUX message to SMD port.

    @details

    @param fd
    @param write_msg
    @param write_msg_len

    @return boolean
*/
/*=========================================================================*/
boolean qbridge_qmux_smd_write_msg
(
  int  fd,
  unsigned char  *write_msg,
  uint32  write_msg_len
)
{
  int rc = 0;
/*-------------------------------------------------------------------------*/
  QBRIDGE_CHECK_NULL_PTR_RET_FALSE(write_msg);

  if (fd < 0)
  {
    QBRIDGE_LOG_E_0("QMUXBRIDGE :: Write aborted - SMD port not open");
    return QBRIDGE_FAILURE;
  }

  if ((rc = write(fd, (void *) write_msg,
                  write_msg_len))
      < 0)
  {
    QBRIDGE_LOG_E_2("QMUXBRIDGE :: Write failed, rc = %d, "
                    "errno = %d!\n", rc, errno);
    return QBRIDGE_FAILURE;
  }
  else
  {
    QBRIDGE_LOG_D_1("QMUXBRIDGE :: %d bytes Data Succefully written "
                    "to modem",write_msg_len);
  }

  return QBRIDGE_TRUE;
} /* qbridge_qmux_smd_write_msg() */

/*===========================================================================
  FUNCTION: qbridge_send_msg_to_host
===========================================================================*/
/*!
    @brief send QMI message to peripheral

    @details

    @param write_msg
    @param write_msg_len

    @return  void
*/
/*=========================================================================*/
void qbridge_send_msg_to_host
(
  void  *data,
  int32  len
)
{
  int ret = QBRIDGE_ERROR;
  int ph_iface_fd;
/*-----------------------------------------------------------------------*/
  QBRIDGE_CHECK_NULL_PTR_RET(data);

  if ( qbridge_config_param.ph_iface.usb_enabled )
  {
    ph_iface_fd = qbridge_config_param.ph_iface.ph_iface_fd;
  }
  else
  {
    QBRIDGE_LOG_E_0("QMUXBRIDGE :: Couldn't set ph_iface_fd");
    return;
  }

  QBRIDGE_LOG_D_2("QMUXBRIDGE :: Going to write buffer of size %d on fd %d",
                   len, ph_iface_fd);

  ret = write(ph_iface_fd, (char*)data, len);
  if (ret == QBRIDGE_ERROR)
  {
    QBRIDGE_LOG_E_1("QMUXBRIDGE :: Couldn't send message to host. "
                    "Errno %d", errno);
  }
  else if (ret != len)
  {
    QBRIDGE_LOG_E_2("QMUXBRIDGE :: Unexpected return value when writing "
                    "to device file: got %d,expected %d",ret,len);
  }
  else
  {
    QBRIDGE_LOG_D_2("QMUXBRIDGE :: Successfully sent message to host "
                    "size %d on fd %d",ret, ph_iface_fd);
  }
} /* void qbridge_send_msg_to_host */

/*===========================================================================
  FUNCTION: qbridge_file_open
===========================================================================*/
/*!
    @brief opens the device file which is used for interfacing with peripheral
    (USB)

    @details

    @param fd

    @return
*/
/*=========================================================================*/

int qbridge_file_open
(
  int  *fd
)
{
  int qbridge_rmnet_fd = QBRIDGE_ERROR;
  int retry_count = 0;
/*--------------------------------------------------------------------------*/
  while (retry_count < QBRIDGE_FILE_NODE_OPEN_MAX_RETRY)
  {
    qbridge_rmnet_fd = open(QBRIDGE_USB_PH_DEV_FILE, O_RDWR);
    if(qbridge_rmnet_fd < 0)
    {
      QBRIDGE_LOG_E_1("QMUXBRIDGE :: Could not open device file. errno %d ",
                       errno);
      sleep(1);
      retry_count++;
      continue;
    }
    else
    {
      QBRIDGE_LOG_I_1("QMUXBRIDGE :: Successfully opened USB dev file."
                      "fd = %d",qbridge_rmnet_fd);
      *fd = qbridge_rmnet_fd;
      qbridge_config_param.ph_iface.usb_enabled = QBRIDGE_TRUE;
      return QBRIDGE_SUCCESS;
    }
  }

  if (retry_count == QBRIDGE_FILE_NODE_OPEN_MAX_RETRY)
  {
    QBRIDGE_LOG_E_1("QMUXBRIDGE :: Could not open device file. Abort. "
                    "errno %d ",errno);
    return QBRIDGE_FAILURE;
  }

  return QBRIDGE_SUCCESS;
}

/*===========================================================================
  FUNCTION: main
===========================================================================*/
/*!
    @brief The main function is first called when QMUXBRIDGE
           gets started

    @details

    @param argc
    @param argv

    @return int
*/
/*=========================================================================*/
int main(int argc, char ** argv)
{
  int ret_val = 0;
/*--------------------------------------------------------------------------*/

  if (!Diag_LSM_Init(NULL))
  {
    QBRIDGE_LOG_D_0("QMUXBRIDGE :: Couldn't initialize Diag!.");
  }
  else
  {
    QBRIDGE_LOG_D_0("QMUXBRIDGE :: Diag Initialized!.");
  }

  QBRIDGE_LOG_STR_2("QMUXBRIDGE :: Build date: %s %s", __DATE__, __TIME__);

  QBRIDGE_MEMSET(&qbridge_config_param, 0, sizeof(qbridge_rmnet_param));

  qbridge_set_state(QBRIDGE_STATE_OPENING);

  QBRIDGE_MEMSCPY(qbridge_config_param.ph_iface.ph_iface_device_file,
                  sizeof(qbridge_config_param.ph_iface.ph_iface_device_file),
                  QBRIDGE_USB_PH_DEV_FILE,
                  strlen(QBRIDGE_USB_PH_DEV_FILE));

  /* Opening USB Interface */
  ret_val = qbridge_file_open(&(qbridge_config_param.ph_iface.ph_iface_fd));

  if(ret_val != QBRIDGE_SUCCESS)
  {
    QBRIDGE_LOG_E_1("QMUXBRIDGE :: Failed to open USB peripheral device file. "
                    "Abort. Error %d",errno);
    return QBRIDGE_FAILURE;
  }

  ret_val = qbridge_initialize_modem_interface();
  if (ret_val != QBRIDGE_TRUE)
  {
    QBRIDGE_LOG_E_0("QMUXBRIDGE :: Error initializing path towards modem.");
    return QBRIDGE_FAILURE;
  }

  QBRIDGE_LOG_STR_1("QMUXBRIDGE :: Blocking on read on %s",
                    QBRIDGE_USB_PH_DEV_FILE);

  /* This will cause main to block.
     Wait for QMUX packets from HOST */
  (void)qbridge_qmux_smd_wait(&qbridge_config_param);

  /* If we are here it means USB node went bad and
     did not recover after trails.Hence exit gracefully */
  (void)qbridge_close_interface_towards_modem();

  QBRIDGE_LOG_D_0("QMUXBRIDGE :: Stopping QMUXBRIDGE");

  return ret_val;
}

/*===========================================================================
  FUNCTION: qbridge_qmux_tx_to_modem
===========================================================================*/
/*!
    @brief Sends a raw QMUX message to the modem

    @details
    If this function returns TRUE, then the request was successfully
    transmitted to the modem.

    @param qmux_msg
    @param qmux_msg_len

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbridge_qmux_tx_to_modem
(
  qbridge_qmux_msg_s  *qmux_msg,
  uint32  qmux_msg_len
)
{
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBRIDGE_CHECK_NULL_PTR_RET_FALSE(qmux_msg);

  QBRIDGE_LOG_D_2("QMUXBRIDGE :: Writing %d bytes on fd %d", 
           qmux_msg_len,qbridge_config_param.qbridge_qmux_smd_info.dev_fd);
  success = qbridge_qmux_smd_write_msg(
                  qbridge_config_param.qbridge_qmux_smd_info.dev_fd,
                  (unsigned char *)qmux_msg,
                  qmux_msg_len);

  return success;
} /* qbridge_qmux_tx_to_modem() */
