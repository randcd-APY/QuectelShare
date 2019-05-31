/*!
  @file
  qbi_hc_linux.c

  @brief
  QBI Host Communications layer for the Linux platform.
*/

/*=============================================================================

  Copyright (c) 2012-2015, 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
06/05/18  mm   Avoding the race condition between write and mbim kill operation
04/02/18  ha   Modified mhi uevent parser
03/19/18  ha   Add support for mhi uevent and deprecate pcie_enabled NV
03/13/18  ha   Change smdcntl8/DATA40 to smdcntl0/DATA5 to allow QTI and QBI
               to send control msgs to modem simultaneously.
03/12/18  mm   Fixed KW P1 issue
01/30/18  ha   Add support to monitor mhi_ctrl
01/05/18  ha   Add support of PCIe enablement
12/23/16  rv   Changed files where USB PID for the composition used on next boot is maintained
10/28/13  bd   Add support for Data Port Mapper (DPM)
10/03/13  hz   Restart USB after crash recovery
05/07/13  bd   Return different wMaxSegmentSize for IPA vs. A2
03/13/13  hz   Add support for multiple data sessions
01/25/13  bd   Support configuration of data path in qbi_hc layer
11/20/12  bd   Differentiate active and next diag config, support HSIC in diag
               config code
10/11/12  bd   Toggle DTR after cable unplug
06/12/12  bd   Add support for crash recovery
05/18/12  bd   Add function to get wMaxSegmentSize
04/17/12  bd   Changed RESET_FUNCTION indication from SIGUSR1 to read() == 0
04/17/12  bd   Added support for QMUX bypass mode
02/07/12  bd   Added module
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_hc.h"

#include "qbi_common.h"
#include "qbi_log.h"
#include "qbi_msg.h"
#include "qbi_os_qcci.h"
#include "qbi_qmi_txn.h"
#include "qbi_svc.h"
#include "qbi_task.h"
#include "qbi_txn.h"
#include "qbi_util.h"
#include "qbi_qmux.h"
#include "qbi_nv_store.h"
#include "qbi_os.h"
#include "qbi_qmi.h"
#include "qbi_svc_bc_common.h"
#include "data_port_mapper_v01.h"
#include "wireless_data_administrative_service_v01.h"

#ifdef QBI_UNIT_TEST
#include "qbi_ut.h"
#endif /* QBI_UNIT_TEST */

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <semaphore.h>
#include <linux/netlink.h>
/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/* Return values indicating success/error in most standard library functions */
#define QBI_HC_LINUX_LIB_ERROR   (-1)
#define QBI_HC_LINUX_LIB_SUCCESS (0)

/*! Location where we can access MBIM USB driver device special file for I/O */
#define QBI_HC_LINUX_DEVFILE_PATH "/dev/android_mbim"

/*! Location where we can access MBIM PCIe driver device special file for I/O */
#define QBI_HC_LINUX_MHI_DEVFILE_PATH "/dev/mhi_pipe_12"

/*! Location where mhi_ctrl state is located. */
#define QBI_HC_LINUX_MHI_CTRL_PATH "/dev/mhi_ctrl"

/*! Files where USB can be enabled/disabled */
#define QBI_HC_LINUX_USB_ENABLE_FILE "/sys/class/android_usb/android0/enable"
#define QBI_HC_LINUX_USB_OVER_HSIC_ENABLE_FILE "/sys/class/android_usb/android1/enable"

/*! How long to pause between open/read attempts when we get an error */
#define QBI_HC_LINUX_DEVFILE_RETRY_DELAY_US (500000)

/*! Number of attempts at opening the device file before we give up (currently
    tuned for a total wait time of 30 seconds) */
#define QBI_HC_LINUX_DEVFILE_OPEN_NUM_RETRIES (60)

#define QBI_HC_LINUX_IOCTL_MAGIC 'o'
#define QBI_HC_LINUX_MHI_IOCTL_MAGIC 'm'

#define QBI_HC_LINUX_IOCTL_GET_NTB_SIZE \
  _IOR(QBI_HC_LINUX_IOCTL_MAGIC, 2, uint32)
#define QBI_HC_LINUX_IOCTL_GET_DATAGRAM_COUNT \
  _IOR(QBI_HC_LINUX_IOCTL_MAGIC, 3, uint16)
#define QBI_HC_LINUX_IOCTL_EP_LOOKUP \
  _IOR(QBI_HC_LINUX_IOCTL_MAGIC, 4, hardware_accl_port_details_v01)

#define QBI_HC_LINUX_MHI_IOCTL_EP_LOOKUP \
  _IOR(QBI_HC_LINUX_MHI_IOCTL_MAGIC, 2, hardware_accl_port_details_v01)

/*! Maximum size of a USB control transfer, in bytes */
#define QBI_HC_LINUX_MAX_CTRL_XFER_SIZE (4096)

/*! Maximum data segment size, in bytes, for targets that use A2 for MBIM
    aggregation */
#define QBI_HC_LINUX_MAX_SEGMENT_SIZE_A2  (4064)

/*! Maximum data segment size, in bytes, for targets that use IPA for MBIM
    aggregation*/
#define QBI_HC_LINUX_MAX_SEGMENT_SIZE_IPA (2048)

/*! File indicating whether a QBI session is active
    @details This file is used as part of crash recovery handling. If we are
    recovering from a QBI crash during an active MBIM session (i.e. the QBI
    context was open), then the host must be informed of the crash. If QBI
    recovered when the MBIM session was closed, then no action is required. */
#define QBI_HC_LINUX_ACTIVE_SESSION_FILE "/var/run/qbi_session_active"

/*! Name of script used to change the USB composition */
#define QBI_HC_LINUX_USB_COMPOSITION_SCRIPT "/usr/bin/usb_composition"

/*! Size of the buffer for the composition change command */
#define QBI_HC_LINUX_USB_COMPOSITION_CMD_LEN (64)

/* Files where the active USB PID is maintained */
#define QBI_HC_LINUX_HSUSB_ACTIVE_PID_FILE "/sys/class/android_usb/android0/idProduct"
#define QBI_HC_LINUX_HSIC_ACTIVE_PID_FILE  "/sys/class/android_usb/android1/idProduct"

/* Files where USB PID for the composition used on next boot is maintained */
#define QBI_HC_LINUX_HSUSB_NEXT_PID_FILE "/data/usb/hsusb_next"
#define QBI_HC_LINUX_HSIC_NEXT_PID_FILE  "/data/usb/hsic_next"

/*! Length of a USB PID when represented as a string */
#define QBI_HC_LINUX_PID_STR_LEN (4)

/*! Path to the SMD device used for issuing DTR signals */
#define QBI_HC_LINUX_SMD_PATH "/dev/smdcntl0"

/*! Path to the IPA device file - its presence indicates IPA support */
#define QBI_HC_LINUX_IPA_PATH "/dev/ipa"

/* Retry parameters for IOCTL_EP_LOOKUP */
#define QBI_HC_LINUX_EP_LOOKUP_RETRY_COUNT    (10)
#define QBI_HC_LINUX_EP_LOOKUP_RETRY_DELAY_US (50000)

/*! Possible MHI channel state strings recevied from uevent */
#define QBI_HC_LINUX_MHI_CHNEL              "MHI_CHANNEL_STATE_12"
#define QBI_HC_LINUX_MHI_CHNEL_CONNECTED    "MHI_CHANNEL_STATE_12=CONNECTED"
#define QBI_HC_LINUX_MHI_CHNEL_CONFIGURED   "MHI_CHANNEL_STATE_12=CONFIGURED"
#define QBI_HC_LINUX_MHI_CHNEL_DISCONNECTED "MHI_CHANNEL_STATE_12=DISCONNECTED"

/*! Path to mhi uevent special file used to trigger mhi uevent request */
#define QBI_HC_LINUX_MHI_UEVENT_REQUEST_PATH \
"/sys/devices/virtual/mhi/mhi_ctrl/uevent"

/*! Group number used by netlink socket to receive uevents from the kernel */
#define QBI_HC_LINUX_UEVENT_BROADCAST_GROUP (0x00000001)

/*! Size of the buffer for the UEVENT msg and UEVENT netlink socket */
#define QBI_HC_LINUX_UEVENT_BUFFER_SIZE (1024)

/*=============================================================================

  Private Typedefs

=============================================================================*/

/*! Pre-agreed logical data port numbers coresponding to SIO_PORT_MUX_A2_CH_0 -
    SIO_PORT_MUX_A2_CH_7 on modem side. */
typedef enum {
  QBI_HC_BIND_DATA_PORT_0 = 0x0E1C,
  QBI_HC_BIND_DATA_PORT_1 = 0x0E1D,
  QBI_HC_BIND_DATA_PORT_2 = 0x0E1E,
  QBI_HC_BIND_DATA_PORT_3 = 0x0E1F,
  QBI_HC_BIND_DATA_PORT_4 = 0x0E20,
  QBI_HC_BIND_DATA_PORT_5 = 0x0E21,
  QBI_HC_BIND_DATA_PORT_6 = 0x0E22,
  QBI_HC_BIND_DATA_PORT_7 = 0x0E23,
  QBI_HC_BIND_DATA_PORT_MAX
} qbi_hc_bind_data_port_e;

/*! Command buffer used to pass messages received from the host to the main QBI
    task for processing */
typedef struct {
  qbi_util_buf_s buf;
} qbi_hc_linux_rx_msg_cmd_s;

/*! Enumeration indicating the detected control channel mode */
typedef enum {
  /*! First control packet not received yet; mode not set */
  QBI_HC_LINUX_CTL_MODE_UNKNOWN = 0,
  /*! Normal MBIM mode. Control packets interpreted as MBIM CIDs */
  QBI_HC_LINUX_CTL_MODE_MBIM,
  /*! Bypass mode. Control packets interprted as QMUX requests */
  QBI_HC_LINUX_CTL_MODE_QMUX
} qbi_hc_linux_ctl_mode_e;

typedef enum {
  QBI_HC_LINUX_TRISTATE_FALSE   = 0,
  QBI_HC_LINUX_TRISTATE_TRUE    = 1,
  QBI_HC_LINUX_TRISTATE_UNKNOWN = 2
} qbi_hc_linux_tristate_e;

typedef struct {
  /*! QBI context associated with this link */
  qbi_ctx_s ctx;

  /*! Indicates whether we have updated the filesystem to reflect the opened/
      closed status of the QBI context
      @see QBI_HC_LINUX_ACTIVE_SESSION_FILE */
  boolean is_session_open;

  /*! Device file descriptor used for IO with USB driver */
  int dev_fd;

  /*! Buffer used to copy data from the USB driver in read() */
  uint8 rx_buf[QBI_HC_LINUX_MAX_CTRL_XFER_SIZE];

  /*! Indicates the control packet mode as MBIM or QMUX */
  qbi_hc_linux_ctl_mode_e ctl_mode;

  /*! Indicates whether MBIM is using the HSUSB or HSIC core */
  boolean mbim_is_over_hsic;

  /*! Tells whether DIAG is included in the active USB composition */
  qbi_hc_diag_config_e active_diag_config;

  /*! Set to TRUE when active_diag_config is populated */
  boolean active_diag_config_initialized;

  /*! Set to TRUE when DPM supported and ep_info is populated, FALSE when DPM
      not supported by the platform, UNKNOWN when EP_LOOKUP not queried yet */
  qbi_hc_linux_tristate_e dpm_supported;

  /*! Endpoint info cache lasting the duration of the MBIM session */
  hardware_accl_port_details_v01 ep_info;

  /*! Indictes whether PCIe is enabled or no. Current QBI implementation
      allows for the co-existence of PCIe with (either HSUSB or HSIC)
      provided that the PID of HSUSB or HSIC does NOT include MBIM */
  qbi_hc_linux_tristate_e  pcie_enabled;

  /*! state of mhi ctrl. The value is updated by the mhi_monitor thread */
  qbi_hc_mhi_ctrl_state_e  mhi_ctrl_state;

  /*! This will be used to monitor whether current write in progress */
  qbi_os_mutex_t mhi_monitor_write_op;

  sem_t mhi_monitor_semaphore;

} qbi_hc_linux_info_s;

typedef struct {
  char diag_included[QBI_HC_LINUX_PID_STR_LEN + 1];
  char diag_excluded[QBI_HC_LINUX_PID_STR_LEN + 1];
} qbi_hc_linux_diag_composition_pair_s;

/*=============================================================================

  Private Variables

=============================================================================*/

static qbi_hc_linux_info_s qbi_hc_linux_info;

static qbi_hc_linux_diag_composition_pair_s qbi_hc_linux_diag_composition_pairs[] = {
  {
    "9043", /* DIAG + NMEA + MDM + MBIM */
    "905B"  /* MBIM */
  },
  {
    "905A", /* Config 1 - DIAG + ADB + MBIM, Config 2 - ECM */
    "9063"  /* Config 1 - RNDIS, Config 2 - ECM, Config 3 - MBIM */
  }
};

static pthread_t reader_thread;

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static void qbi_hc_linux_crash_recovery_update_session_file
(
  void
);

static qbi_svc_action_e qbi_hc_linux_data_format_configure_wda01_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static const qbi_hc_linux_diag_composition_pair_s *qbi_hc_linux_diag_config_tbl_lookup
(
  const char           *pid,
  qbi_hc_diag_config_e *diag_config
);

static boolean qbi_hc_linux_diag_config_read_next_pid
(
  char *pid
);

static boolean qbi_hc_linux_diag_config_read_pid_file
(
  const char *path,
  char       *pid
);

static boolean qbi_hc_linux_is_ipa_supported
(
  void
);

static boolean qbi_hc_linux_mbim_is_over_hsic
(
  void
);

static qbi_hc_linux_tristate_e qbi_hc_linux_does_mhi_ctrl_exist
(
  void
);

static void qbi_hc_linux_reset_dtr
(
  void
);

static void qbi_hc_linux_reset_function_cmd_cb
(
  qbi_ctx_s        *ctx,
  qbi_task_cmd_id_e cmd_id,
  void             *data
);

static void qbi_hc_linux_qmux_rx_from_modem_cb
(
  qbi_ctx_s      *ctx,
  qbi_util_buf_s *buf
);

static void qbi_hc_linux_rx_msg_cmd_cb
(
  qbi_ctx_s        *ctx,
  qbi_task_cmd_id_e cmd_id,
  void             *data
);

static void qbi_hc_linux_rx_msg_cmd_free
(
  qbi_hc_linux_rx_msg_cmd_s *cmd
);

static void qbi_hc_linux_rx_msg_cmd_send
(
  qbi_ctx_s  *ctx,
  const void *msg_buf,
  uint32      msg_buf_len
);

static void qbi_hc_linux_set_ctl_mode
(
  qbi_ctx_s                  *ctx,
  const qbi_util_buf_const_s *buf
);

static void *qbi_hc_linux_monitor_thread_entry
(
  void *arg
);

static boolean qbi_hc_linux_update_mhi_channel_state
(
  qbi_hc_mhi_ctrl_state_e mhi_state
);

static qbi_hc_mhi_ctrl_state_e qbi_hc_linux_get_mhi_state_from_uevent_file
(
  void
);

static int qbi_hc_linux_open_netlink_socket
(
  int    protocol,
  pid_t  pid,
  uint32 group
);

static qbi_hc_mhi_ctrl_state_e qbi_hc_linux_parse_mhi_uevent
(
  const char *msg,
  ssize_t msg_len
);

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_hc_linux_crash_recovery_update_session_file
===========================================================================*/
/*!
    @brief Updates the file used to track whether an MBIM session is open

    @details
*/
/*=========================================================================*/
static void qbi_hc_linux_crash_recovery_update_session_file
(
  void
)
{
  int fd;
/*-------------------------------------------------------------------------*/
  qbi_hc_linux_info.is_session_open =
    (qbi_hc_linux_info.ctx.state == QBI_CTX_STATE_OPENED);
  QBI_LOG_I_1("Detected MBIM session state change to %d; updating file",
              qbi_hc_linux_info.ctx.state);
  if (qbi_hc_linux_info.is_session_open)
  {
    fd = open(QBI_HC_LINUX_ACTIVE_SESSION_FILE, (O_WRONLY | O_CREAT),
              (S_IRUSR | S_IWUSR));
    if (fd == QBI_HC_LINUX_LIB_ERROR)
    {
      QBI_LOG_E_1("Couldn't create crash recovery file: %d", errno);
    }
    else
    {
      (void) close(fd);
    }
  }
  else
  {
    if (unlink(QBI_HC_LINUX_ACTIVE_SESSION_FILE) == QBI_HC_LINUX_LIB_ERROR)
    {
      QBI_LOG_E_1("Couldn't remove crash recovery file: %d", errno);
    }
  }
} /* qbi_hc_linux_crash_recovery_update_file() */

/*===========================================================================
  FUNCTION: qbi_hc_linux_data_format_configure_wda01_rsp_cb
===========================================================================*/
/*!
    @brief Validates QMI_WDA_SET_DATA_FORMAT_RESP as part of configuring the
    data path for 9x25

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_hc_linux_data_format_configure_wda01_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  wda_set_data_format_req_msg_v01 *qmi_req;
  wda_set_data_format_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_req = (wda_set_data_format_req_msg_v01 *) qmi_txn->req.data;
  qmi_rsp = (wda_set_data_format_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error %d from QMI", qmi_rsp->resp.error);
  }
  else if (!qmi_rsp->link_prot_valid ||
           !qmi_rsp->dl_data_aggregation_protocol_valid ||
           !qmi_rsp->ul_data_aggregation_protocol_valid)
  {
    QBI_LOG_E_0("Response missing one or more expected TLVs!");
  }
  else if (qmi_rsp->link_prot != qmi_req->link_prot ||
           qmi_rsp->dl_data_aggregation_protocol !=
             qmi_req->dl_data_aggregation_protocol ||
           qmi_rsp->ul_data_aggregation_protocol !=
             qmi_req->ul_data_aggregation_protocol)
  {
    QBI_LOG_E_0("Modem did not accept one or more requested settings");
  }
  else
  {
    QBI_LOG_I_0("Successfully configured data format for IPA");
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_hc_linux_data_format_configure_wda01_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_hc_linux_diag_config_tbl_lookup
===========================================================================*/
/*!
    @brief Finds an entry in the USB composition table containing the
    provided USB PID, and also determines the diag configuration of that
    composition

    @details

    @param pid Buffer must be at least QBI_HC_LINUX_PID_STR_LEN bytes
    @param diag_config

    @return qbi_hc_linux_diag_composition_pair_s* Pointer to table entry,
    or NULL if no matching entry found or an unexpected error occurred
*/
/*=========================================================================*/
static const qbi_hc_linux_diag_composition_pair_s *qbi_hc_linux_diag_config_tbl_lookup
(
  const char           *pid,
  qbi_hc_diag_config_e *diag_config
)
{
  uint32 i;
  const qbi_hc_linux_diag_composition_pair_s *entry = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(pid);
  QBI_CHECK_NULL_PTR_RET_NULL(diag_config);

  for (i = 0; i < ARR_SIZE(qbi_hc_linux_diag_composition_pairs); i++)
  {
    if (strncasecmp(pid, qbi_hc_linux_diag_composition_pairs[i].diag_excluded,
                    QBI_HC_LINUX_PID_STR_LEN) == 0)
    {
      *diag_config = QBI_HC_DIAG_CONFIG_EXCLUDED;
      entry = &qbi_hc_linux_diag_composition_pairs[i];
      break;
    }
    else if (strncasecmp(
               pid, qbi_hc_linux_diag_composition_pairs[i].diag_included,
               QBI_HC_LINUX_PID_STR_LEN) == 0)
    {
      *diag_config = QBI_HC_DIAG_CONFIG_INCLUDED;
      entry = &qbi_hc_linux_diag_composition_pairs[i];
      break;
    }
  }

  if (entry == NULL)
  {
    *diag_config = QBI_HC_DIAG_CONFIG_UNKNOWN;
  }

  return entry;
} /* qbi_hc_linux_diag_config_tbl_lookup() */

/*===========================================================================
  FUNCTION: qbi_hc_linux_diag_config_read_next_pid
===========================================================================*/
/*!
    @brief Reads the USB PID configured to be used at next boot.

    @details

    @param pid Will be populated with the USB PID (4 digit hexadecimal
    string). Size of buffer must be at least QBI_HC_LINUX_PID_STR_LEN. Null
    termination not provided by this function.

    @return boolean TRUE on successful read, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_hc_linux_diag_config_read_next_pid
(
  char *pid
)
{
  const char *pid_file;
/*-------------------------------------------------------------------------*/
  pid_file = (qbi_hc_linux_info.mbim_is_over_hsic) ?
    QBI_HC_LINUX_HSIC_NEXT_PID_FILE : QBI_HC_LINUX_HSUSB_NEXT_PID_FILE;
  return qbi_hc_linux_diag_config_read_pid_file(pid_file, pid);
} /* qbi_hc_linux_diag_config_read_next_pid() */

/*===========================================================================
  FUNCTION: qbi_hc_linux_diag_config_read_pid_file
===========================================================================*/
/*!
    @brief Reads a USB PID file

    @details

    @param path Path to PID file to read
    @param pid Will be populated with the USB PID (4 digit hexadecimal
    string). Size of buffer must be at least QBI_HC_LINUX_PID_STR_LEN. Null
    termination not provided by this function.

    @return boolean TRUE on successful read, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_hc_linux_diag_config_read_pid_file
(
  const char *path,
  char       *pid
)
{
  int fd;
  ssize_t bytes_read;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(path);
  QBI_CHECK_NULL_PTR_RET_FALSE(pid);

  fd = open(path, O_RDONLY);
  if (fd == QBI_HC_LINUX_LIB_ERROR)
  {
    QBI_LOG_E_1("Couldn't open PID file: %d", errno);
    QBI_LOG_STR_1("Path to PID file: '%s'", path);
  }
  else
  {
    bytes_read = read(fd, pid, QBI_HC_LINUX_PID_STR_LEN);
    if (bytes_read != QBI_HC_LINUX_PID_STR_LEN)
    {
      QBI_LOG_E_2("Read %d bytes from next PID file; expected %d",
                  bytes_read, QBI_HC_LINUX_PID_STR_LEN);
    }
    else
    {
      success = TRUE;
    }
    close(fd);
  }

  return success;
} /* qbi_hc_linux_diag_config_read_pid_file() */

/*===========================================================================
  FUNCTION: qbi_hc_linux_is_ipa_supported
===========================================================================*/
/*!
    @brief Detects whether IPA is used for the MBIM data path

    @details

    @return boolean TRUE if IPA is supported, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_hc_linux_is_ipa_supported
(
  void
)
{
  int ret;
  static qbi_hc_linux_tristate_e ipa_supported = QBI_HC_LINUX_TRISTATE_UNKNOWN;
/*-------------------------------------------------------------------------*/
  if (ipa_supported == QBI_HC_LINUX_TRISTATE_UNKNOWN)
  {
    ret = access(QBI_HC_LINUX_IPA_PATH, F_OK);
    if (ret == QBI_HC_LINUX_LIB_ERROR)
    {
      if (errno != ENOENT)
      {
        QBI_LOG_E_1("Error checking for IPA device file: %d", errno);
      }
      else
      {
        QBI_LOG_I_0("IPA device file not present");
      }
      ipa_supported = QBI_HC_LINUX_TRISTATE_FALSE;
    }
    else
    {
      QBI_LOG_I_0("IPA device file is present");
      ipa_supported = QBI_HC_LINUX_TRISTATE_TRUE;
    }
  }

  return (ipa_supported == QBI_HC_LINUX_TRISTATE_TRUE);
} /* qbi_hc_linux_is_ipa_supported() */

/*===========================================================================
  FUNCTION: qbi_hc_linux_mbim_is_over_hsic
===========================================================================*/
/*!
    @brief Determines whether MBIM transport is over HSUSB or HSIC by reading
    USB system files

    @details

    @return boolean TRUE if HSIC core is enabled and contains an MBIM
    endpoint, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_hc_linux_mbim_is_over_hsic
(
  void
)
{
  int fd;
  ssize_t bytes_read;
  boolean mbim_is_over_hsic = FALSE;
  char hsic_enabled;
  char functions[128];
  char *loc;
/*-------------------------------------------------------------------------*/
  fd = open(QBI_HC_LINUX_USB_OVER_HSIC_ENABLE_FILE, O_RDONLY);
  if (fd == QBI_HC_LINUX_LIB_ERROR)
  {
    QBI_LOG_E_1("Couldn't open HSIC enable file: %d", errno);
  }
  else
  {
    bytes_read = read(fd, &hsic_enabled, sizeof(char));
    if (bytes_read != sizeof(char))
    {
      QBI_LOG_E_2("Couldn't read HSIC enable file (retval %d errno %d)",
                  bytes_read, errno);
      hsic_enabled = '0';
    }
    close(fd);

    if (hsic_enabled == '1')
    {
      fd = open("/sys/class/android_usb/android1/functions", O_RDONLY);
      if (fd == QBI_HC_LINUX_LIB_ERROR)
      {
        QBI_LOG_E_1("Couldn't open HSIC functions file: %d", errno);
      }
      else
      {
        bytes_read = read(fd, functions, sizeof(functions) - 1);
        if (bytes_read <= 0 || bytes_read >= sizeof(functions))
        {
          QBI_LOG_E_2("Couldn't read HSIC functions file (retval %d errno %d)",
                      bytes_read, errno);
        }
        else
        {
          functions[bytes_read] = '\0';
          loc = strstr(functions, "mbim");
          if (loc != NULL)
          {
            mbim_is_over_hsic = TRUE;
          }
        }
      }
      close(fd);
    }
  }

  return mbim_is_over_hsic;
} /* qbi_hc_linux_mbim_is_over_hsic() */

/*===========================================================================
  FUNCTION: qbi_hc_linux_does_mhi_ctrl_exist
===========================================================================*/
/*!
    @brief Checks if mhi_ctrl exists

    @details Checks if LE device in End Point PCIe (EP PCIe) by checking
    existence of mhi_ctrl.

    @return qbi_hc_linux_tristate_e
*/
/*=========================================================================*/
static qbi_hc_linux_tristate_e qbi_hc_linux_does_mhi_ctrl_exist
(
  void
)
{
  int ret = QBI_HC_LINUX_LIB_ERROR;
  qbi_hc_linux_tristate_e pcie_enabled_state = QBI_HC_LINUX_TRISTATE_UNKNOWN;
/*-------------------------------------------------------------------------*/
  ret = access(QBI_HC_LINUX_MHI_CTRL_PATH, F_OK);
  if (ret == QBI_HC_LINUX_LIB_ERROR)
  {
    if (errno != ENOENT)
    {
      QBI_LOG_E_1("Error checking for mhi_ctrl file: %d. "
        "Defaulting to MBIM over USB Interface", errno);
    }
    else
    {
      QBI_LOG_I_0("mhi_ctrl file does not exist. MBIM is over USB Interface");
    }
    pcie_enabled_state = QBI_HC_LINUX_TRISTATE_FALSE;
  }
  else
  {
    QBI_LOG_I_0("mhi_ctrl detected. MBIM is over PCIe Interface");
    pcie_enabled_state = QBI_HC_LINUX_TRISTATE_TRUE;
  }

  return pcie_enabled_state;
} /* qbi_hc_linux_does_mhi_ctrl_exist() */

/*===========================================================================
  FUNCTION: qbi_hc_linux_reset_dtr
===========================================================================*/
/*!
    @brief Toggles the DTR signal to low then high to inform the modem that
    it should clean up the data path

    @details
    The modem requires a DTR signal when the USB cable is unplugged to ensure
    that it fully flushes its hardware.
*/
/*=========================================================================*/
static void qbi_hc_linux_reset_dtr
(
  void
)
{
  int fd;
  int dtr_sig = 0;
/*-------------------------------------------------------------------------*/
  fd = open(QBI_HC_LINUX_SMD_PATH, O_RDWR);
  if (fd == QBI_HC_LINUX_LIB_ERROR)
  {
    QBI_LOG_E_1("Couldn't open SMD channel for DTR: %d", errno);
  }
  else
  {
    /*! @note The SMD implementation of TIOCMGET/TIOCMSET ioctls breaks from the
        standard Linux implementation. Instead of using a pointer to int in the
        3rd argument to pass the modem bits, they are given as the return value
        of the get and passed directly in the 3rd argument for the set (i.e. not
        as a pointer). */
    dtr_sig = ioctl(fd, TIOCMGET, NULL);
    if (dtr_sig == QBI_HC_LINUX_LIB_ERROR)
    {
      QBI_LOG_E_1("Error reading modem bits: %d", errno);
    }

    if (dtr_sig & TIOCM_DTR) {
      QBI_LOG_I_0("Setting DTR low");
      dtr_sig ^= TIOCM_DTR;
      if (ioctl(fd, TIOCMSET, dtr_sig) == QBI_HC_LINUX_LIB_ERROR)
      {
        QBI_LOG_E_1("Error clearing DTR: %d", errno);
      }
    }
    else
    {
      QBI_LOG_W_0("DTR is already low!");
    }

    QBI_LOG_I_0("Setting DTR high");
    dtr_sig |= TIOCM_DTR;
    if (ioctl(fd, TIOCMSET, dtr_sig) == QBI_HC_LINUX_LIB_ERROR)
    {
      QBI_LOG_E_1("Error setting DTR: %d", errno);
    }

    (void) close(fd);
  }
} /* qbi_hc_linux_reset_dtr() */

/*===========================================================================
  FUNCTION: qbi_hc_linux_reset_function_cmd_cb
===========================================================================*/
/*!
    @brief Processes a RESET_FUNCTION command

    @details

    @param ctx
    @param cmd_id
    @param data
*/
/*=========================================================================*/
static void qbi_hc_linux_reset_function_cmd_cb
(
  qbi_ctx_s        *ctx,
  qbi_task_cmd_id_e cmd_id,
  void             *data
)
{
  boolean need_dtr_reset = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);
  QBI_ARG_NOT_USED(cmd_id);
  QBI_ARG_NOT_USED(data);

  QBI_LOG_W_0("Handling RESET_FUNCTION received from host");
  /*! @note When DPM is used modem will handle data path clean up after
      QMI_DPM_CLOSE_PORT_REQ is received, in this case do not reset DTR from
      QBI. Also, after DPM data port is closed, resetting DTR using IOCTL
      method may be blocked depending on SMD implementation.
  */
  if (ctx->state == QBI_CTX_STATE_OPENED && !qbi_hc_dpm_is_used(ctx))
  {
    need_dtr_reset = TRUE;
  }
  qbi_msg_context_close(ctx);

  if (need_dtr_reset)
  {
    qbi_hc_linux_reset_dtr();
  }
  /* Reset the active diag config cache to support on the fly USB PID changes */
  qbi_hc_linux_info.active_diag_config_initialized = FALSE;
} /* qbi_hc_linux_reset_function_cmd_cb() */

/*===========================================================================
  FUNCTION: qbi_hc_linux_qmux_rx_from_modem_cb
===========================================================================*/
/*!
    @brief Processes raw QMUX data received from the modem

    @details
    Assuming we are in QMUX control mode, this data will be forwarded over
    the control channel to the host unmodified.

    @param ctx
    @param buf
*/
/*=========================================================================*/
static void qbi_hc_linux_qmux_rx_from_modem_cb
(
  qbi_ctx_s      *ctx,
  qbi_util_buf_s *buf
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(buf);

  if (qbi_hc_linux_info.ctl_mode != QBI_HC_LINUX_CTL_MODE_QMUX)
  {
    QBI_LOG_E_1("Invalid control mode %d to handle QMUX data from modem!",
                qbi_hc_linux_info.ctl_mode);
  }
  else
  {
    qbi_hc_tx(ctx, buf->data, buf->size);
  }

  qbi_util_buf_free(buf);
  QBI_MEM_FREE(buf);
} /* qbi_hc_linux_qmux_rx_from_modem_cb() */

/*===========================================================================
  FUNCTION: qbi_hc_linux_rx_msg_cmd_cb
===========================================================================*/
/*!
    @brief Callback invoked from the context of the main QBI task to process
    a message received from the host

    @details

    @param ctx
    @param cmd_id
    @param data
*/
/*=========================================================================*/
static void qbi_hc_linux_rx_msg_cmd_cb
(
  qbi_ctx_s        *ctx,
  qbi_task_cmd_id_e cmd_id,
  void             *data
)
{
  qbi_hc_linux_rx_msg_cmd_s *cmd;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(data);
  QBI_ARG_NOT_USED(cmd_id);

  cmd = (qbi_hc_linux_rx_msg_cmd_s *) data;
  qbi_log_pkt(ctx->id, QBI_LOG_PKT_DIRECTION_RX, cmd->buf.data, cmd->buf.size);
  if (qbi_hc_linux_info.ctl_mode == QBI_HC_LINUX_CTL_MODE_UNKNOWN)
  {
    qbi_hc_linux_set_ctl_mode(ctx, (const qbi_util_buf_const_s *) &cmd->buf);
  }

  if (qbi_hc_linux_info.ctl_mode == QBI_HC_LINUX_CTL_MODE_MBIM)
  {
    qbi_msg_input(ctx, (qbi_util_buf_const_s *) &cmd->buf);
  }
  else if (qbi_hc_linux_info.ctl_mode == QBI_HC_LINUX_CTL_MODE_QMUX)
  {
    qbi_qmux_tx_to_modem(ctx, (qbi_qmux_msg_s *) cmd->buf.data, cmd->buf.size);
  }

  qbi_hc_linux_rx_msg_cmd_free(cmd);
} /* qbi_hc_linux_rx_msg_cmd_cb() */

/*===========================================================================
  FUNCTION: qbi_hc_linux_rx_msg_cmd_free
===========================================================================*/
/*!
    @brief Releases all memory associated with a received message command
    buffer

    @details

    @param cmd
*/
/*=========================================================================*/
static void qbi_hc_linux_rx_msg_cmd_free
(
  qbi_hc_linux_rx_msg_cmd_s *cmd
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(cmd);
  qbi_util_buf_free(&cmd->buf);
  QBI_MEM_FREE(cmd);
} /* qbi_hc_linux_rx_msg_cmd_free() */

/*===========================================================================
  FUNCTION: qbi_hc_linux_rx_msg_cmd_send
===========================================================================*/
/*!
    @brief Sends a copy of the given message received from the host to the
    main QBI task for processing

    @details
    Called from the context of the reader thread

    @param ctx
    @param msg_buf
    @param msg_buf_len Size of the message in bytes
*/
/*=========================================================================*/
static void qbi_hc_linux_rx_msg_cmd_send
(
  qbi_ctx_s  *ctx,
  const void *msg_buf,
  uint32      msg_buf_len
)
{
  qbi_hc_linux_rx_msg_cmd_s *cmd;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);
  QBI_CHECK_NULL_PTR_RET(msg_buf);

  cmd = QBI_MEM_MALLOC_CLEAR(sizeof(qbi_hc_linux_rx_msg_cmd_s));
  QBI_CHECK_NULL_PTR_RET(cmd);

  qbi_util_buf_init(&cmd->buf);
  if (qbi_util_buf_alloc_dont_clear(&cmd->buf, msg_buf_len) == NULL)
  {
    QBI_LOG_E_1("Couldn't allocate %d bytes for new RX message!", msg_buf_len);
    qbi_hc_linux_rx_msg_cmd_free(cmd);
  }
  else
  {
    QBI_MEMSCPY(cmd->buf.data, cmd->buf.size, msg_buf, msg_buf_len);
    if (!qbi_task_cmd_send(
          ctx, QBI_TASK_CMD_ID_RX_DATA, qbi_hc_linux_rx_msg_cmd_cb, cmd))
    {
      QBI_LOG_E_0("Couldn't post command for new RX message!");
      qbi_hc_linux_rx_msg_cmd_free(cmd);
    }
  }
} /* qbi_hc_linux_rx_msg_cmd_send() */

/*===========================================================================
  FUNCTION: qbi_hc_linux_set_ctl_mode
===========================================================================*/
/*!
    @brief Sets the control channel mode for the given QBI context based on
    the contents of the provided data packet

    @details

    @param ctx
    @param buf
*/
/*=========================================================================*/
static void qbi_hc_linux_set_ctl_mode
(
  qbi_ctx_s                  *ctx,
  const qbi_util_buf_const_s *buf
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(buf);

  if (qbi_qmux_is_qmi_ctl_request(buf->data, buf->size))
  {
    QBI_LOG_W_0("First control message detected to be QMUX. Enabling bypass "
                "mode");

    if (!qbi_qmux_open(ctx, qbi_hc_linux_qmux_rx_from_modem_cb))
    {
      QBI_LOG_E_0("Couldn't open raw QMUX connection!");
    }
    else
    {
      qbi_hc_linux_info.ctl_mode = QBI_HC_LINUX_CTL_MODE_QMUX;
    }
  }
  else
  {
    QBI_LOG_I_0("Setting control channel mode to MBIM");
    qbi_hc_linux_info.ctl_mode = QBI_HC_LINUX_CTL_MODE_MBIM;
  }
} /* qbi_hc_linux_set_ctl_mode() */

/*=============================================================================

  Public Function Definitions exposed in qbi_hc_linux.h

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_hc_linux_crash_recovery_common_function
===========================================================================*/
/*!
    @brief Common operations between PCIe and USB Interface

    @details
*/
/*=========================================================================*/
void qbi_hc_linux_crash_recovery_common_function
(
  int fd,
  const qbi_ctx_s *ctx
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);

  (void) close(fd);
  (void) unlink(QBI_HC_LINUX_ACTIVE_SESSION_FILE);

  return;
}/* qbi_hc_linux_crash_recovery_check_session_file */

/*===========================================================================
  FUNCTION: qbi_hc_linux_crash_recovery_check_session_file
===========================================================================*/
/*!
    @brief Checks if QBI is recovering from a crash, and if so, takes
    appropriate action

    @details
*/
/*=========================================================================*/
void qbi_hc_linux_crash_recovery_check_session_file
(
  void
)
{
  int fd = 0;
  const char *usb_enable_file;
/*-------------------------------------------------------------------------*/
  fd = open(QBI_HC_LINUX_ACTIVE_SESSION_FILE, O_RDONLY);
  /* PCIe Mode */
  if (QBI_HC_LINUX_TRISTATE_TRUE == qbi_hc_linux_does_mhi_ctrl_exist())
  {
    if (fd != QBI_HC_LINUX_LIB_ERROR)
    {
      QBI_LOG_E_0("QBI crashed with an open context!! Resetting PCIe");
      qbi_hc_linux_crash_recovery_common_function(fd,&qbi_hc_linux_info.ctx);
      qbi_qmi_pcie_handle_mbim_recovery(&qbi_hc_linux_info.ctx);
    }
  }
  else
  {
    /* USB Mode */
    if (fd != QBI_HC_LINUX_LIB_ERROR)
    {
      QBI_LOG_E_0("QBI crashed with an open context!! Resetting USB");

      qbi_hc_linux_crash_recovery_common_function(fd,&qbi_hc_linux_info.ctx);
      /* Restart USB subsystem by initiating soft reconnect */
      usb_enable_file = qbi_hc_linux_mbim_is_over_hsic() ?
        QBI_HC_LINUX_USB_OVER_HSIC_ENABLE_FILE : QBI_HC_LINUX_USB_ENABLE_FILE;

      fd = open(usb_enable_file, O_WRONLY);
      if (fd != QBI_HC_LINUX_LIB_ERROR)
      {
        (void) write(fd, "0", 1);
        (void) write(fd, "1", 1);
        (void) close(fd);
      }
      else
      {
        QBI_LOG_E_1("Couldn't open USB subsystem file: %d", errno);
      }
    }
  }

  return;
} /* qbi_hc_linux_crash_recovery_check_session_file() */

/*===========================================================================
  FUNCTION: qbi_qmi_pcie_handle_mbim_recovery
===========================================================================*/
/*!
    @brief Triggers internal MBIM_CLOSE and MBIM_OPEN
           Triggers QBI events to host in case of mbim crash

    @details
    Used to handle mbim crash

    @param ctx

    @return void
*/
/*=========================================================================*/
void qbi_qmi_pcie_handle_mbim_recovery
(
  qbi_ctx_s  *ctx
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ctx);

  QBI_LOG_D_0("MBIMD_RECOVERY : Trigger MBIM Crash Recovery.");

  ctx->mbim_recovery = TRUE;

  /* 1. Triggering internal open
     2. In mbim open response trigger internal pre mbim 
     3. Trigger post mbim open to notify host with required events/response.
  */
  qbi_qmi_internal_mbim_open(ctx);

  return;
} /* qbi_qmi_pcie_handle_mbim_recovery */

/*===========================================================================
  FUNCTION: qbi_hc_linux_dev_init
===========================================================================*/
/*!
    @brief Opens the device special file, allowing IO through the MBIM USB
    driver

    @details
    This must be called once as part of initialization after qbi_hc_init()
    and before the reader thread or main QBI task loop are started.

    @return boolean TRUE on success, FALSE on fatal error
*/
/*=========================================================================*/
boolean qbi_hc_linux_dev_init
(
  void
)
{
  uint32 attempt = 0;
  boolean success = FALSE;
  int pthread_err;
  pthread_t mhi_monitor_thread;
  qbi_hc_linux_info.dev_fd = QBI_HC_LINUX_LIB_ERROR;
  int ret_val = QBI_HC_LINUX_LIB_ERROR;
/*-------------------------------------------------------------------------*/
  if (qbi_hc_linux_info.pcie_enabled == QBI_HC_LINUX_TRISTATE_UNKNOWN)
  {
    qbi_hc_linux_info.pcie_enabled = qbi_hc_linux_does_mhi_ctrl_exist();
    if (qbi_hc_linux_info.pcie_enabled == QBI_HC_LINUX_TRISTATE_UNKNOWN)
    {
      QBI_LOG_E_0("Could not determine the PCIe state. Bailing out.");
      return FALSE;
    }
  }

  if (qbi_hc_pcie_is_enabled(&qbi_hc_linux_info.ctx))
  {
    /* Interface is PCIe */
    /* Semaphore signaled by mhi_monitor thread to inform the reader thread
       that mhi_state is connected */
    if (0 != sem_init(&qbi_hc_linux_info.mhi_monitor_semaphore, 0, 0))
    {
      QBI_LOG_E_1("Failed to create semaphore, error = %d. Bailing out.", errno);
      return FALSE;
    }

    /* Mutex is created to protect the write operation */
    qbi_os_mutex_init(&qbi_hc_linux_info.mhi_monitor_write_op);

    /*! spawn a listener thread to monitor mhi_ctrl state. */
    pthread_err = pthread_create(
        &mhi_monitor_thread, NULL, qbi_hc_linux_monitor_thread_entry, NULL);
    if (pthread_err != 0)
    {
      QBI_LOG_E_1("Failed to spawn mhi_monitor thread: %d. Bailing out.",
                  pthread_err);
      sem_destroy(&qbi_hc_linux_info.mhi_monitor_semaphore);
      return FALSE;
    }
    qbi_hc_linux_info.mbim_is_over_hsic = qbi_hc_linux_mbim_is_over_hsic();
    success = TRUE;
  }
  else
  {
    /* Interface is USB */
    do
    {
      qbi_hc_linux_info.dev_fd = open(QBI_HC_LINUX_DEVFILE_PATH, O_RDWR);
      if (qbi_hc_linux_info.dev_fd == QBI_HC_LINUX_LIB_ERROR)
      {
        QBI_LOG_W_1("Couldn't open device file: %d", errno);
        if (errno == EBUSY)
        {
          QBI_LOG_E_0("Device file already open! Is QBI already running?");
        }
        else if (errno == ENODEV || errno == ENOENT)
        {
          QBI_LOG_I_0("Driver not ready yet; retrying after delay...");
          usleep(QBI_HC_LINUX_DEVFILE_RETRY_DELAY_US);
        }

      }

    } while (qbi_hc_linux_info.dev_fd == QBI_HC_LINUX_LIB_ERROR &&
        (errno == ENODEV || errno == ENOENT) &&
        attempt++ < QBI_HC_LINUX_DEVFILE_OPEN_NUM_RETRIES);

    if (qbi_hc_linux_info.dev_fd != QBI_HC_LINUX_LIB_ERROR)
    {
      qbi_hc_linux_info.mbim_is_over_hsic = qbi_hc_linux_mbim_is_over_hsic();
      success = TRUE;
    }
  }
  return success;
} /* qbi_hc_linux_dev_init() */

/*===========================================================================
  FUNCTION: qbi_hc_linux_reader_thread_entry
===========================================================================*/
/*!
    @brief Entry point for the reader thread used to handle commands received
    from the host by the USB driver

    @details

    @param arg

    @return void*
*/
/*=========================================================================*/
void *qbi_hc_linux_reader_thread_entry
(
  void *arg
)
{
  int ret;
/*-------------------------------------------------------------------------*/
  QBI_ARG_NOT_USED(arg);
  QBI_LOG_I_0("Reader thread started");

  if (qbi_hc_pcie_is_enabled(&qbi_hc_linux_info.ctx))
  {
    /* Read from PCIe interface */
    while (1)
    {
      if(qbi_hc_linux_info.mhi_ctrl_state != QBI_HC_MHI_CTRL_CONNECTED)
      {
        /*! @note MHI is not CONNECTED, wait for MHI Monitor thread to signal.
            MHI Monitor thread will signal once MHI is CONNECTED. Also this
            guarantees that Reader thread will not proceed until MHI Monitor
            thread is running. */
        QBI_LOG_I_0("Reader thread is waiting for MHI to be in Connected state.");
        if (0 != sem_wait(&qbi_hc_linux_info.mhi_monitor_semaphore))
        {
          QBI_LOG_E_1("Reader thread failed to get signal from MHI Monitor"
                     " thread: %d. Terminating Reader thread", errno);
          break;
        }
      }

      if (qbi_hc_linux_info.dev_fd == QBI_HC_LINUX_LIB_ERROR)
      {
        qbi_hc_linux_info.dev_fd = open(QBI_HC_LINUX_MHI_DEVFILE_PATH, O_RDWR);
        if (qbi_hc_linux_info.dev_fd == QBI_HC_LINUX_LIB_ERROR)
        {
          QBI_LOG_E_1("Open MHI pipe failed with error: %d. Terminating Reader"
                      " thread.",errno);
          break;
        }
      }

      ret = read(qbi_hc_linux_info.dev_fd, qbi_hc_linux_info.rx_buf,
          QBI_HC_LINUX_MAX_CTRL_XFER_SIZE);
      if (ret < 0)
      {
        QBI_LOG_E_2("Read failed with return value %d, errno %d", ret, errno);
        usleep(QBI_HC_LINUX_DEVFILE_RETRY_DELAY_US);

        /*! @note closing an invalid blocking pipe is a blocking call. Thus,
            attempting to close the pipe could cause reader thread to block.*/
        /*
        QBI_LOG_I_1("Attempting to close MHI pipe.");
        if ( 0 == close(qbi_hc_linux_info.dev_fd))
        {
          qbi_hc_linux_info.dev_fd = QBI_HC_LINUX_LIB_ERROR;
          QBI_LOG_I_0("MHI pipe is closed.");
        }
        */
      }
      else if (ret == 0)
      {
        if (!qbi_task_cmd_send(
              &qbi_hc_linux_info.ctx, QBI_TASK_CMD_ID_RESET,
              qbi_hc_linux_reset_function_cmd_cb, NULL))
        {
          QBI_LOG_E_0("Couldn't pass RESET_FUNCTION command to QBI task!");
        }
      }
      else
      {
        qbi_hc_linux_rx_msg_cmd_send(
            &qbi_hc_linux_info.ctx, qbi_hc_linux_info.rx_buf, (uint32) ret);
      }
    }
  }
  else
  {
    /* Read from USB interface */
    while(1)
    {
      ret = read(qbi_hc_linux_info.dev_fd, qbi_hc_linux_info.rx_buf,
          QBI_HC_LINUX_MAX_CTRL_XFER_SIZE);
      if (ret < 0)
      {
        QBI_LOG_E_2("Read failed with return value %d, errno %d", ret, errno);
        usleep(QBI_HC_LINUX_DEVFILE_RETRY_DELAY_US);
      }
      else if (ret == 0)
      {
        if (!qbi_task_cmd_send(
              &qbi_hc_linux_info.ctx, QBI_TASK_CMD_ID_RESET,
              qbi_hc_linux_reset_function_cmd_cb, NULL))
        {
          QBI_LOG_E_0("Couldn't pass RESET_FUNCTION command to QBI task!");
        }
      }
      else
      {
        qbi_hc_linux_rx_msg_cmd_send(
            &qbi_hc_linux_info.ctx, qbi_hc_linux_info.rx_buf, (uint32) ret);
      }
    }
  }

  /* Reader thread should never exit */
  return NULL;
} /* qbi_hc_linux_reader_thread_entry() */

/*===========================================================================
  FUNCTION: qbi_hc_linux_start_reader_thread
===========================================================================*/
/*!
    @brief starts reader thread. Reader thread is used to handle commands
    received from the host by the USB driver

    @details

    @param void

    @return int
*/
/*=========================================================================*/
int qbi_hc_linux_start_reader_thread
(
  void
)
{
  return  pthread_create(
    &reader_thread, NULL, qbi_hc_linux_reader_thread_entry, NULL);

} /* qbi_hc_linux_start_reader_thread() */

/*=============================================================================

  Public Function Definitions exposed in qbi_hc.h

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_hc_data_format_configure
===========================================================================*/
/*!
    @brief Configures the data format for MBIM using platform-specific means

    @details
    This function must not be called if
    qbi_hc_data_format_is_configured_by_hc() returns FALSE.

    @param ctx
    @param listener_txn Pointer to a configured listener transaction to be
    notified of the result of asynchronous operations

    @return boolean TRUE on success, FALSE on failure. Note that a return
    value of TRUE does not mean that configuration is complete - if any async
    operations are required, their result will be communicated through the
    listener transaction.
*/
/*=========================================================================*/
boolean qbi_hc_data_format_configure
(
  qbi_ctx_s *ctx,
  qbi_txn_s *listener_txn
)
{
  qbi_txn_s *txn;
  boolean success = FALSE;
  wda_set_data_format_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_LOG_I_0("Configuring data format for IPA");

  /* On 9x25, IPA is configured by the USB driver for MBIM aggregation, but QBI
     still needs to configure the modem for IP mode. We also disable modem-side
     aggregation to be sure there are no conflicts with IPA aggregation. */
  txn = qbi_txn_alloc(
    ctx, QBI_SVC_ID_UNKNOWN, 0, QBI_TXN_CMD_TYPE_INTERNAL, 0, 0, NULL);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);

  qmi_req = (wda_set_data_format_req_msg_v01 *) qbi_qmi_txn_alloc_ret_req_buf(
    txn, QBI_QMI_SVC_WDA, QMI_WDA_SET_DATA_FORMAT_REQ_V01,
    qbi_hc_linux_data_format_configure_wda01_rsp_cb);

  if (qmi_req == NULL)
  {
    QBI_LOG_E_0("Couldn't allocate QMI transaction or setup notifier!");
    qbi_txn_free(txn);
  }
  else
  {
    qmi_req->link_prot_valid = TRUE;
    qmi_req->link_prot = WDA_LINK_LAYER_IP_MODE_V01;
    qmi_req->dl_data_aggregation_protocol_valid = TRUE;
    qmi_req->dl_data_aggregation_protocol = WDA_DL_DATA_AGG_DISABLED_V01;
    qmi_req->ul_data_aggregation_protocol_valid = TRUE;
    qmi_req->ul_data_aggregation_protocol = WDA_DL_DATA_AGG_DISABLED_V01;

    /* qbi_svc_proc_action returns TRUE if the transaction was freed, which in
       this case means that QMI dispatch failed. Make sure that QMI dispatch was
       successful before setting up the notifier, so that the notify_cb will
       not be invoked in a synchronous context leading to potential problems
       with referencing a freed transaction, etc. */
    if (!qbi_svc_proc_action(txn, QBI_SVC_ACTION_SEND_QMI_REQ))
    {
      success = qbi_txn_notify_setup_notifier(listener_txn, txn);
    }
  }

  return success;
} /* qbi_hc_data_format_configure() */

/*===========================================================================
  FUNCTION: qbi_hc_data_format_is_configured_by_hc
===========================================================================*/
/*!
    @brief Determines whether the data format is configured by the host
    communications platform layer, and QMI_WDA_SET_DATA_FORMAT should not
    be used

    @details
    After 9x25 there is no new platform using HC for configuring data format.
    Keep this API available for now.

    @param ctx

    @return boolean TRUE if the data format should be configured by the
    host communications platform layer, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_hc_data_format_is_configured_by_hc
(
  const qbi_ctx_s *ctx
)
{
/*-------------------------------------------------------------------------*/
  QBI_ARG_NOT_USED(ctx);

  return FALSE;
} /* qbi_hc_data_format_is_configured_by_hc() */

/*===========================================================================
  FUNCTION: qbi_hc_diag_config_get_active
===========================================================================*/
/*!
    @brief Determines whether a DIAG endpoint is included in the active USB
    composition

    @details

    @param active_diag_config

    @return boolean TRUE if active_diag_config was set, FALSE if a failure
    occurred and active_diag_config was not modified
*/
/*=========================================================================*/
boolean qbi_hc_diag_config_get_active
(
  qbi_hc_diag_config_e *active_diag_config
)
{
  char pid[QBI_HC_LINUX_PID_STR_LEN + 1];
  const char *pid_file;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(active_diag_config);

  if (qbi_hc_linux_info.active_diag_config_initialized)
  {
    *active_diag_config = qbi_hc_linux_info.active_diag_config;
  }
  else
  {
    QBI_MEMSET(pid, 0, sizeof(pid));
    pid_file = (qbi_hc_linux_info.mbim_is_over_hsic) ?
      QBI_HC_LINUX_HSIC_ACTIVE_PID_FILE : QBI_HC_LINUX_HSUSB_ACTIVE_PID_FILE;
    if (!qbi_hc_linux_diag_config_read_pid_file(pid_file, pid))
    {
      QBI_LOG_E_0("Couldn't read active USB PID!");
    }
    else
    {
      (void) qbi_hc_linux_diag_config_tbl_lookup(pid, active_diag_config);
      QBI_LOG_STR_2("Active USB PID is %s - diag config %d",
                    pid, *active_diag_config);
      qbi_hc_linux_info.active_diag_config = *active_diag_config;
      qbi_hc_linux_info.active_diag_config_initialized = TRUE;
    }
  }

  return qbi_hc_linux_info.active_diag_config_initialized;
} /* qbi_hc_diag_config_get_active() */

/*===========================================================================
  FUNCTION: qbi_hc_diag_config_get_next
===========================================================================*/
/*!
    @brief Determine if the USB composition that will be used on next boot
    matches one of two preconfigured settings known to include or exclude
    a DIAG endpoint

    @details

    @param next_diag_config Will be set to the composition type

    @return boolean TRUE if next_diag_config was set, FALSE if a failure
    occurred and next_diag_config was not modified
*/
/*=========================================================================*/
boolean qbi_hc_diag_config_get_next
(
  qbi_hc_diag_config_e *next_diag_config
)
{
  char pid[QBI_HC_LINUX_PID_STR_LEN + 1];
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(next_diag_config);

  QBI_MEMSET(pid, 0, sizeof(pid));
  if (!qbi_hc_linux_diag_config_read_next_pid(pid))
  {
    QBI_LOG_E_0("Couldn't read next USB PID!");
  }
  else
  {
    (void) qbi_hc_linux_diag_config_tbl_lookup(pid, next_diag_config);
    QBI_LOG_STR_2("Next USB PID is %s - diag config %d",
                  pid, *next_diag_config);
    success = TRUE;
  }

  return success;
} /* qbi_hc_diag_config_get_next() */

/*===========================================================================
  FUNCTION: qbi_hc_diag_config_set_next
===========================================================================*/
/*!
    @brief Set the USB composition that will be used on next boot to one of
    two preconfigured settings, which either include or exclude a DIAG
    endpoint

    @details
    The new USB composition may require a device reset to take effect

    @param requested_diag_config

    @return qbi_hc_diag_config_set_status_e
*/
/*=========================================================================*/
qbi_hc_diag_config_set_status_e qbi_hc_diag_config_set_next
(
  qbi_hc_diag_config_e requested_diag_config
)
{
  qbi_hc_diag_config_set_status_e status;
  char pid[QBI_HC_LINUX_PID_STR_LEN];
  const qbi_hc_linux_diag_composition_pair_s *entry;
  qbi_hc_diag_config_e current_diag_config;
  int ret;
  const char *next_pid;
  char hsic;
  char cmd[QBI_HC_LINUX_USB_COMPOSITION_CMD_LEN];
/*-------------------------------------------------------------------------*/
  if (requested_diag_config != QBI_HC_DIAG_CONFIG_INCLUDED &&
      requested_diag_config != QBI_HC_DIAG_CONFIG_EXCLUDED)
  {
    QBI_LOG_E_1("Tried to set invalid diag config %d", requested_diag_config);
    status = QBI_HC_DIAG_CONFIG_SET_STATUS_INVALID_PARAM;
  }
  else if (!qbi_hc_linux_diag_config_read_next_pid(pid))
  {
    QBI_LOG_E_0("Couldn't read current composition!");
    status = QBI_HC_DIAG_CONFIG_SET_STATUS_READ_FAILURE;
  }
  else
  {
    entry = qbi_hc_linux_diag_config_tbl_lookup(pid, &current_diag_config);
    if (current_diag_config == QBI_HC_DIAG_CONFIG_UNKNOWN)
    {
      QBI_LOG_E_0("Current composition is unknown; not changing");
      status = QBI_HC_DIAG_CONFIG_SET_STATUS_UNKNOWN_CONFIG;
    }
    else if (requested_diag_config == current_diag_config)
    {
      QBI_LOG_I_1("Current composition matches requested diag config %d",
                  requested_diag_config);
      status = QBI_HC_DIAG_CONFIG_SET_STATUS_SUCCESS;
    }
    else
    {
      if (requested_diag_config == QBI_HC_DIAG_CONFIG_INCLUDED)
      {
        next_pid = entry->diag_included;
      }
      else
      {
        next_pid = entry->diag_excluded;
      }
      hsic = (qbi_hc_linux_info.mbim_is_over_hsic) ? 'y' : 'n';

      QBI_LOG_STR_3("Setting new composition %s for diag config %d hsic %c",
                    next_pid, requested_diag_config, hsic);
      (void) snprintf(
        cmd, sizeof(cmd), QBI_HC_LINUX_USB_COMPOSITION_SCRIPT " %s %c y n",
        next_pid, hsic);
      ret = system(cmd);
      if (ret == QBI_HC_LINUX_LIB_ERROR || !WIFEXITED(ret) ||
          WEXITSTATUS(ret) != 0)
      {
        QBI_LOG_E_1("Changing composition failed, return value %d", ret);
        if (ret != QBI_HC_LINUX_LIB_ERROR && WIFEXITED(ret))
        {
          QBI_LOG_E_1("Exited with status %d", WEXITSTATUS(ret));
        }
        status = QBI_HC_DIAG_CONFIG_SET_STATUS_WRITE_FAILURE;
      }
      else
      {
        status = QBI_HC_DIAG_CONFIG_SET_STATUS_SUCCESS;
      }
    }
  }

  return status;
} /* qbi_hc_diag_config_set_next() */

/*===========================================================================
  FUNCTION: qbi_hc_dpm_is_used
===========================================================================*/
/*!
    @brief Determines whether Data Port Mapper (DPM) is supported and
    required by the platform (e.g. MDM9x35)

    @details
    Note that this does not necessarily mean that QBI must send the DPM
    messages, only that DPM is used on the platform.

    @param ctx

    @return boolean
*/
/*=========================================================================*/
boolean qbi_hc_dpm_is_used
(
  const qbi_ctx_s *ctx
)
{
  const qbi_hc_linux_info_s *info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(ctx->hc_state);

  info = (const qbi_hc_linux_info_s *) ctx->hc_state;
  if (info->dpm_supported == QBI_HC_LINUX_TRISTATE_UNKNOWN)
  {
    /* Update the cache, including dpm_supported flag */
    (void) qbi_hc_dpm_get_data_port_info(ctx, NULL, NULL);
  }

  return (info->dpm_supported == QBI_HC_LINUX_TRISTATE_TRUE);
} /* qbi_hc_dpm_is_used() */

/*===========================================================================
  FUNCTION: qbi_hc_dpm_get_data_port_info
===========================================================================*/
/*!
    @brief Retrieves port details to use with DPM-related functionality

    @details

    @param ctx
    @param data_ep_id
    @param ep_info Hardware-accelerated data port info. Optional, can be
    NULL

    @return boolean TRUE if data populated successfully, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_hc_dpm_get_data_port_info
(
  const qbi_ctx_s                *ctx,
  data_ep_id_type_v01            *data_ep_id,
  hardware_accl_port_details_v01 *ep_info
)
{
  int ret;
  qbi_hc_linux_info_s *info;
  boolean data_copied = FALSE;
  uint32 retry_count = 0;
  int32 request;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(ctx->hc_state);

  info = (qbi_hc_linux_info_s *) ctx->hc_state;
  while (info->dpm_supported == QBI_HC_LINUX_TRISTATE_UNKNOWN)
  {
    request = qbi_hc_pcie_is_enabled(ctx) ?
    QBI_HC_LINUX_MHI_IOCTL_EP_LOOKUP : QBI_HC_LINUX_IOCTL_EP_LOOKUP;
    ret = ioctl(info->dev_fd, request, &info->ep_info);
    if (ret == QBI_HC_LINUX_LIB_ERROR)
    {
      if (errno == EINVAL)
      {
        QBI_LOG_I_0("Driver does not support EP_LOOKUP (DPM not used)");
        info->dpm_supported = QBI_HC_LINUX_TRISTATE_FALSE;
      }
      else
      {
        /* It's possible that USB or PCIe were not ready for the IOCTL yet, so retry */
        QBI_LOG_E_2("Couldn't get endpoint details from driver: %d (retry %d)",
                    errno, retry_count);
        if (retry_count++ > QBI_HC_LINUX_EP_LOOKUP_RETRY_COUNT)
        {
          QBI_LOG_E_0("Reached maximum number of retries - giving up!");
          break;
        }
        usleep(QBI_HC_LINUX_EP_LOOKUP_RETRY_DELAY_US);
      }
    }
    else
    {
      info->dpm_supported = QBI_HC_LINUX_TRISTATE_TRUE;
    }
  }

  if (info->dpm_supported == QBI_HC_LINUX_TRISTATE_TRUE && data_ep_id != NULL)
  {
    QBI_MEMSCPY(data_ep_id, sizeof(data_ep_id_type_v01),
                &info->ep_info.ep_id, sizeof(info->ep_info.ep_id));
    data_copied = TRUE;
  }
  if (info->dpm_supported == QBI_HC_LINUX_TRISTATE_TRUE && ep_info != NULL)
  {
    QBI_MEMSCPY(ep_info, sizeof(hardware_accl_port_details_v01),
                &info->ep_info, sizeof(info->ep_info));
    data_copied = TRUE;
  }

  return data_copied;
} /* qbi_hc_dpm_get_data_port_info() */

/*===========================================================================
  FUNCTION: qbi_hc_dpm_get_mux_id
===========================================================================*/
/*!
    @brief Returns the mux_id to be used for data calls on this session.

    @details

    @param ctx
    @param session_id session id for this data call.

    @return mux_id to be used.
*/
/*=========================================================================*/
uint8_t qbi_hc_dpm_get_mux_id
(
  const qbi_ctx_s *ctx,
  uint8_t          session_id
)
{
/*-------------------------------------------------------------------------*/
  QBI_ARG_NOT_USED(ctx);

  /*! @note Changing the mux id to anything other than session id will cause
      USB Device Emulation (UDE) not to work (i.e UDE assumes mux id is
      session id) */ 
  return session_id;
} /* qbi_hc_dpm_get_mux_id() */

/*===========================================================================
  FUNCTION: qbi_hc_get_dl_ntb_max_datagrams
===========================================================================*/
/*!
    @brief Returns the maximum number of aggregated datagrams set by host

    @details Returns maximum number of aggregated datagrams. For USB case
    ioctl() is called. For PCIe case return 32. For error case return 0

    @param ctx

    @return uint32
*/
/*=========================================================================*/
uint32 qbi_hc_get_dl_ntb_max_datagrams
(
  const qbi_ctx_s *ctx
)
{
  int ret;
  const qbi_hc_linux_info_s *info;
  uint16 max_dgrams = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ZERO(ctx);
  QBI_CHECK_NULL_PTR_RET_ZERO(ctx->hc_state);

  info = (const qbi_hc_linux_info_s *) ctx->hc_state;

  if (!qbi_hc_pcie_is_enabled(ctx))
  {
    ret = ioctl(info->dev_fd, QBI_HC_LINUX_IOCTL_GET_DATAGRAM_COUNT, &max_dgrams);
    if (ret == QBI_HC_LINUX_LIB_ERROR)
    {
      QBI_LOG_E_1("Couldn't get max datagrams per NTB from driver: %d", errno);
      max_dgrams = 0;
    }
    QBI_LOG_I_1("Maximum datagrams per NTB for USB: %d", max_dgrams);
  }
  else
  {
    max_dgrams = 32;
    QBI_LOG_I_1("Maximum datagrams per NTB for PCIe: %d", max_dgrams);
  }

  return (uint32) max_dgrams;
} /* qbi_hc_get_dl_ntb_max_datagrams() */

/*===========================================================================
  FUNCTION: qbi_hc_get_max_segment_size
===========================================================================*/
/*!
    @brief Returns the maximum segment size supported by the device

    @details
    This is the wMaxSegmentSize value reported in the MBIM functional
    descriptor. The device must never report an MTU that is larger than
    this value.

    @param ctx

    @return uint32
*/
/*=========================================================================*/
uint32 qbi_hc_get_max_segment_size
(
  const qbi_ctx_s *ctx
)
{
/*-------------------------------------------------------------------------*/
  QBI_ARG_NOT_USED(ctx);

  return qbi_hc_linux_is_ipa_supported() ?
    QBI_HC_LINUX_MAX_SEGMENT_SIZE_IPA : QBI_HC_LINUX_MAX_SEGMENT_SIZE_A2;
} /* qbi_hc_get_max_segment_size() */

/*===========================================================================
  FUNCTION: qbi_hc_get_dl_ntb_max_size
===========================================================================*/
/*!
    @brief Returns the maximum size of aggregated packet set by host

    @details Checks if MBIM is over USB of PCIe. For USB case an ioctl() is
    called. For PCIe case returns 32*1024. For Error case returns 0

    @param ctx

    @return uint32
*/
/*=========================================================================*/
uint32 qbi_hc_get_dl_ntb_max_size
(
  const qbi_ctx_s *ctx
)
{
  int ret;
  const qbi_hc_linux_info_s *info;
  uint32 ntb_max_size = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ZERO(ctx);
  QBI_CHECK_NULL_PTR_RET_ZERO(ctx->hc_state);

  info = (const qbi_hc_linux_info_s *) ctx->hc_state;

  if (!qbi_hc_pcie_is_enabled(ctx))
  {
    ret = ioctl(info->dev_fd, QBI_HC_LINUX_IOCTL_GET_NTB_SIZE, &ntb_max_size);
    if (ret == QBI_HC_LINUX_LIB_ERROR)
    {
      QBI_LOG_E_1("Couldn't get NTB max size from driver: %d", errno);
      ntb_max_size = 0;
    }
    QBI_LOG_I_1("Maximum NTB size for USB: %d", ntb_max_size);
  }
  else
  {
    /* set max_size to 32*1024 */
    ntb_max_size = 32768;
    QBI_LOG_I_1("Maximum NTB size for PCIe: %d", ntb_max_size);
  }

  return ntb_max_size;
} /* qbi_hc_get_dl_ntb_max_size() */

/*===========================================================================
  FUNCTION: qbi_hc_get_max_xfer
===========================================================================*/
/*!
    @brief Returns the maximum control transfer size supported by the device

    @details

    @return uint32
*/
/*=========================================================================*/
uint32 qbi_hc_get_max_xfer
(
  void
)
{
/*-------------------------------------------------------------------------*/
  return QBI_HC_LINUX_MAX_CTRL_XFER_SIZE;
} /* qbi_hc_max_xfer() */

/*===========================================================================
  FUNCTION: qbi_hc_init
===========================================================================*/
/*!
    @brief Perform one-time initialization of the host communications layer

    @details
    This implementation must be run after qbi_task_init() to allow it to
    register its context.
*/
/*=========================================================================*/
void qbi_hc_init
(
  void
)
{
/*-------------------------------------------------------------------------*/
  qbi_hc_linux_info.dev_fd = QBI_HC_LINUX_LIB_ERROR;
  qbi_hc_linux_info.ctx.id = QBI_CTX_ID_PRIMARY;
  qbi_hc_linux_info.ctx.hc_state = (void *) &qbi_hc_linux_info;
  qbi_hc_linux_info.dpm_supported = QBI_HC_LINUX_TRISTATE_UNKNOWN;
  qbi_hc_linux_info.pcie_enabled = QBI_HC_LINUX_TRISTATE_UNKNOWN;
  qbi_task_register_context(&qbi_hc_linux_info.ctx);
} /* qbi_hc_init() */

/*===========================================================================
  FUNCTION: qbi_hc_get_bind_data_port
===========================================================================*/
/*!
    @brief Map session ID to data port used for binding operation

    @details

    @param session_id

    @return uint16
*/
/*=========================================================================*/
uint16 qbi_hc_get_bind_data_port
(
  uint32 session_id
)
{
  uint16_t sio_port = QBI_HC_BIND_DATA_PORT_0;
/*-------------------------------------------------------------------------*/
  switch (session_id)
  {
    case 0:
      sio_port = QBI_HC_BIND_DATA_PORT_0;
      break;

    case 1:
      sio_port = QBI_HC_BIND_DATA_PORT_1;
      break;

    case 2:
      sio_port = QBI_HC_BIND_DATA_PORT_2;
      break;

    case 3:
      sio_port = QBI_HC_BIND_DATA_PORT_3;
      break;

    case 4:
      sio_port = QBI_HC_BIND_DATA_PORT_4;
      break;

    case 5:
      sio_port = QBI_HC_BIND_DATA_PORT_5;
      break;

    case 6:
      sio_port = QBI_HC_BIND_DATA_PORT_6;
      break;

    case 7:
      sio_port = QBI_HC_BIND_DATA_PORT_7;
      break;

    default:
      QBI_LOG_E_1("Unexpected port index %d!", session_id);
      break;
  }

  return sio_port;
} /* qbi_hc_get_bind_data_port */

/*===========================================================================
  FUNCTION: qbi_hc_tx
===========================================================================*/
/*!
    @brief Transmit data to the host

    @details

    @param ctx
    @param data
    @param len
*/
/*=========================================================================*/
void qbi_hc_tx
(
  qbi_ctx_s *ctx,
  void      *data,
  uint32     len
)
{
  int ret;
  qbi_hc_linux_info_s *info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);
  QBI_CHECK_NULL_PTR_RET(data);
  #ifdef QBI_UNIT_TEST
  if (ctx->id == QBI_CTX_ID_UNIT_TEST)
  {
    qbi_ut_rx(ctx, data, len);
    return;
  }
  #endif /* QBI_UNIT_TEST */
  QBI_CHECK_NULL_PTR_RET(ctx->hc_state);

  info = (qbi_hc_linux_info_s *) ctx->hc_state;
  qbi_log_pkt(ctx->id, QBI_LOG_PKT_DIRECTION_TX, data, len);

  qbi_os_mutex_lock(&qbi_hc_linux_info.mhi_monitor_write_op);
  ret = write(info->dev_fd, data, len);
  qbi_os_mutex_unlock(&qbi_hc_linux_info.mhi_monitor_write_op);

  if (ret == QBI_HC_LINUX_LIB_ERROR)
  {
    QBI_LOG_E_1("Couldn't send message to host: %d", errno);
  }
  else if (ret != len)
  {
    QBI_LOG_E_3("Unexpected return value when writing to device file: got %d, "
                "expected %d (errno %d)", ret, len, errno);
  }

  if ((ctx->state == QBI_CTX_STATE_OPENED && !info->is_session_open) ||
      (ctx->state == QBI_CTX_STATE_CLOSED && info->is_session_open))
  {
    qbi_hc_linux_crash_recovery_update_session_file();
    if (ctx->state == QBI_CTX_STATE_CLOSED)
    {
      info->dpm_supported = QBI_HC_LINUX_TRISTATE_UNKNOWN;
    }
  }
} /* qbi_hc_tx() */

/*===========================================================================
  FUNCTION: qbi_hc_pcie_is_enabled
===========================================================================*/
/*!
    @brief checks if PCIe is enabled

    @details reads pcie_enabled filed from qbi_hc_linux_info_s

    @param ctx

    @return boolean
*/
/*=========================================================================*/
boolean qbi_hc_pcie_is_enabled
(
  qbi_ctx_s *ctx
)
{
  const qbi_hc_linux_info_s *info;
/*------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);

  info = (qbi_hc_linux_info_s *) ctx->hc_state;

  return (info->pcie_enabled == QBI_HC_LINUX_TRISTATE_TRUE);
} /* qbi_hc_pcie_is_enabled() */

/*===========================================================================
  FUNCTION: qbi_hc_linux_mhi_monitor_thread_entry
===========================================================================*/
/*!
    @brief Entry point for the MHI Monitor thread. Opens netlink socket to
    receive uevents from the kernel and act upon it.

    @details Opens netlink socket and starts polling uevents from the kernel.
    Parses the uevent, checks if it is from MHI driver. If uevent from MHI
    then, get the status and act on it. Else disregard the uevent.

    @param arg

    @return void*
*/
/*=========================================================================*/
static void *qbi_hc_linux_monitor_thread_entry
(
  void *arg
)
{
  int ret = QBI_HC_LINUX_LIB_ERROR;
  int poll_ret = QBI_HC_LINUX_LIB_ERROR;
  struct pollfd pfd;
  int timeout = -1;
  ssize_t recv_msg_len = 0;
  char msg[QBI_HC_LINUX_UEVENT_BUFFER_SIZE];
  qbi_hc_mhi_ctrl_state_e mhi_state;
/*-------------------------------------------------------------------------*/
  QBI_ARG_NOT_USED(arg);

  QBI_LOG_I_0("MHI Monitor Thread started");
  pfd.fd = qbi_hc_linux_open_netlink_socket(NETLINK_KOBJECT_UEVENT, getpid(),
    QBI_HC_LINUX_UEVENT_BROADCAST_GROUP);
  if(pfd.fd < 0)
  {
    QBI_LOG_E_0("Error creating netlink socket. Terminating Monitor Thread.");
    sem_post(&qbi_hc_linux_info.mhi_monitor_semaphore);
    return;
  }

  pfd.events = POLLIN;
  mhi_state = qbi_hc_linux_get_mhi_state_from_uevent_file();
  qbi_hc_linux_update_mhi_channel_state(mhi_state);

  while (1)
  {
    /* polling data from mhi_ctrl file */
    /* specifying -1 means wait infinite amount of time. */
    poll_ret = poll(&pfd, 1, timeout);
    if (poll_ret < 0)
    {
      QBI_LOG_E_0("Poll from mhi_ctrl file returned with error."
                 " Terminating Monitor Thread.");
      break;
    }

    if (pfd.revents & POLLIN)
    {
      recv_msg_len = recv(pfd.fd, (void *)&msg, sizeof(msg) - 1,
        MSG_DONTWAIT | MSG_TRUNC);
      if (recv_msg_len <= 0)
      {
        if (recv_msg_len == 0)
        {
          QBI_LOG_E_0("Error recevied empty uevent msg. Ignoring msg");
        }
        else
        {
          QBI_LOG_E_1("Error receving uevent msg %d. Ignoring msg", errno);
        }
        continue;
      }
      /*! @note discard the msg if its truncated. MHI driver uevents msgs are
          of size smaller than QBI_HC_LINUX_UEVENT_BUFFER_SIZE. So MHI msgs
          should never be truncated. */
      else if (recv_msg_len > sizeof(msg) - 1)
      {
        QBI_LOG_E_0("Error uevent datagram is truncated");
        continue;
      }
      else
      {
        /* Ensure that msg received is null terminated */
        msg[recv_msg_len] = '\0';
        mhi_state = qbi_hc_linux_parse_mhi_uevent(msg, recv_msg_len + 1);
        if (mhi_state != QBI_HC_MHI_CTRL_UNKNOWN)
        {
          qbi_hc_linux_update_mhi_channel_state(mhi_state);
        }
      }
    }
  }

  close(pfd.fd);
  return;
}/* qbi_hc_linux_mhi_monitor_thread_entry() */

/*===========================================================================
  FUNCTION: qbi_hc_linux_update_mhi_channel_state
===========================================================================*/
/*!
    @brief Compares the current mhi state with previous mhi state and handles
    transition from one state to another.

    @details

    @param mhi_state

    @return TRUE
*/
/*=========================================================================*/
static boolean qbi_hc_linux_update_mhi_channel_state
(
  qbi_hc_mhi_ctrl_state_e mhi_state
)
{
  qbi_hc_mhi_ctrl_state_e previous_mhi_channel_state;
  qbi_hc_mhi_ctrl_state_e current_mhi_channel_state;
  int pthread_kill_err;
/*-------------------------------------------------------------------------*/
  previous_mhi_channel_state = qbi_hc_linux_info.mhi_ctrl_state;
  current_mhi_channel_state =  mhi_state;

  QBI_LOG_I_2("Previous MHI channel state %d. Current MHI channel state %d",
     previous_mhi_channel_state, current_mhi_channel_state);

  /* Given previous_mhi_channel_state and current_mhi_channel_state,
     now take action accordingly. */
  if (previous_mhi_channel_state != QBI_HC_MHI_CTRL_CONNECTED &&
      current_mhi_channel_state == QBI_HC_MHI_CTRL_CONNECTED)
  {
    qbi_hc_linux_info.mhi_ctrl_state = QBI_HC_MHI_CTRL_CONNECTED;
    sem_post(&qbi_hc_linux_info.mhi_monitor_semaphore);
  }
  if (previous_mhi_channel_state == QBI_HC_MHI_CTRL_CONNECTED &&
      current_mhi_channel_state != QBI_HC_MHI_CTRL_CONNECTED)
  {
    qbi_hc_linux_info.mhi_ctrl_state = current_mhi_channel_state;
    qbi_os_mutex_lock(&qbi_hc_linux_info.mhi_monitor_write_op);
    /*! note once we MHI layer gives and indication that PCIe Interface is not
        connected anymore, then call terminate the mbimd.
        This is written under the assumption that some LE module will keep on
        to trigger mbimd. */
    /*! No need to release mutex lock since process itself is killed */
    pthread_kill_err = pthread_kill(reader_thread, SIGTERM);
    if (pthread_kill_err == 0)
    {
      QBI_LOG_I_0("pthread_kill() was sent successfully ");
    }
    else
    {
      QBI_LOG_E_1("pthread_kill() is not successful error: %d, Resetting is"
                  " required.",pthread_kill_err);
    }
  }
  return TRUE;
} /* qbi_hc_linux_update_mhi_channel_state() */

/*===========================================================================
  FUNCTION: qbi_hc_linux_get_mhi_state_from_uevent_file
===========================================================================*/
/*!
    @brief Gets MHI state by reading from MHI specific uevent file.

    @details When MBIM comes up there is a chance where it might have missed
    the uevent broadcast from MHI. So when MBIM initializes it will read the
    MHI state from MHI uevent file.

    @param void

    @return State of mhi channel 12
*/
/*=========================================================================*/
static qbi_hc_mhi_ctrl_state_e qbi_hc_linux_get_mhi_state_from_uevent_file
(
  void
)
{
  int uevent_fd = QBI_HC_LINUX_LIB_ERROR;
  ssize_t msg_len = 0;
  char msg[QBI_HC_LINUX_UEVENT_BUFFER_SIZE] = {0};
  qbi_hc_mhi_ctrl_state_e mhi_state = QBI_HC_MHI_CTRL_UNKNOWN;
/*-------------------------------------------------------------------------*/
  uevent_fd = open(QBI_HC_LINUX_MHI_UEVENT_REQUEST_PATH, O_RDONLY | O_CLOEXEC);
  if (uevent_fd == QBI_HC_LINUX_LIB_ERROR)
  {
    QBI_LOG_E_1("Error opening MHI uevent file %d",errno);
  }

  QBI_LOG_I_0("Reading MHI state from uevent file");
  msg_len = read(uevent_fd, msg, sizeof(msg) - 1);
  close(uevent_fd);

  if (msg_len == QBI_HC_LINUX_LIB_ERROR)
  {
    QBI_LOG_E_1("Error reading MHI state from MHI uevent file %d",errno);
  }
  else if (msg_len == 0)
  {
    QBI_LOG_E_0("Error MHI uevent file is empty");
  }
  else
  {
    /* Ensure that the msg is null terminated */
    msg[msg_len] = '\0';
    mhi_state = qbi_hc_linux_parse_mhi_uevent(msg, msg_len + 1);
  }

  return mhi_state;
} /* qbi_hc_linux_get_mhi_state_from_uevent_file() */

/*===========================================================================
  FUNCTION: qbi_hc_linux_open_netlink_socket
===========================================================================*/
/*!
    @brief Opens a general purpose netlink socket

    @details

    @param protcol
    @param pid
    @param multicast group

    @return int netlink socket descriptor on Success, QBI_HC_LINUX_LIB_ERROR
    on Failure.
*/
/*=========================================================================*/
static int qbi_hc_linux_open_netlink_socket
(
  int    protocol,
  pid_t  pid,
  uint32 group
)
{
  int netlink_sock_fd = QBI_HC_LINUX_LIB_ERROR;
  struct sockaddr_nl netlink_sock_src_addr;
  int netlink_sock_buf_size = QBI_HC_LINUX_UEVENT_BUFFER_SIZE;
  int ret = QBI_HC_LINUX_LIB_ERROR;
/*-------------------------------------------------------------------------*/
  memset(&netlink_sock_src_addr, 0, sizeof(netlink_sock_src_addr));
  netlink_sock_src_addr.nl_family  = AF_NETLINK;
  netlink_sock_src_addr.nl_pid     = pid;
  netlink_sock_src_addr.nl_groups  = group;
  netlink_sock_fd = socket(PF_NETLINK, SOCK_DGRAM | SOCK_CLOEXEC,
    protocol);
  if (netlink_sock_fd < 0)
  {
    QBI_LOG_E_0("Error creating uevent netlink socket");
  }
  else
  {
    if (setsockopt(netlink_sock_fd, SOL_SOCKET, SO_RCVBUF,
      &netlink_sock_buf_size, sizeof(netlink_sock_buf_size)) < 0)
    {
      QBI_LOG_E_1("Error setting netlink socket options %d", errno);
      close(netlink_sock_fd);
    }
    else if (bind(netlink_sock_fd,
      (const struct sockaddr *)&netlink_sock_src_addr,
      sizeof(netlink_sock_src_addr)) < 0)
    {
      QBI_LOG_E_1("Error binding netlink socket %d",errno);
      close(netlink_sock_fd);
    }
    else
    {
      ret = netlink_sock_fd;
    }
  }

  return ret;
} /* qbi_hc_linux_open_netlink_socket() */

/*===========================================================================
  FUNCTION: qbi_hc_linux_parse_mhi_uevent
===========================================================================*/
/*!
    @brief Parses the MHI uevent message passed

    @details Receives a null terminated stream of bytes. Replaces all the
    null characters in stream with space except the last null byte.
    Checks the MHI states in parsed null terminated stream.

    @param msg
    @param msg_len

    @return State of mhi channel 12
*/
/*=========================================================================*/
static qbi_hc_mhi_ctrl_state_e qbi_hc_linux_parse_mhi_uevent
(
  const char *msg,
  ssize_t msg_len
)
{
  char parsed_msg[QBI_HC_LINUX_UEVENT_BUFFER_SIZE] = {0};
  ssize_t index = 0;
  char *mhi_str = NULL;
  qbi_hc_mhi_ctrl_state_e mhi_state = QBI_HC_MHI_CTRL_UNKNOWN;
/*-------------------------------------------------------------------------*/
  while (index < msg_len)
  {
    index += snprintf(parsed_msg + index, msg_len - index, "%s ",
      msg + index);
  }

  mhi_str = strstr(parsed_msg, QBI_HC_LINUX_MHI_CHNEL);
  if (NULL != mhi_str)
  {
    /* The passed msg contains information about MHI state. Try to get the
       state */
    if (NULL != strstr(mhi_str, QBI_HC_LINUX_MHI_CHNEL_CONFIGURED))
    {
      QBI_LOG_I_0("Received MHI uevent with CONFIGURED state");
      mhi_state = QBI_HC_MHI_CTRL_CONFIGURED;
    }
    else if (NULL != strstr(mhi_str, QBI_HC_LINUX_MHI_CHNEL_CONNECTED))
    {
      QBI_LOG_I_0("Received MHI uevent with CONNECTED state");
      mhi_state = QBI_HC_MHI_CTRL_CONNECTED;
    }
    else if (NULL != strstr(mhi_str, QBI_HC_LINUX_MHI_CHNEL_DISCONNECTED))
    {
      QBI_LOG_I_0("Received MHI uevent with DISCONNECTED state");
      mhi_state = QBI_HC_MHI_CTRL_DISCONNECTED;
    }
  }

  return mhi_state;
} /* qbi_hc_linux_parse_mhi_uevent() */
