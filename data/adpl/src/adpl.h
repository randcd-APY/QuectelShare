
/******************************************************************************

                        ADPL.H

******************************************************************************/

/******************************************************************************

  @file    adpl.h
  @brief   Accelerated Data Path Logging module

  DESCRIPTION
  Header file for Accelerated DPL module.

  ---------------------------------------------------------------------------
  Copyright (c) 2014-2015,2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

******************************************************************************/


/******************************************************************************

                      EDIT HISTORY FOR FILE

  $Id:$

when       who        what, where, why
--------   ---        --------------------------------------------------------
04/25/18   vv         Initial version

******************************************************************************/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#ifndef __ADPL_H
#define __ADPL_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_addr.h>
#include <linux/rtnetlink.h>
#include <linux/netlink.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <stdint.h>
#include <stdarg.h>
#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#define strlcat g_strlcat
#endif

#ifndef FEATURE_MDM_LE
 #include <cutils/properties.h>
#endif

#include "ds_util.h"
#include "qmi_client.h"
#include "qmi_qmux_if.h"
#include "qmi_port_defs.h"
#include "qmi_client_instance_defs.h"
#include "data_common_v01.h"
#include "qcmap_utils.h"

/*===========================================================================
                              MACRO DEFINITIONS
===========================================================================*/

/*--------------------------------------------------------------------------
 Generic definitions
---------------------------------------------------------------------------*/
#define ADPL_SUCCESS   0
#define ADPL_FAILURE   (-1)
#define TRUE          1
#define FALSE         0

/*--------------------------------------------------------------------------
  RNDIS/ECM definitions
---------------------------------------------------------------------------*/

#define MAX_NUM_OF_FD                        10
#define ADPL_QMI_MSG_TIMEOUT_VALUE           90000
#define ADPL_FILE_NODE_OPEN_MAX_RETRY        10
#define ADPL_QMI_CLIENT_MAX_RETRY            10
#define ADPL_MAX_FILE_NAME_SIZE              50
#define ONE_SEC                              1
#define ADPL_MAX_COMMAND_STR_LEN             300
#define ADPL_MAX_TRANSFER_SIZE               2048
#define FRMNET_CTRL_IOCTL_MAGIC              'r'
#define FRMNET_CTRL_GET_LINE_STATE           _IOR(FRMNET_CTRL_IOCTL_MAGIC, 2, int)
#define FRMNET_CTRL_EP_LOOKUP                _IOR(FRMNET_CTRL_IOCTL_MAGIC, 3, ep_info_type)
#define MHI_UCI_IOCTL_MAGIC                  'm'
#define MHI_UCI_DPL_EP_LOOKUP                _IOR(MHI_UCI_IOCTL_MAGIC, 3, ep_info_type)
#define SET_DTR_HIGH                         1
#define SET_DTR_LOW                          0
#define ADPL_USB_RETRY                       5

#define ADPL_USB_DEV_FILE             "/dev/dpl_ctrl"
#define ADPL_USB_DEV_NODE             "/dev/dpl_ctrl0"
#define ADPL_MHI_DEV_FILE             "/dev/mhi_pipe_14"

#define MHI_ADPL_DATA_INTERFACE       "rmnet_mhi1"
#define USB_ADPL_DATA_INTERFACE       "dpl_usb0"

#define PROPERTY_MTU_SIZE                    5
#define ADPL_PROPERTY_VALUE_MAX              256
#define MAX_COMMAND_STR_LEN                  200
#define ADPL_PROPERTY_AGGR            "persist.data.dpl.aggr.enable"
#define ADPL_PROPERTY_AGGR_SIZE              (5)
#define ADPL_PROPERTY_AGGR_DEFAULT    ADPL_MTU_INVALID
#define ADPL_MTU_INVALID                     (0)
#define ADPL_MTU_MAX                         (100000)
#define ADPL_SETUP_RETRY_MAX                 5
#define ADPL_QMI_MAX_TIMEOUT                 10000
#define ADPL_MAX_BUF_LEN                     256
#define ADPL_MAX_LEN                         10
#define ADPL_MAX_PH_DRIVER                    2

#define MHI_CONNECTED                         1
#define MHI_DISCONNECTED                      0

#define DEFAULT_MTU_MRU_VALUE                2000
#define ADPL_NOTIFY_FOLDER            "/systemrw/data/adpl"
#define ADPL_NOTIFY_NODE              "/systemrw/data/adpl/mhi_enable"
#define ADPL_NOTIFY_FILE_NAME         "mhi_enable"
#define ADPL_CONFIG_FILE              "/systemrw/data/adpl/adpl_config"
#define ADPL_MODE_TAG                 "mode"

/*INOTIFY MACROS*/
#define INOTIFY_EVENT_SIZE  (sizeof(struct inotify_event))
#define INOTIFY_BUF_LEN     (INOTIFY_EVENT_SIZE + NAME_MAX + 1)
/*===========================================================================
                              VARIABLE DECLARARTIONS
===========================================================================*/
/*--------------------------------------------------------------------------
  GENERIC DECLARATIONS
---------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
  FILE LOGGING DECALRATIONS
---------------------------------------------------------------------------*/
  boolean       file_log_mode;

/*--------------------------------------------------------------------------
  DPM CLIENT DECALRATIONS
---------------------------------------------------------------------------*/
  qmi_client_type                      dpm_notifier;

/*--------------------------------------------------------------------------
   ADPL config modes
---------------------------------------------------------------------------*/
typedef enum
{
  DEFAULT_MODE = 0,
  PCIE_ONLY,
  USB_ONLY
} adpl_config_e;

/*--------------------------------------------------------------------------
   Events that need to propagated to QCMAP from QTI
---------------------------------------------------------------------------*/
typedef enum
{
  ADPL_DPM_MODEM_NOT_IN_SERVICE_EVENT = 1,
  ADPL_DPM_INIT_EVENT
} adpl_event_e;

/*--------------------------------------------------------------------------
  RMNET DECLARATIONS
---------------------------------------------------------------------------*/

typedef enum {
  DATA_EP_TYPE_RESERVED   = 0x00,
  /*Reserved */
  DATA_EP_TYPE_HSIC       = 0x01,
  /*HSIC*/
  DATA_EP_TYPE_HSUSB      = 0x02,
  /*HSUSB */
  DATA_EP_TYPE_PCIE       = 0x03,
  /* PCIE */
  DATA_EP_TYPE_EMBEDDED   = 0x04
  /* Embedded */
} peripheral_ep_type_enum;

typedef enum {
  PH_DRIVER_TYPE_USB   = 0x00,
  PH_DRIVER_TYPE_MHI   = 0x01
}peripheral_driver_type_enum;

typedef struct {
  peripheral_ep_type_enum  ep_type;
  /* Peripheral end point type. Values:
       @ENUM()
       All other values are reserved and will be ignored by service or clients.
  */
  uint32_t peripheral_iface_id;
  /* Peripheral interface number.*/
} peripheral_ep_info_type;


typedef struct {
  uint32_t consumer_pipe_num;
  /* Consumer pipe number for the hardware accelerated port*/

  uint32_t producer_pipe_num;
  /* Producer pipe number for the hardware accelerated port*/
} ipa_ep_pair_type;

typedef struct
{
  peripheral_ep_info_type ph_ep_info;

  ipa_ep_pair_type ipa_ep_pair;

} ep_info_type;

typedef struct
{
  int                          ph_iface_fd;
  char                         ph_iface_device_file[ADPL_MAX_FILE_NAME_SIZE];
  char                         ph_data_iface_name[ADPL_MAX_FILE_NAME_SIZE];
  boolean                      ph_enabled;
}ph_iface_info;

/*-------------------------------------------------------------------------
  Structure to hold parameters for ADPL accelerated path enablement.
--------------------------------------------------------------------------*/
typedef struct {
  ph_iface_info             ph_iface[ADPL_MAX_PH_DRIVER];
  int                       adpl_iface_fd;
  int                       modem_iface_fd;
  qcmap_sk_fd_set_info_t    *fd_set;
  ep_info_type              ep_info;
  boolean                   dtr_enabled;
  qmi_client_type           adpl_dpm_handle;
  ds_target_t               target;
  boolean                   mhi_connected;

  adpl_config_e             adpl_mode;
  qmi_connection_id_type    qmux_conn_id;
  qmi_qmux_if_hndl_t        adpl_qmux_qmi_handle;
} adpl_param;

/*============================================================
Log Message Macros
=============================================================*/
#define LOG_MSG_INFO1_LEVEL           MSG_LEGACY_MED
#define LOG_MSG_INFO2_LEVEL           MSG_LEGACY_MED
#define LOG_MSG_INFO3_LEVEL           MSG_LEGACY_LOW
#define LOG_MSG_ERROR_LEVEL           MSG_LEGACY_ERROR

/*============================================================
Log Message Print Api's
=============================================================*/

void PRINT_msg(const char *funcName, uint32_t lineNum, const char*fmt, ...);
#undef PRINT_MSG
#define PRINT_MSG( level, fmtString, x, y, z)                         \
        if ( file_log_mode )                                         \
        {                                                            \
          PRINT_msg( __FUNCTION__, __LINE__, fmtString, x, y, z);\
        }\
        else\
        {\
          MSG_SPRINTF_4( MSG_SSID_LINUX_DATA, level, "%s(): " fmtString,      \
                         __FUNCTION__, x, y, z);\
        }\

#define PRINT_MSG_6( level, fmtString, a, b, c, d, e, f)              \
        if ( file_log_mode )\
        {\
          PRINT_msg( __FUNCTION__, __LINE__, fmtString, a, b, c, d, e, f);   \
        }\
        else\
        {\
          MSG_SPRINTF_7( MSG_SSID_LINUX_DATA, level, "%s(): " fmtString,            \
                         __FUNCTION__, a, b, c, d, e, f);\
        }\

#undef LOG_MSG_INFO1
#undef LOG_MSG_INFO2
#undef LOG_MSG_INFO3
#undef LOG_MSG_ERROR
#define LOG_MSG_INFO1( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_INFO1_LEVEL, fmtString, x, y, z);                \
}
#define LOG_MSG_INFO2( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_INFO2_LEVEL, fmtString, x, y, z);                \
}
#define LOG_MSG_INFO3( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_INFO3_LEVEL, fmtString, x, y, z);                \
}
#define LOG_MSG_INFO1_6( fmtString, a, b, c, d, e, f)                 \
{                                                                     \
  PRINT_MSG_6 ( LOG_MSG_INFO1_LEVEL, fmtString, a, b, c, d, e, f);    \
}
#define LOG_MSG_ERROR( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_ERROR_LEVEL, fmtString, x, y, z);                \
}
#define LOG_MSG_ERROR_6( fmtString, a, b, c, d, e, f)                 \
{                                                                     \
  PRINT_MSG_6( LOG_MSG_ERROR_LEVEL, fmtString, a, b, c, d, e, f);     \
}


/*===========================================================================
                       FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================

FUNCTION PRINT_BUFFER()

DESCRIPTION

  This function
  - prints the QMI packet.
  - used for debugging.

DEPENDENCIES
  None.

RETURN VALUE
  ADPL_SUCCESS on success
  ADPL_FAILURE on failure

SIDE EFFECTS
  None

=========================================================================*/
void print_buffer
(
  char *buf,
  int size
);

/*===========================================================================

FUNCTION ADPL_DPM_INIT()

DESCRIPTION
  Initializes a Data Port Mapper QMI client for ADPL

DEPENDENCIES
  None.

RETURN VALUE
  ADPL_SUCCESS on success
  ADPL_FAILURE on failure

SIDE EFFECTS
  None

=========================================================================*/

int adpl_dpm_init
(
   adpl_param         * adpl_state
);

/*===========================================================================

FUNCTION ADPL_MODEM_RECV_MSG_THRU_QMUXD()

DESCRIPTION
  Receives message from modem to support SSR.

DEPENDENCIES
  None.

RETURN VALUE
  ADPL_SUCCESS on success
  ADPL_FAILURE on failure

SIDE EFFECTS
  None

=========================================================================*/

void adpl_modem_recv_msg_thru_qmuxd
(
  qmi_connection_id_type conn_id,
  qmi_service_id_type    service_id,
  qmi_client_id_type     client_id,
  unsigned char          control_flags,
  unsigned char         *rx_msg,
  int                    rx_msg_len
);

/*===========================================================================

FUNCTION ADPL_MODEM_INIT_THRU_QMUXD()

DESCRIPTION

  This function
  - Function to register for SSR via qmuxd in Fusion targets.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

=========================================================================*/

int adpl_modem_init_thru_qmuxd
(
  adpl_param          * adpl_state,
  qcmap_sock_thrd_fd_read_f read_f
);

/*===========================================================================

FUNCTION ADPL_DATA_INIT_BRIDGE()

DESCRIPTION
  - initializes the RmNet data driver

DEPENDENCIES
  None.

RETURN VALUE
  ADPL_SUCCESS on success
  ADPL_FAILURE on failure


SIDE EFFECTS
  None

=========================================================================*/

int adpl_data_init_bridge();


/*===========================================================================

FUNCTION ADPL_DATA_TEARDOWN_BRIDGE()

DESCRIPTION
  resets RmNet data driver

DEPENDENCIES
  None.

RETURN VALUE
  ADPL_SUCCESS on success
  ADPL_FAILURE on failure


SIDE EFFECTS
  None

=========================================================================*/

int adpl_data_teardown_bridge();

/*===========================================================================

FUNCTION ADPL_PH_SET_MTU()

DESCRIPTION
  - sets the MTU on the peripheral interface

DEPENDENCIES
  None.

RETURN VALUE
  ADPL_SUCCESS on success
  ADPL_FAILURE on failure

SIDE EFFECTS
  None


==========================================================================*/
int adpl_ph_set_mtu
(
  uint32_t mtu,
  char* data_iface
);

/*===========================================================================

FUNCTION ADPL_MODEM_GET_EP_ID()

DESCRIPTION
  - obtains PCIe EP ID

DEPENDENCIES
  None.

RETURN VALUE
  ADPL_SUCCESS on success
  ADPL_FAILURE on failure

SIDE EFFECTS
  None

==========================================================================*/

int adpl_modem_get_ep_id
(
  uint32_t *ep_id,
  char* data_iface
);

/*===========================================================================

FUNCTION ADPL_MODEM_SET_MRU()

DESCRIPTION
  - sets MRU on modem interface

DEPENDENCIES
  None.

RETURN VALUE
  ADPL_SUCCESS on success
  ADPL_FAILURE on failure

SIDE EFFECTS
  None

==========================================================================*/
int adpl_modem_set_mru
(
  uint32_t mru,
  char* data_iface
);

/*===========================================================================

FUNCTION ADPL_PH_INIT()

DESCRIPTION

  This function
  - opens the ph device file for ADPL logging
  - adds the ph fd to wait on select call

DEPENDENCIES
  None.

RETURN VALUE
  ADPL_SUCCESS on success
  ADPL_FAILURE on failure

SIDE EFFECTS
  None

=========================================================================*/

int adpl_ph_init
(
  adpl_param           * adpl_state,
  qcmap_sk_fd_set_info_t    * fd_set,
  qcmap_sock_thrd_fd_read_f read_f
);

/*===========================================================================

FUNCTION ADPL_PROCESS_PH_RESET()

DESCRIPTION
  - processes peripheral reset for ADPL logging
  - upon USB cable plug-in : DPM open
  - upon USB cable plug-out : DPM close.

DEPENDENCIES
  None.

RETURN VALUE
  ADPL_SUCCESS on success
  ADPL_FAILURE on failure


SIDE EFFECTS
  None

=========================================================================*/
int adpl_process_ph_reset();

/*===========================================================================

FUNCTION ADPL_PH_RECV_MSG()

DESCRIPTION

  This function
  - receives data from USB interface.

DEPENDENCIES
  None.

RETURN VALUE
  ADPL_SUCCESS on success
  ADPL_FAILURE on failure


SIDE EFFECTS
  None

=========================================================================*/
int adpl_ph_recv_msg
(
   int adpl_ph_fd
);

/*===========================================================================

FUNCTION ADPL_DPM_PORT_OPEN()

DESCRIPTION
  Opens the USB tethered ADPL port using DPM port open request

DEPENDENCIES
  None.

RETURN VALUE
  ADPL_SUCCESS on success
  ADPL_FAILURE on failure


SIDE EFFECTS
  None

=========================================================================*/

int adpl_dpm_port_open();

/*===========================================================================

FUNCTION ADPL_DPM_RELEASE()

DESCRIPTION
  Releases a DPM QMI client

DEPENDENCIES
  None.

RETURN VALUE
  ADPL_SUCCESS on success
  ADPL_FAILURE on failure


SIDE EFFECTS
  None

=========================================================================*/
int adpl_dpm_release();

/*===========================================================================

FUNCTION ADPL_MODEM_NOT_IN_SERVICE()

DESCRIPTION
  Function invoked during SSR functionality, when modem is not in service.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

=========================================================================*/
void adpl_modem_not_in_service();

/*===========================================================================

FUNCTION ADPL_MODEM_IN_SERVICE()

DESCRIPTION
  Function is used during SSR functionality, when modem is back in service.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

==========================================================================*/
void adpl_modem_in_service();

/*===========================================================================

FUNCTION DPM_ERROR_CB()

DESCRIPTION
  This function is invoked by dpm, to notify ADPL when modem is not in
  service.

DEPENDENCIES
  None.

RETURN VALUE


SIDE EFFECTS
  None

=========================================================================*/
void dpm_erro_cb
(
  __attribute__((unused)) qmi_client_type user_handle,
 __attribute__((unused)) qmi_client_error_type error,
  __attribute__((unused)) void *err_cb_data
);

/*=========================================================================

FUNCTION ADPL_DPM_PORT_CLOSE()

DESCRIPTION
  Closes the USB tethered ADPL port using DPM port close request

DEPENDENCIES
  None.

RETURN VALUE
  ADPL_SUCCESS on success
  ADPL_FAILURE on failure


SIDE EFFECTS
  None

=========================================================================*/
int adpl_dpm_port_close();


/*===========================================================================

FUNCTION DPM_SERVICE_AVAILABLE_CB()

DESCRIPTION
  This function is invoked by dpm, to notify ADPL when service is available.

DEPENDENCIES
  None.

RETURN VALUE


SIDE EFFECTS
  None

=========================================================================*/
void dpm_service_available_cb
(
   __attribute__((unused)) qmi_client_type user_handle,
   __attribute__((unused)) qmi_idl_service_object_type service_obj,
   __attribute__((unused)) qmi_client_notify_event_type service_event,
   __attribute__((unused)) void *notify_cb_data
);

/*===========================================================================

FUNCTION DPM_CLIENT_INIT()

DESCRIPTION
  This function is invoked to initialize DPM Client

DEPENDENCIES
  None.

RETURN VALUE


SIDE EFFECTS
  None

=========================================================================*/
int dpm_client_init();


/*===========================================================================

FUNCTION ADPL_PH_GET_EP_INFO()

DESCRIPTION
  Gets the ph, IPA EP information from ph

DEPENDENCIES
  None.

RETURN VALUE
  ADPL_SUCCESS on success
  ADPL_FAILURE on failure

SIDE EFFECTS
  None

=========================================================================*/

int adpl_ph_get_ep_info();

/*===========================================================================

FUNCTION ADPL_FILE_OPEN()

DESCRIPTION

  This function
  - opens the device file which is used for interfacing with peripheral

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

=========================================================================*/


int adpl_file_open
(
   char * adpl_dev_file,
   int  * fd
);

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
void adpl_check_mhi_state();


/*===========================================================================

FUNCTION APDL_PH_GET_MHI_STATE()

DESCRIPTION
  - gets the mhi state

DEPENDENCIES
  None.

RETURN VALUE
  MHI_STATE value on success
  ADPL_FAILURE on failure

SIDE EFFECTS
  None

==========================================================================*/

int adpl_ph_get_mhi_state();


/*===========================================================================

FUNCTION APDL_PH_SET_MHI_STATE()

DESCRIPTION
  - sets the mhi state

DEPENDENCIES
  None.

RETURN VALUE
  ADPL_SUCCESS value on success
  ADPL_FAILURE on failure

SIDE EFFECTS
  None

==========================================================================*/

int adpl_ph_set_mhi_state();

/*===========================================================================

FUNCTION ADPL_QMI_WDA_SET_DATA_FORMAT()

DESCRIPTION
  Initializes a WDA QMI client and set data format

DEPENDENCIES
  None.

RETURN VALUE
  ADPL_SUCCESS on success
  ADPL_FAILURE on failure


SIDE EFFECTS
  None

=========================================================================*/

int adpl_qmi_wda_set_data_format
(
   int aggr_size,
   int mhi_ep_type,
   int mhi_id
);

/*===========================================================================

FUNCTION ADPL_IS_IFACE_UP()

DESCRIPTION
- checks if the interface is up

DEPENDENCIES
  None.

RETURN VALUE
  TRUE on success
  FALSE on failure

SIDE EFFECTS
  None

==========================================================================*/
int adpl_is_iface_up
(
  char* data_iface
);

#endif /* __ADPL_CMDQ_H__ */
