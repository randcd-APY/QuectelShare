/*===========================================================================

                         M C M _ M O B I L E A P _ S V C _ H D L R. CPP

DESCRIPTION

  The MCM MobileAP Service Handler Source File.

EXTERNALIZED FUNCTIONS

  mcm_mobileap_svc_init()
    Register the MCM MobileAP service with QMUX for all applicable QMI links.

  Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/
/*===========================================================================

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/05/14    pm     Removed dependency on dss_new. Defined log message macros
02/24/14    vm     Changes to Enable/Disable Station Mode to be in accordance
                   with IoE 9x15
01/03/14    vm     Changes to support IoE on 9x25
10/08/13    pm     Using sockets for context switching when interrupts arrive
07/22/13    cp     Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mcm_client_v01.h"
#include "mcm_ipc.h"

#include "qmi_idl_lib.h"
#include "qmi_csi.h"
#include "mcm_common_v01.h"
#include "mcm_mobileap_v01.h"
#include "mcm_service_object_v01.h"
#include "mcm_mobileap_svc_hdlr.h"
#include "mcm_constants.h"
#include "diag_lsm.h"
#include "ds_util.h"
#include "ds_list.h"
#include "comdef.h"


#ifdef TARGET_IS_9615
#include "MCM_QCMAP_ConnectionManager.h"
#include "qcmap_cmdq.h"
#include "dsi_netctrl.h"
#else /* TARGET_IS_9615 */
#include "QCMAP_Client.h"
#endif /* !(TARGET_IS_9615) */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/
#define MCM_MOBILEAP_SVC_LOG(...)                         \
  fprintf( stderr, "%s %d:", __FILE__, __LINE__); \
  fprintf( stderr, __VA_ARGS__ )

#define MCM_MOBILEAP_SVC_HANDLE 0x65432


#ifdef TARGET_IS_9615
static MCM_QCMAP_ConnectionManager *QcMapMgr = NULL;
#else /* TARGET_IS_9615 */
QCMAP_Client *QcMapClient = NULL;
#endif /* !(TARGET_IS_9615) */

/* Server socket */
unsigned int svc_cb_sockfd;
/* Client socket */
unsigned int qcmap_cb_sockfd;

/*.Interrupt sockets */
unsigned int intr_clt_cb_sockfd;
unsigned int intr_svc_cb_sockfd;

/*SIGTERM Flag*/
boolean sigterm_received = FALSE;

/*Exit Function Flag*/
boolean exit_func_called = FALSE;

/* Time to wait for enabling mobile ap to make sure DSI is inited. */
#define MOBILEAP_INIT_TIMEOUT_SEC 1
#define MOBILEAP_INIT_TIMEOUT_USEC 100000

/*==========================================================================
                                DATA TYPES

===========================================================================*/

/*---------------------------------------------------------------------------
  MCM MOBILEAP Service Client state info
---------------------------------------------------------------------------*/
typedef struct {
  qmi_client_handle              clnt;
  unsigned int                   client_fd;

  boolean                        qcmap_status_ind_regd;
  int                            qcmap_op_ind_msg_id;
  boolean                        qcmap_op_ind_pending;
  unsigned int                   qcmap_op_token_id;
  qmi_req_handle                 qcmap_op_req_handle;


  boolean                        wwan_status_ind_regd;
  int                            wwan_op_ind_msg_id;
  boolean                        wwan_op_ind_pending;
#ifndef TARGET_IS_9615
  int                            ipv6_wwan_op_ind_msg_id;
  boolean                        ipv6_wwan_op_ind_pending;
  qmi_req_handle                 ipv6_wwan_op_req_handle;
#endif
  unsigned int                   wwan_op_token_id;
  qmi_req_handle                 wwan_op_req_handle;

  boolean                        nat_status_ind_regd;
  int                            nat_op_ind_msg_id;
  boolean                        nat_op_ind_pending;
  unsigned int                   nat_op_token_id;
  qmi_req_handle                 nat_op_req_handle;

  boolean                        client_enabled;
  uint32_t                          event_mask;
}mcm_mobileap_svc_client_info_type;

static mcm_mobileap_svc_state_info_type    mcm_mobileap_svc_state;
static qmi_csi_os_params               os_params;
static boolean                         mcm_mobileap_svc_inited = FALSE;
static mcm_mobileap_svc_handle_type mcm_mobileap_svc_handle;



mcm_error_t_v01 mcm_mobileap_errno[MAX_MCM_MOBILEAP_ERRNO] =
{
  MCM_ERROR_NO_EFFECT_V01,
  MCM_SUCCESS_V01,
  MCM_ERROR_INVALID_OPERATION_V01,
  MCM_ERROR_NOT_SUPPORTED_V01,
  MCM_ERROR_INVALID_HANDLE_V01,
  MCM_ERROR_NETWORK_NOT_READY_V01,
  MCM_SUCCESS_V01
};

#ifdef TARGET_IS_9615

/*===========================================================================
LOG Msg Macros
=============================================================================*/
#define LOG_MSG_INFO1_LEVEL           MSG_LEGACY_MED
#define LOG_MSG_INFO2_LEVEL           MSG_LEGACY_MED
#define LOG_MSG_INFO3_LEVEL           MSG_LEGACY_LOW
#define LOG_MSG_ERROR_LEVEL           MSG_LEGACY_ERROR
#define PRINT_MSG( level, fmtString, x, y, z)                         \
        MSG_SPRINTF_4( MSG_SSID_LINUX_DATA, level, "%s(): " fmtString,      \
                       __FUNCTION__, x, y, z);
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
#define LOG_MSG_ERROR( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_ERROR_LEVEL, fmtString, x, y, z);                \
}

#endif

/*===========================================================================

                               INTERNAL DATA

===========================================================================*/

/*---------------------------------------------------------------------------
  MCM MobileAP service command handlers
  forward declarations & cmd handler dispatch table definition
---------------------------------------------------------------------------*/


static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_event_register
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);


static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_enable(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_disable(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);


static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_bring_up_wwan(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_tear_down_wwan(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_get_nat_type
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_change_nat_type
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);



static qmi_csi_cb_error mcm_mobileap_svc_hdlr_get_auto_connect
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error mcm_mobileap_svc_hdlr_set_auto_connect
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_set_dmz
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);


static qmi_csi_cb_error mcm_mobileap_svc_hdlr_delete_dmz
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error mcm_mobileap_svc_hdlr_get_dmz
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error mcm_mobileap_svc_hdlr_get_ipsec_vpn_pt
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error mcm_mobileap_svc_hdlr_set_ipsec_vpn_pt
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error mcm_mobileap_svc_hdlr_get_pptp_vpn_pt
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error mcm_mobileap_svc_hdlr_set_pptp_vpn_pt
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error mcm_mobileap_svc_hdlr_get_l2tp_vpn_pt
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error mcm_mobileap_svc_hdlr_set_l2tp_vpn_pt
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

/*===========================================================================

                       FORWARD FUNCTION DECLARATIONS

===========================================================================*/
static qmi_csi_cb_error mcm_mobileap_svc_hdlr_client_connect_cb
(
  qmi_client_handle         client_handle,
  void                      *service_handle,
  void                      **connection_handle
);

static void mcm_mobileap_svc_hdlr_client_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_handle
);

static qmi_csi_cb_error mcm_mobileap_svc_hdlr_client_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error mcm_mobileap_svc_hdlr_get_firewall_handles_list
(
 mcm_mobileap_svc_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
 );

static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_delete_extd_firewall_entry
(
 mcm_mobileap_svc_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
 );

static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_get_extd_firewall_entry
(
 mcm_mobileap_svc_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
 );

static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_add_extd_firewall_entry
(
 mcm_mobileap_svc_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
 );


static qmi_csi_cb_error mcm_mobileap_svc_hdlr_set_firewall_config
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);


static qmi_csi_cb_error mcm_mobileap_svc_hdlr_get_nat_timeout
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error mcm_mobileap_svc_hdlr_set_nat_timeout
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error mcm_mobileap_svc_hdlr_get_roaming
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error mcm_mobileap_svc_hdlr_set_roaming
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_add_static_nat_entry
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_delete_static_nat_entry
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_get_static_nat_entry
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);


static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_disable_station_mode
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_enable_station_mode
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);


static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_enable_wlan
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_disable_wlan
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_set_dhcpd_config
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error mcm_mobileap_svc_hdlr_get_wwan_stats
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error mcm_mobileap_svc_hdlr_reset_wwan_stats
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error mcm_mobileap_svc_hdlr_get_ipv4_wwan_config
(
  mcm_mobileap_svc_client_info_type		 *clnt_info,
  qmi_req_handle			 req_handle,
  int 					 msg_id,
  void					*req_c_struct,
  int 					 req_c_struct_len,
  void					 *service_handle
);

static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_set_dualap_config
(
  mcm_mobileap_svc_client_info_type		 *clnt_info,
  qmi_req_handle			 req_handle,
  int 					 msg_id,
  void					*req_c_struct,
  int 					 req_c_struct_len,
  void					 *service_handle
);



static qmi_csi_cb_error (* const req_handle_table[])
(
 mcm_mobileap_svc_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
) =
{
  mcm_mobileap_svc_hdlr_enable,             /* Request handler for message ID 0x400 */
  mcm_mobileap_svc_hdlr_disable,            /* Request handler for message ID 0x401 */
  mcm_mobileap_svc_hdlr_bring_up_wwan,                /* Request handler for message ID 0x402 */
  mcm_mobileap_svc_hdlr_tear_down_wwan,               /* Request handler for message ID 0x403 */
  mcm_mobileap_svc_hdlr_add_static_nat_entry,         /* Request handler for message ID 0x404 */
  mcm_mobileap_svc_hdlr_get_static_nat_entry,         /* Request handler for message ID 0x405 */
  mcm_mobileap_svc_hdlr_delete_static_nat_entry,         /* Request handler for message ID 0x406 */
  mcm_mobileap_svc_hdlr_set_nat_timeout,                        /* Request handler for message ID 0x407 */
  mcm_mobileap_svc_hdlr_get_nat_timeout,                        /* Request handler for message ID 0x408 */
  mcm_mobileap_svc_hdlr_change_nat_type,                        /* Request handler for message ID 0x409 */
  mcm_mobileap_svc_hdlr_get_nat_type,                        /* Request handler for message ID 0x40A */
  mcm_mobileap_svc_hdlr_add_extd_firewall_entry,             /* Request handler for message ID 0x40B */
  mcm_mobileap_svc_hdlr_get_firewall_handles_list,           /* Request handler for message ID 0x40C */
  mcm_mobileap_svc_hdlr_get_extd_firewall_entry,             /* Request handler for message ID 0x40D */
  mcm_mobileap_svc_hdlr_delete_extd_firewall_entry,             /* Request handler for message ID 0x40E */
  mcm_mobileap_svc_hdlr_set_firewall_config,                   /* Request handler for message ID 0x40F */
  mcm_mobileap_svc_hdlr_set_dmz,             /* Request handler for message ID 0x410 */
  mcm_mobileap_svc_hdlr_delete_dmz,             /* Request handler for message ID 0x411 */
  mcm_mobileap_svc_hdlr_get_dmz,             /* Request handler for message ID 0x412 */
  mcm_mobileap_svc_hdlr_get_ipv4_wwan_config,             /* Request handler for message ID 0x413 */
  mcm_mobileap_svc_hdlr_get_wwan_stats,                   /* Request handler for message ID 0x414 */
  mcm_mobileap_svc_hdlr_reset_wwan_stats,                 /* Request handler for message ID 0x415 */
  mcm_mobileap_svc_hdlr_set_dhcpd_config,                 /* Request handler for message ID 0x416 */
  mcm_mobileap_svc_hdlr_enable_wlan,             /* Request handler for message ID 0x417 */
  mcm_mobileap_svc_hdlr_disable_wlan,             /* Request handler for message ID 0x418 */
  mcm_mobileap_svc_hdlr_get_ipsec_vpn_pt,         /* Request handler for message ID 0x419 */
  mcm_mobileap_svc_hdlr_set_ipsec_vpn_pt,         /* Request handler for message ID 0x41A */
  mcm_mobileap_svc_hdlr_get_pptp_vpn_pt,         /* Request handler for message ID 0x41B */
  mcm_mobileap_svc_hdlr_set_pptp_vpn_pt,         /* Request handler for message ID 0x41C */
  mcm_mobileap_svc_hdlr_get_l2tp_vpn_pt,         /* Request handler for message ID 0x41D */
  mcm_mobileap_svc_hdlr_set_l2tp_vpn_pt,         /* Request handler for message ID 0x41E */
  mcm_mobileap_svc_hdlr_set_auto_connect,         /* Request handler for message ID 0x41F */
  mcm_mobileap_svc_hdlr_get_auto_connect,         /* Request handler for message ID 0x420 */
  mcm_mobileap_svc_hdlr_set_roaming,             /* Request handler for message ID 0x421 */
  mcm_mobileap_svc_hdlr_get_roaming,             /* Request handler for message ID 0x422 */
  mcm_mobileap_svc_hdlr_set_dualap_config,             /* Request handler for message ID 0x423 */
  mcm_mobileap_svc_hdlr_enable_station_mode,             /* Request handler for message ID 0x424 */
  mcm_mobileap_svc_hdlr_disable_station_mode,             /* Request handler for message ID 0x425 */
  mcm_mobileap_svc_hdlr_event_register                      /* Request handler for message ID 0x426 */
};

/*===========================================================================

                       EXTERNAL FUNCTION DEFINTIONS

===========================================================================*/
void sighandler(int signal)
{
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
  int numBytes = 0, len;
  sockaddr_un client_endpt;

  LOG_MSG_INFO1("Sighandler : Signal Received", 0, 0, 0);

  client_endpt.sun_family = AF_UNIX;
  strcpy(client_endpt.sun_path, QCMAP_INTR_UDS_FILE);
  len = strlen(client_endpt.sun_path) + sizeof(client_endpt.sun_family);

  if ((numBytes = sendto(intr_clt_cb_sockfd, (void *)&signal, sizeof(int), 0,
       (struct sockaddr *)&client_endpt, len)) == -1)
  {
    LOG_MSG_ERROR("Send Failed from mcm callback context", 0, 0, 0);
  }
  else
  {
    LOG_MSG_INFO1("Send succeeded in mcm callback context", 0, 0, 0);
  }
  mcm_set_service_ready(MCM_MOBILEAP_SERVICE, 0);
  exit(1);
}

#ifndef TARGET_IS_9615

/* Call back function for Indications. */
static void mcm_mobileap_qcmap_client_callback
(
  qmi_client_type user_handle,     /* QMI user handle       */
  unsigned int    msg_id,          /* Indicator message ID  */
  void           *ind_buf,         /* Raw indication data   */
  unsigned int    ind_buf_len,     /* Raw data length       */
  void           *ind_cb_data      /* User call back handle */
)

{
  mcm_error_t_v01 mcm_err_num;
  mcm_mobileap_qcmap_client_event_buffer_t event_buffer;
  int numBytes = 0, len;
  sockaddr_un client_endpt;
  int i;

  /* ind_cb_data can be NULL. */
  ds_assert(ind_buf != NULL);

  LOG_MSG_INFO1("mcm_mobileap_qcmap_client_callback: Event Received %d"
                " handle %d", msg_id, user_handle, 0);

  client_endpt.sun_family = AF_UNIX;
  strcpy(client_endpt.sun_path, QCMAP_CB_UDS_FILE);
  len = strlen(client_endpt.sun_path) + sizeof(client_endpt.sun_family);

  event_buffer.qcmap_handle = user_handle;
  event_buffer.evt = msg_id;

  if (ind_buf_len < MAX_IND_STRUCT_LEN)
    memcpy(event_buffer.ind_buf, (char *)ind_buf, ind_buf_len);
  else
    memcpy(event_buffer.ind_buf, (char *)ind_buf, MAX_IND_STRUCT_LEN);

  event_buffer.ind_buf_len = ind_buf_len;

  if ((numBytes = sendto(qcmap_cb_sockfd, (void *)&event_buffer,
                          sizeof(mcm_mobileap_qcmap_client_event_buffer_t), 0,
                         (struct sockaddr *)&client_endpt, len)) == -1)
  {
    LOG_MSG_ERROR("Send Failed QCCI callback context", 0, 0, 0);
    return;
  }
  LOG_MSG_INFO1("Send succeeded in QCCI callback context", 0, 0, 0);

  return;
}


void mcm_mobileap_qcmap_client_ind
(mcm_mobileap_qcmap_client_event_buffer_t *event_buffer)
{
  qmi_client_error_type qmi_error;
  unsigned int client_index = 0;
  mcm_mobileap_svc_client_info_type *qcmap_cp = NULL;
  mcm_mobileap_svc_state_info_type *qcmap_sp;
  mcm_mobileap_enable_resp_msg_v01 mobileap_enable_resp_msg;
  mcm_mobileap_disable_resp_msg_v01 mobileap_disable_resp_msg;
  mcm_mobileap_bring_up_wwan_resp_msg_v01 bring_up_wwan_resp_msg;
  mcm_mobileap_tear_down_wwan_resp_msg_v01 tear_down_wwan_resp_msg;
  mcm_mobileap_unsol_event_ind_msg_v01 ind_msg;
  mcm_mobileap_station_mode_enable_resp_msg_v01 enable_sta_resp;
  mcm_mobileap_station_mode_disable_resp_msg_v01 disable_sta_resp;
  qmi_csi_error rc;
  qcmap_msgr_wwan_status_ind_msg_v01 wwan_ind_data;
  qcmap_msgr_wwan_status_enum_v01 conn_status;
  qmi_client_type user_handle;
  unsigned int    msg_id;
  void           *ind_buf;
  unsigned int    ind_buf_len;

  memset(&ind_msg, 0, sizeof(ind_msg));
  ds_assert(event_buffer != NULL);

  user_handle = event_buffer->qcmap_handle;    /* QMI user handle       */
  msg_id = event_buffer->evt;                  /* Indicator message ID  */
  ind_buf = event_buffer->ind_buf;             /* Raw indication data   */
  ind_buf_len = event_buffer->ind_buf_len;     /* Raw data length       */

  qcmap_sp = mcm_mobileap_svc_handle.cb_ptr->svc_cb;
  LOG_MSG_INFO1(" Recieved an indication: msg Id: 0x%0x ", msg_id, 0, 0);

  for (client_index = 0;
       client_index < MCM_MOBILEAP_SVC_MAX_CLIENT_HANDLES; client_index++)
  {
    qcmap_cp = (mcm_mobileap_svc_client_info_type *)
               (qcmap_sp->client_handle_list[client_index]);
    if (qcmap_cp)
    {
      LOG_MSG_INFO1(" Send an indication/response: msg Id: 0x%0x client id: %d",
                    msg_id, qcmap_cp->clnt, 0);
      if((msg_id == QMI_QCMAP_MSGR_BRING_UP_WWAN_IND_V01) ||
         (msg_id == QMI_QCMAP_MSGR_WWAN_STATUS_IND_V01))
      {
        qcmap_msgr_bring_up_wwan_ind_msg_v01 ind_data;

        /* If the msg_id is Bring Up WWAN */
        if (msg_id == QMI_QCMAP_MSGR_BRING_UP_WWAN_IND_V01)
        {
          qmi_error = qmi_client_message_decode(user_handle,
                                 QMI_IDL_INDICATION,
                                 msg_id,
                                 ind_buf,
                                 ind_buf_len,
                                 &ind_data,
                                 sizeof(qcmap_msgr_bring_up_wwan_ind_msg_v01));
          if (qmi_error != QMI_NO_ERR)
          {
            LOG_MSG_INFO1("mcm_mobileap_qcmap_client_ind:"
                          " qmi_client_message_decode error %d", qmi_error,0,0);
            break;
          }
          conn_status = ind_data.conn_status;
        }
        else
        {
          /* If the msg_id is WWAN Status Indication */
          qmi_error = qmi_client_message_decode(user_handle,
                                    QMI_IDL_INDICATION,
                                    msg_id,
                                    ind_buf,
                                    ind_buf_len,
                                    &wwan_ind_data,
                                    sizeof(qcmap_msgr_wwan_status_ind_msg_v01));
          if (qmi_error != QMI_NO_ERR)
          {
            LOG_MSG_INFO1("mcm_mobileap_qcmap_client_ind: "
                          "qmi_client_message_decode error %d",qmi_error, 0, 0);
            break;
          }
          conn_status = wwan_ind_data.wwan_status;
        }
        if (conn_status == QCMAP_MSGR_WWAN_STATUS_CONNECTED_V01)
        {
          if ((qcmap_cp->wwan_op_ind_pending) &&
              (qcmap_cp->wwan_op_ind_msg_id ==
               MCM_MOBILEAP_BRING_UP_WWAN_RESP_V01))
          {
            bring_up_wwan_resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;
            bring_up_wwan_resp_msg.resp.error = MCM_SUCCESS_V01;
            LOG_MSG_INFO1("Sending WAN Connected Response 0x%x",
                          qcmap_cp->wwan_op_ind_msg_id,0,0);
            qmi_csi_send_resp(qcmap_cp->wwan_op_req_handle,
                              qcmap_cp->wwan_op_ind_msg_id,
                              &bring_up_wwan_resp_msg,
                              sizeof(mcm_mobileap_bring_up_wwan_resp_msg_v01));
            qcmap_cp->wwan_op_ind_pending = FALSE;
            qcmap_cp->wwan_op_ind_msg_id = -1;
          }
          else if((qcmap_cp->event_mask) & MCM_MOBILEAP_WAN_CONNECTED_EVENT_MASK)
          {
            ind_msg.event_id = MCM_MOBILEAP_WAN_CONNECTED_EV_V01;
            LOG_MSG_INFO1("Sending WAN Connected Indication 0x%x",
                          ind_msg.event_id, 0, 0);
            rc = qmi_csi_send_ind(qcmap_cp->clnt,
                                  MCM_MOBILEAP_UNSOL_EVENT_IND_V01,
                                  &ind_msg,
                                  sizeof(mcm_mobileap_unsol_event_ind_msg_v01));
          }
        }
        else if (conn_status == QCMAP_MSGR_WWAN_STATUS_CONNECTING_FAIL_V01)
        {
          if ((qcmap_cp->wwan_op_ind_pending) &&
              (qcmap_cp->wwan_op_ind_msg_id ==
               MCM_MOBILEAP_BRING_UP_WWAN_RESP_V01))
          {
            bring_up_wwan_resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
            bring_up_wwan_resp_msg.resp.error = MCM_ERROR_CALL_FAILED_V01;
            LOG_MSG_INFO1("Sending WAN Connecting Fail Response 0x%x",
                          qcmap_cp->wwan_op_ind_msg_id, 0, 0);
            qmi_csi_send_resp(qcmap_cp->wwan_op_req_handle,
                              qcmap_cp->wwan_op_ind_msg_id,
                              &bring_up_wwan_resp_msg,
                              sizeof(mcm_mobileap_bring_up_wwan_resp_msg_v01));
            qcmap_cp->wwan_op_ind_pending = FALSE;
            qcmap_cp->wwan_op_ind_msg_id = -1;
          }
          else if((qcmap_cp->event_mask) &
                  (MCM_MOBILEAP_WAN_CONNECTING_FAIL_EVENT_MASK))
          {
            ind_msg.event_id = MCM_MOBILEAP_WAN_CONNECTING_FAIL_EV_V01;
            LOG_MSG_INFO1("Sending WAN Connecting Fail Indication 0x%x",
                          ind_msg.event_id, 0, 0);
            rc = qmi_csi_send_ind(qcmap_cp->clnt,
                                  MCM_MOBILEAP_UNSOL_EVENT_IND_V01,
                                  &ind_msg,
                                  sizeof(mcm_mobileap_unsol_event_ind_msg_v01));
          }
        }
        else if (conn_status == QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTED_V01)
        {
          if ((qcmap_cp->ipv6_wwan_op_ind_pending) &&
              (qcmap_cp->ipv6_wwan_op_ind_msg_id ==
               MCM_MOBILEAP_BRING_UP_WWAN_RESP_V01))
          {
            bring_up_wwan_resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;
            bring_up_wwan_resp_msg.resp.error = MCM_SUCCESS_V01;
            LOG_MSG_INFO1("Sending WAN IPv6 Connected Response 0x%x",
                          qcmap_cp->ipv6_wwan_op_ind_msg_id,0,0);
            qmi_csi_send_resp(qcmap_cp->ipv6_wwan_op_req_handle,
                              qcmap_cp->ipv6_wwan_op_ind_msg_id,
                              &bring_up_wwan_resp_msg,
                              sizeof(mcm_mobileap_bring_up_wwan_resp_msg_v01));
            qcmap_cp->ipv6_wwan_op_ind_pending = FALSE;
            qcmap_cp->ipv6_wwan_op_ind_msg_id = -1;
          }
          else if((qcmap_cp->event_mask) &
                  (MCM_MOBILEAP_WAN_IPv6_CONNECTED_EVENT_MASK))
          {
            ind_msg.event_id = MCM_MOBILEAP_WAN_IPv6_CONNECTED_EV_V01;
            LOG_MSG_INFO1("Sending WAN IPv6 Connected Indication 0x%x",
                          ind_msg.event_id,0,0);
            rc = qmi_csi_send_ind(qcmap_cp->clnt,
                                  MCM_MOBILEAP_UNSOL_EVENT_IND_V01,
                                  &ind_msg,
                                  sizeof(mcm_mobileap_unsol_event_ind_msg_v01));
          }
        }
        else if (conn_status == QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTING_FAIL_V01)
        {
          if ((qcmap_cp->ipv6_wwan_op_ind_pending) &&
              (qcmap_cp->ipv6_wwan_op_ind_msg_id ==
               MCM_MOBILEAP_BRING_UP_WWAN_RESP_V01))
          {
            bring_up_wwan_resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
            bring_up_wwan_resp_msg.resp.error = MCM_ERROR_CALL_FAILED_V01;
            LOG_MSG_INFO1("Sending WAN IPv6 Connecting Fail Response 0x%x",
                          qcmap_cp->ipv6_wwan_op_ind_msg_id,0,0);
            qmi_csi_send_resp(qcmap_cp->ipv6_wwan_op_req_handle,
                              qcmap_cp->ipv6_wwan_op_ind_msg_id,
                              &bring_up_wwan_resp_msg,
                              sizeof(mcm_mobileap_bring_up_wwan_resp_msg_v01));
            qcmap_cp->ipv6_wwan_op_ind_pending = FALSE;
            qcmap_cp->ipv6_wwan_op_ind_msg_id = -1;
          }
          else if((qcmap_cp->event_mask) &
                  (MCM_MOBILEAP_WAN_IPv6_CONNECTING_FAIL_EVENT_MASK))
          {
            ind_msg.event_id = MCM_MOBILEAP_WAN_IPv6_CONNECTING_FAIL_EV_V01;
            LOG_MSG_INFO1("Sending WAN IPv6 Connecting Fail Indication 0x%x",
                          ind_msg.event_id,0,0);
            rc = qmi_csi_send_ind(qcmap_cp->clnt,
                                  MCM_MOBILEAP_UNSOL_EVENT_IND_V01,
                                  &ind_msg,
                                  sizeof(mcm_mobileap_unsol_event_ind_msg_v01));
          }
        }
      }
      if ((msg_id == QMI_QCMAP_MSGR_TEAR_DOWN_WWAN_IND_V01) ||
          (msg_id == QMI_QCMAP_MSGR_WWAN_STATUS_IND_V01))
      {
        qcmap_msgr_tear_down_wwan_ind_msg_v01 ind_data;

        /* If the msg_id is Tear Down WWAN */
        if (msg_id == QMI_QCMAP_MSGR_TEAR_DOWN_WWAN_IND_V01)
        {
          qmi_error = qmi_client_message_decode(user_handle,
                                 QMI_IDL_INDICATION,
                                 msg_id,
                                 ind_buf,
                                 ind_buf_len,
                                 &ind_data,
                                 sizeof(qcmap_msgr_tear_down_wwan_ind_msg_v01));
          if (qmi_error != QMI_NO_ERR)
          {
            LOG_MSG_INFO1("mcm_mobileap_qcmap_client_ind:"
                          " qmi_client_message_decode error %d", qmi_error,0,0);
            break;
          }
          conn_status = ind_data.conn_status;
        }
        else
        {
          /* If the msg_id is WWAN Status Indication */
          qmi_error = qmi_client_message_decode(user_handle,
                                    QMI_IDL_INDICATION,
                                    msg_id,
                                    ind_buf,
                                    ind_buf_len,
                                    &wwan_ind_data,
                                    sizeof(qcmap_msgr_wwan_status_ind_msg_v01));
          if (qmi_error != QMI_NO_ERR)
          {
            LOG_MSG_INFO1("mcm_mobileap_qcmap_client_ind: "
                          "qmi_client_message_decode error %d",qmi_error, 0, 0);
                  break;
          }
          conn_status = wwan_ind_data.wwan_status;
        }
        if (conn_status == QCMAP_MSGR_WWAN_STATUS_DISCONNECTED_V01)
        {
          if ((qcmap_cp->wwan_op_ind_pending) &&
              (qcmap_cp->wwan_op_ind_msg_id ==
               MCM_MOBILEAP_TEAR_DOWN_WWAN_RESP_V01))
          {
            tear_down_wwan_resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;
            tear_down_wwan_resp_msg.resp.error = MCM_SUCCESS_V01;
            LOG_MSG_INFO1("Sending WAN Disconnected Response 0x%x",
                          qcmap_cp->wwan_op_ind_msg_id,0,0);
            qmi_csi_send_resp(qcmap_cp->wwan_op_req_handle,
                              qcmap_cp->wwan_op_ind_msg_id,
                              &tear_down_wwan_resp_msg,
                              sizeof(mcm_mobileap_tear_down_wwan_resp_msg_v01));
            qcmap_cp->wwan_op_ind_pending = FALSE;
            qcmap_cp->wwan_op_ind_msg_id = -1;
          }
          else if((qcmap_cp->event_mask) &
                   MCM_MOBILEAP_WAN_DISCONNECTED_EVENT_MASK)
          {
            ind_msg.event_id = MCM_MOBILEAP_WAN_DISCONNECTED_EV_V01;
            LOG_MSG_INFO1("Sending WAN Disconnected Indication 0x%x",
                          ind_msg.event_id, 0, 0);
            rc = qmi_csi_send_ind(qcmap_cp->clnt,
                                  MCM_MOBILEAP_UNSOL_EVENT_IND_V01,
                                  &ind_msg,
                                  sizeof(mcm_mobileap_unsol_event_ind_msg_v01));
          }
        }
        else if (conn_status == QCMAP_MSGR_WWAN_STATUS_DISCONNECTING_FAIL_V01)
        {
          if ((qcmap_cp->wwan_op_ind_pending) &&
              (qcmap_cp->wwan_op_ind_msg_id ==
               MCM_MOBILEAP_TEAR_DOWN_WWAN_RESP_V01))
          {
            tear_down_wwan_resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
            tear_down_wwan_resp_msg.resp.error = MCM_ERROR_INTERNAL_V01;
            LOG_MSG_INFO1("Sending WAN Disconnecting fail Response 0x%x",
                          qcmap_cp->wwan_op_ind_msg_id,0,0);
            qmi_csi_send_resp(qcmap_cp->wwan_op_req_handle,
                              qcmap_cp->wwan_op_ind_msg_id,
                              &tear_down_wwan_resp_msg,
                              sizeof(mcm_mobileap_tear_down_wwan_resp_msg_v01));
            qcmap_cp->wwan_op_ind_pending = FALSE;
            qcmap_cp->wwan_op_ind_msg_id = -1;
          }
        }
        else if (conn_status == QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTED_V01)
        {
          if ((qcmap_cp->ipv6_wwan_op_ind_pending) &&
              (qcmap_cp->ipv6_wwan_op_ind_msg_id ==
               MCM_MOBILEAP_TEAR_DOWN_WWAN_RESP_V01))
          {
            tear_down_wwan_resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;
            tear_down_wwan_resp_msg.resp.error = MCM_SUCCESS_V01;
            LOG_MSG_INFO1("Sending IPv6 WAN Disconnected Response 0x%x",
                          qcmap_cp->ipv6_wwan_op_ind_msg_id,0,0);
            qmi_csi_send_resp(qcmap_cp->ipv6_wwan_op_req_handle,
                              qcmap_cp->ipv6_wwan_op_ind_msg_id,
                              &tear_down_wwan_resp_msg,
                              sizeof(mcm_mobileap_tear_down_wwan_resp_msg_v01));
            qcmap_cp->ipv6_wwan_op_ind_pending = FALSE;
            qcmap_cp->ipv6_wwan_op_ind_msg_id = -1;
          }
          else if((qcmap_cp->event_mask) &
                  (MCM_MOBILEAP_WAN_IPv6_DISCONNECTED_EVENT_MASK))
          {
            ind_msg.event_id = MCM_MOBILEAP_WAN_IPv6_DISCONNECTED_EV_V01;
            LOG_MSG_INFO1("Sending WAN IPv6 Disconnected Indication 0x%x",
                          ind_msg.event_id,0,0);
            rc = qmi_csi_send_ind(qcmap_cp->clnt,
                                  MCM_MOBILEAP_UNSOL_EVENT_IND_V01,
                                  &ind_msg,
                                  sizeof(mcm_mobileap_unsol_event_ind_msg_v01));
          }
        }
        else if (conn_status ==
                 QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTING_FAIL_V01)
        {
          if ((qcmap_cp->ipv6_wwan_op_ind_pending) &&
              (qcmap_cp->ipv6_wwan_op_ind_msg_id ==
               MCM_MOBILEAP_TEAR_DOWN_WWAN_RESP_V01))
          {
            tear_down_wwan_resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
            tear_down_wwan_resp_msg.resp.error = MCM_ERROR_INTERNAL_V01;
            LOG_MSG_INFO1("Sending WAN IPv6 Disconnecting Fail Response 0x%x",
                          qcmap_cp->ipv6_wwan_op_ind_msg_id,0,0);
            qmi_csi_send_resp(qcmap_cp->ipv6_wwan_op_req_handle,
                              qcmap_cp->ipv6_wwan_op_ind_msg_id,
                              &tear_down_wwan_resp_msg,
                              sizeof(mcm_mobileap_tear_down_wwan_resp_msg_v01));
            qcmap_cp->ipv6_wwan_op_ind_pending = FALSE;
            qcmap_cp->ipv6_wwan_op_ind_msg_id = -1;
          }
        }
      }
      else if (msg_id == QMI_QCMAP_MSGR_MOBILE_AP_STATUS_IND_V01)
      {
        qcmap_msgr_mobile_ap_status_ind_msg_v01 ind_data;
        qmi_error = qmi_client_message_decode(user_handle,
                               QMI_IDL_INDICATION,
                               msg_id,
                               ind_buf,
                               ind_buf_len,
                               &ind_data,
                               sizeof(qcmap_msgr_mobile_ap_status_ind_msg_v01));
        if (qmi_error != QMI_NO_ERR)
        {
          LOG_MSG_INFO1("mcm_mobileap_qcmap_client_ind: "
                        "qmi_client_message_decode error %d",qmi_error, 0, 0);
          break;
        }
        if (ind_data.mobile_ap_status ==
            QCMAP_MSGR_MOBILE_AP_STATUS_DISCONNECTED_V01)
        {

          if(exit_func_called)
          {
            /*Close Client sockets*/
            close(qcmap_cb_sockfd);
            close(intr_clt_cb_sockfd);

            /*Close Server sockets*/
            close(svc_cb_sockfd);
            close(intr_svc_cb_sockfd);

            LOG_MSG_INFO1("Server exiting because EXIT FUNCTION was called",
                          0,0,0);
            qmi_csi_unregister(mcm_mobileap_svc_state.service_handle);
            exit(0);
          }

          if((qcmap_cp->qcmap_op_ind_pending) &&
             (qcmap_cp->qcmap_op_ind_msg_id == MCM_MOBILEAP_DISABLE_RESP_V01))
          {
            mobileap_disable_resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;
            mobileap_disable_resp_msg.resp.error = MCM_SUCCESS_V01;
            LOG_MSG_INFO1("Sending MobileAP Disabled Response 0x%x",
                          ind_msg.event_id,0,0);
            qmi_csi_send_resp(qcmap_cp->qcmap_op_req_handle,
                              qcmap_cp->qcmap_op_ind_msg_id,
                              &mobileap_disable_resp_msg,
                              sizeof(mcm_mobileap_disable_resp_msg_v01));
            qcmap_cp->qcmap_op_ind_pending = FALSE;
            qcmap_cp->qcmap_op_ind_msg_id = -1;
          }
          else if((qcmap_cp->event_mask) & (MCM_MOBILEAP_DISABLED_EVENT_MASK))
          {
            ind_msg.event_id = MCM_MOBILEAP_DISABLED_EV_V01;
            LOG_MSG_INFO1("Sending Disabled Indication 0x%x",
                          ind_msg.event_id,0,0);
            rc = qmi_csi_send_ind(qcmap_cp->clnt,
                                  MCM_MOBILEAP_UNSOL_EVENT_IND_V01,
                                  &ind_msg,
                                  sizeof(mcm_mobileap_unsol_event_ind_msg_v01));
          }
          if(sigterm_received)
          {
            /*Close Client sockets*/
            close(qcmap_cb_sockfd);
            close(intr_clt_cb_sockfd);

            /*Close Server sockets*/
            close(svc_cb_sockfd);
            close(intr_svc_cb_sockfd);

            LOG_MSG_INFO1("Server exiting because SIGTERM was sent to it",
                          0,0,0);
            qmi_csi_unregister(mcm_mobileap_svc_state.service_handle);
            exit(0);
          }
        }
      }
      else if (msg_id == QMI_QCMAP_MSGR_STATION_MODE_STATUS_IND_V01)
      {
        qcmap_msgr_station_mode_status_ind_msg_v01 ind_data;
        qmi_error = qmi_client_message_decode(user_handle,
                            QMI_IDL_INDICATION,
                            msg_id,
                            ind_buf,
                            ind_buf_len,
                            &ind_data,
                            sizeof(qcmap_msgr_station_mode_status_ind_msg_v01));
        if (qmi_error != QMI_NO_ERR)
        {
          LOG_MSG_INFO1("mcm_mobileap_qcmap_client_ind:"
                        " qmi_client_message_decode error %d",qmi_error, 0, 0);
          break;
        }
        if (ind_data.station_mode_status ==
            QCMAP_MSGR_STATION_MODE_CONNECTED_V01)
        {
          if((qcmap_cp->qcmap_op_ind_pending) &&
               (qcmap_cp->qcmap_op_ind_msg_id ==
                MCM_MOBILEAP_STATION_MODE_ENABLE_REQ_V01))
          {
            enable_sta_resp.resp.result = MCM_RESULT_SUCCESS_V01;
            enable_sta_resp.resp.error = MCM_SUCCESS_V01;
            LOG_MSG_INFO1("Sending Station Mode Enable Response 0x%x.",
                          qcmap_cp->qcmap_op_ind_msg_id,0,0);
            qmi_csi_send_resp(qcmap_cp->qcmap_op_req_handle,
                              qcmap_cp->qcmap_op_ind_msg_id,
                              &enable_sta_resp, sizeof(enable_sta_resp));
            qcmap_cp->qcmap_op_ind_pending = FALSE;
            qcmap_cp->qcmap_op_ind_msg_id = -1;
          }
          else if(qcmap_cp->event_mask & MCM_MOBILEAP_STA_CONNECTED_EVENT_MASK)
          {
            ind_msg.event_id = MCM_MOBILEAP_STA_CONNECTED_EV_V01;
            LOG_MSG_INFO1("Sending Station Connected Indication 0x%x",
                          ind_msg.event_id,0,0);
            rc = qmi_csi_send_ind(qcmap_cp->clnt,
                                  MCM_MOBILEAP_UNSOL_EVENT_IND_V01,
                                  &ind_msg,
                                  sizeof(mcm_mobileap_unsol_event_ind_msg_v01));
          }
        }
        else if (ind_data.station_mode_status ==
                 QCMAP_MSGR_STATION_MODE_DISCONNECTED_V01)
        {
          if((qcmap_cp->qcmap_op_ind_pending) &&
             (qcmap_cp->qcmap_op_ind_msg_id ==
              MCM_MOBILEAP_STATION_MODE_DISABLE_REQ_V01))
          {
            disable_sta_resp.resp.result = MCM_RESULT_SUCCESS_V01;
            disable_sta_resp.resp.error = MCM_SUCCESS_V01;
            LOG_MSG_INFO1("Sending MobileAP Disabled Response 0x%x.",
                          qcmap_cp->qcmap_op_ind_msg_id,0,0);
            qmi_csi_send_resp(qcmap_cp->qcmap_op_req_handle,
                              qcmap_cp->qcmap_op_ind_msg_id,
                              &disable_sta_resp, sizeof(disable_sta_resp));
            qcmap_cp->qcmap_op_ind_pending = FALSE;
            qcmap_cp->qcmap_op_ind_msg_id = -1;
          }
        }
      }
    }
  }
  return;
}
#endif /* !(TARGET_IS_9615) */

void mcm_mobileap_svc_handle_intr_cb(int signal)
{
  LOG_MSG_INFO1("mcm_mobileap_svc_handle_intr_cb : Processing signal",0,0,0);
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
#ifndef TARGET_IS_9615
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
#endif /* !(TARGET_IS_9615) */
  switch (signal)
  {
    case SIGINT:
    case SIGTERM:
      LOG_MSG_INFO1("SIGTERM Received",0,0,0);
#ifdef TARGET_IS_9615
      if (QcMapMgr)
      {
        sigterm_received = TRUE;
        QcMapMgr->Disable(&mcm_err_num);
      }
#else /* TARGET_IS_9615 */
     if (QcMapClient)
     {
       sigterm_received = TRUE;
       QcMapClient->DisableMobileAP(&qmi_err_num);
     }
#endif /* !(TARGET_IS_9615) */
      break;
#ifdef TARGET_IS_9615
    case SIGUSR1:
      LOG_MSG_INFO1("SIGUSR1 Received",0,0,0);
        {
         if(QcMapMgr->TetheringOnly())
         {
           QcMapMgr->SetQtiState(TRUE);
           if(QcMapMgr->GetAutoconnect(&mcm_err_num))
           {
             LOG_MSG_INFO1("Bring up WWAN call in tethering mode",0,0,0);
             if (QcMapMgr->ConnectBackHaul(&mcm_err_num))
             {
               LOG_MSG_INFO1("ConnectBackHaul succeeds",0,0,0);
             }
             else
             {
               LOG_MSG_INFO1("ConnectBackHaul fails",0,0,0);
             }
           }
         }
         else
           LOG_MSG_INFO1("Tethering is not enabled or autoconnect is disabled",0,0,0);
         break;
        }
    case SIGUSR2:
      LOG_MSG_INFO1("SIGUSR2 Received",0,0,0);
        {
          if(QcMapMgr->TetheringOnly())
          {
            LOG_MSG_INFO1("Tear down WWAN call in thethering mode",0,0,0);
            if (QcMapMgr->DisconnectBackHaul(&mcm_err_num))
            {
              LOG_MSG_INFO1("DisconnectBackHaul succeeds",0,0,0);
            }
            else
            {
              LOG_MSG_INFO1("DisconnectBackHaul fails",0,0,0);
            }
            QcMapMgr->SetQtiState(FALSE);
          }
          else
            LOG_MSG_INFO1("Tethering is not enabled",0,0,0);
          break;
        }
#endif /* TARGET_IS_9615 */
    default:
      LOG_MSG_INFO1("Received unexpected signal %d", signal,0,0);
      break;
  }
}

int create_socket(unsigned int *sockfd)
{

  if ((*sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == QCMAP_CM_ERROR)
  {
    LOG_MSG_ERROR("Error creating socket, errno: %d", errno, 0, 0);
    return QCMAP_CM_ERROR;
  }

  if(fcntl(*sockfd, F_SETFD, FD_CLOEXEC) < 0)
  {
    LOG_MSG_ERROR("Couldn't set Close on Exec, errno: %d", errno, 0, 0);
  }

  return QCMAP_CM_ENOERROR;
}

int create_svc_cb_socket()
{
  int val, rval;
  sockaddr_un qcmap_cb;
  int len;
  struct timeval rcv_timeo;

  rval = create_socket(&svc_cb_sockfd);
  if( rval == QCMAP_CM_ERROR || svc_cb_sockfd < 0 )
  {
    LOG_MSG_ERROR("failed to create svc_cb_sockfd ", 0, 0, 0);
    return QCMAP_CM_ERROR;
  }
  else if (svc_cb_sockfd >= FD_SETSIZE)
  {
    LOG_MSG_ERROR(" svc_cb_sockfd fd=%d is exceeding range", svc_cb_sockfd, 0, 0);
    return QCMAP_CM_ERROR;
  }
  rcv_timeo.tv_sec = 0;
  rcv_timeo.tv_usec = 100000;
  setsockopt(svc_cb_sockfd, SOL_SOCKET, SO_RCVTIMEO, (void *)&rcv_timeo, sizeof(rcv_timeo));
  val = fcntl(svc_cb_sockfd, F_GETFL, 0);
  fcntl(svc_cb_sockfd, F_SETFL, val | O_NONBLOCK);
  /* set bit in os_params */
  FD_SET(svc_cb_sockfd, &os_params.fds);
  os_params.max_fd = MAX(os_params.max_fd, svc_cb_sockfd);

  qcmap_cb.sun_family = AF_UNIX;
  strcpy(qcmap_cb.sun_path, QCMAP_CB_UDS_FILE);
  unlink(qcmap_cb.sun_path);
  len = strlen(qcmap_cb.sun_path) + sizeof(qcmap_cb.sun_family);
  if (bind(svc_cb_sockfd, (struct sockaddr *)&qcmap_cb, len) == QCMAP_CM_ERROR)
  {
    LOG_MSG_ERROR("Error binding the socket, errno: %d", errno, 0, 0);
    return QCMAP_CM_ERROR;
  }
  return QCMAP_CM_ENOERROR;
}

int create_intr_svc_cb_socket()
{
  int val, rval;
  sockaddr_un qcmap_cb;
  int len;
  struct timeval rcv_timeo;

  rval = create_socket(&intr_svc_cb_sockfd);
  if( rval == QCMAP_CM_ERROR || intr_svc_cb_sockfd < 0 )
  {
    LOG_MSG_ERROR("failed to create intr_svc_cb_sockfd ", 0, 0, 0);
    return QCMAP_CM_ERROR;
  }
  else if (intr_svc_cb_sockfd >= FD_SETSIZE)
  {
    LOG_MSG_ERROR(" intr_svc_cb_sockfd fd =%d is exceeding range", intr_svc_cb_sockfd, 0, 0);
    return QCMAP_CM_ERROR;
  }

  rcv_timeo.tv_sec = 0;
  rcv_timeo.tv_usec = 100000;
  setsockopt(intr_svc_cb_sockfd, SOL_SOCKET, SO_RCVTIMEO, (void *)&rcv_timeo, sizeof(rcv_timeo));
  val = fcntl(intr_svc_cb_sockfd, F_GETFL, 0);
  fcntl(intr_svc_cb_sockfd, F_SETFL, val | O_NONBLOCK);
  /* set bit in os_params */
  FD_SET(intr_svc_cb_sockfd, &os_params.fds);
  os_params.max_fd = MAX(os_params.max_fd, intr_svc_cb_sockfd);
  qcmap_cb.sun_family = AF_UNIX;
  strcpy(qcmap_cb.sun_path, QCMAP_INTR_UDS_FILE);
  unlink(qcmap_cb.sun_path);
  len = strlen(qcmap_cb.sun_path) + sizeof(qcmap_cb.sun_family);
  if (bind(intr_svc_cb_sockfd, (struct sockaddr *)&qcmap_cb, len) == QCMAP_CM_ERROR)
  {
    LOG_MSG_ERROR("Error binding the socket, errno: %d", errno, 0, 0);
    return QCMAP_CM_ERROR;
  }
  return QCMAP_CM_ENOERROR;
}

#ifdef TARGET_IS_9615
void mcm_mobileap_evnt_hndl_fn(int handle, uint32_t evt, void *user_data)
{
  mcm_mobileap_svc_client_info_type *qcmap_cp=NULL;
  mcm_mobileap_svc_status_cb_data   *cb_data;
  qmi_csi_error rc;
  unsigned int client_index=0;
  unsigned int softAp_index;
  mcm_mobileap_svc_handle_type    *map_handle;
  mcm_mobileap_svc_state_info_type *qcmap_sp;
  mcm_mobileap_enable_resp_msg_v01   mobileap_enable_resp_msg;
  mcm_mobileap_disable_resp_msg_v01   mobileap_disable_resp_msg;
  mcm_mobileap_bring_up_wwan_resp_msg_v01 bring_up_wwan_resp_msg;
  mcm_mobileap_tear_down_wwan_resp_msg_v01 tear_down_wwan_resp_msg;
  mcm_mobileap_set_nat_type_resp_msg_v01 change_nat_type_resp_msg;
  mcm_mobileap_unsol_event_ind_msg_v01    ind_msg;
  boolean change_nat_type_requested = FALSE;

  LOG_MSG_INFO1("mcm_mobileap_evnt_hndl_fn : enter",0,0,0);

  map_handle = (mcm_mobileap_svc_handle_type *) user_data;
  if (map_handle != NULL)
  {
    cb_data = map_handle->cb_ptr;
  }
  else
    return;

  if ( evt == QCMAP_CM_EVENT_WAN_DISCONNECTED &&
       QcMapMgr->GetNatTypeChangeStatus() )
  {
    change_nat_type_requested = TRUE;
  }

  QcMapMgr->MCM_MOBILEAP_handle_callback_events(handle,(qcmap_cm_event_e) evt, map_handle->Mgr);

  memset(&ind_msg, 0, sizeof(mcm_mobileap_unsol_event_ind_msg_v01));

  if (map_handle->cb_ptr != NULL)
  {
    qcmap_sp = cb_data->svc_cb;
  }
  else
    return;

  for (client_index=0; client_index < MCM_MOBILEAP_SVC_MAX_CLIENT_HANDLES; client_index++)
  {
    qcmap_cp = (mcm_mobileap_svc_client_info_type *)(qcmap_sp->client_handle_list[client_index]);
    if ( qcmap_cp)
    {
      switch(evt)
      {
        case QCMAP_CM_EVENT_ENABLED:
        if ((qcmap_cp->qcmap_op_ind_pending) &&
            (qcmap_cp->qcmap_op_ind_msg_id == MCM_MOBILEAP_ENABLE_RESP_V01))
        {
          LOG_MSG_INFO1("Sending Enabled Message.",0,0,0);
          memset(&mobileap_enable_resp_msg, 0, sizeof(mcm_mobileap_enable_resp_msg_v01));
          mobileap_enable_resp_msg.mcm_mobileap_handle_valid = TRUE;
          mobileap_enable_resp_msg.mcm_mobileap_handle = MCM_MOBILEAP_SVC_HANDLE;
          mobileap_enable_resp_msg.resp.result= MCM_RESULT_SUCCESS_V01;
          mobileap_enable_resp_msg.resp.error= MCM_SUCCESS_V01;
          qmi_csi_send_resp(qcmap_cp->qcmap_op_req_handle, qcmap_cp->qcmap_op_ind_msg_id,
                            &mobileap_enable_resp_msg,
                            sizeof(mcm_mobileap_enable_resp_msg_v01));
          qcmap_cp->qcmap_op_ind_pending = FALSE;
          qcmap_cp->qcmap_op_ind_msg_id = -1;

        }
        else if((qcmap_cp->event_mask) & (MCM_MOBILEAP_ENABLED_EVENT_MASK))
        {
          ind_msg.event_id = MCM_MOBILEAP_ENABLED_EV_V01;
          LOG_MSG_INFO1("Sending Enabled Indication 0x%x", ind_msg.event_id,0,0);
          rc = qmi_csi_send_ind(qcmap_cp->clnt, MCM_MOBILEAP_UNSOL_EVENT_IND_V01,
                                &ind_msg, sizeof(mcm_mobileap_unsol_event_ind_msg_v01));
        }
        break;

        case QCMAP_CM_EVENT_DISABLED:
        if((qcmap_cp->qcmap_op_ind_pending) &&
           (qcmap_cp->qcmap_op_ind_msg_id == MCM_MOBILEAP_DISABLE_RESP_V01))
        {
          mobileap_disable_resp_msg.resp.result  = MCM_RESULT_SUCCESS_V01;
          mobileap_disable_resp_msg.resp.error= MCM_SUCCESS_V01;
          qmi_csi_send_resp(qcmap_cp->qcmap_op_req_handle, qcmap_cp->qcmap_op_ind_msg_id,
                            &mobileap_disable_resp_msg, sizeof(mcm_mobileap_disable_resp_msg_v01));
          qcmap_cp->qcmap_op_ind_pending = FALSE;
          qcmap_cp->qcmap_op_ind_msg_id = -1;
        }
        else if((qcmap_cp->event_mask) & (MCM_MOBILEAP_DISABLED_EVENT_MASK))
        {
          ind_msg.event_id = MCM_MOBILEAP_DISABLED_EV_V01;
          LOG_MSG_INFO1("Sending Disabled Indication 0x%x", ind_msg.event_id,0,0);
          rc = qmi_csi_send_ind(qcmap_cp->clnt, MCM_MOBILEAP_UNSOL_EVENT_IND_V01,
                                &ind_msg, sizeof(mcm_mobileap_unsol_event_ind_msg_v01));
          qcmap_cp->qcmap_op_ind_pending = FALSE;
        }
        if(sigterm_received)
        {
          /*Close Client sockets*/
          close(qcmap_cb_sockfd);
          close(intr_clt_cb_sockfd);

          /*Close Server sockets*/
          close(svc_cb_sockfd);
          close(intr_svc_cb_sockfd);

          LOG_MSG_INFO1("Server exiting because SIGTERM was sent to it",0,0,0);

          qmi_csi_unregister(mcm_mobileap_svc_state.service_handle);
          exit(0);
        }
        break;

        case QCMAP_CM_EVENT_LAN_CONNECTING:
        if((qcmap_cp->event_mask) & (MCM_MOBILEAP_LAN_CONNECTING_EVENT_MASK))
        {
          ind_msg.event_id = MCM_MOBILEAP_LAN_CONNECTING_EV_V01;
          LOG_MSG_INFO1("Sending LAN Connecting Indication 0x%x", ind_msg.event_id,0,0);
          rc = qmi_csi_send_ind(qcmap_cp->clnt, MCM_MOBILEAP_UNSOL_EVENT_IND_V01,
                                &ind_msg, sizeof(mcm_mobileap_unsol_event_ind_msg_v01));
        }
        break;

        case QCMAP_CM_EVENT_LAN_CONNECTING_FAIL:
        if((qcmap_cp->event_mask) & (MCM_MOBILEAP_LAN_CONNECTING_FAIL_EVENT_MASK))
        {
          ind_msg.event_id = MCM_MOBILEAP_LAN_CONNECTING_FAIL_EV_V01;
          LOG_MSG_INFO1("Sending LAN Connecting Fail Indication 0x%x", ind_msg.event_id,0,0);
          rc = qmi_csi_send_ind(qcmap_cp->clnt, MCM_MOBILEAP_UNSOL_EVENT_IND_V01,
                                &ind_msg, sizeof(mcm_mobileap_unsol_event_ind_msg_v01));
        }
        break;

        case QCMAP_CM_EVENT_LAN_IPv6_CONNECTING_FAIL:
        if((qcmap_cp->event_mask) & (MCM_MOBILEAP_LAN_IPv6_CONNECTING_FAIL_EVENT_MASK))
        {
          ind_msg.event_id = MCM_MOBILEAP_LAN_IPv6_CONNECTING_FAIL_EV_V01;
          LOG_MSG_INFO1("Sending LAN IPv6 Connecting Fail Indication 0x%x", ind_msg.event_id,0,0);
          rc = qmi_csi_send_ind(qcmap_cp->clnt, MCM_MOBILEAP_UNSOL_EVENT_IND_V01,
                                &ind_msg, sizeof(mcm_mobileap_unsol_event_ind_msg_v01));
        }
        break;

        case QCMAP_CM_EVENT_LAN_CONNECTED:
        if((qcmap_cp->event_mask) & (MCM_MOBILEAP_LAN_CONNECTED_EVENT_MASK))
        {
          ind_msg.event_id = MCM_MOBILEAP_LAN_CONNECTED_EV_V01;
          LOG_MSG_INFO1("Sending LAN Connected Indication 0x%x", ind_msg.event_id,0,0);
          rc = qmi_csi_send_ind(qcmap_cp->clnt, MCM_MOBILEAP_UNSOL_EVENT_IND_V01,
                                &ind_msg, sizeof(mcm_mobileap_unsol_event_ind_msg_v01));
        }
        break;

        case QCMAP_CM_EVENT_LAN_IPv6_CONNECTED:
         if((qcmap_cp->event_mask) & (MCM_MOBILEAP_LAN_IPv6_CONNECTED_EVENT_MASK))
        {
          ind_msg.event_id = MCM_MOBILEAP_LAN_IPv6_CONNECTED_EV_V01;
          LOG_MSG_INFO1("Sending LAN IPv6 Connected Indication 0x%x", ind_msg.event_id,0,0);
          rc = qmi_csi_send_ind(qcmap_cp->clnt, MCM_MOBILEAP_UNSOL_EVENT_IND_V01,
                                &ind_msg, sizeof(mcm_mobileap_unsol_event_ind_msg_v01));
        }
        break;

        case QCMAP_CM_EVENT_LAN_DISCONNECTED:

        if((qcmap_cp->event_mask) & (MCM_MOBILEAP_LAN_DISCONNECTED_EVENT_MASK))
        {
          ind_msg.event_id = MCM_MOBILEAP_LAN_DISCONNECTED_EV_V01;
          LOG_MSG_INFO1("Sending LAN Disconnected Indication 0x%x", ind_msg.event_id,0,0);
          rc = qmi_csi_send_ind(qcmap_cp->clnt, MCM_MOBILEAP_UNSOL_EVENT_IND_V01,
                                &ind_msg, sizeof(mcm_mobileap_unsol_event_ind_msg_v01));
        }
        break;

        case QCMAP_CM_EVENT_LAN_IPv6_DISCONNECTED:

        if((qcmap_cp->event_mask) & (MCM_MOBILEAP_LAN_IPv6_DISCONNECTED_EVENT_MASK))
        {
          ind_msg.event_id = MCM_MOBILEAP_LAN_IPv6_DISCONNECTED_EV_V01;
          LOG_MSG_INFO1("Sending LAN IPv6 Disconnected Indication 0x%x", ind_msg.event_id,0,0);
          rc = qmi_csi_send_ind(qcmap_cp->clnt, MCM_MOBILEAP_UNSOL_EVENT_IND_V01,
                                &ind_msg, sizeof(mcm_mobileap_unsol_event_ind_msg_v01));
        }
        break;

        case QCMAP_CM_EVENT_WAN_CONNECTING:
        if((qcmap_cp->event_mask) & (MCM_MOBILEAP_WAN_CONNECTING_EVENT_MASK))
        {
          ind_msg.event_id = MCM_MOBILEAP_WAN_CONNECTING_EV_V01;
          LOG_MSG_INFO1("Sending WAN Connecting Indication 0x%x", ind_msg.event_id,0,0);
          rc = qmi_csi_send_ind(qcmap_cp->clnt, MCM_MOBILEAP_UNSOL_EVENT_IND_V01,
                                &ind_msg, sizeof(mcm_mobileap_unsol_event_ind_msg_v01));
        }
        break;

        case QCMAP_CM_EVENT_WAN_CONNECTING_FAIL:
        if ((qcmap_cp->wwan_op_ind_pending) &&
            (qcmap_cp->wwan_op_ind_msg_id == MCM_MOBILEAP_BRING_UP_WWAN_RESP_V01))
        {
          bring_up_wwan_resp_msg.resp.result  = MCM_RESULT_FAILURE_V01;
          bring_up_wwan_resp_msg.resp.error= MCM_ERROR_CALL_FAILED_V01;
          qmi_csi_send_resp(qcmap_cp->wwan_op_req_handle, qcmap_cp->wwan_op_ind_msg_id,
                            &bring_up_wwan_resp_msg,
                            sizeof(mcm_mobileap_bring_up_wwan_resp_msg_v01));

          qcmap_cp->wwan_op_ind_pending = FALSE;
          qcmap_cp->wwan_op_ind_msg_id = -1;
        }
        else if((qcmap_cp->event_mask) & (MCM_MOBILEAP_WAN_CONNECTING_FAIL_EVENT_MASK))
        {
          ind_msg.event_id = MCM_MOBILEAP_WAN_CONNECTING_FAIL_EV_V01;
          LOG_MSG_INFO1("Sending WAN Connecting Fail Indication 0x%x", ind_msg.event_id,0,0);
          rc = qmi_csi_send_ind(qcmap_cp->clnt, MCM_MOBILEAP_UNSOL_EVENT_IND_V01,
                                &ind_msg, sizeof(mcm_mobileap_unsol_event_ind_msg_v01));
          qcmap_cp->wwan_op_ind_pending = FALSE;
        }
        break;

        case QCMAP_CM_EVENT_WAN_IPv6_CONNECTING_FAIL:
        if ((qcmap_cp->wwan_op_ind_pending) &&
            (qcmap_cp->wwan_op_ind_msg_id == MCM_MOBILEAP_BRING_UP_WWAN_RESP_V01))
        {
          bring_up_wwan_resp_msg.resp.result  = MCM_RESULT_FAILURE_V01;
          bring_up_wwan_resp_msg.resp.error= MCM_ERROR_CALL_FAILED_V01;
          qmi_csi_send_resp(qcmap_cp->wwan_op_req_handle, qcmap_cp->wwan_op_ind_msg_id,
                            &bring_up_wwan_resp_msg,
                            sizeof(mcm_mobileap_bring_up_wwan_resp_msg_v01));
          qcmap_cp->wwan_op_ind_pending = FALSE;
          qcmap_cp->wwan_op_ind_msg_id = -1;
        }
        else if((qcmap_cp->event_mask) & (MCM_MOBILEAP_WAN_IPv6_CONNECTING_FAIL_EVENT_MASK))
        {
          ind_msg.event_id = MCM_MOBILEAP_WAN_IPv6_CONNECTING_FAIL_EV_V01;
          LOG_MSG_INFO1("Sending WAN IPv6 Connecting Fail Indication 0x%x", ind_msg.event_id,0,0);
          rc = qmi_csi_send_ind(qcmap_cp->clnt, MCM_MOBILEAP_UNSOL_EVENT_IND_V01,
                                &ind_msg, sizeof(mcm_mobileap_unsol_event_ind_msg_v01));
          qcmap_cp->wwan_op_ind_pending = FALSE;
        }
        break;

        case QCMAP_CM_EVENT_WAN_CONNECTED:
        if ((qcmap_cp->wwan_op_ind_pending) &&
            (qcmap_cp->wwan_op_ind_msg_id == MCM_MOBILEAP_BRING_UP_WWAN_RESP_V01))
        {
          bring_up_wwan_resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;
          bring_up_wwan_resp_msg.resp.error = MCM_SUCCESS_V01;
          qmi_csi_send_resp(qcmap_cp->wwan_op_req_handle, qcmap_cp->wwan_op_ind_msg_id,
                            &bring_up_wwan_resp_msg,
                            sizeof(mcm_mobileap_bring_up_wwan_resp_msg_v01));
          qcmap_cp->wwan_op_ind_pending = FALSE;
          qcmap_cp->wwan_op_ind_msg_id = -1;
        }
        else if((qcmap_cp->event_mask) & (MCM_MOBILEAP_WAN_CONNECTED_EVENT_MASK))
        {
          ind_msg.event_id = MCM_MOBILEAP_WAN_CONNECTED_EV_V01;
          LOG_MSG_INFO1("Sending WAN Connected Indication 0x%x", ind_msg.event_id,0,0);
          rc = qmi_csi_send_ind(qcmap_cp->clnt, MCM_MOBILEAP_UNSOL_EVENT_IND_V01,
                                &ind_msg, sizeof(mcm_mobileap_unsol_event_ind_msg_v01));
          qcmap_cp->wwan_op_ind_pending = FALSE;
        }
        break;

        case QCMAP_CM_EVENT_WAN_IPv6_CONNECTED:
        if ((qcmap_cp->wwan_op_ind_pending) &&
            (qcmap_cp->wwan_op_ind_msg_id == MCM_MOBILEAP_BRING_UP_WWAN_RESP_V01))
        {
          bring_up_wwan_resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;
          bring_up_wwan_resp_msg.resp.error = MCM_SUCCESS_V01;
          qmi_csi_send_resp(qcmap_cp->wwan_op_req_handle, qcmap_cp->wwan_op_ind_msg_id,
                            &bring_up_wwan_resp_msg,
                            sizeof(mcm_mobileap_bring_up_wwan_resp_msg_v01));
          qcmap_cp->wwan_op_ind_pending = FALSE;
          qcmap_cp->wwan_op_ind_msg_id = -1;
        }
        else if((qcmap_cp->event_mask) & (MCM_MOBILEAP_WAN_IPv6_CONNECTED_EVENT_MASK))
        {
          ind_msg.event_id = MCM_MOBILEAP_WAN_IPv6_CONNECTED_EV_V01;
          LOG_MSG_INFO1("Sending WAN IPv6 Connected Indication 0x%x", ind_msg.event_id,0,0);
          rc = qmi_csi_send_ind(qcmap_cp->clnt, MCM_MOBILEAP_UNSOL_EVENT_IND_V01,
                                &ind_msg, sizeof(mcm_mobileap_unsol_event_ind_msg_v01));
          qcmap_cp->wwan_op_ind_pending = FALSE;
        }
        break;

        case QCMAP_CM_EVENT_WAN_DISCONNECTED:
        if ((qcmap_cp->wwan_op_ind_pending) &&
            (qcmap_cp->wwan_op_ind_msg_id == MCM_MOBILEAP_TEAR_DOWN_WWAN_RESP_V01))
        {
          tear_down_wwan_resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;
          tear_down_wwan_resp_msg.resp.error = MCM_SUCCESS_V01;
          qmi_csi_send_resp(qcmap_cp->wwan_op_req_handle, qcmap_cp->wwan_op_ind_msg_id,
                            &tear_down_wwan_resp_msg,
                            sizeof(mcm_mobileap_tear_down_wwan_resp_msg_v01));
          qcmap_cp->wwan_op_ind_pending = FALSE;
          qcmap_cp->wwan_op_ind_msg_id = -1;
        }
        else if(change_nat_type_requested && (qcmap_cp->nat_op_ind_pending))
        {
          change_nat_type_resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;
          change_nat_type_resp_msg.resp.error = MCM_SUCCESS_V01;
          LOG_MSG_INFO1("Got Disconnect Backhaul Event. Change Nat Type Completed.",0,0,0);
          qmi_csi_send_resp(qcmap_cp->nat_op_req_handle, qcmap_cp->nat_op_ind_msg_id,
                            &change_nat_type_resp_msg,
                            sizeof(mcm_mobileap_set_nat_type_resp_msg_v01));
          qcmap_cp->nat_op_ind_pending = FALSE;
          qcmap_cp->nat_op_ind_msg_id = -1;
        }
        else if((qcmap_cp->event_mask) & (MCM_MOBILEAP_WAN_DISCONNECTED_EVENT_MASK))
        {
          ind_msg.event_id = MCM_MOBILEAP_WAN_DISCONNECTED_EV_V01;
          LOG_MSG_INFO1("Sending WAN Disconnected Indication 0x%x", ind_msg.event_id,0,0);
          rc = qmi_csi_send_ind(qcmap_cp->clnt, MCM_MOBILEAP_UNSOL_EVENT_IND_V01,
                                &ind_msg, sizeof(mcm_mobileap_unsol_event_ind_msg_v01));
          qcmap_cp->wwan_op_ind_pending = FALSE;
        }
        break;

        case QCMAP_CM_EVENT_WAN_IPv6_DISCONNECTED:
        if ((qcmap_cp->wwan_op_ind_pending) &&
            (qcmap_cp->wwan_op_ind_msg_id == MCM_MOBILEAP_TEAR_DOWN_WWAN_RESP_V01))
        {
          tear_down_wwan_resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;
          tear_down_wwan_resp_msg.resp.error = MCM_SUCCESS_V01;
          qmi_csi_send_resp(qcmap_cp->wwan_op_req_handle, qcmap_cp->wwan_op_ind_msg_id,
                            &tear_down_wwan_resp_msg,
                            sizeof(mcm_mobileap_tear_down_wwan_resp_msg_v01));
          qcmap_cp->wwan_op_ind_pending = FALSE;
          qcmap_cp->wwan_op_ind_msg_id = -1;
        }
        else if((qcmap_cp->event_mask) & (MCM_MOBILEAP_WAN_IPv6_DISCONNECTED_EVENT_MASK))
        {
          ind_msg.event_id = MCM_MOBILEAP_WAN_IPv6_DISCONNECTED_EV_V01;
          LOG_MSG_INFO1("Sending WAN IPv6 Disconnected Indication 0x%x", ind_msg.event_id,0,0);
          rc = qmi_csi_send_ind(qcmap_cp->clnt, MCM_MOBILEAP_UNSOL_EVENT_IND_V01,
                                &ind_msg, sizeof(mcm_mobileap_unsol_event_ind_msg_v01));
          qcmap_cp->wwan_op_ind_pending = FALSE;
        }
        break;

      }

    }
 }

}

#endif /* TARGET_IS_9615 */

/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_INIT()

  DESCRIPTION
    Register the MCM MOBILEAP service with QMUX for all applicable QMI links.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int mcm_mobileap_svc_init
(
  void
)
{
  qmi_csi_error         rc;
  int len;
  int val, rval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1("MCM MobileAP service init %d", mcm_mobileap_svc_inited,0,0);

  if (!mcm_mobileap_svc_inited)
    mcm_mobileap_svc_inited = TRUE;
  else
  {
    LOG_MSG_INFO1("MCM MobileAP service already initialized",0,0,0);
    return QCMAP_CM_ENOERROR;
  }

  /* Create qcmap -> mcm_mobileap_svc client socket */
  if (create_socket(&qcmap_cb_sockfd) != QCMAP_CM_ENOERROR)
  {
    LOG_MSG_ERROR("mcm_mobileap_svc_init::error creating qcmap_cb_sockfd socket", 0, 0, 0);
  }

  /*Create interrupt cb client socket*/
   if (create_socket(&intr_clt_cb_sockfd) != QCMAP_CM_ENOERROR)
  {
    LOG_MSG_ERROR("mcm_mobileap_svc_init::error creating intr_clt_cb_sockfd socket", 0, 0, 0);
  }

  /*-------------------------------------------------------------------------
    Register MCM MOBILEAP service with QCSI
  -------------------------------------------------------------------------*/
  rc = qmi_csi_register
       (
       mcm_mobileap_get_service_object_v01(),
       mcm_mobileap_svc_hdlr_client_connect_cb,
       mcm_mobileap_svc_hdlr_client_disconnect_cb,
       mcm_mobileap_svc_hdlr_client_req_cb,
       &mcm_mobileap_svc_state,
       &os_params,
       &mcm_mobileap_svc_state.service_handle
       );

  if(rc != QMI_CSI_NO_ERR)
  {
    LOG_MSG_ERROR("Unable to register MCM MobileAP service! Error %d", rc,0,0);
    ds_assert(0);
    return QCMAP_CM_ERROR;
  }

  LOG_MSG_INFO1("MCM MobileAP service registered with QCSI Framework",0,0,0);

  /* Create mcm svc  -> qcmap cb socket */
  rval = create_svc_cb_socket();
  if (rval != QCMAP_CM_ENOERROR)
  {
    LOG_MSG_ERROR("Unable to create qcmap cb socket!", 0,0,0);
    return QCMAP_CM_ERROR;
  }

  rval = create_intr_svc_cb_socket();
  if (rval != QCMAP_CM_ENOERROR)
  {
    LOG_MSG_ERROR("Unable to create interrupt cb socket!", 0,0,0);
    return QCMAP_CM_ERROR;
  }

/* For 9x25 targets we'll be doing Diag Init in QCMAP_Client library. */
#ifdef TARGET_IS_9615
  /*Initialize the Diag for QXDM logs*/
  if (TRUE != Diag_LSM_Init(NULL))
  {
     MCM_MOBILEAP_SVC_LOG("Diag_LSM_Init failed !!");
  }
#endif /* TARGET_IS_9615 */
  LOG_MSG_INFO1("Successfully completed initialization",0,0,0);
  return QCMAP_CM_ENOERROR;
} /* mcm_mobileap_svc_init */

#ifndef TARGET_IS_9615

/*==================================================================
FUNCTION mcm_mobileap_exit_func

DESCRIPTION
Used by ril framework to make server exit gracefully

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
====================================================================*/

void mcm_mobileap_exit_func()
{
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;

  printf("\n mcm_mobileap_exit_func ENTER",0,0,0);

  if (QcMapClient)
  {
    exit_func_called = TRUE;
    if (!QcMapClient->DisableMobileAP(&qmi_err_num))
    {
      close(qcmap_cb_sockfd);
      close(intr_clt_cb_sockfd);

      /*Close Server sockets*/
      close(svc_cb_sockfd);
      close(intr_svc_cb_sockfd);

      LOG_MSG_INFO1("Server exiting because EXIT FUNCTION was called",
                    0,0,0);
      qmi_csi_unregister(mcm_mobileap_svc_state.service_handle);
      exit(0);
    }
  }

  printf("\n mcm_mobileap_exit_func EXIT",0,0,0);
}

mcm_ipc_srv_mgr_exit_func mobileap_exit;
#endif

int main(int argc, char **argv)
{
  fd_set master_fd_set;
  struct sockaddr_storage their_addr,their_addr_2;
  char buf[MAX_BUF_LEN];
  int nbytes=0, count=0;
  socklen_t addr_len, addr_len_2;
  addr_len = addr_len_2 = sizeof(struct sockaddr_storage);
  struct timeval *timeout=NULL;
  int tmp_handle = 0;
  int signal_val = 0;
  int ret_value;
  boolean bootup_init = FALSE;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
  memset(&their_addr, 0, sizeof(struct sockaddr_storage));
  memset(&their_addr_2, 0, sizeof(struct sockaddr_storage));
  memset(buf, 0, MAX_BUF_LEN);
#ifdef TARGET_IS_9615
  mcm_mobileap_event_buffer_t *event_buffer = NULL;
#else
  mcm_mobileap_qcmap_client_event_buffer_t *event_buffer = NULL;
#endif

  mcm_set_service_ready(MCM_MOBILEAP_SERVICE, 0);

#ifdef TARGET_IS_9615
  switch(argc)
  {
    /* If command line parameters were entered, ...*/
    case 2:
      /* ... read the XML file path from argv[1]. */
      LOG_MSG_INFO1("%s XML path %s\n", argv[0], argv[1],0);
      QcMapMgr = new MCM_QCMAP_ConnectionManager(argv[1]);
      break;
    case 3:
      if (0 != strncasecmp(argv[2],"d",1))
      {
        exit(1);
      }
      QcMapMgr = new MCM_QCMAP_ConnectionManager(argv[1]);
      LOG_MSG_INFO1("Deamon mode %s %s \n",argv[0], argv[1],0);
      break;
    default:
      /* Else, use default paramters to configure the Mobile AP. */
      QcMapMgr = new MCM_QCMAP_ConnectionManager(NULL);
      break;
  }

  if(QcMapMgr->IoEMode())
  {
    system("killall -9 QCMAP_ConnectionManager");
  }
  else
  {
    LOG_MSG_INFO1("MCM_MOBILEAP_ConnectionManager exiting because IoE Mode is NOT SET",0,0,0);
    return 0;
  }

  /* initilize DSI library	*/
  if (DSI_SUCCESS != dsi_init(DSI_MODE_GENERAL))
  {
    LOG_MSG_ERROR("dsi_init failed try Again!!",0,0,0);
  }

  /* Initialize qcmap_cmdq module */
  qcmap_cmdq_init();
#else /* TARGET_IS_9615 */

  /* Create a QCMAP Client object for 9x25. */
  QcMapClient = new QCMAP_Client( mcm_mobileap_qcmap_client_callback );

  if (QcMapClient->qmi_qcmap_msgr_handle == 0)
  {
    printf("\nCouldn't setup QcMapClient..exiting");
    sighandler(SIGTERM);
    exit(1);
  }

  mobileap_exit.srv_id = MCM_MOBILEAP_V01;
  mobileap_exit.srv_exit_func = mcm_mobileap_exit_func;
  mcm_ipc_srv_mgr_start(&mobileap_exit);

#endif /* !(TARGET_IS_9615) */

  /* Initialize MobileAP Service */
  mcm_mobileap_svc_init();

  /* Register the sighandlers, so the app may be shutdown with a
     kill command.*/
  signal(SIGTERM, sighandler);
  signal(SIGINT, sighandler);

#ifdef TARGET_IS_9615
  signal(SIGUSR1, sighandler);
  signal(SIGUSR2, sighandler);
  if (QcMapMgr->TetheringOnly())
  {
    timeout = calloc(1, sizeof(struct timeval));
    if(timeout !=NULL)
    {
      timeout->tv_sec  = MOBILEAP_INIT_TIMEOUT_SEC ;
      timeout->tv_usec = MOBILEAP_INIT_TIMEOUT_USEC;
    }
    bootup_init = TRUE;
  }
#endif /* TARGET_IS_9615 */

  while(mcm_ipc_get_service_is_ready() == 0) {
    usleep(100000); 
  }
  mcm_set_service_ready(MCM_MOBILEAP_SERVICE, 1);
  while(1)
  {
    master_fd_set = os_params.fds;
    ret_value = select(os_params.max_fd+1, &master_fd_set, NULL, NULL, timeout);
    if (ret_value < 0)
    {
      LOG_MSG_ERROR("Error in select, errno:%d", errno, 0, 0);
      if( errno == EINTR )
       continue;
      else
       return -1;
    }

#ifdef TARGET_IS_9615
    /* If there is a timer expire then we check for bootup enable configs */
    if(ret_value == 0 && bootup_init )
    {
      if (QcMapMgr->Enable(&tmp_handle, &mcm_mobileap_svc_handle,&mcm_err_num))
      {
        LOG_MSG_INFO1("Enable Mobile AP on Bootup succeeds.\n",0,0,0);
        mcm_mobileap_svc_handle.handle = tmp_handle;
      }
      else
      {
        LOG_MSG_ERROR("Enable Mobile AP On Bootup FAILS!!! errno:%d.\n",mcm_err_num,0,0);
      }
      if( timeout != NULL )
      {
        free( timeout );
      }
      timeout = NULL;
      bootup_init = FALSE;
      continue;
    }
#endif /* TARGET_IS_9615 */

    for (int i = 0; i <= os_params.max_fd; i++)
    {
      if (FD_ISSET(i, &master_fd_set))
      {
        LOG_MSG_INFO1("i  %d    %d",i,svc_cb_sockfd,0);
        if ( i == svc_cb_sockfd )
        {
          LOG_MSG_INFO1("Received Message from QCMAP Callaback",0,0,0);
          if ( (nbytes = recvfrom(i, buf, MAX_BUF_LEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len)) <= 0 )
          {
            if ( nbytes == 0 )
            {
              LOG_MSG_INFO1("Completed full recv from QCMAP callback", 0, 0, 0);
            }
            else
            {
              LOG_MSG_ERROR("recvfrom returned error, errno:%d", errno, 0, 0);
            }
          }
          else
          {
#ifdef TARGET_IS_9615
            event_buffer = (mcm_mobileap_event_buffer_t *)buf;
            if (event_buffer != NULL)
            {
              mcm_mobileap_evnt_hndl_fn( event_buffer->qcmap_handle,
                                        event_buffer->evt, event_buffer->user_data);
            }
#else /* TARGET_IS_9615 */
            event_buffer = (mcm_mobileap_qcmap_client_event_buffer_t *)buf;
            if (event_buffer != NULL)
            {
              mcm_mobileap_qcmap_client_ind(event_buffer);
            }
#endif /* !(TARGET_IS_9615) */
          }
        }
        else if ( i == intr_svc_cb_sockfd )
        {
           LOG_MSG_INFO1("Received Message from interrupt Callaback",0,0,0);
           if ( (nbytes = recvfrom(i, &signal_val, sizeof(int) , 0,
                                    (struct sockaddr *)&their_addr_2,
                                    &addr_len_2)) <= 0 )
           {
             if ( nbytes == 0 )
             {
               LOG_MSG_INFO1("Completed full recv from mcm callback", 0, 0, 0);
             }
             else
             {
               LOG_MSG_ERROR("recvfrom returned error, errno:%d", errno, 0, 0);
             }
           }
           else
           {
             if (signal_val != 0)
             {
               LOG_MSG_INFO1("Calling mcm_mobileap_svc_handle_intr_cb",0,0,0);
               mcm_mobileap_svc_handle_intr_cb(signal_val);
             }
           }
        }
        else
        {
          LOG_MSG_INFO1("Received QMI Message",0,0,0);
          qmi_csi_handle_event(mcm_mobileap_svc_state.service_handle, &os_params);
        }
      }
    }
  }
  qmi_csi_unregister(mcm_mobileap_svc_state.service_handle);
  mcm_set_service_ready(MCM_MOBILEAP_SERVICE, 0);
  LOG_MSG_INFO1("MCM MobileAP service exiting", 0, 0, 0);
  return 0;
}

static qmi_csi_cb_error mcm_mobileap_svc_hdlr_client_connect_cb
(
  qmi_client_handle          client_handle,
  void                      *service_handle,
  void                     **connection_handle
)
{
  mcm_mobileap_svc_client_info_type    *clnt_info_ptr;
  mcm_mobileap_svc_state_info_type     *qcmap_svc_ptr;
  unsigned int                     index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(connection_handle != NULL);
  ds_assert(service_handle != NULL);

  qcmap_svc_ptr = (mcm_mobileap_svc_state_info_type *) service_handle;

  for (index=0; index < MCM_MOBILEAP_SVC_MAX_CLIENT_HANDLES; index++)
  {
    if (NULL == qcmap_svc_ptr->client_handle_list[index])
    {
      LOG_MSG_INFO1("mcm_mobileap_svc_hdlr_client_connect_cb: index=%d",index,0,0);
      clnt_info_ptr = (mcm_mobileap_svc_client_info_type *)malloc(
                        sizeof(mcm_mobileap_svc_client_info_type));
      if(!clnt_info_ptr)
      {
        LOG_MSG_INFO1("mcm_mobileap_svc_hdlr_client_connect_cb: Out of mem",0,0,0);
        return QMI_CSI_CB_CONN_REFUSED;
      }
      else
        break;
    }
  }

  if (index == MCM_MOBILEAP_SVC_MAX_CLIENT_HANDLES)
  {
    LOG_MSG_ERROR("Client handle slots exhausted", 0, 0, 0);
    return QMI_CSI_CB_NO_MEM;
  }

  memset(clnt_info_ptr, 0, sizeof(mcm_mobileap_svc_client_info_type));
  clnt_info_ptr->clnt = client_handle;
  qcmap_svc_ptr->num_clients++;
  qcmap_svc_ptr->client_handle_list[index] = *connection_handle = clnt_info_ptr;

  LOG_MSG_INFO1("mcm_mobileap_svc_hdlr_client_connect_cb: Alloc client 0x%p",
                qcmap_svc_ptr->client_handle_list[index],0,0);

  return QMI_CSI_CB_NO_ERR;
} /* mcm_mobileap_svc_hdlr_client_connect_cb() */

static void mcm_mobileap_svc_hdlr_client_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_handle
)
{
  mcm_mobileap_svc_state_info_type       *qcmap_svc_ptr;
  mcm_mobileap_svc_handle_type    *map_handle;
  unsigned int                         client_index;
  unsigned int                         index;
  int                                  ret_val, err_num;
  short                                ps_errno;
  boolean                              last_client = TRUE;
  mcm_mobileap_svc_client_info_type      *clnt_info;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
  int                client_count = 1;
#ifndef TARGET_IS_9615
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
#endif /* !(TARGET_IS_9615) */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(service_handle != NULL);


  qcmap_svc_ptr = (mcm_mobileap_svc_state_info_type *)service_handle;

  for (client_index=0;client_index < MCM_MOBILEAP_SVC_MAX_CLIENT_HANDLES; client_index++)
  {
    if (qcmap_svc_ptr->client_handle_list[client_index] == connection_handle)
      break;
  }
  if (client_index == MCM_MOBILEAP_SVC_MAX_CLIENT_HANDLES)
  {
    LOG_MSG_ERROR("mcm_mobileap_svc_hdlr_client_disconnect_cb: Invalid Handle %p",
                  connection_handle,0,0);
    return;
  }

  clnt_info = (mcm_mobileap_svc_client_info_type *)qcmap_svc_ptr->client_handle_list[client_index];

  if(clnt_info->client_enabled)
  {
    if(qcmap_svc_ptr->client_ref_count > client_count)
    {
      qcmap_svc_ptr->client_ref_count--;
      clnt_info->client_enabled = FALSE;
    }
    else if(qcmap_svc_ptr->client_ref_count == client_count)
    {
#ifdef TARGET_IS_9615
      ret_val = QcMapMgr->Disable(&mcm_err_num);
#else /* TARGET_IS_9615 */
      ret_val= QcMapClient->DisableMobileAP(&qmi_err_num);
      mcm_err_num = qmi_to_mcm_error[qmi_err_num];
#endif /* !(TARGET_IS_9615) */
      if ( ret_val)
      {
        qcmap_svc_ptr->client_ref_count -= client_count;
        clnt_info->client_enabled = FALSE;
        LOG_MSG_INFO1("MobileAP Disable in progress.",0,0,0);
      }
      else
      {
        LOG_MSG_ERROR("MobileAP Disable request fails.",0,0,0);
      }
    }
  }

  qcmap_svc_ptr->num_clients--;
  LOG_MSG_INFO1("mcm_mobileap_svc_hdlr_client_disconnect_cb: Releasing client %p, num_clients: %d",
                qcmap_svc_ptr->client_handle_list[client_index], qcmap_svc_ptr->num_clients,0);
  free(qcmap_svc_ptr->client_handle_list[client_index]);
  qcmap_svc_ptr->client_handle_list[client_index] = NULL;


  for (index=0; (index < MCM_MOBILEAP_SVC_MAX_CLIENT_HANDLES); index++)
  {
    if (qcmap_svc_ptr->client_handle_list[index] != NULL)
    {
      last_client=FALSE;
      break;
    }
  }

  LOG_MSG_INFO1("mcm_mobileap_svc_hdlr_client_disconnect_cb: last_client: %d", last_client,0,0);
  if (last_client)
  {
    LOG_MSG_INFO1("mcm_mobileap_svc_hdlr_client_disconnect_cb: Last Client True!!",0,0,0);
    /* Terminate SoftAP handle, if it still exists, since the last client has disconnected */
    map_handle = &mcm_mobileap_svc_handle;
    if (map_handle->handle != 0)
    {
#ifdef TARGET_IS_9615
      if ((map_handle->cb_ptr) && (QcMapMgr->qcmap_tear_down_in_progress == false))
      {
        LOG_MSG_INFO1("mcm_mobileap_svc_hdlr_client_disconnect_cb: Freeing cbPtr!!",0,0,0);
        free(map_handle->cb_ptr);
        map_handle->cb_ptr = NULL;
      }
#endif /* TARGET_IS_9615 */
      map_handle->handle = 0;
    }
  }
  return;
} /* mcm_mobileap_svc_hdlr_client_disconnect_cb() */


static qmi_csi_cb_error mcm_mobileap_svc_hdlr_client_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_handle
)
{
  qmi_csi_cb_error                rc;
  mcm_mobileap_svc_client_info_type   *clnt_info;
  mcm_mobileap_svc_state_info_type    *qcmap_svc_ptr;
  unsigned int                    client_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(connection_handle != NULL);
  ds_assert(service_handle != NULL);

  rc = QMI_CSI_CB_INTERNAL_ERR;
  clnt_info = (mcm_mobileap_svc_client_info_type*)connection_handle;
  qcmap_svc_ptr = (mcm_mobileap_svc_state_info_type*) service_handle;

  LOG_MSG_INFO1("\nEntered mcm_mobileap_svc_hdlr_client_req_cb",0,0,0);

  for (client_index=0;client_index < MCM_MOBILEAP_SVC_MAX_CLIENT_HANDLES; client_index++)
  {
    LOG_MSG_INFO1("\nLooking for client %p...", clnt_info,0,0);
    if(qcmap_svc_ptr->client_handle_list[client_index] == clnt_info)
    {
      LOG_MSG_INFO1("Found",0,0,0);
      break;
    }
  }
  if (client_index == MCM_MOBILEAP_SVC_MAX_CLIENT_HANDLES)
  {
      LOG_MSG_INFO1("mcm_mobileap_svc_hdlr_client_req_cb: Invalid clnt handle %p",
                    clnt_info,0,0);
      return rc;
  }

  if((msg_id - MCM_MOBILEAP_ENABLE_REQ_V01) < (sizeof(req_handle_table) / sizeof(*req_handle_table)))
  {
    if(req_handle_table[msg_id - MCM_MOBILEAP_ENABLE_REQ_V01])
    {
      rc = req_handle_table[msg_id - MCM_MOBILEAP_ENABLE_REQ_V01] (clnt_info, req_handle, msg_id,
                                             req_c_struct, req_c_struct_len,
                                             service_handle);
    }
    else
    {
      LOG_MSG_INFO1("mcm_mobileap_svc_hdlr_client_req_cb: NULL message ID handler: %d",
                    (msg_id - MCM_MOBILEAP_ENABLE_REQ_V01),0,0);
    }
  }
  else
  {
    LOG_MSG_INFO1("mcm_mobileap_svc_hdlr_client_req_cb: Invalid message ID: %d", (msg_id - 0x400),0,0);
  }

  return rc;
} /* mcm_mobileap_svc_hdlr_client_req_cb() */

/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_EVENT_REGISTER()

  DESCRIPTION
    Register for events

  PARAMETERS
    sp:          MCM MobileAP service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    mcm_mobileap_svc_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_event_register
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  mcm_mobileap_svc_client_info_type *qcmap_cp=NULL;
  mcm_mobileap_event_register_req_msg_v01   *req_ptr;
  mcm_mobileap_event_register_resp_msg_v01   resp_msg;
  mcm_mobileap_svc_status_cb_data    *cb_data_ptr=NULL;
  mcm_mobileap_svc_handle_type    *map_handle;
  qmi_csi_error rc;
  int              ret_val;
  unsigned int     index;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
  int              handle;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_cp = clnt_info;
  req_ptr = (mcm_mobileap_event_register_req_msg_v01 *)req_c_struct;
  LOG_MSG_INFO1 ("Client %p registering for unsolicited events",
                 clnt_info, 0, 0);

/*---------------------------------------------------------------------
      Set Event Mask for this client
  ---------------------------------------------------------------------*/

  memset(&resp_msg, 0, sizeof(mcm_mobileap_event_register_resp_msg_v01));

  qcmap_cp->event_mask = MCM_MOBILEAP_NONE_EVENT_MASK;

  if(req_ptr->register_event_disabled_valid)
  {
    if(req_ptr->register_event_disabled)
    {
      qcmap_cp->event_mask = (qcmap_cp->event_mask | MCM_MOBILEAP_DISABLED_EVENT_MASK);
    }
  }

  if(req_ptr->register_event_enabled_valid)
  {
    if(req_ptr->register_event_enabled)
    {
      qcmap_cp->event_mask = (qcmap_cp->event_mask | MCM_MOBILEAP_ENABLED_EVENT_MASK);
    }
  }

  if(req_ptr->register_event_lan_connected_valid)
  {
    if(req_ptr->register_event_lan_connected)
    {
      qcmap_cp->event_mask = (qcmap_cp->event_mask | MCM_MOBILEAP_LAN_CONNECTED_EVENT_MASK);
    }
  }

  if(req_ptr->register_event_lan_connecting_valid)
  {
    if(req_ptr->register_event_lan_connecting)
    {
      qcmap_cp->event_mask = (qcmap_cp->event_mask | MCM_MOBILEAP_LAN_CONNECTING_EVENT_MASK);
    }
  }

  if(req_ptr->register_event_lan_connecting_fail_valid)
  {
    if(req_ptr->register_event_lan_connecting_fail)
    {
      qcmap_cp->event_mask = (qcmap_cp->event_mask | MCM_MOBILEAP_LAN_CONNECTING_FAIL_EVENT_MASK);
    }
  }

  if(req_ptr->register_event_lan_disconnected_valid)
  {
    if(req_ptr->register_event_lan_disconnected)
    {
      qcmap_cp->event_mask = (qcmap_cp->event_mask | MCM_MOBILEAP_LAN_DISCONNECTED_EVENT_MASK);
    }
  }

  if(req_ptr->register_event_lan_ipv6_connected_valid)
  {
    if(req_ptr->register_event_lan_ipv6_connected)
    {
      qcmap_cp->event_mask = (qcmap_cp->event_mask | MCM_MOBILEAP_LAN_IPv6_CONNECTED_EVENT_MASK);
    }
  }

  if(req_ptr->register_event_lan_ipv6_connecting_fail_valid)
  {
    if(req_ptr->register_event_lan_ipv6_connecting_fail)
    {
      qcmap_cp->event_mask = (qcmap_cp->event_mask | MCM_MOBILEAP_LAN_IPv6_CONNECTING_FAIL_EVENT_MASK);
    }
  }

  if(req_ptr->register_event_lan_ipv6_disconnected_valid)
  {
    if(req_ptr->register_event_lan_ipv6_disconnected)
    {
      qcmap_cp->event_mask = (qcmap_cp->event_mask | MCM_MOBILEAP_LAN_IPv6_DISCONNECTED_EVENT_MASK);
    }
  }

  if(req_ptr->register_event_sta_connected_valid)
  {
    if(req_ptr->register_event_sta_connected)
    {
      qcmap_cp->event_mask = (qcmap_cp->event_mask | MCM_MOBILEAP_STA_CONNECTED_EVENT_MASK);
    }
  }

  if(req_ptr->register_event_wan_connected_valid)
  {
    if(req_ptr->register_event_wan_connected)
    {
      qcmap_cp->event_mask = (qcmap_cp->event_mask | MCM_MOBILEAP_WAN_CONNECTED_EVENT_MASK);
    }
  }

  if(req_ptr->register_event_wan_connecting_valid)
  {
    if(req_ptr->register_event_wan_connecting)
    {
     qcmap_cp->event_mask = (qcmap_cp->event_mask | MCM_MOBILEAP_WAN_CONNECTING_EVENT_MASK);
    }
  }

  if(req_ptr->register_event_wan_connecting_fail_valid)
  {
    if(req_ptr->register_event_wan_connecting_fail)
    {
      qcmap_cp->event_mask = (qcmap_cp->event_mask | MCM_MOBILEAP_WAN_CONNECTING_FAIL_EVENT_MASK);
    }
  }
  if(req_ptr->register_event_wan_disconnected_valid)
  {
    if(req_ptr->register_event_wan_disconnected)
    {
      qcmap_cp->event_mask = (qcmap_cp->event_mask | MCM_MOBILEAP_WAN_DISCONNECTED_EVENT_MASK);
    }
  }
  if(req_ptr->register_event_wan_ipv6_connected_valid)
  {
    if(req_ptr->register_event_wan_ipv6_connected)
    {
      qcmap_cp->event_mask = (qcmap_cp->event_mask | MCM_MOBILEAP_WAN_IPv6_CONNECTED_EVENT_MASK);
    }
  }
  if(req_ptr->register_event_wan_ipv6_connecting_fail_valid)
  {
    if(req_ptr->register_event_wan_ipv6_connecting_fail)
    {
      qcmap_cp->event_mask = (qcmap_cp->event_mask | MCM_MOBILEAP_WAN_IPv6_CONNECTING_FAIL_EVENT_MASK);
    }
   }
  if (req_ptr->register_event_wan_ipv6_disconnected_valid)
  {
    if(req_ptr->register_event_wan_ipv6_disconnected)
    {
      qcmap_cp->event_mask = (qcmap_cp->event_mask | MCM_MOBILEAP_WAN_IPv6_DISCONNECTED_EVENT_MASK);
    }
  }

  LOG_MSG_INFO1("Event Register successful!!",0,0,0);
  resp_msg.response.result = MCM_RESULT_SUCCESS_V01;
  resp_msg.response.error = MCM_SUCCESS_V01;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_set_auto_connect_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;

}

/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_ENABLE()

  DESCRIPTION
    Enable SoftAP functionality on modem.

  PARAMETERS
    sp:          MCM MobileAP service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    mcm_mobileap_svc_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_enable
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  mcm_mobileap_svc_state_info_type    *qcmap_sp;
  mcm_mobileap_enable_resp_msg_v01   resp_msg;
  mcm_mobileap_svc_status_cb_data    *cb_data_ptr=NULL;
  qmi_csi_error rc;
  int              ret_val;
  unsigned int     index;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
  int              handle;
  boolean ret;
#ifndef TARGET_IS_9615
  int client_index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  mcm_mobileap_svc_client_info_type *qcmap_cp = NULL;
  mcm_mobileap_svc_state_info_type *qcmap_sp_ind;
  mcm_mobileap_unsol_event_ind_msg_v01 ind_msg;
#endif /* !(TARGET_IS_9615) */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type *)service_handle;

  LOG_MSG_INFO1 ("Client %p enabling Mobile AP", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(mcm_mobileap_enable_resp_msg_v01));

  /*---------------------------------------------------------------------
    Enable SoftAP
  ---------------------------------------------------------------------*/

  if (mcm_mobileap_svc_handle.cb_ptr == NULL)
  {
    cb_data_ptr = (mcm_mobileap_svc_status_cb_data*)malloc(
                         sizeof(mcm_mobileap_svc_status_cb_data));

    if (cb_data_ptr == NULL)
    {
      LOG_MSG_ERROR("Out of mem for WWAN cb data", 0, 0, 0);
      return QMI_CSI_CB_NO_MEM;
    }

    cb_data_ptr->svc_cb = qcmap_sp;
    cb_data_ptr->map_instance = MCM_MOBILEAP_SVC_HANDLE;
    //cb_data_ptr->qcmap_cb = mcm_mobileap_status_ind;

    mcm_mobileap_svc_handle.cb_ptr = cb_data_ptr;

  }

  if(!clnt_info->client_enabled)
  {
    if(qcmap_sp->client_ref_count > 0)
    {
      clnt_info->client_enabled = TRUE;
      qcmap_sp->client_ref_count++;
      index = MCM_MOBILEAP_SVC_HANDLE;
      LOG_MSG_INFO1("Enabled MobileAP inst %d",
                  index,0,0);
      resp_msg.mcm_mobileap_handle_valid = true;
      resp_msg.mcm_mobileap_handle = index;
      qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                        sizeof(mcm_mobileap_enable_resp_msg_v01));
      return QMI_CSI_CB_NO_ERR;
    }
    else if(qcmap_sp->client_ref_count == 0)
    {

#ifdef TARGET_IS_9615
      ret = QcMapMgr->Enable(&handle,
                             &mcm_mobileap_svc_handle,
                             &mcm_err_num);
#else /* TARGET_IS_9615 */
      ret = QcMapClient->EnableMobileAP(&qmi_err_num);
      mcm_err_num = qmi_to_mcm_error[qmi_err_num];
      handle = QcMapClient->mobile_ap_handle;

#endif /* !(TARGET_IS_9615) */
      if ( ret )
      {
        clnt_info->client_enabled = true;
        qcmap_sp->client_ref_count++;
        LOG_MSG_INFO1("MobileAP Enable succeeds.\n",0,0,0);

#ifndef TARGET_IS_9615
        qcmap_sp_ind = mcm_mobileap_svc_handle.cb_ptr->svc_cb;

        /* Send the indications for all the clients which have registered for the events */
        for (client_index = 0;
             client_index < MCM_MOBILEAP_SVC_MAX_CLIENT_HANDLES; client_index++)
        {
          qcmap_cp = (mcm_mobileap_svc_client_info_type *)
                     (qcmap_sp_ind->client_handle_list[client_index]);
          if (qcmap_cp)
          {
            if((qcmap_cp->event_mask) & (MCM_MOBILEAP_ENABLED_EVENT_MASK))
            {
              ind_msg.event_id = MCM_MOBILEAP_ENABLED_EV_V01;
              LOG_MSG_INFO1("Sending MobileAP Enabled Indication 0x%x",
                            ind_msg.event_id, 0, 0);
              rc = qmi_csi_send_ind(qcmap_cp->clnt,
                                  MCM_MOBILEAP_UNSOL_EVENT_IND_V01,
                                  &ind_msg,
                                  sizeof(mcm_mobileap_unsol_event_ind_msg_v01));
            }
          }
        }
#endif /* !(TARGET_IS_9615) */
      }
      else
      {
        LOG_MSG_INFO1("MobileAP Enable fails.\n",0,0,0);
        resp_msg.mcm_mobileap_handle_valid = false;
        resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
        resp_msg.resp.error = mcm_err_num;
        qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                        sizeof(mcm_mobileap_enable_resp_msg_v01));
        return QMI_CSI_CB_NO_ERR;
      }
      LOG_MSG_INFO1("Check Index %d", index, 0, 0);

      mcm_mobileap_svc_handle.handle = handle;
#ifdef TARGET_IS_9615
      if (mcm_err_num == MCM_ERROR_NO_EFFECT_V01)
#else /* TARGET_IS_9615 */
      if (mcm_err_num == MCM_SUCCESS_V01)
#endif /* !(TARGET_IS_9615) */
      {
        index = MCM_MOBILEAP_SVC_HANDLE;
        LOG_MSG_INFO1("Enabled MobileAP inst %d", index, 0, 0);
        resp_msg.mcm_mobileap_handle_valid = true;
        resp_msg.mcm_mobileap_handle = index;
        qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                          sizeof(mcm_mobileap_enable_resp_msg_v01));
      }
      else
      {
        /* Set pending to TRUE to notify client with final resposne
         * once final MobileAP state is received
         *  */
        clnt_info->qcmap_op_ind_pending = TRUE;
        clnt_info->qcmap_op_req_handle = req_handle;
        clnt_info->qcmap_op_ind_msg_id = MCM_MOBILEAP_ENABLE_RESP_V01;
      }
      return QMI_CSI_CB_NO_ERR;
    }
  }
  else
  {
    LOG_MSG_ERROR("The client has already enabled MobileAP", 0, 0, 0);
    resp_msg.mcm_mobileap_handle_valid = false;
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_NO_EFFECT_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_enable_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  return QMI_CSI_CB_NO_ERR;
} /* mcm_mobileap_svc_hdlr_enable() */


/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_DISABLE()

  DESCRIPTION
    Disable SoftAP functionality on modem.

  PARAMETERS
    sp:          MCM MobileAP service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    mcm_mobileap_svc_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_disable
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  mcm_mobileap_svc_state_info_type            *qcmap_sp;
  mcm_mobileap_disable_req_msg_v01   *req_ptr;
  mcm_mobileap_disable_resp_msg_v01   resp_msg;
  mcm_mobileap_unsol_event_ind_msg_v01    ind_msg;
  mcm_mobileap_svc_handle_type         *map_handle;
  unsigned int             index;
  int                client_count = 1;
  qmi_csi_error rc;
  boolean ret;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
#ifndef TARGET_IS_9615
  int client_index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  mcm_mobileap_svc_client_info_type *qcmap_cp = NULL;
  mcm_mobileap_svc_state_info_type *qcmap_sp_ind;
#endif /* !(TARGET_IS_9615) */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type*)service_handle;
  req_ptr = (mcm_mobileap_disable_req_msg_v01*)req_c_struct;
  memset(&resp_msg, 0, sizeof(mcm_mobileap_disable_resp_msg_v01));

  LOG_MSG_INFO1("mcm_mobileap_svc_hdlr_disable::enter handle: 0x%x",
                req_ptr->mcm_mobileap_handle,0,0);
  /*---------------------------------------------------------------------
    Disable SOftAP. This would trigger all RmNet calls to stop.
  ---------------------------------------------------------------------*/
  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE)
  {
    LOG_MSG_ERROR("Incorrect handle passed %d",req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_disable_resp_msg_v01));

    return QMI_CSI_CB_NO_ERR;
  }

  map_handle = &mcm_mobileap_svc_handle;
  if (map_handle->handle == 0)
  {
    LOG_MSG_ERROR("MobileAP inst %d already disabled",index,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_NO_EFFECT_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_disable_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p disabling MobileAP inst %d",
                 clnt_info, index,0);
  LOG_MSG_INFO1("qcmap_sp->client_ref_count = %d , client_count = %d ",
                 qcmap_sp->client_ref_count,client_count,0);

  // If client is enabled but ref count = 0,
  // implies mobileap disable is already called on the server side
  if ((clnt_info->client_enabled) && (qcmap_sp->client_ref_count == 0))
  {
    clnt_info->client_enabled = FALSE;
    resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;
    resp_msg.resp.error = MCM_ERROR_NO_EFFECT_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                            sizeof(mcm_mobileap_disable_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if(clnt_info->client_enabled)
  {
    if(qcmap_sp->client_ref_count > client_count)
    {
      qcmap_sp->client_ref_count--;
      clnt_info->client_enabled = FALSE;
      resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;
      resp_msg.resp.error = MCM_ERROR_NO_EFFECT_V01;
      qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                        sizeof(mcm_mobileap_disable_resp_msg_v01));
      return QMI_CSI_CB_NO_ERR;
    }
    else if(qcmap_sp->client_ref_count == client_count)
    {
#ifdef TARGET_IS_9615
      ret = QcMapMgr->Disable(&mcm_err_num);
#else /* TARGET_IS_9615 */
      ret = QcMapClient->DisableMobileAP(&qmi_err_num);
      mcm_err_num = qmi_to_mcm_error[qmi_err_num];
#endif /* !(TARGET_IS_9615) */
      if ( ret )
      {
        LOG_MSG_INFO1("MobileAP Disable in progress mcm_error: 0x%x",mcm_err_num,0,0);
        resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;
        resp_msg.resp.error = MCM_SUCCESS_V01;
        qcmap_sp->client_ref_count = qcmap_sp->client_ref_count - client_count;
        clnt_info->client_enabled = FALSE;
        map_handle->handle = 0;
      }
      else
      {
        LOG_MSG_ERROR("MobileAP Disable request fails.",0,0,0);
        resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
        resp_msg.resp.error = mcm_err_num;
        qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                          sizeof(mcm_mobileap_disable_resp_msg_v01));
        return QMI_CSI_CB_NO_ERR;
      }

#ifndef TARGET_IS_9615
      /*.If backhaul is not connected, Mobileap will be disabled instantly. And since
      responses are being sent much before the response pending flag is set to TRUE.
      Hence, mcm_err_num is set to MCM_ERROR_NO_EFFECT_V01
      So that a response is sent back to the client.
      */
#endif
      if (mcm_err_num == MCM_ERROR_NO_EFFECT_V01)
      {
        LOG_MSG_INFO1("Disabled MobileAP inst %d", index, 0, 0);
        qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                          sizeof(mcm_mobileap_disable_resp_msg_v01));
      }
      else
      {
        /* Set pending to TRUE to notify client with final resposne
          * once final MobileAP state is received
          */
        clnt_info->qcmap_op_ind_pending = TRUE;
        clnt_info->qcmap_op_req_handle = req_handle;
        clnt_info->qcmap_op_ind_msg_id = MCM_MOBILEAP_DISABLE_RESP_V01;
      }
      return QMI_CSI_CB_NO_ERR;
    }
  }
  else
  {
    LOG_MSG_ERROR("MobileAP not enabled by client, but trying to disable!!", 0, 0, 0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_disable_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  return QMI_CSI_CB_NO_ERR;
} /* mcm_mobileap_svc_hdlr_disable() */

/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_TEAR_DOWN_WWAN()

  DESCRIPTION
    Tear down currently active WWAN connection.

  PARAMETERS
    sp:          MCM MobileAP service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_tear_down_wwan
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  mcm_mobileap_svc_state_info_type       *qcmap_sp;
  mcm_mobileap_tear_down_wwan_req_msg_v01 *req_ptr;
  mcm_mobileap_tear_down_wwan_resp_msg_v01 resp_msg;
  mcm_mobileap_svc_handle_type    *map_handle;
  qmi_csi_error rc;
  unsigned int             index;
  boolean ret;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
#ifndef TARGET_IS_9615
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_wwan_call_type_v01 call_type = QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01;
#endif /* !(TARGET_IS_9615) */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type*)service_handle;
  req_ptr = (mcm_mobileap_tear_down_wwan_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(mcm_mobileap_tear_down_wwan_resp_msg_v01));
  rc = QMI_CSI_NO_ERR;

  /*---------------------------------------------------------------------
      Tear down WWAN for this client
  ---------------------------------------------------------------------*/
  index = (req_ptr->mcm_mobileap_handle);
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
      (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mcm_mobileap_handle, 0, 0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_tear_down_wwan_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p ending WWAN call in MobileAP inst %d",
                 clnt_info, index, 0);

#ifdef TARGET_IS_9615
  ret = QcMapMgr->DisconnectBackHaul(&mcm_err_num);
#else /* TARGET_IS_9615 */
  if(req_ptr->ip_version_valid)
  {
    if (req_ptr->ip_version == MCM_MOBILEAP_IP_V4_V01)
      call_type = QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01;
    else
      call_type = QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01;
  }

  ret = QcMapClient->DisconnectBackHaul(call_type, &qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];
#endif /* !(TARGET_IS_9615) */
  if ( ret )
  {
    LOG_MSG_INFO1("Tear Down WWAN in progress.",0,0,0);
    resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;
    resp_msg.resp.error = MCM_SUCCESS_V01;
  }
  else
  {
    LOG_MSG_ERROR("WWAN teardown failed Error:0x%x", mcm_err_num, 0, 0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_tear_down_wwan_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if ( mcm_err_num == MCM_ERROR_NO_EFFECT_V01 )
  {
    resp_msg.resp_valid = TRUE;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_tear_down_wwan_resp_msg_v01));
    LOG_MSG_INFO1("\nSent WWAN Disconnected message to client %p",
                  clnt_info, 0, 0);
  }
  else
  {
    /* Set pending to TRUE to notify client with indication
     * once final WWAN state is received
     */
#ifdef TARGET_IS_9615
    if (QcMapMgr->GetNatTypeChangeStatus())
    {
      clnt_info->nat_op_ind_pending = TRUE;
      clnt_info->nat_op_req_handle = req_handle;
      clnt_info->nat_op_ind_msg_id = MCM_MOBILEAP_TEAR_DOWN_WWAN_RESP_V01; //////////////CHECK CHECK CHECK CHECK CHECK CHECK//////
    }
#endif /* TARGET_IS_9615 */

    clnt_info->wwan_op_req_handle = req_handle;

#ifndef TARGET_IS_9615
    if (call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01)
    {
#endif
      clnt_info->wwan_op_ind_pending = TRUE;
      clnt_info->wwan_op_ind_msg_id = MCM_MOBILEAP_TEAR_DOWN_WWAN_RESP_V01;
      clnt_info->wwan_op_req_handle = req_handle;
#ifndef TARGET_IS_9615
    }
    else
    {
      clnt_info->ipv6_wwan_op_ind_pending = TRUE;
      clnt_info->ipv6_wwan_op_ind_msg_id = MCM_MOBILEAP_TEAR_DOWN_WWAN_RESP_V01;
      clnt_info->ipv6_wwan_op_req_handle = req_handle;
    }
#endif
  }


//Scenario when wwan does not exist still client wants to tear it down

  return QMI_CSI_CB_NO_ERR;
} /* mcm_mobileap_svc_hdlr_tear_down_wwan() */

/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_BRING_UP_WWAN()

  DESCRIPTION
    Bring up WWAN connection in MobileAP mode.

  PARAMETERS
    sp:          MCM MobileAP service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    mcm_mobileap_svc_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_bring_up_wwan
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  mcm_mobileap_svc_state_info_type       *qcmap_sp;
  mcm_mobileap_bring_up_wwan_req_msg_v01  *req_ptr;
  mcm_mobileap_bring_up_wwan_resp_msg_v01  resp_msg;
  mcm_mobileap_svc_handle_type    *map_handle;
  unsigned int       index;
  qmi_csi_error                     rc;
  boolean ret = FALSE;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
#ifndef TARGET_IS_9615
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_wwan_call_type_v01 call_type = QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01;
#endif /* !(TARGET_IS_9615) */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type*)service_handle;
  req_ptr = (mcm_mobileap_bring_up_wwan_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(mcm_mobileap_bring_up_wwan_resp_msg_v01));

  LOG_MSG_INFO1(" %s Incorrect handle %d passed", __FUNCTION__,
                req_ptr->mcm_mobileap_handle, 0);

  /*---------------------------------------------------------------------
        Start the network interface for this client
  ---------------------------------------------------------------------*/
  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
      (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_bring_up_wwan_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p starting WWAN call in MobileAP inst %d",
                 clnt_info, index,0);

#ifdef TARGET_IS_9615
  ret = QcMapMgr->ConnectBackHaul(&mcm_err_num);
#else /* TARGET_IS_9615 */
  if(req_ptr->ip_version_valid)
  {
    if (req_ptr->ip_version == MCM_MOBILEAP_IP_V4_V01)
      call_type = QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01;
    else
      call_type = QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01;
  }
  ret = QcMapClient->ConnectBackHaul(call_type, &qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];
#endif /* !(TARGET_IS_9615) */
  if ( ret )
  {
    LOG_MSG_INFO1("Bring Up WWAN in progress.",0,0,0);
    resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;
    resp_msg.resp.error = MCM_SUCCESS_V01;
  }
  else
  {
    /* TODO: Send an indication about the failure. */
    LOG_MSG_ERROR("WWAN bringup failed 0x%x", mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_bring_up_wwan_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if ( mcm_err_num == MCM_ERROR_NO_EFFECT_V01 )
  {
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_bring_up_wwan_resp_msg_v01));

    LOG_MSG_INFO1("\nSent WWAN Connected message to client %p",
                  clnt_info, 0, 0);
  }
  else
  {
    /* Set pending to TRUE to notify client with indication
     * once final WWAN state is received
     *  */

#ifndef TARGET_IS_9615
    if (call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01)
    {
#endif /* !(TARGET_IS_9615) */
      clnt_info->wwan_op_ind_pending = TRUE;
      clnt_info->wwan_op_ind_msg_id = MCM_MOBILEAP_BRING_UP_WWAN_RESP_V01;
      clnt_info->wwan_op_req_handle= req_handle;
#ifndef TARGET_IS_9615
    }
    else
    {
      clnt_info->ipv6_wwan_op_ind_pending = TRUE;
      clnt_info->ipv6_wwan_op_ind_msg_id = MCM_MOBILEAP_BRING_UP_WWAN_RESP_V01;
      clnt_info->ipv6_wwan_op_req_handle= req_handle;
    }
#endif /* !(TARGET_IS_9615) */
  }

  return QMI_CSI_CB_NO_ERR;
} /* mcm_mobileap_svc_hdlr_bring_up_wwan() */



/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_GET_NAT_TYPE()

  DESCRIPTION
    Get NAT Type value on Apps.

  PARAMETERS
    sp:        MCM MobileAP service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_get_nat_type
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  mcm_mobileap_svc_state_info_type    *qcmap_sp;
  mcm_mobileap_get_nat_type_req_msg_v01  *req_ptr;
  mcm_mobileap_get_nat_type_resp_msg_v01  resp_msg;
  mcm_mobileap_svc_handle_type    *map_handle;
  unsigned int       index;
#ifdef TARGET_IS_9615
  qcmap_cm_nat_type cur_nat_type;
#else /* TARGET_IS_9615 */
  qcmap_msgr_nat_enum_v01 nat_type;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
#endif /* !(TARGET_IS_9615) */
  boolean ret;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type*)service_handle;
  req_ptr = (mcm_mobileap_get_nat_type_req_msg_v01 *)req_c_struct;
  index = req_ptr->mcm_mobileap_handle;
  LOG_MSG_INFO1 ("Client %p starting Get NAT type in MobileAP inst %d",
                 clnt_info, index,0);
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
        (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_get_nat_type_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p starting Get NAT type in MobileAP inst %d",
                 clnt_info, index,0);
  memset(&resp_msg, 0, sizeof(mcm_mobileap_get_nat_type_resp_msg_v01));


  /*---------------------------------------------------------------------
   Get NAT Type.
  ---------------------------------------------------------------------*/
#ifdef TARGET_IS_9615
  ret = QcMapMgr->GetNatType(&cur_nat_type, &mcm_err_num);
#else /* TARGET_IS_9615 */
  ret = QcMapClient->GetNatType(&nat_type, &qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];
#endif /* !(TARGET_IS_9615) */
  if ( ret )
  {
    LOG_MSG_INFO1("Got NAT type value successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Get NAT Type failed 0x%x", mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_get_nat_type_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  resp_msg.nat_type_valid = TRUE;

#ifdef TARGET_IS_9615
  resp_msg.nat_type= (mcm_mobileap_nat_type_t_v01)cur_nat_type;
#else /* TARGET_IS_9615 */
  resp_msg.nat_type = (mcm_mobileap_nat_type_t_v01) nat_type;
#endif /* !(TARGET_IS_9615) */

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_get_nat_type_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* mcm_mobileap_msgr_get_nat_type */

/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_CHANGE_NAT_TYPE()

  DESCRIPTION
    Set NAT Type on Apps.

  PARAMETERS
    sp:          MCM Mobileap service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error mcm_mobileap_svc_hdlr_change_nat_type
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  mcm_mobileap_svc_state_info_type    *qcmap_sp;
  mcm_mobileap_set_nat_type_req_msg_v01  *req_ptr;
  mcm_mobileap_set_nat_type_resp_msg_v01  resp_msg;
  mcm_mobileap_svc_handle_type    *map_handle;
  unsigned int       index;
  boolean ret;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
#ifndef TARGET_IS_9615
    qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
#endif /* !(TARGET_IS_9615) */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type*)service_handle;
  req_ptr = (mcm_mobileap_set_nat_type_req_msg_v01 *)req_c_struct;
  index = req_ptr->mcm_mobileap_handle;
  LOG_MSG_INFO1 ("Client %p starting Get NAT type in MobileAP inst %d",
                 clnt_info, index,0);

  if (index != MCM_MOBILEAP_SVC_HANDLE ||
        (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_set_nat_type_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  memset(&resp_msg, 0, sizeof(mcm_mobileap_set_nat_type_resp_msg_v01));


/*---------------------------------------------------------------------
   Set NAT Type.
  ---------------------------------------------------------------------*/
#ifdef TARGET_IS_9615
  ret = QcMapMgr->ChangeNatType((qcmap_cm_nat_type)req_ptr->nat_type,
                                 &mcm_err_num);
#else /* TARGET_IS_9615 */
  ret = QcMapClient->SetNatType((qcmap_msgr_nat_enum_v01)req_ptr->nat_type,
                                 &qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];
#endif /* !(TARGET_IS_9615) */
  if ( ret )
  {
    LOG_MSG_INFO1("Sent Change NAT Type request.\n",0,0,0);
    resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;
    resp_msg.resp.error = MCM_SUCCESS_V01;
  }
  else
  {
    LOG_MSG_ERROR("Set NAT Type failed 0x%x!!", mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_set_nat_type_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if ( mcm_err_num == MCM_ERROR_NO_EFFECT_V01 )
  {
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_set_nat_type_resp_msg_v01));
    LOG_MSG_INFO1("\nSent NAT Type Change message to client %p",
                  clnt_info, 0, 0);
  }
  else
  {
    /* Set pending to TRUE to notify client with indication
     * once final NAT Type is received
     *
     */
    clnt_info->nat_op_ind_pending = TRUE;
    clnt_info->nat_op_ind_msg_id = MCM_MOBILEAP_SET_NAT_TYPE_RESP_V01;
    clnt_info->nat_op_req_handle = req_handle;
  }
  return QMI_CSI_CB_NO_ERR;
} /* mcm_mobileap_svc_hdlr_set_nat_type */



/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_GET_AUTO_CONNECT()

  DESCRIPTION
    Get Autoconnect flag value on Apps.

  PARAMETERS
    sp:          MCM Mobilap service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error mcm_mobileap_svc_hdlr_get_auto_connect
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  mcm_mobileap_svc_state_info_type    *qcmap_sp;
  mcm_mobileap_get_auto_connect_req_msg_v01  *req_ptr;
  mcm_mobileap_get_auto_connect_resp_msg_v01  resp_msg;
  mcm_mobileap_svc_handle_type    *map_handle;
  int err_num=0;
  unsigned int              index;
  boolean    auto_connect_flag;
  boolean ret;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
#ifndef TARGET_IS_9615
    qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
#endif /* !(TARGET_IS_9615) */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type *)service_handle;
  req_ptr = (mcm_mobileap_get_auto_connect_req_msg_v01 *)req_c_struct;

  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
      (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_get_auto_connect_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p get auto_connect flag in MobileAP inst %p", clnt_info,index,0);

  memset(&resp_msg, 0, sizeof(mcm_mobileap_get_auto_connect_resp_msg_v01));
  /*---------------------------------------------------------------------

  ---------------------------------------------------------------------*/
#ifdef TARGET_IS_9615
  auto_connect_flag =  QcMapMgr->GetAutoconnect(&mcm_err_num);
#else /* TARGET_IS_9615 */
  QcMapClient->GetAutoconnect(&auto_connect_flag , &qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];
#endif /* !(TARGET_IS_9615) */

  resp_msg.auto_conn_flag_valid = TRUE;
  resp_msg.auto_conn_flag = auto_connect_flag;

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_get_auto_connect_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* mcm_mobileap_svc_hdlr_get_auto_connect */

/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_SET_AUTO_CONNECT()

  DESCRIPTION
    Set auto connect flag value on Apps.

  PARAMETERS
    sp:          MCM Mobileap service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error mcm_mobileap_svc_hdlr_set_auto_connect
(
  mcm_mobileap_svc_client_info_type    *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                     *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  mcm_mobileap_svc_state_info_type *qcmap_sp;
  mcm_mobileap_set_auto_connect_req_msg_v01  *req_ptr;
  mcm_mobileap_set_auto_connect_resp_msg_v01  resp_msg;
  mcm_mobileap_svc_handle_type  *map_handle;
  int err_num=0;
  unsigned int index;
  uint8_t    auto_connect_flag=0;
  int              ret_val;
  boolean ret;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
#ifndef TARGET_IS_9615
    qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
#endif /* !(TARGET_IS_9615) */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type *)service_handle;
  req_ptr = (mcm_mobileap_set_auto_connect_req_msg_v01 *)req_c_struct;

  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
      (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_set_auto_connect_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p set auto_connect flag in MobileAP inst %p", clnt_info,index,0);

  memset(&resp_msg, 0, sizeof(mcm_mobileap_set_auto_connect_resp_msg_v01));


/*----------------------------------------------------------*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef TARGET_IS_9615
  ret = QcMapMgr->SetAutoconnect(req_ptr->enable, &mcm_err_num);
#else /* TARGET_IS_9615 */
  ret = QcMapClient->SetAutoconnect(req_ptr->enable, &qmi_err_num);
#endif /* !(TARGET_IS_9615) */
  if ( ret )
  {
    LOG_MSG_INFO1("Set Auto connect flag successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Set Auto connect failed 0x%x!!",mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_set_auto_connect_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_set_auto_connect_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* mcm_mobileap_svc_hdlr_set_auto_connect */


/*===========================================================================
  FUNCTION mcm_mobileap_svc_hdlr_add_dmz()

  DESCRIPTION
    Add DMZ on Apps.

  PARAMETERS
    sp:          MCM Mobileaps service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    mcm_mobileap_svc_hdlr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_set_dmz
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  mcm_mobileap_svc_state_info_type    *qcmap_sp;

  mcm_mobileap_add_dmz_req_msg_v01 *req_ptr;
  mcm_mobileap_add_dmz_resp_msg_v01 resp_msg;
  mcm_mobileap_svc_handle_type    *map_handle;
  unsigned int     index;
  boolean ret;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
#ifndef TARGET_IS_9615
    qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
#endif /* !(TARGET_IS_9615) */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type *)service_handle;
  req_ptr = (mcm_mobileap_add_dmz_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p add dmz", clnt_info,0,0);

  index = (req_ptr->mcm_mobileap_handle);
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
      (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_add_dmz_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }


  memset(&resp_msg, 0, sizeof(mcm_mobileap_add_dmz_resp_msg_v01));

  /*---------------------------------------------------------------------
   Add DMZ
  ---------------------------------------------------------------------*/

#ifdef TARGET_IS_9615
  ret = QcMapMgr->AddDMZ(req_ptr->dmz_ip_addr, &mcm_err_num);

  if (ret)
#else /* TARGET_IS_9615 */
  ret = QcMapClient->AddDMZ(req_ptr->dmz_ip_addr, &qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];

  if ( ret || (mcm_err_num == MCM_ERROR_INTERFACE_NOT_FOUND_V01))
#endif /* !(TARGET_IS_9615) */
  {
    LOG_MSG_INFO1("Added DMZ successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("DMZ add failed 0x%x!!",mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_add_dmz_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_add_dmz_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* mcm_mobileap_svc_hdlr_add_dmz() */


/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_DELETE_DMZ()

  DESCRIPTION
    Delete DMZ on Apps.

  PARAMETERS
    sp:          MCM Mobileap service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_delete_dmz
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  mcm_mobileap_svc_state_info_type    *qcmap_sp;
  mcm_mobileap_delete_dmz_req_msg_v01 *req_ptr;
  mcm_mobileap_delete_dmz_resp_msg_v01 resp_msg;
  mcm_mobileap_svc_handle_type    *map_handle;

  unsigned int     index;
  boolean ret;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
#ifndef TARGET_IS_9615
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  uint32_t dmz_ip = 0;
#endif /* !(TARGET_IS_9615) */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type *)service_handle;
  req_ptr = (mcm_mobileap_delete_dmz_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p deleting dmz", clnt_info,0,0);

  /*---------------------------------------------------------------------
   Delete DMZ
  ---------------------------------------------------------------------*/

 index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
      (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_delete_dmz_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  memset(&resp_msg, 0, sizeof(mcm_mobileap_delete_dmz_resp_msg_v01));

#ifdef TARGET_IS_9615
  ret = QcMapMgr->DeleteDMZ(req_ptr->dmz_ip_addr, &mcm_err_num);

  if (ret)
#else /* TARGET_IS_9615 */

  /*  Compare the DMZ IP with already configured one before deleting. */
  ret = QcMapClient->GetDMZ(&dmz_ip, &qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];
  if ( ret )
  {
    LOG_MSG_INFO1("Obtaining DMZ address for comparision.\n",0,0,0);
    if (dmz_ip != req_ptr->dmz_ip_addr)
    {
      mcm_err_num = MCM_ERROR_BADPARM_V01;
      LOG_MSG_ERROR("DMZ Address provided is not configured 0x%x!!",
                    mcm_err_num,0,0);
      resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
      resp_msg.resp.error = mcm_err_num;
      qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_delete_dmz_resp_msg_v01));
      return QMI_CSI_CB_NO_ERR;
    }
  }
  else
  {
    LOG_MSG_ERROR("Error while obtaining DMZ address0x%x!!", mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_delete_dmz_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  ret = QcMapClient->DeleteDMZ(&qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];

  if ( ret || (mcm_err_num == MCM_ERROR_INTERFACE_NOT_FOUND_V01))
#endif /* !(TARGET_IS_9615) */
  {
    LOG_MSG_INFO1("Deleted DMZ successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("DMZ delete failed 0x%x!!", mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_delete_dmz_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_delete_dmz_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* mcm_mobileap_svc_hdlr_delete_dmz() */


/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_GET_DMZ()

  DESCRIPTION
    Get DMZ on Apps.

  PARAMETERS
    sp:          MCM Mobileap service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_get_dmz
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  mcm_mobileap_svc_state_info_type    *qcmap_sp;
  mcm_mobileap_get_dmz_req_msg_v01 *req_ptr;
  mcm_mobileap_get_dmz_resp_msg_v01 resp_msg;
  mcm_mobileap_svc_handle_type    *map_handle;

  unsigned int     index;
  boolean ret;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
#ifndef TARGET_IS_9615
    qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
#endif /* !(TARGET_IS_9615) */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type *)service_handle;
  req_ptr = (mcm_mobileap_get_dmz_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p get dmz", clnt_info,0,0);

  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
        (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_get_dmz_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  memset(&resp_msg, 0, sizeof(mcm_mobileap_get_dmz_resp_msg_v01));

  /*---------------------------------------------------------------------
   Get DMZ
  ---------------------------------------------------------------------*/
#ifdef TARGET_IS_9615
  ret  = QcMapMgr->GetDMZ(&(resp_msg.dmz_ip_addr), &mcm_err_num);
#else /* TARGET_IS_9615 */
  ret = QcMapClient->GetDMZ(&(resp_msg.dmz_ip_addr), &qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];
#endif /* !(TARGET_IS_9615) */
  if ( ret )
  {
    LOG_MSG_INFO1("Get DMZ succeeded\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Get DMZ failed 0x%x!!", mcm_err_num,0,0);
    resp_msg.dmz_ip_addr_valid = false;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_get_dmz_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  resp_msg.dmz_ip_addr_valid = true;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_get_dmz_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* mcm_mobileap_svc_hdlr_get_dmz() */


/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_GET_IPSEC_VPN_PT()

  DESCRIPTION
    Get IPSEC VPN Passthrough flag value on Apps.

  PARAMETERS
    sp:          MCM Mobileap service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error mcm_mobileap_svc_hdlr_get_ipsec_vpn_pt
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  mcm_mobileap_svc_state_info_type   *qcmap_sp;
  mcm_mobileap_get_ipsec_vpn_pass_through_req_msg_v01 *req_ptr;
  mcm_mobileap_get_ipsec_vpn_pass_through_resp_msg_v01 resp_msg;
  mcm_mobileap_svc_handle_type *map_handle;
  unsigned int index;
  uint8_t    ipsec_vpn_pt_flag=0;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
  boolean ret;
#ifndef TARGET_IS_9615
    qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
#endif /* !(TARGET_IS_9615) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type *)service_handle;
  req_ptr = (mcm_mobileap_get_ipsec_vpn_pass_through_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p get ipsec vpn pass through", clnt_info,0,0);

  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
      (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_get_ipsec_vpn_pass_through_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  memset(&resp_msg, 0, sizeof(mcm_mobileap_get_ipsec_vpn_pass_through_resp_msg_v01));

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef TARGET_IS_9615
  ret = QcMapMgr->GetIPSECVpnPassThrough(&ipsec_vpn_pt_flag, &mcm_err_num);
#else /* TARGET_IS_9615 */
  ret = QcMapClient->GetIPSECVpnPassthrough((boolean *)&ipsec_vpn_pt_flag,
                                            &qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];
#endif /* !(TARGET_IS_9615) */
  if ( ret )
  {
    LOG_MSG_INFO1("Got IPSEC VPN Passthrough successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Get IPSEC VPN Passthrough failed 0x%x!!",mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_get_ipsec_vpn_pass_through_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  resp_msg.vpn_pass_through_value_valid = TRUE;
  resp_msg.vpn_pass_through_value = ipsec_vpn_pt_flag;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_get_ipsec_vpn_pass_through_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* mcm_mobileap_svc_hdlr_get_ipsec_vpn_pt */

/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_SET_IPSEC_VPN_PT()

  DESCRIPTION
    Set IPSEC VPN Passthrough flag value on Apps.

  PARAMETERS
    sp:          MCM Mobileap service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error mcm_mobileap_svc_hdlr_set_ipsec_vpn_pt
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  mcm_mobileap_svc_state_info_type *qcmap_sp;
  mcm_mobileap_set_ipsec_vpn_pass_through_req_msg_v01 *req_ptr;
  mcm_mobileap_set_ipsec_vpn_pass_through_resp_msg_v01 resp_msg;
  mcm_mobileap_svc_handle_type *map_handle;
  unsigned int  index;
  uint8_t    ipsec_vpn_pt_flag=0;
  boolean ret, enable;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
#ifndef TARGET_IS_9615
    qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
#endif /* !(TARGET_IS_9615) */

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type *)service_handle;
  req_ptr = (mcm_mobileap_set_ipsec_vpn_pass_through_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p set ipsec vpn pass through", clnt_info,0,0);

  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
     (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed", req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_set_ipsec_vpn_pass_through_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  memset(&resp_msg, 0, sizeof(mcm_mobileap_set_ipsec_vpn_pass_through_resp_msg_v01));


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*---------------------------------------------------------------------*/
enable = (boolean)req_ptr->vpn_pass_through_value;
LOG_MSG_ERROR("\n\n Set IPSEC VPN Passthrough enable: %d\n\n ", enable,0,0);
#ifdef TARGET_IS_9615
  ret = QcMapMgr->SetIPSECVpnPassThrough(enable, &mcm_err_num);

  if (ret)
#else /* TARGET_IS_9615 */
  ret = QcMapClient->SetIPSECVpnPassthrough(enable, &qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];

  if ( ret || (mcm_err_num == MCM_ERROR_INTERFACE_NOT_FOUND_V01))
#endif /* !(TARGET_IS_9615) */
  {
    LOG_MSG_INFO1("Set IPSEC VPN Passthrough successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Set IPSEC VPN Passthrough failed 0x%x!!", mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_set_ipsec_vpn_pass_through_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_set_ipsec_vpn_pass_through_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* mcm_mobileap_svc_hdlr_set_ipsec_vpn_pt */


/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_GET_PPTP_VPN_PT()

  DESCRIPTION
    Get PPTP VPN Passthrough flag value on Apps.

  PARAMETERS
    sp:          MCM Mobileap service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error mcm_mobileap_svc_hdlr_get_pptp_vpn_pt
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  mcm_mobileap_svc_state_info_type *qcmap_sp;
  mcm_mobileap_get_pptp_vpn_pass_through_req_msg_v01 *req_ptr;
  mcm_mobileap_get_pptp_vpn_pass_through_resp_msg_v01 resp_msg;
  mcm_mobileap_svc_handle_type  *map_handle;
  unsigned int index;
  uint8_t  pptp_vpn_pt_flag=0;
  boolean ret;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
#ifndef TARGET_IS_9615
    qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
#endif /* !(TARGET_IS_9615) */

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type *)service_handle;
  req_ptr = (mcm_mobileap_get_pptp_vpn_pass_through_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p get pptp vpn pass through", clnt_info,0,0);

  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
      (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_get_pptp_vpn_pass_through_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  memset(&resp_msg, 0, sizeof(mcm_mobileap_get_pptp_vpn_pass_through_resp_msg_v01));

  /*---------------------------------------------------------------------

  ---------------------------------------------------------------------*/
#ifdef TARGET_IS_9615
  ret = QcMapMgr->GetPPTPVpnPassThrough(&pptp_vpn_pt_flag, &mcm_err_num);
#else /* TARGET_IS_9615 */
  ret = QcMapClient->GetPPTPVpnPassthrough((boolean *)&pptp_vpn_pt_flag,
                                           &qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];
#endif /* !(TARGET_IS_9615) */
  if ( ret )
  {
    LOG_MSG_INFO1("Got PPTP VPN Passthrough successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Get PPTP VPN Passthrough failed 0x%x!!", mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_get_pptp_vpn_pass_through_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  resp_msg.vpn_pass_through_value_valid = TRUE;
  resp_msg.vpn_pass_through_value = pptp_vpn_pt_flag;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_get_pptp_vpn_pass_through_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* mcm_mobileap_svc_hdlr_get_pptp_vpn_pt */

/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_SET_PPTP_VPN_PT()

  DESCRIPTION
    Set PPTP VPN Passthrough flag value on Apps.

  PARAMETERS
    sp:          QMI_QCMAP MSGR's service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error mcm_mobileap_svc_hdlr_set_pptp_vpn_pt
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  mcm_mobileap_svc_state_info_type   *qcmap_sp;
  mcm_mobileap_set_pptp_vpn_pass_through_req_msg_v01 *req_ptr;
  mcm_mobileap_set_pptp_vpn_pass_through_resp_msg_v01 resp_msg;
  mcm_mobileap_svc_handle_type  *map_handle;

  unsigned int index;
  uint8_t pptp_vpn_pt_flag=0;
  boolean ret;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
#ifndef TARGET_IS_9615
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
#endif /* !(TARGET_IS_9615) */

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type *)service_handle;
  req_ptr = (mcm_mobileap_set_pptp_vpn_pass_through_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p set pptp vpn pass through", clnt_info,0,0);

  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
      (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                     sizeof(mcm_mobileap_set_pptp_vpn_pass_through_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  memset(&resp_msg, 0, sizeof(mcm_mobileap_set_pptp_vpn_pass_through_resp_msg_v01));


  /*---------------------------------------------------------------------

  ---------------------------------------------------------------------*/
#ifdef TARGET_IS_9615
  ret = QcMapMgr->SetPPTPVpnPassThrough(req_ptr->vpn_pass_through_value,
                                        &mcm_err_num);
  if (ret)
#else /* TARGET_IS_9615 */
  ret = QcMapClient->SetPPTPVpnPassthrough(
                                     (boolean)(req_ptr->vpn_pass_through_value),
                                     &qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];

  if ( ret || (mcm_err_num == MCM_ERROR_INTERFACE_NOT_FOUND_V01))
#endif /* !(TARGET_IS_9615) */
  {
    LOG_MSG_INFO1("Set PPTP VPN Passthrough successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Set PPTP VPN Passthrough failed 0x%x!!", mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_set_pptp_vpn_pass_through_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_set_pptp_vpn_pass_through_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* mcm_mobileap_svc_hdlr_set_pptp_vpn_pt */

/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_L2TP_VPN_PT()

  DESCRIPTION
    Get L2TP VPN Passthrough flag value on Apps.

  PARAMETERS
    sp:          QMI_QCMAP MSGR's service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error mcm_mobileap_svc_hdlr_get_l2tp_vpn_pt
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  mcm_mobileap_svc_state_info_type *qcmap_sp;
  mcm_mobileap_get_l2tp_vpn_pass_through_req_msg_v01 *req_ptr;
  mcm_mobileap_get_l2tp_vpn_pass_through_resp_msg_v01 resp_msg;
  mcm_mobileap_svc_handle_type  *map_handle;

  unsigned int index;
  uint8_t l2tp_vpn_pt_flag=0;
  boolean ret;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
#ifndef TARGET_IS_9615
    qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
#endif /* !(TARGET_IS_9615) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type *)service_handle;
  req_ptr = (mcm_mobileap_get_l2tp_vpn_pass_through_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p get l2tp vpn pass through", clnt_info,0,0);

  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
      (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_get_l2tp_vpn_pass_through_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  memset(&resp_msg, 0, sizeof(mcm_mobileap_get_l2tp_vpn_pass_through_resp_msg_v01));


  /*---------------------------------------------------------------------

  ---------------------------------------------------------------------*/
#ifdef TARGET_IS_9615
  ret = QcMapMgr->GetL2TPVpnPassThrough(&l2tp_vpn_pt_flag, &mcm_err_num);
#else /* TARGET_IS_9615 */
  ret = QcMapClient->GetL2TPVpnPassthrough(&l2tp_vpn_pt_flag, &qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];
#endif /* !(TARGET_IS_9615) */
  if ( ret )
  {
    LOG_MSG_INFO1("Got L2TP VPN Passthrough successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Get L2TP VPN Passthrough failed 0x%x!!", mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_get_l2tp_vpn_pass_through_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  resp_msg.vpn_pass_through_value_valid = TRUE;
  resp_msg.vpn_pass_through_value = l2tp_vpn_pt_flag;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_get_l2tp_vpn_pass_through_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* mcm_mobileap_svc_hdlr_get_l2tp_vpn_pt */

/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_L2TP_VPN_PT()

  DESCRIPTION
    Set L2TP VPN Passthrough flag value on Apps.

  PARAMETERS
    sp:          MCM Mobileap service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error mcm_mobileap_svc_hdlr_set_l2tp_vpn_pt
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  mcm_mobileap_svc_state_info_type   *qcmap_sp;
  mcm_mobileap_set_l2tp_vpn_pass_through_req_msg_v01 *req_ptr;
  mcm_mobileap_set_l2tp_vpn_pass_through_resp_msg_v01 resp_msg;
  mcm_mobileap_svc_handle_type  *map_handle;

  unsigned int index;
  uint8_t l2tp_vpn_pt_flag=0;
  boolean ret;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
#ifndef TARGET_IS_9615
    qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
#endif /* !(TARGET_IS_9615) */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type *)service_handle;
  req_ptr = (mcm_mobileap_set_l2tp_vpn_pass_through_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p set l2tp vpn pass through", clnt_info,0,0);

  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
      (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_set_l2tp_vpn_pass_through_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  memset(&resp_msg, 0, sizeof(mcm_mobileap_set_l2tp_vpn_pass_through_resp_msg_v01));

  /*---------------------------------------------------------------------

  ---------------------------------------------------------------------*/
#ifdef TARGET_IS_9615
  ret = QcMapMgr->SetL2TPVpnPassThrough(req_ptr->vpn_pass_through_value,
                                        &mcm_err_num);
  if (ret)
#else /* TARGET_IS_9615 */
  ret = QcMapClient->SetL2TPVpnPassthrough(
                                       (boolean)req_ptr->vpn_pass_through_value,
                                       &qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];
  if ( ret || (mcm_err_num == MCM_ERROR_INTERFACE_NOT_FOUND_V01))
#endif /* !(TARGET_IS_9615) */
  {
    LOG_MSG_INFO1("Set L2TP VPN Passthrough successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Set L2TP VPN Passthrough failed 0x%x!!", mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_set_l2tp_vpn_pass_through_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_set_l2tp_vpn_pass_through_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* mcm_mobileap_svc_hdlr_set_l2tp_vpn_pt */


/*==========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_GET_FIREWALL_ENTRIES_HANDLE_LIST_REQ_V01

  DESCRIPTION
    Get Firewall handle list .

  PARAMETERS
    sp:          MCM Mobileap service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error mcm_mobileap_svc_hdlr_get_firewall_handles_list
(
 mcm_mobileap_svc_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
 )
{

  mcm_mobileap_svc_state_info_type *qcmap_sp;
  mcm_mobileap_get_firewall_entries_handle_list_req_msg_v01 *req_ptr;
  mcm_mobileap_get_firewall_entries_handle_list_resp_msg_v01  resp_msg;
  mcm_mobileap_svc_handle_type  *map_handle;
  unsigned int index;
  boolean ret;
  int i;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
#ifdef TARGET_IS_9615
  qcmap_cm_extd_firewall_conf_t firewall_entry;
#else /* TARGET_IS_9615 */
    qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_firewall_conf_t extd_firewall_handle_list;
#endif /* !(TARGET_IS_9615) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    ds_assert(req_c_struct != NULL);
    ds_assert(req_c_struct_len > 0);
    ds_assert(clnt_info != NULL);
    ds_assert(service_handle != NULL);

    qcmap_sp = (mcm_mobileap_svc_state_info_type *)service_handle;
    req_ptr = (mcm_mobileap_get_firewall_entries_handle_list_req_msg_v01 *)req_c_struct;

    LOG_MSG_INFO1 ("Client %p get firewall handles list", clnt_info,0,0);

    index = req_ptr->mcm_mobileap_handle;
    if (index != MCM_MOBILEAP_SVC_HANDLE ||
        (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
    {
      LOG_MSG_ERROR("Incorrect handle %d passed",
                    req_ptr->mcm_mobileap_handle,0,0);
      resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
      resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
      qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                        sizeof(mcm_mobileap_get_firewall_entries_handle_list_resp_msg_v01));
      return QMI_CSI_CB_NO_ERR;
    }

  memset(&resp_msg, 0, sizeof(mcm_mobileap_get_firewall_entries_handle_list_resp_msg_v01));

#ifdef TARGET_IS_9615
  if ( req_ptr->ip_version == MCM_MOBILEAP_IP_FAMILY_V4_V01 )
  {
    firewall_entry.extd_firewall_handle_list.ip_family=IP_V4;
  }
  else if ( req_ptr->ip_version == MCM_MOBILEAP_IP_FAMILY_V6_V01 )
  {
    firewall_entry.extd_firewall_handle_list.ip_family=IP_V6;
  }
  else
  {
    LOG_MSG_ERROR("Ip version %d not supported",req_ptr->ip_version,0,0);
    mcm_err_num = MCM_ERROR_INVALID_ARG_V01;
    goto get_handlist_error;
  }
  ret = QcMapMgr->GetFireWallHandleList(&firewall_entry, &mcm_err_num);
#else /* TARGET_IS_9615 */
  if ( req_ptr->ip_version == MCM_MOBILEAP_IP_FAMILY_V4_V01 )
  {
    extd_firewall_handle_list.extd_firewall_handle_list.ip_family = IP_V4;
  }
  else if ( req_ptr->ip_version == MCM_MOBILEAP_IP_FAMILY_V6_V01 )
  {
    extd_firewall_handle_list.extd_firewall_handle_list.ip_family = IP_V6;
  }
  else
  {
    LOG_MSG_ERROR("Ip version %d not supported", req_ptr->ip_version, 0, 0);
    mcm_err_num = MCM_ERROR_INVALID_ARG_V01;
    goto get_handlist_error;
  }
  ret = QcMapClient->GetFireWallHandlesList(
                 &extd_firewall_handle_list.extd_firewall_handle_list,
                 &qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];
#endif /* !(TARGET_IS_9615) */
  if ( ret )
  {
    LOG_MSG_INFO1("Get Firewall Handle list successfully.\n",0,0,0);
    resp_msg.firewall_handle_list_valid= TRUE;

#ifdef TARGET_IS_9615
    resp_msg.firewall_handle_list_len =
                       firewall_entry.extd_firewall_handle_list.num_of_entries;
    for( i = 0; i < resp_msg.firewall_handle_list_len ; i++)
    {
      resp_msg.firewall_handle_list[i] =
                       firewall_entry.extd_firewall_handle_list.handle_list[i];
    }
#else /* TARGET_IS_9615 */
    resp_msg.firewall_handle_list_len =
            extd_firewall_handle_list.extd_firewall_handle_list.num_of_entries;
    for(i = 0; i < resp_msg.firewall_handle_list_len ; i++)
    {
      resp_msg.firewall_handle_list[i] =
            extd_firewall_handle_list.extd_firewall_handle_list.handle_list[i];
    }
#endif /* !(TARGET_IS_9615) */
  }
  else
  {
    get_handlist_error:
    LOG_MSG_ERROR("Get Firewall Handle list Failed 0x%x!!", mcm_err_num, 0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
        sizeof(mcm_mobileap_get_firewall_entries_handle_list_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
      sizeof(mcm_mobileap_get_firewall_entries_handle_list_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

}


/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_DELETE_EXTD_FIREWALL_ENTRY_REQ_V01()

  DESCRIPTION
    Add Firewall entry on Apps.

  PARAMETERS
    sp:          MCM Mobileap service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_delete_extd_firewall_entry
(
 mcm_mobileap_svc_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
 )
{
  mcm_mobileap_svc_state_info_type *qcmap_sp;
  mcm_mobileap_delete_firewall_entry_req_msg_v01 *req_ptr;
  mcm_mobileap_delete_firewall_entry_resp_msg_v01  resp_msg;
  mcm_mobileap_svc_handle_type  *map_handle;

  unsigned int index;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
  boolean ret;
#ifdef TARGET_IS_9615
  qcmap_cm_extd_firewall_conf_t firewall_entry;
#else /* TARGET_IS_9615 */
  int status;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
#endif /* !(TARGET_IS_9615) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type *)service_handle;
  req_ptr = (mcm_mobileap_delete_firewall_entry_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p delete firewall entry", clnt_info,0,0);

  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
      (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                   req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_delete_firewall_entry_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  memset(&resp_msg, 0, sizeof(mcm_mobileap_delete_firewall_entry_resp_msg_v01));

#ifdef TARGET_IS_9615
  firewall_entry.extd_firewall_handle.handle=req_ptr->firewall_handle;
   ret = QcMapMgr->DeleteExtdFireWallEntry(&firewall_entry, &mcm_err_num);

   if (ret)
#else /* TARGET_IS_9615 */
  status = QcMapClient->DeleteFireWallEntry(req_ptr->firewall_handle, &qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];
  if ((status == 0) || (mcm_err_num == MCM_ERROR_INTERFACE_NOT_FOUND_V01))
#endif /* !(TARGET_IS_9615) */
  {
    LOG_MSG_INFO1("Delete entry successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Delete entry failed 0x%x",mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
        sizeof(mcm_mobileap_delete_firewall_entry_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
      sizeof(mcm_mobileap_delete_firewall_entry_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

}


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_EXTD_FIREWALL_ENTRY_REQ_V01()

  DESCRIPTION
    Get Firewall entry on Apps.

  PARAMETERS
    sp:          MCM Mobileap service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
    ===========================================================================*/
static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_get_extd_firewall_entry
(
 mcm_mobileap_svc_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
 )
{

  mcm_mobileap_svc_state_info_type *qcmap_sp;
  mcm_mobileap_get_firewall_entry_req_msg_v01 *req_ptr;
  mcm_mobileap_get_firewall_entry_resp_msg_v01  resp_msg;
  mcm_mobileap_svc_handle_type  *map_handle;
  uint8_t ip_vsn,next_hdr_prot=0;
  boolean ret;
  unsigned int index;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
  ip_filter_type ip_filter;
#ifdef TARGET_IS_9615
  qcmap_cm_extd_firewall_conf_t extd_firewall_conf;
#else /* TARGET_IS_9615 */
  qcmap_msgr_firewall_entry_conf_t firewall_entry;
    qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
#endif /* !(TARGET_IS_9615) */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type *)service_handle;
  req_ptr = (mcm_mobileap_get_firewall_entry_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p get firewall entry", clnt_info,0,0);

  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
      (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                   req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_get_firewall_entry_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  memset(&resp_msg, 0, sizeof(mcm_mobileap_get_firewall_entry_resp_msg_v01));
  LOG_MSG_INFO1("\n\n %s Received Firewall Handle : %d\n\n",
                __FUNCTION__, req_ptr->firewall_handle,0);

#ifdef TARGET_IS_9615

  extd_firewall_conf.extd_firewall_entry.firewall_handle =
                                     req_ptr->firewall_handle;
  ret = QcMapMgr->GetExtdFireWallEntry(&extd_firewall_conf, &mcm_err_num);
  ip_filter = extd_firewall_conf.extd_firewall_entry.filter_spec;
#else /* TARGET_IS_9615 */
  firewall_entry.firewall_handle = req_ptr->firewall_handle;
  ret = QcMapClient->GetFireWallEntry(&firewall_entry, &qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];
  ip_filter = firewall_entry.filter_spec;
#endif /* !(TARGET_IS_9615) */
  if ( ret )
  {
    LOG_MSG_INFO1("Got Firewall entry successfully ",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("No Entries found 0x%x!!", mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
        sizeof(mcm_mobileap_get_firewall_entry_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  ip_vsn = ip_filter.ip_vsn;

  resp_msg.ip_version_valid = TRUE;

  switch(ip_vsn)
  {
    LOG_MSG_INFO1("IP_VSN %d", ip_vsn , 0, 0);

    case IP_V4:

      resp_msg.ip_version = MCM_MOBILEAP_IP_FAMILY_V4_V01;
      if ( ip_filter.ip_hdr.v4.field_mask &
           IPFLTR_MASK_IP4_SRC_ADDR )
      {
        resp_msg.ip4_src_addr_valid = TRUE;
        resp_msg.ip4_src_addr.addr = (ip_filter.ip_hdr.v4.src.addr.ps_s_addr);
        resp_msg.ip4_src_addr.subnet_mask =
          (ip_filter.ip_hdr.v4.src.subnet_mask.ps_s_addr);
        LOG_MSG_INFO1("IP4 source address is:",resp_msg.ip4_src_addr.addr, 0, 0);
        IPV4_ADDR_MSG(ip_filter.ip_hdr.v4.src.addr.ps_s_addr);
        LOG_MSG_INFO1("IP4 subnet mask is:", resp_msg.ip4_src_addr.subnet_mask, 0, 0);
        IPV4_ADDR_MSG(ip_filter.ip_hdr.v4.src.subnet_mask.ps_s_addr);
      }

      if ( ip_filter.ip_hdr.v4.field_mask &
           IPFLTR_MASK_IP4_DST_ADDR )
      {
        resp_msg.ip4_dst_addr_valid = TRUE;
        resp_msg.ip4_dst_addr.addr = (ip_filter.ip_hdr.v4.dst.addr.ps_s_addr);
        resp_msg.ip4_dst_addr.subnet_mask =
          (ip_filter.ip_hdr.v4.dst.subnet_mask.ps_s_addr);
        LOG_MSG_INFO1("IP4 dst address is:",resp_msg.ip4_dst_addr.addr, 0, 0);
        IPV4_ADDR_MSG(ip_filter.ip_hdr.v4.dst.addr.ps_s_addr);
        LOG_MSG_INFO1("IP4 dst subnet mask is:", resp_msg.ip4_dst_addr.subnet_mask, 0, 0);
        IPV4_ADDR_MSG(ip_filter.ip_hdr.v4.dst.subnet_mask.ps_s_addr);
      }

      if ( ip_filter.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_TOS )
      {
        resp_msg.ip4_tos_valid = TRUE;
        resp_msg.ip4_tos.value = ip_filter.ip_hdr.v4.tos.val;
        resp_msg.ip4_tos.mask = ip_filter.ip_hdr.v4.tos.mask;
        LOG_MSG_INFO1("IP4 TOS val %d mask %d",
                      resp_msg.ip4_tos.value , resp_msg.ip4_tos.mask, 0);
      }

      if ( ip_filter.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_NEXT_HDR_PROT )
      {
        LOG_MSG_INFO1("IP4 next header protocol %d ", resp_msg.next_hdr_prot, 0, 0);
        resp_msg.next_hdr_prot_valid = TRUE;
        resp_msg.next_hdr_prot = ip_filter.ip_hdr.v4.next_hdr_prot;
        next_hdr_prot = ip_filter.ip_hdr.v4.next_hdr_prot;
      }
      break;

    case IP_V6:

      resp_msg.ip_version = MCM_MOBILEAP_IP_FAMILY_V6_V01;
      if ( ip_filter.ip_hdr.v6.field_mask &
           IPFLTR_MASK_IP6_SRC_ADDR )
      {
        memcpy(resp_msg.ip6_src_addr.addr,
               ip_filter.ip_hdr.v6.src.addr.in6_u.u6_addr8,
               MCM_MOBILEAP_IPV6_ADDR_LEN_V01*sizeof(uint8_t));

        resp_msg.ip6_src_addr.prefix_len =
          ip_filter.ip_hdr.v6.src.prefix_len;
        resp_msg.ip6_src_addr_valid = TRUE;

        LOG_MSG_INFO1("IPV6 src_addr is:",
                      ip_filter.ip_hdr.v6.src.addr.in6_u.u6_addr64, 0, 0);
        IPV6_ADDR_MSG(ip_filter.ip_hdr.v6.src.addr.in6_u.u6_addr64);
        LOG_MSG_INFO1("IP6 prefix length %d  ",
                      resp_msg.ip6_src_addr.prefix_len, 0, 0);
      }

      if ( ip_filter.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_DST_ADDR )
      {

        memcpy(resp_msg.ip6_dst_addr.addr,
               ip_filter.ip_hdr.v6.dst.addr.in6_u.u6_addr8,
               MCM_MOBILEAP_IPV6_ADDR_LEN_V01*sizeof(uint8_t));
        resp_msg.ip6_dst_addr.prefix_len = ip_filter.ip_hdr.v6.dst.prefix_len;
        resp_msg.ip6_dst_addr_valid = TRUE;
        LOG_MSG_INFO1("IPV6 dst_addr is:",
                      ip_filter.ip_hdr.v6.dst.addr.in6_u.u6_addr8, 0, 0);
        IPV6_ADDR_MSG(ip_filter.ip_hdr.v6.dst.addr.in6_u.u6_addr64);
        LOG_MSG_INFO1("IP6 prefix length %d  ",
                      resp_msg.ip6_dst_addr.prefix_len, 0, 0);
      }

      if ( ip_filter.ip_hdr.v6.field_mask &
           IPFLTR_MASK_IP6_TRAFFIC_CLASS )
      {
        resp_msg.ip6_trf_cls.value = ip_filter.ip_hdr.v6.trf_cls.val;
        resp_msg.ip6_trf_cls.mask = ip_filter.ip_hdr.v6.trf_cls.mask;
        resp_msg.ip6_trf_cls_valid = TRUE;
        LOG_MSG_INFO1("trf class val %d  mask %d",
                      resp_msg.ip6_trf_cls.value,
                      resp_msg.ip6_trf_cls.mask, 0);
      }

      if ( ip_filter.ip_hdr.v6.field_mask &
           IPFLTR_MASK_IP6_NEXT_HDR_PROT )
      {
        resp_msg.next_hdr_prot_valid = TRUE;
        next_hdr_prot = ip_filter.ip_hdr.v6.next_hdr_prot;
        resp_msg.next_hdr_prot = next_hdr_prot;
        LOG_MSG_INFO1("IP6 next header protocol  %d  ", next_hdr_prot,0, 0);
      }
      break;
    default:
      LOG_MSG_ERROR("Unsupported IP protocol %d", ip_vsn, 0, 0);
  }

  switch(next_hdr_prot)
  {
    case PS_IPPROTO_TCP:

      if ( ip_filter.next_prot_hdr.tcp.field_mask
           & IPFLTR_MASK_TCP_SRC_PORT )
      {
        resp_msg.tcp_udp_src.port = (ip_filter.next_prot_hdr.tcp.src.port);
        resp_msg.tcp_udp_src.range = ip_filter.next_prot_hdr.tcp.src.range;
        resp_msg.tcp_udp_src_valid = TRUE;
        LOG_MSG_INFO1("TCP src port  %d  range %d",
                      resp_msg.tcp_udp_src.port,
                      resp_msg.tcp_udp_src.range, 0);
      }

      if ( ip_filter.next_prot_hdr.tcp.field_mask
           & IPFLTR_MASK_TCP_DST_PORT )
      {
        resp_msg.tcp_udp_dst.port = (ip_filter.next_prot_hdr.tcp.dst.port);
        resp_msg.tcp_udp_dst.range = ip_filter.next_prot_hdr.tcp.dst.range;
        resp_msg.tcp_udp_dst_valid = TRUE;
        LOG_MSG_INFO1("TCP dst port  %d  range %d",
                      resp_msg.tcp_udp_dst.port,
                      resp_msg.tcp_udp_dst.range, 0);
      }
      break;

    case PS_IPPROTO_UDP:

      if ( ip_filter.next_prot_hdr.udp.field_mask
           & IPFLTR_MASK_UDP_SRC_PORT )
      {
        resp_msg.tcp_udp_src.port = (ip_filter.next_prot_hdr.udp.src.port);
        resp_msg.tcp_udp_src.range = ip_filter.next_prot_hdr.udp.src.range;
        resp_msg.tcp_udp_src_valid = TRUE;
        LOG_MSG_INFO1("UDP src port  %d  range %d",
                      resp_msg.tcp_udp_src.port,
                      resp_msg.tcp_udp_src.range, 0);
      }

      if ( ip_filter.next_prot_hdr.udp.field_mask
           & IPFLTR_MASK_UDP_DST_PORT )
      {
        resp_msg.tcp_udp_dst.port = ip_filter.next_prot_hdr.udp.dst.port;
        resp_msg.tcp_udp_dst.range = ip_filter.next_prot_hdr.udp.dst.range;
        resp_msg.tcp_udp_dst_valid = TRUE;
        LOG_MSG_INFO1("UDP dst port  %d  range %d",
                      resp_msg.tcp_udp_dst.port ,
                      resp_msg.tcp_udp_dst.range, 0);
      }
      break;

    case PS_IPPROTO_ICMP:
    case PS_IPPROTO_ICMP6:

      if ( ip_filter.next_prot_hdr.icmp.field_mask
           & IPFLTR_MASK_ICMP_MSG_TYPE )
      {
        resp_msg.icmp_type = ip_filter.next_prot_hdr.icmp.type;
        resp_msg.icmp_type_valid = TRUE;
        LOG_MSG_INFO1("ICMP type  %d  ", resp_msg.icmp_type , 0, 0);
      }

      if ( ip_filter.next_prot_hdr.udp.field_mask
           & IPFLTR_MASK_ICMP_MSG_CODE )
      {
        resp_msg.icmp_code = ip_filter.next_prot_hdr.icmp.code;
        resp_msg.icmp_code_valid = TRUE;
        LOG_MSG_INFO1("ICMP code  %d  ", resp_msg.icmp_code , 0, 0);
      }
      break;

    case PS_IPPROTO_ESP:

      if ( ip_filter.next_prot_hdr.esp.field_mask
           & IPFLTR_MASK_ESP_SPI )
      {
        resp_msg.esp_spi = (ip_filter.next_prot_hdr.esp.spi);
        resp_msg.esp_spi_valid = TRUE;
        LOG_MSG_INFO1("ESP spi %d", resp_msg.esp_spi , 0, 0);
      }
      break;

    case PS_IPPROTO_TCP_UDP:

      if ( ip_filter.next_prot_hdr.tcp_udp_port_range.field_mask
           & IPFLTR_MASK_TCP_UDP_SRC_PORT )
      {
        resp_msg.tcp_udp_src.port =
          (ip_filter.next_prot_hdr.tcp_udp_port_range.src.port);
        resp_msg.tcp_udp_src.range =
          ip_filter.next_prot_hdr.tcp_udp_port_range.src.range;
        resp_msg.tcp_udp_src_valid = TRUE;
        LOG_MSG_INFO1("TCP_UDP port %d range %d",
                      resp_msg.tcp_udp_src.port ,
                      resp_msg.tcp_udp_src.range, 0);
      }

      if ( ip_filter.next_prot_hdr.tcp_udp_port_range.field_mask
           & IPFLTR_MASK_TCP_UDP_DST_PORT )
      {
        resp_msg.tcp_udp_dst.port =
          (ip_filter.next_prot_hdr.tcp_udp_port_range.dst.port);
        resp_msg.tcp_udp_dst.range =
          ip_filter.next_prot_hdr.tcp_udp_port_range.dst.range;
        resp_msg.tcp_udp_dst_valid = TRUE;
        LOG_MSG_INFO1("TCP_UDP port %d range %d",
                      resp_msg.tcp_udp_dst.port ,
                      resp_msg.tcp_udp_dst.range, 0);
      }
     break;

    default:
      LOG_MSG_ERROR("Unsupported next header protocol %d",next_hdr_prot, 0, 0);
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
      sizeof(mcm_mobileap_get_firewall_entry_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_ADD_EXTD_FIREWALL_ENTRY_REQ_V01()

  DESCRIPTION
    Add Firewall entry on Apps.

  PARAMETERS
    sp:          QMI_QCMAP MSGR's service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_add_extd_firewall_entry
(
 mcm_mobileap_svc_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
 )
{
  mcm_mobileap_svc_state_info_type *qcmap_sp;
  mcm_mobileap_add_firewall_entry_req_msg_v01 *req_ptr;
  mcm_mobileap_add_firewall_entry_resp_msg_v01  resp_msg;
  mcm_mobileap_svc_handle_type  *map_handle;

  unsigned int index;
  int next_hdr_prot,mask;
  boolean ret;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
  ip_filter_type ip_filter;
#ifdef TARGET_IS_9615
  qcmap_cm_extd_firewall_conf_t extd_firewall_conf;
#else /* TARGET_IS_9615 */
  qcmap_msgr_firewall_conf_t firewall_entry;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
#endif /* !(TARGET_IS_9615) */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type *)service_handle;
  req_ptr = (mcm_mobileap_add_firewall_entry_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p add firewall entry", clnt_info,0,0);

  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
      (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_add_firewall_entry_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  memset(&resp_msg, 0, sizeof(mcm_mobileap_add_firewall_entry_resp_msg_v01));
  if(req_ptr->ip_version == MCM_MOBILEAP_IP_FAMILY_V4_V01)
  {
    ip_filter.ip_vsn = IP_V4;
  }
  else if (req_ptr->ip_version == MCM_MOBILEAP_IP_FAMILY_V6_V01)
  {
    ip_filter.ip_vsn = IP_V6;
  }
  else
  {
    LOG_MSG_ERROR("Unsupported IP Version %d",
                   req_ptr->ip_version , 0, 0);
    mcm_err_num = MCM_ERROR_INVALID_ARG_V01;
    goto firewall_add_error;
  }

  switch (ip_filter.ip_vsn)
  {
    case IP_V4:
      if( req_ptr->ip4_src_addr_valid )
      {
        ip_filter.ip_hdr.v4.src.addr.ps_s_addr = (req_ptr->ip4_src_addr.addr);
        ip_filter.ip_hdr.v4.src.subnet_mask.ps_s_addr =
                                        (req_ptr->ip4_src_addr.subnet_mask);
        ip_filter.ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_SRC_ADDR;
        LOG_MSG_INFO1("IP4 source address is:",0,0,0);
        IPV4_ADDR_MSG(htonl(ip_filter.ip_hdr.v4.src.addr.ps_s_addr));
        LOG_MSG_INFO1("IP4 source subnet mask is:", 0, 0, 0);
        IPV4_ADDR_MSG(htonl(ip_filter.ip_hdr.v4.src.subnet_mask.ps_s_addr));
      }

      if(req_ptr->ip4_tos_valid)
      {
        ip_filter.ip_hdr.v4.tos.val = req_ptr->ip4_tos.value;
        ip_filter.ip_hdr.v4.tos.mask = req_ptr->ip4_tos.mask;
        ip_filter.ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_TOS;
        LOG_MSG_INFO1("IP4  TOS value %d mask %d ",
                      ip_filter.ip_hdr.v4.tos.val, ip_filter.ip_hdr.v4.tos.mask, 0);
      }

      if(req_ptr->next_hdr_prot_valid)
      {
       ip_filter.ip_hdr.v4.next_hdr_prot=req_ptr->next_hdr_prot;
       ip_filter.ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_NEXT_HDR_PROT;
      }
      break;

    case IP_V6:
      if (req_ptr->ip6_src_addr_valid)
      {
        memcpy(ip_filter.ip_hdr.v6.src.addr.in6_u.u6_addr8,
               req_ptr->ip6_src_addr.addr,
               MCM_MOBILEAP_IPV6_ADDR_LEN_V01*sizeof(uint8_t));

        ip_filter.ip_hdr.v6.src.prefix_len =
                                 req_ptr->ip6_src_addr.prefix_len;
        ip_filter.ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_SRC_ADDR;

        LOG_MSG_INFO1("IPV6 src_addr is:",0, 0, 0);
        IPV6_ADDR_MSG(ip_filter.ip_hdr.v6.src.addr.in6_u.u6_addr64);
        LOG_MSG_INFO1("IPV6 prefix len %d",
                      ip_filter.ip_hdr.v6.src.prefix_len , 0, 0);
      }

      if(req_ptr->ip6_trf_cls_valid)
      {
        ip_filter.ip_hdr.v6.trf_cls.val = req_ptr->ip6_trf_cls.value;
        ip_filter.ip_hdr.v6.trf_cls.mask = req_ptr->ip6_trf_cls.mask;
        ip_filter.ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_TRAFFIC_CLASS;
        LOG_MSG_INFO1("IPV6 trf class value %d mask %d",
                      ip_filter.ip_hdr.v6.trf_cls.val ,
                      ip_filter.ip_hdr.v6.trf_cls.mask, 0);
      }

      if(req_ptr->next_hdr_prot_valid)
      {
        ip_filter.ip_hdr.v6.next_hdr_prot=req_ptr->next_hdr_prot;
        ip_filter.ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_NEXT_HDR_PROT ;
      }
      break;

    default:
      LOG_MSG_ERROR("Bad IP version %d", ip_filter.ip_vsn, 0, 0);
  }

  LOG_MSG_INFO1("Next header protocol %d  ",
                req_ptr->next_hdr_prot, 0, 0);
  if(req_ptr->next_hdr_prot_valid)
  {
   switch(req_ptr->next_hdr_prot)
   {
    case PS_IPPROTO_TCP:
     next_hdr_prot = PS_IPPROTO_TCP;

     if (req_ptr->tcp_udp_src_valid)
     {
       ip_filter.next_prot_hdr.tcp.field_mask |= IPFLTR_MASK_TCP_SRC_PORT;
       ip_filter.next_prot_hdr.tcp.src.port = req_ptr->tcp_udp_src.port;
       ip_filter.next_prot_hdr.tcp.src.range = req_ptr->tcp_udp_src.range;
       LOG_MSG_INFO1("TCP src port %d range %d",
                      req_ptr->tcp_udp_src.port,
                      req_ptr->tcp_udp_src.range, 0);
     }
     if(req_ptr->tcp_udp_dst_valid)
     {
       ip_filter.next_prot_hdr.tcp.field_mask |= IPFLTR_MASK_TCP_DST_PORT;
       ip_filter.next_prot_hdr.tcp.dst.port = req_ptr->tcp_udp_dst.port;
       ip_filter.next_prot_hdr.tcp.dst.range = req_ptr->tcp_udp_dst.range;
       LOG_MSG_INFO1("TCP dst port %d range %d",
                        req_ptr->tcp_udp_dst.port,
                        req_ptr->tcp_udp_dst.range, 0);
     }
        break;

     case PS_IPPROTO_UDP:
      next_hdr_prot = PS_IPPROTO_UDP;

      if (req_ptr->tcp_udp_src_valid)
      {
        ip_filter.next_prot_hdr.udp.field_mask |= IPFLTR_MASK_UDP_SRC_PORT;
        ip_filter.next_prot_hdr.udp.src.port = req_ptr->tcp_udp_src.port;
        ip_filter.next_prot_hdr.udp.src.range = req_ptr->tcp_udp_src.range;
        LOG_MSG_INFO1("UDP src port %d range %d",
                       req_ptr->tcp_udp_src.port, req_ptr->tcp_udp_src.range, 0);
      }

      if(req_ptr->tcp_udp_dst_valid)
      {
        ip_filter.next_prot_hdr.udp.field_mask |= IPFLTR_MASK_UDP_DST_PORT;
        ip_filter.next_prot_hdr.udp.dst.port = req_ptr->tcp_udp_dst.port;
        ip_filter.next_prot_hdr.udp.dst.range = req_ptr->tcp_udp_dst.range;
        LOG_MSG_INFO1("UDP dst port %d range %d",
                       req_ptr->tcp_udp_dst.port, req_ptr->tcp_udp_dst.range, 0);
        }

       break;

      case PS_IPPROTO_ICMP:
      case PS_IPPROTO_ICMP6:
      if(req_ptr->next_hdr_prot == PS_IPPROTO_ICMP)
      {
        next_hdr_prot = PS_IPPROTO_ICMP;
      }
      else
      {
        next_hdr_prot = PS_IPPROTO_ICMP6;
      }
      if (req_ptr->icmp_type_valid)
      {
        ip_filter.next_prot_hdr.icmp.field_mask |=
                                                IPFLTR_MASK_ICMP_MSG_TYPE;
        ip_filter.next_prot_hdr.icmp.type = req_ptr->icmp_type;
        LOG_MSG_INFO1("ICMP type %d ",
                       ip_filter.next_prot_hdr.icmp.type , 0, 0);
      }
      if (req_ptr->icmp_code_valid)
      {
         ip_filter.next_prot_hdr.icmp.field_mask |=
                                                 IPFLTR_MASK_ICMP_MSG_CODE;
         ip_filter.next_prot_hdr.icmp.code = req_ptr->icmp_code;
         LOG_MSG_INFO1("ICMP code %d ",
                        ip_filter.next_prot_hdr.icmp.code, 0, 0);
       }
        break;
      case PS_IPPROTO_ESP:
        next_hdr_prot = PS_IPPROTO_ESP;
        if (req_ptr->esp_spi_valid)
        {
          ip_filter.next_prot_hdr.esp.field_mask |= IPFLTR_MASK_ESP_SPI;
          ip_filter.next_prot_hdr.esp.spi = req_ptr->esp_spi;
          LOG_MSG_INFO1("ESP SPI %d  ",req_ptr->esp_spi, 0, 0);
        }
         break;
      case PS_IPPROTO_TCP_UDP:
        next_hdr_prot = PS_IPPROTO_TCP_UDP;

        if (req_ptr->tcp_udp_src_valid)
        {
          ip_filter.next_prot_hdr.tcp_udp_port_range.field_mask |=
                                               IPFLTR_MASK_TCP_UDP_SRC_PORT;
          ip_filter.next_prot_hdr.tcp_udp_port_range.src.port =
                                     req_ptr->tcp_udp_src.port;
          ip_filter.next_prot_hdr.tcp_udp_port_range.src.range =
                                                req_ptr->tcp_udp_src.range;
          LOG_MSG_INFO1("TCP_UDP src port %d range %d",
                         req_ptr->tcp_udp_src.port, req_ptr->tcp_udp_src.range, 0);
        }

        if(req_ptr->tcp_udp_dst_valid)
        {
          ip_filter.next_prot_hdr.tcp_udp_port_range.field_mask |=
                                                  IPFLTR_MASK_TCP_UDP_DST_PORT;
          ip_filter.next_prot_hdr.tcp_udp_port_range.dst.port =
                                       req_ptr->tcp_udp_dst.port;
          ip_filter.next_prot_hdr.tcp_udp_port_range.dst.range =
                                                req_ptr->tcp_udp_dst.range;
          LOG_MSG_INFO1("TCP_UDP dst port %d range %d",
                        req_ptr->tcp_udp_dst.port,
                        req_ptr->tcp_udp_dst.range, 0);
        }
          break;
      default:
        LOG_MSG_ERROR("Unsupported next header protocol %d",
                       req_ptr->next_hdr_prot , 0, 0);
        mcm_err_num = MCM_ERROR_INVALID_ARG_V01;
        goto firewall_add_error;
      }
   }

#ifdef TARGET_IS_9615
  memset(&extd_firewall_conf, 0, sizeof(qcmap_cm_extd_firewall_conf_t));
  extd_firewall_conf.extd_firewall_entry.filter_spec = ip_filter;
  ret = QcMapMgr->AddExtdFireWallEntry(&extd_firewall_conf, &mcm_err_num);

  if (ret)
#else /* TARGET_IS_9615 */
  memset(&firewall_entry, 0, sizeof(qcmap_msgr_firewall_entry_conf_t));
  firewall_entry.extd_firewall_entry.filter_spec = ip_filter;
  ret = QcMapClient->AddFireWallEntry(&firewall_entry,&qmi_err_num );
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];
  if ( ret || (mcm_err_num == MCM_ERROR_INTERFACE_NOT_FOUND_V01))
#endif /* !(TARGET_IS_9615) */
  {
    LOG_MSG_INFO1("Added Firewall entry successfully.\n",0,0,0);
    memset(&resp_msg, 0, sizeof(mcm_mobileap_add_firewall_entry_resp_msg_v01));
    resp_msg.firewall_handle_valid=true;
#ifdef TARGET_IS_9615
    resp_msg.firewall_handle =
             extd_firewall_conf.extd_firewall_entry.firewall_handle;
#else /* TARGET_IS_9615 */
    resp_msg.firewall_handle =
             firewall_entry.extd_firewall_entry.firewall_handle;
#endif /* !(TARGET_IS_9615) */
  }
  else
  {
  //CHECK CHECK CHECK CHECK CHECK////////////////////////////////////////////////
    firewall_add_error:
    LOG_MSG_ERROR("Add Firewall entry failed 0x%x!!", mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;

    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
        sizeof(mcm_mobileap_add_firewall_entry_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
      sizeof(mcm_mobileap_add_firewall_entry_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_GET_NAT_TIMEOUT()

  DESCRIPTION
    Get NAT Timeout value of a particular type on Apps.

  PARAMETERS
    sp:          MCM Mobileap service instance state pointer for this qmi link
    cmd_buf_p:   Message Headerbil
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/

static qmi_csi_cb_error mcm_mobileap_svc_hdlr_set_firewall_config
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  mcm_mobileap_svc_state_info_type    *qcmap_sp;
  mcm_mobileap_set_firewall_config_req_msg_v01 *req_ptr;
  mcm_mobileap_set_firewall_config_resp_msg_v01 resp_msg;
  mcm_mobileap_svc_handle_type  *map_handle;

  unsigned int index;
  boolean enable_firewall, pkts_allowed = false;
  boolean ret;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
#ifndef TARGET_IS_9615
    qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
#endif /* !(TARGET_IS_9615) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type *)service_handle;
  req_ptr = (mcm_mobileap_set_firewall_config_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p set firewall config", clnt_info,0,0);

  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
      (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
    req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_set_firewall_config_resp_msg_v01));
   return QMI_CSI_CB_NO_ERR;
  }

  memset(&resp_msg, 0, sizeof(mcm_mobileap_set_firewall_config_resp_msg_v01));

  /*---------------------------------------------------------------------

  ---------------------------------------------------------------------*/
  enable_firewall = req_ptr->firewall_enabled;
  if( req_ptr->pkts_allowed_valid )
  {
   pkts_allowed = req_ptr->pkts_allowed;
  }

#ifdef TARGET_IS_9615
  ret = QcMapMgr->SetFirewall( enable_firewall, pkts_allowed, &mcm_err_num);
#else /* TARGET_IS_9615 */
  ret = QcMapClient->SetFirewall(enable_firewall, pkts_allowed, &qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];
#endif /* !(TARGET_IS_9615) */
  if ( ret )
  {
    LOG_MSG_INFO1("Set firewall successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Set firewall config failed: 0x%x!!", mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_set_firewall_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_set_firewall_config_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}




/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_GET_NAT_TIMEOUT()

  DESCRIPTION
    Get NAT Timeout value of a particular type on Apps.

  PARAMETERS
    sp:          MCM Mobileap service instance state pointer for this qmi link
    cmd_buf_p:   Message Headerbil
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error mcm_mobileap_svc_hdlr_get_nat_timeout
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  mcm_mobileap_svc_state_info_type *qcmap_sp;
  mcm_mobileap_get_nat_timeout_req_msg_v01 *req_ptr;
  mcm_mobileap_get_nat_timeout_resp_msg_v01  resp_msg;
  mcm_mobileap_svc_handle_type  *map_handle;

  unsigned int index;
  uint32_t timeout= 0;
  boolean ret;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
#ifndef TARGET_IS_9615
    qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_nat_timeout_enum_v01 timeout_type;
#endif /* !(TARGET_IS_9615) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    ds_assert(req_c_struct != NULL);
    ds_assert(req_c_struct_len > 0);
    ds_assert(clnt_info != NULL);
    ds_assert(service_handle != NULL);

    qcmap_sp = (mcm_mobileap_svc_state_info_type*)service_handle;
    req_ptr = (mcm_mobileap_get_nat_timeout_req_msg_v01*)req_c_struct;

    LOG_MSG_INFO1 ("Client %p get nat timeout", clnt_info,0,0);

    index = req_ptr->mcm_mobileap_handle;
    if (index != MCM_MOBILEAP_SVC_HANDLE ||
        (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
    {
      LOG_MSG_ERROR("Incorrect handle %d passed",
                    req_ptr->mcm_mobileap_handle,0,0);
      resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
      resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
      qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                        sizeof(mcm_mobileap_get_nat_timeout_resp_msg_v01));
      return QMI_CSI_CB_NO_ERR;
    }
#ifdef TARGET_IS_9615
    if (req_ptr->timeout_type != MCM_MOBILEAP_NAT_TIMEOUT_GENERIC_V01)
    {
      LOG_MSG_ERROR("Timeout Type not supported,", req_ptr->timeout_type,0,0);
      resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
      resp_msg.resp.error = MCM_ERROR_INVALID_ARG_V01;
      qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                        sizeof(mcm_mobileap_get_nat_timeout_resp_msg_v01));
      return QMI_CSI_CB_NO_ERR;
    }
#else /* TARGET_IS_9615 */
    if ((req_ptr->timeout_type < MCM_MOBILEAP_NAT_TIMEOUT_GENERIC_V01) ||
        (req_ptr->timeout_type > MCM_MOBILEAP_NAT_TIMEOUT_UDP_V01))
    {
      LOG_MSG_ERROR("Timeout Type not supported,", req_ptr->timeout_type,0,0);
      resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
      resp_msg.resp.error = MCM_ERROR_INVALID_ARG_V01;
      qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                        sizeof(mcm_mobileap_get_nat_timeout_resp_msg_v01));
      return QMI_CSI_CB_NO_ERR;
    }
#endif /* !(TARGET_IS_9615) */
    memset(&resp_msg, 0, sizeof(mcm_mobileap_get_nat_timeout_resp_msg_v01));


  /*---------------------------------------------------------------------
   Get NAT Timeout.
  ---------------------------------------------------------------------*/

#ifdef TARGET_IS_9615
  ret = QcMapMgr->GetNATEntryTimeout((uint16_t *)&timeout, &mcm_err_num);
#else /* TARGET_IS_9615 */
  ret = QcMapClient->GetNatTimeout(
                    (qcmap_msgr_nat_timeout_enum_v01 )(req_ptr->timeout_type),
                     &timeout, &qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];
#endif /* !(TARGET_IS_9615) */
  if ( ret )
  {
    LOG_MSG_INFO1("Got NAT timeout value successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Get NAT timeout failed 0x%x!", mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_get_nat_timeout_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  resp_msg.timeout_value_valid = TRUE;
  resp_msg.timeout_value = timeout;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_get_nat_timeout_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* mcm_mobileap_svc_hdlr_get_nat_entry_timeout */

/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_SET_NAT_TIMEOUT()

  DESCRIPTION
    Set NAT Timeout value on Apps.

  PARAMETERS
    sp:          MCM Mobileap service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error mcm_mobileap_svc_hdlr_set_nat_timeout
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  mcm_mobileap_svc_state_info_type    *qcmap_sp;
  unsigned int     index;
  mcm_mobileap_svc_handle_type    *map_handle;
  mcm_mobileap_set_nat_timeout_req_msg_v01 *req_ptr;
  mcm_mobileap_set_nat_timeout_resp_msg_v01 resp_msg;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
  boolean ret;
#ifndef TARGET_IS_9615
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
#endif /* !(TARGET_IS_9615) */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type*)service_handle;
  req_ptr = (mcm_mobileap_set_nat_timeout_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1("\nClient %p set nat timeout: index: %d map_handle: %d \n"
                 , clnt_info,req_ptr->mcm_mobileap_handle, mcm_mobileap_svc_handle.handle);

  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
     (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_set_nat_timeout_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  memset(&resp_msg, 0, sizeof(mcm_mobileap_set_nat_timeout_resp_msg_v01));

  /*---------------------------------------------------------------------
   Set NAT Timeout.
  ---------------------------------------------------------------------*/
#ifdef TARGET_IS_9615
  ret = QcMapMgr->SetNATEntryTimeout((uint16_t)req_ptr->timeout_value, &mcm_err_num);
#else /* TARGET_IS_9615 */
  ret = QcMapClient->SetNatTimeout(
                     (qcmap_msgr_nat_timeout_enum_v01 )req_ptr->timeout_type,
                     (uint32_t)req_ptr->timeout_value, &qmi_err_num );
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];
#endif /* !(TARGET_IS_9615) */
  if ( ret )
  {
    LOG_MSG_INFO1("Set NAT Timeout successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Set NAT Timeout failed 0x%x!!", mcm_err_num, 0, 0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_set_nat_timeout_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_set_nat_timeout_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* mcm_mobileap_svc_hdlr_set_nat_timeout */


/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_GET_ROAMING()

  DESCRIPTION
    Gets Roaming flag value on Apps.

  PARAMETERS
    sp:          MCM Mobileap service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error mcm_mobileap_svc_hdlr_get_roaming
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  mcm_mobileap_svc_state_info_type    *qcmap_sp;

  mcm_mobileap_get_roaming_pref_req_msg_v01 *req_ptr;
  mcm_mobileap_get_roaming_pref_resp_msg_v01 resp_msg;
  unsigned int   index;
  mcm_mobileap_svc_handle_type	 *map_handle;

  boolean    auto_connect_flag;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;

#ifndef TARGET_IS_9615
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
#endif /* !(TARGET_IS_9615) */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type *)service_handle;
  req_ptr = (mcm_mobileap_get_roaming_pref_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p get roaming flag", clnt_info,0,0);

  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
     (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_get_roaming_pref_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  memset(&resp_msg, 0, sizeof(mcm_mobileap_get_roaming_pref_resp_msg_v01));

  /*---------------------------------------------------------------------
       Get Roaming flag
  ---------------------------------------------------------------------*/
  resp_msg.allow_wwan_calls_while_roaming_valid = TRUE;
#ifdef TARGET_IS_9615
  resp_msg.allow_wwan_calls_while_roaming = QcMapMgr->GetRoaming(&mcm_err_num);
#else /* TARGET_IS_9615 */
  QcMapClient->GetRoaming((boolean *)&resp_msg.allow_wwan_calls_while_roaming,
                          &qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];
#endif /* !(TARGET_IS_9615) */
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_get_roaming_pref_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_SET_ROAMING()

  DESCRIPTION
    Set roaming flag value on Apps.

  PARAMETERS
    sp:          MCM Mobileap service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error mcm_mobileap_svc_hdlr_set_roaming
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  mcm_mobileap_svc_state_info_type    *qcmap_sp;

  mcm_mobileap_set_roaming_pref_req_msg_v01 *req_ptr;
  mcm_mobileap_set_roaming_pref_resp_msg_v01 resp_msg;
  uint8_t    roaming_flag=0;
  unsigned int     index;
  mcm_mobileap_svc_handle_type    *map_handle;
  boolean ret;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
#ifndef TARGET_IS_9615
    qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
#endif /* !(TARGET_IS_9615) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type*)service_handle;
  req_ptr = (mcm_mobileap_set_roaming_pref_req_msg_v01*)req_c_struct;

  LOG_MSG_INFO1 ("Client %p set roaming flag", clnt_info,0,0);

  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
     (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_set_roaming_pref_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  /*---------------------------------------------------------------------

  ---------------------------------------------------------------------*/

  memset(&resp_msg, 0, sizeof(mcm_mobileap_set_roaming_pref_resp_msg_v01));

#ifdef TARGET_IS_9615
  ret = QcMapMgr->SetRoaming(req_ptr->allow_wwan_calls_while_roaming, &mcm_err_num);
#else /* TARGET_IS_9615 */
  ret = QcMapClient->SetRoaming((boolean)req_ptr->allow_wwan_calls_while_roaming,
                                &qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];
#endif /* !(TARGET_IS_9615) */
  if ( ret )
  {
    LOG_MSG_INFO1("Set Roaming flag successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Set Roaming flag failed 0x%x!!",mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_set_roaming_pref_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_set_roaming_pref_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}



/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_ADD_STATIC_NAT_ENTRY()

  DESCRIPTION
    Add Static NAT entry on Apps.

  PARAMETERS
    sp:          MCM Mobileap service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_add_static_nat_entry
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  mcm_mobileap_svc_state_info_type    *qcmap_sp;

  mcm_mobileap_add_static_nat_entry_req_msg_v01 *req_ptr;
  mcm_mobileap_add_static_nat_entry_resp_msg_v01 resp_msg;
  mcm_mobileap_svc_handle_type    *map_handle;

  unsigned int     index;
  boolean ret;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
#ifdef TARGET_IS_9615
  qcmap_cm_port_fwding_entry_conf_t nat_entry;
#else /* TARGET_IS_9615 */
  qcmap_msgr_snat_entry_config_v01 snat_entry;
    qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
#endif /* !(TARGET_IS_9615) */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type*)service_handle;
  req_ptr = (mcm_mobileap_add_static_nat_entry_req_msg_v01*)req_c_struct;

  LOG_MSG_INFO1 ("Client %p add static nat entry", clnt_info,0,0);

  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
      (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_add_static_nat_entry_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }


  memset(&resp_msg, 0, sizeof(mcm_mobileap_add_static_nat_entry_resp_msg_v01));
#ifdef TARGET_IS_9615
  nat_entry.port_fwding_private_ip = req_ptr->nat_entry_config.port_fwding_private_ip;
  nat_entry.port_fwding_private_port = req_ptr->nat_entry_config.port_fwding_private_port;
  nat_entry.port_fwding_global_port = req_ptr->nat_entry_config.port_fwding_global_port;
  nat_entry.port_fwding_protocol = req_ptr->nat_entry_config.port_fwding_protocol;
  ret = QcMapMgr->AddStaticNatEntry(&nat_entry, &mcm_err_num);
  if (ret)
#else /* TARGET_IS_9615 */
  snat_entry.private_ip_addr = req_ptr->nat_entry_config.port_fwding_private_ip;
  snat_entry.private_port = req_ptr->nat_entry_config.port_fwding_private_port;
  snat_entry.global_port = req_ptr->nat_entry_config.port_fwding_global_port;
  snat_entry.protocol = req_ptr->nat_entry_config.port_fwding_protocol;
  ret = QcMapClient->AddStaticNatEntry(&snat_entry, &qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];
  if ( ret || (mcm_err_num == MCM_ERROR_INTERFACE_NOT_FOUND_V01))
#endif /* !(TARGET_IS_9615) */
  {
    LOG_MSG_INFO1("Added SNAT entry successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Static NAT entry add failed 0x%x!!",mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_add_static_nat_entry_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_add_static_nat_entry_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* mcm_mobileap_svc_hdlr_add_static_nat_entry() */


/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_DELETE_STATIC_NAT_ENTRY()

  DESCRIPTION
    Delete Static NAT entry on Apps.

  PARAMETERS
    sp:          MCM Mobileap service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_delete_static_nat_entry
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  mcm_mobileap_svc_state_info_type    *qcmap_sp;

  mcm_mobileap_delete_static_nat_entry_req_msg_v01 *req_ptr;
  mcm_mobileap_delete_static_nat_entry_resp_msg_v01 resp_msg;
  mcm_mobileap_svc_handle_type    *map_handle;
  unsigned int     index;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
  boolean ret;
#ifdef TARGET_IS_9615
  qcmap_cm_port_fwding_entry_conf_t nat_entry;
#else /* TARGET_IS_9615 */
  qcmap_msgr_snat_entry_config_v01 snat_entry;
    qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
#endif /* !(TARGET_IS_9615) */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type *)service_handle;
  req_ptr = (mcm_mobileap_delete_static_nat_entry_req_msg_v01*)req_c_struct;

  LOG_MSG_INFO1 ("Client %p deleting static NAT entry", clnt_info,0,0);

  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
     (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_delete_static_nat_entry_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  memset(&resp_msg, 0, sizeof(mcm_mobileap_delete_static_nat_entry_resp_msg_v01));

  /*---------------------------------------------------------------------
   Delete Static NAT Entry
  ---------------------------------------------------------------------*/
#ifdef TARGET_IS_9615
  nat_entry.port_fwding_private_ip = req_ptr->snat_entry.port_fwding_private_ip;
  nat_entry.port_fwding_private_port = req_ptr->snat_entry.port_fwding_private_port;
  nat_entry.port_fwding_global_port = req_ptr->snat_entry.port_fwding_global_port;
  nat_entry.port_fwding_protocol = req_ptr->snat_entry.port_fwding_protocol;
  ret = QcMapMgr->DeleteStaticNatEntry(&nat_entry, &mcm_err_num);
  if (ret)
#else /* TARGET_IS_9615 */
  snat_entry.private_ip_addr = req_ptr->snat_entry.port_fwding_private_ip;
  snat_entry.private_port = req_ptr->snat_entry.port_fwding_private_port;
  snat_entry.global_port = req_ptr->snat_entry.port_fwding_global_port;
  snat_entry.protocol = req_ptr->snat_entry.port_fwding_protocol;
  ret = QcMapClient->DeleteStaticNatEntry(&snat_entry, &qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];

  if ( ret || (mcm_err_num == MCM_ERROR_INTERFACE_NOT_FOUND_V01))
#endif /* !(TARGET_IS_9615) */
  {
    LOG_MSG_INFO1("Deleted SNAT entry successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Static NAT entry delete failed 0x%x!!", mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_delete_static_nat_entry_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_delete_static_nat_entry_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* mcm_mobileap-svc_hdlr_delete_static_nat_entry() */




/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_STATIC_NAT_ENTRY()

  DESCRIPTION
    Get Static NAT entries on Apps.

  PARAMETERS
    sp:          QMI_QCMAP MSGR's service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_get_static_nat_entry
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  mcm_mobileap_svc_state_info_type    *qcmap_sp;

  mcm_mobileap_get_static_nat_entry_req_msg_v01 *req_ptr;
  mcm_mobileap_get_static_nat_entry_resp_msg_v01 resp_msg;
  mcm_mobileap_svc_handle_type    *map_handle;

  unsigned int     index;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
  boolean ret;
#ifdef TARGET_IS_9615
  qcmap_cm_port_fwding_entry_conf_t nat_entry;
#else /* TARGET_IS_9615 */
  qcmap_msgr_snat_entry_config_v01 snat_config;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
#endif /* !(TARGET_IS_9615) */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type *)service_handle;
  req_ptr = (mcm_mobileap_get_static_nat_entry_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p get static NAT entries", clnt_info,0,0);

  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
      (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                   req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_get_static_nat_entry_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  memset(&resp_msg, 0, sizeof(mcm_mobileap_get_static_nat_entry_resp_msg_v01));

  /*---------------------------------------------------------------------
   Get Static NAT Entries
  ---------------------------------------------------------------------*/
#ifdef TARGET_IS_9615
  resp_msg.snat_entries_len = QcMapMgr->GetStaticNatEntry(
                 (qcmap_cm_port_fwding_entry_conf_t *)resp_msg.snat_entries,
                  req_ptr->max_entries, &mcm_err_num);
#else /* TARGET_IS_9615 */
  ret = QcMapClient->GetStaticNatConfig(
                  (qcmap_msgr_snat_entry_config_v01 *)resp_msg.snat_entries,
                   &(resp_msg.snat_entries_len), &qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];
  if (ret == FALSE)
  {
    LOG_MSG_ERROR("Get Static NAT Config failed: error %d",
                   mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_get_static_nat_entry_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
#endif /* !(TARGET_IS_9615) */

  if (resp_msg.snat_entries_len)
  {
    LOG_MSG_INFO1("Get SNAT entry succeeded.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("No Static NAT entries configured 0x%x",mcm_err_num,0,0);

    resp_msg.snat_entries_valid= false;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_get_static_nat_entry_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  resp_msg.snat_entries_valid = true;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_get_static_nat_entry_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_static_nat_entries() */


/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_DISABLE_STATION_MODE()

  DESCRIPTION
    Get Static NAT entries on Apps.

  PARAMETERS
    sp:          MCM Mobileap service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_disable_station_mode
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  mcm_mobileap_svc_state_info_type    *qcmap_sp;

  mcm_mobileap_station_mode_disable_req_msg_v01 *req_ptr;
  mcm_mobileap_station_mode_disable_resp_msg_v01 resp_msg;
  mcm_mobileap_svc_handle_type    *map_handle;
  boolean ret = FALSE;
  unsigned int     index;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
#ifndef TARGET_IS_9615
  qcmap_msgr_wlan_mode_enum_v01 wlan_mode;
  qcmap_msgr_access_profile_v01 guest_ap_acess_profile;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  boolean supplicant_status;
#endif /* !(TARGET_IS_9615) */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type *)service_handle;
  req_ptr = (mcm_mobileap_station_mode_disable_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p disable station mode", clnt_info,0,0);

  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
      (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_station_mode_disable_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  memset(&resp_msg, 0, sizeof(mcm_mobileap_station_mode_disable_resp_msg_v01));

  /*---------------------------------------------------------------------
   Get Static NAT Entries
  ---------------------------------------------------------------------*/
#ifdef TARGET_IS_9615
  ret = QcMapMgr->DisableStaMode(&mcm_err_num);
#else /* TARGET_IS_9615 */

  /* Disable the Station Mode based on the current status  */
  if (QcMapClient->GetWLANStatus(&wlan_mode, &qmi_err_num))
  {
    if (wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01)
    {
      supplicant_status = FALSE;
      ret = QcMapClient->SetSupplicantConfig(supplicant_status, &qmi_err_num);
      mcm_err_num = qmi_to_mcm_error[qmi_err_num];
    }
    else
    {
      mcm_err_num = MCM_ERROR_INVALID_STATE_V01;
    }
  }
  else
  {
    mcm_err_num = qmi_to_mcm_error[qmi_err_num];
  }
#endif /* !(TARGET_IS_9615) */
  if ( ret )
  {
    LOG_MSG_INFO1("Disable Station mode succeeded.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Disable Station Mode Failed 0x%x", mcm_err_num,0,0);
    resp_msg.resp.error = mcm_err_num;
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_station_mode_disable_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_station_mode_disable_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* mcm_mobileap_svc_hdlr_disable_station_mode() */


/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_DISABLE_STATION_MODE()

  DESCRIPTION
    Get Static NAT entries on Apps.

  PARAMETERS
    sp:          MCM Mobileap service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_enable_station_mode
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  mcm_mobileap_svc_state_info_type    *qcmap_sp;

  mcm_mobileap_station_mode_enable_req_msg_v01 *req_ptr;
  mcm_mobileap_station_mode_enable_resp_msg_v01 resp_msg;
  mcm_mobileap_svc_handle_type    *map_handle;
  boolean ret = FALSE;
  unsigned int     index;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
#ifndef TARGET_IS_9615
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_wlan_mode_enum_v01 wlan_config_mode, wlan_mode;
  qcmap_msgr_access_profile_v01 guest_ap_access_profile =
                              QCMAP_MSGR_ACCESS_PROFILE_MIN_ENUM_VAL_V01;
  qcmap_msgr_station_mode_config_v01 station_config;
  boolean supplicant_status;
#endif /* !(TARGET_IS_9615) */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type *)service_handle;
  req_ptr = (mcm_mobileap_station_mode_enable_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p enable station mode", clnt_info,0,0);

  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
     (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_station_mode_enable_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  memset(&resp_msg, 0, sizeof(mcm_mobileap_station_mode_enable_resp_msg_v01));

  /*---------------------------------------------------------------------
   Enable Station Mode.
  ---------------------------------------------------------------------*/
#ifdef TARGET_IS_9615
  ret = QcMapMgr->EnableStaMode((qcmap_sta_connection_config*)&req_ptr->cfg, &mcm_err_num);
#else /* TARGET_IS_9615 */
    /* Call SetWLANConfig and SetSupplicantConfig only when the wlan driver is in
        QCMAP_MSGR_WLAN_MODE_AP_STA_V01*/
  if (QcMapClient->GetWLANStatus(&wlan_mode, &qmi_err_num))
  {
    if (wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01)
    {
      /* GetWLANConfig to obtain the guest_ap_access_profile. */
      QcMapClient->GetWLANConfig(&wlan_config_mode, &guest_ap_access_profile,
                                 &station_config, &qmi_err_num);

      memcpy(&station_config, &(req_ptr->cfg),
             sizeof(qcmap_msgr_station_mode_config_v01));

      if (QcMapClient->SetWLANConfig(wlan_config_mode, guest_ap_access_profile,
                                     station_config, &qmi_err_num))
      {
        supplicant_status = TRUE;
        if (QcMapClient->SetSupplicantConfig(supplicant_status, &qmi_err_num))
        {
          LOG_MSG_INFO1("Enable Station mode succeeded.\n",0,0,0);
          ret = TRUE;
        }
      }
    }
    else
    {
      LOG_MSG_ERROR("Cannot set Station configuration. WLAN Mode is not set to "
                    "AP+STA mode.", 0,0,0);
      resp_msg.resp.error = MCM_ERROR_INVALID_STATE_V01;
      resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
      qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                        sizeof(mcm_mobileap_station_mode_enable_resp_msg_v01));
      return QMI_CSI_CB_NO_ERR;
    }
  }
#endif /* !(TARGET_IS_9615) */

  if (ret)
  {
    LOG_MSG_INFO1("Enable Station mode succeeded.\n",0,0,0);
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_station_mode_enable_resp_msg_v01));
  }
  else
  {
#ifndef TARGET_IS_9615
    mcm_err_num = qmi_to_mcm_error[qmi_err_num];
#endif /* !(TARGET_IS_9615) */
    LOG_MSG_ERROR("Enable Station Mode Failed 0x%x", mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_station_mode_enable_resp_msg_v01));
                      return QMI_CSI_CB_NO_ERR;
  }

  return QMI_CSI_CB_NO_ERR;
} /* mcm_mobileap_svc_hdlr_enable_station_mode() */




/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_ENABLE_WLAN()

  DESCRIPTION
    Enable WLAN functionality on Apps.

  PARAMETERS
    sp:          MCM Mobileap service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_enable_wlan
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  mcm_mobileap_svc_state_info_type    *qcmap_sp;
  mcm_mobileap_svc_handle_type    *map_handle;
  mcm_mobileap_enable_wlan_req_msg_v01    *req_ptr;
  mcm_mobileap_enable_wlan_resp_msg_v01   resp_msg;
  boolean ret;
  unsigned int     index;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
#ifndef TARGET_IS_9615
  boolean supplicant_status;

  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_wlan_mode_enum_v01 wlan_mode;
#endif /* !(TARGET_IS_9615) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type *)service_handle;
  req_ptr = (mcm_mobileap_enable_wlan_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p enable wlan", clnt_info,0,0);

  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
     (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_enable_wlan_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }


  memset(&resp_msg, 0, sizeof(mcm_mobileap_enable_wlan_resp_msg_v01));


  /*---------------------------------------------------------------------
    Enable WLAN
  ---------------------------------------------------------------------*/
#ifdef TARGET_IS_9615
  ret = QcMapMgr->EnableWLAN(&mcm_err_num);
#else /* TARGET_IS_9615 */
  ret = QcMapClient->EnableWLAN(&qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];

  /* If the WLAN Status is QCMAP_MSGR_WLAN_MODE_AP_STA_V01, kill the wpa_supplicant
   as they need to be enabled only when Station Mode enable is invoked.*/
  if (ret && QcMapClient->GetWLANStatus(&wlan_mode, &qmi_err_num))
  {
    if (wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01)
    {
      supplicant_status = FALSE;
      ret = QcMapClient->SetSupplicantConfig(supplicant_status, &qmi_err_num);
      mcm_err_num = qmi_to_mcm_error[qmi_err_num];
    }
  }
#endif /* !(TARGET_IS_9615) */
  if ( ret )
  {
    LOG_MSG_INFO1("Enable WLAN succeeds.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Enable WLAN fails. 0x%x\n", mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_enable_wlan_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_enable_wlan_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* mcm_mobileap_svc_hdlr_enable_wlan() */

/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_DISABLE_WLAN()

  DESCRIPTION
    Enable WLAN functionality on Apps.

  PARAMETERS
    sp:          MCM Mobileap service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_disable_wlan
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  mcm_mobileap_svc_state_info_type    *qcmap_sp;
  mcm_mobileap_svc_handle_type    *map_handle;
  mcm_mobileap_disable_wlan_req_msg_v01    *req_ptr;
  mcm_mobileap_disable_wlan_resp_msg_v01   resp_msg;

  unsigned int     index;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
  boolean ret;
#ifndef TARGET_IS_9615
    qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
#endif /* !(TARGET_IS_9615) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type *)service_handle;
  req_ptr = (mcm_mobileap_disable_wlan_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p disable wlan", clnt_info,0,0);

  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
     (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_disable_wlan_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }


  memset(&resp_msg, 0, sizeof(mcm_mobileap_disable_wlan_resp_msg_v01));


  /*---------------------------------------------------------------------
    Enable WLAN
  ---------------------------------------------------------------------*/
#ifdef TARGET_IS_9615
  ret = QcMapMgr->DisableWLAN(&mcm_err_num);
#else /* TARGET_IS_9615 */
  ret = QcMapClient->DisableWLAN(&qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];
#endif /* !(TARGET_IS_9615) */
  if ( ret )
  {
    LOG_MSG_INFO1("Disable WLAN succeeds.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Disable WLAN fails. 0x%x\n", mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_disable_wlan_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_disable_wlan_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* mcm_mobileap_svc_hdlr_disable_wlan() */



/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_SET_DHCP_CONFIG()

  DESCRIPTION
    Enable WLAN functionality on Apps.

  PARAMETERS
    sp:          MCM Mobileap service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_set_dhcpd_config
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  mcm_mobileap_svc_state_info_type    *qcmap_sp;
  mcm_mobileap_svc_handle_type    *map_handle;
  mcm_mobileap_set_dhcpd_config_req_msg_v01    *req_ptr;
  mcm_mobileap_set_dhcpd_config_resp_msg_v01   resp_msg;
  boolean ret;
  unsigned int     index;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
#ifndef TARGET_IS_9615
  qcmap_msgr_lan_config_v01 lan_config, get_lan_config;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  char *endptr;
  uint32_t val, time_in_secs = 0;
#endif /* !(TARGET_IS_9615) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type *)service_handle;
  req_ptr = (mcm_mobileap_set_dhcpd_config_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p set dhcpd config", clnt_info,0,0);

  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
     (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_set_dhcpd_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }


  memset(&resp_msg, 0, sizeof(mcm_mobileap_set_dhcpd_config_resp_msg_v01));


  /*---------------------------------------------------------------------
    Set DHCP Config
  ---------------------------------------------------------------------*/
#ifdef TARGET_IS_9615
  ret = QcMapMgr->SetDHCPDConfig(req_ptr->dhcpd_config.intf,
                               req_ptr->dhcpd_config.start,
                               req_ptr->dhcpd_config.end,
                               req_ptr->dhcpd_config.leasetime,
                               &mcm_err_num);
#else /* TARGET_IS_9615 */
  val = strtol(req_ptr->dhcpd_config.leasetime, &endptr, 10);
  if ((*endptr == '\0') || (*endptr == '\n') || (*endptr == '\r'))
  {
    time_in_secs = val;
  }
  else if ((*endptr == 'h') || (*endptr == 'H'))
  {
    time_in_secs = val * 3600;
  }
  else if ((*endptr == 'm') || (*endptr == 'M'))
  {
    time_in_secs = val * 60;
  }

  if (time_in_secs < 120)
  {
    LOG_MSG_ERROR("Set DHCPD Config fails. Lease time should be greater "
                  "than or equal to 120 Secs", 0,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_ARG_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_set_dhcpd_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  /*  Need to fill the gateway IP and subnet mask.*/
  /* Do a GetLan Config to obtain the IP address and Mask */
  ret = QcMapClient->GetLANConfig(&get_lan_config, &qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];
  if (!ret)
  {
    LOG_MSG_ERROR("Obtaining LAN Configuration fails. 0x%x\n", mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_set_dhcpd_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  lan_config.gw_ip = get_lan_config.gw_ip;
  lan_config.netmask = get_lan_config.netmask;
  lan_config.enable_dhcp = TRUE;
  lan_config.dhcp_config.dhcp_start_ip = req_ptr->dhcpd_config.start;
  lan_config.dhcp_config.dhcp_end_ip = req_ptr->dhcpd_config.end;
  lan_config.dhcp_config.lease_time = time_in_secs;
  ret = QcMapClient->SetLANConfig(lan_config, &qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];
  if (ret)
  {
    LOG_MSG_INFO1("Setting of DHCPD Configuration succeeds.\n",0,0,0);
    if (!QcMapClient->ActivateLAN(&qmi_err_num))
    {
      mcm_err_num = qmi_to_mcm_error[qmi_err_num];
      LOG_MSG_ERROR("Activate LAN Config fails. 0x%x\n", mcm_err_num,0,0);
      resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
      resp_msg.resp.error = mcm_err_num;
      qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_set_dhcpd_config_resp_msg_v01));
      return QMI_CSI_CB_NO_ERR;
    }
  }
  else
  {
    LOG_MSG_ERROR("Set DHCPD Config fails. 0x%x\n", mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_set_dhcpd_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
#endif /* !(TARGET_IS_9615) */
  if (ret)
  {
    LOG_MSG_INFO1("Set DHCPD Config succeeds.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Set DHCPD Config fails. 0x%x\n", mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_set_dhcpd_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_set_dhcpd_config_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* mcm_mobileap_svc_hdlr_set_dhcpd_config() */




/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_HDLR_GET_WWAN_STATS()

  DESCRIPTION
    Get Autoconnect flag value on Apps.

  PARAMETERS
    sp:          MCM Mobilap service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error mcm_mobileap_svc_hdlr_get_wwan_stats
(
  mcm_mobileap_svc_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  mcm_mobileap_svc_state_info_type    *qcmap_sp;
  mcm_mobileap_get_wwan_stats_req_msg_v01  *req_ptr;
  mcm_mobileap_get_wwan_stats_resp_msg_v01  resp_msg;
  mcm_mobileap_svc_handle_type    *map_handle;
  unsigned int              index;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
  boolean ret;
#ifdef TARGET_IS_9615
  qcmap_cm_statistics_t   wwan_stats;
#else /* TARGET_IS_9615 */
  qcmap_msgr_wwan_statistics_type_v01 wwan_statistics;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
#endif /* !(TARGET_IS_9615) */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type *)service_handle;
  req_ptr = (mcm_mobileap_get_wwan_stats_req_msg_v01 *)req_c_struct;

  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
      (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_get_wwan_stats_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p get wwan stats in MobileAP %d", clnt_info,req_ptr->mcm_mobileap_handle,0);

  memset(&resp_msg, 0, sizeof(mcm_mobileap_get_wwan_stats_resp_msg_v01));
  /*---------------------------------------------------------------------
Get WWAN Statistics
  ---------------------------------------------------------------------*/
#ifdef TARGET_IS_9615
  ret = QcMapMgr->GetWWANStatistics((ip_version_enum_type)req_ptr->ip_family,
                                   &wwan_stats, &mcm_err_num);
  if ( ret )
    {
      resp_msg.wwan_stats.bytes_rx = wwan_stats.bytes_rx;
      resp_msg.wwan_stats.bytes_tx = wwan_stats.bytes_tx;
      resp_msg.wwan_stats.pkts_rx = wwan_stats.pkts_rx;
      resp_msg.wwan_stats.pkts_tx= wwan_stats.pkts_tx;
      resp_msg.wwan_stats.pkts_dropped_rx = wwan_stats.pkts_dropped_rx;
      resp_msg.wwan_stats.pkts_dropped_tx = wwan_stats.pkts_dropped_tx;
    }
#else /* TARGET_IS_9615 */
  ret = QcMapClient->GetWWANStatistics(
                 (qcmap_msgr_ip_family_enum_v01)req_ptr->ip_family,
                 &wwan_statistics, &qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];
  if ( ret )
  {
    resp_msg.wwan_stats.bytes_rx = wwan_statistics.bytes_rx;
    resp_msg.wwan_stats.bytes_tx = wwan_statistics.bytes_tx;
    resp_msg.wwan_stats.pkts_rx = wwan_statistics.pkts_rx;
    resp_msg.wwan_stats.pkts_tx= wwan_statistics.pkts_tx;
    resp_msg.wwan_stats.pkts_dropped_rx = wwan_statistics.pkts_dropped_rx;
    resp_msg.wwan_stats.pkts_dropped_tx = wwan_statistics.pkts_dropped_tx;
  }
#endif /* !(TARGET_IS_9615) */
  if ( ret )
  {
    LOG_MSG_INFO1("Get WWAN Stats succcessful \n",0,0,0);
    resp_msg.wwan_stats_valid = TRUE;
  }
    else
    {
      LOG_MSG_ERROR("Get WWAN Stats fails. 0x%x\n", mcm_err_num,0,0);
      resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
      resp_msg.resp.error = mcm_err_num;
      qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                        sizeof(mcm_mobileap_get_wwan_stats_resp_msg_v01));
      return QMI_CSI_CB_NO_ERR;
    }
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_get_wwan_stats_resp_msg_v01));

    return QMI_CSI_CB_NO_ERR;

  } /* mcm_mobileap_svc_hdlr_get_wwan_stats() */


  /*===========================================================================
    FUNCTION MCM_MOBILEAP_SVC_HDLR_RESET_WWAN_STATS()

    DESCRIPTION
      Get Autoconnect flag value on Apps.

    PARAMETERS
      sp:		   MCM Mobilap service instance state pointer for this qmi link
      cmd_buf_p:   Message Header
      cl_sp:	   Coresponding client state pointer
      sdu_in:	   input command data

    RETURN VALUE
      dsm * pointing to the response to be sent to host
      NULL if no response was generated or command was discarded

    DEPENDENCIES
      qmi_qcmap_msgr_init() must have been called

    SIDE EFFECTS
      None
  ===========================================================================*/
static qmi_csi_cb_error mcm_mobileap_svc_hdlr_reset_wwan_stats
(
  mcm_mobileap_svc_client_info_type *clnt_info,
  qmi_req_handle         req_handle,
  int                    msg_id,
  void                   *req_c_struct,
  int                    req_c_struct_len,
  void                   *service_handle
)
{

  mcm_mobileap_svc_state_info_type	*qcmap_sp;
  mcm_mobileap_reset_wwan_stats_req_msg_v01  *req_ptr;
  mcm_mobileap_reset_wwan_stats_resp_msg_v01  resp_msg;
  mcm_mobileap_svc_handle_type *map_handle;
  unsigned int index;
  boolean ret;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
#ifndef TARGET_IS_9615
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
#endif /* !(TARGET_IS_9615) */
  ip_version_enum_type ip_family;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type *)service_handle;
  req_ptr = (mcm_mobileap_reset_wwan_stats_req_msg_v01 *)req_c_struct;

  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
      (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_reset_wwan_stats_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p reset wwan stats in MobileAP %p", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(mcm_mobileap_reset_wwan_stats_resp_msg_v01));
  /*---------------------------------------------------------------------
   Get WWAN Statistics
   ---------------------------------------------------------------------*/
  if (req_ptr->ip_family == MCM_MOBILEAP_IP_FAMILY_V4_V01)
    ip_family = IP_V4;
  else if (req_ptr->ip_family == MCM_MOBILEAP_IP_FAMILY_V6_V01)
    ip_family = IP_V6;

#ifdef TARGET_IS_9615
  ret = QcMapMgr->ResetWWANStatistics( ip_family, &mcm_err_num);
#else /* TARGET_IS_9615 */
  ret = QcMapClient->ResetWWANStatistics((qcmap_msgr_ip_family_enum_v01)ip_family, &qmi_err_num);
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];
#endif /* !(TARGET_IS_9615) */
  if ( ret )
  {
    LOG_MSG_INFO1("Reset WWAN Stats succcessful \n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Reset WWAN Stats fails. 0x%x\n", mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_reset_wwan_stats_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_reset_wwan_stats_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* mcm_mobileap_svc_hdlr_get_wwan_stats() */



  /*===========================================================================
    FUNCTION MCM_MOBILEAP_SVC_HDLR_GET_IPV4_WWAN_CONFIG()

    DESCRIPTION
      Get Autoconnect flag value on Apps.

    PARAMETERS
      sp:		   MCM Mobilap service instance state pointer for this qmi link
      cmd_buf_p:   Message Header
      cl_sp:	   Coresponding client state pointer
      sdu_in:	   input command data

    RETURN VALUE
      dsm * pointing to the response to be sent to host
      NULL if no response was generated or command was discarded

    DEPENDENCIES
      qmi_qcmap_msgr_init() must have been called

    SIDE EFFECTS
      None
  ===========================================================================*/
static qmi_csi_cb_error mcm_mobileap_svc_hdlr_get_ipv4_wwan_config
(
  mcm_mobileap_svc_client_info_type  *clnt_info,
  qmi_req_handle          req_handle,
  int                     msg_id,
  void                   *req_c_struct,
  int                     req_c_struct_len,
  void                   *service_handle
)
{

  mcm_mobileap_svc_state_info_type *qcmap_sp;
  mcm_mobileap_get_ipv4_wwan_config_req_msg_v01  *req_ptr;
  mcm_mobileap_get_ipv4_wwan_config_resp_msg_v01  resp_msg;
  mcm_mobileap_svc_handle_type *map_handle;
  unsigned int index;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
  boolean ret;
#ifndef TARGET_IS_9615
  qcmap_msgr_ip_family_enum_v01 ip_family;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_nw_params_t qcmap_nw_params;
  ip_family = QCMAP_MSGR_IP_FAMILY_V4_V01;
#endif /* !(TARGET_IS_9615) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type *)service_handle;
  req_ptr = (mcm_mobileap_get_ipv4_wwan_config_req_msg_v01 *)req_c_struct;

  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
      (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_get_ipv4_wwan_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p get ipv4 wwan config in MobileAP %p", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(mcm_mobileap_get_ipv4_wwan_config_resp_msg_v01));
  /*---------------------------------------------------------------------
   Get IPV4 WWAN Network Configuration
   ---------------------------------------------------------------------*/
#ifdef TARGET_IS_9615
  ret = QcMapMgr->GetIPv4WWANNetworkConfiguration((uint32_t *)&resp_msg.v4_addr,
                                                (uint32_t *)&resp_msg.v4_prim_dns_addr,
                                                (uint32_t *)&resp_msg.v4_sec_dns_addr,
                                                &mcm_err_num);
#else /* TARGET_IS_9615 */
  ip_family = QCMAP_MSGR_IP_FAMILY_V4_V01;
  ret = QcMapClient->GetNetworkConfiguration(ip_family,
                                       &qcmap_nw_params, &qmi_err_num );
  mcm_err_num = qmi_to_mcm_error[qmi_err_num];
  if ( ret )
  {
    resp_msg.v4_addr = qcmap_nw_params.v4_conf.public_ip.s_addr;
    resp_msg.v4_prim_dns_addr = qcmap_nw_params.v4_conf.primary_dns.s_addr;
    resp_msg.v4_sec_dns_addr = qcmap_nw_params.v4_conf.secondary_dns.s_addr;
  }
#endif /* !(TARGET_IS_9615) */
  if ( ret )
  {
    LOG_MSG_INFO1("Get IPv4 WWAN Network Configuration Successful \n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Get IPv4 WWAN Network Configuration fails. 0x%x\n", mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_get_ipv4_wwan_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  resp_msg.v4_addr_valid = true;
  resp_msg.v4_prim_dns_addr_valid = true;
  resp_msg.v4_sec_dns_addr_valid = true;

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_get_ipv4_wwan_config_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* mcm_mobileap_svc_hdlr_get_ipv4_wwan_config() */




  /*===========================================================================
    FUNCTION MCM_MOBILEAP_SVC_HDLR_SET_DUALAP_CONFIG()

DESCRIPTION
      Enable WLAN functionality on Apps.

    PARAMETERS
      sp:		   MCM Mobileap service instance state pointer for this qmi link
      cmd_buf_p:   Message Header
      cl_sp:	   Coresponding client state pointer
      sdu_in:	   input command data

    RETURN VALUE
      dsm * pointing to the response to be sent to host
      NULL if no response was generated or command was discarded

    DEPENDENCIES
      qmi_qcmap_msgr_init() must have been called

    SIDE EFFECTS
      None
  ===========================================================================*/
 static qmi_csi_cb_error  mcm_mobileap_svc_hdlr_set_dualap_config
(
  mcm_mobileap_svc_client_info_type *clnt_info,
  qmi_req_handle         req_handle,
  int                    msg_id,
  void                  *req_c_struct,
  int                    req_c_struct_len,
  void                  *service_handle
)
{
  mcm_mobileap_svc_state_info_type *qcmap_sp;
  mcm_mobileap_svc_handle_type *map_handle;
  mcm_mobileap_set_dualap_config_req_msg_v01  *req_ptr;
  mcm_mobileap_set_dualap_config_resp_msg_v01 resp_msg;

  unsigned int index;
  mcm_error_t_v01 mcm_err_num = MCM_SUCCESS_V01;
#ifndef TARGET_IS_9615
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
#endif /* !(TARGET_IS_9615) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (mcm_mobileap_svc_state_info_type *)service_handle;
  req_ptr = (mcm_mobileap_set_dualap_config_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p set dualap config", clnt_info,0,0);

  index = req_ptr->mcm_mobileap_handle;
  if (index != MCM_MOBILEAP_SVC_HANDLE ||
     (map_handle = &mcm_mobileap_svc_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                   req_ptr->mcm_mobileap_handle,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_set_dualap_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }


  memset(&resp_msg, 0, sizeof(mcm_mobileap_set_dualap_config_resp_msg_v01));


/*---------------------------------------------------------------------
  Set DUALAP Config
---------------------------------------------------------------------*/
#ifdef TARGET_IS_9615
  if (QcMapMgr->SetDualAPConfig(req_ptr->dualap_config.enable,
                                req_ptr->dualap_config.a5_ip_address,
                                req_ptr->dualap_config.sub_net_mask,
                                &mcm_err_num))
  {
    LOG_MSG_INFO1("Set DUALAP Config succeeds.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Set DUALAP Config fails. 0x%x\n", mcm_err_num,0,0);
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.resp.error = mcm_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_mobileap_set_dualap_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
#else /* TARGET_IS_9615 */
  mcm_err_num = MCM_ERROR_NOT_SUPPORTED_V01;
  LOG_MSG_ERROR("Set DUALAP Config not supported. 0x%x\n", mcm_err_num,0,0);
  resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
  resp_msg.resp.error = mcm_err_num;
#endif /* !(TARGET_IS_9615) */
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_mobileap_set_dualap_config_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* mcm_mobileap_svc_hdlr_set_dhcpd_config() */
