
/******************************************************************************

                        ADPL_MAIN.C

******************************************************************************/

/******************************************************************************

  @file    adpl_main.c
  @brief   Accelerated Data Path Logging module

  DESCRIPTION
  Implementation of Accelerated Data Path Logging module.

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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <linux/if.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/inotify.h>

#include "adpl_cmdq.h"
#include "adpl.h"

/*===========================================================================
                              VARIABLE DECLARATIONS
===========================================================================*/

static adpl_param             adpl_config_param;
qcmap_sk_fd_set_info_t           sk_fdset;

/* Global Netlink Socket. variable */
qcmap_sk_info_t     sk_info;


/*===========================================================================
                              FUNCTION DEFINITIONS
===========================================================================*/

/*==========================================================================

FUNCTION PRINT_MSG()

DESCRIPTION

  This function is used to print QTI logs to a file

DEPENDENCIES
  None.

RETURN VALUE


SIDE EFFECTS
  None

==========================================================================*/

void PRINT_msg
(
  const char *funcName,
  uint32_t lineNum,
  const char*fmt, ...
)
{
}

/*==========================================================================

FUNCTION ADPL_CHECK_MHI_STATE()

DESCRIPTION

  Check the MHI state written by QTI daemon onto the file node.

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS

SIDE EFFECTS
  None

==========================================================================*/
void adpl_check_mhi_state()
{
  char         process[MAX_COMMAND_STR_LEN];
  FILE         *cmd = NULL;
  char         mode[ADPL_MAX_LEN] = {0};
  int          ret_val;
  int          state = 0;

  snprintf(process, MAX_COMMAND_STR_LEN, "cat %s", ADPL_NOTIFY_NODE);
  cmd = popen(process, "r");
  if(cmd != NULL)
  {
    fgets(mode, ADPL_MAX_LEN, cmd);
    state = atoi(mode);
    pclose(cmd);
  }
  if (state == adpl_ph_get_mhi_state())
  {
    LOG_MSG_INFO1("ADPL is already in the current mode %d", state,0,0);
    return;
  }
  if (state == MHI_DISCONNECTED)
  {
    LOG_MSG_INFO1("MHI is not enabled",0,0,0);
    ds_system_call("echo ADPL: MHI is not enabled > /dev/kmsg",
                             strlen("echo ADPL: MHI is not enabled > /dev/kmsg"));

    if(adpl_ph_set_mhi_state(MHI_DISCONNECTED) != ADPL_SUCCESS)
    {
      LOG_MSG_ERROR("Setting MHI state %d failed",MHI_DISCONNECTED,0,0);
    }
    else
    {
      adpl_process_ph_reset();
    }
  }
  else if (state == MHI_CONNECTED)
  {
    LOG_MSG_INFO1("MHI is enabled",0,0,0);
    ds_system_call("echo ADPL: MHI is enabled > /dev/kmsg",
                             strlen("echo ADPL: MHI is enabled > /dev/kmsg"));
    if (!adpl_config_param.ph_iface[PH_DRIVER_TYPE_USB].ph_enabled
        && (adpl_config_param.adpl_mode == PCIE_ONLY || adpl_config_param.adpl_mode == DEFAULT_MODE))
    {
      adpl_config_param.ph_iface[PH_DRIVER_TYPE_MHI].ph_enabled = true;
      if (!adpl_config_param.ph_iface[PH_DRIVER_TYPE_MHI].ph_iface_fd)
      {
        ret_val = adpl_file_open(adpl_config_param.ph_iface[PH_DRIVER_TYPE_MHI].ph_iface_device_file,
                              &(adpl_config_param.ph_iface[PH_DRIVER_TYPE_MHI].ph_iface_fd));

        if(ret_val == ADPL_FAILURE)
        {
          LOG_MSG_ERROR("Failed to open ADPL device file. Error %d",
                        errno, 0, 0);
          return;
        }
      }

      if(adpl_ph_set_mhi_state(MHI_CONNECTED) != ADPL_SUCCESS)
      {
        LOG_MSG_ERROR("Setting MHI state %d failed",MHI_CONNECTED,0,0);
      }
      else
      {
        adpl_process_ph_reset();
      }
    }
  }
  else
  {
    LOG_MSG_ERROR("Unable to read correctly",0,0,0);
  }
  return;
}

/*===========================================================================
  FUNCTION ADPL_MHI_STATE_RESET
=============================================================================
FUNCTION ADPL_MHI_STATE_RESET()

DESCRIPTION

  Listen to the MHI state whenever modified by QTI daemon on the file node.

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS

SIDE EFFECTS
  None
===========================================================================*/
int adpl_mhi_state_reset
(
  int mhi_state_fd
)
{
  int length;
  char buffer[INOTIFY_BUF_LEN] = {0};
  uint32_t mask = IN_MODIFY;

  LOG_MSG_INFO1("Received notifications in dir %s", ADPL_NOTIFY_FOLDER, 0, 0);

  struct inotify_event* event = NULL ;

  length = read(mhi_state_fd, buffer, INOTIFY_BUF_LEN);

  if (length < 0)
  {
    LOG_MSG_ERROR("inotify read() error return length: %d and mask: 0x%x\n", length, mask, 0);
    return ADPL_FAILURE;
  }
  event = (struct inotify_event*)calloc(1,length);
  if(event == NULL)
  {
    LOG_MSG_ERROR("Unable to allocate memory to event", 0, 0, 0);
    return ADPL_FAILURE;
  }

  memcpy(event, buffer, length);

  if (event->len > 0)
  {
     if (!(event->mask & IN_ISDIR))
     {
       if(event->mask & IN_MODIFY)
       {
         LOG_MSG_INFO1("event mask is modify", 0, 0, 0);
         if(strncmp(event->name, ADPL_NOTIFY_FILE_NAME, strlen(ADPL_NOTIFY_FILE_NAME))==0)
         {
           LOG_MSG_INFO1("QTI modified the file",0,0,0);
           adpl_check_mhi_state();
         }
       }
     }
  }
  free(event);

  return ADPL_SUCCESS;
}

/*===========================================================================
  FUNCTION ADPL_READ_CONFIG
=============================================================================
FUNCTION ADPL_READ_CONFIG()

DESCRIPTION

  Read the config(USB only/PCIe only/dynamic) ADPL needs to
  support on boot-up

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS

SIDE EFFECTS
  None
===========================================================================*/
void adpl_read_config
(
  adpl_param           * adpl_state
)
{
  char         process[MAX_COMMAND_STR_LEN];
  FILE         *cmd = NULL;
  char         mode[ADPL_MAX_LEN] = {0};
  int          config;
  FILE         *fp;

  fp = fopen(ADPL_CONFIG_FILE, "r");
  if (fp!=NULL)
  {
    snprintf(process, MAX_COMMAND_STR_LEN, "cat %s | grep -i %s | grep -Eo '[0-9]'",
             ADPL_CONFIG_FILE, ADPL_MODE_TAG);
    cmd = popen(process, "r");
    if(cmd != NULL)
    {
      fgets(mode, ADPL_MAX_LEN, cmd);
      config = atoi(mode);
      if (config <= USB_ONLY && config >= DEFAULT_MODE)
      {
        adpl_state->adpl_mode = config;
      }
      else
      {
        LOG_MSG_ERROR("Invalid Configuration entered. Falling back to default mode",0,0,0);
        adpl_state->adpl_mode = DEFAULT_MODE;
      }
      pclose(cmd);
    }

  }
  else
  {
    adpl_state->adpl_mode = DEFAULT_MODE;
  }

   return;
}

/*==========================================================================

FUNCTION MAIN()

DESCRIPTION

  The main function for ADPL which is first called when ADPL gets started on
  boot up.

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS
  -1 on FAILURE

SIDE EFFECTS
  None

==========================================================================*/
int main(int argc, char ** argv)
{
  int                       ret_val;
  int                       inotify_fd;
  uint32_t                  mask;
  int                       wd;

/*-----------------------------------------------------------------------
    Initialize ADPL variables
------------------------------------------------------------------------*/
  memset(&sk_fdset, 0, sizeof(qcmap_sk_fd_set_info_t));
  memset(&adpl_config_param, 0, sizeof(adpl_param));

/*-------------------------------------------------------------------------
  Initializing Diag for QXDM logs
-------------------------------------------------------------------------*/
  if (TRUE != Diag_LSM_Init(NULL))
  {
     printf("Diag_LSM_Init failed !!");
  }

  LOG_MSG_INFO1("Start ADPL", 0, 0, 0);
  adpl_read_config(&adpl_config_param);
  inotify_fd = inotify_init();
  mask = IN_MODIFY;

  if (inotify_fd < 0)
  {
    LOG_MSG_ERROR("Error in inotify_init for checking the mhi state", 0, 0, 0);
  }
  else
  {
    wd = inotify_add_watch(inotify_fd, ADPL_NOTIFY_FOLDER, mask);
    if(wd > 0)
    {
      LOG_MSG_INFO1("Successfully got the watch descriptor for checking the mhi state", 0, 0, 0);
      if( qcmap_nl_addfd_map(&sk_fdset,
                             inotify_fd,
                             adpl_mhi_state_reset,
                             MAX_NUM_OF_FD) != ADPL_SUCCESS)
      {
        LOG_MSG_ERROR("Failed to map inotify fd with the check mhi state function",0,0,0);
        inotify_rm_watch(inotify_fd, wd);
        close(inotify_fd);
        inotify_fd = 0;
        return ADPL_FAILURE;
      }
    }
    else
    {
      LOG_MSG_ERROR("Failed to add a watch to the ADPL notify folder",0,0,0);
      close(inotify_fd);
      inotify_fd = 0;
    }
  }


  adpl_config_param.target = ds_get_target();

  memcpy(adpl_config_param.ph_iface[PH_DRIVER_TYPE_USB].ph_iface_device_file,
         ADPL_USB_DEV_FILE,
         strlen(ADPL_USB_DEV_FILE));

  memcpy(adpl_config_param.ph_iface[PH_DRIVER_TYPE_MHI].ph_iface_device_file,
         ADPL_MHI_DEV_FILE,
         strlen(ADPL_MHI_DEV_FILE));

/*---------------------------------------------------------------------
  Initialize peripheral interface for ADPL logging
---------------------------------------------------------------------*/
  ret_val = adpl_ph_init(&adpl_config_param,
                        &sk_fdset,
                        adpl_ph_recv_msg);
  if(ret_val != ADPL_SUCCESS)
  {
    LOG_MSG_ERROR("Failed to initialize peripheral interface for ADPL logging",
                  0, 0, 0);
  }
/*---------------------------------------------------------------------
  Initialize ADPL modem interface
---------------------------------------------------------------------*/
  if ((DS_TARGET_FUSION4_5_PCIE == adpl_config_param.target ||
       DS_TARGET_FUSION_8084_9X45 == adpl_config_param.target ||
       DS_TARGET_FUSION_8096_9X55 == adpl_config_param.target))
  {
    ret_val = adpl_modem_init_thru_qmuxd(&adpl_config_param,
                                        (qcmap_sock_thrd_fd_read_f)adpl_modem_recv_msg_thru_qmuxd);
    if(ret_val != ADPL_SUCCESS)
    {
      LOG_MSG_ERROR("Failed to initialize ADPL modem interface",
                    0, 0, 0);
      return ADPL_FAILURE;
    }
  }

/*---------------------------------------------------------------------
      Initialize DPL command queue
----------------------------------------------------------------------*/
    adpl_cmdq_init();

/*---------------------------------------------------------------------
  Initialize DPM client
---------------------------------------------------------------------*/

    if(DS_TARGET_FUSION4_5_PCIE != adpl_config_param.target &&
       DS_TARGET_FUSION_8084_9X45 != adpl_config_param.target &&
       DS_TARGET_FUSION_8096_9X55 != adpl_config_param.target)
    {
      ret_val = adpl_modem_init(&adpl_config_param);
      if(ret_val != ADPL_SUCCESS)
      {
        LOG_MSG_ERROR("Failed to initialize Modem",
                      0, 0, 0);
        return ADPL_FAILURE;
      }

      ret_val = adpl_dpm_init(&adpl_config_param);
      if(ret_val != ADPL_SUCCESS)
      {
        LOG_MSG_ERROR("Failed to initialize DPM",
                      0, 0, 0);
        return ADPL_FAILURE;
      }
    }


/*--------------------------------------------------------------------
  Start the listener which listens to netlink events and QMI packets
  coming on USB-Rmnet device file
---------------------------------------------------------------------*/
  ret_val = qcmap_listener_start(&sk_fdset);

  if(ret_val != QCMAP_UTIL_SUCCESS)
  {
    LOG_MSG_ERROR("Failed to start NL listener",
                   0, 0, 0);
  }

  return ADPL_SUCCESS;
}
