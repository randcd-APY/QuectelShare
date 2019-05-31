/*===========================================================================

                         D S _ Q M I _ Q C M A P _ M S G R. CPP

DESCRIPTION

  The Data Services QMI Qualcomm Mobile Access Point Messenger service source file.

EXTERNALIZED FUNCTIONS

  qmi_qcmap_msgr_init()
    Initialize the QMI QCMAP CM service

  Copyright (c) 2012-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/
/*===========================================================================

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/12/12    gk     Created module
10/26/12    cp     Added support for Dual AP and different types of NAT.
12/19/12    sb     Added support for RNDIS/ECM USb tethering
02/27/13    cp     Added support to get IPV6 WAN status.
04/17/13    mp     Added support to get IPv6 WWAN/STA mode configuration.
05/09/13    tw     Fix mobileAP disable error when UPnP is up.
06/12/13    sg     Added DHCP Reservation feature
09/17/13    at     Added support to Enable/Disable ALGs
01/03/14    vm     Changes to support IoE on 9x25
01/16/14    cp     Added support for modem loopback call.
01/20/14    sr     Added support for connected devices in SoftAP
25/02/14    pm     Added handling of RTM_NEWADDR event for STA assoc
02/24/14    vm     Changes to Enable/Disable Station Mode to be in accordance
                   with IoE 9x15
03/20/14    vm     Set appropriate error number while disabling MobileAP to
                   send response/indications to IoE clients.
03/27/14    cp     Added support to DUN+SoftAP.
05/02/14    pm     Removed dependency on dss_new
01/05/15    rk     qtimap offtarget support.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <vector>

#include "comdef.h"

#include "qmi_idl_lib.h"
#include "qmi_csi.h"
#include "common_v01.h"
#include "qualcomm_mobile_access_point_msgr_v01.h"
#include "qcmap_cm_api.h"
#include "ds_qmi_qcmap_msgr.h"

#include "QCMAP_ConnectionManager.h"
#include "QCMAP_Backhaul_WWAN.h"
#include "QCMAP_Backhaul_WLAN.h"
#include "QCMAP_Backhaul_Cradle.h"
#include "QCMAP_Tethered_Backhaul.h"
#include "QCMAP_Backhaul_Ethernet.h"
#include "QCMAP_Backhaul.h"
#include "QCMAP_WLAN.h"
#include "QCMAP_WLAN_SCM.h"
#include "QCMAP_Firewall.h"
#include "QCMAP_NATALG.h"
#include "QCMAP_Tethering.h"
#include "QCMAP_MediaService.h"
#include "QCMAP_LAN.h"
#include "QCMAP_LAN_Manager.h"
#include "QCMAP_BT_Tethering.h"
#include "QCMAP_Virtual_LAN.h"
#include "QCMAP_L2TP.h"
#include "QCMAP_GSB.h"

#include "limits.h"
#include "qcmap_cmdq.h"
#include "dsi_netctrl.h"
#include "qcmap_netlink.h"

/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/



#define QCMAP_MSGR_SOFTAP_HANDLE 0x65432
/*===========================================================================

   The original delay for dss_init was 6 seconds. Allowing this value to be
   set at runtime to determine if delay still necessary. */
#define MAX_BUF_LEN 256

/* Invalid Profile Handle */
#define INVALID_PROFILE_HANDLE   0

#define SET_PROFILE_HANDLE_IN_QMI_MSG(qmi_msg, profile_handle) \
    qmi_msg.profile_handle_valid = TRUE;  \
    qmi_msg.profile_handle = profile_handle;


#define IS_WLAN_IFACE(iface_name) \
        ((qcmap_nl_is_recv_on_dev(iface_name, QCMAP_NL_WLAN0_INTERFACE) == QCMAP_UTIL_SUCCESS) || \
        (qcmap_nl_is_recv_on_dev(iface_name, QCMAP_NL_WLAN1_INTERFACE) == QCMAP_UTIL_SUCCESS) || \
        (qcmap_nl_is_recv_on_dev(iface_name, QCMAP_NL_WLAN2_INTERFACE) == QCMAP_UTIL_SUCCESS) || \
        (qcmap_nl_is_recv_on_dev(iface_name, QCMAP_NL_WLAN3_INTERFACE) == QCMAP_UTIL_SUCCESS) || \
        (qcmap_nl_is_recv_on_dev(iface_name, QCMAP_NL_WLAN4_INTERFACE) == QCMAP_UTIL_SUCCESS))

#define IS_ETH_IFACE(iface_name) \
        (qcmap_nl_is_recv_on_dev(iface_name, QCMAP_NL_ETH_INTERFACE) == QCMAP_UTIL_SUCCESS)

#define IS_ECM_IFACE(iface_name) \
        (qcmap_nl_is_recv_on_dev(iface_name, QCMAP_NL_ECM_INTERFACE) == QCMAP_UTIL_SUCCESS)


/* Get's Backhaul Object from QMI client_info pointer */
/* If cached profileHandle is INVALID, then return default profile object,
      else return Backhaul object for cached profile */
#define GET_BACKHAUL_OBJ_FOR_QMI_CLIENT(clnt_info) \
             (clnt_info->profileHandle == INVALID_PROFILE_HANDLE) ? \
                         GET_DEFAULT_BACKHAUL_OBJECT() : \
                         GET_BACKHAUL_OBJECT(clnt_info->profileHandle)

#define CHECK_QCMAP_BACKHAUL_OBJ(backhaul_obj, resp_msg) \
    if (backhaul_obj == NULL) \
    {  \
        LOG_MSG_ERROR("%s(): Backhaul_Obj = NULL", __func__, 0, 0); \
        resp_msg.resp.result = QMI_RESULT_FAILURE_V01;  \
        resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;  \
        qmi_csi_send_resp(req_handle, msg_id, &resp_msg, sizeof(resp_msg)); \
        return QMI_CSI_CB_NO_ERR;   \
    }  \

/* Get's Backhaul WWAN Object from QMI client_info pointer */
/* If cached profileHandle is INVALID, then return default profile object,
      else return WWAN object for cached profile */
#define GET_BACKHAUL_WWAN_OBJ_FOR_QMI_CLIENT(clnt_info) \
             (clnt_info->profileHandle == INVALID_PROFILE_HANDLE) ? \
                         GET_DEFAULT_BACKHAUL_WWAN_OBJECT() : \
                         GET_BACKHAUL_WWAN_OBJECT(clnt_info->profileHandle)

#define CHECK_QCMAP_OBJ(obj, resp_msg) \
    if (obj == NULL) \
    {  \
        LOG_MSG_ERROR("%s(): Backhaul_WWAN_Obj = NULL", __func__, 0, 0); \
        resp_msg.resp.result = QMI_RESULT_FAILURE_V01;  \
        resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;  \
        qmi_csi_send_resp(req_handle, msg_id, &resp_msg, sizeof(resp_msg)); \
        return QMI_CSI_CB_NO_ERR;   \
    }  \

/* Get's NAT/ALG Object from QMI client_info pointer */
/* If cached profileHandle is INVALID, then return default profile object,
      else return WWAN object for cached profile */
#define GET_NATALG_OBJ_FOR_QMI_CLIENT(clnt_info) \
             (clnt_info->profileHandle == INVALID_PROFILE_HANDLE) ? \
                         GET_DEFAULT_NATALG_OBJECT() : \
                         GET_NATALG_OBJECT(clnt_info->profileHandle)

/* Get's Firewall Object from QMI client_info pointer */
/* If cached profileHandle is INVALID, then return default profile object,
      else return WWAN object for cached profile */
#define GET_FIREWALL_OBJ_FOR_QMI_CLIENT(clnt_info) \
             (clnt_info->profileHandle == INVALID_PROFILE_HANDLE) ? \
                         GET_DEFAULT_FIREWALL_OBJECT() : \
                         GET_FIREWALL_OBJECT(clnt_info->profileHandle)


/* Modem Loopback mode */
unsigned int modem_loopback_mode;

/* Server sockets */
unsigned int qcmap_dsi_sockfd;
unsigned int qcmap_cmdq_sockfd;
unsigned int qcmap_nas_sockfd;
unsigned int qcmap_dsd_sockfd;
unsigned int qcmap_qmi_service_sockfd;
unsigned int qcmap_timer_sockfd;
unsigned int qcmap_sta_sockfd;
unsigned int qcmap_nl_sockfd;
unsigned int qcmap_scm_sockfd;
unsigned int qcmap_pmip_sockfd;

/* Client sockets */
unsigned int dsi_qcmap_sockfd;
unsigned int cmdq_qcmap_sockfd;
unsigned int nas_qcmap_sockfd;
unsigned int dsd_qcmap_sockfd;
unsigned int qmi_service_qcmap_sockfd;
unsigned int timer_qcmap_sockfd;
unsigned int nl_qcmap_sockfd;
unsigned int scm_qcmap_sockfd;
unsigned int pmip_qcmap_sockfd;
unsigned int pmip_qcmap_ind_sockfd;

struct sigevent sev;

/*==========================================================================
                                DATA TYPES

===========================================================================*/

/*---------------------------------------------------------------------------
  QMI QCMAP CM Client state info
---------------------------------------------------------------------------*/
typedef struct {
  qmi_client_handle                 clnt;
  boolean                           packet_stats_status_ind_regd;
  boolean                           qcmap_status_ind_regd;
  boolean                           station_mode_status_ind_regd;
  boolean                           cradle_mode_status_ind_regd;
  boolean                           ethernet_mode_status_ind_regd;
  boolean                           bt_tethering_status_ind_regd;
  boolean                           bt_tethering_wan_ind_regd;
  boolean                           wlan_status_ind_regd;
  boolean                           backhaul_status_ind_regd;
  boolean                           wwan_roaming_status_ind_regd;
  boolean                           client_enabled;
  int16_t                           bridge_vlan_id;
  profile_handle_type_v01           profileHandle;
}qmi_qcmap_msgr_client_info_type;

static qmi_qcmap_msgr_state_info_type    qmi_qcmap_msgr_state;
static qmi_csi_os_params               os_params;
static boolean                         qmi_qcmap_msgr_inited = false;
static qmi_qcmap_msgr_softap_handle_type qcmap_handle;

/*===========================================================================

                               INTERNAL DATA

===========================================================================*/

/*---------------------------------------------------------------------------
  QMI service command handlers
  forward declarations & cmd handler dispatch table definition
---------------------------------------------------------------------------*/

static qmi_csi_cb_error  qmi_qcmap_msgr_mobile_ap_enable(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_mobile_ap_disable(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_enable_ipv4(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_disable_ipv4(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_enable_ipv6(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_disable_ipv6(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_bring_up_wwan(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_tear_down_wwan(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_wwan_status_ind_reg(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_packet_stats_status_ind_reg(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_enable_wlan(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_disable_wlan(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_mobile_ap_status_ind_reg(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_add_static_nat_entry(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_delete_static_nat_entry(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_get_static_nat_entries(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_set_dmz(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_delete_dmz(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_get_dmz(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_get_wwan_stats(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_reset_wwan_stats(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_get_ipsec_vpn_pt(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_set_ipsec_vpn_pt(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_get_pptp_vpn_pt(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_set_pptp_vpn_pt(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_get_l2tp_vpn_pt(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_set_l2tp_vpn_pt(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_set_nat_type(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_get_nat_type(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_get_auto_connect(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_set_auto_connect(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);
static qmi_csi_cb_error  qmi_qcmap_msgr_add_firewall_entry(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);
static qmi_csi_cb_error  qmi_qcmap_msgr_del_firewall_entry(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_get_wwan_config(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_tethered_link_up(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_tethered_link_down(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_station_mode_status_ind_reg(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);
static qmi_csi_cb_error  qmi_qcmap_msgr_cradle_mode_status_ind_reg(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_add_dhcp_reservation_record(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                    *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_get_dhcp_reservation_records(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                    *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_edit_dhcp_reservation_record(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                    *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_delete_dhcp_reservation_record(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_dynamic_dns_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_set_dynamic_dns_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_disable_dynamic_dns
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_enable_dynamic_dns
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);
static qmi_csi_cb_error qmi_qcmap_msgr_get_dlna_whitelisting
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);
static qmi_csi_cb_error qmi_qcmap_msgr_set_dlna_whitelisting
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);
static qmi_csi_cb_error qmi_qcmap_msgr_add_dlna_whitelist_ip
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);
static qmi_csi_cb_error qmi_qcmap_msgr_delete_dlna_whitelist_ip
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_enable_sta_mode
(
  qmi_qcmap_msgr_client_info_type *clnt_info,
  qmi_req_handle                  req_handle,
  int                             msg_id,
  void                            *req_c_struct,
  int                             req_c_struct_len,
  void                            *service_handle
);

static qmi_csi_cb_error qmi_qcmap_disable_sta_mode
(
  qmi_qcmap_msgr_client_info_type *clnt_info,
  qmi_req_handle                  req_handle,
  int                             msg_id,
  void                            *req_c_struct,
  int                             req_c_struct_len,
  void                            *service_handle
);

static qmi_csi_cb_error qmi_qcmap_wlan_status_ind_reg
(
  qmi_qcmap_msgr_client_info_type *clnt_info,
  qmi_req_handle                  req_handle,
  int                             msg_id,
  void                            *req_c_struct,
  int                             req_c_struct_len,
  void                            *service_handle
);
static qmi_csi_cb_error  qmi_qcmap_msgr_ethernet_mode_status_ind_reg(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);
/*===========================================================================

                       FORWARD FUNCTION DECLARATIONS

===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_svc_connect_cb
(
  qmi_client_handle         client_handle,
  void                      *service_handle,
  void                      **connection_handle
);

static void qmi_qcmap_msgr_svc_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_handle_client_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_handle
);
static qmi_csi_cb_error qmi_qcmap_msgr_get_firewall_handles_list
(
 qmi_qcmap_msgr_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_get_firewall_entry
(
 qmi_qcmap_msgr_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
);
static qmi_csi_cb_error qmi_qcmap_msgr_get_mobileap_status
(
 qmi_qcmap_msgr_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
);
static qmi_csi_cb_error qmi_qcmap_msgr_get_wwan_status
(
 qmi_qcmap_msgr_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_station_mode_status
(
 qmi_qcmap_msgr_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_get_nat_timeout(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_set_nat_timeout(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_get_wlan_status(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);


static qmi_csi_cb_error  qmi_qcmap_msgr_set_wlan_config(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_get_wlan_config(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_activate_wlan(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_set_lan_config(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_get_lan_config(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_activate_lan(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_get_roaming(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_set_roaming(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

/* Define service handle table for QCMAP messages */
static qmi_csi_cb_error qmi_qcmap_msgr_set_firewall_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_firewall_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_set_wwan_policy
(
 qmi_qcmap_msgr_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
 );

static qmi_csi_cb_error qmi_qcmap_msgr_get_wwan_policy
(
 qmi_qcmap_msgr_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
 );

static qmi_csi_cb_error qmi_qcmap_msgr_get_ipv4_state
(
 qmi_qcmap_msgr_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
 );

static qmi_csi_cb_error qmi_qcmap_msgr_get_ipv6_state
(
 qmi_qcmap_msgr_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
 );

static qmi_csi_cb_error qmi_qcmap_msgr_enable_upnp
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_disable_upnp
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_upnp_status
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_enable_dlna
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_disable_dlna
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_dlna_status
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_set_dlna_media_dir
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_dlna_media_dir
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_enable_multicast_dns_responder
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_disable_multicast_dns_responder
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_multicast_dns_responder_status
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_set_qcmap_bootup_cfg
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_qcmap_bootup_cfg
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_data_bitrate
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_set_upnp_notify_interval
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_upnp_notify_interval
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_set_dlna_notify_interval
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_dlna_notify_interval
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_activate_hostapd_config(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_activate_supplicant_config(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_get_active_wlan_if_info(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_enable_socksv5_proxy
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);


static qmi_csi_cb_error qmi_qcmap_msgr_disable_socksv5_proxy
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_socksv5_proxy_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_set_socksv5_proxy_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_enable_alg(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_disable_alg(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_set_webserver_wwan_access_flag(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_get_webserver_wwan_access_flag(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_set_sip_server_info(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_get_sip_server_info(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_get_ipv6_sip_server_info(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_restore_factory_config(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_get_connected_devices_info(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_set_supplicant_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_cradle_mode
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_set_cradle_mode
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_prefix_delegation_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_set_prefix_delegation_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_prefix_delegation_status
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_gateway_url
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_set_gateway_url
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_get_tiny_proxy_status
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);


static qmi_csi_cb_error qmi_qcmap_msgr_enable_tiny_proxy
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);


static qmi_csi_cb_error qmi_qcmap_msgr_disable_tiny_proxy
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_set_backhaul_priority
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_backhaul_priority
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);
static qmi_csi_cb_error qmi_qcmap_msgr_get_ethernet_mode
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_set_ethernet_mode
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_set_ip_passthrough_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_ip_passthrough_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_ip_passthrough_state
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);
static qmi_csi_cb_error qmi_qcmap_msgr_bt_tethering_status_ind_reg
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);
static qmi_csi_cb_error qmi_qcmap_msgr_bt_tethering_wan_ind_reg
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);


static qmi_csi_cb_error qmi_qcmap_msgr_bring_up_bt_tethering
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_bring_down_bt_tethering
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_bt_tethering_status
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_set_initial_packet_threshold
  (
    qmi_qcmap_msgr_client_info_type        *clnt_info,
    qmi_req_handle           req_handle,
    int                      msg_id,
    void                    *req_c_struct,
    int                      req_c_struct_len,
    void                     *service_handle
  );

static qmi_csi_cb_error qmi_qcmap_msgr_get_initial_packet_threshold
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);


static qmi_csi_cb_error qmi_qcmap_msgr_enable_packet_stats
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);


static qmi_csi_cb_error qmi_qcmap_msgr_disable_packet_stats
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_reset_packet_stats
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_packet_stats_status
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_create_wwan_policy
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_update_wwan_policy
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_delete_wwan_policy
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_wwan_policy_list
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);
static qmi_csi_cb_error qmi_qcmap_msgr_set_vlan_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_vlan_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_delete_vlan_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_set_unmanaged_l2tp_state
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_set_l2tp_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_set_MTU_for_L2TP_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_set_TCP_MSS_for_L2TP_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_l2tp_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_delete_l2tp_tunnel_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_add_pdn_to_vlan_mapping
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_delete_pdn_to_vlan_mapping
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_pdn_to_vlan_mappings
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);


static qmi_csi_cb_error qmi_qcmap_msgr_set_wwan_profile_preference
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_enable_gsb
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_disable_gsb
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_set_gsb_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_gsb_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_delete_gsb_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_get_dun_dongle_mode(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_set_dun_dongle_mode(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_data_path_opt_status
(

  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_set_data_path_opt_status
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_pmip_mode
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_set_pmip_mode
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_get_backhaul_status(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_qcmap_msgr_indication_register(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_wwan_roaming_status
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_lan_bridges
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_select_lan_bridge
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_set_always_on_wlan
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_always_on_wlan
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_set_p2p_role
(
 qmi_qcmap_msgr_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_p2p_role
(
 qmi_qcmap_msgr_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
);

static qmi_csi_cb_error qmi_qcmap_msgr_get_wwan_profile_preference
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error (* const req_handle_table[])
(
 qmi_qcmap_msgr_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
) =
{
  NULL,NULL,NULL,                              /* Request handler for message ID 0x00 - 0x02*/
  qmi_qcmap_msgr_indication_register,          /* Request handler for message ID 0x03*/
  NULL,NULL,NULL,NULL,                         /* Request handler for message ID 0x04 - 0x07*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,     /* Request handler for message ID 0x08 - 0x0F*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,     /* Request handler for message ID 0x10 - 0x17*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,     /* Request handler for message ID 0x18 - 0x1F*/
  qmi_qcmap_msgr_mobile_ap_enable,             /* Request handler for message ID 0x20 */
  qmi_qcmap_msgr_mobile_ap_disable,            /* Request handler for message ID 0x21 */
  qmi_qcmap_msgr_bring_up_wwan,                /* Request handler for message ID 0x22 */
  qmi_qcmap_msgr_tear_down_wwan,               /* Request handler for message ID 0x23 */
  qmi_qcmap_msgr_wwan_status_ind_reg,          /* Request handler for message ID 0x24 */
  NULL,                                        /* Request handler for message ID 0x25 */
  qmi_qcmap_msgr_enable_wlan,                  /* Request handler for message ID 0x26 */
  qmi_qcmap_msgr_disable_wlan,                 /* Request handler for message ID 0x27 */
  qmi_qcmap_msgr_mobile_ap_status_ind_reg,     /* Request handler for message ID 0x28 */
  NULL,
  qmi_qcmap_msgr_add_static_nat_entry,         /* Request handler for message ID 0x2A */
  qmi_qcmap_msgr_delete_static_nat_entry,      /* Request handler for message ID 0x2B */
  qmi_qcmap_msgr_get_static_nat_entries,       /* Request handler for message ID 0x2C */
  qmi_qcmap_msgr_set_dmz,                      /* Request handler for message ID 0x2D */
  qmi_qcmap_msgr_delete_dmz,                   /* Request handler for message ID 0x2E */
  qmi_qcmap_msgr_get_dmz,                      /* Request handler for message ID 0x2F */
  qmi_qcmap_msgr_get_wwan_stats,               /* Request handler for message ID 0x30 */
  qmi_qcmap_msgr_reset_wwan_stats,             /* Request handler for message ID 0x31 */
  qmi_qcmap_msgr_get_ipsec_vpn_pt,             /* Request handler for message ID 0x32 */
  qmi_qcmap_msgr_set_ipsec_vpn_pt,             /* Request handler for message ID 0x33 */
  qmi_qcmap_msgr_get_pptp_vpn_pt,              /* Request handler for message ID 0x34 */
  qmi_qcmap_msgr_set_pptp_vpn_pt,              /* Request handler for message ID 0x35 */
  qmi_qcmap_msgr_get_l2tp_vpn_pt,              /* Request handler for message ID 0x36 */
  qmi_qcmap_msgr_set_l2tp_vpn_pt,              /* Request handler for message ID 0x37 */
  qmi_qcmap_msgr_set_nat_type,                 /* Request handler for message ID 0x38 */
  qmi_qcmap_msgr_get_nat_type,                 /* Request handler for message ID 0x39 */
  qmi_qcmap_msgr_set_auto_connect,             /* Request handler for message ID 0x3A */
  qmi_qcmap_msgr_get_auto_connect,             /* Request handler for message ID 0x3B */
  qmi_qcmap_msgr_add_firewall_entry,           /* Request handler for message ID 0x3C */
  qmi_qcmap_msgr_get_firewall_handles_list,    /* Request handler for message ID 0x3D */
  qmi_qcmap_msgr_get_firewall_entry,           /* Request handler for message ID 0x3E */
  qmi_qcmap_msgr_del_firewall_entry,           /* Request handler for message ID 0x3F */
  qmi_qcmap_msgr_get_wwan_config,              /* Request handler for message ID 0x40 */
  qmi_qcmap_msgr_get_mobileap_status,          /* Request handler for message ID 0x41 */
  qmi_qcmap_msgr_get_wwan_status,              /* Request handler for message ID 0x42 */
  qmi_qcmap_msgr_set_nat_timeout,              /* Request handler for message ID 0x43 */
  qmi_qcmap_msgr_get_nat_timeout,              /* Request handler for message ID 0x44 */
  qmi_qcmap_msgr_set_lan_config,               /* Request handler for message ID 0x45 */
  qmi_qcmap_msgr_get_wlan_status,              /* Request handler for message ID 0x46 */
  qmi_qcmap_msgr_activate_wlan,                /* Request handler for message ID 0x47 */
  qmi_qcmap_msgr_get_lan_config,               /* Request handler for message ID 0x48 */
  qmi_qcmap_msgr_tethered_link_up,                  /* Request handler for message ID 0x49 */
  qmi_qcmap_msgr_tethered_link_down,                /* Request handler for message ID 0x4A */
  qmi_qcmap_msgr_enable_ipv6,                  /* Request handler for message ID 0x4B */
  qmi_qcmap_msgr_disable_ipv6,                 /* Request handler for message ID 0x4C */
  qmi_qcmap_msgr_set_roaming,                  /*Request handler for message ID 0x4D */
  qmi_qcmap_msgr_get_roaming,                  /*Request handler for message ID 0x4E */
  qmi_qcmap_msgr_set_wwan_policy,              /*Request handler for message ID 0x4F */
  qmi_qcmap_msgr_get_wwan_policy,              /*Request handler for message ID 0x50 */
  qmi_qcmap_msgr_get_ipv6_state,               /*Request handler for message ID 0x51 */
  qmi_qcmap_msgr_enable_upnp,                  /*Request handler for message ID 0x52 */
  qmi_qcmap_msgr_disable_upnp,                  /*Request handler for message ID 0x53 */
  qmi_qcmap_msgr_enable_dlna,                   /* Request handler for message ID 0x54 */
  qmi_qcmap_msgr_disable_dlna,                   /* Request handler for message ID 0x55 */
  qmi_qcmap_msgr_set_firewall_config,          /*Request handler for message ID 0x56 */
  qmi_qcmap_msgr_get_firewall_config,          /*Request handler for message ID 0x57 */
  qmi_qcmap_msgr_enable_multicast_dns_responder,     /*Request handler for message ID 0x58 */
  qmi_qcmap_msgr_disable_multicast_dns_responder,    /*Request handler for message ID 0x59 */
  qmi_qcmap_msgr_get_upnp_status,              /*Request handler for message ID 0x5A */
  qmi_qcmap_msgr_get_dlna_status,              /*Request handler for message ID 0x5B */
  qmi_qcmap_msgr_get_multicast_dns_responder_status,  /*Request handler for message ID 0x5C */
  qmi_qcmap_msgr_station_mode_status_ind_reg,         /* Request handler for message ID 0x5D */
  NULL,                                               /* Request handler for message ID 0x5E */
  qmi_qcmap_msgr_get_station_mode_status,             /* Request handler for message ID 0x5F */
  qmi_qcmap_msgr_set_qcmap_bootup_cfg,                /* Request handler for message ID 0x60 */
  qmi_qcmap_msgr_get_qcmap_bootup_cfg,               /* Request handler for message ID 0x61 */
  qmi_qcmap_msgr_set_dlna_media_dir,                  /* Request handler for message ID 0x62 */
  qmi_qcmap_msgr_get_dlna_media_dir,                   /* Request handler for message ID 0x63 */
  qmi_qcmap_msgr_set_wlan_config,                       /*Request handler for message ID 0x64 */
  qmi_qcmap_msgr_activate_lan,                            /*Request handler for message ID 0x65 */
  qmi_qcmap_msgr_get_wlan_config,                       /*Request handler for message ID 0x66 */
  qmi_qcmap_msgr_enable_ipv4,                  /* Request handler for message ID 0x67 */
  qmi_qcmap_msgr_disable_ipv4,                 /* Request handler for message ID 0x68 */
  qmi_qcmap_msgr_get_ipv4_state,               /* Request handler for message ID 0x69 */
  qmi_qcmap_msgr_get_data_bitrate,             /*Request handler for message ID 0x6A */
  qmi_qcmap_msgr_get_upnp_notify_interval,     /*Request handler for message ID 0x6B */
  qmi_qcmap_msgr_set_upnp_notify_interval,     /*Request handler for message ID 0x6C */
  qmi_qcmap_msgr_get_dlna_notify_interval,     /*Request handler for message ID 0x6D */
  qmi_qcmap_msgr_set_dlna_notify_interval,     /*Request handler for message ID 0x6E */
  qmi_qcmap_msgr_add_dhcp_reservation_record,         /* Request handler for message ID 0x6F*/
  qmi_qcmap_msgr_get_dhcp_reservation_records,        /* Request handler for message ID 0x70*/
  qmi_qcmap_msgr_edit_dhcp_reservation_record,        /* Request handler for message ID 0x71*/
  qmi_qcmap_msgr_delete_dhcp_reservation_record,       /* Request handler for message ID 0x72*/
  qmi_qcmap_msgr_activate_hostapd_config,                /* Request handler for message ID 0x73 */
  qmi_qcmap_msgr_activate_supplicant_config,                /* Request handler for message ID 0x74 */
  qmi_qcmap_msgr_enable_alg,                   /* Request handler for message ID 0x75 */
  qmi_qcmap_msgr_disable_alg,                   /* Request handler for message ID 0x76 */
  qmi_qcmap_msgr_get_webserver_wwan_access_flag,           /* Request handler for message ID 0x77 */
  qmi_qcmap_msgr_set_webserver_wwan_access_flag,           /* Request handler for message ID 0x78 */
  qmi_qcmap_msgr_set_sip_server_info,                      /* Request handler for message ID 0x79 */
  qmi_qcmap_msgr_get_sip_server_info,                      /* Request handler for message ID 0x7A */
  qmi_qcmap_msgr_restore_factory_config,                   /* Request handler for message ID 0x7B */
  NULL,                                                    /* Request handler for message ID 0x7C */
  NULL,                                                    /* Request handler for message ID 0x7D */
  NULL,                                                    /* Request handler for message ID 0x7E */
  qmi_qcmap_msgr_get_connected_devices_info,               /* Request handler for message ID 0x7F */
  NULL,                                                    /* Request handler for message ID 0x80 */
  NULL,                                                    /* Request handler for message ID 0x81 */
  qmi_qcmap_msgr_get_ipv6_sip_server_info,                 /* Request handler for message ID 0x82 */
  qmi_qcmap_msgr_set_supplicant_config,                    /* Request handler for message ID 0x83 */
  qmi_qcmap_msgr_get_cradle_mode,                          /* Request handler for message ID 0x84 */
  qmi_qcmap_msgr_set_cradle_mode,                          /* Request handler for message ID 0x85 */
  NULL,                                                    /* Request handler for message ID 0x86 */
  qmi_qcmap_msgr_get_prefix_delegation_config,             /* Request handler for message ID 0x87 */
  qmi_qcmap_msgr_set_prefix_delegation_config,             /* Request handler for message ID 0x88 */
  qmi_qcmap_msgr_get_prefix_delegation_status,             /* Request handler for message ID 0x89 */
  qmi_qcmap_msgr_set_gateway_url,                          /* Request handler for message ID 0x8A */
  qmi_qcmap_msgr_get_gateway_url,                          /* Request handler for message ID 0x8B */
  qmi_qcmap_msgr_enable_dynamic_dns,                       /* Request handler for message ID 0x8C */
  qmi_qcmap_msgr_disable_dynamic_dns,                      /* Request handler for message ID 0x8D */
  qmi_qcmap_msgr_set_dynamic_dns_config,                   /* Request handler for message ID 0x8E */
  qmi_qcmap_msgr_get_dynamic_dns_config,                   /* Request handler for message ID 0x8F */
  qmi_qcmap_msgr_get_tiny_proxy_status,                    /* Request handler for message ID 0x90 */
  qmi_qcmap_msgr_enable_tiny_proxy,                        /* Request handler for message ID 0x91 */
  qmi_qcmap_msgr_disable_tiny_proxy,                       /* Request handler for message ID 0x92 */
  qmi_qcmap_msgr_set_dlna_whitelisting,                    /* Request handler for message ID 0x93 */
  qmi_qcmap_msgr_get_dlna_whitelisting,                    /* Request handler for message ID 0x94 */
  qmi_qcmap_msgr_add_dlna_whitelist_ip,                    /* Request handler for message ID 0x95 */
  qmi_qcmap_msgr_delete_dlna_whitelist_ip,                 /* Request handler for message ID 0x96 */
  qmi_qcmap_enable_sta_mode,                               /* Request handler for message ID 0x97 */
  qmi_qcmap_wlan_status_ind_reg,                           /* Request handler for message ID 0x98 */
  NULL,                                                    /* Request handler for message ID 0x99 */
  qmi_qcmap_msgr_set_backhaul_priority,                    /* Request handler for message ID 0x9A */
  qmi_qcmap_msgr_get_backhaul_priority,                    /* Request handler for message ID 0x9B */
  qmi_qcmap_disable_sta_mode,                              /* Request handler for message ID 0x9C */
  qmi_qcmap_msgr_cradle_mode_status_ind_reg,               /* Request handler for message ID 0x9D */
  qmi_qcmap_msgr_get_ethernet_mode,                        /* Request handler for message ID 0x9E */
  qmi_qcmap_msgr_set_ethernet_mode,                        /* Request handler for message ID 0x9F */
  NULL,                                                    /* Request handler for message ID 0xA0 */
  qmi_qcmap_msgr_ethernet_mode_status_ind_reg,             /* Request handler for message ID 0xA1 */
  qmi_qcmap_msgr_set_ip_passthrough_config,                /* Request handler for message ID 0xA2 */
  qmi_qcmap_msgr_get_ip_passthrough_config,                /* Request handler for message ID 0xA3 */
  qmi_qcmap_msgr_get_ip_passthrough_state,                 /* Request handler for message ID 0xA4 */
  qmi_qcmap_msgr_bring_up_bt_tethering,                    /* Request handler for message ID 0xA5 */
  qmi_qcmap_msgr_bring_down_bt_tethering,                  /* Request handler for message ID 0xA6 */
  qmi_qcmap_msgr_get_bt_tethering_status,                  /* Request handler for message ID 0xA7 */
  qmi_qcmap_msgr_bt_tethering_status_ind_reg,              /* Request handler for message ID 0xA8 */
  NULL,                                                    /* Request handler for message ID 0xA9 */
  qmi_qcmap_msgr_bt_tethering_wan_ind_reg,                 /* Request handler for message ID 0xAA */
  NULL,                                                    /* Request handler for message ID 0xAB */
  qmi_qcmap_msgr_set_initial_packet_threshold,             /* Request handler for message ID 0xAC */
  qmi_qcmap_msgr_get_initial_packet_threshold,             /* Request handler for message ID 0xAD */
  qmi_qcmap_msgr_enable_socksv5_proxy,                     /* Request handler for message ID 0xAE */
  qmi_qcmap_msgr_disable_socksv5_proxy,                    /* Request handler for message ID 0xAF */
  qmi_qcmap_msgr_get_socksv5_proxy_config,                 /* Request handler for message ID 0xB0 */
  qmi_qcmap_msgr_set_socksv5_proxy_config,                 /* Request handler for message ID 0xB1 */
  qmi_qcmap_msgr_enable_packet_stats,                      /* Request handler for message ID 0xB2 */
  qmi_qcmap_msgr_disable_packet_stats,                     /* Request handler for message ID 0xB3 */
  qmi_qcmap_msgr_reset_packet_stats,                       /* Request handler for message ID 0xB4 */
  qmi_qcmap_msgr_get_packet_stats_status,                  /* Request handler for message ID 0xB5 */
  qmi_qcmap_msgr_packet_stats_status_ind_reg,              /* Request handler for message ID 0xB6 */
  NULL,                                                    /* Request handler for message ID 0xB7 */
  qmi_qcmap_msgr_delete_wwan_policy,                       /* Request handler for message ID 0xB8 */
  qmi_qcmap_msgr_wwan_policy_list,                         /* Request handler for message ID 0xB9 */
  qmi_qcmap_msgr_set_vlan_config,                          /* Request handler for message ID 0xBA */
  qmi_qcmap_msgr_get_vlan_config,                          /* Request handler for message ID 0xBB */
  qmi_qcmap_msgr_delete_vlan_config,                       /* Request handler for message ID 0xBC */
  qmi_qcmap_msgr_set_unmanaged_l2tp_state,                 /* Request handler for message ID 0xBD */
  qmi_qcmap_msgr_set_l2tp_config,                          /* Request handler for message ID 0xBE */
  qmi_qcmap_msgr_set_MTU_for_L2TP_config,                  /* Request handler for message ID 0xBF */
  qmi_qcmap_msgr_set_TCP_MSS_for_L2TP_config,              /* Request handler for message ID 0xC0 */
  qmi_qcmap_msgr_get_l2tp_config,                          /* Request handler for message ID 0xC1 */
  qmi_qcmap_msgr_delete_l2tp_tunnel_config,                /* Request handler for message ID 0xC2 */
  qmi_qcmap_msgr_add_pdn_to_vlan_mapping,                  /* Request handler for message ID 0xC3 */
  qmi_qcmap_msgr_get_pdn_to_vlan_mappings,                 /* Request handler for message ID 0xC4 */
  qmi_qcmap_msgr_create_wwan_policy,                       /* Request handler for message ID 0xC5 */
  qmi_qcmap_msgr_update_wwan_policy,                       /* Request handler for message ID 0xC6 */
  qmi_qcmap_msgr_set_wwan_profile_preference,              /* Request handler for message ID 0xC7 */
  qmi_qcmap_msgr_enable_gsb,                               /* Request handler for message ID 0xC8 */
  qmi_qcmap_msgr_disable_gsb,                              /* Request handler for message ID 0xC9 */
  qmi_qcmap_msgr_get_gsb_config,                           /* Request handler for message ID 0xCA */
  qmi_qcmap_msgr_set_gsb_config,                           /* Request handler for message ID 0xCB */
  qmi_qcmap_msgr_delete_gsb_config,                        /* Request handler for message ID 0xCC */
  qmi_qcmap_msgr_set_dun_dongle_mode,                      /* Request handler for message ID 0xCD */
  qmi_qcmap_msgr_get_dun_dongle_mode,                      /* Request handler for message ID 0xCE */
  qmi_qcmap_msgr_get_data_path_opt_status,                 /* Request handler for message ID 0xCF */
  qmi_qcmap_msgr_set_data_path_opt_status,                 /* Request handler for message ID 0xD0 */
  qmi_qcmap_msgr_get_pmip_mode,                            /* Request handler for message ID 0xD1 */
  qmi_qcmap_msgr_set_pmip_mode,                            /* Request handler for message ID 0xD2 */
  qmi_qcmap_msgr_get_backhaul_status,                      /* Request handler for message ID 0xD3 */
  NULL,                                                    /* Request handler for message ID 0xD4 */
  qmi_qcmap_msgr_get_wwan_roaming_status,                  /* Request handler for message ID 0xD5 */
  NULL,                                                    /* Request handler for message ID 0xD6 */
  qmi_qcmap_msgr_delete_pdn_to_vlan_mapping,               /* Request handler for message ID 0xD7 */
  qmi_qcmap_msgr_get_lan_bridges,                          /* Request handler for message ID 0xD8 */
  qmi_qcmap_msgr_select_lan_bridge,                        /* Request handler for message ID 0xD9 */
  qmi_qcmap_msgr_get_active_wlan_if_info,                  /* Request handler for message ID 0xDA */
  qmi_qcmap_msgr_set_always_on_wlan,                       /* Request handler for message ID 0xDB */
  qmi_qcmap_msgr_get_always_on_wlan,                       /* Request handler for message ID 0xDC */
  qmi_qcmap_msgr_get_wwan_profile_preference,              /* Request handler for message ID 0xDD */
  qmi_qcmap_msgr_set_p2p_role,                             /* Request handler for message ID 0xDE */
  qmi_qcmap_msgr_get_p2p_role                              /* Request handler for message ID 0xDF */
};

/*===========================================================================

                       EXTERNAL FUNCTION DEFINTIONS

===========================================================================*/


/*===========================================================================
  FUNCTION sigHandler
===========================================================================*/
/*!
@brief
  signal handler.

@input
  signal - signal to be handled.

@return
  None

@dependencies
  None

@sideefects
  None
*/
/*=========================================================================*/
void sigHandler(int signal)
{
  int err_num=0;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_WLAN_SCM* QcMapWLANSCMMgr=QCMAP_WLAN_SCM::Get_Instance(false);

  switch (signal)
  {
    case SIGTERM:
    case SIGINT:

        QCMAP_ConnectionManager::Disable_Ext(&err_num, &qmi_err_num,true);
        if (err_num == QCMAP_CM_EALDDISCONN)
        {
          LOG_MSG_INFO1("Mobile AP already disabled!!",0,0,0);
        }

      if (QcMapMgr)
        delete QcMapMgr;
      if (QcMapWLANSCMMgr)
        delete QcMapWLANSCMMgr;

      /* Client sockets */
      close(dsi_qcmap_sockfd);
      close(cmdq_qcmap_sockfd);
      close(nas_qcmap_sockfd);
      close(dsd_qcmap_sockfd);
      close(qmi_service_qcmap_sockfd);
      close(timer_qcmap_sockfd);
      close(nl_qcmap_sockfd);
      close(scm_qcmap_sockfd);
      close(pmip_qcmap_sockfd);
      close(pmip_qcmap_ind_sockfd);

      /* Server sockets */
      close(qcmap_dsi_sockfd);
      close(qcmap_cmdq_sockfd);
      close(qcmap_nas_sockfd);
      close(qcmap_dsd_sockfd);
      close(qcmap_qmi_service_sockfd);
      close(qcmap_timer_sockfd);
      close(qcmap_sta_sockfd);
      close(qcmap_nl_sockfd);
      close(qcmap_scm_sockfd);
      close(qcmap_pmip_sockfd);

      qmi_csi_unregister(qmi_qcmap_msgr_state.service_handle);
      exit(0);
      break;

    default:
      LOG_MSG_INFO1("Received unexpected signal %s\n", signal,0,0);
      break;
  }
}

/*===========================================================================
  FUNCTION readable_addr
  ===========================================================================
  @brief
    converts the numeric address into a text string suitable
    for presentation

  @input
    domain - identifies ipv4 or ipv6 domain
    addr   - contains the numeric address
    str    - this is an ouput value contains address in text string

  @return
    0  - success
  - 1 - failure

  @dependencies
    It depends on inet_ntop()

  @sideefects
  None
  =========================================================================*/
int readable_addr(int domain, const uint32 *addr, char *str)
{
  if (inet_ntop(domain, (void *)addr, str, INET6_ADDRSTRLEN) == NULL)
  {
    LOG_MSG_ERROR("\n Not in presentation format \n",0,0,0);
    return -1;
  }

  return 0;
}


/*===========================================================================
  FUNCTION create_socket
  ===========================================================================
  @brief
    creates a unix domain socket

  @input
    sockfd - socket handler

  @return
    0  - success
  - 1 - failure

  @dependencies
    none

  @sideefects
  None
  =========================================================================*/
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


/*===========================================================================
  FUNCTION create_server_socket
  ===========================================================================
  @brief
    creates server socket

  @input
    unsigned int sock

  @return
    0  - success
  - 1 - failure

  @dependencies
    none

  @sideefects
  None
  =========================================================================*/
int create_server_socket(unsigned int* sockfd, char* file_path)
{
  int val, rval;
  struct sockaddr_un qcmap_un;
  int len;
  struct timeval rcv_timeo;

  rval = create_socket(sockfd);
  if( rval == QCMAP_CM_ERROR || *sockfd < 0 )
  {
    LOG_MSG_ERROR("failed to create server sockfd ", 0, 0, 0);
    return QCMAP_CM_ERROR;
  }
  else if (*sockfd >= FD_SETSIZE)
  {
    LOG_MSG_ERROR(" create_server_socket fd=%d is exceeding range", *sockfd, 0, 0);
    return QCMAP_CM_ERROR;
  }

  rcv_timeo.tv_sec = 0;
  rcv_timeo.tv_usec = 100000;
  setsockopt(*sockfd, SOL_SOCKET, SO_RCVTIMEO, (void *)&rcv_timeo, sizeof(rcv_timeo));
  val = fcntl(*sockfd, F_GETFL, 0);
  fcntl(*sockfd, F_SETFL, val | O_NONBLOCK);
  /* set bit in os_params */
  FD_SET(*sockfd, &(os_params.fds));
  os_params.max_fd = MAX(os_params.max_fd, *sockfd);
  qcmap_un.sun_family = AF_UNIX;
  strlcpy(qcmap_un.sun_path, file_path, sizeof(qcmap_un.sun_path));
  unlink(qcmap_un.sun_path);
  len = strlen(qcmap_un.sun_path) + sizeof(qcmap_un.sun_family);

  if (bind(*sockfd, (struct sockaddr *)&qcmap_un, len) == QCMAP_CM_ERROR)
  {
    LOG_MSG_ERROR("Error binding the server socket, errno: %d", errno, 0, 0);
    return QCMAP_CM_ERROR;
  }
  return QCMAP_CM_ENOERROR;
}
int create_qcmap_service_available_socket()
{
  int val, rval;
  struct sockaddr_un qcmap_dsd;
  int len;
  struct timeval rcv_timeo;

  rval = create_socket(&qcmap_qmi_service_sockfd);
  if( rval == QCMAP_CM_ERROR || qcmap_qmi_service_sockfd < 0 )
  {
    LOG_MSG_ERROR("failed to create qcmap_qmi_service_sockfd ", 0, 0, 0);
    return QCMAP_CM_ERROR;
  }
  else if (qcmap_qmi_service_sockfd >= FD_SETSIZE)
  {
    LOG_MSG_ERROR(" qcmap_qmi_service_sockfd fd=%d is exceeding range",
                  qcmap_qmi_service_sockfd, 0, 0);
    return QCMAP_CM_ERROR;
  }

  rcv_timeo.tv_sec = 0;
  rcv_timeo.tv_usec = 100000;
  setsockopt(qcmap_qmi_service_sockfd, SOL_SOCKET, SO_RCVTIMEO, (void *)&rcv_timeo, sizeof(rcv_timeo));
  val = fcntl(qcmap_qmi_service_sockfd, F_GETFL, 0);
  fcntl(qcmap_qmi_service_sockfd, F_SETFL, val | O_NONBLOCK);
  /* set bit in os_params */
  FD_SET(qcmap_qmi_service_sockfd, &(os_params.fds));
  os_params.max_fd = MAX(os_params.max_fd, qcmap_qmi_service_sockfd);

  qcmap_dsd.sun_family = AF_UNIX;
  strlcpy(qcmap_dsd.sun_path, QCMAP_QMI_SERVICE_UDS_FILE, sizeof(QCMAP_QMI_SERVICE_UDS_FILE));
  unlink(qcmap_dsd.sun_path);
  len = strlen(qcmap_dsd.sun_path) + sizeof(qcmap_dsd.sun_family);
  if (bind(qcmap_qmi_service_sockfd, (struct sockaddr *)&qcmap_dsd, len) == QCMAP_CM_ERROR)
  {
    LOG_MSG_ERROR("Error binding the socket, errno: %d", errno, 0, 0);
    return QCMAP_CM_ERROR;
  }
  return QCMAP_CM_ENOERROR;
}

int create_qcmap_pmip_socket()
{
  int val, rval;
  struct sockaddr_un qcmap_pmip;
  int len;
  struct timeval rcv_timeo;

  rval = create_socket(&qcmap_pmip_sockfd);
  if( rval == QCMAP_CM_ERROR || qcmap_pmip_sockfd < 0 )
  {
    LOG_MSG_ERROR("failed to create qcmap_sta_sockfd ", 0, 0, 0);
    return QCMAP_CM_ERROR;
  }
  else if (qcmap_pmip_sockfd >= FD_SETSIZE)
  {
    LOG_MSG_ERROR(" qcmap_pmip_sockfd fd=%d is exceeding range", qcmap_pmip_sockfd, 0, 0);
    return QCMAP_CM_ERROR;
  }

  rcv_timeo.tv_sec = 0;
  rcv_timeo.tv_usec = 100000;
  setsockopt(qcmap_pmip_sockfd, SOL_SOCKET, SO_RCVTIMEO, (void *)&rcv_timeo, sizeof(rcv_timeo));
  val = fcntl(qcmap_pmip_sockfd, F_GETFL, 0);
  fcntl(qcmap_pmip_sockfd, F_SETFL, val | O_NONBLOCK);
  /* set bit in os_params */
  FD_SET(qcmap_pmip_sockfd, &(os_params.fds));
  os_params.max_fd = MAX(os_params.max_fd, qcmap_pmip_sockfd);

  qcmap_pmip.sun_family = AF_UNIX;
  strlcpy(qcmap_pmip.sun_path, QCMAP_PMIP_FILE, sizeof(QCMAP_PMIP_FILE));
  unlink(qcmap_pmip.sun_path);
  len = strlen(qcmap_pmip.sun_path) + sizeof(qcmap_pmip.sun_family);
  if (bind(qcmap_pmip_sockfd, (struct sockaddr *)&qcmap_pmip, len) == QCMAP_CM_ERROR)
  {
    LOG_MSG_ERROR("Error binding the socket, errno: %d", errno, 0, 0);
    return QCMAP_CM_ERROR;
  }
  return QCMAP_CM_ENOERROR;
}

/*===========================================================================
  FUNCTION qcmap_netlink_start
  ===========================================================================
  @brief
    start QCMAP netlink socket monitor thread for NETLINK NEIGH Message

  @input
    none

  @return
    void

  @dependencies
    none

  @sideefects
  None
  =========================================================================*/
void* qcmap_netlink_start(void)
{
  qcmap_sk_fd_set_info_t sk_fdset;
  int ret_val = 0;
  unsigned int groups = 0;
  qcmap_sk_info_t     sk_info;

  memset(&sk_fdset, 0, sizeof(qcmap_sk_fd_set_info_t));
  LOG_MSG_INFO1("netlink starter memset sk_fdset succeeds\n",0,0,0);
  groups = RTMGRP_NEIGH | RTMGRP_IPV4_IFADDR | RTMGRP_IPV6_ROUTE | RTMGRP_IPV6_IFADDR | RTMGRP_LINK;

  if( qcmap_nl_listener_init(
                                NETLINK_ROUTE,
                                groups,
                                &sk_fdset,
                                qcmap_nl_recv_msg,
                                &sk_info,
                                QCMAP_NL_MAX_NUM_OF_FD) == QCMAP_UTIL_SUCCESS)
  {
    LOG_MSG_INFO1("Open netlink socket succeeds",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Netlink socket open failed",0,0,0);
    return NULL;
  }

  ret_val = qcmap_packet_socket_init(&sk_fdset,
                                     qcmap_packet_sock_recv_msg);
  if (ret_val != QCMAP_NL_SUCCESS)
  {
    LOG_MSG_ERROR("Failed to initialize QCMAP packet socket listener thread\n",
                  0,0,0);
    return NULL;
  }

  ret_val = qcmap_listener_start(&sk_fdset);
  if (ret_val != QCMAP_UTIL_SUCCESS)
  {
    LOG_MSG_ERROR("Failed to start listening to RTM Socket QCMAP netlink "
                  "event listener thread\n",0,0,0);
    return NULL;
  }

  LOG_MSG_ERROR(" Unreachable Code -- If reached means something wrong"
                " in QCMAP Netlink thread",0,0,0);

  return NULL;
}

/*===========================================================================
  FUNCTION qmi_qcmap_msgr_init
  ===========================================================================
  DESCRIPTION
    initializes QMI QCMAP CM service

  PARAMETERS
    None

  RETURN VALUE
    0 - success
   -1 - fail

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int qmi_qcmap_msgr_init
(
  void
)
{
  qmi_csi_error         rc;
  int rval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1("QMI QCMAP CM service init %d", qmi_qcmap_msgr_inited,0,0);

  if (!qmi_qcmap_msgr_inited)
    qmi_qcmap_msgr_inited = TRUE;
  else
  {
    LOG_MSG_INFO1("QMI QCMAP CM service already initialized",0,0,0);
    return QCMAP_CM_ENOERROR;
  }

  /* Create dsi -> qcmap client socket */
  if (create_socket(&dsi_qcmap_sockfd) != QCMAP_CM_ENOERROR)
  {
    LOG_MSG_ERROR("qmi_qcmap_msgr_init::error creating dsi_qcmap_sockfd socket", 0, 0, 0);
  }

  /* Create cmdq -> qcmap client socket */
  if (create_socket(&cmdq_qcmap_sockfd) != QCMAP_CM_ENOERROR)
  {
    LOG_MSG_ERROR("qmi_qcmap_msgr_init::error creating cmdq_qcmap_sockfd socket", 0, 0, 0);
  }

  /* Create nas -> qcmap client socket */
  if (create_socket(&nas_qcmap_sockfd) != QCMAP_CM_ENOERROR)
  {
    LOG_MSG_ERROR("qmi_qcmap_msgr_init::error creating nas_qcmap_sockfd socket", 0, 0, 0);
  }

  /* Create dsd -> qcmap client socket */
  if (create_socket(&dsd_qcmap_sockfd) != QCMAP_CM_ENOERROR)
  {
    LOG_MSG_ERROR("qmi_qcmap_msgr_init::error creating dsd_qcmap_sockfd socket", 0, 0, 0);
  }
  /* Create qmi service -> qcmap client socket */
  if (create_socket(&qmi_service_qcmap_sockfd) != QCMAP_CM_ENOERROR)
  {
    LOG_MSG_ERROR("qmi_qcmap_msgr_init::error creating qmi_service_qcmap_sockfd socket", 0, 0, 0);
  }

  /* Create timer -> qcmap timer socket */
  if (create_socket(&timer_qcmap_sockfd) != QCMAP_CM_ENOERROR)
  {
    LOG_MSG_ERROR("qmi_qcmap_msgr_init::error creating timer_qcmap_sockfd socket", 0, 0, 0);
  }

  /* Create netlink -> qcmap client socket */
  if (create_socket(&nl_qcmap_sockfd) != QCMAP_CM_ENOERROR)
  {
    LOG_MSG_ERROR("qmi_qcmap_msgr_init::error creating nl_qcmap_sockfd socket", 0, 0, 0);
  }

  /* Create pmip -> qcmap client socket */
  if (create_socket(&pmip_qcmap_sockfd) != QCMAP_CM_ENOERROR)
  {
    LOG_MSG_ERROR("qmi_qcmap_msgr_init::error creating pmip_qcmap_sockfd socket", 0, 0, 0);
  }

  /* Create pmip -> qcmap client socket */
  if (create_socket(&pmip_qcmap_ind_sockfd) != QCMAP_CM_ENOERROR)
  {
    LOG_MSG_ERROR("qmi_qcmap_msgr_init::error creating pmip_qcmap_ind_sockfd socket", 0, 0, 0);
  }

  /*-------------------------------------------------------------------------
    Register QMI QCMAP CM service with QCSI
  -------------------------------------------------------------------------*/
  rc = qmi_csi_register
       (
       qcmap_msgr_get_service_object_v01(),
       qmi_qcmap_msgr_svc_connect_cb,
       qmi_qcmap_msgr_svc_disconnect_cb,
       qmi_qcmap_msgr_handle_client_req_cb,
       &qmi_qcmap_msgr_state,
       &os_params,
       &qmi_qcmap_msgr_state.service_handle
       );

  if(rc != QMI_CSI_NO_ERR)
  {
    LOG_MSG_ERROR("Unable to register QCMAP CM service! Error %d", rc,0,0);
    ds_assert(0);
    return QCMAP_CM_ERROR;
  }

  LOG_MSG_INFO1("QMI QCMAP CM service registered with QMI Framework",0,0,0);

  /* Create qcmap -> dsi server socket */
  rval = create_server_socket(&qcmap_dsi_sockfd, QCMAP_DSI_UDS_FILE);
  if (rval != QCMAP_CM_ENOERROR)
  {
    LOG_MSG_ERROR("Unable to create qcmap dsi socket!", 0,0,0);
    return QCMAP_CM_ERROR;
  }

  /* Create qcmap -> cmdq server socket */
  rval = create_server_socket(&qcmap_cmdq_sockfd, QCMAP_CMDQ_UDS_FILE);

  if (rval != QCMAP_CM_ENOERROR)
  {
    LOG_MSG_ERROR("Unable to create qcmap cmdq socket!", 0,0,0);
    return QCMAP_CM_ERROR;
  }

  /* Create qcmap -> nas server socket */
  rval = create_server_socket(&qcmap_nas_sockfd, QCMAP_NAS_UDS_FILE);

  if (rval != QCMAP_CM_ENOERROR)
  {
    LOG_MSG_ERROR("Unable to create qcmap nas socket!", 0,0,0);
    return QCMAP_CM_ERROR;
  }

  /* Create qcmap -> dsd server socket */
  rval = create_server_socket(&qcmap_dsd_sockfd, QCMAP_DSD_UDS_FILE);

  if (rval != QCMAP_CM_ENOERROR)
  {
    LOG_MSG_ERROR("Unable to create qcmap dsd socket!", 0,0,0);
    return QCMAP_CM_ERROR;
  }
  /* Create qcmap -> servivce available server socket */
  rval = create_qcmap_service_available_socket();

  if (rval != QCMAP_CM_ENOERROR)
  {
    LOG_MSG_ERROR("Unable to create qcmap service available socket!", 0,0,0);
    return QCMAP_CM_ERROR;
  }

  /* Create qcmap -> timer server socket */
  rval = create_server_socket(&qcmap_timer_sockfd, QCMAP_TIMER_UDS_FILE);

  if (rval != QCMAP_CM_ENOERROR)
  {
    LOG_MSG_ERROR("Unable to create qcmap nas socket!", 0,0,0);
    return QCMAP_CM_ERROR;
  }

  /* Create qcmap -> netlink server socket */
  rval = create_server_socket(&qcmap_nl_sockfd ,QCMAP_NL_UDS_FILE);
  if (rval != QCMAP_CM_ENOERROR)
  {
    LOG_MSG_ERROR("Unable to create qcmap netlink socket!", 0,0,0);
    return QCMAP_CM_ERROR;
  }

  /* Create qcmap -> sta server socket */
  rval = create_server_socket(&qcmap_sta_sockfd, QCMAP_STA_UDS_FILE);

  if (rval != QCMAP_CM_ENOERROR)
  {
    LOG_MSG_ERROR("Unable to create qcmap sta socket!", 0,0,0);
    return QCMAP_CM_ERROR;
  }

  /* Create qcmap -> pmip server socket */
  rval = create_qcmap_pmip_socket();

  if (rval != QCMAP_CM_ENOERROR)
  {
    LOG_MSG_ERROR("Unable to create qcmap_pmip_sockfd socket!", 0,0,0);
    return QCMAP_CM_ERROR;
  }

  LOG_MSG_INFO1("Successfully completed initialization",0,0,0);
  return QCMAP_CM_ENOERROR;
} /* qmi_qcmap_msgr_init */

struct sigaction sa;

/*===========================================================================
  FUNCTION main
  ===========================================================================
  @brief
    main funcion

  @input
    argc
    argv

  @return
    0 - success
    exit - fail

  @dependencies
    usr to provide input

  @sideefects
    None
  =========================================================================*/

int main(int argc, char **argv)
{

  int nbytes=0;
  char buf[MAX_BUF_LEN];
  char command[MAX_CMD_SIZE];
  struct sockaddr_storage their_addr;
  socklen_t addr_len = sizeof(struct sockaddr_storage);
  qcmap_dsi_buffer_t *qcmap_dsi_buffer = NULL;
  qcmap_nas_buffer_t *qcmap_nas_buffer = NULL;
  qcmap_dsd_buffer_t *qcmap_dsd_buffer = NULL;
  qcmap_qmi_service_buffer_t *qcmap_qmi_service_buffer = NULL;
  qcmap_timer_buffer_t *qcmap_timer_buffer = NULL;
  qcmap_sta_buffer_t *qcmap_sta_buffer = NULL;
  qcmap_nl_sock_msg_t *qcmap_nl_buffer = NULL;
  qcmap_nl_sock_msg_t qcmap_nl_wlan_buffer;
  qcmap_scm_buffer_t *qcmap_scm_buffer = NULL;
  qcmap_pmip_sock_msg_t *qcmap_pmip_buffer = NULL;


  uint8_t             mac_addr[QCMAP_MSGR_MAC_ADDR_LEN_V01];
  uint8_t retry_on_eio = 0;
  fd_set master_fd_set;
  int tmp_handle = 0;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  size_t len = 0;
  int ret;
  pthread_t qcmap_netlink_thread = 0;
  uint8 mac_addr_char[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];
  qcmap_msgr_wlan_mode_enum_v01 wlan_mode = QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01;
  qcmap_msgr_guest_profile_config_v01 guest_access_profile;
  qcmap_msgr_station_mode_config_v01 station_config;
  uint8 origIPv6[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  char ipv6addr[INET6_ADDRSTRLEN];
  memset(origIPv6,0,QCMAP_MSGR_IPV6_ADDR_LEN_V01);
  memset(ipv6addr, 0, INET6_ADDRSTRLEN);
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  memset(command, 0, MAX_CMD_SIZE);


  QCMAP_ConnectionManager* QcMapMgr=NULL;
  QCMAP_WLAN* QcMapWLANMgr = NULL;

  ds_system_call("echo QCMAP:Start Main > /dev/kmsg",
                 strlen("echo QCMAP:Start Main > /dev/kmsg"));

#ifndef FEATURE_QTIMAP_OFFTARGET
  /*Initialize the Diag for QXDM logs*/
  if (TRUE != Diag_LSM_Init(NULL))
  {
     QCMAP_CM_LOG("Diag_LSM_Init failed !!");
  }
#endif

  /* Register the sighandlers, so the app may be shutdown with a
     kill command.*/
  signal(SIGTERM, sigHandler);
  signal(SIGINT, sigHandler);
  signal(SIGUSR1, SIG_IGN);

  /* Register User Signal Handler used for QCMAP Timers */
  sa.sa_flags = SA_SIGINFO | SA_RESTART;
  sa.sa_sigaction = qcmap_timer_handler;
  sigemptyset(&sa.sa_mask);
  sigaction(SIG, &sa, NULL);


  /* Create a thread to monitor Netlink Neighbour*/
  ret = pthread_create(&qcmap_netlink_thread, NULL, qcmap_netlink_start, NULL);
  if (QCMAP_CM_SUCCESS != ret)
  {
     LOG_MSG_ERROR("unable to create netlink thread for QCMAP return reason: %d\n",ret,0,0);
     return ret;
  }

  switch(argc)
  {
    /* If command line parameters were entered, ...*/
    case 2:
      /* ... read the XML file path from argv[1]. */
      LOG_MSG_INFO1("%s XML path %s\n", argv[0], argv[1],0);
      QcMapMgr = QCMAP_ConnectionManager::Get_Instance(argv[1],true);
      break;
    case 3:
       if (0 == strncasecmp(argv[2], "d", 1))
       {
          QcMapMgr = QCMAP_ConnectionManager::Get_Instance(argv[1],true);
          LOG_MSG_INFO1("Deamon mode set %s %s \n",argv[0], argv[1],0);
          break;
       }
       else if (0 == strncasecmp(argv[2], "l", 1))
       {
          QcMapMgr = QCMAP_ConnectionManager::Get_Instance(argv[1],true);
          modem_loopback_mode = 1;
          LOG_MSG_INFO1("Loopback mode set %s %s %s \n",argv[0], argv[1],argv[2]);
          break;
       }
       else
       {
        exit(1);
       }
    default:
      /* Else, use default paramters to configure the Mobile AP. */
      QcMapMgr = QCMAP_ConnectionManager::Get_Instance(argv[1],true);
      break;
  }

  if( QcMapMgr == NULL)
  {
    LOG_MSG_ERROR("Insufficient memory to initialize QCMAP object", 0, 0, 0);
    return QCMAP_CM_ERROR;
  }

  /* Initialize qcmap_cmdq module */
  qmi_qcmap_msgr_init();

  //GET Dual Wifi Config, set dual wifi mode or legacy/enable SCM
  //Mak sure we can only set dual wifi mode if compile flag is defined

  if ((QcMapMgr->dual_wifi_mode = QCMAP_WLAN_SCM::DualWifiEnabled()) == true)
  {
    //enable SCM
    snprintf(command,MAX_CMD_SIZE,"%s -c %s -B", SCM_CTRL_PATH, SCM_CFG_PATH);
    ds_system_call(command, strlen(command));

    /* start SCM threads */
    /* Create scm -> qcmap client socket */
    if (create_socket(&scm_qcmap_sockfd) != QCMAP_CM_ENOERROR)
    {
      LOG_MSG_ERROR("qmi_qcmap_msgr_init::error creating scm_qcmap_sockfd socket", 0, 0, 0);
    }

    /* Create qcmap -> scm server socket */
    ret = create_server_socket(&qcmap_scm_sockfd, QCMAP_SCM_UDS_FILE);

    if (ret != QCMAP_CM_ENOERROR)
    {
      LOG_MSG_ERROR("Unable to create qcmap sta socket!", 0,0,0);
      return QCMAP_CM_ERROR;
    }
  }

  if ( QCMAP_ConnectionManager::get_wlan_enable_cfg_flag() )
  {

    if ( QCMAP_WLAN::GetWLANConfig(&wlan_mode, &guest_access_profile, &station_config, &qmi_err_num) )
    {
      QCMAP_CM_LOG("Got WLAN Config successfully.\n");
    }

    /* If wlan mode is ap-sta and mobileap needs to be enabled on bootup,
     * enable mobileap and then enable wlan */
    if( (wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01 ||
         wlan_mode == QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01 ||
         wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01) &&
        QCMAP_ConnectionManager::get_mobileap_enable_cfg_flag() )
    {
      if (QCMAP_ConnectionManager::Enable(&tmp_handle, &qcmap_handle,&qmi_err_num))
      {
        LOG_MSG_INFO1("Enable Mobile AP on Bootup succeeds.\n",0,0,0);
        qcmap_handle.handle = tmp_handle;
      }
      else
      {
        LOG_MSG_ERROR("Enable Mobile AP On Bootup FAILS!!! errno:%d.\n",qmi_err_num,0,0);
      }
    }

    /* For WLAN bootup KPI log */
    ds_system_call("echo QCMAP:WLAN mode > /dev/kmsg",
            strlen("echo QCMAP:WLAN mode > /dev/kmsg"));

    if ( QCMAP_WLAN::EnableWLAN(&qmi_err_num,true) )
    {
      LOG_MSG_INFO1("Enable WLAN on Bootup succeeds.\n",0,0,0);
    }
    else
    {
      LOG_MSG_INFO1("Enable WLAN on Bootup Failsi errno:%d.\n",qmi_err_num,0,0);
    }
  }

  /* If Wlan is not in STA mode, wlan is enabled first so that wlan
   * service is available at the earliest. After enabling wlan, mobileap is
   * enabled */
  if( (wlan_mode != QCMAP_MSGR_WLAN_MODE_AP_STA_V01 &&
       wlan_mode != QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01 &&
       wlan_mode != QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01) &&
      QCMAP_ConnectionManager::get_mobileap_enable_cfg_flag() )
  {
    if ( QCMAP_ConnectionManager::Enable(&tmp_handle, &qcmap_handle,&qmi_err_num ) )
    {
      LOG_MSG_INFO1("Enable Mobile AP on Bootup succeeds.\n",0,0,0);
      qcmap_handle.handle = tmp_handle;
    }
    else
    {
      LOG_MSG_ERROR("Enable Mobile AP On Bootup FAILS!!! errno:%d.\n",qmi_err_num,0,0);
    }
  }

#ifndef FEATURE_MOBILEAP_APQ_PLATFORM
  /* initilize qcril library  */
  if (DSI_SUCCESS != dsi_init_ex(DSI_MODE_GENERAL, qcmap_cm_dsi_net_init_cb, NULL))
  {
    LOG_MSG_ERROR("dsi_init failed !!",0,0,0);
    return -1;
  }
#endif

  if(QCMAP_NATALG::GetSOCKSv5ProxyEnableCfg(&qmi_err_num) && (qmi_err_num == QMI_ERR_NONE_V01))
  {
    QCMAP_NATALG::EnableSOCKSv5Proxy(&qmi_err_num);
  }

  if ( QCMAP_ConnectionManager::get_gsb_enable_cfg_flag() )
  {
      if (QCMAP_GSB::ConfigureGSB(&qmi_err_num))
      {
        LOG_MSG_INFO1("Enable GSB on bootup succeeded",0,0,0);
      }
  }

  /*KPI log message*/
  ds_system_call("echo QCMAP:Init Complete > /dev/kmsg",
          strlen("echo QCMAP:Init Complete > /dev/kmsg"));
  QCMAP_CM_LOG_FUNC_ENTRY();

  while(1)
  {
    master_fd_set = os_params.fds;
    ret = select(os_params.max_fd+1, &master_fd_set, NULL, NULL, NULL);
    if (ret < 0)
    {
      LOG_MSG_ERROR("Error in select, errno:%d", errno, 0, 0);
      if( errno == EINTR || errno == EAGAIN)
      {
        /* reset retry as we may have reset FD descriptors
           successfully in this case*/
        if (retry_on_eio > 0)
        {
          retry_on_eio = 0;
        }
        continue;
      }
      else if(errno == EIO)
      {
        ds_system_call("echo QCMAP EIO error > /dev/kmsg",
          strlen("echo QCMAP EIO error > /dev/kmsg"));
        /* if we are continously failing with EIO for more
           than 3 times we exit*/
        if (MAX_EIO_RETRY < retry_on_eio)
        {
          /* better to log this in console as we can make sure it
                              was not QCMAP crash*/
          ds_system_call("echo QCMAP..exiting > /dev/kmsg",
                         strlen("echo QCMAP..exiting > /dev/kmsg"));
          return -1;
        }
        retry_on_eio++;
        continue;
      }
      else
      {
        /* better to log this in console as we can make sure it
                              was not QCMAP crash*/
        ds_system_call("echo QCMAP..exiting > /dev/kmsg",
                         strlen("echo QCMAP..exiting > /dev/kmsg"));
        return -1;
      }
    }

    if (retry_on_eio > 0)
    {
      /* reset retry as we may have reset FD
         descriptors in this case*/
      retry_on_eio = 0;
    }

    for (int i = 0; ( i <= os_params.max_fd ); i++)
    {
      if (FD_ISSET(i, &master_fd_set))
      {
           LOG_MSG_INFO1("some fd set, %d, qcmap_dsi_sockfd:%d, qcmap_nl_sockfd:%d", i, qcmap_dsi_sockfd, qcmap_nl_sockfd);
        if ( i == qcmap_dsi_sockfd )
        {
           LOG_MSG_INFO1("Received Message from dsi_callback",0,0,0);
           if ( ( nbytes = recvfrom(i, buf, MAX_BUF_LEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len)) <= 0 )
           {
             if ( nbytes == 0 )
             {
               LOG_MSG_INFO1("Completed full recv from dsi callback", 0, 0, 0);
             }
             else
             {
               LOG_MSG_ERROR("recvfrom returned error, errno:%d", errno, 0, 0);
             }
           }
           else
           {
             qcmap_dsi_buffer = (qcmap_dsi_buffer_t *)buf;
             if (qcmap_dsi_buffer->dsi_nethandle != NULL)
             {
               QCMAP_Backhaul_WWAN::ProcessDSI_NetEvent( qcmap_dsi_buffer->dsi_nethandle,
                                                         qcmap_dsi_buffer->user_data,
                                                         qcmap_dsi_buffer->evt,
                                                         qcmap_dsi_buffer->payload_ptr );
             }
             else if(qcmap_dsi_buffer->user_data != NULL)
             {
               /*Indicates that dsi_net is initialized*/
               if (*((uint32_t *)(qcmap_dsi_buffer->user_data)) == DSI_INITED)
               {
                 LOG_MSG_INFO1("DSI Init Callback Received", 0,0,0);
                 free(qcmap_dsi_buffer->user_data);
                 qcmap_dsi_buffer->user_data = NULL;
                 qcmap_cm_process_dsi_init_ind();
               }
             }
           }
        }
        else if ( i == qcmap_scm_sockfd )
        {
          QCMAP_WLAN_SCM* QcMapWLANSCMMgr=QCMAP_WLAN_SCM::Get_Instance(true);
          LOG_MSG_INFO1("Received Message from scm_context",0,0,0);
          if ( (nbytes = recvfrom(i, buf, MAX_BUF_LEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len)) <= 0 )
          {
            if ( nbytes == 0 )
            {
              LOG_MSG_INFO1("Completed full recv from qcmap_timer_sockfd context", 0, 0, 0);
            }
            else
            {
              LOG_MSG_ERROR("recvfrom returned error, errno:%d", errno, 0, 0);
            }
          }
          else
          {
            qcmap_scm_buffer = (qcmap_scm_buffer_t *)buf;
            QcMapWLANSCMMgr->scm_process_qmi_qcmap_ind(qcmap_scm_buffer->user_handle,
                                                       qcmap_scm_buffer->msg_id,
                                                       qcmap_scm_buffer->ind_buf,
                                                       qcmap_scm_buffer->ind_buf_len,
                                                       qcmap_scm_buffer->ind_cb_data);
            free(qcmap_scm_buffer->ind_buf);
          }
        }
        else if ( i == qcmap_cmdq_sockfd )
        {
           LOG_MSG_INFO1("Received Message from cmdq_context",0,0,0);
           if ( (nbytes = recvfrom(i, buf, MAX_BUF_LEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len)) <= 0 )
           {
             if ( nbytes == 0 )
             {
               LOG_MSG_INFO1("Completed full recv from cmdq context", 0, 0, 0);
             }
             else
             {
               LOG_MSG_ERROR("recvfrom returned error, errno:%d", errno, 0, 0);
             }
           }
           else
           {
             qcmap_dsi_buffer = (qcmap_dsi_buffer_t *)buf;
             if (qcmap_dsi_buffer && qcmap_dsi_buffer->dsi_nethandle != NULL)
             {
               LOG_MSG_INFO1("Call ProcessDSI_NetEvent: 0x%x, evt:%d", qcmap_dsi_buffer->dsi_nethandle, qcmap_dsi_buffer->evt, 0);
               QCMAP_Backhaul_WWAN::ProcessDSI_NetEvent( qcmap_dsi_buffer->dsi_nethandle,
                                        qcmap_dsi_buffer->user_data,
                                        qcmap_dsi_buffer->evt,
                                        qcmap_dsi_buffer->payload_ptr );
             }
             else
                LOG_MSG_INFO1("qcmap_dsi_buffer is NULL", 0, 0, 0);
           }
        }
        else if ( i == qcmap_nas_sockfd )
        {
           LOG_MSG_INFO1("Received Message from nas_context",0,0,0);
           if ( (nbytes = recvfrom(i, buf, MAX_BUF_LEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len)) <= 0 )
           {
             if ( nbytes == 0 )
             {
               LOG_MSG_INFO1("Completed full recv from nas context", 0, 0, 0);
             }
             else
             {
               LOG_MSG_ERROR("recvfrom returned error, errno:%d", errno, 0, 0);
             }
           }
           else
           {
             qcmap_nas_buffer = (qcmap_nas_buffer_t *)buf;

             qcmap_cm_process_qmi_nas_ind (
             qcmap_nas_buffer->user_handle,
             qcmap_nas_buffer->msg_id,
             qcmap_nas_buffer->ind_buf,
             qcmap_nas_buffer->ind_buf_len,
             qcmap_nas_buffer->ind_cb_data );

             free(qcmap_nas_buffer->ind_buf);
           }
        }
        else if ( i == qcmap_dsd_sockfd )
        {
           LOG_MSG_INFO1("\n\nReceived Message from dsd_context\n\n",0,0,0);
           if ( (nbytes = recvfrom(i, buf, MAX_BUF_LEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len)) <= 0 )
           {
             if ( nbytes == 0 )
             {
               LOG_MSG_INFO1("Completed full recv from nas context", 0, 0, 0);
             }
             else
             {
               LOG_MSG_ERROR("recvfrom returned error, errno:%d", errno, 0, 0);
             }
           }
           else
           {
             qcmap_dsd_buffer = (qcmap_dsd_buffer_t *)buf;

             qcmap_cm_process_qmi_dsd_ind (
             qcmap_dsd_buffer->user_handle,
             qcmap_dsd_buffer->msg_id,
             qcmap_dsd_buffer->ind_buf,
             qcmap_dsd_buffer->ind_buf_len,
             qcmap_dsd_buffer->ind_cb_data );

             free(qcmap_dsd_buffer->ind_buf);
           }
        }
        else if (i == qcmap_qmi_service_sockfd)
        {
           LOG_MSG_INFO1("\n\nReceived Message from srv available context\n\n",0,0,0);
           if ( (nbytes = recvfrom(i, buf, MAX_BUF_LEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len)) <= 0 )
           {
             if ( nbytes == 0 )
             {
               LOG_MSG_INFO1("Completed full recv from nas context", 0, 0, 0);
             }
             else
             {
               LOG_MSG_ERROR("recvfrom returned error, errno:%d", errno, 0, 0);
             }
           }
           else
           {
             qcmap_qmi_service_buffer =  (qcmap_qmi_service_buffer_t *)buf;

             qcmap_cm_process_qmi_service_availabilty (
                     qcmap_qmi_service_buffer->qmi_service_id );
           }
        }
        else if ( i == qcmap_timer_sockfd )
        {
           LOG_MSG_INFO1("Received Message from timer_context",0,0,0);
           if ( (nbytes = recvfrom(i, buf, MAX_BUF_LEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len)) <= 0 )
           {
             if ( nbytes == 0 )
             {
               LOG_MSG_INFO1("Completed full recv from qcmap_timer_sockfd context", 0, 0, 0);
             }
             else
             {
               LOG_MSG_ERROR("recvfrom returned error, errno:%d", errno, 0, 0);
             }
           }
           else
           {
             qcmap_timer_buffer = (qcmap_timer_buffer_t *)buf;
             qcmap_cm_process_qmi_timer_ind(qcmap_timer_buffer->msg_id, qcmap_timer_buffer->user_data);
           }
        }
        else if ( i == qcmap_sta_sockfd )
        {
           LOG_MSG_INFO1("Received Message from sta_context", 0, 0, 0);
           addr_len = sizeof(struct sockaddr_storage);
           memset(buf,0,MAX_BUF_LEN);
           len = sizeof(qcmap_sta_buffer_t);
           if ( (nbytes = recvfrom(i, buf, len , 0, (struct sockaddr *)&their_addr, &addr_len)) <= 0 )
           {
             if ( nbytes == 0 )
             {
               LOG_MSG_INFO1("Completed full recv from sta context", 0, 0, 0);
             }
             else
             {
               LOG_MSG_ERROR("recvfrom returned error, errno:%d", errno, 0, 0);
             }
           }
           else
           {
             qcmap_sta_buffer = (qcmap_sta_buffer_t *)buf;
             LOG_MSG_INFO1("Received event %d from sta context",
                           qcmap_sta_buffer->event, 0, 0);

             qcmap_msgr_device_type_enum_v01 device_type = QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01;
             if (qcmap_sta_buffer->iface_num == QcMapMgr->ap_dev_num1)
             {
               device_type = QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01;
             }
             else if (qcmap_sta_buffer->iface_num == QcMapMgr->ap_dev_num2)
             {
               device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01;
             }
             else if (QcMapMgr->dual_wifi_mode &&
                       qcmap_sta_buffer->iface_num == QcMapMgr->ap_dev_num3)
             {
               device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01;
             }
             else if ( qcmap_sta_buffer->iface_num == QcMapMgr->ap_dev_num4)
             {
               device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_3_V01;
             }
             else
             {
               LOG_MSG_ERROR("Device num does not match either AP, %d", qcmap_sta_buffer->iface_num, 0, 0);
             }

             if (qcmap_sta_buffer->event == AP_STA_CONNECTED)
             {
               memset(&qcmap_nl_wlan_buffer, 0, sizeof(qcmap_nl_wlan_buffer));
               memset(mac_addr, 0, QCMAP_MSGR_MAC_ADDR_LEN_V01);
               ds_mac_addr_pton(qcmap_sta_buffer->mac_addr, mac_addr);
               memcpy(qcmap_nl_wlan_buffer.nl_addr.mac_addr, mac_addr, QCMAP_MSGR_MAC_ADDR_LEN_V01);
               if(QCMAP_ConnectionManager::MatchMacAddrInList(&qcmap_nl_wlan_buffer) == 0)
               {
                 LOG_MSG_INFO1("No match found for the WLAN MAC,"
                               "so add a linked list node device type %d",qcmap_sta_buffer->iface_num, 0, 0);

                 if (!(QCMAP_ConnectionManager::AddNewDeviceEntry( (void*)mac_addr,
                     device_type,
                     NULL,
                     NULL)))
                 {
                   LOG_MSG_ERROR("Error in adding a new device entry ", 0, 0, 0);
                 }
                 else
                 {
                   Getclientaddr(&qcmap_nl_wlan_buffer.nl_addr,qcmap_nl_wlan_buffer.vlan_id);
                   LOG_MSG_INFO1("WLAN Client conected",0, 0, 0);
                   if(QCMAP_ConnectionManager::MatchMacAddrInList(&qcmap_nl_wlan_buffer) == 0)
                   {
                     LOG_MSG_ERROR("No match found for the WLAN MAC -"
                         "Recived a NEWNEIGH Event before AP-STA-CONNECTED",0, 0, 0);
                   }
                 }
               }
               else
               {
                 LOG_MSG_INFO1("Update device entry info if needed", 0, 0, 0);
                 QCMAP_ConnectionManager::UpdateDeviceEntryInfo(&qcmap_nl_wlan_buffer,
                                                                device_type);
               }
             }
             else if (qcmap_sta_buffer->event == AP_STA_DISCONNECTED)
             {
               QCMAP_Backhaul_WWAN::DeleteDelegatedPrefix(
                                              (void*)qcmap_sta_buffer->mac_addr,
                                              &qmi_err_num);

               if(!(QCMAP_ConnectionManager::DeleteDeviceEntryInfo(
                  (void*)qcmap_sta_buffer->mac_addr,
                  NULL,
                  device_type,
                  QCMAP_AP_STA_DISCONNECTED,
                  0)))
               {
                 LOG_MSG_ERROR("Error in deleting a device entry ", 0, 0, 0);
               }
             }
             else
             {
               qcmap_cm_process_sta_ind (qcmap_sta_buffer->sta_cookie,
                                         qcmap_sta_buffer->event);
             }
           }
        }
        else if ( i == qcmap_nl_sockfd )
        {
           nbytes = recvfrom(i, buf, MAX_BUF_LEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len);
           if (nbytes > 0)
           {
              LOG_MSG_INFO1("Completed full recv from netlink_thread context Recvd bytes# %d ",nbytes, 0, 0);
           }
           else
           {
              LOG_MSG_ERROR("recvfrom returned error, errno:%d", errno, 0, 0);
              break;
           }
           qcmap_nl_buffer = (qcmap_nl_sock_msg_t *)buf;
           if (QcMapMgr != NULL && QcMapMgr->packet_stats_enabled)
           {
             QcMapMgr->UpdateCDIWithNeighInfo(qcmap_nl_buffer);
           }
           LOG_MSG_INFO1(" Netlink Message Info: Event = %d, Interface = %d VLAN ID = %d",
                         qcmap_nl_buffer->nl_event,qcmap_nl_buffer->nl_iface,qcmap_nl_buffer->vlan_id);

           if ((qcmap_nl_buffer->nl_iface == QCMAP_NL_USB) && (QcMapMgr))
           {
             if(qcmap_nl_buffer->nl_event == QCMAP_NL_NEWNEIGH)
             {
               /* Check if an entry already exists. If not add the entry. */
               if (QCMAP_Tethering::GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) == true)
               {
                 if(QCMAP_ConnectionManager::MatchMacAddrInList(qcmap_nl_buffer) == 0)
                 {
                   LOG_MSG_INFO1("No match found for the USB MAC,"
                                 "so add a linked list node\n",0, 0, 0);
                   QCMAP_Tethering::SetUSBMac(qcmap_nl_buffer->nl_addr.mac_addr);
                   if(!(QCMAP_ConnectionManager::AddNewDeviceEntry(
                      (void*)qcmap_nl_buffer->nl_addr.mac_addr,
                      QCMAP_MSGR_DEVICE_TYPE_USB_V01,
                      &qcmap_nl_buffer->nl_addr.ip_addr,
                      qcmap_nl_buffer->nl_addr.ip_v6_addr,
                      qcmap_nl_buffer->vlan_id,
                      qcmap_nl_buffer->nl_addr.isValidIPv4address,
                      qcmap_nl_buffer->nl_addr.isValidIPv6address
                      )))
                   {
                      LOG_MSG_ERROR("Error in adding a new device entry ",
                                      0, 0, 0);
                   }
                   else
                   {
                     Getclientaddr(&qcmap_nl_buffer->nl_addr, qcmap_nl_buffer->vlan_id);
                     LOG_MSG_INFO1("USB Client connected",0, 0, 0);
                     if(QCMAP_ConnectionManager::MatchMacAddrInList(qcmap_nl_buffer) == 0)
                     {
                       ds_mac_addr_ntop(qcmap_nl_buffer->nl_addr.mac_addr,
                                        mac_addr_char);
                       ds_log_med("No match found for the USB Client MAC %s",
                                   mac_addr_char, 0, 0);
                     }
                   }
                 }
               }
               else
               {
                 LOG_MSG_INFO1("Recieved NEWNEIGH Event before USB is enabled from QTI --- Ignore it",0,0,0);
               }
             }
             else if(qcmap_nl_buffer->nl_event == QCMAP_NL_NEWADDR)
             {
               if (qcmap_nl_buffer->nl_addr.ip_vsn == IP_V4)
               {
                 LOG_MSG_INFO1("Received RTM_NEWADDR for IPv4 on ecm0",0,0,0);
                 QCMAP_Backhaul_Cradle::ProcessCradleAddrAssign(ECM_IFACE,
                                                                IP_V4);
               }
               else if (qcmap_nl_buffer->nl_addr.ip_vsn == IP_V6)
               {
                 LOG_MSG_INFO1("Received RTM_NEWADDR for IPv6 on ecm0",0,0,0);
                 QCMAP_Backhaul_Cradle::ProcessCradleAddrAssign(ECM_IFACE,
                                                                IP_V6);
               }
             }
             else if (qcmap_nl_buffer->nl_event == QCMAP_NL_DELNEIGH)
             {
               if(qcmap_nl_buffer->nl_addr.ip_vsn == IP_V6)
               {
                 LOG_MSG_ERROR("Delneigh event!!!", 0, 0, 0);
                 QCMAP_Backhaul::DeleteV6ExternalRoute(qcmap_nl_buffer->nl_addr.ip_v6_addr, ECM_IFACE);
               }

               if (qcmap_nl_buffer->vlan_id != 0)
               {
                 (void)QCMAP_ConnectionManager::DeleteDeviceEntryInfo((void *)qcmap_nl_buffer->nl_addr.mac_addr,
                                                                      NULL,
                                                                      QCMAP_MSGR_DEVICE_TYPE_USB_V01,
                                                                      QCMAP_DELNEIGH,
                                                                      qcmap_nl_buffer->vlan_id);
               }
             }
           }
           //If netlink event is received from WLAN interface
           else if((qcmap_nl_buffer->nl_iface == QCMAP_NL_WLAN) && (QcMapMgr))
           {
             if (qcmap_nl_buffer->nl_event == QCMAP_NL_NEWNEIGH)
             {
               //Store the MAC and IP address in QCMAP Manager context
               LOG_MSG_INFO1("Received RTM_NEWNEIGH for WLAN Client",0, 0, 0);
               if(QCMAP_ConnectionManager::MatchMacAddrInList(qcmap_nl_buffer) == 0)
               {
                   LOG_MSG_ERROR("No match found for the WLAN MAC -"
                                 "Recived a NEWNEIGH Event "
                                 "before AP-STA-CONNECTED",0, 0, 0);
               }
             }
             else if (qcmap_nl_buffer->nl_event == QCMAP_NL_NEWADDR)
             {
               QcMapWLANMgr = QCMAP_WLAN::Get_Instance(false);

               if (qcmap_nl_buffer->nl_addr.ip_vsn == IP_V4)
               {
                 LOG_MSG_INFO1("Received RTM_NEWADDR for IPv4 on wlan STA iface",0,0,0);
                 QCMAP_Backhaul_WLAN::ProcessAddrAssign(&qcmap_handle, WLAN_IFACE_STRING(QcMapMgr->sta_iface_index), IP_V4);
                 /* Print the IP Details */
                 IPV4_ADDR_MSG(qcmap_nl_buffer->nl_addr.ip_addr);
               }
               else if (qcmap_nl_buffer->nl_addr.ip_vsn == IP_V6)
               {
                 LOG_MSG_INFO1("Received RTM_NEWADDR for IPv6 on STA iface",0,0,0);
                 QCMAP_Backhaul_WLAN::ProcessAddrAssign(&qcmap_handle, WLAN_IFACE_STRING(QcMapMgr->sta_iface_index), IP_V6);
                 IPV6_ADDR_MSG(qcmap_nl_buffer->nl_addr.ip_v6_addr);
               }
               LOG_MSG_INFO1("QCMAP_NL_NEWADDR on WLAN%d with IP Adddress",QcMapMgr->sta_iface_index,0,0);

               /* Send WLAN Status IND */
               if (QcMapWLANMgr)
               {
                 if (qcmap_nl_buffer->nl_addr.ip_vsn == IP_V4)
                 {
                   QcMapWLANMgr->current_wlan_status.wlan_state[0].ip_type =
                             QCMAP_MSGR_IP_FAMILY_V4_V01;
                   QcMapWLANMgr->current_wlan_status.wlan_state[0].ip4_addr =
                             qcmap_nl_buffer->nl_addr.ip_addr;
                 }
                 else if (qcmap_nl_buffer->nl_addr.ip_vsn == IP_V6)
                 {
                   QcMapWLANMgr->current_wlan_status.wlan_state[0].ip_type =
                             QCMAP_MSGR_IP_FAMILY_V6_V01;
                   memcpy(QcMapWLANMgr->current_wlan_status.wlan_state[0].ip6_addr,
                          qcmap_nl_buffer->nl_addr.ip_v6_addr,
                          sizeof(qcmap_nl_buffer->nl_addr.ip_v6_addr));
                 }
                 QcMapWLANMgr->current_wlan_status.wlan_state[0].wlan_iface_state =
                                QCMAP_MSGR_WLAN_CONNECTED_V01;
                 QcMapWLANMgr->SendWLANStatusIND();
               }
             }
             else if (qcmap_nl_buffer->nl_event == QCMAP_NL_DELNEIGH &&
                      qcmap_nl_buffer->nl_addr.ip_vsn == IP_V6 &&
                      QcMapMgr->sta_iface_index != QCMAP_LAN_INVALID_IFACE_INDEX)
             {
               QCMAP_Backhaul::DeleteV6ExternalRoute(qcmap_nl_buffer->nl_addr.ip_v6_addr, WLAN_IFACE_STRING(QcMapMgr->sta_iface_index));
             }
             else
             {
               LOG_MSG_ERROR("Invalid Event received from Netlink Thread"
                             "Event =%d", qcmap_nl_buffer->nl_event, 0, 0);
             }
           }
           else if ((qcmap_nl_buffer->nl_iface == QCMAP_NL_PPP))
           {
             if (qcmap_nl_buffer->nl_event == QCMAP_NL_PPP_IPV6_ROUTE)
             {
               QCMAP_Tethering::StorePPPIPv6IID(qcmap_nl_buffer->nl_addr.ip_v6_addr);
             }
             else
             {
               LOG_MSG_ERROR("Invalid Event received from Netlink Thread Event =%d",
                             qcmap_nl_buffer->nl_event, 0, 0);
             }
           }
           // If netlink event is recieved on Bridge interface
           else if((qcmap_nl_buffer->nl_iface == QCMAP_NL_BRIDGE) && (QcMapMgr))
           {
             QcMapWLANMgr = QCMAP_WLAN::Get_Instance(false);

             if (qcmap_nl_buffer->nl_event == QCMAP_NL_NEWADDR)
             {
               if (qcmap_nl_buffer->nl_addr.ip_vsn == IP_V4)
               {
                 LOG_MSG_INFO1("Received RTM_NEWADDR for IPv4 on bridge0",0,0,0);
                 if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
                   QCMAP_Backhaul_WLAN::ProcessAddrAssign(&qcmap_handle, BRIDGE_IFACE, IP_V4);
                 else if (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated())
                   QCMAP_Backhaul_Cradle::ProcessCradleAddrAssign(BRIDGE_IFACE,
                                                                  IP_V4);
               }
               else if(qcmap_nl_buffer->nl_addr.ip_vsn == IP_V6)
               {
                 LOG_MSG_INFO1("Received RTM_NEWADDR for IPv6 on bridge0",0,0,0);
                 if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
                   QCMAP_Backhaul_WLAN::ProcessIPv6AddrAssign(&qcmap_handle, qcmap_nl_buffer);
                 else if (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated())
                   QCMAP_Backhaul_Cradle::ProcessCradleAddrAssign(BRIDGE_IFACE,
                                                                  IP_V6);
               }
               if (QcMapWLANMgr)
               {
                 if (qcmap_nl_buffer->nl_addr.ip_vsn == IP_V4)
                 {
                   QcMapWLANMgr->current_wlan_status.wlan_state[0].ip_type =
                             QCMAP_MSGR_IP_FAMILY_V4_V01;
                   QcMapWLANMgr->current_wlan_status.wlan_state[0].ip4_addr =
                             qcmap_nl_buffer->nl_addr.ip_addr;
                 }
                 else if (qcmap_nl_buffer->nl_addr.ip_vsn == IP_V6)
                 {
                   QcMapWLANMgr->current_wlan_status.wlan_state[0].ip_type =
                             QCMAP_MSGR_IP_FAMILY_V6_V01;
                   memcpy(QcMapWLANMgr->current_wlan_status.wlan_state[0].ip6_addr,
                          qcmap_nl_buffer->nl_addr.ip_v6_addr,
                          sizeof(qcmap_nl_buffer->nl_addr.ip_v6_addr));
                 }
                 QcMapWLANMgr->current_wlan_status.wlan_state[0].wlan_iface_state =
                                QCMAP_MSGR_WLAN_CONNECTED_V01;
                 QcMapWLANMgr->SendWLANStatusIND();
               }
             }
             else if (qcmap_nl_buffer->nl_event == QCMAP_NL_NEWNEIGH)
             {
               Getclientaddr(&qcmap_nl_buffer->nl_addr,qcmap_nl_buffer->vlan_id);
               if(QCMAP_ConnectionManager::MatchMacAddrInList(qcmap_nl_buffer) == 0)
               {
                 LOG_MSG_INFO1("No match found for the WLAN/USB/ETH client MAC,",
                               0, 0, 0);
               }
             }
             else if ( qcmap_nl_buffer->nl_event == QCMAP_NL_DELNEIGH )
             {
               if(!(QCMAP_ConnectionManager::DeleteDeviceEntryInfo(
                  (void*)qcmap_nl_buffer->nl_addr.mac_addr,
                  NULL,
                  QCMAP_MSGR_DEVICE_TYPE_USB_V01,
                  QCMAP_DELNEIGH,
                  qcmap_nl_buffer->vlan_id)))
               {
                 ds_mac_addr_ntop(qcmap_nl_buffer->nl_addr.mac_addr, mac_addr_char);
                 ds_log_med("Error in deleting a device entry with MAC %s ",
                            mac_addr_char, 0, 0);
               }
             }
             else
             {
               LOG_MSG_ERROR("Invalid Event received from Netlink Thread"
                             "Event =%d", qcmap_nl_buffer->nl_event, 0, 0);
             }
           }
           else if ((qcmap_nl_buffer->nl_iface == QCMAP_NL_ANY))
           {
             QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=GET_DEFAULT_BACKHAUL_CRADLE_OBJECT();
             QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=GET_DEFAULT_BACKHAUL_ETHERNET_OBJECT();
             QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
             QCMAP_Backhaul* QcMapBackhaul=GET_DEFAULT_BACKHAUL_OBJECT();

             /*
             Without Multi-PDN introduction, we assumed that we will never get
             RA if default BH is down. As M-PDN was introduced, we discovered
             a scenario where default BH was down but concurrent BH was UP.
             This resulted in RA trapped by QCMAP and we went ahead and
             processed them.Here are issues resulting from it:
             1. if default BH is down and connecurrent BH up, then we update
             our CDI client's IPV6 with concurrent BH prefixes
             2. If default BH is up, intermittently we were updating client's
             IPV6 with prefixes from  other BH as we dont validate RA.

             Following fix will ensure proper handling of RA as we validate if
             they are coming from default Iface.if not we ignore them.
             This fix need to be revisited however with M-PDN /VLAN
             architecture. (TO DO).
             */
             if (QcMapBackhaul && qcmap_nl_buffer->nl_event == QCMAP_NL_RA)
             {
               if (strncmp(qcmap_nl_buffer->nl_addr.iface_name, RMNET_INTERFACE, strlen(RMNET_INTERFACE)) == 0)
               {
                 QcMapBackhaul = QCMAP_Backhaul::GetBackhaulObjectForWWANInterface(QCMAP_MSGR_IP_FAMILY_V6_V01,
                                                        qcmap_nl_buffer->nl_addr.iface_name);
                 if (QcMapBackhaul == NULL)
                   QcMapBackhaul = GET_DEFAULT_BACKHAUL_OBJECT(); //Use Default if backhaul object is not found;
               }
             }

             if (QcMapBackhaul && qcmap_nl_buffer->nl_event == QCMAP_NL_RA)
             {
               memcpy(origIPv6, qcmap_nl_buffer->nl_addr.ip_v6_addr,QCMAP_MSGR_IPV6_ADDR_LEN_V01);
               inet_ntop(AF_INET6,(void *)origIPv6, ipv6addr, INET6_ADDRSTRLEN);
               LOG_MSG_INFO1("nl V6 Address %s, ipv6 valid? %d",
                            ipv6addr,qcmap_nl_buffer->nl_addr.isValidIPv6address,0);
               LOG_MSG_INFO1("nl iface %s", qcmap_nl_buffer->nl_addr.iface_name, 0, 0);
               if(QcMapBackhaul->GetDeviceName(devname, QCMAP_MSGR_IP_FAMILY_V6_V01,
                                                   &qmi_err_num) != NO_BACKHAUL)
               {
                 LOG_MSG_INFO1("def dev iface %s", devname, 0, 0);
                 if (strncmp(qcmap_nl_buffer->nl_addr.iface_name, devname,
                             strlen(qcmap_nl_buffer->nl_addr.iface_name)) == 0)
                 {
                   QcMapBackhaul->UpdateGlobalV6addr(&(qcmap_nl_buffer->nl_addr), true);
                   /*Check if AP_STA Router or Cradle WAN Router mode is enabled
                   and cache the RA gateway information*/
                   if (QcMapBackhaulCradleMgr || QcMapBackhaulWLANMgr || QcMapBackhaulEthMgr)
                   {
                     QcMapBackhaul->AddIPv6DefaultRouteInfo(&qcmap_nl_buffer->nl_addr);
                   }
                 }
               }
               else
               {
                 /*
                   With AP+STA backhaul, it takes a little extra time for
                   device to coneect to STA, this results in race conditions
                   between RA reception and current BH setup. So additional
                   logic added to update default route if IF is WLAN as long
                   as RA is received.
                  */
                  if(QcMapBackhaul->GetDeviceName(devname,QCMAP_MSGR_IP_FAMILY_V6_V01, &qmi_err_num) !=0)
                  {
                    QcMapBackhaul->UpdateGlobalV6addr(&(qcmap_nl_buffer->nl_addr), true);
                  }
                 QcMapBackhaul->AddIPv6DefaultRouteInfo(&qcmap_nl_buffer->nl_addr);
               }
             }
             else if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
             {
               if (qcmap_nl_buffer->nl_event == QCMAP_NL_RS &&
                    memcmp(qcmap_nl_buffer->nl_addr.iface_name, BRIDGE_IFACE, strlen(BRIDGE_IFACE)) == 0)
               {
                 QCMAP_Backhaul::SendRSwithUnspecifiedSrc(qcmap_nl_buffer);
               }
               else if (qcmap_nl_buffer->nl_event == QCMAP_NL_NS)
               {
                 QCMAP_Backhaul::ProcessNS(qcmap_nl_buffer);
               }
               else if (qcmap_nl_buffer->nl_event == QCMAP_NL_NA)
               {
                 QCMAP_Backhaul::ProcessNA(&(qcmap_nl_buffer->nl_addr));
               }
             }
             else if (QCMAP_Backhaul_WLAN::IsSTAAvailableV6() && (qcmap_nl_buffer->nl_event == QCMAP_NL_NA ||
                                                                  qcmap_nl_buffer->nl_event == QCMAP_NL_NS))
             {
               //change this to wlan+ap_dev_num1
               QCMAP_Backhaul::AddV6ExternalRoute(&(qcmap_nl_buffer->nl_addr), WLAN_IFACE_STRING(QcMapMgr->sta_iface_index));
             }
             else if (QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6() &&
                      qcmap_nl_buffer->nl_event == QCMAP_NL_NA)
             {
               QCMAP_Backhaul::AddV6ExternalRoute(&(qcmap_nl_buffer->nl_addr), ECM_IFACE);
             }
             else if (QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6() &&
                      qcmap_nl_buffer->nl_event == QCMAP_NL_NA)
             {
               QCMAP_Backhaul::AddV6ExternalRoute(&(qcmap_nl_buffer->nl_addr),
                                                  ETH_IFACE);
             }
             else if (QCMAP_BT_TETHERING::IsBTBackhaulAvailableV6() &&
                      qcmap_nl_buffer->nl_event == QCMAP_NL_NA)
             {
               QCMAP_Backhaul::AddV6ExternalRoute(&(qcmap_nl_buffer->nl_addr),
                                                  BT_IFACE);
             }
             else
             {
               LOG_MSG_ERROR("Invalid Event received from Netlink Thread Event =%d",
                             qcmap_nl_buffer->nl_event, 0, 0);
             }
           }
           else if ((qcmap_nl_buffer->nl_iface == QCMAP_NL_ETH) && (QcMapMgr))
           {
             if (qcmap_nl_buffer->nl_event == QCMAP_NL_NEWNEIGH)
             {
               /* Check if an entry already exists. If not add the entry. */
               if(QCMAP_ConnectionManager::MatchMacAddrInList(qcmap_nl_buffer) == 0)
               {
                 LOG_MSG_INFO1("No match found for the Ethernet Client MAC,"
                               "so add a linked list node with vlan_id:%d\n",qcmap_nl_buffer->vlan_id, 0, 0);

                 if(!(QCMAP_ConnectionManager::AddNewDeviceEntry(
                    (void*)qcmap_nl_buffer->nl_addr.mac_addr,
                    QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01,
                     &qcmap_nl_buffer->nl_addr.ip_addr,
                     qcmap_nl_buffer->nl_addr.ip_v6_addr,
                     qcmap_nl_buffer->vlan_id,
                     qcmap_nl_buffer->nl_addr.isValidIPv4address,
                     qcmap_nl_buffer->nl_addr.isValidIPv6address)))
                 {
                   LOG_MSG_ERROR("Error in adding a new device entry ",
                                 0, 0, 0);
                 }
                 else
                 {
                   Getclientaddr(&qcmap_nl_buffer->nl_addr,qcmap_nl_buffer->vlan_id);
                   LOG_MSG_INFO1("Ethernet Client connected",0, 0, 0);
                   if(QCMAP_ConnectionManager::MatchMacAddrInList(qcmap_nl_buffer) == 0)
                   {
                     LOG_MSG_ERROR("No match found for the Ethernet client",
                     0, 0, 0);
                   }
                 }
               }
            }
            else if(qcmap_nl_buffer->nl_event == QCMAP_NL_NEWADDR)
            {
              if (qcmap_nl_buffer->nl_addr.ip_vsn == IP_V4)
              {
                LOG_MSG_INFO1("Received RTM_NEWADDR for IPv4 on eth0",0,0,0);
                QCMAP_Backhaul_Ethernet::ProcessEthBackhaulAddrAssign(ETH_IFACE,
                                                               IP_V4);
              }
              else if (qcmap_nl_buffer->nl_addr.ip_vsn == IP_V6)
              {
                LOG_MSG_INFO1("Received RTM_NEWADDR for IPv6 on eth0",0,0,0);
                QCMAP_Backhaul_Ethernet::ProcessEthBackhaulAddrAssign(ETH_IFACE,
                                                                IP_V6);
              }
            }
            else if (qcmap_nl_buffer->nl_event == QCMAP_NL_DELNEIGH) 
            {
               if(qcmap_nl_buffer->nl_addr.ip_vsn == IP_V6)
            {
               QCMAP_Backhaul::DeleteV6ExternalRoute(qcmap_nl_buffer->nl_addr.ip_v6_addr, ETH0);
            }

               if (qcmap_nl_buffer->vlan_id != 0)
               {
                 (void)QCMAP_ConnectionManager::DeleteDeviceEntryInfo((void *)qcmap_nl_buffer->nl_addr.mac_addr,
                                                                      NULL,
                                                                      QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01,
                                                                      QCMAP_DELNEIGH,
                                                                      qcmap_nl_buffer->vlan_id);
               }
            }
            else
            {
              LOG_MSG_ERROR("Invalid Event received from Netlink Thread Event =%d",
                             qcmap_nl_buffer->nl_event, 0, 0);
            }
          }

          else if ((qcmap_nl_buffer->nl_iface == QCMAP_NL_BT) && (QcMapMgr))

          {
            if ( qcmap_nl_buffer->nl_event == QCMAP_NL_NEWNEIGH )
            {
              /* Check if an entry already exists. If not add the entry. */
              if(QCMAP_ConnectionManager::MatchMacAddrInList(qcmap_nl_buffer) == 0)
              {
                LOG_MSG_INFO1("No match found for the BT Client MAC,"
                              "so add a linked list node\n",0, 0, 0);

                if(!(QCMAP_ConnectionManager::AddNewDeviceEntry(
                   (void*)qcmap_nl_buffer->nl_addr.mac_addr,
                   QCMAP_MSGR_DEVICE_TYPE_BT_V01,
                   NULL,
                   NULL,
                   qcmap_nl_buffer->vlan_id)))
                {
                  LOG_MSG_ERROR("Error in adding a new device entry ",
                                0, 0, 0);
                }
                else
                {
                  Getclientaddr(&qcmap_nl_buffer->nl_addr,qcmap_nl_buffer->vlan_id);
                  LOG_MSG_INFO1("BT Client connected",0, 0, 0);
                  if(QCMAP_ConnectionManager::MatchMacAddrInList(qcmap_nl_buffer) == 0)
                  {
                    LOG_MSG_ERROR("No match found for the BT client",
                                  0, 0, 0);
                   }
                 }
               }
            }
            else if(qcmap_nl_buffer->nl_event == QCMAP_NL_NEWADDR)
            {
              if (qcmap_nl_buffer->nl_addr.ip_vsn == IP_V4)
              {
                 LOG_MSG_INFO1("Received RTM_NEWADDR for IPv4 on %s", BT_IFACE, 0, 0);
                 QCMAP_BT_TETHERING::ProcessBTWANAddrAssign(BT_IFACE,
                                                            IP_V4);
              }
              else if (qcmap_nl_buffer->nl_addr.ip_vsn == IP_V6)
              {
                LOG_MSG_INFO1("Received RTM_NEWADDR for IPv6 on %s", BT_IFACE, 0, 0);
                QCMAP_BT_TETHERING::ProcessBTWANAddrAssign(BT_IFACE,
                                                           IP_V6);
              }
            }
            else
            {
              LOG_MSG_ERROR("Invalid Event received from Netlink Thread Event =%d",
                             qcmap_nl_buffer->nl_event, 0, 0);
            }
          }

        }

        else if (i == qcmap_pmip_sockfd)
        {
            LOG_MSG_INFO1("recvfrom fd %d ", i, 0, 0);
            nbytes = recvfrom(i, buf, MAX_BUF_LEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len);
           if (nbytes > 0)
           {
              LOG_MSG_INFO1("Completed full recv from qcmap_pmip_sockfd context Recvd bytes# %d ",nbytes, 0, 0);
           }
           else
           {
              LOG_MSG_ERROR("recvfrom returned error, errno:%d", errno, 0, 0);
              break;
           }
            QCMAP_Backhaul* QcMapBackhaulMgr = GET_DEFAULT_BACKHAUL_OBJECT();
            if (!(QcMapBackhaulMgr && (QcMapBackhaulMgr->IsPmipV6ModeEnabled() || QcMapBackhaulMgr->IsPmipV4ModeEnabled())))
            {
              LOG_MSG_ERROR("PmipMode is disabled for ipv6 & ipv4",0, 0, 0);
              break;
            }

            qcmap_pmip_buffer = (qcmap_pmip_sock_msg_t *)buf;
            LOG_MSG_INFO1("PMIP event %d ",qcmap_pmip_buffer->pmip_event, 0, 0);
            if(qcmap_pmip_buffer->pmip_event == QCMAP_PMIP_QUERY_DEVICE_REQ)
            {
              LOG_MSG_INFO1("\nQCMAP_PMIP_QUERY_DEVICE rcvd - send Ack\n",0,0,0);
              QcMapBackhaulMgr->PmipQueryDevice(&qcmap_pmip_buffer->dev_info);
            }
        }

        else /* QMI message from the client */
        {
          LOG_MSG_INFO1("Received QMI Message",0,0,0);
          qmi_csi_handle_event(qmi_qcmap_msgr_state.service_handle, &os_params);
        }
      }
    }
  }
  qmi_csi_unregister(qmi_qcmap_msgr_state.service_handle);
  pthread_join(qcmap_netlink_thread, NULL);
  LOG_MSG_INFO1("QMI QCMAP CM service exiting",0,0,0);
  return 0;
}


/*===========================================================================
  FUNCTION qmi_qcmap_msgr_svc_connect_cb
  ===========================================================================
  @brief
    callback function for qmi_qcmap_msgr_svc_connect event

  @input
    client_handle
    service_handle
    connection_handle -

  @return
    void

  @dependencies
    none

  @sideefects
  None
  =========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_svc_connect_cb
(
  qmi_client_handle          client_handle,
  void                      *service_handle,
  void                     **connection_handle
)
{
  qmi_qcmap_msgr_client_info_type    *clnt_info_ptr;
  qmi_qcmap_msgr_state_info_type     *qcmap_svc_ptr;
  unsigned int                     index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(connection_handle != NULL);
  ds_assert(service_handle != NULL);

  qcmap_svc_ptr = (qmi_qcmap_msgr_state_info_type *) service_handle;

  for (index=0; index < QCMAP_MSGR_MAX_CLIENT_HANDLES; index++)
  {
    if (NULL == qcmap_svc_ptr->client_handle_list[index])
    {
      LOG_MSG_INFO1("\n qmi_qcmap_msgr_svc_connect_cb: index=%d\n",index,0,0);
      clnt_info_ptr = (qmi_qcmap_msgr_client_info_type *)malloc(
                        sizeof(qmi_qcmap_msgr_client_info_type));
      if(!clnt_info_ptr)
      {
        LOG_MSG_INFO1("qcmap_msgr_svc_connect_cb: Out of mem",0,0,0);
        return QMI_CSI_CB_CONN_REFUSED;
      }
      else
        break;
    }
  }

  if (index == QCMAP_MSGR_MAX_CLIENT_HANDLES)
  {
    LOG_MSG_ERROR("Client handle slots exhausted", 0, 0, 0);
    return QMI_CSI_CB_NO_MEM;
  }

  memset(clnt_info_ptr, 0, sizeof(qmi_qcmap_msgr_client_info_type));
  clnt_info_ptr->clnt = client_handle;
  /* Set Profile Handle to Invalid */
  /* When client does any api calls,
   *   - if profileHandle is Invalid, then use default backhaul object's.
   *   - if profileHandle is set to valid, then use backhual, wwan ... as specified.
   */
  clnt_info_ptr->profileHandle = INVALID_PROFILE_HANDLE;
  qcmap_svc_ptr->num_clients++;
  qcmap_svc_ptr->client_handle_list[index] = *connection_handle = clnt_info_ptr;
  LOG_MSG_INFO1("qcmap_msgr_svc_connect_cb: Alloc client 0x%p",
                qcmap_svc_ptr->client_handle_list[index],0,0);

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_svc_connect_cb() */


/*===========================================================================
  FUNCTION qmi_qcmap_msgr_svc_disconnect_cb
  ===========================================================================
  @brief
    callback function for qmi_qcmap_msgr_svc_disconnect event

  @input
    connection_handle -
    service_handle    -

  @return
    void

  @dependencies
    none

  @sideefects
  None
  =========================================================================*/

static void qmi_qcmap_msgr_svc_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_handle
)
{
  qmi_qcmap_msgr_state_info_type       *qcmap_svc_ptr;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  unsigned int                         client_index;
  unsigned int                         index;
  int                                  ret_val,err_num;
  boolean                              last_client=TRUE;
  qmi_qcmap_msgr_client_info_type      *clnt_info;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_upnp_mode_enum_v01        status = 0;
  int                client_count = 1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(service_handle != NULL);
  qcmap_svc_ptr = (qmi_qcmap_msgr_state_info_type *)service_handle;

  for (client_index=0;client_index < QCMAP_MSGR_MAX_CLIENT_HANDLES; client_index++)
  {
    if (qcmap_svc_ptr->client_handle_list[client_index] == connection_handle)
      break;
  }
  if (client_index == QCMAP_MSGR_MAX_CLIENT_HANDLES)
  {
    LOG_MSG_ERROR("qcmap_msgr_svc_disconnect_cb: Invalid Handle %p", connection_handle,0,0);
    return;
  }

  /* Increment Client count to 2 if UPnP is running since it has it's own mobile_ap client */
  if (QCMAP_MediaService::GetUPNPStatus(&status, &qmi_err_num) && status == QCMAP_MSGR_UPNP_MODE_UP_V01)
  {
    client_count = 2;
  }

  clnt_info = (qmi_qcmap_msgr_client_info_type *)qcmap_svc_ptr->client_handle_list[client_index];

  if(clnt_info->client_enabled)
  {
    /* Remove this clnt_info from all Backhaul WWAN Objects */
    QCMAP_Backhaul_WWAN::RemoveQMIClient( (uint32)clnt_info);

    if(qcmap_svc_ptr->client_ref_count > client_count)
    {
      qcmap_svc_ptr->client_ref_count--;
      clnt_info->client_enabled = false;
    }
    else if(qcmap_svc_ptr->client_ref_count == client_count)
    {
     if (ret_val=QCMAP_ConnectionManager::Disable(&err_num, &qmi_err_num))
      {
        if (err_num == QCMAP_CM_EALDDISCONN)
        {
          qcmap_svc_ptr->client_ref_count -= client_count;
          clnt_info->client_enabled = false;
          LOG_MSG_INFO1("MobileAP Disable succeeds.",0,0,0);
        }
        else
        {
          qcmap_svc_ptr->client_ref_count -= client_count;
          clnt_info->client_enabled = false;
          LOG_MSG_INFO1("MobileAP Disable in progress.",0,0,0);
        }
      }
      else
      {
        LOG_MSG_ERROR("MobileAP Disable request fails.",0,0,0);
      }
    }
  }

  qcmap_svc_ptr->num_clients--;
  LOG_MSG_INFO1("qcmap_msgr_svc_disconnect_cb: Releasing client %p, num_clients: %d",
                qcmap_svc_ptr->client_handle_list[client_index], qcmap_svc_ptr->num_clients,0);
  free(qcmap_svc_ptr->client_handle_list[client_index]);
  qcmap_svc_ptr->client_handle_list[client_index] = NULL;


  for (index=0; (index < QCMAP_MSGR_MAX_CLIENT_HANDLES); index++)
  {
    if (qcmap_svc_ptr->client_handle_list[index] != NULL)
    {
      last_client=false;
      break;
    }
  }

  LOG_MSG_INFO1("\nqcmap_msgr_svc_disconnect_cb: last_client: %d", last_client,0,0);
  if (last_client)
  {
    LOG_MSG_INFO1("qcmap_msgr_svc_disconnect_cb: Last Client True!!",0,0,0);
    /* Terminate SoftAP handle, if it still exists, since the last client has disconnected */
    map_handle = &qcmap_handle;
    if (map_handle->handle != 0)
    {
      if ((map_handle->cb_ptr) && ( QCMAP_ConnectionManager::qcmap_tear_down_in_progress == false))
      {
        LOG_MSG_INFO1("\nqcmap_msgr_svc_disconnect_cb: Freeing cbPtr!!",0,0,0);
        free(map_handle->cb_ptr);
        map_handle->cb_ptr = NULL;
      }
      map_handle->handle = 0;
    }
  }
  return;

} /* qmi_qcmap_msgr_svc_disconnect_cb() */


/*===========================================================================
  FUNCTION qmi_qcmap_msgr_handle_client_req_cb
  ===========================================================================
  @brief
    callback function for qmi_qcmap_msgr_handle_client_req

  @input
    connection_handle -
    req_handle        -
    msg_id
    req_c_struct
    req_c_struct_len
    service_handle    -


  @return
    void

  @dependencies
    none

  @sideefects
  None
  =========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_handle_client_req_cb
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
  qmi_qcmap_msgr_client_info_type   *clnt_info;
  qmi_qcmap_msgr_state_info_type    *qcmap_svc_ptr;
  unsigned int                    client_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(connection_handle != NULL);
  ds_assert(service_handle != NULL);

  rc = QMI_CSI_CB_INTERNAL_ERR;
  clnt_info = (qmi_qcmap_msgr_client_info_type*)connection_handle;
  qcmap_svc_ptr = (qmi_qcmap_msgr_state_info_type*) service_handle;

  LOG_MSG_INFO1("\nEntered qmi_qcmap_msgr_handle_client_req_cb",0,0,0);

  for (client_index=0;client_index < QCMAP_MSGR_MAX_CLIENT_HANDLES; client_index++)
  {
    LOG_MSG_INFO1("\nLooking for client %p...", clnt_info,0,0);
    if(qcmap_svc_ptr->client_handle_list[client_index] == clnt_info)
    {
      LOG_MSG_INFO1("Found",0,0,0);
      break;
    }
  }
  if (client_index == QCMAP_MSGR_MAX_CLIENT_HANDLES)
  {
      LOG_MSG_INFO1("qcmap_msgr_handle_req_cb: Invalid clnt handle %p",
                    clnt_info,0,0);
      return rc;
  }

  if(msg_id < (sizeof(req_handle_table) / sizeof(*req_handle_table)))
  {
    if(req_handle_table[msg_id])
    {
      rc = req_handle_table[msg_id] (clnt_info, req_handle, msg_id,
                                     req_c_struct, req_c_struct_len,
                                     service_handle);
    }
    else
    {
      LOG_MSG_INFO1("qcmap_msgr_handle_req_cb: NULL message ID handler: %d",
                    msg_id,0,0);
    }
  }
  else
  {
    LOG_MSG_INFO1("qcmap_msgr_handle_req_cb: Invalid message ID: %d", msg_id,0,0);
  }

  return rc;
} /* qmi_qcmap_msgr_handle_client_req_cb() */


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_BACKHAUL_STATUS_IND()

  DESCRIPTION
    Dispatches a unicast indication to notify registered QCMobileAP client
    about status of backhaul.

  RETURN VALUE
    None

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
   None
===========================================================================*/
void qmi_qcmap_msgr_backhaul_status_ind
(
  void                                *user_data,        /* Call back User data */
  qcmap_cm_backhaul_status    backhaul_status            /* Backhaul Status */
)
{

  qmi_qcmap_msgr_client_info_type      *qcmap_cp=NULL;
  qcmap_msgr_backhaul_status_ind_msg_v01     ind_msg;
  qmi_qcmap_msgr_status_cb_data   *cb_data;
  qmi_csi_error                     rc;
  unsigned int                      client_index=0;
  qmi_qcmap_msgr_state_info_type *qcmap_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert((user_data != NULL));

  cb_data = (qmi_qcmap_msgr_status_cb_data*)user_data;
  qcmap_sp = cb_data->svc_cb;
  qcmap_cp = (qmi_qcmap_msgr_client_info_type *)(qcmap_sp->client_handle_list[0]);

  //go through all the handles and see which client is waiting for backhaul status indication
  for (client_index=0;client_index < QCMAP_MSGR_MAX_CLIENT_HANDLES; client_index++)
  {
    qcmap_cp = (qmi_qcmap_msgr_client_info_type *)(qcmap_sp->client_handle_list[client_index]);
    if (qcmap_cp && qcmap_cp->backhaul_status_ind_regd)
    {
      memset(&ind_msg, 0, sizeof(qcmap_msgr_backhaul_status_ind_msg_v01));

      if (backhaul_status.backhaul_v4_available >= 0)
      {
        ind_msg.backhaul_v4_status_valid = TRUE;
        ind_msg.backhaul_v4_status = (backhaul_status.backhaul_v4_available)?true:false;
        LOG_MSG_INFO1("\nSending backhaul IPV4 status %d to client %p", backhaul_status.backhaul_v4_available, qcmap_cp, 0);
      }
        if (backhaul_status.backhaul_v6_available >= 0)
      {
        ind_msg.backhaul_v6_status_valid = TRUE;
        ind_msg.backhaul_v6_status = (backhaul_status.backhaul_v6_available)?true:false;
        LOG_MSG_INFO1("\nSending backhaul IPV6 status %d to client %p", backhaul_status.backhaul_v6_available, qcmap_cp, 0);
      }
      if ((backhaul_status.backhaul_type >= QCMAP_MSGR_WWAN_BACKHAUL_V01) && (backhaul_status.backhaul_type <= QCMAP_MSGR_BT_BACKHAUL_V01))
      {
        ind_msg.backhaul_type_valid = TRUE;
        ind_msg.backhaul_type = backhaul_status.backhaul_type;
        LOG_MSG_INFO1("\nSending backhaul type %d to client %p", backhaul_status.backhaul_type, qcmap_cp,0);
      }
      rc = qmi_csi_send_ind(qcmap_cp->clnt, QMI_QCMAP_MSGR_BACKHAUL_STATUS_IND_V01,
                  &ind_msg, sizeof(qcmap_msgr_backhaul_status_ind_msg_v01));
      if ( rc != QMI_CSI_NO_ERR )
      {
        LOG_MSG_ERROR("\nCannot send Backhaul status ind to client %p, error %d", qcmap_cp, rc, 0);
      }
      else
      {
        LOG_MSG_INFO1("\nSent Backhaul Status ind to client %p, error %d", qcmap_cp, rc, 0);
      }
    }
  }
} /* qmi_qcmap_msgr_backhaul_status_ind */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_WWAN_STATUS_IND()
  ===========================================================================
  DESCRIPTION
    Dispatches a unicast indication to notify registered QCMobileAP client
      about WWAN status.

  INPUT
    user_data
    conn_status    - connection status
    call_end_type  - call end type enum value

  RETURN VALUE
    None

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_qcmap_msgr_wwan_status_ind
(
  void                                    *user_data,
  profile_handle_type_v01                 profile_handle,
  qcmap_msgr_wwan_status_enum_v01         conn_status,
  qcmap_msgr_wwan_call_end_type_enum_v01  call_end_type,
  int                                     call_end_reason_code,
  qcmap_msgr_wwan_info_v01                *wwan_info
)
{
  qmi_qcmap_msgr_client_info_type      *qcmap_cp=NULL;
  qcmap_msgr_wwan_status_ind_msg_v01     ind_msg;
  qmi_qcmap_msgr_status_cb_data   *cb_data;
  qmi_csi_error                     rc;
  unsigned int                      client_index=0;
  qmi_qcmap_msgr_state_info_type *qcmap_sp;
  union
  {
    qcmap_msgr_bring_up_wwan_ind_msg_v01   bring_up_ind;
    qcmap_msgr_tear_down_wwan_ind_msg_v01  tear_down_ind;
  }wwan_op_ind_msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(user_data != NULL);
  LOG_MSG_INFO1("qmi_qcmap_msgr_wwan_status_ind",0,0,0);
  cb_data = (qmi_qcmap_msgr_status_cb_data*)user_data;
  qcmap_sp = cb_data->svc_cb;
  //qcmap_cp = (qmi_qcmap_msgr_client_info_type *)(qcmap_sp->client_handle_list[0]);
  QCMAP_Backhaul_WWAN *Backhaul_WWAN_Obj;
  Backhaul_WWAN_Obj = GET_BACKHAUL_WWAN_OBJECT(profile_handle);
  qmi_client_ref_type *qmi_client;
  map<uint32, qmi_client_ref_type*>::iterator it;


  if (wwan_info == NULL)
  {
    LOG_MSG_INFO1(" wwan_info is empty", 0,0,0);
  }

  if (Backhaul_WWAN_Obj == NULL)
  {
    LOG_MSG_ERROR("Backhaul_WWAN_Obj is NULL, can't sent events to QMI_Clients", 0,0,0);
    return;
  }

  if (Backhaul_WWAN_Obj->qmi_client_ref == NULL)
  {
    LOG_MSG_ERROR("Backhaul_WWAN->qmi_client_ref is NULL, can't sent events to QMI_Clients", 0,0,0);
    return;
  }

  for(it = Backhaul_WWAN_Obj->qmi_client_ref->begin();
        it != Backhaul_WWAN_Obj->qmi_client_ref->end();
        it++ )
  {
    qmi_client = it->second;
    qcmap_cp = (qmi_qcmap_msgr_client_info_type *)it->first;

    if ( qcmap_cp && qmi_client && (qmi_client->wwan_op_ind_msg_id || qmi_client->wwan_status_ind_regd) )
    {
      if (qmi_client->wwan_op_ind_msg_id)
      {
        if (qmi_client->wwan_op_ind_msg_id == QMI_QCMAP_MSGR_BRING_UP_WWAN_IND_V01)
        {
          SET_PROFILE_HANDLE_IN_QMI_MSG(wwan_op_ind_msg.bring_up_ind, profile_handle);

          wwan_op_ind_msg.bring_up_ind.conn_status = conn_status;
          wwan_op_ind_msg.bring_up_ind.mobile_ap_handle = cb_data->map_instance;

          if ( conn_status == QCMAP_MSGR_WWAN_STATUS_CONNECTING_FAIL_V01 ||
               conn_status == QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTING_FAIL_V01)
          {
            wwan_op_ind_msg.bring_up_ind.wwan_call_end_reason_valid = TRUE;
            wwan_op_ind_msg.bring_up_ind.wwan_call_end_reason.wwan_call_end_reason_type = call_end_type;
            wwan_op_ind_msg.bring_up_ind.wwan_call_end_reason.wwan_call_end_reason_code = call_end_reason_code;
            LOG_MSG_INFO1("Sending WWAN status ind_msg.wwan_call_end_reason_type =%d ind_msg.wwan_call_end_reason_code=%d ",
                              call_end_type,call_end_reason_code,0);
          }

          if ( (conn_status == QCMAP_MSGR_WWAN_STATUS_CONNECTED_V01 ||
               conn_status == QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTED_V01) &&
               (wwan_info != NULL) && wwan_info->iface_name[0] != '\0' )
          {
            wwan_op_ind_msg.bring_up_ind.wwan_info_valid = TRUE;
            strlcpy(wwan_op_ind_msg.bring_up_ind.wwan_info.iface_name, wwan_info->iface_name,
                    sizeof(wwan_op_ind_msg.bring_up_ind.wwan_info.iface_name));
            wwan_op_ind_msg.bring_up_ind.wwan_info.v4_addr = wwan_info->v4_addr;
            wwan_op_ind_msg.bring_up_ind.wwan_info.v4_prim_dns_addr = wwan_info->v4_prim_dns_addr;
            wwan_op_ind_msg.bring_up_ind.wwan_info.v4_sec_dns_addr = wwan_info->v4_sec_dns_addr;

            memcpy(wwan_op_ind_msg.bring_up_ind.wwan_info.v6_addr, wwan_info->v6_addr,
                    sizeof(wwan_op_ind_msg.bring_up_ind.wwan_info.v6_addr));
            memcpy(wwan_op_ind_msg.bring_up_ind.wwan_info.v6_prim_dns_addr,
                    wwan_info->v6_prim_dns_addr,
                    sizeof(wwan_op_ind_msg.bring_up_ind.wwan_info.v6_prim_dns_addr));
            memcpy(wwan_op_ind_msg.bring_up_ind.wwan_info.v6_sec_dns_addr,
                    wwan_info->v6_sec_dns_addr,
                    sizeof(wwan_op_ind_msg.bring_up_ind.wwan_info.v6_sec_dns_addr));
            LOG_MSG_INFO1("Sending WWAN status wwan_info.iface_name: %s",
                              wwan_op_ind_msg.bring_up_ind.wwan_info.iface_name,0,0);
          }

          LOG_MSG_INFO1("Sending WWAN status %d to client %p", conn_status, qcmap_cp,0);
          rc = qmi_csi_send_ind(qcmap_cp->clnt, QMI_QCMAP_MSGR_BRING_UP_WWAN_IND_V01,
                                &wwan_op_ind_msg.bring_up_ind,
                                sizeof(qcmap_msgr_bring_up_wwan_ind_msg_v01));
        }
        else if (qmi_client->wwan_op_ind_msg_id == QMI_QCMAP_MSGR_TEAR_DOWN_WWAN_IND_V01)
        {
          SET_PROFILE_HANDLE_IN_QMI_MSG(wwan_op_ind_msg.tear_down_ind, profile_handle);

          wwan_op_ind_msg.tear_down_ind.conn_status = conn_status;
          wwan_op_ind_msg.tear_down_ind.mobile_ap_handle = cb_data->map_instance;

          if ( conn_status == QCMAP_MSGR_WWAN_STATUS_DISCONNECTED_V01 ||
               conn_status == QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTED_V01)
          {
            wwan_op_ind_msg.tear_down_ind.wwan_call_end_reason_valid = TRUE;
            wwan_op_ind_msg.tear_down_ind.wwan_call_end_reason.wwan_call_end_reason_type = call_end_type;
            wwan_op_ind_msg.tear_down_ind.wwan_call_end_reason.wwan_call_end_reason_code = call_end_reason_code;
            LOG_MSG_INFO1("Sending WWAN status call_end_reason_type =%d call_end_reason_code=%d",
                            call_end_type,call_end_reason_code,0);
          }
          LOG_MSG_INFO1("Sending WWAN status %d to client %p", conn_status, qcmap_cp,0);
          rc = qmi_csi_send_ind(qcmap_cp->clnt, QMI_QCMAP_MSGR_TEAR_DOWN_WWAN_IND_V01,
                               &wwan_op_ind_msg.tear_down_ind,
                               sizeof(qcmap_msgr_tear_down_wwan_ind_msg_v01));
        }
        if ( rc != QMI_CSI_NO_ERR )
        {
          LOG_MSG_ERROR("Cannot send WWAN op pending ind to client %p, error %d", qcmap_cp, rc, 0);
        }
        else
        {
          LOG_MSG_INFO1("Sent WWAN op pending indication %d to client %p",
                        qmi_client->wwan_op_ind_msg_id, qcmap_cp,0);
        }
        /* Reset QMI Client for Bring_UP or Tear_Down notification */
        Backhaul_WWAN_Obj->InsertQMIClient((uint32) qcmap_cp, QMI_CLIENT_REG_WWAN_BRING_UP_DOWN_EVENT, 0);
      }
      else if (qmi_client->wwan_status_ind_regd)
      {
        memset(&ind_msg, 0, sizeof(qcmap_msgr_wwan_status_ind_msg_v01));

        ind_msg.wwan_status = conn_status;
        SET_PROFILE_HANDLE_IN_QMI_MSG(ind_msg, profile_handle)

        if ( conn_status == QCMAP_MSGR_WWAN_STATUS_CONNECTING_FAIL_V01 ||
             conn_status == QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTING_FAIL_V01 ||
             conn_status == QCMAP_MSGR_WWAN_STATUS_DISCONNECTED_V01 ||
             conn_status == QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTED_V01)
        {
          ind_msg.wwan_call_end_reason_valid = TRUE;
          ind_msg.wwan_call_end_reason.wwan_call_end_reason_type = call_end_type;
          ind_msg.wwan_call_end_reason.wwan_call_end_reason_code = call_end_reason_code;
        }

        if ( (conn_status == QCMAP_MSGR_WWAN_STATUS_CONNECTED_V01 ||
               conn_status == QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTED_V01) &&
               (wwan_info != NULL) && wwan_info->iface_name[0] != '\0' )
        {
          ind_msg.wwan_info_valid = TRUE;
          strlcpy(ind_msg.wwan_info.iface_name, wwan_info->iface_name,
                  sizeof(ind_msg.wwan_info.iface_name));
          ind_msg.wwan_info.v4_addr = wwan_info->v4_addr;
          ind_msg.wwan_info.v4_prim_dns_addr = wwan_info->v4_prim_dns_addr;
          ind_msg.wwan_info.v4_sec_dns_addr = wwan_info->v4_sec_dns_addr;

          memcpy(ind_msg.wwan_info.v6_addr, wwan_info->v6_addr,
                  sizeof(ind_msg.wwan_info.v6_addr));
          memcpy(ind_msg.wwan_info.v6_prim_dns_addr,
                  wwan_info->v6_prim_dns_addr,
                  sizeof(ind_msg.wwan_info.v6_prim_dns_addr));
          memcpy(ind_msg.wwan_info.v6_sec_dns_addr,
                  wwan_info->v6_sec_dns_addr,
                  sizeof(ind_msg.wwan_info.v6_sec_dns_addr));
          LOG_MSG_INFO1("Sending WWAN status wwan_info.iface_name: %s",
                            ind_msg.wwan_info.iface_name,0,0);
        }

        LOG_MSG_INFO1("Sending WWAN status %d to client %p", conn_status, qcmap_cp,0);
        rc = qmi_csi_send_ind(qcmap_cp->clnt, QMI_QCMAP_MSGR_WWAN_STATUS_IND_V01,
                              &ind_msg, sizeof(qcmap_msgr_wwan_status_ind_msg_v01));
        if ( rc != QMI_CSI_NO_ERR )
        {
          LOG_MSG_ERROR("Cannot send WWAN ind to client %p, error %d", qcmap_cp, rc, 0);
        }
        else
        {
          LOG_MSG_INFO1("Sent WWAN ind to client %p, error %d", qcmap_cp, rc, 0);
        }
      }
    }
  }
} /* qmi_qcmap_msgr_wwan_status_ind() */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_PACKET_STATS_STATUS_IND()
  ===========================================================================
  DESCRIPTION
    Dispatches a unicast indication to notify registered QCMobileAP client
      about packet stats status.

  INPUT
    user_data
    conn_status    - status change type
    number_of_entries  - number of clients whose info is sent

  RETURN VALUE
    None

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_qcmap_msgr_packet_stats_status_ind
(
  void                           *user_data,
  qcmap_msgr_packet_stats_state_enum_v01 status,
  int   number_of_entries,
  qcmap_cm_client_data_info_t* device_info
)
{
  qmi_qcmap_msgr_client_info_type      *qcmap_cp=NULL;
  qcmap_msgr_packet_stats_status_ind_msg_v01     ind_msg;
  qmi_qcmap_msgr_status_cb_data   *cb_data;
  qmi_csi_error                     rc;
  unsigned int                      client_index=0;
  qmi_error_type_v01                          qmi_err_num = QMI_ERR_NONE_V01;
  qmi_qcmap_msgr_state_info_type *qcmap_sp;
  unsigned int dummy =0;
  uint8 zero_buff[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  boolean payload_prepared = false;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(user_data != NULL);
  memset(zero_buff,0,QCMAP_MSGR_IPV6_ADDR_LEN_V01);

  cb_data = (qmi_qcmap_msgr_status_cb_data*)user_data;
  qcmap_sp = cb_data->svc_cb;
  qcmap_cp = (qmi_qcmap_msgr_client_info_type *)(qcmap_sp->client_handle_list[0]);
  //go through all the handles and see which client is waiting for wwan status indication
  memset(&ind_msg, 0, sizeof(qcmap_msgr_packet_stats_status_ind_msg_v01));

  for (client_index=0;client_index < QCMAP_MSGR_MAX_CLIENT_HANDLES; client_index++)
  {
    qcmap_cp = (qmi_qcmap_msgr_client_info_type *)(qcmap_sp->client_handle_list[client_index]);
    if (qcmap_cp && (qcmap_cp->packet_stats_status_ind_regd || QcMapMgr->packet_stats_enabled))
    {
      ind_msg.conn_status = status;
      if (device_info != NULL && 1==number_of_entries)
      {
        ind_msg.number_of_entries = number_of_entries;
        if (device_info != NULL)
        {
          //update tx_bytes & rx_bytes
          device_info->bytes_rx = device_info->exception_data.rx_data+
                                           device_info->ipa_data.rx_data+
                                           device_info->sfe_data.rx_data;
          device_info->bytes_tx = device_info->exception_data.tx_data+
                                           device_info->ipa_data.tx_data+
                                           device_info->sfe_data.tx_data;
          LOG_MSG_INFO1("Device Stats: RX: %llu TX: %llu", device_info->bytes_rx, device_info->bytes_tx, 0);
          ind_msg.info[0].bytes_rx = device_info->bytes_rx;
          ind_msg.info[0].bytes_tx = device_info->bytes_tx;
          ind_msg.info[0].lease_expiry_time = device_info->lease_expiry_time;
          ind_msg.info[0].device_type = device_info->device_type;
          memcpy (ind_msg.info[0].host_name,device_info->host_name,
            QCMAP_MSGR_DEVICE_NAME_MAX_V01);
          memcpy (ind_msg.info[0].client_mac_addr, device_info->mac_addr,
            QCMAP_MSGR_MAC_ADDR_LEN_V01);
          memcpy (ind_msg.info[0].ll_ipv6_addr,
                  device_info->link_local_v6_addr, QCMAP_MSGR_IPV6_ADDR_LEN_V01);
          ind_msg.info[0].ipv4_addr = device_info->ip_addr;

          for (int j = 0; j < QCMAP_MSGR_MAX_IPV6_ADDR_V01; j++)
          {
            //if no more ipv6 then break
            if (!memcmp(device_info->ipv6[j].addr, zero_buff,QCMAP_MSGR_IPV6_ADDR_LEN_V01))
              break;
            memcpy(ind_msg.info[0].ipv6[j].addr,
                   device_info->ipv6[j].addr,QCMAP_MSGR_IPV6_ADDR_LEN_V01);
          }
        }
      }
      else
      {
        switch (status)
        {
        case QCMAP_MSGR_PACKET_STATS_WLAN_DISABLED_V01:
          if (!payload_prepared)
          {
            if (QcMapMgr->TraverseCDIAndUpdate(QCMAP_UPDATE_WLAN_CLIENTS_DATA,
                                            ind_msg.info,
                                            &ind_msg.number_of_entries))
            {
              LOG_MSG_INFO1("TraverseCDIAndUpdate succeeded, entries: %d",
                          ind_msg.number_of_entries,0,0);
            }
            else
            {
              LOG_MSG_INFO1("TraverseCDIAndUpdate No Connected Devices Information available currently",0,0,0);
              ind_msg.number_of_entries = 0;
            }
            payload_prepared = true;
          }
          break;
        case QCMAP_MSGR_PACKET_STATS_MOBILEAP_DISABLED_V01:
        case QCMAP_MSGR_PACKET_STATS_IPV6_WWAN_DISCONNECTED_V01:
        case QCMAP_MSGR_PACKET_STATS_IPV4_WWAN_DISCONNECTED_V01:
        case QCMAP_MSGR_PACKET_STATS_BH_SWITCHED_V01:
          if (!payload_prepared)
          {
            if (QcMapMgr->GetConnectedDevicesInfo(ind_msg.info,
                                                &ind_msg.number_of_entries,
                                                &qmi_err_num))
            {
              LOG_MSG_INFO1("Get Connected Devices Information succeeded, entries: %d\n",
                            ind_msg.number_of_entries,0,0);
            }
            else
            {
              LOG_MSG_ERROR("No Connected Devices Information available currently",0,0,0);
              ind_msg.number_of_entries = 0;
            }

            //clean up connection info..
            if (QcMapMgr->TraverseCDIAndUpdate(QCMAP_CLEANUP_PACKET_STATS_RULES,
                                              NULL,
                                              &dummy))
            {
              LOG_MSG_INFO1("TraverseCDIAndUpdate succeeded, updated entries: %d",
                            dummy,0,0);
            }
            else
            {
              LOG_MSG_ERROR("No Connected Devices Information available currently",0,0,0);
            }
            payload_prepared = true;
          }
          break;
        }
      }

      rc = qmi_csi_send_ind(qcmap_cp->clnt, QMI_QCMAP_MSGR_PACKET_STATS_STATUS_IND_V01, &ind_msg, sizeof(qcmap_msgr_packet_stats_status_ind_msg_v01));
      if ( rc != QMI_CSI_NO_ERR )
      {
        LOG_MSG_ERROR("Cannot send packet stats info to client %p, error %d", qcmap_cp, rc, 0);
      }
      else
      {
        LOG_MSG_INFO1("Sent packet stats info to client %p, error %d", qcmap_cp, rc, 0);
      }

    }
  }
} /* qmi_qcmap_msgr_packet_stats_status_ind() */


/*===========================================================================
  FUNCTION qmi_qcmap_msgr_mobile_ap_status_ind
  ===========================================================================
  DESCRIPTION
    Dispatches a unicast indication to notify registered QCMobileAP client
      about Mobile AP status.

  INPUT
    user_data
    conn_status - connection status

  RETURN VALUE
    None

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_qcmap_msgr_mobile_ap_status_ind
(
  void                           *user_data,
  qcmap_msgr_mobile_ap_status_enum_v01  conn_status
)
{
  qmi_qcmap_msgr_client_info_type      *qcmap_cp=NULL;
  qcmap_msgr_mobile_ap_status_ind_msg_v01     ind_msg;
  qmi_qcmap_msgr_status_cb_data   *cb_data;
  qmi_csi_error                     rc;
  unsigned int                      client_index=0;
  qmi_qcmap_msgr_state_info_type *qcmap_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(user_data != NULL);

  cb_data = (qmi_qcmap_msgr_status_cb_data*)user_data;
  qcmap_sp = cb_data->svc_cb;
  qcmap_cp = (qmi_qcmap_msgr_client_info_type *)(qcmap_sp->client_handle_list[0]);
  //go through all the handles and see which client is waiting for wwan status indication
  for (client_index=0;client_index < QCMAP_MSGR_MAX_CLIENT_HANDLES; client_index++)
  {
    qcmap_cp = (qmi_qcmap_msgr_client_info_type *)(qcmap_sp->client_handle_list[client_index]);
    if (qcmap_cp && qcmap_cp->qcmap_status_ind_regd)
    {
      memset(&ind_msg, 0, sizeof(qcmap_msgr_mobile_ap_status_ind_msg_v01));
      ind_msg.mobile_ap_status = conn_status;
      LOG_MSG_INFO1("\nSending Mobile AP status %d to client %p", conn_status, qcmap_cp,0);
      rc = qmi_csi_send_ind(qcmap_cp->clnt, QMI_QCMAP_MSGR_MOBILE_AP_STATUS_IND_V01, &ind_msg, sizeof(qcmap_msgr_mobile_ap_status_ind_msg_v01));
      if ( rc != QMI_CSI_NO_ERR )
      {
        LOG_MSG_ERROR("\nCannot send Mobile AP status to client %p, error %d", qcmap_cp, rc, 0);
      }
      else
      {
        LOG_MSG_INFO1("\nSent Mobile AP status to client %p, error %d", qcmap_cp, rc, 0);
      }
    }
  }
} /* qmi_qcmap_msgr_mobile_ap_status_ind() */

/*===========================================================================
  FUNCTION QMI_QCMAP_STATION_MODE_STATUS_IND()
  ===========================================================================
  DESCRIPTION
    Dispatches a unicast indication to notify registered QCMobileAP client
      about station mode.

  INPUT
    user_data
    conn_status - connection status

  RETURN VALUE
    None

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_qcmap_msgr_station_mode_status_ind
(
  void                           *user_data,
  qcmap_msgr_station_mode_status_enum_v01  conn_status
)
{
  qmi_qcmap_msgr_client_info_type      *qcmap_cp=NULL;
  qcmap_msgr_station_mode_status_ind_msg_v01     ind_msg;
  qmi_qcmap_msgr_status_cb_data   *cb_data;
  qmi_csi_error                     rc;
  unsigned int                      client_index=0;
  qmi_qcmap_msgr_state_info_type *qcmap_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(user_data != NULL);

  cb_data = (qmi_qcmap_msgr_status_cb_data*)user_data;
  qcmap_sp = cb_data->svc_cb;
  qcmap_cp = (qmi_qcmap_msgr_client_info_type *)(qcmap_sp->client_handle_list[0]);

  //go through all the handles and see which client is waiting for station mode status indication
  for (client_index=0;client_index < QCMAP_MSGR_MAX_CLIENT_HANDLES; client_index++)
  {
    qcmap_cp = (qmi_qcmap_msgr_client_info_type *)(qcmap_sp->client_handle_list[client_index]);
    if (qcmap_cp && qcmap_cp->station_mode_status_ind_regd)
    {
      memset(&ind_msg, 0, sizeof(qcmap_msgr_station_mode_status_ind_msg_v01));
      ind_msg.station_mode_status = conn_status;
      LOG_MSG_INFO1("\nSending Station mode status %d to client %p", conn_status, qcmap_cp,0);
      rc = qmi_csi_send_ind(qcmap_cp->clnt, QMI_QCMAP_MSGR_STATION_MODE_STATUS_IND_V01, &ind_msg, sizeof(qcmap_msgr_station_mode_status_ind_msg_v01));
      if ( rc != QMI_CSI_NO_ERR )
      {
        LOG_MSG_ERROR("\nCannot send Station mode ind to client %p, error %d", qcmap_cp, rc, 0);
      }
      else
      {
        LOG_MSG_INFO1("\nSent station mode ind to client %p, error %d", qcmap_cp, rc, 0);
      }
    }
  }
} /* qmi_qcmap_msgr_station_mode_status_ind() */


/*===========================================================================
  FUNCTION qmi_qcmap_msgr_cradle_mode_status_ind
  ===========================================================================
  DESCRIPTION
    Dispatches a unicast indication to notify registered QCMobileAP client
      about cradle mode status.

  INPUT
    user_data
    state - state indication enum

  RETURN VALUE
    None

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_qcmap_msgr_cradle_mode_status_ind
(
  void                           *user_data,
  qcmap_msgr_cradle_status_enum_v01     state
)
{
  qmi_qcmap_msgr_client_info_type      *qcmap_cp=NULL;
  qcmap_msgr_cradle_mode_status_ind_msg_v01     ind_msg;
  qmi_qcmap_msgr_status_cb_data   *cb_data;
  qmi_csi_error                     rc;
  unsigned int                      client_index=0;
  qmi_qcmap_msgr_state_info_type *qcmap_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(user_data);

  cb_data = (qmi_qcmap_msgr_status_cb_data*)user_data;
  qcmap_sp = cb_data->svc_cb;
  qcmap_cp = (qmi_qcmap_msgr_client_info_type *)(qcmap_sp->client_handle_list[0]);

  //go through all the handles and see which client is waiting for cradle mode status indication
  for (client_index=0;client_index < QCMAP_MSGR_MAX_CLIENT_HANDLES; client_index++)
  {
    qcmap_cp = (qmi_qcmap_msgr_client_info_type *)(qcmap_sp->client_handle_list[client_index]);
    if (qcmap_cp && qcmap_cp->cradle_mode_status_ind_regd)
    {
      memset(&ind_msg, 0, sizeof(qcmap_msgr_cradle_mode_status_ind_msg_v01));
      ind_msg.cradle_status = state;
      LOG_MSG_INFO1("\nSending Cradle mode status %d to client %p", state, qcmap_cp,0);
      rc = qmi_csi_send_ind(qcmap_cp->clnt, QMI_QCMAP_MSGR_CRADLE_MODE_STATUS_IND_V01, &ind_msg, sizeof(qcmap_msgr_cradle_mode_status_ind_msg_v01));
      if ( rc != QMI_CSI_NO_ERR )
      {
        LOG_MSG_ERROR("\nCannot send Cradle mode ind to client %p, error %d", qcmap_cp, rc, 0);
      }
      else
      {
        LOG_MSG_INFO1("\nSent Cradle mode ind to client %p, error %d", qcmap_cp, rc, 0);
      }
    }
  }
} /* qmi_qcmap_msgr_cradle_mode_status_ind() */
/*===========================================================================
  FUNCTION QMI_QCMAP_ETHERNET_MODE_STATUS_IND()

  DESCRIPTION
    Dispatches a unicast indication to notify registered QCMobileAP client
      about Ethernet mode.

  RETURN VALUE
    None

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_qcmap_msgr_ethernet_mode_status_ind
(
  void                           *user_data,
  qcmap_msgr_ethernet_status_enum_v01     state
)
{
  qmi_qcmap_msgr_client_info_type      *qcmap_cp=NULL;
  qcmap_msgr_ethernet_mode_status_ind_msg_v01     ind_msg;
  qmi_qcmap_msgr_status_cb_data   *cb_data;
  qmi_csi_error                     rc;
  unsigned int                      client_index=0;
  qmi_qcmap_msgr_state_info_type *qcmap_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(user_data);

  cb_data = (qmi_qcmap_msgr_status_cb_data*)user_data;
  qcmap_sp = cb_data->svc_cb;
  qcmap_cp = (qmi_qcmap_msgr_client_info_type *)(qcmap_sp->client_handle_list[0]);

  //go through all the handles and see which client is waiting
  // for Eth Backhaul mode status indication
  for (client_index=0;client_index < QCMAP_MSGR_MAX_CLIENT_HANDLES; client_index++)
  {
    qcmap_cp = (qmi_qcmap_msgr_client_info_type *)(qcmap_sp->client_handle_list[client_index]);
    if (qcmap_cp && qcmap_cp->ethernet_mode_status_ind_regd)
    {
      memset(&ind_msg, 0, sizeof(qcmap_msgr_ethernet_mode_status_ind_msg_v01));
      ind_msg.eth_status = state;
      LOG_MSG_INFO1("\nSending Ethernet mode status %d to client %p", state, qcmap_cp,0);
      rc = qmi_csi_send_ind(qcmap_cp->clnt,
                            QMI_QCMAP_MSGR_ETHERNET_MODE_STATUS_IND_V01,
                            &ind_msg,
                       sizeof(qcmap_msgr_ethernet_mode_status_ind_msg_v01));
      if ( rc != QMI_CSI_NO_ERR )
      {
        LOG_MSG_ERROR("\nCannot send Ethernet mode ind to client %p,"
                      "error %d", qcmap_cp, rc, 0);
      }
      else
      {
        LOG_MSG_INFO1("\nSent Ethernet mode ind to client %p, error %d",
                      qcmap_cp, rc, 0);
      }
    }
  }
} /* qmi_qcmap_msgr_ethernet_mode_status_ind() */

/*===========================================================================
  FUNCTION qmi_qcmap_msgr_bt_tethering_status_ind()

  DESCRIPTION
    Dispatches a unicast indication to notify registered QCMobileAP client
      about BT Tethering.
RETURN VALUE
    None

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_qcmap_msgr_bt_tethering_status_ind
(
  void                           *user_data,
  qcmap_msgr_bt_tethering_status_enum_v01 conn_status
)
{
  qmi_qcmap_msgr_client_info_type      *qcmap_cp=NULL;
  qcmap_msgr_bt_tethering_status_ind_msg_v01     ind_msg;
  qmi_qcmap_msgr_status_cb_data   *cb_data;
  qmi_csi_error                     rc;
  unsigned int                      client_index=0;
  qmi_qcmap_msgr_state_info_type *qcmap_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(user_data != NULL);

  cb_data = (qmi_qcmap_msgr_status_cb_data*)user_data;
  qcmap_sp = cb_data->svc_cb;
  qcmap_cp = (qmi_qcmap_msgr_client_info_type *)(qcmap_sp->client_handle_list[0]);

  //go through all the handles and see which client is waiting for BT Tethering status indication
  for (client_index=0;client_index < QCMAP_MSGR_MAX_CLIENT_HANDLES; client_index++)
  {
    qcmap_cp = (qmi_qcmap_msgr_client_info_type *)(qcmap_sp->client_handle_list[client_index]);
    if (qcmap_cp && qcmap_cp->bt_tethering_status_ind_regd)
    {
      memset(&ind_msg, 0, sizeof(qcmap_msgr_bt_tethering_status_ind_msg_v01));
      ind_msg.bt_tethering_status = conn_status;
      LOG_MSG_INFO1("\nSending BT Tethering status %d to client %p", conn_status, qcmap_cp,0);
      rc = qmi_csi_send_ind(qcmap_cp->clnt, QMI_QCMAP_MSGR_BT_TETHERING_STATUS_IND_V01,
                            &ind_msg, sizeof(qcmap_msgr_bt_tethering_status_ind_msg_v01));
      if ( rc != QMI_CSI_NO_ERR )
      {
        LOG_MSG_ERROR("\nCannot send BT Tethering ind to client %p, error %d", qcmap_cp, rc, 0);
      }
      else
      {
        LOG_MSG_INFO1("\nSent BT Tethering ind to client %p, error %d", qcmap_cp, rc, 0);
      }
    }
  }
} /* qmi_qcmap_msgr_bt_tethering_status_ind() */

/*===========================================================================
  FUNCTION qmi_qcmap_msgr_bt_wan_ind()

  DESCRIPTION
    Dispatches a unicast indication to notify registered QCMobileAP client
      about BT Tethering WAN backhaul connect/disconnect.
RETURN VALUE
    None

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_qcmap_msgr_bt_wan_ind
(
  void                           *user_data,
  qcmap_msgr_bt_tethering_wan_enum_v01 conn_status
)
{
  qmi_qcmap_msgr_client_info_type      *qcmap_cp=NULL;
  qcmap_msgr_bt_tethering_wan_ind_msg_v01     ind_msg;
  qmi_qcmap_msgr_status_cb_data   *cb_data;
  qmi_csi_error                     rc;
  unsigned int                      client_index=0;
  qmi_qcmap_msgr_state_info_type *qcmap_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(user_data != NULL);

  cb_data = (qmi_qcmap_msgr_status_cb_data*)user_data;
  qcmap_sp = cb_data->svc_cb;
  qcmap_cp = (qmi_qcmap_msgr_client_info_type *)(qcmap_sp->client_handle_list[0]);

  //go through all the handles and see which client is waiting for BT Tethering wan indication
  for (client_index=0;client_index < QCMAP_MSGR_MAX_CLIENT_HANDLES; client_index++)
  {
    qcmap_cp = (qmi_qcmap_msgr_client_info_type *)(qcmap_sp->client_handle_list[client_index]);
    if (qcmap_cp && qcmap_cp->bt_tethering_wan_ind_regd)
    {
      memset(&ind_msg, 0, sizeof(qcmap_msgr_bt_tethering_wan_ind_msg_v01));
      ind_msg.bt_tethering_wan = conn_status;
      LOG_MSG_INFO1("\nSending BT Tethering wan indication %d to client %p", conn_status, qcmap_cp,0);
      rc = qmi_csi_send_ind(qcmap_cp->clnt, QMI_QCMAP_MSGR_BT_TETHERING_WAN_IND_V01,
                            &ind_msg, sizeof(qcmap_msgr_bt_tethering_wan_ind_msg_v01));
      if ( rc != QMI_CSI_NO_ERR )
      {
        LOG_MSG_ERROR("\nCannot send BT Tethering WAN ind to client %p, error %d", qcmap_cp, rc, 0);
      }
      else
      {
        LOG_MSG_INFO1("\nSent BT Tethering WAN ind to client %p, error %d", qcmap_cp, rc, 0);
      }
    }
  }
} /* qmi_qcmap_msgr_bt_wan_ind() */


/*===========================================================================
  FUNCTION qcmap_msgr_wwan_roaming_ind()

  DESCRIPTION
    Dispatches a unicast indication to notify registered QCMobileAP client
      about change in the WWAN Backhaul Roaming status.
RETURN VALUE
    None

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
void qcmap_msgr_wwan_roaming_ind
(
  void                           *user_data,
  qcmap_msgr_wwan_roaming_status_enum_v01 wwan_roaming_status
)
{
  qmi_qcmap_msgr_client_info_type      *qcmap_cp=NULL;
  qcmap_msgr_wwan_roaming_status_ind_msg_v01     ind_msg;
  qmi_qcmap_msgr_status_cb_data   *cb_data;
  qmi_csi_error                     rc;
  unsigned int                      client_index=0;
  unsigned int                      softAp_index;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qmi_qcmap_msgr_state_info_type *qcmap_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(user_data != NULL);

  cb_data = (qmi_qcmap_msgr_status_cb_data*)user_data;
  qcmap_sp = cb_data->svc_cb;
  qcmap_cp = (qmi_qcmap_msgr_client_info_type *)(qcmap_sp->client_handle_list[0]);

  //go through all the handles and see which client is registered for wwan roaming status indication
  for (client_index=0;client_index < QCMAP_MSGR_MAX_CLIENT_HANDLES; client_index++)
  {
    qcmap_cp = (qmi_qcmap_msgr_client_info_type *)(qcmap_sp->client_handle_list[client_index]);
    if (qcmap_cp && qcmap_cp->wwan_roaming_status_ind_regd)
    {
      memset(&ind_msg, 0, sizeof(qcmap_msgr_wwan_roaming_status_ind_msg_v01));
      ind_msg.wwan_roaming_status = wwan_roaming_status;
      LOG_MSG_INFO1("\nSending wwan Roaming status ind %d to client %p", wwan_roaming_status, qcmap_cp,0);
      rc = qmi_csi_send_ind(qcmap_cp->clnt, QMI_QCMAP_MSGR_WWAN_ROAMING_STATUS_IND_V01,
                            &ind_msg, sizeof(qcmap_msgr_wwan_roaming_status_ind_msg_v01));
      if ( rc != QMI_CSI_NO_ERR )
      {
        LOG_MSG_ERROR("\nCannot send wwan Roaming status ind to client %p, error %d", qcmap_cp, rc, 0);
      }
      else
      {
        LOG_MSG_INFO1("\nSent wwan Roaming status ind to client %p, error %d", qcmap_cp, rc, 0);
      }
    }
  }
}/* qcmap_msgr_wwan_roaming_ind() */


/*===========================================================================
  FUNCTION qmi_qcmap_msgr_wwan_status_ind_reg()
  ===========================================================================
  DESCRIPTION
    qmi_qcmap_msgr_wwan_status_ind register function

  INPUT
    clnt_info       - clint information stracture
    req_handle      - request handle
    msg_id          - message id
    req_c_struct    - struct void pointer
    req_c_struct_len- struct length
    service_handle  - service handle

  RETURN VALUE
    None

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_qcmap_msgr_wwan_status_ind_reg
(
  qmi_qcmap_msgr_client_info_type      *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                     *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle)
{
  qmi_qcmap_msgr_state_info_type                    *qcmap_sp;
  qcmap_msgr_wwan_status_ind_register_req_msg_v01   *req_ptr;
  qcmap_msgr_wwan_status_ind_register_resp_msg_v01  resp_msg;
  QCMAP_Backhaul_WWAN  *QCMAP_Backhaul_WWAN_Obj = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_wwan_status_ind_register_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_wwan_status_ind_register_resp_msg_v01));

  DECLARE_HASH_MAP_ITERATOR_FOR_BACKHAUL;
  START_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash);
  while (END_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash))
  {
    QCMAP_Backhaul_WWAN_Obj = GET_BACKHAUL_WWAN_OBJ_AND_INC;
    if (QCMAP_Backhaul_WWAN_Obj)
    {
      QCMAP_Backhaul_WWAN_Obj->InsertQMIClient((uint32 )clnt_info, QMI_CLIENT_REG_WWAN_STATUS_IND, (int) TRUE);
    }
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, sizeof(qcmap_msgr_wwan_status_ind_register_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION qmi_qcmap_msgr_packet_stats_status_ind_reg()
  ===========================================================================
  DESCRIPTION
    qmi_qcmap_msgr_packet_stats_status_ind register function

  INPUT
    clnt_info       - clint information stracture
    req_handle      - request handle
    msg_id          - message id
    req_c_struct    - struct void pointer
    req_c_struct_len- struct length
    service_handle  - service handle

  RETURN VALUE
    None

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_qcmap_msgr_packet_stats_status_ind_reg
(
  qmi_qcmap_msgr_client_info_type      *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                     *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle)
{
  qmi_qcmap_msgr_state_info_type                    *qcmap_sp;
  qcmap_msgr_packet_stats_status_ind_register_req_msg_v01   *req_ptr;
  qcmap_msgr_packet_stats_status_ind_register_resp_msg_v01  resp_msg;
  qmi_csi_cb_error   rc;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_packet_stats_status_ind_register_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_packet_stats_status_ind_register_resp_msg_v01));
  rc = QMI_CSI_CB_INTERNAL_ERR;

  clnt_info->packet_stats_status_ind_regd = req_ptr->register_indication;
  LOG_MSG_INFO1 ("Client %p, reg %d for WWAN status", clnt_info, clnt_info->packet_stats_status_ind_regd,0);
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, sizeof(qcmap_msgr_packet_stats_status_ind_register_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION qmi_qcmap_msgr_mobile_ap_status_ind_reg

  DESCRIPTION
    qmi_qcmap_mobileAp status indicator

  INPUT
    clnt_info       - clint information stracture
    req_handle      - request handle
    msg_id          - message id
    req_c_struct    - struct void pointer
    req_c_struct_len- struct length
    service_handle  - service handle

  RETURN VALUE
    None

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/

static qmi_csi_cb_error  qmi_qcmap_msgr_mobile_ap_status_ind_reg
(
  qmi_qcmap_msgr_client_info_type      *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                     *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle)
{
  qmi_qcmap_msgr_state_info_type                    *qcmap_sp;
  qcmap_msgr_mobile_ap_status_ind_register_req_msg_v01   *req_ptr;
  qcmap_msgr_mobile_ap_status_ind_register_resp_msg_v01  resp_msg;
  qmi_csi_cb_error   rc;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_mobile_ap_status_ind_register_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_mobile_ap_status_ind_register_resp_msg_v01));
  rc = QMI_CSI_CB_INTERNAL_ERR;

  clnt_info->qcmap_status_ind_regd = req_ptr->register_indication;
  LOG_MSG_INFO1 ("Client %p, reg %d for Mobile AP status", clnt_info, clnt_info->qcmap_status_ind_regd,0);
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, sizeof(qcmap_msgr_mobile_ap_status_ind_register_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}


/*===========================================================================
  FUNCTION qmi_qcmap_msgr_station_mode_status_ind_reg

  DESCRIPTION
    qmi_qcmap_msgr station mode status indication function

  INPUT
    clnt_info       - clint information stracture
    req_handle      - request handle
    msg_id          - message id
    req_c_struct    - struct void pointer
    req_c_struct_len- struct length
    service_handle  - service handle

  RETURN VALUE
    None

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_qcmap_msgr_station_mode_status_ind_reg
(
  qmi_qcmap_msgr_client_info_type      *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                     *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle)
{
  qmi_qcmap_msgr_state_info_type                    *qcmap_sp;
  qcmap_msgr_station_mode_status_ind_register_req_msg_v01   *req_ptr;
  qcmap_msgr_station_mode_status_ind_register_resp_msg_v01  resp_msg;
  qmi_csi_cb_error   rc;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_station_mode_status_ind_register_req_msg_v01 *)
             req_c_struct;
  memset(&resp_msg, 0,
         sizeof(qcmap_msgr_station_mode_status_ind_register_resp_msg_v01));
  rc = QMI_CSI_CB_INTERNAL_ERR;

  clnt_info->station_mode_status_ind_regd = req_ptr->register_indication;
  LOG_MSG_INFO1 ("Client %p, reg %d for STATION Mode status",
                 clnt_info,
                 clnt_info->station_mode_status_ind_regd,0);
  qmi_csi_send_resp(req_handle,
            msg_id,
            &resp_msg,
            sizeof(qcmap_msgr_station_mode_status_ind_register_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}
/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_CRADLE_MODE_STATUS_IND()

  DESCRIPTION
    Dispatches a unicast indication to register for status indications
      about Cradle mode.

  RETURN VALUE
    None

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_qcmap_msgr_cradle_mode_status_ind_reg
(
  qmi_qcmap_msgr_client_info_type      *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                     *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle)
{
  qmi_qcmap_msgr_state_info_type                    *qcmap_sp;
  qcmap_msgr_cradle_mode_status_ind_register_req_msg_v01   *req_ptr;
  qcmap_msgr_cradle_mode_status_ind_register_resp_msg_v01  resp_msg;
  qmi_csi_cb_error   rc;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_cradle_mode_status_ind_register_req_msg_v01 *)
             req_c_struct;
  memset(&resp_msg,
         0,
         sizeof(qcmap_msgr_cradle_mode_status_ind_register_resp_msg_v01));
  rc = QMI_CSI_CB_INTERNAL_ERR;

  clnt_info->cradle_mode_status_ind_regd = req_ptr->register_indication;
  LOG_MSG_INFO1 ("Client %p, reg %d for CRADLE Mode status",
                 clnt_info,
                 clnt_info->cradle_mode_status_ind_regd,0);
  qmi_csi_send_resp(req_handle,
              msg_id,
              &resp_msg,
              sizeof(qcmap_msgr_cradle_mode_status_ind_register_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}
/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_ETHERNET_MODE_STATUS_IND_REG()

  DESCRIPTION
    Dispatches a unicast indication to register for status indications
      about Ethernet mode.

  RETURN VALUE
    None

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_qcmap_msgr_ethernet_mode_status_ind_reg
(
  qmi_qcmap_msgr_client_info_type      *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                     *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle)
{
  qmi_qcmap_msgr_state_info_type                    *qcmap_sp;
  qcmap_msgr_ethernet_mode_status_ind_register_req_msg_v01   *req_ptr;
  qcmap_msgr_ethernet_mode_status_ind_register_resp_msg_v01  resp_msg;
  qmi_csi_cb_error   rc;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_ethernet_mode_status_ind_register_req_msg_v01 *)
             req_c_struct;
  memset(&resp_msg,
         0,
         sizeof(qcmap_msgr_ethernet_mode_status_ind_register_resp_msg_v01));
  rc = QMI_CSI_CB_INTERNAL_ERR;

  clnt_info->ethernet_mode_status_ind_regd = req_ptr->register_indication;
  LOG_MSG_INFO1 ("Client %p, reg %d for Ethernet Mode status",
                 clnt_info,
                 clnt_info->ethernet_mode_status_ind_regd,0);
  qmi_csi_send_resp(req_handle,
              msg_id,
              &resp_msg,
         sizeof(qcmap_msgr_ethernet_mode_status_ind_register_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_QCMAP_WLAN_STATUS_IND()

  DESCRIPTION
    Dispatches a unicast indication to notify registered QCMobileAP client
    about activity of WLAN.

  RETURN VALUE
    None

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
   None
===========================================================================*/
void qmi_qcmap_msgr_wlan_status_ind
(
  void                                 *user_data,
  qcmap_msgr_wlan_status_ind_msg_v01   *wlan_status_ind
)
{
  qmi_qcmap_msgr_client_info_type      *qcmap_cp=NULL;
  qcmap_msgr_wlan_status_ind_msg_v01   ind_msg;
  qmi_qcmap_msgr_status_cb_data        *cb_data;
  qmi_csi_error                        rc;
  unsigned int                         client_index=0;
  qmi_qcmap_msgr_state_info_type       *qcmap_sp;
  int                                  i;
  in_addr                              ip4_addr;
  char                                 ip6_addr[INET6_ADDRSTRLEN];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert((user_data != NULL) &&(wlan_status_ind != NULL));

  cb_data = (qmi_qcmap_msgr_status_cb_data*)user_data;
  qcmap_sp = cb_data->svc_cb;
  qcmap_cp = (qmi_qcmap_msgr_client_info_type *)(qcmap_sp->client_handle_list[0]);

  //go through all the handles and see which client is waiting for station mode status indication
  for (client_index=0;client_index < QCMAP_MSGR_MAX_CLIENT_HANDLES; client_index++)
  {
    qcmap_cp = (qmi_qcmap_msgr_client_info_type *)(qcmap_sp->client_handle_list[client_index]);
    if (qcmap_cp && qcmap_cp->wlan_status_ind_regd)
    {
      memset(&ind_msg, 0, sizeof(qcmap_msgr_wlan_status_ind_msg_v01));
      memcpy(&ind_msg, wlan_status_ind, sizeof(qcmap_msgr_wlan_status_ind_msg_v01));

      ds_log_med("Sending WLAN Status IND wlan_status %d wlan_mode %d len %d to client %p \n",
                   wlan_status_ind->wlan_status, wlan_status_ind->wlan_mode,
                   wlan_status_ind->wlan_state_len, qcmap_cp);
      for (i = 0; i < wlan_status_ind->wlan_state_len; i++)
      {
        ip4_addr.s_addr =  wlan_status_ind->wlan_state[i].ip4_addr;
        inet_ntop(AF_INET6, wlan_status_ind->wlan_state[i].ip6_addr, ip6_addr, sizeof(ip6_addr));
        ds_log_med("WLAN Status IND iface num %d state %d name %s iface_type %d\n",
              i, wlan_status_ind->wlan_state[i].wlan_iface_state,
              wlan_status_ind->wlan_state[i].wlan_iface_name,
               wlan_status_ind->wlan_state[i].wlan_iface_type);
        ds_log_med("WLAN Status IND iface ip_type %d ip4 addr %s ip6_addr %s\n",
              wlan_status_ind->wlan_state[i].ip_type,
              inet_ntoa(ip4_addr),ip6_addr);
      }
      rc = qmi_csi_send_ind(qcmap_cp->clnt, QMI_QCMAP_MSGR_WLAN_STATUS_IND_V01,
                  &ind_msg, sizeof(qcmap_msgr_wlan_status_ind_msg_v01));
      if ( rc != QMI_CSI_NO_ERR )
      {
        LOG_MSG_ERROR("\nCannot send WLAN Status ind to client %p, error %d", qcmap_cp, rc, 0);
      }
      else
      {
        LOG_MSG_INFO1("\nSent WLAN Status ind to client %p, error %d", qcmap_cp, rc, 0);
      }
    }
  }
} /* qmi_qcmap_msgr_wlan_status_ind() */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_MOBILE_AP_ENABLE()

  DESCRIPTION
    Enable SoftAP functionality on modem.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_mobile_ap_enable
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_mobile_ap_enable_resp_msg_v01   resp_msg;
  qmi_qcmap_msgr_status_cb_data    *cb_data_ptr=NULL;

  int              handle;
  unsigned int     index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;

  LOG_MSG_INFO1 ("Client %p enabling Mobile AP", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_mobile_ap_enable_resp_msg_v01));

  /*---------------------------------------------------------------------
    Enable SoftAP
  ---------------------------------------------------------------------*/

  if (qcmap_handle.cb_ptr == NULL)
  {
    cb_data_ptr = (qmi_qcmap_msgr_status_cb_data*)malloc(
                         sizeof(qmi_qcmap_msgr_status_cb_data));

    if (cb_data_ptr == NULL)
    {
      LOG_MSG_ERROR("Out of mem for WWAN cb data", 0, 0, 0);
      return QMI_CSI_CB_NO_MEM;
    }

    cb_data_ptr->svc_cb = qcmap_sp;
    cb_data_ptr->map_instance = QCMAP_MSGR_SOFTAP_HANDLE;
    cb_data_ptr->wwan_cb  = qmi_qcmap_msgr_wwan_status_ind;
    cb_data_ptr->packet_stats_cb = qmi_qcmap_msgr_packet_stats_status_ind;
    cb_data_ptr->qcmap_cb = qmi_qcmap_msgr_mobile_ap_status_ind;
    cb_data_ptr->sta_cb = qmi_qcmap_msgr_station_mode_status_ind;
    cb_data_ptr->cradle_cb = qmi_qcmap_msgr_cradle_mode_status_ind;
    cb_data_ptr->ethernet_cb = qmi_qcmap_msgr_ethernet_mode_status_ind;
    cb_data_ptr->wlan_status_cb = qmi_qcmap_msgr_wlan_status_ind;
    cb_data_ptr->bt_tethering_status_cb = qmi_qcmap_msgr_bt_tethering_status_ind;
    cb_data_ptr->bt_wan_cb = qmi_qcmap_msgr_bt_wan_ind;
    cb_data_ptr->backhaul_status_cb = qmi_qcmap_msgr_backhaul_status_ind;
    cb_data_ptr->wwan_roaming_cb = qcmap_msgr_wwan_roaming_ind;

    qcmap_handle.cb_ptr = cb_data_ptr;
  }

  if(!clnt_info->client_enabled)
  {
    if(qcmap_sp->client_ref_count > 0)
    {
      clnt_info->client_enabled = TRUE;
      qcmap_sp->client_ref_count++;
      index = QCMAP_MSGR_SOFTAP_HANDLE;
      LOG_MSG_INFO1("Enabled MobileAP inst %d",
                  index,0,0);
      resp_msg.mobile_ap_handle_valid = TRUE;
      resp_msg.mobile_ap_handle = index;
      qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                        sizeof(qcmap_msgr_mobile_ap_enable_resp_msg_v01));

      return QMI_CSI_CB_NO_ERR;
    }
    else if(qcmap_sp->client_ref_count == 0)
    {
      if ((!QCMAP_ConnectionManager::get_mobileap_enable_cfg_flag()) ||
          (!QCMAP_ConnectionManager::is_cm_enabled))
      {
        if (QCMAP_ConnectionManager::Enable(&handle, &qcmap_handle, &qmi_err_num))
        {
          clnt_info->client_enabled = TRUE;
          qcmap_sp->client_ref_count++;
          qcmap_handle.handle = handle;
          LOG_MSG_INFO1("MobileAP Enable succeeds.\n",0,0,0);
        }
        else
        {
          LOG_MSG_INFO1("MobileAP Enable fails.\n",0,0,0);
          resp_msg.mobile_ap_handle_valid = false;
          resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
          resp_msg.resp.error = qmi_err_num;
          qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                          sizeof(qcmap_msgr_mobile_ap_enable_resp_msg_v01));
          return QMI_CSI_CB_NO_ERR;
        }
      }
      else
      {
        clnt_info->client_enabled = TRUE;
        qcmap_sp->client_ref_count++;
        LOG_MSG_INFO1("MobileAP Enable succeeds on bootup.\n",0,0,0);
      }

      index = QCMAP_MSGR_SOFTAP_HANDLE;
      LOG_MSG_INFO1("Enabled MobileAP inst %d",
                    index,0,0);
      resp_msg.mobile_ap_handle_valid = TRUE;
      resp_msg.mobile_ap_handle = index;
      qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                        sizeof(qcmap_msgr_mobile_ap_enable_resp_msg_v01));

      return QMI_CSI_CB_NO_ERR;
    }
  }
  else
  {
    LOG_MSG_ERROR("The client has already enabled MobileAP", 0, 0, 0);
    resp_msg.mobile_ap_handle_valid = false;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_NO_EFFECT_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_mobile_ap_enable_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  return QMI_CSI_CB_NO_ERR;

} /* qmi_qcmap_msgr_mobile_ap_enable() */


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_MOBILE_AP_DISABLE()

  DESCRIPTION
    Disable SoftAP functionality on modem.

  PARAMETERS
    sp:          QMI_QCMAP's service instance state pointer for this qmi link
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
static qmi_csi_cb_error  qmi_qcmap_msgr_mobile_ap_disable
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type            *qcmap_sp;
  qcmap_msgr_mobile_ap_disable_req_msg_v01   *req_ptr;
  qcmap_msgr_mobile_ap_disable_resp_msg_v01   resp_msg;
  qmi_qcmap_msgr_softap_handle_type         *map_handle;
  qcmap_msgr_upnp_mode_enum_v01             status = 0;
  int                return_val, err_num=0;
  unsigned int             index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  int                client_count = 1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_mobile_ap_disable_req_msg_v01*)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_mobile_ap_disable_resp_msg_v01));

  LOG_MSG_INFO1("qmi_qcmap_msgr_mobile_ap_disable::enter",0,0,0);
  /*---------------------------------------------------------------------
    Disable SOftAP. This would trigger all RmNet calls to stop.
  ---------------------------------------------------------------------*/
  index = req_ptr->mobile_ap_handle;
  if (index != QCMAP_MSGR_SOFTAP_HANDLE)
  {
    LOG_MSG_ERROR("Incorrect handle passed %d",req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_mobile_ap_disable_resp_msg_v01));

    return QMI_CSI_CB_NO_ERR;
  }

  /* Increment Client count to 2 if UPnP is running since it has it's own mobile_ap client */
  if (QCMAP_MediaService::GetUPNPStatus(&status, &qmi_err_num) && status == QCMAP_MSGR_UPNP_MODE_UP_V01)
  {
    client_count = 2;
  }

  map_handle = &qcmap_handle;
  if (map_handle->handle == 0)
  {
    LOG_MSG_ERROR("MobileAP inst %d already disabled",index,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_NO_EFFECT_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_mobile_ap_disable_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p disabling MobileAP inst %d",
                 clnt_info, index,0);
  LOG_MSG_INFO1("qcmap_sp->client_ref_count = %d , client_count = %d ",qcmap_sp->client_ref_count,client_count,0);

  // If client is enabled but ref count = 0, implies mobileap disable is already called on the server side
  if ((clnt_info->client_enabled) && (qcmap_sp->client_ref_count == 0))
  {
    clnt_info->client_enabled = false;
    resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
    resp_msg.resp.error = QMI_ERR_NONE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                            sizeof(qcmap_msgr_mobile_ap_disable_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if(clnt_info->client_enabled)
  {
    if(qcmap_sp->client_ref_count > client_count)
    {
      qcmap_sp->client_ref_count--;
      clnt_info->client_enabled = false;
      resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;

      /* Set the error to QMI_ERR_NO_EFFECT_V01 to indicate the clients
         (esp. IoE Clients) that MobileAP is disabled. */
      resp_msg.resp.error = QMI_ERR_NO_EFFECT_V01;
      qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                            sizeof(qcmap_msgr_mobile_ap_disable_resp_msg_v01));
      return QMI_CSI_CB_NO_ERR;
    }
    else if(qcmap_sp->client_ref_count == client_count)
    {
      if (return_val=QCMAP_ConnectionManager::Disable(&err_num, &qmi_err_num))
      {
        if (err_num == QCMAP_CM_EALDDISCONN)
        {
          qcmap_sp->client_ref_count = qcmap_sp->client_ref_count - client_count;
          LOG_MSG_INFO1("Client reference count %d",qcmap_sp->client_ref_count,0,0);
          clnt_info->client_enabled = false;
          resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
          resp_msg.resp.error = QMI_ERR_NONE_V01;
          qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                            sizeof(qcmap_msgr_mobile_ap_disable_resp_msg_v01));
          map_handle->handle = 0;
          return QMI_CSI_CB_NO_ERR;
        }
        LOG_MSG_INFO1("MobileAP Disable in progress.",0,0,0);
        resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
        resp_msg.resp.error = QMI_ERR_NONE_V01;

        /*.If backhaul is not connected, Mobileap will be disabled instantly.
           And since call back function is being called much before the response
           pending flag is set to TRUE, responses are not sent to the client.
           Hence, we set qcmap_disable_resp_msg_v01.resp.error to
           QMI_ERR_NO_EFFECT_V01, so that the caller of this function sends a
           response back to the client. Used for IoE 9x25.
          */
        if (err_num == QCMAP_CM_DISCONNECTED)
          resp_msg.resp.error = QMI_ERR_NO_EFFECT_V01;
      }
      else
      {
        LOG_MSG_ERROR("MobileAP Disable request fails.",0,0,0);
        resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
        resp_msg.resp.error = qmi_err_num;
        qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                          sizeof(qcmap_msgr_mobile_ap_disable_resp_msg_v01));
        return QMI_CSI_CB_NO_ERR;
      }

      qcmap_sp->client_ref_count = qcmap_sp->client_ref_count - client_count;
      LOG_MSG_INFO1("Client reference count %d",qcmap_sp->client_ref_count,0,0);
      clnt_info->client_enabled = false;
      qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                        sizeof(qcmap_msgr_mobile_ap_disable_resp_msg_v01));
      map_handle->handle = 0;
      return QMI_CSI_CB_NO_ERR;
    }
  }
  else
  {
    LOG_MSG_ERROR("MobileAP not enabled by client, but trying to disable!!", 0, 0, 0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_mobile_ap_disable_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_mobile_ap_disable() */

/*===========================================================================
  FUNCTION QMI_QCMAPI_ENABLE_IPV4()

  DESCRIPTION
    Enable IPV4 Functionality.

  PARAMETERS
    sp:          QMI_QCMAP's service instance state pointer for this qmi link
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
static qmi_csi_cb_error  qmi_qcmap_msgr_enable_ipv4
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type       *qcmap_sp;
  qcmap_msgr_enable_ipv4_req_msg_v01 *req_ptr;
  qcmap_msgr_enable_ipv4_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  boolean            retval;
  unsigned int             index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_Backhaul* QcMapBackhaul = GET_DEFAULT_BACKHAUL_OBJECT();
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_enable_ipv4_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_enable_ipv4_resp_msg_v01));

  /*---------------------------------------------------------------------
      Enable IPV4
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_enable_ipv4_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p Enabling IPV4 in MobileAP inst %d", clnt_info, index,0);

if ( ! ( (QcMapBackhaul != NULL)   &&
            (retval = QcMapBackhaul->EnableIPV4(&qmi_err_num)) ))
  {
    LOG_MSG_ERROR("Enabling IPV4 failed",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    if(QcMapBackhaul == NULL)
      resp_msg.resp.error =  QMI_ERR_INTERNAL_V01;
    else
      resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_enable_ipv4_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_enable_ipv4_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_enable_ipv4() */

/*===========================================================================
  FUNCTION QMI_QCMAPI_DISABLE_IPV4()

  DESCRIPTION
    Disable IPV4 Functionality.

  PARAMETERS
    sp:          QMI_QCMAP's service instance state pointer for this qmi link
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
static qmi_csi_cb_error  qmi_qcmap_msgr_disable_ipv4
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type       *qcmap_sp;
  qcmap_msgr_disable_ipv4_req_msg_v01 *req_ptr;
  qcmap_msgr_disable_ipv4_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  boolean            retval;
  unsigned int             index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_Backhaul* QcMapBackhaul = GET_DEFAULT_BACKHAUL_OBJECT();
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_disable_ipv4_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_disable_ipv4_resp_msg_v01));

  /*---------------------------------------------------------------------
      Disable IPV4
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_disable_ipv4_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p Disabling IPV4 in MobileAP inst %d", clnt_info, index,0);

  if (! ( (QcMapBackhaul != NULL) && (retval = QcMapBackhaul->DisableIPV4(&qmi_err_num) ) ) )
  {
    LOG_MSG_ERROR("Disabling IPV4 failed",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    if(QcMapBackhaul == NULL)
      resp_msg.resp.error =  QMI_ERR_INTERNAL_V01;
    else
      resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_disable_ipv4_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_disable_ipv4_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_disable_ipv4() */

/*===========================================================================
  FUNCTION QMI_QCMAPI_ENABLE_IPV6()

  DESCRIPTION
    Enable IPV6 Functionality.

  PARAMETERS
    sp:          QMI_QCMAP's service instance state pointer for this qmi link
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
static qmi_csi_cb_error  qmi_qcmap_msgr_enable_ipv6
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type       *qcmap_sp;
  qcmap_msgr_enable_ipv6_req_msg_v01 *req_ptr;
  qcmap_msgr_enable_ipv6_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  boolean            retval;
  unsigned int             index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_Backhaul* QcMapBackhaul = GET_DEFAULT_BACKHAUL_OBJECT();
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_enable_ipv6_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_enable_ipv6_resp_msg_v01));

  /*---------------------------------------------------------------------
      Disable IPV6
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_enable_ipv6_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p Enabling IPV6 in MobileAP inst %d", clnt_info, index,0);
  CHECK_QCMAP_OBJ(QcMapBackhaul, resp_msg);

  if (!(retval = QcMapBackhaul->EnableIPV6(&qmi_err_num)))
  {
    LOG_MSG_ERROR("Enabling IPV6 failed",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_enable_ipv6_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_enable_ipv6_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_enable_ipv6() */

/*===========================================================================
  FUNCTION QMI_QCMAPI_DISABLE_IPV6()

  DESCRIPTION
    Disable IPV6 Functionality.

  PARAMETERS
    sp:          QMI_QCMAP's service instance state pointer for this qmi link
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
static qmi_csi_cb_error  qmi_qcmap_msgr_disable_ipv6
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type       *qcmap_sp;
  qcmap_msgr_disable_ipv6_req_msg_v01 *req_ptr;
  qcmap_msgr_disable_ipv6_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  boolean            retval;
  unsigned int             index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_Backhaul* QcMapBackhaul = GET_DEFAULT_BACKHAUL_OBJECT();
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_disable_ipv6_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_disable_ipv6_resp_msg_v01));

  /*---------------------------------------------------------------------
      Disable IPV6
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_disable_ipv6_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p Disabling IPV6 in MobileAP inst %d", clnt_info, index,0);

  if (! ( (QcMapBackhaul != NULL) && (retval = QcMapBackhaul->DisableIPV6(&qmi_err_num)) ))
  {
    LOG_MSG_ERROR("Disabling IPV6 failed",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    if(QcMapBackhaul == NULL)
      resp_msg.resp.error =  QMI_ERR_INTERNAL_V01;
    else
      resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_disable_ipv6_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_disable_ipv6_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_disable_ipv6() */


/*===========================================================================
  FUNCTION QMI_QCMAPI_TEAR_DOWN_WWAN()

  DESCRIPTION
    Tear down currently active WWAN connection.

  PARAMETERS
    sp:          QMI_QCMAP's service instance state pointer for this qmi link
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
static qmi_csi_cb_error  qmi_qcmap_msgr_tear_down_wwan
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type       *qcmap_sp;
  qcmap_msgr_tear_down_wwan_req_msg_v01 *req_ptr;
  qcmap_msgr_tear_down_wwan_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qcmap_msgr_wwan_call_type_v01 call_type = QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01;
  qmi_csi_cb_error   rc;
  int                err_num=0;
  unsigned int             index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_Backhaul_WWAN  *QCMAP_Backhaul_WWAN_Obj = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_tear_down_wwan_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_tear_down_wwan_resp_msg_v01));
  rc = QMI_CSI_CB_INTERNAL_ERR;

  /*---------------------------------------------------------------------
      Tear down WWAN for this client
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_tear_down_wwan_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if ( req_ptr->call_type_valid )
  {
    call_type = req_ptr->call_type;
  }

  if ( req_ptr->profile_handle_valid )
  {
    LOG_MSG_INFO1 (" Get WWAN Object for profile_handle: %d", req_ptr->profile_handle, 0, 0);
    QCMAP_Backhaul_WWAN_Obj = GET_BACKHAUL_WWAN_OBJECT(req_ptr->profile_handle);
  }
  else
  {
    QCMAP_Backhaul_WWAN_Obj = GET_BACKHAUL_WWAN_OBJ_FOR_QMI_CLIENT(clnt_info);
  }
  CHECK_QCMAP_OBJ(QCMAP_Backhaul_WWAN_Obj, resp_msg);

  LOG_MSG_INFO1 ("Client %p ending WWAN call in MobileAP inst %d", clnt_info, index,0);


  if (!(QCMAP_Backhaul_WWAN_Obj->DisconnectBackHaul(call_type, &err_num, &qmi_err_num)))
  {
    LOG_MSG_ERROR("WWAN teardown failed",0,0,0);
    if ( call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01 )
    {
      resp_msg.conn_status = QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTING_FAIL_V01;
    }
    else
    {
      resp_msg.conn_status = QCMAP_MSGR_WWAN_STATUS_DISCONNECTING_FAIL_V01;
    }
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_tear_down_wwan_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if (err_num == QCMAP_CM_EALDDISCONN)
  {
    LOG_MSG_INFO1("WWAN Already disconnected!!",0,0,0);
    if ( call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01 )
    {
      resp_msg.conn_status = QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTED_V01;
    }
    else
    {
      resp_msg.conn_status = QCMAP_MSGR_WWAN_STATUS_DISCONNECTED_V01;
    }
    resp_msg.conn_status_valid = TRUE;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_tear_down_wwan_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  if ( call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01 )
  {
    resp_msg.conn_status = QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTING_V01;
  }
  else
  {
    resp_msg.conn_status = QCMAP_MSGR_WWAN_STATUS_DISCONNECTING_V01;
  }
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_tear_down_wwan_resp_msg_v01));

  /* Insert QMI client for WWAN TEAR_DOWN indication */
  QCMAP_Backhaul_WWAN_Obj->InsertQMIClient((uint32 )clnt_info,
                                            QMI_CLIENT_REG_WWAN_BRING_UP_DOWN_EVENT,
                                            QMI_QCMAP_MSGR_TEAR_DOWN_WWAN_IND_V01);

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_tear_down_wwan() */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_CM_BRING_UP_WWAN()

  DESCRIPTION
    Bring up WWAN connection in MobileAP mode.

  PARAMETERS
    sp:          QMI_QCMAP's service instance state pointer for this qmi link
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
static qmi_csi_cb_error  qmi_qcmap_msgr_bring_up_wwan
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type        *qcmap_sp;
  qcmap_msgr_bring_up_wwan_req_msg_v01  *req_ptr;
  qcmap_msgr_bring_up_wwan_resp_msg_v01  resp_msg;
  qmi_qcmap_msgr_softap_handle_type     *map_handle;
  qcmap_msgr_wwan_call_type_v01 call_type = QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01;
  qmi_csi_cb_error   rc;
  boolean            retval;
  unsigned int       index;
  int                err_num=0;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_Backhaul_WWAN  *QCMAP_Backhaul_WWAN_Obj = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_bring_up_wwan_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_bring_up_wwan_resp_msg_v01));
  rc = QMI_CSI_CB_INTERNAL_ERR;

  /*---------------------------------------------------------------------
        Start the network interface for this client
  ---------------------------------------------------------------------*/
  index = req_ptr->mobile_ap_handle;
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_bring_up_wwan_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if ( req_ptr->call_type_valid )
  {
    call_type = req_ptr->call_type;
  }

  if ( req_ptr->profile_handle_valid )
  {
    LOG_MSG_INFO1 (" Get WWAN Object using profile_handle ",0,0,0);
    QCMAP_Backhaul_WWAN_Obj = GET_BACKHAUL_WWAN_OBJECT(req_ptr->profile_handle);
  }
  else
  {
    QCMAP_Backhaul_WWAN_Obj = GET_BACKHAUL_WWAN_OBJ_FOR_QMI_CLIENT(clnt_info);
  }
  CHECK_QCMAP_OBJ(QCMAP_Backhaul_WWAN_Obj, resp_msg);

  LOG_MSG_INFO1 ("Client %p starting WWAN call in MobileAP inst %d",
                 clnt_info, index,0);


  if (!(retval=QCMAP_Backhaul_WWAN_Obj->ConnectBackHaul(call_type, &err_num, &qmi_err_num)))
  {
    LOG_MSG_ERROR("WWAN bringup failed calltype %d ret %d qmierr %d",
                   call_type,retval,qmi_err_num);
    if ( call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01 )
    {
      resp_msg.conn_status = QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTING_FAIL_V01;
    }
    else
    {
      resp_msg.conn_status = QCMAP_MSGR_WWAN_STATUS_CONNECTING_FAIL_V01;
    }
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_bring_up_wwan_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if (err_num == QCMAP_CM_EALDCONN)
  {
    LOG_MSG_INFO1("WWAN Already connected!!",0,0,0);
    if ( call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01 )
    {
      resp_msg.conn_status = QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTED_V01;
    }
    else
    {
      resp_msg.conn_status = QCMAP_MSGR_WWAN_STATUS_CONNECTED_V01;
    }
    resp_msg.conn_status_valid = TRUE;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_bring_up_wwan_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if ( call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01 )
  {
    resp_msg.conn_status = QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTING_V01;
  }
  else
  {
    resp_msg.conn_status = QCMAP_MSGR_WWAN_STATUS_CONNECTING_V01;
  }
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_bring_up_wwan_resp_msg_v01));

  QCMAP_Backhaul_WWAN_Obj->InsertQMIClient((uint32 )clnt_info,
                                            QMI_CLIENT_REG_WWAN_BRING_UP_DOWN_EVENT,
                                            QMI_QCMAP_MSGR_BRING_UP_WWAN_IND_V01);

  /* Insert QMI client for WWAN BRING_UP indication */
  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_bring_up_wwan() */


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_ENABLE_WLAN()

  DESCRIPTION
    Enable WLAN functionality on Apps.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_enable_wlan
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qcmap_msgr_enable_wlan_req_msg_v01    *req_ptr;
  qcmap_msgr_enable_wlan_resp_msg_v01   resp_msg;

  unsigned int     index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  boolean privileged_client = false;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_enable_wlan_resp_msg_v01));

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_enable_wlan_req_msg_v01 *)req_c_struct;

  if ( req_ptr->privileged_client_valid )
  {
    privileged_client = req_ptr->privileged_client;
  }

  /* Skip the validation for privileged client. */
  index = (req_ptr->mobile_ap_handle);
  if (!privileged_client &&
      (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0))
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_enable_wlan_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p enabling WLAN", clnt_info,0,0);

  /*---------------------------------------------------------------------
    Enable WLAN
  ---------------------------------------------------------------------*/
  if (QCMAP_WLAN::EnableWLAN(&qmi_err_num, false, privileged_client))
  {
    LOG_MSG_INFO1("Enable WLAN succeeds.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Enable WLAN fails.\n",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_enable_wlan_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1("\nEnabled WLAN",0,0,0);
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_enable_wlan_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_qcmap_msgr_enable_wlan() */


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_DISABLE_WLAN()

  DESCRIPTION
    Disable WLAN functionality on Apps.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_disable_wlan
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type        *qcmap_sp;
  qmi_qcmap_msgr_softap_handle_type     *map_handle;
  qcmap_msgr_disable_wlan_req_msg_v01   *req_ptr;
  qcmap_msgr_disable_wlan_resp_msg_v01   resp_msg;

  unsigned int     index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  boolean privileged_client = false;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_disable_wlan_resp_msg_v01));

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_disable_wlan_req_msg_v01 *)req_c_struct;

  if ( req_ptr->privileged_client_valid )
  {
    privileged_client = req_ptr->privileged_client;
  }

  /* Skip the validation for privileged client. */
  index = (req_ptr->mobile_ap_handle);
  if (!privileged_client &&
      (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0))
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_disable_wlan_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p disabling WLAN", clnt_info,0,0);



  /*---------------------------------------------------------------------
    Disable WLAN
  ---------------------------------------------------------------------*/

  if (QCMAP_WLAN::DisableWLAN( &qmi_err_num, privileged_client ))
  {
    LOG_MSG_INFO1("Disable WLAN succeeds.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Disable WLAN fails.\n",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_disable_wlan_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1("\nDisabled WLAN",0,0,0);
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_disable_wlan_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_qcmap_msgr_disable_wlan() */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_ADD_STATIC_NAT_ENTRY()

  DESCRIPTION
    Add Static NAT entry on Apps.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_add_static_nat_entry
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_add_static_nat_entry_req_msg_v01 *req_ptr;
  qcmap_msgr_add_static_nat_entry_resp_msg_v01 resp_msg;
  qcmap_cm_port_fwding_entry_conf_t nat_entry;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_NATALG* QcMapNatAlg = NULL;

  unsigned int     index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_add_static_nat_entry_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p static NAT entry", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_add_static_nat_entry_resp_msg_v01));

  /*---------------------------------------------------------------------
   Add Static NAT Entry
  ---------------------------------------------------------------------*/

  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_add_static_nat_entry_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  nat_entry.port_fwding_private_ip = req_ptr->snat_entry_config.private_ip_addr;
  nat_entry.port_fwding_private_port = req_ptr->snat_entry_config.private_port;
  nat_entry.port_fwding_global_port = req_ptr->snat_entry_config.global_port;
  nat_entry.port_fwding_protocol = req_ptr->snat_entry_config.protocol;

  QcMapNatAlg = GET_NATALG_OBJ_FOR_QMI_CLIENT(clnt_info);
  CHECK_QCMAP_OBJ(QcMapNatAlg, resp_msg);

  if (QcMapNatAlg->AddStaticNatEntry(&nat_entry, &qmi_err_num))
  {
    LOG_MSG_INFO1("Added SNAT entry successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Static NAT entry add failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_add_static_nat_entry_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_add_static_nat_entry_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_add_static_nat_entry() */


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_DELETE_STATIC_NAT_ENTRY()

  DESCRIPTION
    Delete Static NAT entry on Apps.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_delete_static_nat_entry
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_delete_static_nat_entry_req_msg_v01 *req_ptr;
  qcmap_msgr_delete_static_nat_entry_resp_msg_v01 resp_msg;
  qcmap_cm_port_fwding_entry_conf_t nat_entry;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;

  unsigned int     index;
  QCMAP_NATALG* QcMapNatAlg = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_delete_static_nat_entry_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p deleting static NAT entry", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_delete_static_nat_entry_resp_msg_v01));

  /*---------------------------------------------------------------------
   Delete Static NAT Entry
  ---------------------------------------------------------------------*/

  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_delete_static_nat_entry_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  nat_entry.port_fwding_private_ip = req_ptr->snat_entry_config.private_ip_addr;
  nat_entry.port_fwding_private_port = req_ptr->snat_entry_config.private_port;
  nat_entry.port_fwding_global_port = req_ptr->snat_entry_config.global_port;
  nat_entry.port_fwding_protocol = req_ptr->snat_entry_config.protocol;

  QcMapNatAlg = GET_NATALG_OBJ_FOR_QMI_CLIENT(clnt_info);
  CHECK_QCMAP_OBJ(QcMapNatAlg, resp_msg);

  if (QcMapNatAlg->DeleteStaticNatEntry(&nat_entry, &qmi_err_num))
  {
    LOG_MSG_INFO1("Deleted SNAT entry successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Static NAT entry delete failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_delete_static_nat_entry_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_delete_static_nat_entry_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_delete_static_nat_entry() */


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
static qmi_csi_cb_error  qmi_qcmap_msgr_get_static_nat_entries
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_get_static_nat_entries_req_msg_v01 *req_ptr;
  qcmap_msgr_get_static_nat_entries_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_NATALG* QcMapNatAlg = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_static_nat_entries_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p get static NAT entries", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_static_nat_entries_resp_msg_v01));

  QcMapNatAlg = GET_NATALG_OBJ_FOR_QMI_CLIENT(clnt_info);

  /* Check if QcMapNatAlg is NULL and retun failure */
  CHECK_QCMAP_OBJ(QcMapNatAlg, resp_msg);

  /*---------------------------------------------------------------------
   Get Static NAT Entries
  ---------------------------------------------------------------------*/
  if (QcMapNatAlg && QcMapNatAlg->GetStaticNatEntries((qcmap_msgr_snat_entry_config_v01 *)&resp_msg.snat_config[0], &resp_msg.snat_config_len, &qmi_err_num))
  {
    LOG_MSG_INFO1("Get SNAT entry succeeded.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("No Static NAT entries configured",0,0,0);

    resp_msg.snat_config_valid = false;
    if(QcMapNatAlg == NULL)
      resp_msg.resp.error =  QMI_ERR_INTERNAL_V01;
    else
      resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_static_nat_entries_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  resp_msg.snat_config_valid = true;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_static_nat_entries_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_static_nat_entries() */


/*===========================================================================
  FUNCTION qmi_qcmap_msgr_set_dmz()

  DESCRIPTION
    Add DMZ on Apps.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_set_dmz
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_set_dmz_req_msg_v01 *req_ptr;
  qcmap_msgr_set_dmz_resp_msg_v01 resp_msg;
  uint32 dmz_ip=0;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;

  unsigned int     index;
  QCMAP_NATALG* QcMapNatAlg = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_set_dmz_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p dmz", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_dmz_resp_msg_v01));

  QcMapNatAlg = GET_NATALG_OBJ_FOR_QMI_CLIENT(clnt_info);

  /* Check if QcMapNatAlg is NULL and retun failure */
  CHECK_QCMAP_OBJ(QcMapNatAlg, resp_msg);

  /*---------------------------------------------------------------------
   Add DMZ
  ---------------------------------------------------------------------*/

  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_dmz_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  dmz_ip = req_ptr->dmz_ip_addr;

  if (QcMapNatAlg && QcMapNatAlg->AddDMZ(dmz_ip, &qmi_err_num))
  {
    LOG_MSG_INFO1("Added DMZ successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("DMZ add failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    if(QcMapNatAlg == NULL)
      resp_msg.resp.error =  QMI_ERR_INTERNAL_V01;
    else
      resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_dmz_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_dmz_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_set_dmz() */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_DELETE_DMZ()

  DESCRIPTION
    Delete DMZ on Apps.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_delete_dmz
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_delete_dmz_req_msg_v01 *req_ptr;
  qcmap_msgr_delete_dmz_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;

  unsigned int     index;
  QCMAP_NATALG* QcMapNatAlg = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_delete_dmz_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p deleting dmz", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_delete_dmz_resp_msg_v01));

  QcMapNatAlg = GET_NATALG_OBJ_FOR_QMI_CLIENT(clnt_info);

  /*---------------------------------------------------------------------
   Delete DMZ
  ---------------------------------------------------------------------*/

 index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_delete_dmz_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if (QcMapNatAlg && QcMapNatAlg->DeleteDMZ(&qmi_err_num))
  {
    LOG_MSG_INFO1("Deleted DMZ successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("DMZ delete failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    if(QcMapNatAlg == NULL)
      resp_msg.resp.error =  QMI_ERR_INTERNAL_V01;
    else
      resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_delete_dmz_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_delete_dmz_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_delete_dmz() */


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_DMZ()

  DESCRIPTION
    Get DMZ on Apps.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_get_dmz
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_get_dmz_req_msg_v01 *req_ptr;
  qcmap_msgr_get_dmz_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_NATALG* QcMapNatAlg = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_dmz_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p get dmz", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_dmz_resp_msg_v01));

  QcMapNatAlg = GET_NATALG_OBJ_FOR_QMI_CLIENT(clnt_info);

  /* Check if QcMapNatAlg is NULL and retun failure */
  CHECK_QCMAP_OBJ(QcMapNatAlg, resp_msg);

  /*---------------------------------------------------------------------
   Get DMZ
  ---------------------------------------------------------------------*/
  if (QcMapNatAlg && QcMapNatAlg->GetDMZ(&resp_msg.dmz_ip_addr, &qmi_err_num))
  {
    LOG_MSG_INFO1("Get DMZ succeeded.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Get DMZ failed!!",0,0,0);
    resp_msg.dmz_ip_addr_valid = false;
    if(QcMapNatAlg == NULL)
      resp_msg.resp.error =  QMI_ERR_INTERNAL_V01;
    else
      resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_dmz_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  resp_msg.dmz_ip_addr_valid = true;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_dmz_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_dmz() */

/*===========================================================================

  FUNCTION QMI_QCMAP_MSGR_GET_WWAN_STATS()

  DESCRIPTION
    Get wwan stats on Apps.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_get_wwan_stats
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_get_wwan_stats_req_msg_v01 *req_ptr;
  qcmap_msgr_get_wwan_stats_resp_msg_v01 resp_msg;
  qcmap_msgr_ip_family_enum_v01 ip_family;
  qcmap_msgr_wwan_statistics_type_v01 wwan_stats;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  unsigned int     index;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  QCMAP_Backhaul_WWAN  *QCMAP_Backhaul_WWAN_Obj = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_wwan_stats_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p get wwan stats", clnt_info,0,0);

  memset(&wwan_stats, 0, sizeof(qcmap_msgr_wwan_statistics_type_v01));
  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_wwan_stats_resp_msg_v01));

  /*---------------------------------------------------------------------

  ---------------------------------------------------------------------*/

 index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_get_wwan_stats_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  ip_family = req_ptr->ip_family;

  QCMAP_Backhaul_WWAN_Obj = GET_BACKHAUL_WWAN_OBJ_FOR_QMI_CLIENT(clnt_info);
  CHECK_QCMAP_OBJ(QCMAP_Backhaul_WWAN_Obj, resp_msg);

  if (QCMAP_Backhaul_WWAN_Obj->GetWWANStatistics(ip_family, &wwan_stats, &qmi_err_num))
  {
    LOG_MSG_INFO1("Got WWAN Stats successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Get WWAN Stats get failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_wwan_stats_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  resp_msg.wwan_stats_valid = TRUE;
  memcpy((void *)&resp_msg.wwan_stats, (void *)&wwan_stats, sizeof(qcmap_msgr_wwan_statistics_type_v01));

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_wwan_stats_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_wwan_stats() */


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_RESET_WWAN_STATS()

  DESCRIPTION
    Reset wwan stats on Apps.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_reset_wwan_stats
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_reset_wwan_stats_req_msg_v01 *req_ptr;
  qcmap_msgr_reset_wwan_stats_resp_msg_v01 resp_msg;
  qcmap_msgr_ip_family_enum_v01 ip_family;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  unsigned int     index;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  QCMAP_Backhaul_WWAN  *QCMAP_Backhaul_WWAN_Obj = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_reset_wwan_stats_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p reset wwan stats", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_reset_wwan_stats_resp_msg_v01));

  /*---------------------------------------------------------------------

  ---------------------------------------------------------------------*/

 index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_reset_wwan_stats_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  ip_family = req_ptr->ip_family;

  QCMAP_Backhaul_WWAN_Obj = GET_BACKHAUL_WWAN_OBJ_FOR_QMI_CLIENT(clnt_info);
  CHECK_QCMAP_OBJ(QCMAP_Backhaul_WWAN_Obj, resp_msg);

  if (QCMAP_Backhaul_WWAN_Obj->ResetWWANStatistics(ip_family, &qmi_err_num))
  {
    LOG_MSG_INFO1("Reset WWAN Stats successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Reset WWAN Stats failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_reset_wwan_stats_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_reset_wwan_stats_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_reset_wwan_stats() */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_IPSEC_VPN_PT()

  DESCRIPTION
    Get IPSEC VPN Passthrough flag value on Apps.

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
static qmi_csi_cb_error qmi_qcmap_msgr_get_ipsec_vpn_pt
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_get_ipsec_vpn_pass_through_req_msg_v01 *req_ptr;
  qcmap_msgr_get_ipsec_vpn_pass_through_resp_msg_v01 resp_msg;
  uint8    ipsec_vpn_pt_flag=0;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_NATALG* QcMapNatAlg = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_ipsec_vpn_pass_through_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p get ipsec vpn passthrough flag", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_ipsec_vpn_pass_through_resp_msg_v01));

  QcMapNatAlg = GET_NATALG_OBJ_FOR_QMI_CLIENT(clnt_info);

  /* Check if QcMapNatAlg is NULL and retun failure */
  CHECK_QCMAP_OBJ(QcMapNatAlg, resp_msg);

  /*---------------------------------------------------------------------

  ---------------------------------------------------------------------*/
  if (QcMapNatAlg && QcMapNatAlg->GetSetPassthroughFlag(CONFIG_IPSEC_VPN_PASSTHROUGH, GET_VALUE, &ipsec_vpn_pt_flag ,&qmi_err_num))
  {
    LOG_MSG_INFO1("Got IPSEC VPN Passthrough successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Get IPSEC VPN Passthrough failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    if(QcMapNatAlg == NULL)
      resp_msg.resp.error =  QMI_ERR_INTERNAL_V01;
    else
      resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_ipsec_vpn_pass_through_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  resp_msg.vpn_pass_through_value_valid = TRUE;
  resp_msg.vpn_pass_through_value = ipsec_vpn_pt_flag;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_ipsec_vpn_pass_through_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_ipsec_vpn_pt */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_SET_IPSEC_VPN_PT()

  DESCRIPTION
    Set IPSEC VPN Passthrough flag value on Apps.

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
static qmi_csi_cb_error qmi_qcmap_msgr_set_ipsec_vpn_pt
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_set_ipsec_vpn_pass_through_req_msg_v01 *req_ptr;
  qcmap_msgr_set_ipsec_vpn_pass_through_resp_msg_v01 resp_msg;
  uint8    ipsec_vpn_pt_flag=0;
  unsigned int index;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_NATALG* QcMapNatAlg = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_set_ipsec_vpn_pass_through_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p set ipsec vpn passthrough flag", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_ipsec_vpn_pass_through_resp_msg_v01));

  QcMapNatAlg = GET_NATALG_OBJ_FOR_QMI_CLIENT(clnt_info);

  /* Check if QcMapNatAlg is NULL and retun failure */
  CHECK_QCMAP_OBJ(QcMapNatAlg, resp_msg);

  /*---------------------------------------------------------------------

  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_ipsec_vpn_pass_through_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  ipsec_vpn_pt_flag = req_ptr->vpn_pass_through_value;

  if (QcMapNatAlg && QcMapNatAlg->GetSetPassthroughFlag(CONFIG_IPSEC_VPN_PASSTHROUGH, SET_VALUE, &ipsec_vpn_pt_flag ,&qmi_err_num))
  {
    LOG_MSG_INFO1("Set IPSEC VPN Passthrough successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Set IPSEC VPN Passthrough failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    if(QcMapNatAlg == NULL)
      resp_msg.resp.error =  QMI_ERR_INTERNAL_V01;
    else
      resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_ipsec_vpn_pass_through_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_ipsec_vpn_pass_through_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_set_ipsec_vpn_pt */


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_PPTP_VPN_PT()

  DESCRIPTION
    Get PPTP VPN Passthrough flag value on Apps.

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
static qmi_csi_cb_error qmi_qcmap_msgr_get_pptp_vpn_pt
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_get_pptp_vpn_pass_through_req_msg_v01 *req_ptr;
  qcmap_msgr_get_pptp_vpn_pass_through_resp_msg_v01 resp_msg;
  uint8    pptp_vpn_pt_flag=0;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_NATALG* QcMapNatAlg = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_pptp_vpn_pass_through_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p get pptp vpn passthrough flag", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_pptp_vpn_pass_through_resp_msg_v01));

  QcMapNatAlg = GET_NATALG_OBJ_FOR_QMI_CLIENT(clnt_info);

  /* Check if QcMapNatAlg is NULL and retun failure */
  CHECK_QCMAP_OBJ(QcMapNatAlg, resp_msg);

  /*---------------------------------------------------------------------

  ---------------------------------------------------------------------*/
  if (QcMapNatAlg && QcMapNatAlg->GetSetPassthroughFlag(CONFIG_PPTP_VPN_PASSTHROUGH, GET_VALUE, &pptp_vpn_pt_flag ,&qmi_err_num))
  {
    LOG_MSG_INFO1("Got PPTP VPN Passthrough successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Get PPTP VPN Passthrough failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    if(QcMapNatAlg == NULL)
      resp_msg.resp.error =  QMI_ERR_INTERNAL_V01;
    else
      resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_pptp_vpn_pass_through_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  resp_msg.vpn_pass_through_value_valid = TRUE;
  resp_msg.vpn_pass_through_value = pptp_vpn_pt_flag;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_pptp_vpn_pass_through_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_pptp_vpn_pt */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_SET_PPTP_VPN_PT()

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
static qmi_csi_cb_error qmi_qcmap_msgr_set_pptp_vpn_pt
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_set_pptp_vpn_pass_through_req_msg_v01 *req_ptr;
  qcmap_msgr_set_pptp_vpn_pass_through_resp_msg_v01 resp_msg;
  uint8    pptp_vpn_pt_flag=0;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  unsigned int index;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  QCMAP_NATALG* QcMapNatAlg = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_set_pptp_vpn_pass_through_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p set pptp vpn passthrough flag", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_pptp_vpn_pass_through_resp_msg_v01));

  QcMapNatAlg = GET_NATALG_OBJ_FOR_QMI_CLIENT(clnt_info);

  /* Check if QcMapNatAlg is NULL and retun failure */
  CHECK_QCMAP_OBJ(QcMapNatAlg, resp_msg);

  /*---------------------------------------------------------------------

  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_pptp_vpn_pass_through_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  pptp_vpn_pt_flag = req_ptr->vpn_pass_through_value;

  if (QcMapNatAlg && QcMapNatAlg->GetSetPassthroughFlag(CONFIG_PPTP_VPN_PASSTHROUGH, SET_VALUE, &pptp_vpn_pt_flag ,&qmi_err_num))
  {
    LOG_MSG_INFO1("Set PPTP VPN Passthrough successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Set PPTP VPN Passthrough failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    if(QcMapNatAlg == NULL)
      resp_msg.resp.error =  QMI_ERR_INTERNAL_V01;
    else
      resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_pptp_vpn_pass_through_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_pptp_vpn_pass_through_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_set_pptp_vpn_pt */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_L2TP_VPN_PT()

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
static qmi_csi_cb_error qmi_qcmap_msgr_get_l2tp_vpn_pt
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qcmap_msgr_get_l2tp_vpn_pass_through_req_msg_v01 *req_ptr;
  qcmap_msgr_get_l2tp_vpn_pass_through_resp_msg_v01 resp_msg;
  uint8    l2tp_vpn_pt_flag=0;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_NATALG* QcMapNatAlg = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_l2tp_vpn_pass_through_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p get l2tp vpn passthrough flag", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_l2tp_vpn_pass_through_resp_msg_v01));

  QcMapNatAlg = GET_NATALG_OBJ_FOR_QMI_CLIENT(clnt_info);

  /* Check if QcMapNatAlg is NULL and retun failure */
  CHECK_QCMAP_OBJ(QcMapNatAlg, resp_msg);

  /*---------------------------------------------------------------------

  ---------------------------------------------------------------------*/
  if (QcMapNatAlg && QcMapNatAlg->GetSetPassthroughFlag(CONFIG_L2TP_VPN_PASSTHROUGH, GET_VALUE, &l2tp_vpn_pt_flag ,&qmi_err_num))
  {
    LOG_MSG_INFO1("Got L2TP VPN Passthrough successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Get L2TP VPN Passthrough failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    if(QcMapNatAlg == NULL)
      resp_msg.resp.error =  QMI_ERR_INTERNAL_V01;
    else
      resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_l2tp_vpn_pass_through_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  resp_msg.vpn_pass_through_value_valid = TRUE;
  resp_msg.vpn_pass_through_value = l2tp_vpn_pt_flag;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_l2tp_vpn_pass_through_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_l2tp_vpn_pt */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_SET_L2TP_VPN_PT()

  DESCRIPTION
    Set L2TP VPN Passthrough flag value on Apps.

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
static qmi_csi_cb_error qmi_qcmap_msgr_set_l2tp_vpn_pt
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_set_l2tp_vpn_pass_through_req_msg_v01 *req_ptr;
  qcmap_msgr_set_l2tp_vpn_pass_through_resp_msg_v01 resp_msg;
  uint8    l2tp_vpn_pt_flag=0;
  unsigned int index;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qmi_error_type_v01 qmi_err_num =  QMI_ERR_NONE_V01;
  QCMAP_NATALG* QcMapNatAlg = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_set_l2tp_vpn_pass_through_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p set l2tp vpn passthrough flag", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_l2tp_vpn_pass_through_resp_msg_v01));

  QcMapNatAlg = GET_NATALG_OBJ_FOR_QMI_CLIENT(clnt_info);

  /* Check if QcMapNatAlg is NULL and retun failure */
  CHECK_QCMAP_OBJ(QcMapNatAlg, resp_msg);

  /*---------------------------------------------------------------------

  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_l2tp_vpn_pass_through_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  l2tp_vpn_pt_flag = req_ptr->vpn_pass_through_value;

  if (QcMapNatAlg && QcMapNatAlg->GetSetPassthroughFlag(CONFIG_L2TP_VPN_PASSTHROUGH, SET_VALUE, &l2tp_vpn_pt_flag ,&qmi_err_num))
  {
    LOG_MSG_INFO1("Set L2TP VPN Passthrough successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Set L2TP VPN Passthrough failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    if(QcMapNatAlg == NULL)
      resp_msg.resp.error =  QMI_ERR_INTERNAL_V01 ;
    else
      resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_l2tp_vpn_pass_through_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_l2tp_vpn_pass_through_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_set_l2tp_vpn_pt */


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_NAT_TYPE()

  DESCRIPTION
    Get NAT Type value on Apps.

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
static qmi_csi_cb_error qmi_qcmap_msgr_get_nat_type
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type *qcmap_sp;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_get_nat_type_req_msg_v01* req_ptr;
  qcmap_msgr_get_nat_type_resp_msg_v01 resp_msg;
  qcmap_msgr_nat_enum_v01 cur_nat_type = QCMAP_MSGR_NAT_ENUM_MIN_ENUM_VAL_V01;
  QCMAP_NATALG* QcMapNatAlg = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;

  printf ("Client %p get nat type", clnt_info);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_nat_type_resp_msg_v01));
  req_ptr = (qcmap_msgr_get_nat_type_req_msg_v01 *)req_c_struct;

  QcMapNatAlg = GET_NATALG_OBJ_FOR_QMI_CLIENT(clnt_info);

  /*---------------------------------------------------------------------
   Get NAT Type.
  ---------------------------------------------------------------------*/
  if (QcMapNatAlg && QcMapNatAlg->GetNatType(&cur_nat_type, &qmi_err_num))
  {
    QCMAP_CM_LOG("Got NAT type value successfully.\n");
  }
  else
  {
    QCMAP_CM_LOG("Get NAT type failed!!");
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    if(QcMapNatAlg == NULL)
      resp_msg.resp.error =  QMI_ERR_INTERNAL_V01;
    else
      resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_nat_type_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  resp_msg.nat_type_valid = TRUE;
  resp_msg.nat_type = cur_nat_type;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_nat_type_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_nat_type */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_SET_NAT_TYPE()

  DESCRIPTION
    Set NAT Type on Apps.

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
static qmi_csi_cb_error qmi_qcmap_msgr_set_nat_type
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_set_nat_type_req_msg_v01 *req_ptr;
  qcmap_msgr_set_nat_type_resp_msg_v01 resp_msg;
  unsigned int index;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  QCMAP_NATALG* QcMapNatAlg = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_set_nat_type_req_msg_v01 *)req_c_struct;

  printf ("Client %p set nat type", clnt_info);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_nat_type_resp_msg_v01));

  QcMapNatAlg = GET_NATALG_OBJ_FOR_QMI_CLIENT(clnt_info);

  /* Check if QcMapNatAlg is NULL and retun failure */
  CHECK_QCMAP_OBJ(QcMapNatAlg, resp_msg);

  /*---------------------------------------------------------------------
   Set NAT Type.
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_nat_type_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if (QcMapNatAlg && QcMapNatAlg->SetNatType(req_ptr->nat_type, &qmi_err_num))
  {
    QCMAP_CM_LOG("Set NAT Type successfully.\n");
  }
  else
  {
    QCMAP_CM_LOG("Set NAT Type failed!!");
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    if(QcMapNatAlg == NULL)
      resp_msg.resp.error =  QMI_ERR_INTERNAL_V01;
    else
      resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_nat_type_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_nat_type_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_set_nat_type */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_AUTO_CONNECT()

  DESCRIPTION
    Get Autoconnect flag value on Apps.

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
static qmi_csi_cb_error qmi_qcmap_msgr_get_auto_connect
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_get_auto_connect_req_msg_v01 *req_ptr;
  qcmap_msgr_get_auto_connect_resp_msg_v01 resp_msg;
  boolean    auto_connect_flag;
  QCMAP_Backhaul_WWAN  *QCMAP_Backhaul_WWAN_Obj = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_auto_connect_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p get auto_connect flag", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_auto_connect_resp_msg_v01));
  /*---------------------------------------------------------------------

  ---------------------------------------------------------------------*/
  QCMAP_Backhaul_WWAN_Obj = GET_BACKHAUL_WWAN_OBJ_FOR_QMI_CLIENT(clnt_info);
  CHECK_QCMAP_OBJ(QCMAP_Backhaul_WWAN_Obj, resp_msg);

  auto_connect_flag =  QCMAP_Backhaul_WWAN_Obj->GetAutoconnect();

  resp_msg.auto_conn_flag_valid = TRUE;
  resp_msg.auto_conn_flag = auto_connect_flag;

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_auto_connect_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_auto_connect */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_SET_AUTO_CONNECT()

  DESCRIPTION
    Set auto connect flag value on Apps.

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
static qmi_csi_cb_error qmi_qcmap_msgr_set_auto_connect
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_set_auto_connect_req_msg_v01 *req_ptr;
  qcmap_msgr_set_auto_connect_resp_msg_v01 resp_msg;
  unsigned int index;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_Backhaul_WWAN  *QCMAP_Backhaul_WWAN_Obj = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_set_auto_connect_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p set auto_connect flag", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_auto_connect_resp_msg_v01));

  /*---------------------------------------------------------------------

  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_auto_connect_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  QCMAP_Backhaul_WWAN_Obj = GET_BACKHAUL_WWAN_OBJ_FOR_QMI_CLIENT(clnt_info);
  CHECK_QCMAP_OBJ(QCMAP_Backhaul_WWAN_Obj, resp_msg);

  if (QCMAP_Backhaul_WWAN_Obj->SetAutoconnect(req_ptr->enable, &qmi_err_num))
  {
    LOG_MSG_INFO1("Set Auto connect flag successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Set Auto connect failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_auto_connect_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_auto_connect_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_set_auto_connect */



/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_ADD_FIREWALL_ENTRY_REQ_V01()

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
static qmi_csi_cb_error  qmi_qcmap_msgr_add_firewall_entry
(
 qmi_qcmap_msgr_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
 )
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qcmap_msgr_add_firewall_entry_req_msg_v01 *req_ptr;
  qcmap_msgr_add_firewall_entry_resp_msg_v01 resp_msg;
  qcmap_msgr_firewall_conf_t  firewall_conf;
  int              ret_val = QCMAP_CM_ERROR, next_hdr_prot;
  qmi_error_type_v01  qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_Firewall* QcMapFirewall = NULL;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_add_firewall_entry_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_add_firewall_entry_resp_msg_v01));
  memset(&firewall_conf, 0, sizeof(qcmap_msgr_firewall_conf_t));

  if (req_ptr->upnp_pinhole_valid)
  {
    firewall_conf.extd_firewall_entry.upnp_pinhole_handle = 1;
  }
  else
    firewall_conf.extd_firewall_entry.upnp_pinhole_handle = 0;

  if (req_ptr->direction_valid)
  {
    if(QcMapMgr && !IS_UL_FIREWALL_ALLOWED(QcMapMgr->target) && (req_ptr->direction == QCMAP_MSGR_UL_FIREWALL))
    {
      LOG_MSG_ERROR("Unsupported target for UL firewall", 0 , 0, 0);
      qmi_err_num = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
      goto firewall_add_error;
    }

    if (req_ptr->direction == QCMAP_MSGR_UL_FIREWALL_V01)
      firewall_conf.extd_firewall_entry.firewall_direction = QCMAP_MSGR_UL_FIREWALL;
    else
      firewall_conf.extd_firewall_entry.firewall_direction = QCMAP_MSGR_DL_FIREWALL;
  }
  else
  {
    firewall_conf.extd_firewall_entry.firewall_direction = QCMAP_MSGR_DL_FIREWALL;
  }
  LOG_MSG_INFO1("firewall_direction : %d",
                 firewall_conf.extd_firewall_entry.firewall_direction,0,0);

  if(req_ptr->ip_version == QCMAP_MSGR_IP_FAMILY_V4_V01)
  {
    firewall_conf.extd_firewall_entry.filter_spec.ip_vsn = IP_V4;
  }
  else if (req_ptr->ip_version == QCMAP_MSGR_IP_FAMILY_V6_V01)
  {
    firewall_conf.extd_firewall_entry.filter_spec.ip_vsn = IP_V6;
  }
  else
  {
    LOG_MSG_ERROR("Unsupported IP Version %d",
                   req_ptr->ip_version , 0, 0);
    qmi_err_num = QCMAP_MSGR_INVALID_PARAM;
    goto firewall_add_error;
  }

  switch (firewall_conf.extd_firewall_entry.filter_spec.ip_vsn)
  {
    case IP_V4:
      if( req_ptr->ip4_src_addr_valid )
      {
        firewall_conf.extd_firewall_entry.filter_spec.ip_hdr.v4.src.addr.ps_s_addr =
                                        (req_ptr->ip4_src_addr.addr);
        firewall_conf.extd_firewall_entry.filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr =
                                        (req_ptr->ip4_src_addr.subnet_mask);
        firewall_conf.extd_firewall_entry.filter_spec.ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_SRC_ADDR;
        LOG_MSG_INFO1("IP4 source address is:",0,0,0);
        IPV4_ADDR_MSG(firewall_conf.extd_firewall_entry.filter_spec.ip_hdr.v4.src.addr.ps_s_addr);
        LOG_MSG_INFO1("IP4 source subnet mask is:", 0, 0, 0);
        IPV4_ADDR_MSG(firewall_conf.extd_firewall_entry.filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr);
      }

      if(req_ptr->ip4_tos_valid)
      {
        firewall_conf.extd_firewall_entry.filter_spec.ip_hdr.v4.tos.val = req_ptr->ip4_tos.value;
        firewall_conf.extd_firewall_entry.filter_spec.ip_hdr.v4.tos.mask = req_ptr->ip4_tos.mask;
        firewall_conf.extd_firewall_entry.filter_spec.ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_TOS;
        LOG_MSG_INFO1("IP4  TOS value %d mask %d ",
                      firewall_conf.extd_firewall_entry.filter_spec.ip_hdr.v4.tos.val,
                      firewall_conf.extd_firewall_entry.filter_spec.ip_hdr.v4.tos.mask, 0);
      }

      if(req_ptr->next_hdr_prot_valid)
      {
       firewall_conf.extd_firewall_entry.filter_spec.ip_hdr.v4.next_hdr_prot=req_ptr->next_hdr_prot;
       firewall_conf.extd_firewall_entry.filter_spec.ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_NEXT_HDR_PROT;
      }
      break;

    case IP_V6:
      if (req_ptr->ip6_src_addr_valid)
      {
        memcpy(firewall_conf.extd_firewall_entry.filter_spec.ip_hdr.v6.src.addr.in6_u.u6_addr8,
               req_ptr->ip6_src_addr.addr,
               QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));

        firewall_conf.extd_firewall_entry.filter_spec.ip_hdr.v6.src.prefix_len =
                                 req_ptr->ip6_src_addr.prefix_len;
        firewall_conf.extd_firewall_entry.filter_spec.ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_SRC_ADDR;

        LOG_MSG_INFO1("IPV6 src_addr is:",0, 0, 0);
        IPV6_ADDR_MSG(firewall_conf.extd_firewall_entry.filter_spec.ip_hdr.v6.src.addr.in6_u.u6_addr64);
        LOG_MSG_INFO1("IPV6 prefix len %d",
                      firewall_conf.extd_firewall_entry.filter_spec.ip_hdr.v6.src.prefix_len , 0, 0);
      }

      if (req_ptr->ip6_dst_addr_valid)
      {
        memcpy(firewall_conf.extd_firewall_entry.filter_spec.ip_hdr.v6.dst.addr.in6_u.u6_addr8,
               req_ptr->ip6_dst_addr.addr,
               QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));

        firewall_conf.extd_firewall_entry.filter_spec.ip_hdr.v6.dst.prefix_len=
                                 req_ptr->ip6_dst_addr.prefix_len;
        firewall_conf.extd_firewall_entry.filter_spec.ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_DST_ADDR;

        LOG_MSG_INFO1("IPV6 dst_addr is:",0, 0, 0);
        IPV6_ADDR_MSG(firewall_conf.extd_firewall_entry.filter_spec.ip_hdr.v6.dst.addr.in6_u.u6_addr64);
        LOG_MSG_INFO1("IPV6 prefix len %d",
                      firewall_conf.extd_firewall_entry.filter_spec.ip_hdr.v6.src.prefix_len , 0, 0);
      }

      if(req_ptr->ip6_trf_cls_valid)
      {
        firewall_conf.extd_firewall_entry.filter_spec.ip_hdr.v6.trf_cls.val = req_ptr->ip6_trf_cls.value;
        firewall_conf.extd_firewall_entry.filter_spec.ip_hdr.v6.trf_cls.mask = req_ptr->ip6_trf_cls.mask;
        firewall_conf.extd_firewall_entry.filter_spec.ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_TRAFFIC_CLASS;
        LOG_MSG_INFO1("IPV6 trf class value %d mask %d",
                      firewall_conf.extd_firewall_entry.filter_spec.ip_hdr.v6.trf_cls.val ,
                      firewall_conf.extd_firewall_entry.filter_spec.ip_hdr.v6.trf_cls.mask, 0);
      }

      if(req_ptr->next_hdr_prot_valid)
      {
        firewall_conf.extd_firewall_entry.filter_spec.ip_hdr.v6.next_hdr_prot=req_ptr->next_hdr_prot;
        firewall_conf.extd_firewall_entry.filter_spec.ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_NEXT_HDR_PROT ;
      }
      break;

    default:
      LOG_MSG_ERROR("Bad IP version %d", firewall_conf.extd_firewall_entry.filter_spec.ip_vsn, 0, 0);
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
       firewall_conf.extd_firewall_entry.filter_spec.next_prot_hdr.tcp.field_mask |= IPFLTR_MASK_TCP_SRC_PORT;
       firewall_conf.extd_firewall_entry.filter_spec.next_prot_hdr.tcp.src.port =
                                         req_ptr->tcp_udp_src.port;
       firewall_conf.extd_firewall_entry.filter_spec.next_prot_hdr.tcp.src.range = req_ptr->tcp_udp_src.range;
       LOG_MSG_INFO1("TCP src port %d range %d",
                      req_ptr->tcp_udp_src.port,
                      req_ptr->tcp_udp_src.range, 0);
     }
     if(req_ptr->tcp_udp_dst_valid)
     {
       firewall_conf.extd_firewall_entry.filter_spec.next_prot_hdr.tcp.field_mask |= IPFLTR_MASK_TCP_DST_PORT;
       firewall_conf.extd_firewall_entry.filter_spec.next_prot_hdr.tcp.dst.port =
                                         req_ptr->tcp_udp_dst.port;
       firewall_conf.extd_firewall_entry.filter_spec.next_prot_hdr.tcp.dst.range = req_ptr->tcp_udp_dst.range;
       LOG_MSG_INFO1("TCP dst port %d range %d",
                        req_ptr->tcp_udp_dst.port,
                        req_ptr->tcp_udp_dst.range, 0);
     }
        break;

     case PS_IPPROTO_UDP:
      next_hdr_prot = PS_IPPROTO_UDP;

      if (req_ptr->tcp_udp_src_valid)
      {
        firewall_conf.extd_firewall_entry.filter_spec.next_prot_hdr.udp.field_mask |= IPFLTR_MASK_UDP_SRC_PORT;
        firewall_conf.extd_firewall_entry.filter_spec.next_prot_hdr.udp.src.port =
                                         req_ptr->tcp_udp_src.port;
        firewall_conf.extd_firewall_entry.filter_spec.next_prot_hdr.udp.src.range = req_ptr->tcp_udp_src.range;
        LOG_MSG_INFO1("UDP src port %d range %d",
                       req_ptr->tcp_udp_src.port,
                       req_ptr->tcp_udp_src.range, 0);
      }

      if(req_ptr->tcp_udp_dst_valid)
      {
        firewall_conf.extd_firewall_entry.filter_spec.next_prot_hdr.udp.field_mask |= IPFLTR_MASK_UDP_DST_PORT;
        firewall_conf.extd_firewall_entry.filter_spec.next_prot_hdr.udp.dst.port =
                                         req_ptr->tcp_udp_dst.port;
        firewall_conf.extd_firewall_entry.filter_spec.next_prot_hdr.udp.dst.range = req_ptr->tcp_udp_dst.range;
        LOG_MSG_INFO1("UDP dst port %d range %d",
                       req_ptr->tcp_udp_dst.port,
                       req_ptr->tcp_udp_dst.range, 0);
        }

       break;

      case PS_IPPROTO_ICMP:
      case PS_IPPROTO_ICMP6:
      if(firewall_conf.extd_firewall_entry.filter_spec.ip_vsn == IP_V4 &&\
         req_ptr->next_hdr_prot == PS_IPPROTO_ICMP)
      {
        next_hdr_prot = PS_IPPROTO_ICMP;
      }
      else if(firewall_conf.extd_firewall_entry.filter_spec.ip_vsn == IP_V6 &&\
         req_ptr->next_hdr_prot == PS_IPPROTO_ICMP6)
      {
        next_hdr_prot = PS_IPPROTO_ICMP6;
      }
      else
      {
        LOG_MSG_ERROR("Invalid ICMP type in next header protocol %d",
                       req_ptr->next_hdr_prot , 0, 0);
        qmi_err_num = QCMAP_MSGR_INVALID_PARAM;
        goto firewall_add_error;
      }
      if (req_ptr->icmp_type_valid)
      {
        firewall_conf.extd_firewall_entry.filter_spec.next_prot_hdr.icmp.field_mask |=
                                                IPFLTR_MASK_ICMP_MSG_TYPE;
        firewall_conf.extd_firewall_entry.filter_spec.next_prot_hdr.icmp.type = req_ptr->icmp_type;
        LOG_MSG_INFO1("ICMP type %d ",
                       firewall_conf.extd_firewall_entry.filter_spec.next_prot_hdr.icmp.type , 0, 0);
      }
      if (req_ptr->icmp_code_valid)
      {
         firewall_conf.extd_firewall_entry.filter_spec.next_prot_hdr.icmp.field_mask |=
                                                 IPFLTR_MASK_ICMP_MSG_CODE;
         firewall_conf.extd_firewall_entry.filter_spec.next_prot_hdr.icmp.code = req_ptr->icmp_code;
         LOG_MSG_INFO1("ICMP code %d ",
                        firewall_conf.extd_firewall_entry.filter_spec.next_prot_hdr.icmp.code, 0, 0);
       }
        break;
      case PS_IPPROTO_ESP:
        next_hdr_prot = PS_IPPROTO_ESP;
        if (req_ptr->esp_spi_valid)
        {
          firewall_conf.extd_firewall_entry.filter_spec.next_prot_hdr.esp.field_mask |=
          IPFLTR_MASK_ESP_SPI;
          firewall_conf.extd_firewall_entry.filter_spec.next_prot_hdr.esp.spi = req_ptr->esp_spi;
          LOG_MSG_INFO1("ESP SPI %d  ",req_ptr->esp_spi, 0, 0);
        }
         break;
      case PS_IPPROTO_TCP_UDP:
        next_hdr_prot = PS_IPPROTO_TCP_UDP;

        if (req_ptr->tcp_udp_src_valid)
        {
          firewall_conf.extd_firewall_entry.filter_spec.next_prot_hdr.tcp_udp_port_range.field_mask |=
                                               IPFLTR_MASK_TCP_UDP_SRC_PORT;
          firewall_conf.extd_firewall_entry.filter_spec.next_prot_hdr.tcp_udp_port_range.src.port =
                                     req_ptr->tcp_udp_src.port;
          firewall_conf.extd_firewall_entry.filter_spec.next_prot_hdr.tcp_udp_port_range.src.range =
                                                req_ptr->tcp_udp_src.range;
          LOG_MSG_INFO1("TCP_UDP src port %d range %d",
                         req_ptr->tcp_udp_src.port,
                         req_ptr->tcp_udp_src.range, 0);
        }

        if(req_ptr->tcp_udp_dst_valid)
        {
          firewall_conf.extd_firewall_entry.filter_spec.next_prot_hdr.tcp_udp_port_range.field_mask |=
                                                  IPFLTR_MASK_TCP_UDP_DST_PORT;
          firewall_conf.extd_firewall_entry.filter_spec.next_prot_hdr.tcp_udp_port_range.dst.port =
                                       req_ptr->tcp_udp_dst.port;
          firewall_conf.extd_firewall_entry.filter_spec.next_prot_hdr.tcp_udp_port_range.dst.range =
                                                req_ptr->tcp_udp_dst.range;
          LOG_MSG_INFO1("TCP_UDP dst port %d range %d",
                        req_ptr->tcp_udp_dst.port,
                        req_ptr->tcp_udp_dst.range, 0);
        }
          break;
      default:
        LOG_MSG_ERROR("Unsupported next header protocol %d",
                       req_ptr->next_hdr_prot , 0, 0);
        qmi_err_num = QCMAP_MSGR_INVALID_PARAM;
        goto firewall_add_error;
      }
   }

  QcMapFirewall = GET_FIREWALL_OBJ_FOR_QMI_CLIENT(clnt_info);

  /* Check if QcMapFirewall is NULL and retun failure */
  CHECK_QCMAP_OBJ(QcMapFirewall, resp_msg);

  if(QcMapFirewall)
    ret_val = QcMapFirewall->AddFireWallEntry(&firewall_conf, &qmi_err_num);
  if(ret_val == QCMAP_CM_SUCCESS)
  {
    LOG_MSG_INFO1("Added Firewall entry successfully.\n",0,0,0);
    memset(&resp_msg, 0, sizeof(qcmap_msgr_add_firewall_entry_resp_msg_v01));
    resp_msg.firewall_handle_valid=true;
    resp_msg.firewall_handle=firewall_conf.extd_firewall_entry.firewall_handle;
  }
  else
  {
    firewall_add_error:
    LOG_MSG_ERROR("Add Firewall entry failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE;
    if(ret_val == QCMAP_MSGR_ENTRY_PRESENT)
      resp_msg.resp.error = qmi_err_num;
    else if(ret_val == QCMAP_MSGR_ENTRY_FULL)
      resp_msg.resp.error = qmi_err_num ;
    else
      resp_msg.resp.error =  qmi_err_num;

    if(QcMapFirewall == NULL)
      resp_msg.resp.error = QMI_ERR_INTERNAL_V01;

    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
        sizeof(qcmap_msgr_add_firewall_entry_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
      sizeof(qcmap_msgr_add_firewall_entry_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}

/*==========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_FIREWALL_ENTRIES_HANDLE_LIST_REQ_V01

  DESCRIPTION
    Get Firewall handle list .

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
static qmi_csi_cb_error qmi_qcmap_msgr_get_firewall_handles_list
(
 qmi_qcmap_msgr_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
 )
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qcmap_msgr_get_firewall_entries_handle_list_req_msg_v01 *req_ptr;
  qcmap_msgr_get_firewall_entries_handle_list_resp_msg_v01  resp_msg;
  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_firewall_entries_handle_list_req_msg_v01 *)req_c_struct;
  qcmap_msgr_firewall_conf_t firewall_entry;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  int i=0;
  QCMAP_Firewall* QcMapFirewall = NULL;

  if ( req_ptr->ip_version == QCMAP_MSGR_IP_FAMILY_V4_V01 )
  {
    firewall_entry.ip_family=IP_V4;
  }
  else if ( req_ptr->ip_version == QCMAP_MSGR_IP_FAMILY_V6_V01 )
  {
    firewall_entry.ip_family=IP_V6;
  }
  else
  {
    LOG_MSG_ERROR("Ip version %d not supported",req_ptr->ip_version,0,0);
    qmi_err_num = QCMAP_MSGR_INVALID_PARAM;
    goto get_handlist_error;
  }

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_firewall_entries_handle_list_resp_msg_v01));

  QcMapFirewall = GET_FIREWALL_OBJ_FOR_QMI_CLIENT(clnt_info);

  /* Check if QcMapFirewall is NULL and retun failure */
  CHECK_QCMAP_OBJ(QcMapFirewall, resp_msg);

  if (QcMapFirewall && QcMapFirewall->GetFireWallHandleList(&firewall_entry, &qmi_err_num))
  {
    LOG_MSG_INFO1("Get Firewall Handle list successfully.",0,0,0);
    resp_msg.firewall_handle_list_valid= TRUE;
    resp_msg.firewall_handle_list_len=firewall_entry.extd_firewall_handle_list.num_of_entries;
    for( i=0; i<resp_msg.firewall_handle_list_len ; i++)
    {
      resp_msg.firewall_handle_list[i]=firewall_entry.extd_firewall_handle_list.handle_list[i];
    }
  }
  else
  {
    get_handlist_error:
    LOG_MSG_ERROR("Get Firewall Handle list Failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    if(QcMapFirewall == NULL)
      resp_msg.resp.error =  QMI_ERR_INTERNAL_V01;
    else
      resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
        sizeof(qcmap_msgr_get_firewall_entries_handle_list_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
      sizeof(qcmap_msgr_get_firewall_entries_handle_list_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

}


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_DELETE_FIREWALL_ENTRY_REQ_V01()

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
static qmi_csi_cb_error  qmi_qcmap_msgr_del_firewall_entry
(
 qmi_qcmap_msgr_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
 )
{
  qmi_qcmap_msgr_state_info_type    *qcmap_s;
  qcmap_msgr_delete_firewall_entry_req_msg_v01 *req_ptr;
  qcmap_msgr_delete_firewall_entry_resp_msg_v01 resp_msg;
  qcmap_s = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_delete_firewall_entry_req_msg_v01 *)req_c_struct;
  qcmap_msgr_firewall_conf_t firewall_entry;
  qmi_error_type_v01  qmi_err_num = QMI_ERR_NONE_V01;
  firewall_entry.extd_firewall_entry.firewall_handle=req_ptr->firewall_handle;
  QCMAP_Firewall* QcMapFirewall = NULL;

  memset(&resp_msg, 0, sizeof(qcmap_msgr_delete_firewall_entry_resp_msg_v01));

  QcMapFirewall = GET_FIREWALL_OBJ_FOR_QMI_CLIENT(clnt_info);

  /* Check if QcMapFirewall is NULL and retun failure */
  CHECK_QCMAP_OBJ(QcMapFirewall, resp_msg);

  if (QcMapFirewall && QcMapFirewall->DeleteFireWallEntry(&firewall_entry.extd_firewall_entry, req_ptr->firewall_handle, &qmi_err_num))
  {
    LOG_MSG_INFO1("Delete entry successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Delete entry failed ",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    if(QcMapFirewall == NULL)
      resp_msg.resp.error =  QMI_ERR_INTERNAL_V01;
    else
      resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
        sizeof(qcmap_msgr_delete_firewall_entry_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
      sizeof(qcmap_msgr_delete_firewall_entry_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

}


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_FIREWALL_ENTRY_REQ_V01()

  DESCRIPTION
    Get Firewall entry on Apps.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_get_firewall_entry
(
 qmi_qcmap_msgr_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
 )
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_get_firewall_entry_req_msg_v01 *req_ptr;
  qcmap_msgr_get_firewall_entry_resp_msg_v01 resp_msg;
  QCMAP_Firewall* QcMapFirewall = NULL;

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_firewall_entry_req_msg_v01 *)req_c_struct;

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_firewall_entry_resp_msg_v01));

  QcMapFirewall = GET_FIREWALL_OBJ_FOR_QMI_CLIENT(clnt_info);

  /* Check if QcMapFirewall is NULL and retun failure */
  CHECK_QCMAP_OBJ(QcMapFirewall, resp_msg);

  if (QcMapFirewall && QcMapFirewall->GetFireWallEntry(&resp_msg,req_ptr->firewall_handle, &qmi_err_num))
  {
    LOG_MSG_INFO1("\n Entries filled successfully for display NUM entry ",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("No Entries found !!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    if(QcMapFirewall == NULL)
      resp_msg.resp.error =  QMI_ERR_INTERNAL_V01;
    else
      resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
        sizeof(qcmap_msgr_get_firewall_entry_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
      sizeof(qcmap_msgr_get_firewall_entry_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_MOBILEAP_STATUS_REQ_V01()

  DESCRIPTION
    Gets MobileAP Status.

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
static qmi_csi_cb_error qmi_qcmap_msgr_get_mobileap_status
(
 qmi_qcmap_msgr_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
 )
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qcmap_msgr_mobile_ap_status_resp_v01 resp_msg;
  qmi_error_type_v01                   qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;

  memset(&resp_msg, 0, sizeof(qcmap_msgr_mobile_ap_status_resp_v01));

  resp_msg.mobile_ap_status_valid = TRUE;
  if( QCMAP_ConnectionManager::GetMobileAPhandle(&qmi_err_num) != 0 )
    resp_msg.mobile_ap_status = QCMAP_MSGR_MOBILE_AP_STATUS_CONNECTED_V01;
  else
    resp_msg.mobile_ap_status = QCMAP_MSGR_MOBILE_AP_STATUS_DISCONNECTED_V01;

  LOG_MSG_INFO1("Sending Mobile ap status ",0,0,0);
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
      sizeof(qcmap_msgr_mobile_ap_status_resp_v01));

  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_WWAN_STATUS_REQ_V01()

  DESCRIPTION
    Get WWAN status

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
static qmi_csi_cb_error qmi_qcmap_msgr_get_wwan_status
(
 qmi_qcmap_msgr_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
 )
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qcmap_msgr_wwan_status_req_msg_v01 *req_ptr;
  qcmap_msgr_wwan_status_resp_msg_v01 resp_msg;
  uint8_t status = 0;
  unsigned int index;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qcmap_msgr_wwan_call_type_v01 call_type = QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01;
  qmi_error_type_v01                   qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_Backhaul_WWAN  *QCMAP_Backhaul_WWAN_Obj = NULL;
/*-----------------------------------------------------------------------------*/
  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_wwan_status_req_msg_v01 *)req_c_struct;

  memset(&resp_msg, 0, sizeof(qcmap_msgr_wwan_status_resp_msg_v01));

  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_wwan_status_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if ( req_ptr->call_type_valid )
  {
    call_type = req_ptr->call_type;
  }

  QCMAP_Backhaul_WWAN_Obj = GET_BACKHAUL_WWAN_OBJ_FOR_QMI_CLIENT(clnt_info);
  CHECK_QCMAP_OBJ(QCMAP_Backhaul_WWAN_Obj, resp_msg);

  if (QCMAP_Backhaul_WWAN_Obj->GetWWANStatus(call_type, &status, &qmi_err_num))
  {
    resp_msg.conn_status_valid = TRUE;
    if ( call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01 )
    {
      if(status == QCMAP_CM_WAN_CONNECTING)
        resp_msg.conn_status = QCMAP_MSGR_WWAN_STATUS_CONNECTING_V01;
      else if (status == QCMAP_CM_WAN_DISCONNECTING )
        resp_msg.conn_status = QCMAP_MSGR_WWAN_STATUS_DISCONNECTING_V01;
      else if (status == QCMAP_CM_WAN_CONNECTED)
        resp_msg.conn_status = QCMAP_MSGR_WWAN_STATUS_CONNECTED_V01;
      else
        resp_msg.conn_status = QCMAP_MSGR_WWAN_STATUS_DISCONNECTED_V01;
    }
    else
    {
      if ( status == QCMAP_CM_V6_WAN_CONNECTING )
        resp_msg.conn_status = QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTING_V01;
      else if ( status == QCMAP_CM_V6_WAN_DISCONNECTING )
        resp_msg.conn_status = QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTING_V01;
      else if ( status == QCMAP_CM_V6_WAN_CONNECTED )
        resp_msg.conn_status = QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTED_V01;
      else
        resp_msg.conn_status = QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTED_V01;
    }
    LOG_MSG_INFO1("\n Entries filled successfully for display NUM entry ",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("No Entries found !!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
        sizeof(qcmap_msgr_wwan_status_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
      sizeof(qcmap_msgr_wwan_status_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_STATION_MODE_STATUS_REQ_V01()

  DESCRIPTION
    Gets Station Mode Status.

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
static qmi_csi_cb_error qmi_qcmap_msgr_get_station_mode_status
(
 qmi_qcmap_msgr_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
 )
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qcmap_msgr_get_station_mode_status_resp_msg_v01 resp_msg;
  boolean ret;
  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_station_mode_status_resp_msg_v01));
  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
  resp_msg.conn_status_valid = TRUE;
  ret = (QCMAP_Backhaul_WLAN::IsSTAAvailableV4() ||
         QCMAP_Backhaul_WLAN::IsSTAAvailableV6());

  if ( ret )
    resp_msg.conn_status = QCMAP_MSGR_STATION_MODE_CONNECTED_V01;
  else
    resp_msg.conn_status = QCMAP_MSGR_STATION_MODE_DISCONNECTED_V01;

  LOG_MSG_INFO1("Sending Station Mode status ",0,0,0);
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
      sizeof(qcmap_msgr_get_station_mode_status_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_WWAN_CONFIG()

  DESCRIPTION
    Get wwan config on Apps.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_get_wwan_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_get_wwan_config_req_msg_v01 *req_ptr;
  qcmap_msgr_get_wwan_config_resp_msg_v01 resp_msg;
  unsigned int     index;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  in_addr public_ip;
  in_addr primary_dns;
  in_addr secondary_dns;
  uint8_t public_ip_v6[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  uint8_t primary_dns_v6[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  uint8_t secondary_dns_v6[QCMAP_MSGR_IPV6_ADDR_LEN_V01];

  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_Backhaul  *QCMAP_Backhaul_Obj = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  memset(&public_ip,0,sizeof(in_addr));
  memset(&primary_dns,0,sizeof(in_addr));
  memset(&secondary_dns,0,sizeof(in_addr));

  memset(public_ip_v6,0,QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
  memset(primary_dns_v6,0,QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
  memset(secondary_dns_v6,0,QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_wwan_config_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p get wwan stats", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_wwan_config_resp_msg_v01));

  /*---------------------------------------------------------------------

  ---------------------------------------------------------------------*/

  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_get_wwan_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  QCMAP_Backhaul_Obj = GET_BACKHAUL_OBJ_FOR_QMI_CLIENT(clnt_info);
  CHECK_QCMAP_OBJ(QCMAP_Backhaul_Obj, resp_msg);

  if(req_ptr->addr_type_op == (QCMAP_MSGR_MASK_V4_ADDR_V01 |
                              QCMAP_MSGR_MASK_V4_DNS_ADDR_V01))
  {
    if (QCMAP_Backhaul_Obj->GetNetworkConfig((in_addr_t *)&public_ip.s_addr,
                                       (uint32 *)&primary_dns.s_addr,
                                       (in_addr_t *)&secondary_dns.s_addr,
                                       &qmi_err_num))
    {
      LOG_MSG_INFO1("Got WWAN Config successfully.\n",0,0,0);
    }
    else
    {
      LOG_MSG_ERROR("Get WWAN Config get failed!!",0,0,0);
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      resp_msg.resp.error = qmi_err_num;
      qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_get_wwan_config_resp_msg_v01));
      return QMI_CSI_CB_NO_ERR;
    }

    if ( public_ip.s_addr != 0 )
    {
      resp_msg.v4_addr_valid = TRUE;
      resp_msg.v4_addr = htonl(public_ip.s_addr);
    }
    if ( primary_dns.s_addr != 0 )
    {
      resp_msg.v4_prim_dns_addr_valid = TRUE;
      resp_msg.v4_prim_dns_addr = htonl(primary_dns.s_addr);
    }
    if ( secondary_dns.s_addr != 0 )
    {
      resp_msg.v4_sec_dns_addr_valid = TRUE;
      resp_msg.v4_sec_dns_addr = htonl(secondary_dns.s_addr);
    }
  }

  if(req_ptr->addr_type_op == (QCMAP_MSGR_MASK_V6_ADDR_V01 |
                              QCMAP_MSGR_MASK_V6_DNS_ADDR_V01))
  {
    if (QCMAP_Backhaul_Obj->GetIPv6NetworkConfig(public_ip_v6,
                                       primary_dns_v6,
                                       secondary_dns_v6,
                                       &qmi_err_num))
    {
      LOG_MSG_INFO1("Got IPv6 WWAN Config successfully.\n",0,0,0);
    }
    else
    {
      LOG_MSG_ERROR("Get WWAN Config get failed!!",0,0,0);
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      resp_msg.resp.error = qmi_err_num;
      qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_get_wwan_config_resp_msg_v01));
      return QMI_CSI_CB_NO_ERR;
    }

    if (!PS_IN6_IS_ADDR_UNSPECIFIED(public_ip_v6))
    {
      resp_msg.v6_addr_valid = TRUE;
      memcpy(resp_msg.v6_addr, public_ip_v6,
             QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
    }
    if (!PS_IN6_IS_ADDR_UNSPECIFIED(primary_dns_v6))
    {
      resp_msg.v6_prim_dns_addr_valid = TRUE;
      memcpy(resp_msg.v6_prim_dns_addr, primary_dns_v6,
             QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
    }
    if (!PS_IN6_IS_ADDR_UNSPECIFIED(secondary_dns_v6))
    {
      resp_msg.v6_sec_dns_addr_valid = TRUE;
      memcpy(resp_msg.v6_sec_dns_addr, secondary_dns_v6,
             QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
    }
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_wwan_config_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_wwan_config() */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_NAT_TIMEOUT()

  DESCRIPTION
    Get NAT Timeout value of a particular type on Apps.

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
static qmi_csi_cb_error qmi_qcmap_msgr_get_nat_timeout
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qmi_error_type_v01  qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_get_nat_timeout_req_msg_v01 *req_ptr;
  qcmap_msgr_get_nat_timeout_resp_msg_v01 resp_msg;
  uint32 timeout_value = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_nat_timeout_req_msg_v01 *)req_c_struct;

  printf ("Client %p Get Nat Timeout\n", clnt_info);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_nat_type_resp_msg_v01));

  /*---------------------------------------------------------------------
   Get NAT Timeout.
  ---------------------------------------------------------------------*/
  if (QCMAP_NATALG::GetNatTimeout(req_ptr->nat_timeout_type, &timeout_value, &qmi_err_num))
  {
    QCMAP_CM_LOG("Got NAT timeout value successfully.\n");
  }
  else
  {
    QCMAP_CM_LOG("Get NAT timeout failed!!");
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_nat_timeout_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  resp_msg.timeout_value_valid = TRUE;
  resp_msg.timeout_value = timeout_value;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_nat_timeout_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_nat_timeout */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_SET_NAT_TIMEOUT()

  DESCRIPTION
    Set NAT Timeout value on Apps.

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
static qmi_csi_cb_error qmi_qcmap_msgr_set_nat_timeout
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  unsigned int     index;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qmi_error_type_v01  qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_set_nat_timeout_req_msg_v01 *req_ptr;
  qcmap_msgr_set_nat_timeout_resp_msg_v01 resp_msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_set_nat_timeout_req_msg_v01 *)req_c_struct;

  printf ("Client %p set nat timeout", clnt_info);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_nat_timeout_resp_msg_v01));

  /*---------------------------------------------------------------------
   Set NAT Timeout.
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_nat_timeout_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  if (QCMAP_NATALG::SetNatTimeout(req_ptr->nat_timeout_type, req_ptr->timeout_value, &qmi_err_num))
  {
    QCMAP_CM_LOG("Set NAT Timeout successfully.\n");
  }
  else
  {
    QCMAP_CM_LOG("Set NAT Timeout failed!!");
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_nat_timeout_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_nat_timeout_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_set_nat_timeout */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_WLAN_STATUS()

  DESCRIPTION
    Get the current WLAN status.

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
static qmi_csi_cb_error qmi_qcmap_msgr_get_wlan_status
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_get_wlan_status_resp_msg_v01 resp_msg;
  unsigned int     index;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qcmap_msgr_get_wlan_status_req_msg_v01  *req_ptr = NULL;
  qcmap_msgr_wlan_mode_enum_v01 wlan_mode =
                                    QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);
  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_wlan_status_req_msg_v01 *)req_c_struct;

  printf ("Client %p Get WLAN Status\n", clnt_info);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_wlan_status_resp_msg_v01));

  /*---------------------------------------------------------------------
   Get WLAN Status.
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     ( map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_get_wlan_status_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  if (QCMAP_WLAN::GetWLANStatus(&wlan_mode, &qmi_err_num))
  {
    QCMAP_CM_LOG("Got WLAN Status successfully.\n");
  }
  else
  {
    QCMAP_CM_LOG("Get WLAN Status failed!!");
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_wlan_status_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  resp_msg.wlan_mode_valid = TRUE;
  resp_msg.wlan_mode = wlan_mode;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_wlan_status_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_wlan_status */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_LAN_CONFIG()

  DESCRIPTION
    Get the configured LAN Config including USB TE config

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
static qmi_csi_cb_error qmi_qcmap_msgr_get_lan_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_get_lan_config_resp_msg_v01 resp_msg;
  qcmap_msgr_lan_config_v01 lan_cfg;
  QCMAP_LAN* lan_obj = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;

  printf ("Client %p Get LAN Config\n", clnt_info);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_lan_config_resp_msg_v01));
  memset(&lan_cfg,0,sizeof(qcmap_msgr_lan_config_v01));

  /*---------------------------------------------------------------------
   Get LAN Config.
  ---------------------------------------------------------------------*/
  if(!clnt_info)
  {
    LOG_MSG_INFO1("CLNT INFO is null", 0, 0, 0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_lan_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if((lan_obj = GET_LAN_OBJECT(clnt_info->bridge_vlan_id)) == NULL)
  {
    LOG_MSG_INFO1("LAN OBJ is null", 0, 0, 0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_lan_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1("Getting LAN Cfg for Bridge/VLAN: %d", clnt_info->bridge_vlan_id, 0, 0);

  if(lan_obj->GetLANConfig(&lan_cfg, &qmi_err_num))
  {
    QCMAP_CM_LOG("Get LAN Config successfully.\n");
  }
  else
  {
    QCMAP_CM_LOG("Get LAN Config failed!!");
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_lan_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  resp_msg.lan_config_valid = TRUE;
  resp_msg.lan_config = lan_cfg;

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_lan_config_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_lan_config */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_WLAN_CONFIG()

  DESCRIPTION
    Get the configured WLAN Mode, Guest AP Access profile and Station mode config

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
static qmi_csi_cb_error qmi_qcmap_msgr_get_wlan_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_get_wlan_config_resp_msg_v01 resp_msg;
  qcmap_msgr_wlan_mode_enum_v01 wlan_mode =
                                    QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01;
  qcmap_msgr_guest_profile_config_v01 guest_access_profile;
  qcmap_msgr_station_mode_config_v01 station_config;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;

  printf ("Client %p Get WLAN Config\n", clnt_info);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_wlan_config_resp_msg_v01));
  memset(&station_config,0,sizeof(qcmap_msgr_station_mode_config_v01));

  /*---------------------------------------------------------------------
   Get WLAN Config.
  ---------------------------------------------------------------------*/
  if (QCMAP_WLAN::GetWLANConfig(&wlan_mode, &guest_access_profile, &station_config, &qmi_err_num))
  {
    QCMAP_CM_LOG("Got WLAN Config successfully.\n");
  }
  else
  {
    QCMAP_CM_LOG("Get WLAN Config failed!!");
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_wlan_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  resp_msg.wlan_mode_valid = TRUE;
  resp_msg.wlan_mode = wlan_mode;

  resp_msg.guest_ap_access_profile_valid = TRUE;
  resp_msg.guest_ap_access_profile =
  guest_access_profile.guest_ap_profile[QCMAP_MSGR_GUEST_AP_ONE_V01];
  resp_msg.guest_ap_2_access_profile_valid = TRUE;
  resp_msg.guest_ap_2_access_profile =
  guest_access_profile.guest_ap_profile[QCMAP_MSGR_GUEST_AP_TWO_V01];
  resp_msg.guest_ap_3_access_profile_valid = TRUE;
  resp_msg.guest_ap_3_access_profile =
  guest_access_profile.guest_ap_profile[QCMAP_MSGR_GUEST_AP_THREE_V01];

  resp_msg.station_config_valid = TRUE;
  resp_msg.station_config = station_config;

  LOG_MSG_INFO1(" qmi_qcmap_msgr_get_wlan_config ap_sta_bridge_mode=%d ",
                resp_msg.station_config.ap_sta_bridge_mode,0,0);

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_wlan_config_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_wlan_config */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_SET_LAN_CONFIG()

  DESCRIPTION
    Set LAN Config on Apps.

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
static qmi_csi_cb_error qmi_qcmap_msgr_set_lan_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_set_lan_config_req_msg_v01 *req_ptr;
  qcmap_msgr_set_lan_config_resp_msg_v01 resp_msg;
  qcmap_msgr_lan_config_v01 *lan_cfg = NULL;
  QCMAP_LAN* lan_obj = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_set_lan_config_req_msg_v01 *)req_c_struct;

  printf ("Client %p set  LAN config", clnt_info);
  if ( req_ptr->lan_config_valid )
  {
    lan_cfg = &req_ptr->lan_config;
  }

  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_lan_config_resp_msg_v01));

  /*---------------------------------------------------------------------
   Set  LAN Config.
  ---------------------------------------------------------------------*/
  if(!clnt_info)
  {
    LOG_MSG_INFO1("CLNT INFO is null", 0, 0, 0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_lan_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if((lan_obj = GET_LAN_OBJECT(clnt_info->bridge_vlan_id)) == NULL)
  {
    LOG_MSG_INFO1("LAN OBJ is null", 0, 0, 0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_lan_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1("Setting LAN Cfg for Bridge/VLAN: %d", clnt_info->bridge_vlan_id, 0, 0);

  if(lan_cfg && lan_obj->SetLANConfig(lan_cfg, &qmi_err_num))
  {
    QCMAP_CM_LOG("Set LAN Config successfully.\n");
  }
  else
  {
    QCMAP_CM_LOG("Set  LAN Config failed!! with error = %d",qmi_err_num);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_lan_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_lan_config_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_set_lan_config */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_SET_WLAN_CONFIG()

  DESCRIPTION
    Set WLAN Config on Apps.

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
static qmi_csi_cb_error qmi_qcmap_msgr_set_wlan_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
  {

  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_set_wlan_config_req_msg_v01 *req_ptr;
  qcmap_msgr_set_wlan_config_resp_msg_v01 resp_msg;
  unsigned int     index;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qcmap_msgr_guest_profile_config_v01 guest_profile;
  qcmap_msgr_station_mode_config_v01 *station_cfg = NULL;
  qcmap_msgr_station_mode_config_v01 station_cfg_xml;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_set_wlan_config_req_msg_v01 *)req_c_struct;

  printf ("Client %p set WLAN config", clnt_info);

  if (req_ptr->guest_ap_access_profile_valid) {
    guest_profile.guest_ap_profile[QCMAP_MSGR_GUEST_AP_ONE_V01] =
       req_ptr->guest_ap_access_profile;
  }
  else
  {
    guest_profile.guest_ap_profile[QCMAP_MSGR_GUEST_AP_ONE_V01] =
       QCMAP_MSGR_ACCESS_PROFILE_MIN_ENUM_VAL_V01;
  }

  if (req_ptr->guest_ap_2_access_profile_valid)
  {
    guest_profile.guest_ap_profile[QCMAP_MSGR_GUEST_AP_TWO_V01] =
    req_ptr->guest_ap_2_access_profile;
  }
  else
  {
    guest_profile.guest_ap_profile[QCMAP_MSGR_GUEST_AP_TWO_V01] =
    QCMAP_MSGR_ACCESS_PROFILE_MIN_ENUM_VAL_V01;
  }


  if (req_ptr->guest_ap_3_access_profile_valid)
  {
    guest_profile.guest_ap_profile[QCMAP_MSGR_GUEST_AP_THREE_V01] =
    req_ptr->guest_ap_3_access_profile;
  }
  else
  {
    guest_profile.guest_ap_profile[QCMAP_MSGR_GUEST_AP_THREE_V01] =
    QCMAP_MSGR_ACCESS_PROFILE_MIN_ENUM_VAL_V01;
  }

  if (req_ptr->guest_ap_2_access_profile_valid) {
    guest_profile.guest_ap_profile[QCMAP_MSGR_GUEST_AP_TWO_V01] =
       req_ptr->guest_ap_2_access_profile;
  }
  else
  {
    guest_profile.guest_ap_profile[QCMAP_MSGR_GUEST_AP_TWO_V01] =
       QCMAP_MSGR_ACCESS_PROFILE_MIN_ENUM_VAL_V01;
  }

  if ( req_ptr->station_config_valid )
  {
    station_cfg = &req_ptr->station_config;
  }

  if  (req_ptr->wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_P2P_V01 ||
         req_ptr->wlan_mode == QCMAP_MSGR_WLAN_MODE_STA_P2P_V01)
  {
    QCMAP_Backhaul_WLAN::GetSetAPSTAConfigFromXML(GET_VALUE, &station_cfg_xml);
    if( (req_ptr->station_config_valid && station_cfg->ap_sta_bridge_mode) ||
        (!req_ptr->station_config_valid && station_cfg_xml.ap_sta_bridge_mode) )
    {
      LOG_MSG_ERROR("AP-P2P and STA-P2P will only work in Router mode", 0, 0, 0);
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      resp_msg.resp.error = QMI_ERR_NOT_SUPPORTED_V01;
      qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                         sizeof(qcmap_msgr_set_wlan_config_resp_msg_v01));
      return QMI_CSI_CB_NO_ERR;
     }
  }

  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_wlan_config_resp_msg_v01));

  /*---------------------------------------------------------------------
   Set WLAN Config.
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     ( map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_wlan_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  if (QCMAP_WLAN::SetWLANConfig(req_ptr->wlan_mode, &guest_profile, station_cfg, &qmi_err_num))
  {
    QCMAP_CM_LOG("Set WLAN Config successfully.\n");
  }
  else
  {
    QCMAP_CM_LOG("Set WLAN Config failed!!");
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_wlan_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_wlan_config_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_set_wlan_config */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_ACTIVATE_LAN()

  DESCRIPTION
    Activates LAN with available config.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_activate_lan
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_activate_lan_resp_msg_v01   resp_msg;
  qcmap_msgr_activate_lan_req_msg_v01    *req_ptr = NULL;
  QCMAP_LAN* lan_obj = NULL;
  unsigned int     index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;

  LOG_MSG_INFO1 ("Client %p activating LAN", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_activate_lan_resp_msg_v01));
  req_ptr = (qcmap_msgr_activate_lan_req_msg_v01 *)req_c_struct;

  /*---------------------------------------------------------------------
   Activate LAN with the current configuration.
  ---------------------------------------------------------------------*/
  if(!clnt_info)
  {
    LOG_MSG_INFO1("CLNT INFO is null", 0, 0, 0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_activate_lan_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if((lan_obj = GET_LAN_OBJECT(clnt_info->bridge_vlan_id)) == NULL)
  {
    LOG_MSG_INFO1("LAN OBJ is null", 0, 0, 0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_activate_lan_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1("Activate LAN Cfg for Bridge/VLAN: %d", clnt_info->bridge_vlan_id, 0, 0);

  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_activate_lan_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == true ||
      QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated()==true)
  {
    LOG_MSG_ERROR("Can't Activate LAN in Ap-Sta/Cradle wan brideg mode.\n",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_activate_lan_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  else if (lan_obj->ActivateLAN(&qmi_err_num))
  {
    LOG_MSG_INFO1("Activate LAN succeeds.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Activate LAN fails.\n",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_activate_lan_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1("\nActivated LAN",0,0,0);
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_activate_lan_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_qcmap_msgr_activate_lan() */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_ACTIVATE_WLAN()

  DESCRIPTION
    Activates WLAN with available config.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_activate_wlan
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_activate_wlan_resp_msg_v01   resp_msg;
  qcmap_msgr_activate_wlan_req_msg_v01     *req_ptr;

  unsigned int     index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_activate_wlan_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p activating WLAN", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_activate_wlan_resp_msg_v01));

  /*---------------------------------------------------------------------
   Activate WLAN with the current configuration.
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_activate_wlan_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if (QCMAP_WLAN::ActivateWLAN(&qmi_err_num))
  {
    LOG_MSG_INFO1("Activate WLAN succeeds.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Activate WLAN fails.\n",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_activate_wlan_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1("\nActivated WLAN",0,0,0);
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_activate_wlan_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_qcmap_msgr_activate_wlan() */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_ACTIVATE_HOSTAPD_CONFIG()

  DESCRIPTION
    Activates Hostapd with available config.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_activate_hostapd_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_activate_hostapd_config_resp_msg_v01   resp_msg;
  qcmap_msgr_activate_hostapd_config_req_msg_v01     *req_ptr;
  qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type = QCMAP_MSGR_PRIMARY_AP_V01;
  qcmap_msgr_activate_hostapd_action_enum_v01 action_type = QCMAP_MSGR_ACTIVATE_HOSTAPD_ACTION_ENUM_MIN_ENUM_VAL_V01;

  unsigned int     index;
  boolean privileged_client = false;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_activate_hostapd_config_req_msg_v01 *)req_c_struct;

  memset(&resp_msg, 0, sizeof(qcmap_msgr_activate_hostapd_config_resp_msg_v01));

  if ( req_ptr->privileged_client_valid )
  {
    privileged_client = req_ptr->privileged_client;
  }

  index = (req_ptr->mobile_ap_handle);

  /* Skip the validation for privileged client. */
  if ((!privileged_client) &&
      (index != QCMAP_MSGR_SOFTAP_HANDLE))
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_activate_hostapd_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p activating Hostapd", clnt_info,0,0);

  if ( req_ptr->ap_type_valid )
  {
    ap_type = req_ptr->ap_type;
  }

  if ( req_ptr->action_type_valid )
  {
    action_type = req_ptr->action_type;
  }

  LOG_MSG_INFO1 ("ap_type = %d, action_type = %d, privileged client = %d",
                 ap_type, action_type, privileged_client);

  /*---------------------------------------------------------------------
    Activate Hostapd with the current configuration.
   ---------------------------------------------------------------------*/

  if (QCMAP_WLAN::ActivateHostapdConfig(ap_type, action_type, &qmi_err_num))
  {
    LOG_MSG_INFO1("Activate Hostapd succeeds.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Activate Hostapd fails.\n",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_activate_hostapd_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1("\nActivated Hostapd",0,0,0);
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_activate_hostapd_config_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_qcmap_msgr_activate_hostapd_config() */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_ACTIVATE_SUPPLICANT_CONFIG()

  DESCRIPTION
    Activates Supplicant with available config.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_activate_supplicant_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_activate_supplicant_config_resp_msg_v01   resp_msg;
  qcmap_msgr_activate_supplicant_config_req_msg_v01     *req_ptr;

  unsigned int     index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_activate_supplicant_config_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p activating Supplicant", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_activate_supplicant_config_resp_msg_v01));

  /*---------------------------------------------------------------------
   Activate Supplicant with the current configuration.
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_activate_supplicant_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if (QCMAP_Backhaul_WLAN::ActivateSupplicantConfig(&qmi_err_num))
  {
    LOG_MSG_INFO1("Activate Supplicant succeeds.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Activate Supplicant fails.\n",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_activate_supplicant_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1("\nActivated Supplicant",0,0,0);
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_activate_supplicant_config_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_qcmap_msgr_activate_supplicant_config() */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_ACTIVE_WLAN_IF_INFO()

  DESCRIPTION
    Obtains information of active WLAN interfaces.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_get_active_wlan_if_info
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_get_active_wlan_if_info_resp_msg_v01  resp_msg;
  qcmap_msgr_get_active_wlan_if_info_req_msg_v01  *req_ptr;
  qcmap_msgr_wlan_mode_enum_v01 wlan_mode =
                                    QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01;

  uint8 entries = 0;
  unsigned int index = 0;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_active_wlan_if_info_resp_msg_v01));
  LOG_MSG_INFO1("Get WLAN IF info",0,0,0);

/*---------------------------------------------------------------------
  Is Wifi Enabled?
---------------------------------------------------------------------*/
  if (QCMAP_WLAN::GetWLANStatus(&wlan_mode, &qmi_err_num))
  {
    if ((QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01 == wlan_mode)  ||
            QCMAP_MSGR_WLAN_MODE_ENUM_MAX_ENUM_VAL_V01 == wlan_mode)
    {
      LOG_MSG_ERROR("WLAN is not UP",0,0,0);
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      qmi_err_num = QMI_ERR_INVALID_OPERATION_V01;
      resp_msg.resp.error = qmi_err_num;
      qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_active_wlan_if_info_resp_msg_v01));
      return QMI_CSI_CB_NO_ERR;
    }
  }
  else
  {
    LOG_MSG_ERROR("Get WLAN Status failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_active_wlan_if_info_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  /*---------------------------------------------------------------------
   Get Active WLAN IF info.
  ---------------------------------------------------------------------*/
  if (QCMAP_WLAN::GetActiveWLANIFInfo(resp_msg.wlan_if_info, &entries, &qmi_err_num))
  {
    LOG_MSG_INFO1("Obtained active wlan IF info successfully",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Failed to obtain active wlan IF info",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_active_wlan_if_info_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if (entries > 0)
  {
    resp_msg.wlan_if_info_valid = TRUE;
    resp_msg.wlan_if_info_len = QCMAP_MAX_NUM_INTF;
    resp_msg.active_entries = entries;
    resp_msg.active_entries_valid = TRUE;
  }
  else
  {
    resp_msg.wlan_if_info_valid = FALSE;
    resp_msg.wlan_if_info_len = 0;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_active_wlan_if_info_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_qcmap_msgr_get_active_wlan_if_info() */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_ROAMING()

  DESCRIPTION
    Gets Roaming flag value on Apps.

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
static qmi_csi_cb_error qmi_qcmap_msgr_get_roaming
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_get_roaming_pref_req_msg_v01 *req_ptr;
  qcmap_msgr_get_roaming_pref_resp_msg_v01 resp_msg;
  QCMAP_Backhaul_WWAN  *QCMAP_Backhaul_WWAN_Obj = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_roaming_pref_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p get roaming flag", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_roaming_pref_resp_msg_v01));

  QCMAP_Backhaul_WWAN_Obj = GET_BACKHAUL_WWAN_OBJ_FOR_QMI_CLIENT(clnt_info);
  CHECK_QCMAP_OBJ(QCMAP_Backhaul_WWAN_Obj, resp_msg);

  /*---------------------------------------------------------------------
       Get Roaming flag
  ---------------------------------------------------------------------*/
  resp_msg.allow_wwan_calls_while_roaming_valid = TRUE;
  resp_msg.allow_wwan_calls_while_roaming = QCMAP_Backhaul_WWAN_Obj->GetRoaming();
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_roaming_pref_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_SET_ROAMING()

  DESCRIPTION
    Set roaming flag value on Apps.

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
static qmi_csi_cb_error qmi_qcmap_msgr_set_roaming
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_set_roaming_pref_req_msg_v01 *req_ptr;
  qcmap_msgr_set_roaming_pref_resp_msg_v01 resp_msg;
  unsigned int     index;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_Backhaul_WWAN  *QCMAP_Backhaul_WWAN_Obj = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_set_roaming_pref_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p set roaming flag", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_roaming_pref_resp_msg_v01));

  /*---------------------------------------------------------------------

  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_roaming_pref_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  QCMAP_Backhaul_WWAN_Obj = GET_BACKHAUL_WWAN_OBJ_FOR_QMI_CLIENT(clnt_info);
  CHECK_QCMAP_OBJ(QCMAP_Backhaul_WWAN_Obj, resp_msg);

  if (QCMAP_Backhaul_WWAN_Obj->SetRoaming(req_ptr->allow_wwan_calls_while_roaming, &qmi_err_num))
  {
    LOG_MSG_INFO1("Set Roaming flag successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Set Roaming flag failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_roaming_pref_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_roaming_pref_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_TETHERED_LINK_UP()

  DESCRIPTION
    Sets up the RNDIS/ECM/ETH tethered link.

  PARAMETERS
    sp:          QMI_QCMAP's service instance state pointer for this qmi link
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
static qmi_csi_cb_error  qmi_qcmap_msgr_tethered_link_up
(
  qmi_qcmap_msgr_client_info_type    *clnt_info,
  qmi_req_handle                     req_handle,
  int                                msg_id,
  void                               *req_c_struct,
  int                                req_c_struct_len,
  void                               *service_handle
)
{
  qmi_qcmap_msgr_state_info_type        *qcmap_sp;
  qcmap_msgr_usb_link_up_req_msg_v01    *req_ptr;
  qcmap_msgr_usb_link_up_resp_msg_v01   resp_msg;
  qmi_qcmap_msgr_softap_handle_type     *map_handle;
  qmi_csi_cb_error                      rc;
  boolean                               retval;
  int16                                 ps_errno;
  unsigned int                          index;
  int                                   err_num=0;
  qcmap_qti_tethered_link_type          tethered_link = QCMAP_QTI_TETHERED_LINK_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  LOG_MSG_INFO1 ("TETHERED link up message received", 0, 0, 0);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_usb_link_up_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_usb_link_up_resp_msg_v01));
  rc = QMI_CSI_CB_INTERNAL_ERR;

/*-------------------------------------------------------------------------
  Sanity check for qcmap handle
--------------------------------------------------------------------------*/
  index = req_ptr->mobile_ap_handle;
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_usb_link_up_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p bringing up TETHERED link for QTI in MobileAP inst %d",
                 clnt_info, index,0);

  if(req_ptr->usb_link == QCMAP_MSGR_USB_LINK_RNDIS_V01)
  {
    tethered_link = QCMAP_QTI_TETHERED_LINK_RNDIS;
  }
  else if(req_ptr->usb_link == QCMAP_MSGR_USB_LINK_ECM_V01)
  {
    tethered_link = QCMAP_QTI_TETHERED_LINK_ECM;
  }
  else if(req_ptr->usb_link == QCMAP_MSGR_USB_LINK_PPP_V01)
  {
    tethered_link = QCMAP_QTI_TETHERED_LINK_PPP;
  }
  else if(req_ptr->usb_link == QCMAP_MSGR_TETHERED_LINK_ETH_V01)
  {
    tethered_link = QCMAP_QTI_TETHERED_LINK_ETH;
  }
  else
  {
    LOG_MSG_ERROR("Setup of TETHERED link failed for QTI",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_NO_EFFECT_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_usb_link_up_resp_msg_v01));
  }

  if (!(retval = QCMAP_Tethering::SetupTetheredLink(tethered_link,&err_num)))
  {
    LOG_MSG_ERROR("Setup of TETHERED link failed for QTI",0,0,0);

    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_NO_EFFECT_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_usb_link_up_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_usb_link_up_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_TETHERED_LINK_DOWN()

  DESCRIPTION
    Brings down the RNDIS/ECM USB tethered link.

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_qcmap_msgr_tethered_link_down(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle                         req_handle,
  int                                    msg_id,
  void                                   *req_c_struct,
  int                                    req_c_struct_len,
  void                                   *service_handle
)
{
  qmi_qcmap_msgr_state_info_type        *qcmap_sp;
  qcmap_msgr_usb_link_down_req_msg_v01  *req_ptr;
  qcmap_msgr_usb_link_down_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type     *map_handle;
  qmi_csi_cb_error                      rc;
  boolean                               retval;
  unsigned int                          index;
  int                                   err_num=0;
  qcmap_qti_tethered_link_type          tethered_link = QCMAP_QTI_TETHERED_LINK_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  LOG_MSG_INFO1 ("TETHERED link down message received", 0, 0, 0);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_usb_link_down_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_usb_link_down_resp_msg_v01));
  rc = QMI_CSI_CB_INTERNAL_ERR;

/*-------------------------------------------------------------------------
  Sanity check for qcmap handle
--------------------------------------------------------------------------*/
  index = req_ptr->mobile_ap_handle;
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_usb_link_down_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p bringing down TETHERED link for QTI in MobileAP inst %d",
                 clnt_info, index,0);

  if(req_ptr->usb_link == QCMAP_MSGR_USB_LINK_RNDIS_V01)
  {
    tethered_link = QCMAP_QTI_TETHERED_LINK_RNDIS;
    LOG_MSG_INFO1 ("TETHERED link RNDIS", 0, 0, 0);
  }
  else if(req_ptr->usb_link == QCMAP_MSGR_USB_LINK_ECM_V01)
  {
    tethered_link = QCMAP_QTI_TETHERED_LINK_ECM;
    LOG_MSG_INFO1 ("TETHERED link ECM", 0, 0, 0);
  }
  else if(req_ptr->usb_link == QCMAP_MSGR_USB_LINK_PPP_V01)
  {
    tethered_link = QCMAP_QTI_TETHERED_LINK_PPP;
    LOG_MSG_INFO1 ("TETHERED link PPP", 0, 0, 0);
  }
  else if(req_ptr->usb_link == QCMAP_MSGR_TETHERED_LINK_ETH_V01)
  {
    tethered_link = QCMAP_QTI_TETHERED_LINK_ETH;
    LOG_MSG_INFO1 ("TETHERED link ETH", 0, 0, 0);
  }
  else
  {
    LOG_MSG_ERROR("Bring down of TETHERED link failed for QTI",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_NO_EFFECT_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_usb_link_down_resp_msg_v01));

  }

  if (!(retval = QCMAP_Tethering::BringDownTetheredLink(tethered_link, map_handle, &err_num)))
  {
    LOG_MSG_ERROR("Bring down of TETHERED link failed for QTI",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_NO_EFFECT_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_usb_link_down_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_usb_link_down_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_SET_FIREWALL_CONFIG()

  DESCRIPTION
    Sets Roaming flag value on Apps.

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
static qmi_csi_cb_error qmi_qcmap_msgr_set_firewall_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_set_firewall_config_req_msg_v01 *req_ptr;
  qcmap_msgr_set_firewall_config_resp_msg_v01 resp_msg;
  boolean enable_firewall, pkts_allowed,upnp_pinhole_flag;
  qmi_error_type_v01  qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_Firewall* QcMapFirewall = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  enable_firewall = false;
  pkts_allowed = false;
  upnp_pinhole_flag = false;

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_set_firewall_config_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p set firewall config", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_firewall_config_resp_msg_v01));

  QcMapFirewall = GET_FIREWALL_OBJ_FOR_QMI_CLIENT(clnt_info);

  /* Return failure if QcMapFirewall is NULL */
  CHECK_QCMAP_OBJ(QcMapFirewall, resp_msg);

  /*---------------------------------------------------------------------

  ---------------------------------------------------------------------*/
  enable_firewall = req_ptr->firewall_enabled;
  if( req_ptr->pkts_allowed_valid )
  {
    pkts_allowed = req_ptr->pkts_allowed;
  }
  if( req_ptr->upnp_pinhole_flag_valid)
  {
    upnp_pinhole_flag = req_ptr->upnp_pinhole_flag;
  }

  LOG_MSG_INFO1 ("Firewall: %d pckt: %d upnp: %d", enable_firewall,pkts_allowed,upnp_pinhole_flag);

  if (QcMapFirewall &&
      QcMapFirewall->SetFirewallConfig( enable_firewall, pkts_allowed, upnp_pinhole_flag,
                                        &qmi_err_num, req_ptr->pkts_allowed_valid,
                                        req_ptr->upnp_pinhole_flag_valid ))
  {
    LOG_MSG_INFO1("Set firewall config successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Set firewall config failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    if(QcMapFirewall == NULL)
      resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_firewall_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_firewall_config_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_FIREWALL_CONFIG()

  DESCRIPTION
    Gets Roaming flag value on Apps.

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
static qmi_csi_cb_error qmi_qcmap_msgr_get_firewall_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qcmap_msgr_get_firewall_config_req_msg_v01 *req_ptr;
  qcmap_msgr_get_firewall_config_resp_msg_v01 resp_msg;
  qmi_error_type_v01  qmi_err_num = QMI_ERR_NONE_V01;
  int err_num=0;
  boolean enable_firewall, pkts_allowed,upnp_pinhole_flag;
  QCMAP_Firewall* QcMapFirewall = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);
  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_firewall_config_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p get firewall flag", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_firewall_config_resp_msg_v01));

  QcMapFirewall = GET_FIREWALL_OBJ_FOR_QMI_CLIENT(clnt_info);

  /* Check if QcMapFirewall is NULL and retun failure */
  CHECK_QCMAP_OBJ(QcMapFirewall, resp_msg);

  /*---------------------------------------------------------------------

  ---------------------------------------------------------------------*/

  if(QcMapFirewall && QcMapFirewall->GetFirewallConfig(&enable_firewall, &pkts_allowed,
                                                       &upnp_pinhole_flag,&qmi_err_num))
  {
    resp_msg.firewall_enabled_valid = TRUE;
    resp_msg.firewall_enabled = enable_firewall;

    resp_msg.pkts_allowed_valid = TRUE;
    resp_msg.pkts_allowed = pkts_allowed;

    resp_msg.upnp_pinhole_flag_valid = TRUE;
    resp_msg.upnp_pinhole_flag = upnp_pinhole_flag;

    LOG_MSG_INFO1("get firewall config successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("get firewall config failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    if(QcMapFirewall == NULL)
      resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_get_firewall_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_firewall_config_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
FUNCTION QMI_QCMAP_MSGR_GET_WWAN_POLICY()

  DESCRIPTION
    Gets wwan profile..

  PARAMETERS

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_get_wwan_policy
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qcmap_msgr_get_wwan_policy_req_msg_v01  *req_ptr;
  qcmap_msgr_get_wwan_policy_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_Backhaul_WWAN     *QCMAP_Backhaul_WWAN_Obj = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_wwan_policy_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1("Client %p Get WWAN Config\n", clnt_info, 0, 0);

  memset( &resp_msg, 0, sizeof(qcmap_msgr_get_wwan_policy_resp_msg_v01 ) );

  QCMAP_Backhaul_WWAN_Obj = GET_BACKHAUL_WWAN_OBJ_FOR_QMI_CLIENT(clnt_info);
  CHECK_QCMAP_OBJ(QCMAP_Backhaul_WWAN_Obj, resp_msg);
  /*---------------------------------------------------------------------
   Get WWAN Config.
  ---------------------------------------------------------------------*/
  if (QCMAP_Backhaul_WWAN_Obj->GetWWANPolicy(&resp_msg, &qmi_err_num ))
  {
    LOG_MSG_INFO1("Get WWAN Config success.\n", 0, 0, 0);
  }
  else
  {
    LOG_MSG_ERROR("Get WWAN Config failed: %d", qmi_err_num, 0, 0);
    memset( &resp_msg, 0, sizeof(qcmap_msgr_get_wwan_policy_resp_msg_v01 ) );
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_wwan_policy_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  resp_msg.wwan_policy_valid = TRUE;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_wwan_policy_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}
/*===========================================================================
FUNCTION QMI_QCMAP_MSGR_SET_WWAN_POLICY()

  DESCRIPTION
    Sets wwan profile.

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_set_wwan_policy
(
 qmi_qcmap_msgr_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
 )
{

  qmi_qcmap_msgr_state_info_type         *qcmap_sp;
  qcmap_msgr_set_wwan_policy_resp_msg_v01 resp_msg;
  qcmap_msgr_set_wwan_policy_req_msg_v01 *req_ptr;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  unsigned int                          index;
  qmi_qcmap_msgr_softap_handle_type     *map_handle;
  qcmap_msgr_net_policy_info_v01        WWAN_policy ;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  LOG_MSG_INFO1("Client %p Set WWAN Config", clnt_info, 0, 0);
  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_set_wwan_policy_req_msg_v01 *)req_c_struct;
  memset( &resp_msg, 0, sizeof(qcmap_msgr_set_wwan_policy_resp_msg_v01 ) );
  WWAN_policy = req_ptr->wwan_policy;
  /*-------------------------------------------------------------------------
  Sanity check for qcmap handle
--------------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if ( index != QCMAP_MSGR_SOFTAP_HANDLE ||
     ( map_handle = &qcmap_handle)->handle == 0 )
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_wwan_policy_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  /*---------------------------------------------------------------------
   Set WWAN Config.
  ---------------------------------------------------------------------*/
  if (QCMAP_Backhaul_WWAN::SetWWANPolicy(WWAN_policy, &qmi_err_num))
  {
    LOG_MSG_INFO1("Set WWAN Config successfully.\n", 0, 0, 0);
  }
  else
  {
    LOG_MSG_ERROR("Set WWAN Config failed: %d", qmi_err_num, 0, 0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_wwan_policy_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                  sizeof(qcmap_msgr_set_wwan_policy_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}
/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_IPV4_STATE()

  DESCRIPTION
    Gets the state of ipv4.

  PARAMETERS

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_get_ipv4_state
(
 qmi_qcmap_msgr_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
 )
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_get_ipv4_state_resp_msg_v01 resp_msg;
  uint8_t status;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);
  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  memset( &resp_msg, 0, sizeof(qcmap_msgr_get_ipv4_state_resp_msg_v01 ) );
  if (QCMAP_Backhaul::GetIPv4State(&status, &qmi_err_num))
  {
    resp_msg.ipv4_state_valid = TRUE;
    resp_msg.ipv4_state = status;
    LOG_MSG_INFO1("\n ipv4 state successfully determined ", 0, 0, 0);
  }
  else
  {
    LOG_MSG_ERROR("Get IPV4 state failed: %d", qmi_err_num, 0, 0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof( qcmap_msgr_get_ipv4_state_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
      sizeof(qcmap_msgr_get_ipv4_state_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}
/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_IPV6_STATE()

  DESCRIPTION
    Gets the state of ipv6.

  PARAMETERS

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_get_ipv6_state
(
 qmi_qcmap_msgr_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
 )
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_get_ipv6_state_resp_msg_v01 resp_msg;
  uint8_t status;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);
  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  memset( &resp_msg, 0, sizeof(qcmap_msgr_get_ipv6_state_resp_msg_v01 ) );
  if (QCMAP_Backhaul::GetIPv6State(&status, &qmi_err_num))
  {
    resp_msg.ipv6_state_valid = TRUE;
    resp_msg.ipv6_state = status;
    LOG_MSG_INFO1("\n ipv6 state successfully determined ", 0, 0, 0);
  }
  else
  {
    LOG_MSG_ERROR("Get IPV6 state failed: %d", qmi_err_num, 0, 0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof( qcmap_msgr_get_ipv6_state_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
      sizeof(qcmap_msgr_get_ipv6_state_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_ENABLE_UPNP()

  DESCRIPTION
    Enable UPnP daemon.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_enable_upnp
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qcmap_msgr_enable_upnp_resp_msg_v01   resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;

  LOG_MSG_INFO1 ("Client %p enabling UPNP", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_enable_upnp_resp_msg_v01));

  /*---------------------------------------------------------------------
    Enable UPnP
  ---------------------------------------------------------------------*/

  if (QCMAP_MediaService::EnableUPNP(&qmi_err_num))
  {
    LOG_MSG_INFO1("Enable UPNP succeeds.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Enable UPNP fails.\n",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_enable_upnp_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1("\nEnabled UPNP",0,0,0);
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_enable_upnp_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_DISABLE_UPNP()

  DESCRIPTION
    Enable UPnP.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_disable_upnp
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qcmap_msgr_disable_upnp_resp_msg_v01   resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;

  LOG_MSG_INFO1 ("Client %p disabling UPNP", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_disable_upnp_resp_msg_v01));

  /*---------------------------------------------------------------------
    Disable UPnP
  ---------------------------------------------------------------------*/

  if (QCMAP_MediaService::DisableUPNP(&qmi_err_num))
  {
    LOG_MSG_INFO1("Disable UPNP succeeds.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Disable UPNP fails.\n",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_disable_upnp_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1("\nDisabled UPNP",0,0,0);
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_disable_upnp_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_UPNP_STATUS()

  DESCRIPTION
    Returns the status of UPnP.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_get_upnp_status
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qcmap_msgr_get_upnp_status_resp_msg_v01   resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_upnp_mode_enum_v01 status = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  LOG_MSG_INFO1 ("Client %p returning UPNP status", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_upnp_status_resp_msg_v01));

  /*---------------------------------------------------------------------
    Enable UPnP
  ---------------------------------------------------------------------*/

  if (QCMAP_MediaService::GetUPNPStatus(&status, &qmi_err_num))
  {
    resp_msg.upnp_mode_valid = true;
    resp_msg.upnp_mode = status;
  }
  else
  {
    LOG_MSG_ERROR("Get UPnP status fails.\n",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num = QMI_ERR_NONE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_upnp_status_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_upnp_status_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_ENABLE_DLNA()

  DESCRIPTION
    Enable DLNA.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_enable_dlna
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qcmap_msgr_enable_dlna_resp_msg_v01   resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;

  LOG_MSG_INFO1 ("Client %p enabling DLNA", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_enable_dlna_resp_msg_v01));

  /*---------------------------------------------------------------------
    Enable DLNA
  ---------------------------------------------------------------------*/

  if (QCMAP_MediaService::EnableDLNA(&qmi_err_num))
  {
    LOG_MSG_INFO1("Enable DLNA succeeds.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Enable DLNA fails.\n",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_enable_dlna_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1("\nEnabled DLNA",0,0,0);
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_enable_dlna_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_DISABLE_DLNA()

  DESCRIPTION
    Enable DLNA.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_disable_dlna
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qcmap_msgr_disable_dlna_resp_msg_v01   resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;

  LOG_MSG_INFO1 ("Client %p disabling DLNA", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_disable_dlna_resp_msg_v01));

  /*---------------------------------------------------------------------
    Disable DLNA
  ---------------------------------------------------------------------*/

  if (QCMAP_MediaService::DisableDLNA(&qmi_err_num))
  {
    LOG_MSG_INFO1("Disable DLNA succeeds.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Disable DLNA fails.\n",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_disable_dlna_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1("\nDisabled DLNA",0,0,0);
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_disable_dlna_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_DLNA_STATUS()

  DESCRIPTION
    Returns the status of DLNA.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_get_dlna_status
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qcmap_msgr_get_dlna_status_resp_msg_v01   resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_dlna_mode_enum_v01 status = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  LOG_MSG_INFO1 ("Client %p returning DLNA status", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_dlna_status_resp_msg_v01));

  /*---------------------------------------------------------------------
    Get DLNA Status
  ---------------------------------------------------------------------*/

  if (QCMAP_MediaService::GetDLNAStatus(&status, &qmi_err_num))
  {
    resp_msg.dlna_mode_valid = true;
    resp_msg.dlna_mode = status;
  }
  else
  {
    LOG_MSG_ERROR("Get DLNA status fails.\n",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_dlna_status_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_dlna_status_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_SET_DLNA_MEDIA_DIR()

  DESCRIPTION
    Configures the DLNA media directory.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_set_dlna_media_dir
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qcmap_msgr_set_dlna_media_dir_resp_msg_v01   resp_msg;
  qcmap_msgr_set_dlna_media_dir_req_msg_v01    *req_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  LOG_MSG_INFO1 ("Client %p setting DLNA media directory", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_dlna_media_dir_resp_msg_v01));

  req_msg = (qcmap_msgr_set_dlna_media_dir_req_msg_v01*) req_c_struct;

  /*---------------------------------------------------------------------
    Set DLNA Media Directories
  ---------------------------------------------------------------------*/

  if (QCMAP_MediaService::SetDLNAMediaDir(req_msg->media_dir, &qmi_err_num))
  {
    LOG_MSG_INFO1("Set DLNA Media Dir success.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Set DLNA Media Dir fails.\n",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_dlna_media_dir_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_dlna_media_dir_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_DLNA_MEDIA_DIR()

  DESCRIPTION
    Returns the DLNA media directory.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_get_dlna_media_dir
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qcmap_msgr_get_dlna_media_dir_resp_msg_v01   resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  char             media_dir[QCMAP_MSGR_MAX_DLNA_DIR_LEN_V01] = "";
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  LOG_MSG_INFO1 ("Client %p returning DLNA media directory", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_dlna_media_dir_resp_msg_v01));

  /*---------------------------------------------------------------------
    Retreive DLNA Media Directory
  ---------------------------------------------------------------------*/

  if (QCMAP_MediaService::GetDLNAMediaDir( media_dir, &qmi_err_num))
  {
    strlcpy(resp_msg.media_dir, media_dir, sizeof(resp_msg.media_dir));
    resp_msg.media_dir_valid = true;
    resp_msg.media_dir_len = sizeof(resp_msg.media_dir);
  }
  else
  {
    LOG_MSG_ERROR("Get DLNA status fails.\n",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_dlna_media_dir_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_dlna_media_dir_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_ENABLE_MULTICAST_DNS_RESPONDER()

  DESCRIPTION
    Enable M-DNS Daemon

  PARAMETERS
    sp:         QMI_QCMAP MSGR's service instance state pointer for this qmi link
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
static qmi_csi_cb_error  qmi_qcmap_msgr_enable_multicast_dns_responder
(
  qmi_qcmap_msgr_client_info_type    *clnt_info,
  qmi_req_handle                     req_handle,
  int                                msg_id,
  void                               *req_c_struct,
  int                                req_c_struct_len,
  void                               *service_handle
)
{
  qmi_qcmap_msgr_state_info_type                *qcmap_sp;
  qcmap_msgr_enable_multicast_dns_resp_msg_v01  resp_msg;
  qmi_error_type_v01                            qmi_err_num = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;

  LOG_MSG_INFO1 ("Client %p enabling M-DNS", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_enable_multicast_dns_resp_msg_v01));

  /*---------------------------------------------------------------------
    Enable M-DNS Daemon
  ---------------------------------------------------------------------*/

  if (QCMAP_MediaService::EnableMDNS(&qmi_err_num))
  {
    LOG_MSG_INFO1("Enable M-DNS succeeds.\n",0,0,0);
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_enable_multicast_dns_resp_msg_v01));

    return QMI_CSI_CB_NO_ERR;
  }
  else
  {
    LOG_MSG_ERROR("Enable M-DNS fails.\n",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_enable_multicast_dns_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
}


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_DISABLE_MULTICAST_DNS_RESPONDER()

  DESCRIPTION
    Enable M-DNS Daemon.

  PARAMETERS
    sp:         QMI_QCMAP MSGR's service instance state pointer for this qmi link
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
static qmi_csi_cb_error  qmi_qcmap_msgr_disable_multicast_dns_responder
(
  qmi_qcmap_msgr_client_info_type    *clnt_info,
  qmi_req_handle                     req_handle,
  int                                msg_id,
  void                               *req_c_struct,
  int                                req_c_struct_len,
  void                               *service_handle
)
{
  qmi_qcmap_msgr_state_info_type                 *qcmap_sp;
  qcmap_msgr_disable_multicast_dns_resp_msg_v01  resp_msg;
  qmi_error_type_v01                             qmi_err_num = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;

  LOG_MSG_INFO1 ("Client %p disabling M-DNS", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_disable_multicast_dns_resp_msg_v01));

  /*---------------------------------------------------------------------
    Disable M-DNS
  ---------------------------------------------------------------------*/

  if (QCMAP_MediaService::DisableMDNS(&qmi_err_num))
  {
    LOG_MSG_INFO1("Disable M-DNS succeeds.\n",0,0,0);
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_disable_multicast_dns_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  else
  {
    LOG_MSG_ERROR("Disable M-DNS fails.\n",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_disable_multicast_dns_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
}


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_MULTICAST_DNS_RESPONDER_STATUS()

  DESCRIPTION
    Returns the status of MDNS.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_get_multicast_dns_responder_status
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qcmap_msgr_get_multicast_dns_status_resp_msg_v01   resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_mdns_mode_enum_v01 status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  LOG_MSG_INFO1 ("Client %p returning MDNS status", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_multicast_dns_status_resp_msg_v01));

  /*---------------------------------------------------------------------
    Enable UPnP
  ---------------------------------------------------------------------*/

  if (QCMAP_MediaService::GetMDNSStatus(&status, &qmi_err_num))
  {
    resp_msg.mdns_mode_valid = true;
    resp_msg.mdns_mode = status;
  }
  else
  {
    LOG_MSG_ERROR("Get MDNS status fails.\n",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_multicast_dns_status_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_multicast_dns_status_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_SET_QCMAP_BOOTUP_CFG()

  DESCRIPTION
  Set the bootup configuration for QCMAP componets

  PARAMETERS
    sp:         QMI_QCMAP MSGR's service instance state pointer for this qmi link
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
static qmi_csi_cb_error  qmi_qcmap_msgr_set_qcmap_bootup_cfg
(
  qmi_qcmap_msgr_client_info_type    *clnt_info,
  qmi_req_handle                     req_handle,
  int                                msg_id,
  void                               *req_c_struct,
  int                                req_c_struct_len,
  void                               *service_handle
)
{
  qcmap_msgr_set_qcmap_bootup_cfg_resp_msg_v01 resp_msg;
  qcmap_msgr_set_qcmap_bootup_cfg_req_msg_v01  *req_ptr = NULL;
  qmi_error_type_v01                           qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_bootup_flag_v01            mobileap_enable = QCMAP_MSGR_BOOTUP_FLAG_MIN_ENUM_VAL_V01;
  qcmap_msgr_bootup_flag_v01            wlan_enable = QCMAP_MSGR_BOOTUP_FLAG_MIN_ENUM_VAL_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);
  ds_assert(req_c_struct != NULL);

  if (req_ptr = (qcmap_msgr_set_qcmap_bootup_cfg_req_msg_v01*)req_c_struct) {
     // Mobile AP Bootup Flag
     if (req_ptr->mobileap_bootup_flag_valid) {
        mobileap_enable = req_ptr->mobileap_bootup_flag;
     }
     // WLAN Bootup Flag
     if (req_ptr->wlan_bootup_flag_valid) {
        wlan_enable = req_ptr->wlan_bootup_flag;
     }
  }

   LOG_MSG_INFO1 ("qmi_qcmap_msgr_set_qcmap_bootup_cfg mobileap_enable = %d mobileap_enable_vaildi =%d ",
                   req_ptr->mobileap_bootup_flag,
                   req_ptr->mobileap_bootup_flag_valid,0);
   LOG_MSG_INFO1 ("qmi_qcmap_msgr_set_qcmap_bootup_cfg wlan_enable = %d wlan_enable_vaild =%d ",
                   req_ptr->wlan_bootup_flag,
                   req_ptr->wlan_bootup_flag_valid,0);

  LOG_MSG_INFO1 ("Client %p setting booutp configuration for QCMAP", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_qcmap_bootup_cfg_resp_msg_v01));

  /*---------------------------------------------------------------------
    Set QCMAP Bootup CFG
  ---------------------------------------------------------------------*/

  if (QCMAP_ConnectionManager::SetQCMAPBootupConfig(mobileap_enable, wlan_enable, &qmi_err_num))
  {
    LOG_MSG_INFO1("Set QCMAP Bootup CFG succeeds.\n",0,0,0);
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_qcmap_bootup_cfg_resp_msg_v01));

    return QMI_CSI_CB_NO_ERR;
  }
  else
  {
    LOG_MSG_ERROR(" Set QCMAP Bootup CFG Fails \n",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_qcmap_bootup_cfg_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
}

/*===========================================================================
  FUNCTION  QMI_QCMAP_MSGR_GET_QCMAP_BOOTUP_CFG()

  DESCRIPTION
  Get QCMAP Bootup Config

  PARAMETERS
    sp:         QMI_QCMAP MSGR's service instance state pointer for this qmi link
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
static qmi_csi_cb_error  qmi_qcmap_msgr_get_qcmap_bootup_cfg
(
  qmi_qcmap_msgr_client_info_type    *clnt_info,
  qmi_req_handle                     req_handle,
  int                                msg_id,
  void                               *req_c_struct,
  int                                req_c_struct_len,
  void                               *service_handle
)
{
  qcmap_msgr_get_qcmap_bootup_cfg_resp_msg_v01  resp_msg;
  qmi_error_type_v01                            qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_bootup_flag_v01             mobileap_enable, wlan_enable;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  LOG_MSG_INFO1 ("Client %p Getting QCMAP Bootup Cfg", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_qcmap_bootup_cfg_resp_msg_v01));

  /*---------------------------------------------------------------------
    Get QCMAP Bootup Config
  ---------------------------------------------------------------------*/

  if (QCMAP_ConnectionManager::GetQCMAPBootupConfig(&mobileap_enable, &wlan_enable, &qmi_err_num)) {
    LOG_MSG_INFO1(" GetQCMAPBootupConfig succeeds.\n",0,0,0);
    resp_msg.mobileap_bootup_flag         = mobileap_enable;
    resp_msg.mobileap_bootup_flag_valid = true;
    resp_msg.wlan_bootup_flag                = wlan_enable;
    resp_msg.wlan_bootup_flag_valid        = true;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,sizeof(qcmap_msgr_get_qcmap_bootup_cfg_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  } else {
    LOG_MSG_ERROR("GetQCMAPBootupConfig fails.\n",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg, sizeof(qcmap_msgr_get_qcmap_bootup_cfg_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
}

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_DATA_BITRATE()

  DESCRIPTION
    Get the current and max data bitrates

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
static qmi_csi_cb_error qmi_qcmap_msgr_get_data_bitrate
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qcmap_msgr_data_bitrate_v01 data_rate;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_get_data_bitrate_resp_msg_v01 resp_msg;
  qcmap_msgr_get_data_bitrate_req_msg_v01 *req_ptr;
  unsigned int             index;
  QCMAP_Backhaul_WWAN  *QCMAP_Backhaul_WWAN_Obj = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  printf ("Client %p Get Data Bitrate\n", clnt_info);

  req_ptr = (qcmap_msgr_get_data_bitrate_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_data_bitrate_resp_msg_v01));
  memset(&data_rate, 0, sizeof(qcmap_msgr_data_bitrate_v01));
  /*---------------------------------------------------------------------
   Get Data bitrates.
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (&qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_get_data_bitrate_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  QCMAP_Backhaul_WWAN_Obj = GET_BACKHAUL_WWAN_OBJ_FOR_QMI_CLIENT(clnt_info);
  CHECK_QCMAP_OBJ(QCMAP_Backhaul_WWAN_Obj, resp_msg);

  if (QCMAP_Backhaul_WWAN_Obj->GetDataBitrate(&data_rate, &qmi_err_num))
  {
    QCMAP_CM_LOG("Got Data Bitrate successfully.\n");
  }
  else
  {
    QCMAP_CM_LOG("Get Data Bitrate failed!!");
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_data_bitrate_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  resp_msg.data_rate_valid = TRUE;
  resp_msg.data_rate = data_rate;

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_data_bitrate_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_data_bitrate */


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_SET_UPNP_NOTIFY_INTERVAL()

  DESCRIPTION
    Configures the UPnP notify interval.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_set_upnp_notify_interval
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qcmap_msgr_set_upnp_notify_interval_resp_msg_v01   resp_msg;
  qcmap_msgr_set_upnp_notify_interval_req_msg_v01    *req_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);
  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);

  LOG_MSG_INFO1 ("Client %p setting UPnP notify interval", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_upnp_notify_interval_resp_msg_v01));

  req_msg = (qcmap_msgr_set_upnp_notify_interval_req_msg_v01*) req_c_struct;

  /*---------------------------------------------------------------------
    Set UPnP notify interval
  ---------------------------------------------------------------------*/

  if (QCMAP_MediaService::SetUPNPNotifyInterval(req_msg->notify_interval, &qmi_err_num))
  {
    LOG_MSG_INFO1("Set UPnP notify interval success.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Set UPnP notify interval failsed: %d", qmi_err_num,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_upnp_notify_interval_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_upnp_notify_interval_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_UPNP_NOTIFY_INTERVAL()

  DESCRIPTION
    Returns the UPnP notify interval.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_get_upnp_notify_interval
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qcmap_msgr_get_upnp_notify_interval_resp_msg_v01   resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;

  int              notify_int;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  LOG_MSG_INFO1 ("Client %p returning UPnP notify interval", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_upnp_notify_interval_resp_msg_v01));

  /*---------------------------------------------------------------------
    Retreive UPnP notify interval
  ---------------------------------------------------------------------*/

  if (QCMAP_MediaService::GetUPNPNotifyInterval(&notify_int, &qmi_err_num))
  {
    resp_msg.notify_interval = notify_int;
    resp_msg.notify_interval_valid = true;
  }
  else
  {
    LOG_MSG_ERROR("Get UPnP notify interval failed: %d", qmi_err_num,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_upnp_notify_interval_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_upnp_notify_interval_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_SET_DLNA_NOTIFY_INTERVAL()

  DESCRIPTION
    Configures the DLNA notify interval.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_set_dlna_notify_interval
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qcmap_msgr_set_dlna_notify_interval_resp_msg_v01   resp_msg;
  qcmap_msgr_set_dlna_notify_interval_req_msg_v01    *req_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);
  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);

  LOG_MSG_INFO1 ("Client %p setting DLNA notify interval", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_dlna_notify_interval_resp_msg_v01));

  req_msg = (qcmap_msgr_set_dlna_notify_interval_req_msg_v01*) req_c_struct;

  /*---------------------------------------------------------------------
    Set DLNA notify interval
  ---------------------------------------------------------------------*/

  if (QCMAP_MediaService::SetDLNANotifyInterval(req_msg->notify_interval, &qmi_err_num))
  {
    LOG_MSG_INFO1("Set DLNA notify interval success.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Set DLNA notify interval failed: %d", qmi_err_num,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_dlna_notify_interval_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_dlna_notify_interval_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_DLNA_NOTIFY_INTERVAL()

  DESCRIPTION
    Returns the DLNA notify interval.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_get_dlna_notify_interval
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qcmap_msgr_get_dlna_notify_interval_resp_msg_v01   resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;

  int              notify_int;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  LOG_MSG_INFO1 ("Client %p returning DLNA notify interval", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_dlna_notify_interval_resp_msg_v01));

  /*---------------------------------------------------------------------
    Retreive DLNA notify interval
  ---------------------------------------------------------------------*/

  if (QCMAP_MediaService::GetDLNANotifyInterval(&notify_int, &qmi_err_num))
  {
    resp_msg.notify_interval = notify_int;
    resp_msg.notify_interval_valid = true;
  }
  else
  {
    LOG_MSG_ERROR("Get DLNA notify interval failed: %d", qmi_err_num,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_dlna_notify_interval_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_dlna_notify_interval_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}
/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_ADD_DHCP_RESERVATION_RECORD()

  DESCRIPTION
    Add DHCP Reservation Record.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_add_dhcp_reservation_record
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_add_dhcp_reservation_record_req_msg_v01 *req_ptr;
  qcmap_msgr_add_dhcp_reservation_record_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_LAN* default_lan_obj = GET_DEFAULT_LAN_OBJECT();
  unsigned int     index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_add_dhcp_reservation_record_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p requesting to add DHCP Reservation Record", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_add_dhcp_reservation_record_resp_msg_v01));

  /*---------------------------------------------------------------------
   Add DHCP Reservation Record
  ---------------------------------------------------------------------*/

  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_add_dhcp_reservation_record_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if(default_lan_obj && default_lan_obj->AddDHCPReservRecord(&(req_ptr->dhcp_reservation_record),
                                                             &qmi_err_num))
  {
    LOG_MSG_INFO1("Added DHCP Reservation Record successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Adding DHCP Reservation Record failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_add_dhcp_reservation_record_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_add_dhcp_reservation_record_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_add_dhcp_reservation_record() */


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_DHCP_RESERVATION_RECORDS()

  DESCRIPTION
    Get DHCP Reservation Records for AP

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
static qmi_csi_cb_error    qmi_qcmap_msgr_get_dhcp_reservation_records
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  QCMAP_LAN* default_lan_obj = GET_DEFAULT_LAN_OBJECT();
  qcmap_msgr_get_dhcp_reservation_records_req_msg_v01 *req_ptr;
  qcmap_msgr_get_dhcp_reservation_records_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_dhcp_reservation_records_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p get DHCP Reservation Records", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_dhcp_reservation_records_resp_msg_v01));

  /*---------------------------------------------------------------------
   Get DHCP Reservation Records
  ---------------------------------------------------------------------*/

  if(default_lan_obj && default_lan_obj->GetDHCPReservRecords(&resp_msg.dhcp_reservation_records[0],
                                              &resp_msg.dhcp_reservation_records_len, &qmi_err_num))
  {
    LOG_MSG_INFO1("Get DHCP Reservation Records succeeded.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Get DHCP Reservation records failed!!",0,0,0);

    resp_msg.dhcp_reservation_records_valid = false;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_dhcp_reservation_records_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  resp_msg.dhcp_reservation_records_valid = true;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_dhcp_reservation_records_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_dhcp_reservation_records() */



/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_EDIT_DHCP_RESERVATION_RECORD()

  DESCRIPTION
    Edits a DHCP record based on MAC or IP address

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
static qmi_csi_cb_error  qmi_qcmap_msgr_edit_dhcp_reservation_record
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_edit_dhcp_reservation_record_req_msg_v01 *req_ptr;
  qcmap_msgr_edit_dhcp_reservation_record_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_LAN* default_lan_obj = GET_DEFAULT_LAN_OBJECT();
  unsigned int     index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_edit_dhcp_reservation_record_req_msg_v01*)req_c_struct;
  qcmap_msgr_dhcp_reservation_v01 dhcp_reservation_record;

  LOG_MSG_INFO1 ("Client %p Editing a DHCP Reservation Record", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_edit_dhcp_reservation_record_resp_msg_v01));
  memset(&dhcp_reservation_record,0,sizeof(qcmap_msgr_dhcp_reservation_v01));

  /*---------------------------------------------------------------------
   Edit DHCP Reservation Record
  ---------------------------------------------------------------------*/

 index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_edit_dhcp_reservation_record_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  memcpy(&dhcp_reservation_record,&req_ptr->dhcp_reservation_record,\
         sizeof(qcmap_msgr_dhcp_reservation_v01));
  if(default_lan_obj && default_lan_obj->EditDHCPReservRecord(&req_ptr->client_reserved_ip,
                                                            &dhcp_reservation_record, &qmi_err_num))
  {
    LOG_MSG_INFO1("Edited DHCP Reservation Record successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Editing DHCP Reservation Record Failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_edit_dhcp_reservation_record_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_edit_dhcp_reservation_record_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_edit_dhcp_reservation_record() */


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_DELETE_DHCP_RESERVATION_RECORD()

  DESCRIPTION
    Deletes DHCP record based on MAC or IP address

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
static qmi_csi_cb_error  qmi_qcmap_msgr_delete_dhcp_reservation_record
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_delete_dhcp_reservation_record_req_msg_v01  *req_ptr;
  qcmap_msgr_delete_dhcp_reservation_record_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_LAN* default_lan_obj = GET_DEFAULT_LAN_OBJECT();
  unsigned int     index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_delete_dhcp_reservation_record_req_msg_v01*)req_c_struct;

  LOG_MSG_INFO1 ("Client %p Deleting a DHCP Reservation Record", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_delete_dhcp_reservation_record_resp_msg_v01));

  /*---------------------------------------------------------------------
   Delete the DHCP Reservation Record
  ---------------------------------------------------------------------*/

 index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_delete_dhcp_reservation_record_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if(default_lan_obj && default_lan_obj->DeleteDHCPReservRecord(&req_ptr->client_reserved_ip,
                                                                &qmi_err_num))
  {
    LOG_MSG_INFO1("Deleted DHCP Reservation Record successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Deleting DHCP Reservation Record Failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_delete_dhcp_reservation_record_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_delete_dhcp_reservation_record_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_delete_dhcp_reservation_record() */
/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_WEBSERVER_WWAN_ACCESS()

  DESCRIPTION
    Get webserver wwan access flag value on Apps.

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
static qmi_csi_cb_error qmi_qcmap_msgr_get_webserver_wwan_access_flag
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_get_webserver_wwan_access_req_msg_v01 *req_ptr;
  qcmap_msgr_get_webserver_wwan_access_resp_msg_v01 resp_msg;
  uint8    webserver_wwan_access_flag=0;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_webserver_wwan_access_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p get webserver wwan access flag", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_webserver_wwan_access_resp_msg_v01));

  /*---------------------------------------------------------------------

  ---------------------------------------------------------------------*/
  if (QCMAP_Backhaul::GetWebserverWWANAccessFlag(&webserver_wwan_access_flag, &qmi_err_num))
  {
    LOG_MSG_INFO1("Got Webserver WWAN Access Flag successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Get Webserver WWAN Access Flag failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_webserver_wwan_access_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  resp_msg.webserver_wwan_access_valid = TRUE;
  resp_msg.webserver_wwan_access = webserver_wwan_access_flag;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_webserver_wwan_access_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_webserver_wwan_access_flag */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_SET_WEBSERVER_WWAN_ACCESS()

  DESCRIPTION
    Set Webserver WWAN Access flag value on Apps.

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
static qmi_csi_cb_error qmi_qcmap_msgr_set_webserver_wwan_access_flag
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_set_webserver_wwan_access_req_msg_v01 *req_ptr;
  qcmap_msgr_set_webserver_wwan_access_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  unsigned int index;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_set_webserver_wwan_access_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p set webserver wwan access flag", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_webserver_wwan_access_resp_msg_v01));

  /*---------------------------------------------------------------------

  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_webserver_wwan_access_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  if (QCMAP_Backhaul::SetWebserverWWANAccess(req_ptr->webserver_wwan_access, &qmi_err_num))
  {
    LOG_MSG_INFO1("Set Webserver WWAN access successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Set webserver wwan access failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_webserver_wwan_access_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_webserver_wwan_access_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_set_webserver_wwan_access_flag */
/*===========================================================================
  FUNCTION QMI_QCMAPI_ENABLE_ALG()

  DESCRIPTION
    Enable  ALGs Functionality.

  PARAMETERS
    sp:          QMI_QCMAP's service instance state pointer for this qmi link
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
static qmi_csi_cb_error  qmi_qcmap_msgr_enable_alg
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type       *qcmap_sp;
  qcmap_msgr_enable_alg_req_msg_v01 *req_ptr;
  qcmap_msgr_enable_alg_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  unsigned int             index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_enable_alg_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_enable_alg_resp_msg_v01));

  /*---------------------------------------------------------------------
      Enable ALGs
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                   req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_enable_alg_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p Enabling ALGs in MobileAP inst %d mask %d",
                  clnt_info, index, req_ptr->alg_type_op);

  if (!(QCMAP_NATALG::EnableAlg(req_ptr->alg_type_op, &qmi_err_num)))
  {
    LOG_MSG_ERROR("Enabling ALGs %d failed Error: %d", req_ptr->alg_type_op,
                   qmi_err_num, 0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_enable_alg_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_enable_alg_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_enable_alg() */

/*===========================================================================
  FUNCTION QMI_QCMAPI_DISABLE_ALG()

  DESCRIPTION
    Disable ALGs Functionality.

  PARAMETERS
    sp:          QMI_QCMAP's service instance state pointer for this qmi link
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
static qmi_csi_cb_error  qmi_qcmap_msgr_disable_alg
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type       *qcmap_sp;
  qcmap_msgr_disable_alg_req_msg_v01 *req_ptr;
  qcmap_msgr_disable_alg_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  unsigned int             index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_disable_alg_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_disable_alg_resp_msg_v01));

  /*---------------------------------------------------------------------
      Disable ALGs
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_disable_alg_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p Disabling ALGs in MobileAP inst %d mask %d",
                  clnt_info, index, req_ptr->alg_type_op);

  if (!(QCMAP_NATALG::DisableAlg(req_ptr->alg_type_op, &qmi_err_num)))
  {
    LOG_MSG_ERROR("Disabling ALGs %d failed Error: %d ", req_ptr->alg_type_op,
                   qmi_err_num, 0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_disable_alg_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_disable_alg_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_disable_alg() */


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_SET_SIP_SERVER_INFO

  DESCRIPTION
    -Sets the default user configured SIP server information.

  DEPENDENCIES
    qmi_qcmap_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_qcmap_msgr_set_sip_server_info
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type              *qcmap_sp;
  qcmap_msgr_set_sip_server_info_req_msg_v01  *req_ptr;
  qcmap_msgr_set_sip_server_info_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type           *map_handle;
  boolean                                     retval;
  unsigned int                                index;
  qmi_error_type_v01                          qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_Backhaul_WWAN                        *QCMAP_Backhaul_WWAN_Obj = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_set_sip_server_info_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_sip_server_info_resp_msg_v01));


  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_sip_server_info_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  QCMAP_Backhaul_WWAN_Obj = GET_BACKHAUL_WWAN_OBJ_FOR_QMI_CLIENT(clnt_info);
  CHECK_QCMAP_OBJ(QCMAP_Backhaul_WWAN_Obj, resp_msg);

  LOG_MSG_INFO1 ("Client %p MobileAP inst %d set SIP server info type %d",
                  clnt_info, index, req_ptr->default_sip_server_info.pcscf_info_type);

  if (!(retval=QCMAP_Backhaul_WWAN_Obj->SetSIPServerInfo(req_ptr->default_sip_server_info, &qmi_err_num)))
  {
    LOG_MSG_ERROR("Set SIP server info type %d failed Error: %d ",
                  req_ptr->default_sip_server_info.pcscf_info_type,
                  qmi_err_num,
                  0);

    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_sip_server_info_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_sip_server_info_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_set_sip_server_info() */


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_SIP_SERVER_INFO

  DESCRIPTION
   - Retrieves default user configured SIP server information.
   - Retrieves network assigned SIP server information.

  DEPENDENCIES
    qmi_qcmap_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_qcmap_msgr_get_sip_server_info
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type              *qcmap_sp;
  qcmap_msgr_get_sip_server_info_req_msg_v01  *req_ptr;
  qcmap_msgr_get_sip_server_info_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type           *map_handle;
  boolean                                     retval;
  unsigned int                                index;
  qmi_error_type_v01                          qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_sip_server_info_v01              default_sip_server_info;
  qcmap_msgr_sip_server_info_v01
            network_sip_server_info[QCMAP_MSGR_MAX_SIP_SERVER_ENTRIES_V01];
  int                                         count_network_sip_server_info=0;
  QCMAP_Backhaul_WWAN                        *QCMAP_Backhaul_WWAN_Obj = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_get_sip_server_info_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_sip_server_info_resp_msg_v01));


  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_get_sip_server_info_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p MobileAP inst %d",
                  clnt_info, index, 0);

  QCMAP_Backhaul_WWAN_Obj = GET_BACKHAUL_WWAN_OBJ_FOR_QMI_CLIENT(clnt_info);
  CHECK_QCMAP_OBJ(QCMAP_Backhaul_WWAN_Obj, resp_msg);

  memset(&default_sip_server_info, 0, sizeof(default_sip_server_info));
  memset(network_sip_server_info, 0, sizeof(network_sip_server_info));
  if (!(retval=QCMAP_Backhaul_WWAN_Obj->GetSIPServerInfo(&default_sip_server_info,
                                          network_sip_server_info,
                                          &count_network_sip_server_info,
                                          &qmi_err_num)))
  {
    LOG_MSG_ERROR("Get SIP server failed Error: %d ",
                  qmi_err_num,
                  0, 0);

    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_get_sip_server_info_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if (default_sip_server_info.pcscf_info_type != QCMAP_MSGR_PCSCF_INFO_TYPE_MIN_ENUM_VAL_V01)
  {
    resp_msg.default_sip_server_info_valid = TRUE;
    memcpy(&(resp_msg.default_sip_server_info),
           &default_sip_server_info,
           sizeof(default_sip_server_info));
  }

  if (count_network_sip_server_info > 0)
  {
    LOG_MSG_INFO1("Found %d network assigned SIP servers", count_network_sip_server_info, 0, 0);
    resp_msg.network_sip_server_info_valid = TRUE;
    resp_msg.network_sip_server_info_len = count_network_sip_server_info;
    memcpy(resp_msg.network_sip_server_info,
           network_sip_server_info,
           count_network_sip_server_info*sizeof(qcmap_msgr_sip_server_info_v01));
  }
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_sip_server_info_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_sip_server_info() */


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_IPV6_SIP_SERVER_INFO

  DESCRIPTION
   - Retrieves network assigned IPV6 SIP server information.

  DEPENDENCIES
    qmi_qcmap_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_qcmap_msgr_get_ipv6_sip_server_info
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type                   *qcmap_sp;
  qcmap_msgr_get_ipv6_sip_server_info_req_msg_v01  *req_ptr;
  qcmap_msgr_get_ipv6_sip_server_info_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type                *map_handle;
  unsigned int                                     index;
  qmi_error_type_v01                               qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_ipv6_sip_server_info_v01
            network_v6_sip_server_info[QCMAP_MSGR_MAX_SIP_SERVER_ENTRIES_V01];
  int                                      count_network_v6_sip_server_info=0;
  QCMAP_Backhaul_WWAN                        *QCMAP_Backhaul_WWAN_Obj = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_get_ipv6_sip_server_info_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_ipv6_sip_server_info_resp_msg_v01));


  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_get_ipv6_sip_server_info_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  QCMAP_Backhaul_WWAN_Obj = GET_BACKHAUL_WWAN_OBJ_FOR_QMI_CLIENT(clnt_info);
  CHECK_QCMAP_OBJ(QCMAP_Backhaul_WWAN_Obj, resp_msg);

  LOG_MSG_INFO1 ("Client %p MobileAP inst %d",
                  clnt_info, index, 0);

  memset(network_v6_sip_server_info, 0, sizeof(network_v6_sip_server_info));
  if (!(QCMAP_Backhaul_WWAN_Obj->GetV6SIPServerInfo(network_v6_sip_server_info,
                                     &count_network_v6_sip_server_info,
                                     &qmi_err_num)))
  {
    LOG_MSG_ERROR("Get IPV6 SIP server failed Error: %d ",
                  qmi_err_num,
                  0, 0);

    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_get_ipv6_sip_server_info_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if (count_network_v6_sip_server_info > 0)
  {
    LOG_MSG_INFO1("Found %d network assigned IPV6 SIP servers",
                  count_network_v6_sip_server_info, 0, 0);
    resp_msg.network_ipv6_sip_server_info_valid = TRUE;
    resp_msg.network_ipv6_sip_server_info_len = count_network_v6_sip_server_info;
    memcpy(resp_msg.network_ipv6_sip_server_info,
           network_v6_sip_server_info,
           count_network_v6_sip_server_info*sizeof(qcmap_msgr_ipv6_sip_server_info_v01));
  }
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_ipv6_sip_server_info_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_ipv6_sip_server_info() */


/*===========================================================================
  FUNCTION  QMI_QCMAP_MSGR_RESTORE_FACTORY_CONFIG()

  DESCRIPTION
  Restore factory default configuration.

  PARAMETERS
    sp:         QMI_QCMAP MSGR's service instance state pointer for this qmi link
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
static qmi_csi_cb_error  qmi_qcmap_msgr_restore_factory_config
(
  qmi_qcmap_msgr_client_info_type    *clnt_info,
  qmi_req_handle                     req_handle,
  int                                msg_id,
  void                               *req_c_struct,
  int                                req_c_struct_len,
  void                               *service_handle
)
{
  qcmap_msgr_restore_factory_config_req_msg_v01   *req_ptr;
  qcmap_msgr_restore_factory_config_resp_msg_v01  resp_msg;
  qmi_error_type_v01                              qmi_err_num = QMI_ERR_NONE_V01;
  qmi_qcmap_msgr_softap_handle_type               *map_handle;
  unsigned int                                    index;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  LOG_MSG_INFO1 ("Client %p Restore factory default configuration", clnt_info,0,0);

  req_ptr = ( qcmap_msgr_restore_factory_config_req_msg_v01 *)req_c_struct;
  memset( &resp_msg, 0, sizeof( qcmap_msgr_restore_factory_config_resp_msg_v01 ));
  /*---------------------------------------------------------------------
    Reset to factory configuration.
   ---------------------------------------------------------------------*/
  index = req_ptr->mobile_ap_handle;
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle passed %d",req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp( req_handle, msg_id, &resp_msg,
                       sizeof(qcmap_msgr_restore_factory_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  /*Send response to client that the device will be resetting */
  qmi_csi_send_resp( req_handle, msg_id, &resp_msg,
                     sizeof(qcmap_msgr_restore_factory_config_resp_msg_v01));

  if( QCMAP_ConnectionManager::RestoreFactoryConfig( &qmi_err_num ) )
  {
    LOG_MSG_INFO1(" RestoreFactoryConfig done.\n",0,0,0);
    return QMI_CSI_CB_NO_ERR;
  }
   else
  {
    LOG_MSG_ERROR(" RestoreFactoryConfig fails.\n",0,0,0);
    return QMI_CSI_CB_NO_ERR;
  }
}

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_CONNECTED_DEVICES_INFO

  DESCRIPTION
   - Fetches information regarding the devices connected to SoftAP device

  DEPENDENCIES
    qmi_qcmap_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_qcmap_msgr_get_connected_devices_info
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type              *qcmap_sp;
  qcmap_msgr_get_connected_devices_info_req_msg_v01  *req_ptr;
  qcmap_msgr_get_connected_devices_info_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type           *map_handle;
  unsigned int                                index;
  qmi_error_type_v01                          qmi_err_num = QMI_ERR_NONE_V01;

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_get_connected_devices_info_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_connected_devices_info_resp_msg_v01));

  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_get_connected_devices_info_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p MobileAP inst %d",
                 clnt_info, index, 0);

/*---------------------------------------------------------------------
   Get Connected Device Info
  ---------------------------------------------------------------------*/

  if (QCMAP_ConnectionManager::GetConnectedDevicesInfo(resp_msg.connected_devices_info,
                                        &resp_msg.connected_devices_info_len,
                                        &qmi_err_num))
  {
    LOG_MSG_INFO1("Get Connected Devices Information succeeded.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("No Connected Devices Information available currently",0,0,0);

    resp_msg.connected_devices_info_valid = false;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_get_connected_devices_info_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  resp_msg.connected_devices_info_valid = true;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_connected_devices_info_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION qmi_qcmap_msgr_set_supplicant_config()

  DESCRIPTION
    Activate/Deactivate the WPA Supplicant config.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_set_supplicant_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type *qcmap_sp;
  qcmap_msgr_set_supplicant_config_req_msg_v01 *req_ptr;
  qcmap_msgr_set_supplicant_config_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  unsigned int index;
  qmi_qcmap_msgr_softap_handle_type *map_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_set_supplicant_config_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p set supplicant config ", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_supplicant_config_resp_msg_v01));

  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_supplicant_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  if (QCMAP_Backhaul_WLAN::SetWPASupplicantConfig(req_ptr->supplicant_config_status, &qmi_err_num))
  {
    LOG_MSG_INFO1("Station config applied successfully. \n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Station config applying failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_supplicant_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_supplicant_config_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_set_supplicant_config */

/*===========================================================================
  FUNCTION qmi_qcmap_msgr_get_cradle_mode()

  DESCRIPTION
    Retreives the usb cradle mode.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_get_cradle_mode
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qcmap_msgr_get_cradle_mode_req_msg_v01 *req_ptr;
  qcmap_msgr_get_cradle_mode_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_cradle_mode_v01 mode;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct);
  ASSERT(req_c_struct_len > 0);
  ASSERT(clnt_info);
  ASSERT(service_handle);

  req_ptr = (qcmap_msgr_get_cradle_mode_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p get cradle mode ", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_cradle_mode_resp_msg_v01));

  if (QCMAP_Backhaul_Cradle::GetCradleMode(&mode, &qmi_err_num))
  {
    LOG_MSG_INFO1("Cradle mode retreived successfully. \n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Cradle mode retreive failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_get_cradle_mode_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  resp_msg.mode_valid = true;
  resp_msg.mode = mode;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_cradle_mode_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_cradle_mode */

/*===========================================================================
  FUNCTION qmi_qcmap_msgr_set_cradle_mode()

  DESCRIPTION
    Sets the usb cradle mode.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_set_cradle_mode
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qcmap_msgr_set_cradle_mode_req_msg_v01 *req_ptr;
  qcmap_msgr_set_cradle_mode_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  unsigned int index;
  qmi_qcmap_msgr_softap_handle_type *map_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct);
  ASSERT(req_c_struct_len > 0);
  ASSERT(clnt_info);
  ASSERT(service_handle);

  req_ptr = (qcmap_msgr_set_cradle_mode_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p set cradle mode", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_cradle_mode_resp_msg_v01));

  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_cradle_mode_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if (QCMAP_Backhaul_Cradle::SetCradleMode(req_ptr->mode, map_handle, &qmi_err_num))
  {
    LOG_MSG_INFO1("Cradle mode applied successfully. \n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Cradle mode applying failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_cradle_mode_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_cradle_mode_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_set_cradle_mode */

/*===========================================================================
  FUNCTION qmi_qcmap_msgr_get_prefix_delgation_config()

  DESCRIPTION
    Retreives the prefix delegation config.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_get_prefix_delegation_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qcmap_msgr_get_prefix_delegation_config_req_msg_v01 *req_ptr;
  qcmap_msgr_get_prefix_delegation_config_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  boolean prefix_delegation;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct);
  ASSERT(req_c_struct_len > 0);
  ASSERT(clnt_info);
  ASSERT(service_handle);

  req_ptr = (qcmap_msgr_get_prefix_delegation_config_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p get prefix delegation config ", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_prefix_delegation_config_resp_msg_v01));

  if (QCMAP_Backhaul_WWAN::GetPrefixDelegationConfig(&prefix_delegation, &qmi_err_num))
  {
    LOG_MSG_INFO1("Prefix delegation mode retreived successfully. Config %d\n",prefix_delegation,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Prefix Delegation config retreive failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_get_prefix_delegation_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  resp_msg.prefix_delegation_valid = true;
  resp_msg.prefix_delegation = prefix_delegation;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_prefix_delegation_config_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_prefix_delegation_config */

/*===========================================================================
  FUNCTION qmi_qcmap_msgr_set_prefix_delegation_config()

  DESCRIPTION
    Enable/disable prefix delegation config.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_set_prefix_delegation_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qcmap_msgr_set_prefix_delegation_config_req_msg_v01 *req_ptr;
  qcmap_msgr_set_prefix_delegation_config_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  unsigned int index;
  qmi_qcmap_msgr_softap_handle_type *map_handle;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct);
  ASSERT(req_c_struct_len > 0);
  ASSERT(clnt_info);
  ASSERT(service_handle);

  req_ptr = (qcmap_msgr_set_prefix_delegation_config_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p set prefix delegation config", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_prefix_delegation_config_resp_msg_v01));

  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_prefix_delegation_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if (QCMAP_Backhaul_WWAN::SetPrefixDelegationConfig(req_ptr->prefix_delegation, &qmi_err_num))
  {
    LOG_MSG_INFO1("Prefix delegation config %d applied successfully. \n",req_ptr->prefix_delegation,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Prefix delegation config applying failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_prefix_delegation_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_prefix_delegation_config_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_set_prefix_delegation_config */

/*===========================================================================
  FUNCTION qmi_qcmap_msgr_get_prefix_delgation_status()

  DESCRIPTION
    Retreives the current prefix delegation mode.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_get_prefix_delegation_status
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qcmap_msgr_get_prefix_delegation_status_req_msg_v01 *req_ptr;
  qcmap_msgr_get_prefix_delegation_status_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  boolean prefix_delegation;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct);
  ASSERT(req_c_struct_len > 0);
  ASSERT(clnt_info);
  ASSERT(service_handle);

  req_ptr = (qcmap_msgr_get_prefix_delegation_status_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p get prefix delegation config ", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_prefix_delegation_status_resp_msg_v01));

  if (QCMAP_Backhaul_WWAN::GetPrefixDelegationStatus(&prefix_delegation, &qmi_err_num))
  {
    LOG_MSG_INFO1("Prefix delegation mode retreived successfully. Status %d\n",prefix_delegation,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Prefix Delegation config retreive failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_get_prefix_delegation_status_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  resp_msg.prefix_delegation_valid = true;
  resp_msg.prefix_delegation = prefix_delegation;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_prefix_delegation_status_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_prefix_delegation_status */


/*===========================================================================
  FUNCTION qmi_qcmap_msgr_set_gateway_url()

  DESCRIPTION
    Retreive the gateway url.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_set_gateway_url
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  QCMAP_LAN* lan_obj = NULL;
  qcmap_msgr_set_gateway_url_req_msg_v01 *set_url_req;
  qcmap_msgr_set_gateway_url_resp_msg_v01 set_url_resp;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  unsigned int index;
  qmi_qcmap_msgr_softap_handle_type *map_handle;
  QCMAP_LAN_Manager* lan_mgr = QCMAP_LAN_Manager::GetInstance();
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct);
  ASSERT(req_c_struct_len > 0);
  ASSERT(clnt_info);
  ASSERT(service_handle);

  set_url_req = (qcmap_msgr_set_gateway_url_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p Set gateway url ", clnt_info,0,0);

  memset(&set_url_resp, 0, sizeof(qcmap_msgr_set_gateway_url_resp_msg_v01));

  index = (set_url_req->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  set_url_req->mobile_ap_handle,0,0);
    set_url_resp.resp.result = QMI_RESULT_FAILURE_V01;
    set_url_resp.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &set_url_resp,
                      sizeof(qcmap_msgr_set_gateway_url_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  lan_obj = lan_mgr->GetLANBridge(clnt_info->bridge_vlan_id);

  if(lan_obj && lan_obj->SetGatewayUrl((char *)&(set_url_req->gateway_url),
                                                       set_url_req->gateway_url_len,
                                                       &qmi_err_num))
  {
    LOG_MSG_INFO1("set gateway url success \n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("set gateway url failure!!",0,0,0);
    set_url_resp.resp.result = QMI_RESULT_FAILURE_V01;
    set_url_resp.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &set_url_resp,
                      sizeof(qcmap_msgr_set_gateway_url_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &set_url_resp,
                    sizeof(qcmap_msgr_set_gateway_url_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}


/*===========================================================================
  FUNCTION qmi_qcmap_msgr_get_gateway_url()

  DESCRIPTION
    Retreive the gateway url.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_get_gateway_url
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  QCMAP_LAN* lan_obj = NULL;
  qcmap_msgr_get_gateway_url_req_msg_v01 *get_url_req;
  qcmap_msgr_get_gateway_url_resp_msg_v01 get_url_resp;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_LAN_Manager* lan_mgr = QCMAP_LAN_Manager::GetInstance();
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(req_c_struct);
  ASSERT(req_c_struct_len > 0);
  ASSERT(clnt_info);
  ASSERT(service_handle);
  ASSERT(lan_mgr);

  get_url_req = (qcmap_msgr_get_gateway_url_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p Get Gateway URL ", clnt_info,0,0);

  memset(&get_url_resp, 0, sizeof(qcmap_msgr_get_gateway_url_resp_msg_v01));

  lan_obj = lan_mgr->GetLANBridge(clnt_info->bridge_vlan_id);

  if(lan_obj && (lan_obj->GetGatewayUrl((char *)&get_url_resp.gateway_url,
                                                        &get_url_resp.gateway_url_len,
                                                        &qmi_err_num)))
  {
    LOG_MSG_INFO1("Get gateway url success \n",0,0,0);
    get_url_resp.gateway_url_len = strlen(get_url_resp.gateway_url);
    get_url_resp.gateway_url_valid = TRUE;
  }
  else
  {
    LOG_MSG_ERROR("Get gateway url failure!!",0,0,0);
    get_url_resp.resp.result = QMI_RESULT_FAILURE_V01;
    get_url_resp.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &get_url_resp,
                      sizeof(qcmap_msgr_get_gateway_url_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &get_url_resp,
                    sizeof(qcmap_msgr_get_gateway_url_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION qmi_qcmap_msgr_enable_dynamic_dns()

  DESCRIPTION
   Enables ddns support.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_enable_dynamic_dns
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type       *qcmap_sp;
  qcmap_msgr_enable_dynamic_dns_req_msg_v01 *req_ptr;
  qcmap_msgr_enable_dynamic_dns_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type          *map_handle;
  boolean            retval;
  unsigned int       index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_enable_dynamic_dns_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_enable_dynamic_dns_resp_msg_v01));

  /*---------------------------------------------------------------------
      Enable DDNS
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_enable_dynamic_dns_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p Enabling DDNS in MobileAP inst %d", clnt_info, index,0);

  if (!(retval=QCMAP_Backhaul_WWAN::EnableDDNS(&qmi_err_num)))
  {
    LOG_MSG_ERROR("Enabling DDNS failed",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_enable_dynamic_dns_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_enable_dynamic_dns_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}


/*===========================================================================
  FUNCTION qmi_qcmap_msgr_disable_dynamic_dns()

  DESCRIPTION
   Disable ddns support.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_disable_dynamic_dns
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type       *qcmap_sp;
  qcmap_msgr_disable_dynamic_dns_req_msg_v01 *req_ptr;
  qcmap_msgr_disable_dynamic_dns_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type          *map_handle;
  boolean            retval;
  unsigned int       index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_disable_dynamic_dns_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_disable_dynamic_dns_resp_msg_v01));

  /*---------------------------------------------------------------------
      Disable DDNS
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_disable_dynamic_dns_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p Disabling DDNS in MobileAP inst %d", clnt_info, index,0);

  if (!(retval=QCMAP_Backhaul_WWAN::DisableDDNS(&qmi_err_num)))
  {
    LOG_MSG_ERROR("Disabling DDNS failed",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_disable_dynamic_dns_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_disable_dynamic_dns_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}



/*===========================================================================
  FUNCTION qmi_qcmap_msgr_set_dynamic_dns_config()

  DESCRIPTION
   Set ddns support.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_set_dynamic_dns_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type       *qcmap_sp;
  qcmap_msgr_set_dynamic_dns_config_req_msg_v01 *req_ptr;
  qcmap_msgr_set_dynamic_dns_config_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type          *map_handle;
  boolean            retval;
  unsigned int       index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_set_dynamic_dns_config_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_dynamic_dns_config_resp_msg_v01));

  /*---------------------------------------------------------------------
      Set DDNS
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_dynamic_dns_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p Set DDNS in MobileAP inst %d", clnt_info, index,0);

  if (!(retval=QCMAP_Backhaul_WWAN::SetDDNS(*req_ptr,&qmi_err_num)))
  {
    LOG_MSG_ERROR("Set DDNS failed",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_dynamic_dns_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_dynamic_dns_config_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}


/*===========================================================================
  FUNCTION qmi_qcmap_msgr_get_dynamic_dns_config()

  DESCRIPTION
   Get ddns support.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_get_dynamic_dns_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type       *qcmap_sp;
  qcmap_msgr_get_dynamic_dns_config_resp_msg_v01 resp_msg;
  boolean            retval;
  unsigned int       index = 0;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01));

  /*---------------------------------------------------------------------
      Set DDNS
  ---------------------------------------------------------------------*/

  LOG_MSG_INFO1 ("Client %p Get DDNS  MobileAP inst %d", clnt_info, index,0);

  if (!(retval=QCMAP_Backhaul_WWAN::GetDDNS(&resp_msg,&qmi_err_num)))
  {
    LOG_MSG_ERROR("Get DDNS failed",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION qmi_qcmap_msgr_enable_socksv5_proxy()

  DESCRIPTION
   Enables socksv5 proxy.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_enable_socksv5_proxy
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type       *qcmap_sp;
  qcmap_msgr_enable_socksv5_proxy_req_msg_v01 *req_ptr;
  qcmap_msgr_enable_socksv5_proxy_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type          *map_handle;
  boolean            retval;
  int16              ps_errno;
  int                err_num=0;
  unsigned int       index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_enable_socksv5_proxy_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_enable_socksv5_proxy_resp_msg_v01));

  /*---------------------------------------------------------------------
      Enable SOCKSv5 Proxy
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_enable_socksv5_proxy_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p Enabling SOCKSv5 Proxy in MobileAP inst %d", clnt_info, index,0);

  if (!(retval=QCMAP_NATALG::EnableSOCKSv5Proxy(&qmi_err_num)))
  {
    LOG_MSG_ERROR("Enabling SOCKSv5 Proxy failed",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_enable_socksv5_proxy_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_enable_socksv5_proxy_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION qmi_qcmap_msgr_disable_socksv5_proxy()

  DESCRIPTION
   Disable SOCKSv5 Proxy.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_disable_socksv5_proxy
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type       *qcmap_sp;
  qcmap_msgr_disable_socksv5_proxy_req_msg_v01 *req_ptr;
  qcmap_msgr_disable_socksv5_proxy_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type          *map_handle;
  boolean            retval;
  int16              ps_errno;
  int                err_num=0;
  unsigned int       index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_disable_socksv5_proxy_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_disable_socksv5_proxy_resp_msg_v01));

  /*---------------------------------------------------------------------
      Disable SOCKSv5 Proxy
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_disable_socksv5_proxy_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p Disabling SOCKSv5 Proxy in MobileAP inst %d", clnt_info, index,0);

  if (!(retval=QCMAP_NATALG::DisableSOCKSv5Proxy(&qmi_err_num)))
  {
    LOG_MSG_ERROR("Disabling SOCKSv5 Proxy failed",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_disable_socksv5_proxy_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_disable_socksv5_proxy_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION qmi_qcmap_msgr_get_socksv5_proxy_config()

  DESCRIPTION
   Get SOCKSv5 Proxy Config

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_get_socksv5_proxy_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type       *qcmap_sp;
  qcmap_msgr_get_socksv5_proxy_config_req_msg_v01 *req_ptr;
  qcmap_msgr_get_socksv5_proxy_config_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type          *map_handle;
  boolean            retval;
  int16              ps_errno;
  int                err_num=0;
  unsigned int       index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_get_socksv5_proxy_config_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_socksv5_proxy_config_resp_msg_v01));

  /*---------------------------------------------------------------------
      Get SOCKSv5 Proxy Config
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_get_socksv5_proxy_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p Getting SOCKSv5 Config in MobileAP inst %d", clnt_info, index,0);

  if (!(retval = QC_SOCKSv5_Configure::GetSOCKSv5ProxyConfig(&qmi_err_num, &resp_msg)))
  {
    LOG_MSG_ERROR("Getting SOCKSv5 Config failed",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_get_socksv5_proxy_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_socksv5_proxy_config_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION qmi_qcmap_msgr_set_socksv5_proxy_config()

  DESCRIPTION
   Set SOCKSv5 Proxy Auth Config.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_set_socksv5_proxy_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type       *qcmap_sp;
  qcmap_msgr_set_socksv5_proxy_config_req_msg_v01 *req_ptr;
  qcmap_msgr_set_socksv5_proxy_config_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type          *map_handle;
  boolean            retval;
  int16              ps_errno;
  int                err_num=0;
  unsigned int       index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_set_socksv5_proxy_config_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_socksv5_proxy_config_resp_msg_v01));

  /*---------------------------------------------------------------------
      Set SOCKSv5 Proxy Config
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_socksv5_proxy_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1("Client %p Setting SOCKSv5 Config in MobileAP inst %d",clnt_info,index,0);

  switch(req_ptr->config_type)
  {
    case QCMAP_MSGR_SOCKSV5_SET_CONFIG_FILE_PATH_V01:
    {
      if(!req_ptr->config_file_paths_valid || !(retval = QCMAP_NATALG::SetSOCKSv5ConfigFilePath(
                                                req_ptr->config_file_paths.conf_file,
                                                req_ptr->config_file_paths.auth_file)))
      {
        LOG_MSG_ERROR("Setting SOCKSv5 Config File Path failed",0,0,0);
        resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
        resp_msg.resp.error = qmi_err_num;
        qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                          sizeof(qcmap_msgr_set_socksv5_proxy_config_resp_msg_v01));
        return QMI_CSI_CB_NO_ERR;
      }
      break;
    }
    case QCMAP_MSGR_SOCKSV5_SET_AUTH_METHOD_V01:
    {
      if(!req_ptr->auth_method_valid || !(retval = QC_SOCKSv5_Configure::SetSOCKSv5ProxyAuth(
                                          &qmi_err_num, req_ptr->auth_method)))
      {
        LOG_MSG_ERROR("Setting SOCKSv5 Auth failed",0,0,0);
        resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
        resp_msg.resp.error = qmi_err_num;
        qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                          sizeof(qcmap_msgr_set_socksv5_proxy_config_resp_msg_v01));
        return QMI_CSI_CB_NO_ERR;
      }
      break;
    }
    case QCMAP_MSGR_SOCKSV5_EDIT_LAN_IFACE_V01:
    {
      if(!req_ptr->lan_iface_valid || !(retval = QC_SOCKSv5_Configure::EditSOCKSv5ProxyLANIface(
                                        &qmi_err_num, req_ptr->lan_iface)))
      {
        LOG_MSG_ERROR("Setting SOCKSv5 LAN Iface failed",0,0,0);
        resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
        resp_msg.resp.error = qmi_err_num;
        qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                          sizeof(qcmap_msgr_set_socksv5_proxy_config_resp_msg_v01));
        return QMI_CSI_CB_NO_ERR;
      }
      break;
    }
    case QCMAP_MSGR_SOCKSV5_ADD_UNAME_ASSOC_V01:
    {
      if(!req_ptr->uname_assoc_valid || !(retval = QC_SOCKSv5_Configure::AddSOCKSv5ProxyUnameAssoc(
                                         &qmi_err_num, req_ptr->uname_assoc.uname,
                                         req_ptr->uname_assoc.service_no)))
      {
        LOG_MSG_ERROR("Adding SOCKSv5 Uname Assoc failed",0,0,0);
        resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
        resp_msg.resp.error = qmi_err_num;
        qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                          sizeof(qcmap_msgr_set_socksv5_proxy_config_resp_msg_v01));
        return QMI_CSI_CB_NO_ERR;
      }
      break;
    }
    case QCMAP_MSGR_SOCKSV5_DELETE_UNAME_ASSOC_V01:
    {
      if(!req_ptr->uname_assoc_valid || !(retval=QC_SOCKSv5_Configure::DeleteSOCKSv5ProxyUnameAssoc
                                                 (&qmi_err_num, req_ptr->uname_assoc.uname)))
      {
        LOG_MSG_ERROR("Deleting SOCKSv5 Uname Assoc failed",0,0,0);
        resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
        resp_msg.resp.error = qmi_err_num;
        qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                          sizeof(qcmap_msgr_set_socksv5_proxy_config_resp_msg_v01));
        return QMI_CSI_CB_NO_ERR;
      }
      break;
    }
    default:
    {
      LOG_MSG_INFO1("Invalid SetSOCKSv5Config type %d", req_ptr->config_type, 0, 0);
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      resp_msg.resp.error = qmi_err_num;
      break;
    }
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_socksv5_proxy_config_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION qmi_qcmap_msgr_get_tiny_proxy_status()

  DESCRIPTION
    Returns the status of Tiny Proxy.
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
static qmi_csi_cb_error  qmi_qcmap_msgr_get_tiny_proxy_status
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qcmap_msgr_get_tiny_proxy_status_resp_msg_v01   resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_tiny_proxy_mode_enum_v01 status = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  LOG_MSG_INFO1 ("Client %p returning Tiny Proxy status", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_tiny_proxy_status_resp_msg_v01));

  /*---------------------------------------------------------------------
    Get Tiny Proxy Status
  ---------------------------------------------------------------------*/

  if (QCMAP_Backhaul_WWAN::GetTinyProxyStatus(&status, &qmi_err_num))
  {
    resp_msg.tiny_proxy_mode_valid= true;
    resp_msg.tiny_proxy_mode = status;
    LOG_MSG_ERROR("status=%d,resp status=%d",status,resp_msg.tiny_proxy_mode,0);
  }
  else
  {
    LOG_MSG_ERROR("Get Tiny Proxy status fails.\n",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_tiny_proxy_status_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_tiny_proxy_status_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}




/*===========================================================================
  FUNCTION qmi_qcmap_msgr_enable_tiny_proxy()

  DESCRIPTION
   Enables tiny proxy.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_enable_tiny_proxy
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type       *qcmap_sp;
  qcmap_msgr_enable_tiny_proxy_req_msg_v01 *req_ptr;
  qcmap_msgr_enable_tiny_proxy_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type          *map_handle;
  boolean            retval;
  unsigned int       index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_enable_tiny_proxy_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_enable_tiny_proxy_resp_msg_v01));

  /*---------------------------------------------------------------------
      Enable Tiny Proxy
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_enable_tiny_proxy_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p Enabling Tiny Proxy in MobileAP inst %d", clnt_info, index,0);

  if (!(retval=QCMAP_Backhaul_WWAN::EnableTinyProxy(&qmi_err_num)))
  {
    LOG_MSG_ERROR("Enabling Tiny Proxy failed",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_enable_tiny_proxy_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_enable_tiny_proxy_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION qmi_qcmap_msgr_disable_tiny_proxy()

  DESCRIPTION
   Disable ddns support.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_disable_tiny_proxy
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type       *qcmap_sp;
  qcmap_msgr_disable_tiny_proxy_req_msg_v01 *req_ptr;
  qcmap_msgr_disable_tiny_proxy_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type          *map_handle;
  boolean            retval;
  unsigned int       index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_disable_tiny_proxy_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_disable_tiny_proxy_resp_msg_v01));

  /*---------------------------------------------------------------------
      Disable Tiny Proxy
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_disable_tiny_proxy_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p Disabling Tiny Proxy in MobileAP inst %d", clnt_info, index,0);

  if (!(retval=QCMAP_Backhaul_WWAN::DisableTinyProxy(&qmi_err_num)))
  {
    LOG_MSG_ERROR("Disabling DDNS failed",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_disable_tiny_proxy_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_disable_tiny_proxy_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_DLNAWhitelisting()

  DESCRIPTION
    Get DLNA WHitelisting state.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_qcmap_msgr_get_dlna_whitelisting
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_get_dlna_whitelisting_req_msg_v01 *req_ptr;
  qcmap_msgr_get_dlna_whitelisting_resp_msg_v01 resp_msg;
  int dlna_whitelist_allow = 0;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_dlna_whitelisting_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p get dlna whitelist ip", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_dlna_whitelisting_resp_msg_v01));

  /*---------------------------------------------------------------------
   Get DLNA Whitelisting
  ---------------------------------------------------------------------*/

  if (QCMAP_MediaService::GetDLNAWhitelisting(&dlna_whitelist_allow, &qmi_err_num))
  {
    LOG_MSG_INFO1("Get DLNA Whitelist succeeded.\n",0,0,0);

    resp_msg.dlna_whitelist_allow_valid= TRUE;
    resp_msg.dlna_whitelist_allow= dlna_whitelist_allow;
  }
  else
  {
    LOG_MSG_ERROR("Get DLNA Whitelist failed!!",0,0,0);
    resp_msg.dlna_whitelist_allow_valid= false;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_dlna_whitelisting_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  resp_msg.dlna_whitelist_allow_valid = true;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_dlna_whitelisting_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_dmz() */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_SET_DLNAWhitelisting()

  DESCRIPTION
    Set DLNA WHitelisting state.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_qcmap_msgr_set_dlna_whitelisting
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_set_dlna_whitelisting_req_msg_v01 *req_ptr;
  qcmap_msgr_set_dlna_whitelisting_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  int dlna_whitelist_allow;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_set_dlna_whitelisting_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p set dlna whitelist ip", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_dlna_whitelisting_resp_msg_v01));
  dlna_whitelist_allow = req_ptr->dlna_whitelist_allow;

  LOG_MSG_INFO1 ("set dlna whitelist ip state", dlna_whitelist_allow,0,0);
  if (QCMAP_MediaService::SetDLNAWhitelisting( dlna_whitelist_allow,&qmi_err_num))
  {
    LOG_MSG_INFO1("Set DLNA Whitelisting IP successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Set DLNA Whitelisting failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_dlna_whitelisting_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_dlna_whitelisting_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

}


/*===========================================================================
  FUNCTION qmi_qcmap_msgr_add_dlna_whitelist_ip()

  DESCRIPTION
    Add DLNA WHitelisting IP.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_qcmap_msgr_add_dlna_whitelist_ip
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_add_dlna_whitelistip_req_msg_v01 *req_ptr;
  qcmap_msgr_add_dlna_whitelistip_resp_msg_v01 resp_msg;
  uint32 dlna_whitelist_ip=0;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;

  unsigned int     index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_add_dlna_whitelistip_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p dmz", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_add_dlna_whitelistip_resp_msg_v01));

  /*---------------------------------------------------------------------
   Add DLNA Whitelist IP
  ---------------------------------------------------------------------*/

 index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_dmz_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  dlna_whitelist_ip = req_ptr->dlna_whitelist_ip_addr;

  if (QCMAP_MediaService::AddDLNAWhitelistConnection(dlna_whitelist_ip, &qmi_err_num))
  {
    LOG_MSG_INFO1("Added DLNA Whitelist IP successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("DLNA Whitelist IP add failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_add_dlna_whitelistip_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_add_dlna_whitelistip_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_set_dmz() */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_DELETE_DLNA_WHITELIST_IP

  DESCRIPTION
    Delete DLNA WHitelisting state.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_qcmap_msgr_delete_dlna_whitelist_ip
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_delete_dlna_whitelist_ip_req_msg_v01 *req_ptr;
  qcmap_msgr_delete_dlna_whitelist_ip_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  uint32 dlna_whitelist_ip=0;

  unsigned int     index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_delete_dlna_whitelist_ip_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p deleting dlna whitelist ip", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_delete_dmz_resp_msg_v01));

  /*---------------------------------------------------------------------
   Delete DMZ
  ---------------------------------------------------------------------*/

 index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_delete_dmz_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  dlna_whitelist_ip = req_ptr->dlna_whitelist_ip_addr;

  if (QCMAP_MediaService::DeleteDLNAWhitelistingIPEntry(dlna_whitelist_ip,&qmi_err_num))
  {
    LOG_MSG_INFO1("Deleted DLNA Whitelisting IP successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("DLNA Whitelisting IP delete failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_delete_dlna_whitelist_ip_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_delete_dlna_whitelist_ip_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION qmi_qcmap_enable_sta_mode()

  DESCRIPTION
   Set ddns support.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_enable_sta_mode
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type          *qcmap_sp;
  qcmap_msgr_enable_sta_mode_req_msg_v01  *req_ptr;
  qcmap_msgr_enable_sta_mode_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type       *map_handle;
  boolean            retval;
  unsigned int       index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_enable_sta_mode_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_enable_sta_mode_resp_msg_v01));

  /*---------------------------------------------------------------------
      Enable STA Only Mode
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_enable_sta_mode_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p Enabling STA Mode in MobileAP inst %d", clnt_info, index,0);

  if (!(retval = QCMAP_WLAN::EnableSTAOnlyMode(&qmi_err_num)))
  {
    LOG_MSG_ERROR("Enabling STA Only Mode Failed",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_enable_sta_mode_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_enable_sta_mode_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION qmi_qcmap_disable_sta_mode()

  DESCRIPTION
   Set ddns support.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_disable_sta_mode
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type          *qcmap_sp;
  qcmap_msgr_disable_sta_mode_req_msg_v01  *req_ptr;
  qcmap_msgr_disable_sta_mode_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type       *map_handle;
  boolean            retval;
  unsigned int       index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_disable_sta_mode_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_disable_sta_mode_resp_msg_v01));

  /*---------------------------------------------------------------------
      Disable STA Only Mode
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_disable_sta_mode_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p Disabling STA Mode in MobileAP inst %d", clnt_info, index,0);

  if (!(retval = QCMAP_WLAN::DisableSTAOnlyMode(&qmi_err_num)))
  {
    LOG_MSG_ERROR("Disabling STA Only Mode Failed",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_disable_sta_mode_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_disable_sta_mode_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION qmi_qcmap_wlan_status_ind_reg()

  DESCRIPTION
   Set ddns support.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_wlan_status_ind_reg
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type                   *qcmap_sp;
  qcmap_msgr_wlan_status_ind_register_req_msg_v01  *req_ptr;
  qcmap_msgr_wlan_status_ind_register_resp_msg_v01 resp_msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_wlan_status_ind_register_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_wlan_status_ind_register_resp_msg_v01));

  /*---------------------------------------------------------------------
    Set WLAN Status Indication
  ---------------------------------------------------------------------*/
  clnt_info->wlan_status_ind_regd = req_ptr->register_indication;
  LOG_MSG_INFO1 ("Client %p, register_indication %d for WLAN Mode status", clnt_info, clnt_info->wlan_status_ind_regd, 0);
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, sizeof(qcmap_msgr_station_mode_status_ind_register_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}
/*===========================================================================
  FUNCTION qmi_qcmap_msgr_set_backhaul_priority()

  DESCRIPTION
    Sets the backhaul pref

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_set_backhaul_priority
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qcmap_msgr_set_backhaul_pref_req_msg_v01 *req_ptr;
  qcmap_msgr_set_backhaul_pref_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  unsigned int index;
  bool error = false;
  qmi_qcmap_msgr_softap_handle_type *map_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct);
  ASSERT(req_c_struct_len > 0);
  ASSERT(clnt_info);
  ASSERT(service_handle);

  req_ptr = (qcmap_msgr_set_backhaul_pref_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p set backhaul pref", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_backhaul_pref_resp_msg_v01));

  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0 ||
     req_ptr->first_priority_backhaul == req_ptr->second_priority_backhaul ||
     req_ptr->second_priority_backhaul == req_ptr->third_priority_backhaul ||
     req_ptr->third_priority_backhaul == req_ptr->first_priority_backhaul ||
     req_ptr->fourth_priority_backhaul == req_ptr->first_priority_backhaul ||
     req_ptr->fourth_priority_backhaul == req_ptr->second_priority_backhaul ||
     req_ptr->fourth_priority_backhaul == req_ptr->third_priority_backhaul ||
     req_ptr->fifth_priority_backhaul == req_ptr->fourth_priority_backhaul ||
     req_ptr->fifth_priority_backhaul == req_ptr->third_priority_backhaul ||
     req_ptr->fifth_priority_backhaul == req_ptr->second_priority_backhaul ||
     req_ptr->fifth_priority_backhaul == req_ptr->first_priority_backhaul)
  {
    LOG_MSG_INFO1("Incorrect handle %d or incorrect params passed",
                  req_ptr->mobile_ap_handle,0,0);
    LOG_MSG_INFO1("First Backhaul = %d, Second Backhaul = %d, Third Backhaul =%d",
                  req_ptr->first_priority_backhaul,
                  req_ptr->second_priority_backhaul,
                  req_ptr->third_priority_backhaul);
    LOG_MSG_INFO1("Fourth Backhaul = %d",
                  req_ptr->fourth_priority_backhaul,
                  0,0);

    LOG_MSG_INFO1("Fifth Backhaul = %d",
                  req_ptr->fifth_priority_backhaul,
                  0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_backhaul_pref_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if (!QCMAP_Backhaul::SetBackhaulPrioConfigToXML(
                             req_ptr->first_priority_backhaul,
                             CONFIG_FIRST_PREFFERED_BACKHAUL))
  {
    LOG_MSG_ERROR("Error set in first backhaul %d\n!!",
                  req_ptr->first_priority_backhaul,0,0);
    error = true;
  }

  if (!QCMAP_Backhaul::SetBackhaulPrioConfigToXML(
                             req_ptr->second_priority_backhaul,
                             CONFIG_SECOND_PREFFERED_BACKHAUL))
  {
    LOG_MSG_ERROR("Error set in second backhaul %d\n!!",
                  req_ptr->second_priority_backhaul,0,0);
    error = true;
  }

  if (!QCMAP_Backhaul::SetBackhaulPrioConfigToXML(
                             req_ptr->third_priority_backhaul,
                             CONFIG_THIRD_PREFFERED_BACKHAUL))
  {
    LOG_MSG_ERROR("Error set in third bakhaul %d\n!!",
                  req_ptr->third_priority_backhaul,0,0);
    error = true;
  }

  if (!QCMAP_Backhaul::SetBackhaulPrioConfigToXML(
                             req_ptr->fourth_priority_backhaul,
                             CONFIG_FOURTH_PREFFERED_BACKHAUL))
  {
    LOG_MSG_ERROR("Error set in fourth backhaul %d\n!!",
                  req_ptr->fourth_priority_backhaul,0,0);
    error = true;
  }

  if (!QCMAP_Backhaul::SetBackhaulPrioConfigToXML(
                             req_ptr->fifth_priority_backhaul,
                             CONFIG_FIFTH_PREFFERED_BACKHAUL))
  {
    LOG_MSG_ERROR("Error set in fifth backhaul %d\n!!",
                  req_ptr->fifth_priority_backhaul,0,0);
    error = true;
  }

  if (error)
  {
    LOG_MSG_ERROR("Set Backhaul Config Failed..!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_backhaul_pref_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_backhaul_pref_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}/* qmi_qcmap_msgr_set_backhaul_pref */

/*===========================================================================
  FUNCTION qmi_qcmap_msgr_get_backhaul_priority()

  DESCRIPTION
    Sets the backhaul pref

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_get_backhaul_priority
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qcmap_msgr_get_backhaul_pref_req_msg_v01 *req_ptr;
  qcmap_msgr_get_backhaul_pref_resp_msg_v01 resp_msg;
  char *ptr;
  char data[MAX_STRING_LENGTH] = {0};
  char *token=NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct);
  ASSERT(req_c_struct_len > 0);
  ASSERT(clnt_info);
  ASSERT(service_handle);

  req_ptr = (qcmap_msgr_get_backhaul_pref_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p get backhaul pref", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_backhaul_pref_resp_msg_v01));

  QCMAP_Backhaul::GetSetBackhaulConfigFromXML(CONFIG_FIRST_PREFFERED_BACKHAUL,GET_VALUE, data, MAX_STRING_LENGTH);
  token = strtok_r(data, "\n",&ptr);
  if(token)
  {
    if (strncmp(token,WWAN_BACKHAUL,strlen(WWAN_BACKHAUL)) == 0)
      resp_msg.first_priority_backhaul = QCMAP_MSGR_WWAN_BACKHAUL_V01;
    else if (strncmp(token,USB_CRADLE_BACKHAUL,strlen(USB_CRADLE_BACKHAUL)) == 0)
      resp_msg.first_priority_backhaul = QCMAP_MSGR_USB_CRADLE_BACKHAUL_V01;
    else if (strncmp(token,WLAN_BACKHAUL,strlen(WLAN_BACKHAUL)) == 0)
      resp_msg.first_priority_backhaul = QCMAP_MSGR_WLAN_BACKHAUL_V01;
    else if (strncmp(token,ETH_BACKHAUL,strlen(ETH_BACKHAUL)) == 0)
      resp_msg.first_priority_backhaul = QCMAP_MSGR_ETHERNET_BACKHAUL_V01;
    else if (strncmp(token,BT_BACKHAUL,strlen(BT_BACKHAUL)) == 0)
      resp_msg.first_priority_backhaul = QCMAP_MSGR_BT_BACKHAUL_V01;
  }
  QCMAP_Backhaul::GetSetBackhaulConfigFromXML(CONFIG_SECOND_PREFFERED_BACKHAUL,GET_VALUE, data, MAX_STRING_LENGTH);
  token = strtok_r(data, "\n",&ptr);
  if(token)
  {
    if (strncmp(token,WWAN_BACKHAUL,strlen(WWAN_BACKHAUL)) == 0)
      resp_msg.second_priority_backhaul = QCMAP_MSGR_WWAN_BACKHAUL_V01;
    else if (strncmp(token,USB_CRADLE_BACKHAUL,strlen(USB_CRADLE_BACKHAUL)) == 0)
      resp_msg.second_priority_backhaul = QCMAP_MSGR_USB_CRADLE_BACKHAUL_V01;
    else if (strncmp(token,WLAN_BACKHAUL,strlen(WLAN_BACKHAUL)) == 0)
      resp_msg.second_priority_backhaul = QCMAP_MSGR_WLAN_BACKHAUL_V01;
    else if (strncmp(token,ETH_BACKHAUL,strlen(ETH_BACKHAUL)) == 0)
      resp_msg.second_priority_backhaul = QCMAP_MSGR_ETHERNET_BACKHAUL_V01;
    else if (strncmp(token,BT_BACKHAUL,strlen(BT_BACKHAUL)) == 0)
      resp_msg.second_priority_backhaul = QCMAP_MSGR_BT_BACKHAUL_V01;
  }
  QCMAP_Backhaul::GetSetBackhaulConfigFromXML(CONFIG_THIRD_PREFFERED_BACKHAUL,GET_VALUE, data, MAX_STRING_LENGTH);
  token = strtok_r(data, "\n",&ptr);
  if(token)
  {
    if (strncmp(token,WWAN_BACKHAUL,strlen(WWAN_BACKHAUL)) == 0)
      resp_msg.third_priority_backhaul = QCMAP_MSGR_WWAN_BACKHAUL_V01;
    else if (strncmp(token,USB_CRADLE_BACKHAUL,strlen(USB_CRADLE_BACKHAUL)) == 0)
      resp_msg.third_priority_backhaul = QCMAP_MSGR_USB_CRADLE_BACKHAUL_V01;
    else if (strncmp(token,WLAN_BACKHAUL,strlen(WLAN_BACKHAUL)) == 0)
      resp_msg.third_priority_backhaul = QCMAP_MSGR_WLAN_BACKHAUL_V01;
    else if (strncmp(token,ETH_BACKHAUL,strlen(ETH_BACKHAUL)) == 0)
      resp_msg.third_priority_backhaul = QCMAP_MSGR_ETHERNET_BACKHAUL_V01;
    else if (strncmp(token,BT_BACKHAUL,strlen(BT_BACKHAUL)) == 0)
      resp_msg.third_priority_backhaul = QCMAP_MSGR_BT_BACKHAUL_V01;
  }
  QCMAP_Backhaul::GetSetBackhaulConfigFromXML(CONFIG_FOURTH_PREFFERED_BACKHAUL,GET_VALUE, data, MAX_STRING_LENGTH);
  token = strtok_r(data, "\n",&ptr);
  if(token)
  {
    if (strncmp(token,WWAN_BACKHAUL,strlen(WWAN_BACKHAUL)) == 0)
      resp_msg.fourth_priority_backhaul = QCMAP_MSGR_WWAN_BACKHAUL_V01;
    else if (strncmp(token,USB_CRADLE_BACKHAUL,strlen(USB_CRADLE_BACKHAUL)) == 0)
      resp_msg.fourth_priority_backhaul = QCMAP_MSGR_USB_CRADLE_BACKHAUL_V01;
    else if (strncmp(token,WLAN_BACKHAUL,strlen(WLAN_BACKHAUL)) == 0)
      resp_msg.fourth_priority_backhaul = QCMAP_MSGR_WLAN_BACKHAUL_V01;
    else if (strncmp(token,ETH_BACKHAUL,strlen(ETH_BACKHAUL)) == 0)
      resp_msg.fourth_priority_backhaul = QCMAP_MSGR_ETHERNET_BACKHAUL_V01;
    else if (strncmp(token,BT_BACKHAUL,strlen(BT_BACKHAUL)) == 0)
      resp_msg.fourth_priority_backhaul = QCMAP_MSGR_BT_BACKHAUL_V01;
  }

  QCMAP_Backhaul::GetSetBackhaulConfigFromXML(CONFIG_FIFTH_PREFFERED_BACKHAUL,GET_VALUE, data, MAX_STRING_LENGTH);
  token = strtok_r(data, "\n",&ptr);
  if(token)
  {
    if (strncmp(token,WWAN_BACKHAUL,strlen(WWAN_BACKHAUL)) == 0)
      resp_msg.fifth_priority_backhaul = QCMAP_MSGR_WWAN_BACKHAUL_V01;
    else if (strncmp(token,USB_CRADLE_BACKHAUL,strlen(USB_CRADLE_BACKHAUL)) == 0)
      resp_msg.fifth_priority_backhaul = QCMAP_MSGR_USB_CRADLE_BACKHAUL_V01;
    else if (strncmp(token,WLAN_BACKHAUL,strlen(WLAN_BACKHAUL)) == 0)
      resp_msg.fifth_priority_backhaul = QCMAP_MSGR_WLAN_BACKHAUL_V01;
    else if (strncmp(token,ETH_BACKHAUL,strlen(ETH_BACKHAUL)) == 0)
      resp_msg.fifth_priority_backhaul = QCMAP_MSGR_ETHERNET_BACKHAUL_V01;
    else if (strncmp(token,BT_BACKHAUL,strlen(BT_BACKHAUL)) == 0)
      resp_msg.fifth_priority_backhaul = QCMAP_MSGR_BT_BACKHAUL_V01;
  }

  resp_msg.first_priority_backhaul_valid = true;
  resp_msg.second_priority_backhaul_valid = true;
  resp_msg.third_priority_backhaul_valid = true;
  resp_msg.fourth_priority_backhaul_valid = true;
  resp_msg.fifth_priority_backhaul_valid = true;

  LOG_MSG_INFO1("Get Current Backhaul Pref successful",0,0,0);

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_backhaul_pref_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}/* qmi_qcmap_msgr_get_backhaul_pref */

/*===========================================================================
  FUNCTION qmi_qcmap_msgr_get_ethernet_mode()

  DESCRIPTION
    Retreives the ETHERNET mode.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_get_ethernet_mode
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qcmap_msgr_get_ethernet_mode_req_msg_v01 *req_ptr;
  qcmap_msgr_get_ethernet_mode_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_ethernet_mode_v01 mode;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct);
  ASSERT(req_c_struct_len > 0);
  ASSERT(clnt_info);
  ASSERT(service_handle);

  req_ptr = (qcmap_msgr_get_ethernet_mode_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p get ETHERNET mode ", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_ethernet_mode_resp_msg_v01));

  if (QCMAP_Backhaul_Ethernet::GetEthBackhaulMode(&mode, &qmi_err_num))
  {
    LOG_MSG_INFO1("Ethernet mode retreived successfully. \n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Ethernet mode retreive failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_get_ethernet_mode_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  resp_msg.mode_valid = true;
  resp_msg.mode = mode;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_ethernet_mode_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_ethernet_mode */

/*===========================================================================
  FUNCTION qmi_qcmap_msgr_set_ethernet_mode()

  DESCRIPTION
    Sets the Ethernet mode.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_set_ethernet_mode
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qcmap_msgr_set_ethernet_mode_req_msg_v01 *req_ptr;
  qcmap_msgr_set_ethernet_mode_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  unsigned int index;
  qmi_qcmap_msgr_softap_handle_type *map_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct);
  ASSERT(req_c_struct_len > 0);
  ASSERT(clnt_info);
  ASSERT(service_handle);

  req_ptr = (qcmap_msgr_set_ethernet_mode_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p set eth backhaul mode", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_ethernet_mode_resp_msg_v01));

  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_ethernet_mode_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if (QCMAP_Backhaul_Ethernet::SetEthBackhaulMode(
                                   req_ptr->mode, map_handle, &qmi_err_num))
  {
    LOG_MSG_INFO1("Ethernet mode applied successfully. \n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("ETHERNET mode applying failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_ethernet_mode_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_ethernet_mode_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_set_ethernet_mode */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_SET_IP_PASSTHROUGH_CONFIG()

  DESCRIPTION
    Set IP Passthrough Config.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_qcmap_msgr_set_ip_passthrough_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qcmap_msgr_set_ip_passthrough_flag_req_msg_v01*req_ptr;
  qcmap_msgr_set_ip_passthrough_flag_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_ip_passthrough_flag_resp_msg_v01));

  QCMAP_LAN *QcMapLANMgr =GET_DEFAULT_LAN_OBJECT();
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_set_ip_passthrough_flag_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p enable IP Passthrough", clnt_info,0,0);
  CHECK_QCMAP_OBJ(QcMapLANMgr, resp_msg);

/*---------------------------------------------------------------------
   Set IP Passthrough Flag
  ---------------------------------------------------------------------*/

  if (QcMapLANMgr->SetIPPassthroughConfig(req_ptr->enable_state,
      req_ptr->passthrough_config_valid,&req_ptr->passthrough_config, &qmi_err_num))
  {
    LOG_MSG_INFO1("Set IP Passthrough succeeded.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Enable IP Passthrough failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_ip_passthrough_flag_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_ip_passthrough_flag_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_set_ip_passthrough_flag() */


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_IP_PASSTHROUGH_CONFIG()

  DESCRIPTION
    Get IP Passthrough Config.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_qcmap_msgr_get_ip_passthrough_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qcmap_msgr_get_ip_passthrough_flag_req_msg_v01 *req_ptr;
  qcmap_msgr_get_ip_passthrough_flag_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_LAN *QcMapLANMgr =GET_DEFAULT_LAN_OBJECT();
  qcmap_msgr_ip_passthrough_mode_enum_v01 enable_state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_ip_passthrough_flag_resp_msg_v01));

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_ip_passthrough_flag_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("\nClient %p get ip passthrough state", clnt_info,0,0);
  /*---------------------------------------------------------------------
   Get IP Passthrough Flag
  ---------------------------------------------------------------------*/

  if (QcMapLANMgr->GetIPPassthroughConfig
      (&enable_state,&resp_msg.passthrough_config,&qmi_err_num))
  {
    resp_msg.enable_state = enable_state;
    LOG_MSG_INFO1("\nGet IP Passthrough Flag Success\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("\nGet IP Passthrough State failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_ip_passthrough_flag_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

   qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_ip_passthrough_flag_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_ip_passthrough_flag() */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_IP_PASSTHROUGH_STATE()

  DESCRIPTION
    Get IP Passthrough State.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_qcmap_msgr_get_ip_passthrough_state
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qcmap_msgr_get_ip_passthrough_state_req_msg_v01 *req_ptr;
  qcmap_msgr_get_ip_passthrough_state_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_LAN *QcMapLANMgr =GET_DEFAULT_LAN_OBJECT();
  bool active_state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_ip_passthrough_state_resp_msg_v01));

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_ip_passthrough_state_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("\nClient %p get ip passthrough state", clnt_info,0,0);
  CHECK_QCMAP_OBJ(QcMapLANMgr, resp_msg);

  /*---------------------------------------------------------------------
   Get IP Passthrough State
  ---------------------------------------------------------------------*/

  resp_msg.state = QcMapLANMgr->GetIPPassthroughState(&qmi_err_num);
  LOG_MSG_INFO1("\nGet IP Passthrough State Success\n",0,0,0);

 qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_ip_passthrough_state_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_ip_passthrough_state() */

static qmi_csi_cb_error  qmi_qcmap_msgr_bt_tethering_status_ind_reg
(
  qmi_qcmap_msgr_client_info_type      *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                     *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle)
{
  qmi_qcmap_msgr_state_info_type                    *qcmap_sp;
  qcmap_msgr_bt_tethering_status_ind_register_req_msg_v01 *req_ptr;
  qcmap_msgr_bt_tethering_status_ind_register_resp_msg_v01  resp_msg;
  qmi_csi_cb_error   rc;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_bt_tethering_status_ind_register_req_msg_v01 *)
             req_c_struct;
  memset(&resp_msg,
         0,
         sizeof(qcmap_msgr_bt_tethering_status_ind_register_resp_msg_v01));
  rc = QMI_CSI_CB_INTERNAL_ERR;

  clnt_info->bt_tethering_status_ind_regd = req_ptr->register_indication;
  LOG_MSG_INFO1 ("Client %p, reg %d for BT tethering status",
                 clnt_info,
                 clnt_info->bt_tethering_status_ind_regd,0);
  qmi_csi_send_resp(req_handle,
              msg_id,
              &resp_msg,
              sizeof(qcmap_msgr_bt_tethering_status_ind_register_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}

static qmi_csi_cb_error  qmi_qcmap_msgr_bt_tethering_wan_ind_reg
(
  qmi_qcmap_msgr_client_info_type      *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                     *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle)
{
  qmi_qcmap_msgr_state_info_type                    *qcmap_sp;
  qcmap_msgr_bt_tethering_wan_ind_register_req_msg_v01 *req_ptr;
  qcmap_msgr_bt_tethering_wan_ind_register_resp_msg_v01  resp_msg;
  qmi_csi_cb_error   rc;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_bt_tethering_wan_ind_register_req_msg_v01 *)
             req_c_struct;
  memset(&resp_msg,
         0,
         sizeof(qcmap_msgr_bt_tethering_wan_ind_register_resp_msg_v01));
  rc = QMI_CSI_CB_INTERNAL_ERR;

  clnt_info->bt_tethering_wan_ind_regd = req_ptr->register_indication;
  LOG_MSG_INFO1 ("Client %p, reg %d for BT tethering status",
                 clnt_info,
                 clnt_info->bt_tethering_wan_ind_regd,0);
  qmi_csi_send_resp(req_handle,
              msg_id,
              &resp_msg,
              sizeof(qcmap_msgr_bt_tethering_wan_ind_register_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}



/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_BRING_UP_BT_TETHERING()

  DESCRIPTION
    Bring up BT Tethering functionality on Apps.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_bring_up_bt_tethering
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qcmap_msgr_bring_up_bt_tethering_req_msg_v01    *req_ptr;
  qcmap_msgr_bring_up_bt_tethering_resp_msg_v01   resp_msg;

  unsigned int     index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_bt_tethering_mode_enum_v01 req_bt_mode;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_bring_up_bt_tethering_resp_msg_v01));

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_bring_up_bt_tethering_req_msg_v01 *)req_c_struct;


  /* Skip the validation for privileged client. */
  index = (req_ptr->mobile_ap_handle);
  if ((index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0))
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_bring_up_bt_tethering_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p bringing up BT tethering", clnt_info,0,0);
  //if Bluetooth Mode pass in request parameter
  if (req_ptr->bt_mode_valid)
  {
    req_bt_mode = req_ptr->bt_mode;
  }
  else
  {
    req_bt_mode = QCMAP_MSGR_BT_MODE_LAN_V01;
  }

  /*---------------------------------------------------------------------
    Bring UP BT Tethering
  ---------------------------------------------------------------------*/
  if (QCMAP_BT_TETHERING::BringupBTTethering(req_bt_mode,&qmi_err_num))
  {
    LOG_MSG_INFO1("Bring UP BT Tethering succeeds.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Bring UP BT Tethering fails.\n",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_bring_up_bt_tethering_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1("\nBrought UP BT Tethering",0,0,0);
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_bring_up_bt_tethering_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_qcmap_msgr_bring_up_bt_tethering() */


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_BRING_DOWN_BT_TETHERING()

  DESCRIPTION
    BRING DOWN BT TETHERING functionality on Apps.

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
static qmi_csi_cb_error  qmi_qcmap_msgr_bring_down_bt_tethering
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type        *qcmap_sp;
  qmi_qcmap_msgr_softap_handle_type     *map_handle;
  qcmap_msgr_bring_down_bt_tethering_req_msg_v01   *req_ptr;
  qcmap_msgr_bring_down_bt_tethering_resp_msg_v01   resp_msg;

  unsigned int     index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_bring_down_bt_tethering_resp_msg_v01));

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_bring_down_bt_tethering_req_msg_v01 *)req_c_struct;


  /* Skip the validation for privileged client. */
  index = (req_ptr->mobile_ap_handle);
  if ((index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0))
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_bring_down_bt_tethering_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p bringing down BT tethering", clnt_info,0,0);



  /*---------------------------------------------------------------------
    Bring Down BT Tethering
  ---------------------------------------------------------------------*/

  if (QCMAP_BT_TETHERING::BringdownBTTethering( &qmi_err_num))
  {
    LOG_MSG_INFO1("Bring DOWN BT Tethering succeeds.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Bring DOWN BT Tethering fails.\n",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_bring_down_bt_tethering_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1("\nBrought DOWN BT Tethering",0,0,0);
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_bring_down_bt_tethering_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_qcmap_msgr_bring_down_bt_tethering() */


/*===========================================================================
  FUNCTION qmi_qcmap_msgr_get_bt_tethering_status()

  DESCRIPTION
    Get BT tethering  status on Apps.

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
static qmi_csi_cb_error qmi_qcmap_msgr_get_bt_tethering_status
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_get_bt_tethering_status_resp_msg_v01 resp_msg;
  unsigned int     index;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qcmap_msgr_get_bt_tethering_status_req_msg_v01  *req_ptr = NULL;
  qcmap_msgr_bt_tethering_status_enum_v01 bt_teth_status =
                                    QCMAP_MSGR_BT_TETHERING_STATUS_ENUM_MIN_ENUM_VAL_V01;
  qcmap_bt_tethering_mode_enum_v01 bt_tethering_mode=
                                    QCMAP_BT_TETHERING_MODE_ENUM_MIN_ENUM_VAL_V01;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);
  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_bt_tethering_status_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p Get BT Tethering Status\n", clnt_info, 0, 0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_bt_tethering_status_resp_msg_v01));

  /*---------------------------------------------------------------------
   Get WLAN Status.
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     ( map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_get_bt_tethering_status_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  if (QCMAP_BT_TETHERING::GetBTTetheringStatus(&bt_teth_status, &bt_tethering_mode, &qmi_err_num))
  {
    QCMAP_CM_LOG("Got BT Tethering Status successfully.\n");
  }
  else
  {
    QCMAP_CM_LOG("Get BT Tethering Status failed!!");
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_bt_tethering_status_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  resp_msg.bt_tethering_status_valid = true;
  resp_msg.bt_tethering_status= bt_teth_status;
  resp_msg.bt_tethering_mode_valid = true;
  resp_msg.bt_tethering_mode= bt_tethering_mode;
  //mode as well
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_bt_tethering_status_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_bt_tethering_status */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_SET_INITIAL_PACKET_THRESHOLD()

  DESCRIPTION
    Set the packet threshold count to delay the time for the initial
    packets to flow through HW or sfe path. Until the packet threshold is reached
    packets will take the software path


  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_qcmap_msgr_set_initial_packet_threshold
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qcmap_msgr_set_initial_packet_threshold_req_msg_v01*req_ptr;
  qcmap_msgr_set_initial_packet_threshold_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_initial_packet_threshold_resp_msg_v01));
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_set_initial_packet_threshold_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p set initial packet limit", clnt_info,0,0);

  /*---------------------------------------------------------------------
   Set Initial Packet Threshold
  ---------------------------------------------------------------------*/

  if (QCMAP_NATALG::SetInitialPacketThreshold(req_ptr->packet_count,&qmi_err_num))
  {
    LOG_MSG_INFO1("Set initial Packet Threshold succeeded.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Set initial Packet Threshold failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_initial_packet_threshold_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_initial_packet_threshold_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_set_ipa_packet_threshold() */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_INITIAL_PACKET_THRESHOLD()

  DESCRIPTION
    Get the packet threshold count to delay the time for the initial
    packets to flow through HW path. Until the packet threshold is reached
    packets will take the software path


  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_qcmap_msgr_get_initial_packet_threshold
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qcmap_msgr_get_initial_packet_threshold_req_msg_v01    *req_ptr;
  qcmap_msgr_get_initial_packet_threshold_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_initial_packet_threshold_resp_msg_v01));
  int pkt_limit;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_initial_packet_threshold_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p get initial packet limit", clnt_info,0,0);

  /*---------------------------------------------------------------------
   Get Initial Packet Threshold
  ---------------------------------------------------------------------*/

  if (QCMAP_NATALG::GetInitialPacketThreshold(&pkt_limit,&qmi_err_num))
  {
    LOG_MSG_INFO1("Get intial Packet Threshold succeeded.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Get Initial Packet Threshold failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_initial_packet_threshold_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  resp_msg.packet_count = pkt_limit ;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_initial_packet_threshold_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_initial_packet_threshold() */

/*===========================================================================

  FUNCTION QMI_QCMAP_MSGR_ENABLE_PACKET_STATS()

  DESCRIPTION
    Enables Packet Stats
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
static qmi_csi_cb_error  qmi_qcmap_msgr_enable_packet_stats
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qcmap_msgr_enable_packet_stats_req_msg_v01    *req_ptr;
  qcmap_msgr_enable_packet_stats_resp_msg_v01   resp_msg;

  int              ret_val;
  unsigned int     index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  boolean privileged_client = false;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_enable_packet_stats_resp_msg_v01));

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_enable_packet_stats_req_msg_v01 *)req_c_struct;


  /* Skip the validation for privileged client. */
  index = (req_ptr->mobile_ap_handle);
  if ((index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0))
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_enable_packet_stats_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p enabling packet stats", clnt_info,0,0);

  /*---------------------------------------------------------------------
    Enable Packet Stats
  ---------------------------------------------------------------------*/
  if (QcMapMgr->EnablePacketStats(&qmi_err_num))
  {
    LOG_MSG_INFO1("Enable Packet Stats succeeds.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Enable Packet Stats fails.\n",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_enable_packet_stats_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_enable_packet_stats_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_qcmap_msgr_enable_packet_stats() */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_DISABLE_PACKET_STATS()

  DESCRIPTION
    Disables Packet Stats
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
static qmi_csi_cb_error  qmi_qcmap_msgr_disable_packet_stats
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qcmap_msgr_disable_packet_stats_req_msg_v01    *req_ptr;
  qcmap_msgr_disable_packet_stats_resp_msg_v01   resp_msg;

  int              ret_val;
  unsigned int     index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  boolean privileged_client = false;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_disable_packet_stats_resp_msg_v01));

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_disable_packet_stats_req_msg_v01 *)req_c_struct;


  /* Skip the validation for privileged client. */
  index = (req_ptr->mobile_ap_handle);
  if ((index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0))
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_disable_packet_stats_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p enabling packet stats", clnt_info,0,0);

  /*---------------------------------------------------------------------
    Disable Packet Stats
  ---------------------------------------------------------------------*/
  if (QcMapMgr->DisablePacketStats(&qmi_err_num))
  {
    LOG_MSG_INFO1("Disable Packet Stats succeeds.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Disable Packet Stats fails.\n",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_disable_packet_stats_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_disable_packet_stats_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_qcmap_msgr_disable_packet_stats() */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_RESET_PACKET_STATS()

  DESCRIPTION
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_qcmap_msgr_reset_packet_stats
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qcmap_msgr_reset_packet_stats_req_msg_v01    *req_ptr;
  qcmap_msgr_reset_packet_stats_resp_msg_v01   resp_msg;

  int              ret_val;
  unsigned int     index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  boolean privileged_client = false;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_reset_packet_stats_resp_msg_v01));

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_reset_packet_stats_req_msg_v01 *)req_c_struct;


  /* Skip the validation for privileged client. */
  index = (req_ptr->mobile_ap_handle);
  if ((index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0))
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_reset_packet_stats_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p resetting packet stats", clnt_info,0,0);

  /*---------------------------------------------------------------------
    Reset Packet Stats
  ---------------------------------------------------------------------*/
  if (QcMapMgr->ResetPacketStats(&qmi_err_num))
  {
    LOG_MSG_INFO1("Reset Packet Stats succeeds.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Reset Packet Stats fails.\n",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_reset_packet_stats_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_reset_packet_stats_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_qcmap_msgr_reset_packet_stats() */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_PACKET_STATS_STATUS()

  DESCRIPTION
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_qcmap_msgr_get_packet_stats_status
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qcmap_msgr_packet_stats_status_req_msg_v01    *req_ptr;
  qcmap_msgr_packet_stats_status_resp_msg_v01   resp_msg;

  int              ret_val;
  unsigned int     index;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  boolean privileged_client = false;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_packet_stats_status_resp_msg_v01));

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_packet_stats_status_req_msg_v01 *)req_c_struct;


  /* Skip the validation for privileged client. */
  index = (req_ptr->mobile_ap_handle);
  if ((index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0))
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_packet_stats_status_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1 ("Client %p obtaining packet stats status", clnt_info,0,0);

  /*---------------------------------------------------------------------
    Get Packet Stats
  ---------------------------------------------------------------------*/
  if (QcMapMgr->GetPacketStatsStatus(&qmi_err_num))
  {
    resp_msg.status = QCMAP_MSGR_PACKET_STATS_STATUS_ENABLED_V01;
    LOG_MSG_INFO1("Packet Stats Enabled",0,0,0);
  }
  else
  {
    resp_msg.status = QCMAP_MSGR_PACKET_STATS_STATUS_DISABLED_V01;
    LOG_MSG_INFO1("Packet Stats Disabled",0,0,0);
  }
  resp_msg.status_valid = TRUE;

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_packet_stats_status_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_qcmap_msgr_get_packet_stats_status() */

/*===========================================================================
FUNCTION QMI_QCMAP_MSGR_CREATE_WWAN_POLICY()

  DESCRIPTION
    Creates a particular profile and updates XML file

  PARAMETERS

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES

  SIDE EFFECTS
    None
===========================================================================*/

static qmi_csi_cb_error qmi_qcmap_msgr_create_wwan_policy
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  unsigned int                                index;
  qmi_qcmap_msgr_softap_handle_type          *map_handle;
  qcmap_msgr_create_wwan_policy_resp_msg_v01  resp_msg;
  qcmap_msgr_create_wwan_policy_req_msg_v01  *req_ptr;
  profile_handle_type_v01                     profile_handle;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  req_ptr = (qcmap_msgr_create_wwan_policy_req_msg_v01 *)req_c_struct;
  memset( &resp_msg, 0, sizeof(qcmap_msgr_create_wwan_policy_resp_msg_v01 ) );
  /*-------------------------------------------------------------------------
    Sanity check for qcmap handle
  --------------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if ( index != QCMAP_MSGR_SOFTAP_HANDLE ||
     ( map_handle = &qcmap_handle)->handle == 0 )
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_create_wwan_policy_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if (QcMapMgr && !(IS_MULTI_PDN_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_ERROR("Multi-PDN is not allowed on this target(%d)", QcMapMgr->target,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_create_wwan_policy_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  /*---------------------------------------------------------------------
   Create WWAN Config.
  ---------------------------------------------------------------------*/
  if (QCMAP_Backhaul_WWAN::CreateWWANPolicy(req_ptr->wwan_policy, &profile_handle, &qmi_err_num))
  {
    LOG_MSG_INFO1("Create policy successful.\n", 0, 0, 0);
    resp_msg.profile_handle_valid = TRUE;
    resp_msg.profile_handle = profile_handle;
  }
  else
  {
    LOG_MSG_ERROR("Create WWAN policy failed: %d", qmi_err_num, 0, 0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    resp_msg.profile_handle_valid = FALSE;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_create_wwan_policy_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_create_wwan_policy_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}


/*===========================================================================
FUNCTION QMI_QCMAP_MSGR_UPDATE_WWAN_POLICY()

  DESCRIPTION
    Updates a particular profile and updates XML file

  PARAMETERS

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES

  SIDE EFFECTS
    None
===========================================================================*/

static qmi_csi_cb_error qmi_qcmap_msgr_update_wwan_policy
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  unsigned int                                index;
  qmi_qcmap_msgr_softap_handle_type          *map_handle;
  qcmap_msgr_update_wwan_policy_resp_msg_v01  resp_msg;
  qcmap_msgr_update_wwan_policy_req_msg_v01  *req_ptr;
  QCMAP_Backhaul_WWAN                        *pBackhaulWWANObj;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  req_ptr = (qcmap_msgr_update_wwan_policy_req_msg_v01 *)req_c_struct;
  memset( &resp_msg, 0, sizeof(qcmap_msgr_update_wwan_policy_resp_msg_v01 ) );
  /*-------------------------------------------------------------------------
    Sanity check for qcmap handle
  --------------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if ( index != QCMAP_MSGR_SOFTAP_HANDLE ||
     ( map_handle = &qcmap_handle)->handle == 0 )
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_update_wwan_policy_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if (QcMapMgr && !(IS_MULTI_PDN_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_ERROR("Multi-PDN is not allowed on this target(%d)", QcMapMgr->target,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_update_wwan_policy_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  /*---------------------------------------------------------------------
   Update WWAN Config.
  ---------------------------------------------------------------------*/
  pBackhaulWWANObj = GET_BACKHAUL_WWAN_OBJ_FOR_QMI_CLIENT(clnt_info);
  CHECK_QCMAP_BACKHAUL_OBJ(pBackhaulWWANObj, resp_msg);
  if (pBackhaulWWANObj->UpdateWWANPolicy(req_ptr->update_req, req_ptr->wwan_policy, &qmi_err_num))
  {
    LOG_MSG_INFO1("Update policy successful.\n", 0, 0, 0);
  }
  else
  {
    LOG_MSG_ERROR("Update WWAN policy failed: %d", qmi_err_num, 0, 0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_update_wwan_policy_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_update_wwan_policy_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}


/*===========================================================================
FUNCTION QMI_QCMAP_MSGR_DELETE_WWAN_POLICY()

  DESCRIPTION
    Delete's a particular PDN and updates XML file

  PARAMETERS

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_delete_wwan_policy
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  unsigned int                                index;
  qmi_qcmap_msgr_softap_handle_type          *map_handle;
  qcmap_msgr_delete_wwan_policy_resp_msg_v01  resp_msg;
  qcmap_msgr_delete_wwan_policy_req_msg_v01  *req_ptr;
  profile_handle_type_v01                     profile_handle;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  req_ptr = (qcmap_msgr_delete_wwan_policy_req_msg_v01 *)req_c_struct;
  memset( &resp_msg, 0, sizeof(qcmap_msgr_delete_wwan_policy_resp_msg_v01 ) );
  /*-------------------------------------------------------------------------
    Sanity check for qcmap handle
  --------------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if ( index != QCMAP_MSGR_SOFTAP_HANDLE ||
     ( map_handle = &qcmap_handle)->handle == 0 )
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_delete_wwan_policy_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if (QcMapMgr && !(IS_MULTI_PDN_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_ERROR("Multi-PDN is not allowed on this target(%d)", QcMapMgr->target,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_delete_wwan_policy_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  /*---------------------------------------------------------------------
   Delete WWAN Config.
  ---------------------------------------------------------------------*/
  profile_handle = QCMAP_Backhaul::GetDefaultProfileHandle();
  if (clnt_info != NULL && clnt_info->profileHandle != INVALID_PROFILE_HANDLE)
  {
    profile_handle = clnt_info->profileHandle;
  }

  if (QCMAP_Backhaul_WWAN::DeleteWWANPolicy(profile_handle, &qmi_err_num))
  {
    /* Switch Profile Handle to Invalid */
    clnt_info->profileHandle = INVALID_PROFILE_HANDLE;
    LOG_MSG_INFO1("Delete PDN/policy successful.\n", 0, 0, 0);
  }
  else
  {
    LOG_MSG_ERROR("Delete WWAN policy failed: %d", qmi_err_num, 0, 0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_delete_wwan_policy_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_delete_wwan_policy_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
FUNCTION QMI_QCMAP_MSGR_GET_ALL_WWAN_POLICY_LIST()

DESCRIPTION
  Gets all wwan profile..

PARAMETERS

RETURN VALUE
  dsm * pointing to the response to be sent to host
  NULL if no response was generated or command was discarded

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_wwan_policy_list
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qcmap_msgr_wwan_policy_list_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;

  LOG_MSG_INFO1("Client %p Get WWAN Config\n", clnt_info, 0, 0);

  memset( &resp_msg, 0, sizeof(qcmap_msgr_wwan_policy_list_resp_msg_v01 ) );
  /*---------------------------------------------------------------------
   Get ALL WWAN Config.
  ---------------------------------------------------------------------*/
  if (QCMAP_Backhaul_WWAN::GetWWANPolicyList(&resp_msg, &qmi_err_num ))
  {
    LOG_MSG_INFO1("Get WWAN Config success.", 0, 0, 0);
  }
  else
  {
    LOG_MSG_ERROR("Get WWAN Config failed: %d", qmi_err_num, 0, 0);
    memset( &resp_msg, 0, sizeof(qcmap_msgr_wwan_policy_list_resp_msg_v01 ) );
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_wwan_policy_list_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  resp_msg.wwan_policy_valid = TRUE;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_wwan_policy_list_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
FUNCTION QMI_QCMAP_MSGR_SET_WWAN_PROFILE_PREFERENCE()

DESCRIPTION
  Sets WWAN Profile preference..
  a) Switches the WWAN Profile context.
  b) All WWAN API's called later-on will use profile preference set by this API.

PARAMETERS

RETURN VALUE
  dsm * pointing to the response to be sent to host
  NULL if no response was generated or command was discarded

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_set_wwan_profile_preference
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  unsigned int                                         index;
  qmi_qcmap_msgr_softap_handle_type                   *map_handle;
  qcmap_msgr_set_wwan_profile_preference_resp_msg_v01  resp_msg;
  qcmap_msgr_set_wwan_profile_preference_req_msg_v01  *req_ptr;
  profile_handle_type_v01                              profile_handle;
  qcmap_msgr_wwan_policy_list_resp_msg_v01             wwan_policy_resp_msg;
  boolean            profile_switch_success = false;
  int i = 0;
  qmi_error_type_v01 qmi_err_num;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  req_ptr = (qcmap_msgr_set_wwan_profile_preference_req_msg_v01 *)req_c_struct;
  memset( &resp_msg, 0, sizeof(qcmap_msgr_set_wwan_profile_preference_resp_msg_v01 ) );

  /*-------------------------------------------------------------------------
    Sanity check for qcmap handle
  --------------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if ( index != QCMAP_MSGR_SOFTAP_HANDLE ||
     ( map_handle = &qcmap_handle)->handle == 0 )
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_wwan_profile_preference_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if (QcMapMgr && !(IS_MULTI_PDN_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_ERROR("Multi-PDN is not allowed on this target(%d)", QcMapMgr->target,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_wwan_profile_preference_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  profile_switch_success = false;
  profile_handle = req_ptr->profile_handle;
  /*---------------------------------------------------------------------
   Cache WWAN Profile Preference.
  ---------------------------------------------------------------------*/
  if (QCMAP_Backhaul_WWAN::GetWWANPolicyList(&wwan_policy_resp_msg, &qmi_err_num))
  {
    for (i=0; i < wwan_policy_resp_msg.wwan_policy_len; i++)
    {
      if (wwan_policy_resp_msg.wwan_policy[i].profile_handle == profile_handle)
      {
        clnt_info->profileHandle = profile_handle;
        profile_switch_success = true;
        LOG_MSG_INFO1("Switching profileHandle=%p", clnt_info->profileHandle, 0,0);
        break;
      }
    }
  }

  if (profile_switch_success == false)
  {
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    LOG_MSG_ERROR("Set WWAN Profile(%d) Preference failed: %d", profile_handle, resp_msg.resp.error, 0);
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_wwan_profile_preference_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_wwan_profile_preference_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION qmi_qcmap_msgr_set_vlan_config()

  DESCRIPTION
    Sets the VLAN Config.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_set_vlan_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qcmap_msgr_set_vlan_config_req_msg_v01 *req_ptr;
  qcmap_msgr_set_vlan_config_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  unsigned int index;
  qmi_qcmap_msgr_softap_handle_type *map_handle;
  QCMAP_Virtual_LAN* QcMapVLAN = QCMAP_Virtual_LAN::Get_Instance(false);
  bool is_ipa_offloaded = false;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct);
  ASSERT(req_c_struct_len > 0);
  ASSERT(clnt_info);
  ASSERT(service_handle);

  req_ptr = (qcmap_msgr_set_vlan_config_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p set VLAN Config", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_vlan_config_resp_msg_v01));

  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_vlan_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if(!QcMapVLAN)
  {
    LOG_MSG_ERROR("QCMAP_VLAN obj NULL", 0, 0, 0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_vlan_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if(QcMapVLAN->SetVLANConfig(req_ptr->config, true, &qmi_err_num, &is_ipa_offloaded))
  {
    if (req_ptr->config.ipa_offload == 1)
    {
      resp_msg.is_ipa_offload_enabled_valid = true;
      resp_msg.is_ipa_offload_enabled = is_ipa_offloaded;
    }
    resp_msg.resp.error = qmi_err_num;
    LOG_MSG_INFO1("VLAN Config Set Succesfully: 0x%x", resp_msg.resp.error, 0, 0);
  }
  else
  {
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    LOG_MSG_ERROR("Set VLAN Config failed: 0x%x", resp_msg.resp.error, 0, 0);
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_vlan_config_resp_msg_v01));


  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_set_vlan_config */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_VLAN_CONFIG()

  DESCRIPTION
    Get Current VLAN Config

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
static qmi_csi_cb_error    qmi_qcmap_msgr_get_vlan_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_get_vlan_config_req_msg_v01 *req_ptr;
  qcmap_msgr_get_vlan_config_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;

  int              ret_val;
  unsigned int     index;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Virtual_LAN* QcMapVLAN = QCMAP_Virtual_LAN::Get_Instance(false);
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_vlan_config_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p get VLAN Config", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_vlan_config_resp_msg_v01));

  if(!QcMapVLAN)
  {
    LOG_MSG_ERROR("QCMAP_VLAN obj NULL", 0, 0, 0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_vlan_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if(QcMapVLAN->GetVLANConfig(&resp_msg.vlan_config_list[0], &resp_msg.vlan_config_list_len,
                              &qmi_err_num))
  {
    LOG_MSG_INFO1("Get VLAN Config succeeded.\n",0,0,0);

  } else {
    LOG_MSG_ERROR("Get VLAN Config failed!!",0,0,0);
    resp_msg.vlan_config_list_valid = false;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_get_vlan_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  resp_msg.vlan_config_list_valid = true;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_vlan_config_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_vlan_config() */

/*===========================================================================
  FUNCTION qmi_qcmap_msgr_delete_vlan_config()

  DESCRIPTION
    Deletes the VLAN Config.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_delete_vlan_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qcmap_msgr_delete_vlan_config_req_msg_v01 *req_ptr;
  qcmap_msgr_delete_vlan_config_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  unsigned int index;
  qmi_qcmap_msgr_softap_handle_type *map_handle;
  QCMAP_Virtual_LAN* QcMapVLAN = QCMAP_Virtual_LAN::Get_Instance(false);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct);
  ASSERT(req_c_struct_len > 0);
  ASSERT(clnt_info);
  ASSERT(service_handle);

  req_ptr = (qcmap_msgr_delete_vlan_config_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p Delete VLAN Config", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_delete_vlan_config_resp_msg_v01));

  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_delete_vlan_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if(!QcMapVLAN)
  {
    LOG_MSG_ERROR("QCMAP_VLAN obj NULL", 0, 0, 0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_vlan_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if(QcMapVLAN->DeleteVLANConfig(req_ptr->config, &qmi_err_num))
  {
    LOG_MSG_INFO1("VLAN Config Deleted Succesfully. \n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Delete VLAN Config failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_delete_vlan_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_delete_vlan_config_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_delete_vlan_config */

/*===========================================================================
  FUNCTION qmi_qcmap_msgr_set_l2tp_enable_config()

  DESCRIPTION
    Sets the L2TP Enable Config.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_set_unmanaged_l2tp_state
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qcmap_msgr_set_unmanaged_l2tp_state_req_msg_v01 *req_ptr;
  qcmap_msgr_set_unmanaged_l2tp_state_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  unsigned int index;
  qmi_qcmap_msgr_softap_handle_type *map_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct);
  ASSERT(req_c_struct_len > 0);
  ASSERT(clnt_info);
  ASSERT(service_handle);

  req_ptr = (qcmap_msgr_set_unmanaged_l2tp_state_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p set L2TP Enable Config", clnt_info,0,0);

  memset(&resp_msg, 0,sizeof(qcmap_msgr_set_unmanaged_l2tp_state_resp_msg_v01));

  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_unmanaged_l2tp_state_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if (QCMAP_L2TP::SetL2TPState(req_ptr->config, map_handle, &qmi_err_num))
  {
    LOG_MSG_INFO1("L2TP Enable Set Succesfully. \n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("L2TP Enable Config failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_unmanaged_l2tp_state_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_unmanaged_l2tp_state_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_set_l2tp_state*/
/*===========================================================================
  FUNCTION qmi_qcmap_msgr_set_l2tp_config()

  DESCRIPTION
    Sets the L2TP Tunnel Config.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_set_l2tp_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qcmap_msgr_set_l2tp_config_req_msg_v01 *req_ptr;
  qcmap_msgr_set_l2tp_config_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  unsigned int index;
  qmi_qcmap_msgr_softap_handle_type *map_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct);
  ASSERT(req_c_struct_len > 0);
  ASSERT(clnt_info);
  ASSERT(service_handle);

  req_ptr = (qcmap_msgr_set_l2tp_config_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p set L2TP Config", clnt_info,0,0);

  memset(&resp_msg, 0,sizeof(qcmap_msgr_set_l2tp_config_resp_msg_v01));

  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_l2tp_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if (QCMAP_L2TP::SetL2TPConfig(req_ptr->config, map_handle, &qmi_err_num))
  {
    LOG_MSG_INFO1("L2TP Config Set Succesfully. \n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Set L2TP Config failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_l2tp_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_l2tp_config_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_set_l2tp_config */

/*===========================================================================
  FUNCTION qmi_qcmap_msgr_set_MTU_for_L2TP_config()

  DESCRIPTION
    Sets MTU size config to set MTU size on underlyin interfaces.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_set_MTU_for_L2TP_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qcmap_msgr_set_MTU_for_l2tp_config_req_msg_v01 *req_ptr;
  qcmap_msgr_set_MTU_for_l2tp_config_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  unsigned int index;
  qmi_qcmap_msgr_softap_handle_type *map_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct);
  ASSERT(req_c_struct_len > 0);
  ASSERT(clnt_info);
  ASSERT(service_handle);

  req_ptr = (qcmap_msgr_set_MTU_for_l2tp_config_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p set MTU Config for L2TP", clnt_info,0,0);

  memset(&resp_msg, 0,sizeof(qcmap_msgr_set_MTU_for_l2tp_config_resp_msg_v01));

  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_MTU_for_l2tp_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if (QCMAP_L2TP::SetMTUConfigForL2TP(
                                   req_ptr->config, map_handle, &qmi_err_num))
  {
    LOG_MSG_INFO1("L2TP Config with MTU Set Succesfully. \n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Set MTU Config for L2TP failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_MTU_for_l2tp_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_MTU_for_l2tp_config_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_set_MTU_for_L2TP_config */

/*===========================================================================
  FUNCTION qmi_qcmap_msgr_set_TCP_MSS_for_L2TP_config()

  DESCRIPTION
    Sets TCP MSS config to install IP table rules to avoid segmentation of
    L2TP tunelled packets.

  PARAMETERS
    clnt_info        : Client Info
    req_handle       : Request handle
    msg_id           : Message ID corresponding to the API
    req_c_struct     : Request structure
    req_c_struct_len : Length of the request structure
    service_handle   : Service handle

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_set_TCP_MSS_for_L2TP_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qcmap_msgr_set_TCP_MSS_for_l2tp_config_req_msg_v01 *req_ptr;
  qcmap_msgr_set_TCP_MSS_for_l2tp_config_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  unsigned int index;
  qmi_qcmap_msgr_softap_handle_type *map_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct);
  ASSERT(req_c_struct_len > 0);
  ASSERT(clnt_info);
  ASSERT(service_handle);

  req_ptr = (qcmap_msgr_set_TCP_MSS_for_l2tp_config_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p set TCP MSS config for L2TP", clnt_info,0,0);

  memset(&resp_msg, 0,
          sizeof(qcmap_msgr_set_TCP_MSS_for_l2tp_config_resp_msg_v01));

  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                  sizeof(qcmap_msgr_set_TCP_MSS_for_l2tp_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if (QCMAP_L2TP::SetTCPMSSConfigForL2TP(
                                   req_ptr->config, map_handle, &qmi_err_num))
  {
    LOG_MSG_INFO1("TCP MSS Config for L2TP set succesfully. \n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Set LTCP MSS for L2TP failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                 sizeof(qcmap_msgr_set_TCP_MSS_for_l2tp_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
            sizeof(qcmap_msgr_set_TCP_MSS_for_l2tp_config_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_set_TCP_MSS_for_L2TP_config */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_L2TP_CONFIG()

  DESCRIPTION
    Get Current L2TP Config

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
static qmi_csi_cb_error    qmi_qcmap_msgr_get_l2tp_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_get_l2tp_config_req_msg_v01 *req_ptr;
  qcmap_msgr_get_l2tp_config_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;

  int              ret_val;
  unsigned int     index;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_l2tp_config_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p get L2TP Config", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_l2tp_config_resp_msg_v01));

  /*---------------------------------------------------------------------
   Get DHCP Reservation Records
  ---------------------------------------------------------------------*/

  if (QCMAP_L2TP::GetL2TPConfig(&resp_msg.l2tp_config_list[0],\
                                     &resp_msg.l2tp_config_list_len,
                                     &resp_msg.mode,
                                     &resp_msg.mtu_config,
                                     &resp_msg.tcp_mss_config,
                                      &qmi_err_num))
  {
    LOG_MSG_INFO1("Get L2TP Config succeeded.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Get L2TP Config failed!!",0,0,0);

    resp_msg.l2tp_config_list_valid = false;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_l2tp_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  resp_msg.l2tp_config_list_valid = true;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_l2tp_config_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_l2tp_config() */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_DELETE_L2TP_TUNNEL_CONFIG()

  DESCRIPTION
    Delete L2TP Config

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
static qmi_csi_cb_error    qmi_qcmap_msgr_delete_l2tp_tunnel_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_delete_l2tp_tunnel_config_req_msg_v01 *req_ptr;
  qcmap_msgr_delete_l2tp_tunnel_config_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;

  int              ret_val;
  unsigned int     index;
  qmi_qcmap_msgr_softap_handle_type *map_handle;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  req_ptr = (qcmap_msgr_delete_l2tp_tunnel_config_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p delete L2TP Config", clnt_info,0,0);

  memset(&resp_msg, 0,
         sizeof(qcmap_msgr_delete_l2tp_tunnel_config_resp_msg_v01));

  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_delete_l2tp_tunnel_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if (QCMAP_L2TP::DeleteL2TPTunnelConfig(
                            req_ptr->config, map_handle, &qmi_err_num))
  {
    LOG_MSG_INFO1("Delete L2TP Config succeeded.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Delete L2TP Config failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_delete_l2tp_tunnel_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_delete_l2tp_tunnel_config_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_delete_l2tp_tunnel_config() */

/*===========================================================================
FUNCTION QMI_QCMAP_MSGR_ADD_PDN_TO_VLAN_MAPPING()

DESCRIPTION
  Sets what VLAN a PDN corresponds to

PARAMETERS

RETURN VALUE
  dsm * pointing to the response to be sent to host
  NULL if no response was generated or command was discarded

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_add_pdn_to_vlan_mapping
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qcmap_msgr_add_pdn_to_vlan_mapping_req_msg_v01 *req_msg;
  qcmap_msgr_add_pdn_to_vlan_mapping_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  req_msg = (qcmap_msgr_add_pdn_to_vlan_mapping_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1("Client %p Add PDN to VLAN Mapping", clnt_info, 0, 0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_add_pdn_to_vlan_mapping_resp_msg_v01));

  /*---------------------------------------------------------------------
   Add PDN to VLAN Mapping
  ---------------------------------------------------------------------*/
  if(QcMapMgr && !(IS_MULTI_PDN_VLAN_MAPPING_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_ERROR("Multi-PDN to VLAN mapping is not allowed on this target(%d)",
                  QcMapMgr->target,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_NOT_SUPPORTED_V01;
  }
  else
  {
    QCMAP_Backhaul_WWAN *QCMAP_Backhaul_WWAN_Obj = GET_BACKHAUL_WWAN_OBJECT(req_msg->profile_handle);
    CHECK_QCMAP_OBJ(QCMAP_Backhaul_WWAN_Obj, resp_msg);

    if(QCMAP_Backhaul_WWAN_Obj->AddVlanMapping(req_msg->vlan_id, &qmi_err_num))
    {
      LOG_MSG_INFO1("Add PDN to vlan Mapping success.", 0, 0, 0);
    } else {
      LOG_MSG_ERROR("Add PDN to vlan Mapping failed: %d", qmi_err_num, 0, 0);
      memset( &resp_msg, 0, sizeof(qcmap_msgr_add_pdn_to_vlan_mapping_resp_msg_v01));
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      resp_msg.resp.error = qmi_err_num;
    }
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_add_pdn_to_vlan_mapping_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
FUNCTION QMI_QCMAP_MSGR_DELETE_PDN_TO_VLAN_MAPPING()

DESCRIPTION
  Deletes VLAN to PDN mapping

PARAMETERS

RETURN VALUE
  dsm * pointing to the response to be sent to host
  NULL if no response was generated or command was discarded

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_delete_pdn_to_vlan_mapping
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  unsigned int index;
  qcmap_msgr_delete_pdn_to_vlan_mapping_req_msg_v01 *req_msg;
  qcmap_msgr_delete_pdn_to_vlan_mapping_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qmi_qcmap_msgr_softap_handle_type *map_handle;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  req_msg = (qcmap_msgr_delete_pdn_to_vlan_mapping_req_msg_v01 *)req_c_struct;

  index = (req_msg->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     ( map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_msg->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_delete_pdn_to_vlan_mapping_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1("Client %p Delete PDN to VLAN Mapping", clnt_info, 0, 0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_delete_pdn_to_vlan_mapping_resp_msg_v01));

  /*---------------------------------------------------------------------
   Delete PDN to VLAN Mapping
  ---------------------------------------------------------------------*/
  if(QcMapMgr && !(IS_MULTI_PDN_VLAN_MAPPING_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_ERROR("Multi-PDN to VLAN mapping is not allowed on this target(%d)",
                  QcMapMgr->target,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_NOT_SUPPORTED_V01;
  }
  else
  {
    QCMAP_Backhaul_WWAN *QCMAP_Backhaul_WWAN_Obj = GET_BACKHAUL_WWAN_OBJECT(req_msg->profile_handle);
    CHECK_QCMAP_OBJ(QCMAP_Backhaul_WWAN_Obj, resp_msg);

    if (QCMAP_Backhaul_WWAN_Obj->DeleteVlanMapping(req_msg->vlan_id, &qmi_err_num))
    {
      LOG_MSG_INFO1("Delete PDN to VLAN Mapping success.", 0, 0, 0);
    } else {
      LOG_MSG_ERROR("Delete PDN to VLAN Mapping failed: %d", qmi_err_num, 0, 0);
      memset( &resp_msg, 0, sizeof(qcmap_msgr_delete_pdn_to_vlan_mapping_resp_msg_v01));
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      resp_msg.resp.error = qmi_err_num;
    }
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_delete_pdn_to_vlan_mapping_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
FUNCTION QMI_QCMAP_MSGR_GET_PDN_TO_VLAN_MAPPINGS()

DESCRIPTION
  Gets all the PDN/VLAN mappings. Will show -1 for vlan if PDN has no
  corresponding mapping.

PARAMETERS

RETURN VALUE
  dsm * pointing to the response to be sent to host
  NULL if no response was generated or command was discarded

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_get_pdn_to_vlan_mappings
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qcmap_msgr_get_pdn_to_vlan_mappings_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  LOG_MSG_INFO1("Client %p Get PDN to VLAN Mapping", clnt_info, 0, 0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_pdn_to_vlan_mappings_resp_msg_v01));

  /*---------------------------------------------------------------------
   Get PDN to VLAN Mapping.
  ---------------------------------------------------------------------*/
  QCMAP_Backhaul *QcMapBackhaul;
  DECLARE_HASH_MAP_ITERATOR_FOR_BACKHAUL;
  START_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash);
  int index = 0;
  if(QcMapMgr && !(IS_MULTI_PDN_VLAN_MAPPING_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_ERROR("Multi-PDN to VLAN mapping is not allowed on this target(%d)",
                  QcMapMgr->target,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_NOT_SUPPORTED_V01;
  }
  else
  {
    while (END_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash))
    {
      QcMapBackhaul = GET_BACKHAUL_OBJ_AND_INC;
      resp_msg.pdn_to_vlan_mappings[index].profile_handle = QcMapBackhaul->profileHandle;
      resp_msg.pdn_to_vlan_mappings[index++].vlan_id = QcMapBackhaul->vlan_id;
      LOG_MSG_INFO1("Profile %d, VLAN %d", QcMapBackhaul->profileHandle, QcMapBackhaul->vlan_id, 0);
    }

    /* If index is > 0 then list has values in it */
    if (index > 0)
    {
      resp_msg.pdn_to_vlan_mappings_valid = true;
      resp_msg.pdn_to_vlan_mappings_len = index;
      LOG_MSG_INFO1("Number of mappings %d ", index, 0, 0);
    }
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_pdn_to_vlan_mappings_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_ENABLE_GSB()

  DESCRIPTION
    Enables GSB

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
static qmi_csi_cb_error    qmi_qcmap_msgr_enable_gsb
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_enable_gsb_req_msg_v01 *req_ptr;
  qcmap_msgr_enable_gsb_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;


  int              ret_val;
  unsigned int     index;
  qmi_qcmap_msgr_softap_handle_type *map_handle;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  req_ptr = (qcmap_msgr_enable_gsb_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p enable GSB", clnt_info,0,0);

  memset(&resp_msg, 0,
         sizeof(qcmap_msgr_enable_gsb_resp_msg_v01));

  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_enable_gsb_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if (QCMAP_GSB::ConfigureGSB(&qmi_err_num))
  {
    LOG_MSG_INFO1("Enable GSB succeeded",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Enable GSB  failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_enable_gsb_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_enable_gsb_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_enable_gsb() */


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_DISABLE_GSB()

  DESCRIPTION
    DISABLES GSB

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
static qmi_csi_cb_error    qmi_qcmap_msgr_disable_gsb
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_disable_gsb_req_msg_v01 *req_ptr;
  qcmap_msgr_disable_gsb_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;

  int              ret_val;
  unsigned int     index;
  qmi_qcmap_msgr_softap_handle_type *map_handle;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  req_ptr = (qcmap_msgr_disable_gsb_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p disable GSB", clnt_info,0,0);

  memset(&resp_msg, 0,
         sizeof(qcmap_msgr_disable_gsb_resp_msg_v01));

  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_disable_gsb_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if (QCMAP_GSB::DisableGSB(&qmi_err_num))
  {
    LOG_MSG_INFO1("Disable GSB succeeded.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Disable GSB  failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_disable_gsb_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_disable_gsb_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_disable_gsb() */


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_DELETE_GSB_CONFIG()

  DESCRIPTION
    Deletes GSB configuration

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
static qmi_csi_cb_error    qmi_qcmap_msgr_delete_gsb_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_delete_gsb_config_req_msg_v01 *req_ptr;
  qcmap_msgr_delete_gsb_config_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;

  int              ret_val;
  unsigned int     index;
  qmi_qcmap_msgr_softap_handle_type *map_handle;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  req_ptr = (qcmap_msgr_delete_gsb_config_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1("Client %p delete %s config", clnt_info, req_ptr->if_name, 0);

  memset(&resp_msg, 0,
         sizeof(qcmap_msgr_delete_gsb_config_resp_msg_v01));

  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_delete_gsb_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if (QCMAP_GSB::DeleteGSBConfig(req_ptr->if_name, &qmi_err_num, true))
  {
    LOG_MSG_INFO1("Delete GSB config succeeded.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Delete GSB config failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_delete_gsb_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_delete_gsb_config_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_delete_gsb_config() */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_SET_GSB_CONFIG()

  DESCRIPTION
    Set GSB Config on Apps.

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
static qmi_csi_cb_error qmi_qcmap_msgr_set_gsb_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_set_gsb_config_req_msg_v01 *req_ptr;
  qcmap_msgr_set_gsb_config_resp_msg_v01 resp_msg;
  qcmap_msgr_gsb_config_v01 gsb_conf;
  unsigned int     index;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_set_gsb_config_req_msg_v01 *)req_c_struct;

  printf ("Client %p set GSB config", clnt_info);

  if (req_ptr->config_valid)
  {
    memcpy(&gsb_conf, &req_ptr->config,sizeof(qcmap_msgr_gsb_config_v01));
  }
  else
  {
    LOG_MSG_ERROR("GSB config is invalid",0,0,0);
  }



  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_gsb_config_resp_msg_v01));

  /*---------------------------------------------------------------------
   Set GSB Config.
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     ( map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_gsb_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  if (QCMAP_GSB::SetGSBConfig(&gsb_conf, &qmi_err_num))
  {
    LOG_MSG_INFO1("GSB config set successfully",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Set GSB config failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_gsb_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_gsb_config_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_set_gsb_config */


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_GSB_CONFIG()

  DESCRIPTION
    Gets the configured GSB settings

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
static qmi_csi_cb_error qmi_qcmap_msgr_get_gsb_config
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_get_gsb_config_resp_msg_v01 resp_msg;
  uint8 entries = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;

  printf ("Client %p Get GSB Config\n", clnt_info);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_gsb_config_resp_msg_v01));

  /*---------------------------------------------------------------------
   Get GSB Config.
  ---------------------------------------------------------------------*/
  if (QCMAP_GSB::GetGSBConfig(resp_msg.config , &entries, &qmi_err_num))
  {
    LOG_MSG_INFO1("Got GSB Config successfully",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Get GSB Config Failed",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_gsb_config_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if (entries > 0)
  {
    resp_msg.config_valid = TRUE;
    resp_msg.config_len = entries;
  }
  else
  {
    resp_msg.config_valid = FALSE;
    resp_msg.config_len = 0;

  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_gsb_config_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_gsb_config */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_QMI_DUN_DONGLE_MODE()

  DESCRIPTION
    Get Dundonglemode value .

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
static qmi_csi_cb_error qmi_qcmap_msgr_get_dun_dongle_mode
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_get_dun_dongle_mode_req_msg_v01 *req_ptr;
  qcmap_msgr_get_dun_dongle_mode_resp_msg_v01 resp_msg;
  boolean    dun_dongle_mode_status = false;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_dun_dongle_mode_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p get dun dongle mode", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_dun_dongle_mode_resp_msg_v01));
  /*---------------------------------------------------------------------
  Get the DunDongleMode status
  ---------------------------------------------------------------------*/

  if (QCMAP_Tethering::GetDunDongleMode(&dun_dongle_mode_status, &qmi_err_num))
  {
    LOG_MSG_INFO1("Got DUN Dongle Mode status successfully",0,0,0);
    resp_msg.dun_dongle_mode_valid = TRUE;
    resp_msg.dun_dongle_mode = dun_dongle_mode_status;
  }
  else
  {
    LOG_MSG_ERROR("Get DUN Dongle Mode status Failed",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_dun_dongle_mode_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_dun_dongle_mode */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_SET_DUN_DONGLE_MODE()

  DESCRIPTION
    Set Dun dongle mode value.

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
static qmi_csi_cb_error qmi_qcmap_msgr_set_dun_dongle_mode
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_set_dun_dongle_mode_req_msg_v01 *req_ptr;
  qcmap_msgr_set_dun_dongle_mode_resp_msg_v01 resp_msg;
  unsigned int index;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_set_dun_dongle_mode_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p set DUN Dongle Mode", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_dun_dongle_mode_resp_msg_v01));

  /*---------------------------------------------------------------------
  Set DunDongleMode status
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_dun_dongle_mode_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if (QCMAP_Tethering::SetDunDongleMode(req_ptr->enable_dun_dongle_mode,
                                        &qmi_err_num))
  {
    LOG_MSG_INFO1("Set DUN Dongle Mode successfully.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Set DUN Dongle Mode failed!!",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_dun_dongle_mode_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_set_dun_dongle_mode */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_DATA_PATH_OPT_STATUS()

  DESCRIPTION
    Gets the configuration status for data path optimizer
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

static qmi_csi_cb_error qmi_qcmap_msgr_get_data_path_opt_status
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle

)
{

  qmi_qcmap_msgr_state_info_type      *qcmap_sp;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  boolean data_path_opt_status;
  qcmap_msgr_get_data_path_opt_status_resp_msg_v01  resp_msg;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_data_path_opt_status_resp_msg_v01));

  if(!QcMapMgr)
  {
    LOG_MSG_ERROR("QcMap Object not created",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                sizeof(qcmap_msgr_get_data_path_opt_status_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if(QcMapMgr->GetDataPathOptManagerStatus(&(data_path_opt_status), &qmi_err_num))
  {
    resp_msg.data_path_opt_status_valid = TRUE;
    resp_msg.data_path_opt_status = data_path_opt_status;
    LOG_MSG_INFO1("Get Data path opt status successfull = %d",
                                         resp_msg.data_path_opt_status,0,0);
  }
  else
  {
    LOG_MSG_INFO1("Get Data path opt status failed",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
  }
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                   sizeof(qcmap_msgr_get_data_path_opt_status_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_SET_DATA_PATH_OPT_STATUS()

  DESCRIPTION
    Sets the configured data path opt settings

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

static qmi_csi_cb_error qmi_qcmap_msgr_set_data_path_opt_status
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle

)
{

  qcmap_msgr_set_data_path_opt_status_req_msg_v01  *req_ptr;
  qcmap_msgr_set_data_path_opt_status_resp_msg_v01  resp_msg;

  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;

  unsigned int     index;
  qmi_qcmap_msgr_softap_handle_type *map_handle;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  req_ptr = (qcmap_msgr_set_data_path_opt_status_req_msg_v01*) req_c_struct;

  memset(&resp_msg, 0,
      sizeof(qcmap_msgr_set_data_path_opt_status_req_msg_v01));

  if(!QcMapMgr)
  {
    LOG_MSG_ERROR("QcMap Object not created",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                sizeof(qcmap_msgr_set_data_path_opt_status_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  index = req_ptr->mobile_ap_handle;
  if (index != QCMAP_MSGR_SOFTAP_HANDLE || ((map_handle = &qcmap_handle)->handle == 0))
  {
    LOG_MSG_ERROR("Set Data Opt request failed",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                     sizeof(qcmap_msgr_set_data_path_opt_status_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  else
  {
    if (QcMapMgr->SetDataPathOptManagerStatus(req_ptr->data_path_opt_status, &qmi_err_num))
    {
      LOG_MSG_INFO1("Set Data opt status request done successfully = %d",
                                                      req_ptr->data_path_opt_status,0,0);
    }
    else
    {
      LOG_MSG_ERROR("Set Data Status request config failed!!",0,0,0);
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      resp_msg.resp.error = qmi_err_num;
    }
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                   sizeof(qcmap_msgr_set_data_path_opt_status_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION qmi_qcmap_msgr_get_pmip_mode()

  DESCRIPTION
    Gets the current value Proxy IP Mobility (PMIP) mode.

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
static qmi_csi_cb_error qmi_qcmap_msgr_get_pmip_mode
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  unsigned int     index;
  qcmap_msgr_get_pmip_mode_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qcmap_msgr_get_pmip_mode_req_msg_v01  *req_ptr = NULL;
  QCMAP_Backhaul* QcMapBackhaulMgr = GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);
  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_pmip_mode_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p Get PMIP mode\n", clnt_info, 0, 0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_pmip_mode_resp_msg_v01));

  /*---------------------------------------------------------------------
   Get WLAN Status.
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     ( map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_get_pmip_mode_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

#ifndef FEATURE_DATA_TARGET_MDM9607
  LOG_MSG_ERROR("Unsupported target for Pmipv6.Only LYKAN supported ", 0 , 0, 0);
  resp_msg.resp.result = QMI_RESULT_FAILURE;
  resp_msg.resp.error = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;

#else
  if(!QCMAP_Backhaul::GetPmipConfig(&resp_msg, &qmi_err_num))
  {
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
  }
  else
  {
    resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
    resp_msg.resp.error = QMI_ERR_NONE_V01;
  }
#endif

  LOG_MSG_INFO1("qmi_qcmap_msgr_get_pmip_mode: status = %d & err:%d",resp_msg.pmip_mode, resp_msg.resp.error, 0);
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_pmip_mode_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_pmip_mode */


/*===========================================================================
  FUNCTION qmi_qcmap_msgr_set_pmip_mode()

  DESCRIPTION
    Sets the Proxy IP Mobility (PMIP) mode.

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
    Backaul WWAN should be enabled

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_set_pmip_mode
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  unsigned int     index;
  qcmap_msgr_set_pmip_mode_resp_msg_v01 resp_msg;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qcmap_msgr_set_pmip_mode_req_msg_v01  *req_ptr = NULL;
  bool  pmip_mode = FALSE;
  bool  result = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);
  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_set_pmip_mode_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p Set PMIP mode to %d\n", clnt_info, req_ptr->enable_pmip_mode, 0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_pmip_mode_resp_msg_v01));

  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
     ( map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_pmip_mode_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

#ifndef FEATURE_DATA_TARGET_MDM9607
  LOG_MSG_ERROR("Unsupported target for Pmipv6.Only LYKAN supported ", 0 , 0, 0);
  /*
  * Pmip is currently enabled only for mdm9607 . For other target pmip is disabled.
  * If we need to enable Pmip on IPA enabled target then we have to disable IPA so that all packet
  * will go through PMIP tunnel
  */

  resp_msg.resp.result = QMI_RESULT_FAILURE;
  resp_msg.resp.error = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;

#else
  pmip_mode = QCMAP_Backhaul::IsPmipV6ModeEnabled();
  LOG_MSG_INFO1 ("Current status%d\n",pmip_mode,0,0);

  pmip_mode = QCMAP_Backhaul::IsPmipV4ModeEnabled();
  LOG_MSG_INFO1 ("Current pmip V4 status%d\n",pmip_mode,0,0);


  LOG_MSG_INFO1 ("Set New mode%d\n",req_ptr->enable_pmip_mode,0,0);
  result = QCMAP_Backhaul::SetPmipMode(req_ptr, &qmi_err_num);

  LOG_MSG_INFO1("qmi_qcmap_msgr_set_pmip_mode: pmip_mode:%d Result = %d & Error_num:%d",
                 pmip_mode, result, qmi_err_num);
  resp_msg.resp.result = ((result==TRUE)?QMI_RESULT_SUCCESS_V01:QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error = qmi_err_num;
#endif

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_pmip_mode_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_set_pmip_mode */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_BACKHAUL_STATUS()

  DESCRIPTION
    Gets the backhaul status

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
static qmi_csi_cb_error  qmi_qcmap_msgr_get_backhaul_status
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type  *qcmap_sp;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_get_backhaul_status_resp_msg_v01 resp_msg;
  qcmap_cm_backhaul_status backhaul_status;
  QCMAP_Backhaul* QcMapBackhaul = GET_DEFAULT_BACKHAUL_OBJECT();
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_backhaul_status_resp_msg_v01));
  memset(&backhaul_status,0,sizeof(qcmap_cm_backhaul_status));

  /*---------------------------------------------------------------------
   Get Backhaul Status.
  ---------------------------------------------------------------------*/
  if ((QcMapBackhaul != NULL) && (QcMapBackhaul->GetBackhaulStatus(&backhaul_status , &qmi_err_num) == QCMAP_CM_SUCCESS))
  {
    LOG_MSG_INFO1("Get Backhaul Status successfully",0,0,0);;
    if (backhaul_status.backhaul_v4_available >= 0)
    {
      resp_msg.backhaul_v4_status_valid = TRUE;
      resp_msg.backhaul_v4_status = (backhaul_status.backhaul_v4_available)?true:false;
    }
    if (backhaul_status.backhaul_v6_available >= 0)
    {
      resp_msg.backhaul_v6_status_valid = TRUE;
      resp_msg.backhaul_v6_status = (backhaul_status.backhaul_v6_available)?true:false;
    }
    if ((backhaul_status.backhaul_v4_available == true) || (backhaul_status.backhaul_v6_available == true))
    {
      LOG_MSG_INFO1("Backhaul is Availbale ", 0, 0, 0);
      resp_msg.backhaul_type_valid = TRUE;
      resp_msg.backhaul_type = backhaul_status.backhaul_type;
    }
    else
    {
      LOG_MSG_INFO1("No Backhaul ", 0, 0, 0);
    }
  }
  else
  {
    LOG_MSG_ERROR("Get Backhaul Status Failed",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    if (QcMapBackhaul == NULL)
      resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
    else
      resp_msg.resp.error = qmi_err_num;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_get_backhaul_status_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_backhaul_status_resp_msg_v01));

  LOG_MSG_INFO1("Sending resp to qmi client", 0, 0, 0);

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_backhaul_status */

/*===========================================================================
  FUNCTION qmi_qcmap_msgr_indication_register

  DESCRIPTION
    Registers for various indications

  INPUT
    clnt_info       - clint information stracture
    req_handle      - request handle
    msg_id          - message id
    req_c_struct    - struct void pointer
    req_c_struct_len- struct length
    service_handle  - service handle

  RETURN VALUE
    None

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_qcmap_msgr_indication_register
(
  qmi_qcmap_msgr_client_info_type      *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                     *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type                    *qcmap_sp;
  qcmap_msgr_indication_register_req_msg_v01   *req_ptr;
  qcmap_msgr_indication_register_resp_msg_v01  resp_msg;
  qmi_csi_cb_error   rc;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  QCMAP_CM_LOG("qmi_qcmap_msgr_indication_register");
  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_indication_register_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_indication_register_resp_msg_v01));
  rc = QMI_CSI_CB_INTERNAL_ERR;

  /*Atleast one of the optional TLVs should present - ADD new tlvs here*/
  /*Please include any new TLV in the check below*/
  if ((req_ptr == NULL) || ((!req_ptr->register_backhaul_status_indication_valid) && (!req_ptr->register_wwan_roaming_indication_valid)))
  {
    LOG_MSG_ERROR("NO registrations requested",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE;
    resp_msg.resp.error = QMI_ERR_MISSING_ARG_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_indication_register_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if (req_ptr->register_backhaul_status_indication_valid)
  {
    clnt_info->backhaul_status_ind_regd = req_ptr->register_backhaul_status_indication;
    LOG_MSG_INFO1 ("Client %p, reg %d for Backhaul status", clnt_info, clnt_info->backhaul_status_ind_regd,0);
  }
  if (req_ptr->register_wwan_roaming_indication_valid)
  {
    clnt_info->wwan_roaming_status_ind_regd = req_ptr->register_wwan_roaming_indication;
    LOG_MSG_INFO1 ("Client %p set Roaming status ind reg to %d ", clnt_info, clnt_info->wwan_roaming_status_ind_regd,0);
  }
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, sizeof(qcmap_msgr_indication_register_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}/* qmi_qcmap_msgr_indication_register() */


/*===========================================================================
  FUNCTION qmi_qcmap_msgr_get_wwan_roaming_status()

  DESCRIPTION
    Gets the current wwan roaming status

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
static qmi_csi_cb_error qmi_qcmap_msgr_get_wwan_roaming_status
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type    *qcmap_sp;
  qmi_error_type_v01 qmi_err_num;
  qcmap_msgr_get_wwan_roaming_status_resp_msg_v01 resp_msg;
  unsigned int     index;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qcmap_msgr_get_wwan_roaming_status_req_msg_v01  *req_ptr = NULL;
  uint8 roam_status;

  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);
  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_wwan_roaming_status_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p Get WWAN Roaming Status\n", clnt_info, 0, 0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_wwan_roaming_status_resp_msg_v01));

  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_get_wwan_roaming_status_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  /*get the current roaming status*/
  roam_status = qcmap_cm_get_wwan_roam_status();
  QCMAP_CM_LOG("Send roaming Status as :%d\n",roam_status);
  resp_msg.wwan_roaming_status = (qcmap_msgr_wwan_roaming_status_enum_v01)roam_status;

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_wwan_roaming_status_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}/*qmi_qcmap_msgr_get_wwan_roaming_status()*/

/*===========================================================================
FUNCTION QMI_QCMAP_MSGR_GET_LAN_BRIDGES()

DESCRIPTION
  Gets vector of all LAN bridges configured

PARAMETERS

RETURN VALUE

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_get_lan_bridges
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_qcmap_msgr_state_info_type       *qcmap_sp;
  qmi_qcmap_msgr_softap_handle_type                   *map_handle;
  qcmap_msgr_get_lan_bridges_resp_msg_v01  resp_msg;
  qcmap_msgr_get_lan_bridges_req_msg_v01  *req_ptr;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  int i = 0;
  int16_t                              bridge_vlan_id;
  QCMAP_LAN_Manager* lan_mgr = QCMAP_LAN_Manager::GetInstance();
  std::vector<int16_t> bridge_vlan_vec;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type*)service_handle;
  req_ptr = (qcmap_msgr_get_lan_bridges_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_lan_bridges_resp_msg_v01));

  /*---------------------------------------------------------------------
      Get LAN Bridges
  ---------------------------------------------------------------------*/
  if(lan_mgr)
  {
    if(clnt_info)
    {
      if(lan_mgr->GetLANBridge(clnt_info->bridge_vlan_id) == NULL)
      {
        clnt_info->bridge_vlan_id = 0; //assign to the default bridge0
      }
      resp_msg.bridge_list.curr_bridge = clnt_info->bridge_vlan_id;
    } else {
      LOG_MSG_INFO1("clnt_info ptr is NULL", 0, 0, 0);
    }

    LOG_MSG_INFO1("current selected bridge: %d", resp_msg.bridge_list.curr_bridge, 0, 0);

    resp_msg.bridge_list.num_of_bridges = lan_mgr->GetNumOfBridges();

    LOG_MSG_INFO1("There are %d bridges", resp_msg.bridge_list.num_of_bridges, 0, 0);

    bridge_vlan_vec = lan_mgr->GetBridgeIDs();
    for(std::vector<int16_t>::iterator it = bridge_vlan_vec.begin(); it != bridge_vlan_vec.end();
        ++it)
    {
      resp_msg.bridge_list.bridge_arr[i] = *it;
      LOG_MSG_INFO1("There is a bridge%d", resp_msg.bridge_list.bridge_arr[i], 0, 0);
      i++;
    }

  } else {

    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    LOG_MSG_ERROR("LAN Mgr is NULL", 0, 0, 0);
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_lan_bridges_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }


  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_lan_bridges_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
FUNCTION QMI_QCMAP_MSGR_SELECT_LAN_BRIDGE()

DESCRIPTION
  Sets LAN Bridge preference
  a) Switches the LAN Bridge context.
  b) All LAN API's called later-on will use LAN object set by this API.

PARAMETERS

RETURN VALUE

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
static qmi_csi_cb_error qmi_qcmap_msgr_select_lan_bridge
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  unsigned int                                         index;
  qmi_qcmap_msgr_softap_handle_type                   *map_handle;
  qcmap_msgr_select_lan_bridge_resp_msg_v01  resp_msg;
  qcmap_msgr_select_lan_bridge_req_msg_v01  *req_ptr;
  int16_t                              bridge_vlan_id;
  boolean            profile_switch_success = false;
  int i = 0;
  qmi_error_type_v01 qmi_err_num;
  QCMAP_LAN_Manager* lan_mgr = QCMAP_LAN_Manager::GetInstance();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  req_ptr = (qcmap_msgr_select_lan_bridge_req_msg_v01 *)req_c_struct;
  memset( &resp_msg, 0, sizeof(qcmap_msgr_select_lan_bridge_resp_msg_v01 ) );

  /*-------------------------------------------------------------------------
    Sanity check for qcmap handle
  --------------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if ( index != QCMAP_MSGR_SOFTAP_HANDLE ||
     ( map_handle = &qcmap_handle)->handle == 0 )
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_select_lan_bridge_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }


  if(lan_mgr)
  {
    if(lan_mgr->GetLANBridge(req_ptr->bridge_vlan_id) != NULL)
    {
      if(clnt_info)
      {
        clnt_info->bridge_vlan_id = req_ptr->bridge_vlan_id;
        LOG_MSG_INFO1("Set LAN Bridge context to: %d", clnt_info->bridge_vlan_id, 0, 0);
        qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                          sizeof(qcmap_msgr_select_lan_bridge_resp_msg_v01));
        return QMI_CSI_CB_NO_ERR;
      } else {
        LOG_MSG_ERROR("clnt_info is NULL", 0, 0, 0);
      }
    }  else {
      LOG_MSG_ERROR("Not a valid existing LAN Bridge: %d", req_ptr->bridge_vlan_id, 0, 0);
    }
  } else {
    LOG_MSG_ERROR("LAN Mgr is NULL", 0, 0, 0);
  }

  resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
  resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
  LOG_MSG_ERROR("Select LAN Bridge(%d) failed: %d", req_ptr->bridge_vlan_id,
                resp_msg.resp.error, 0);
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                  sizeof(qcmap_msgr_select_lan_bridge_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}
/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_SET_ALWAYS_ON_WLAN()

  DESCRIPTION
    Set always on wlan value.

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
static qmi_csi_cb_error qmi_qcmap_msgr_set_always_on_wlan
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_set_always_on_wlan_req_msg_v01 *req_ptr;
  qcmap_msgr_set_always_on_wlan_resp_msg_v01 resp_msg;
  unsigned int index;
  qmi_qcmap_msgr_softap_handle_type    *map_handle;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  boolean always_on_wlan_status = false;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_set_always_on_wlan_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p Set Always on WLAN", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_always_on_wlan_resp_msg_v01));

  /*---------------------------------------------------------------------
   Set Always on WLAN status
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
      (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_INFO1("Incorrect handle %d passed",
                  req_ptr->mobile_ap_handle,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_always_on_wlan_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  if (QCMAP_WLAN::GetAlwaysOnWLAN(&always_on_wlan_status, &qmi_err_num))
  {
    if (always_on_wlan_status != req_ptr->enable_always_on_wlan)
    {
      if (QCMAP_WLAN::SetAlwaysOnWLAN(req_ptr->enable_always_on_wlan,
                                          &qmi_err_num))
      {
        LOG_MSG_INFO1("Set Always on WLAN successfully.\n",0,0,0);
      }
      else
      {
        LOG_MSG_ERROR("Set Always on WLAN failed, error: %d",qmi_err_num,0,0);
        resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
        resp_msg.resp.error = qmi_err_num;
      }
    }
    else
    {
      LOG_MSG_ERROR("Always on WLAN is already set to %d",always_on_wlan_status,0,0);
      resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
      resp_msg.resp.error = QMI_ERR_NO_EFFECT_V01;
    }
  }
  else
  {
    LOG_MSG_ERROR("Get Always on WLAN failed, error: %d",qmi_err_num,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_set_always_on_wlan_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_set_always_on_wlan */

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_ALWAYS_ON_WLAN()

  DESCRIPTION
    Get Always on WLAN value .

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
static qmi_csi_cb_error qmi_qcmap_msgr_get_always_on_wlan
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_get_always_on_wlan_req_msg_v01 *req_ptr;
  qcmap_msgr_get_always_on_wlan_resp_msg_v01 resp_msg;
  boolean    always_on_wlan_status = false;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_always_on_wlan_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p Get Always on WLAN", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_always_on_wlan_resp_msg_v01));
  /*---------------------------------------------------------------------
  Get the Always on WLAN status
  ---------------------------------------------------------------------*/

  if (QCMAP_WLAN::GetAlwaysOnWLAN(&always_on_wlan_status, &qmi_err_num))
  {
    LOG_MSG_INFO1("Got Always on WLAN status successfully",0,0,0);
    resp_msg.always_on_wlan_status = always_on_wlan_status;
  }
  else
  {
    LOG_MSG_ERROR("Get Always on WLAN status Failed, error : %d",qmi_err_num,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_always_on_wlan_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_qcmap_msgr_get_always_on_wlan */


/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_SET_P2P_ROLE()

  DESCRIPTION
    Set Peer to peer role

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
 static qmi_csi_cb_error qmi_qcmap_msgr_set_p2p_role
 (
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                    *service_handle
 )
{
  LOG_MSG_INFO1("Entered function %s", __func__, 0, 0);
  qmi_qcmap_msgr_state_info_type  *qcmap_sp;

  qcmap_msgr_set_p2p_role_req_msg_v01 *req_ptr;
  qcmap_msgr_set_p2p_role_resp_msg_v01 resp_msg;
  unsigned int index;
  qmi_qcmap_msgr_softap_handle_type  *map_handle;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  boolean always_on_wlan_status = false;
  qcmap_p2p_config p2p_config;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_set_p2p_role_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p Set peer-to-peer", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_p2p_role_resp_msg_v01));
  memset(&p2p_config, 0, sizeof(qcmap_p2p_config));

  if (req_ptr == NULL)
  {
    LOG_MSG_ERROR("No Need to set p2p-role",0,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE;
    resp_msg.resp.error = QMI_ERR_MISSING_ARG_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_set_p2p_role_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

#ifndef FEATURE_MOBILEAP_APQ_PLATFORM
  LOG_MSG_ERROR("Unsupported target for P2P mode", 0, 0, 0);
  /*
  * P2P mode is currently enabled only for APQ Platform.For other target P2P mode is disabled.
  */
  resp_msg.resp.result = QMI_RESULT_FAILURE;
  resp_msg.resp.error = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
#else

  index = (req_ptr->mobile_ap_handle);
  if (index != QCMAP_MSGR_SOFTAP_HANDLE ||
    (map_handle = &qcmap_handle)->handle == 0)
  {
    LOG_MSG_ERROR("Incorrect handle %d passed",
    req_ptr->mobile_ap_handle, 0, 0 );
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                       sizeof(qcmap_msgr_set_p2p_role_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  /*---------------------------------------------------------------------
    Set peer-to-peer-role
  ---------------------------------------------------------------------*/

  p2p_config.p2p_status = req_ptr->p2p_status;

  if(req_ptr->p2p_status)
  {
    if(req_ptr->p2p_role_valid)
    {
      p2p_config.p2p_role_valid = TRUE;
      p2p_config.p2p_role = req_ptr->p2p_role;
    }
    else
    {
      LOG_MSG_ERROR(" Missing p2p_role argument",0, 0, 0);
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      resp_msg.resp.error = QMI_ERR_MISSING_ARG_V01;
      qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                     sizeof(qcmap_msgr_set_p2p_role_resp_msg_v01));
      return QMI_CSI_CB_NO_ERR;
    }
  }

  if (QCMAP_WLAN::set_p2p_role(p2p_config, &qmi_err_num))
  {
     LOG_MSG_INFO1("Set peer-to-peer role success",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Set peer-to-peer role failed, error: %d",qmi_err_num,0,0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
  }
#endif

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                     sizeof(qcmap_msgr_set_p2p_role_resp_msg_v01));

  LOG_MSG_INFO1("Exiting function %s", __func__, 0, 0);

  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_P2P_ROLE()

  DESCRIPTION
    Get Peer to peer role.

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
 static qmi_csi_cb_error qmi_qcmap_msgr_get_p2p_role
 (
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
 )
{
  LOG_MSG_INFO1("Entered function %s", __func__, 0, 0);
  qmi_qcmap_msgr_state_info_type  *qcmap_sp;
  qcmap_msgr_get_p2p_role_resp_msg_v01 resp_msg;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_get_p2p_role_req_msg_v01 *req_ptr;
  qcmap_p2p_config p2p_config;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&p2p_config, 0, sizeof(qcmap_p2p_config));

  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);
  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_p2p_role_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p Get P2P Role", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_p2p_role_resp_msg_v01));

#ifndef FEATURE_MOBILEAP_APQ_PLATFORM
  LOG_MSG_ERROR(" Unsupported target for P2P mode ", 0, 0, 0);
  /*
  * P2P mode is currently enabled only for APQ Platform. For other target P2P mode is disabled.
  */
  resp_msg.resp.result = QMI_RESULT_FAILURE;
  resp_msg.resp.error = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
#else
    /*---------------------------------------------------------------------
        Get peer-to peer status
    ---------------------------------------------------------------------*/
  if(QCMAP_WLAN::get_p2p_role(&p2p_config, &qmi_err_num))
  {
    LOG_MSG_INFO1("Get P2P role success!! P2P status: %d, P2P role: %d ",
                   p2p_config.p2p_status, p2p_config.p2p_role, 0);
  }
  else
  {
    LOG_MSG_ERROR("Could-not get p2p-config",0 ,0, 0);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = qmi_err_num;
  }
  resp_msg.p2p_status = p2p_config.p2p_status;
  resp_msg.p2p_role_valid = p2p_config.p2p_role_valid;
  resp_msg.p2p_role = p2p_config.p2p_role;

  LOG_MSG_INFO1("P2P status: %d, P2P role: %d ",resp_msg.p2p_status, resp_msg.p2p_role,0);
#endif
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(qcmap_msgr_get_p2p_role_resp_msg_v01));

  LOG_MSG_INFO1("Exiting function %s", __func__, 0, 0);

  return QMI_CSI_CB_NO_ERR;

}

/*===========================================================================
  FUNCTION QMI_QCMAP_MSGR_GET_WWAN_PROFILE_PREFERENCE()

  DESCRIPTION
    Gets WWAN Profile Preference.

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
static qmi_csi_cb_error qmi_qcmap_msgr_get_wwan_profile_preference
(
  qmi_qcmap_msgr_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{

  qmi_qcmap_msgr_state_info_type    *qcmap_sp;

  qcmap_msgr_get_wwan_profile_preference_req_msg_v01 *req_ptr;
  qcmap_msgr_get_wwan_profile_preference_resp_msg_v01 resp_msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(req_c_struct != NULL);
  ds_assert(req_c_struct_len > 0);
  ds_assert(clnt_info != NULL);
  ds_assert(service_handle != NULL);

  qcmap_sp = (qmi_qcmap_msgr_state_info_type *)service_handle;
  req_ptr = (qcmap_msgr_get_wwan_profile_preference_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1 ("Client %p Get WWAN Profile preference", clnt_info,0,0);

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_wwan_profile_preference_resp_msg_v01));
  /*---------------------------------------------------------------------
  Get Current Profile Handle
  ---------------------------------------------------------------------*/

  if (clnt_info->profileHandle == INVALID_PROFILE_HANDLE)
  {
    resp_msg.current_profile_handle = QCMAP_Backhaul::defaultProfileHandle;
  }
  else
  {
    resp_msg.current_profile_handle = clnt_info->profileHandle;
  }
  LOG_MSG_INFO1("Current profile handle : %d",resp_msg.current_profile_handle,0,0);

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(qcmap_msgr_get_wwan_profile_preference_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}
