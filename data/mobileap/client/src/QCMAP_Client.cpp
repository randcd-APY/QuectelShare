/*====================================================

FILE:  QCMAP_Client.cpp

SERVICES:
QCMAP Client Implementation

=====================================================

  Copyright (c) 2012-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  07/11/12   gk         Created module.
  10/26/12   cp         Added support for Dual AP and different types of NAT.
  02/27/13   cp         Added support to get IPV6 WAN status.
  04/17/13   mp         Added support to get IPv6 WWAN/STA mode configuration.
  06/12/13   sg         Added support for DHCP Reservation.
  09/17/13   at         Added support to Enable/Disable ALGs
  01/03/14   vm         Changes to support IoE on 9x25
  02/28/14   at         Added support to get IPV6 SIP server info.
  02/24/14   vm         Changes to Enable/Disable Station Mode in IoE 9x25 to
                        be in accordance with IoE 9x15
  01/05/15   rk         qtimap offtarget support.
  03/28/17   spr        Added support for Multi-PDN.
  ===========================================================================*/
#include <fstream>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/select.h>
#include <pthread.h>
#include "ds_util.h"
#include "ds_string.h"
#include "qualcomm_mobile_access_point_msgr_v01.h"
#include "QCMAP_Client.h"

#define QCMAP_MSGR_QMI_TIMEOUT_VALUE     90000
#define DEFAULT_PROFILE_HANDLE           0         /* Default Profile Handle for WWAN */

/*---------------------------------------------------------------------------
  Return values indicating error status
---------------------------------------------------------------------------*/
#define QCMAP_CM_SUCCESS               0         /* Successful operation   */
#define QCMAP_CM_ERROR                -1         /* Unsuccessful operation */
#define TRUE                           1

#define QCMAP_LOG(...)                         \
 LOG_MSG_INFO1( "%s %d:", __FILE__, __LINE__,0); \
 LOG_MSG_INFO1( __VA_ARGS__ ,0,0)

#define QCMAP_LOG_FUNC_ENTRY()  \
 QCMAP_LOG                   \
(                              \
       "Entering function %s\n",  \
       __FUNCTION__               \
)

#define QCMAP_LOG_FUNC_EXIT()   \
 QCMAP_LOG                   \
(                              \
       "Exiting function %s\n",   \
       __FUNCTION__ \
)

#define BZERO_QMI_MSG(qmi_msg) memset(&qmi_msg, 0, sizeof(qmi_msg))

void Dump_firewall_conf( qcmap_msgr_firewall_entry_conf_t *firewall_entry);

#ifdef __cplusplus
       extern "C"
       {
#endif

#ifdef __cplusplus
       }
#endif

/*===========================================================================
  FUNCTION  check_port
  ===========================================================================
  @brief
  Port value is validated against the range 1 - MAX_PORT_VALUE
  @input
  sport - port value
  @return
  0  - success
  -1 - failure
  @dependencies
  None
  @sideefects
  None
  =========================================================================*/
uint16_t check_port (uint32 sport)
{
  if((sport > MAX_PORT_VALUE) || (sport < 1) )
  {
    LOG_MSG_ERROR(" port value should be between 1 - %d\n",MAX_PORT_VALUE,0,0);
    return -1;
  }
  else
    return 0;
}


/*===========================================================================
  FUNCTION readable_addr
  ===========================================================================
  @brief
  converts the convert a numeric address into a text string suitable
  for presentation
  @input
  domain - identifies ipv4 or ipv6 domain
  addr   - contains the numeric address
  str    - this is an ouput value contains address in text string
  @return
  0  - success
  -1 - failure
  @dependencies
  It depends on inet_ntop()
  @sideefects
  None
  =========================================================================*/

int readable_addr(int domain, const uint32 *addr, char *str)
{
  if((addr!=NULL) && (str!=NULL))
  {
    if (inet_ntop(domain, (const char *)addr, str, INET6_ADDRSTRLEN) == NULL)
    {
      printf("\n Not in presentation format \n");
      return -1;
    }
  }

  return 0;
}

/*===================================================================
  Class Definitions
  ===================================================================*/

/*===========================================================================
  FUNCTION QCMAP_Client
  ===========================================================================
  @brief
  Initializes the Client by getting the service list and registers for
  WAN status and mobile ap status.
  @input
  void
  @return
  void
  @dependencies
  @sideefects
  None
  =========================================================================*/
QCMAP_Client::QCMAP_Client(client_status_ind_t client_cb_ind)
{
  qmi_idl_service_object_type qcmap_msgr_qmi_idl_service_object;
  uint32_t num_services = 0, num_entries = 0;
  qmi_service_info info[10];
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;

  QCMAP_LOG_FUNC_ENTRY();

  this->qcmap_msgr_enable = false;
  this->qmi_qcmap_msgr_handle = 0;
  this->mobile_ap_handle = 0;

#ifdef FEATURE_DATA_LOG_QXDM
  /* Initializing Diag for QXDM loga*/
  if (TRUE != Diag_LSM_Init(NULL))
  {
     printf("Diag_LSM_Init failed !!");
  }
#endif
  qcmap_msgr_qmi_idl_service_object = qcmap_msgr_get_service_object_v01();
  if (qcmap_msgr_qmi_idl_service_object == NULL)
  {
    LOG_MSG_ERROR("qcmap_msgr service object not available",0,0,0);
    return;
  }

  qmi_error = qmi_client_notifier_init(qcmap_msgr_qmi_idl_service_object,
                                       &this->qmi_qcmap_msgr_os_params,
                                       &this->qmi_qcmap_msgr_notifier);
  if (qmi_error < 0)
  {
    LOG_MSG_ERROR("qmi_client_notifier_init(qcmap_msgr) returned %d", qmi_error,0,0);
    return;
  }

  /* Check if the service is up, if not wait on a signal */
  while(1)
  {
    qmi_error = qmi_client_get_service_list(qcmap_msgr_qmi_idl_service_object,
                                            NULL,
                                            NULL,
                                            &num_services);
    LOG_MSG_ERROR("qmi_client_get_service_list: %d",qmi_error,0,0);

    if(qmi_error == QMI_NO_ERR)
      break;
    /* wait for server to come up */
    QMI_CCI_OS_SIGNAL_WAIT(&this->qmi_qcmap_msgr_os_params, 0);
  }

  num_entries = num_services;

  LOG_MSG_INFO1("qmi_client_get_service_list: num_e %d num_s %d",
      num_entries, num_services,0);
  /* The server has come up, store the information in info variable */
  qmi_error = qmi_client_get_service_list(qcmap_msgr_qmi_idl_service_object,
                                          info,
                                          &num_entries,
                                          &num_services);

  LOG_MSG_INFO1("qmi_client_get_service_list: num_e %d num_s %d error %d",
      num_entries, num_services, qmi_error);

  if (qmi_error != QMI_NO_ERR)
  {
    qmi_client_release(this->qmi_qcmap_msgr_notifier);
    this->qmi_qcmap_msgr_notifier = NULL;
    LOG_MSG_ERROR("Can not get qcmap_msgr service list %d",
        qmi_error,0,0);
    return;
  }

  qmi_error = qmi_client_init(&info[0],
                              qcmap_msgr_qmi_idl_service_object,
                              client_cb_ind,
                              NULL,
                              NULL,
                              &this->qmi_qcmap_msgr_handle);

  LOG_MSG_INFO1("qmi_client_init: %d",
                 qmi_error,0,0);

  if (qmi_error != QMI_NO_ERR)
  {
    qmi_client_release(this->qmi_qcmap_msgr_notifier);
    this->qmi_qcmap_msgr_notifier = NULL;
    LOG_MSG_ERROR("Can not init qcmap_msgr client %d",
        qmi_error,0,0);
    return;
  }

  return;
}

/*===========================================================================
  FUNCTION QCMAP_Client clean up
  ===========================================================================
  @brief
   Distructor for client object
  @input
  void
  @return
  void
  @dependencies
  @sideefects
  None
  =========================================================================*/
QCMAP_Client::~QCMAP_Client()
{
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  qmi_error = qmi_client_release(this->qmi_qcmap_msgr_notifier);
  this->qmi_qcmap_msgr_notifier = NULL;

  if (qmi_error != QMI_NO_ERR)
  {
    LOG_MSG_ERROR("Can not release client qcmap notifier %d",qmi_error,0,0);
  }

  qmi_error = qmi_client_release(this->qmi_qcmap_msgr_handle);
  this->qmi_qcmap_msgr_handle = NULL;

  if (qmi_error != QMI_NO_ERR)
  {
    LOG_MSG_ERROR("Can not release client qcmap handle %d",
        qmi_error,0,0);
  }
}

/*===========================================================================
  FUNCTION EnableMobileAP
  ===========================================================================*/
/*!
  @brief
  Enables the mobileap

  @return
  void

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::EnableMobileAP(qmi_error_type_v01 *qmi_err_num)
{
  return QCMAP_Client::EnableMobileAP_Ext(qmi_err_num, 0xFFFF);
}

/*===========================================================================
  FUNCTION EnableMobileAP_Ext
  ===========================================================================*/
/*!
  @brief
  Enables the mobileap

  @return
  void

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::EnableMobileAP_Ext(qmi_error_type_v01 *qmi_err_num, uint64_t ind_reg_mask)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_mobile_ap_enable_resp_msg_v01 qcmap_enable_resp_msg_v01;
  qcmap_msgr_mobile_ap_status_ind_register_req_msg_v01 qcmap_mobile_ap_status_ind_reg;
  qcmap_msgr_wwan_status_ind_register_req_msg_v01 wwan_status_ind_reg;
  qcmap_msgr_station_mode_status_ind_register_req_msg_v01 qcmap_station_mode_status_ind_reg;
  qcmap_msgr_cradle_mode_status_ind_register_req_msg_v01 qcmap_cradle_mode_status_ind_reg;
  qcmap_msgr_mobile_ap_status_ind_register_resp_msg_v01 qcmap_mobile_ap_status_ind_rsp;
  qcmap_msgr_wwan_status_ind_register_resp_msg_v01 wwan_status_ind_rsp;
  qcmap_msgr_station_mode_status_ind_register_resp_msg_v01 qcmap_station_mode_status_ind_rsp;
  qcmap_msgr_cradle_mode_status_ind_register_resp_msg_v01 qcmap_cradle_mode_status_ind_rsp;
  qcmap_msgr_ethernet_mode_status_ind_register_req_msg_v01
                                         qcmap_ethernet_mode_status_ind_reg;
  qcmap_msgr_ethernet_mode_status_ind_register_resp_msg_v01
                                         qcmap_ethernet_mode_status_ind_rsp;
  qcmap_msgr_bt_tethering_status_ind_register_req_msg_v01 qcmap_bt_tethering_status_ind_reg;
  qcmap_msgr_bt_tethering_status_ind_register_resp_msg_v01 qcmap_bt_tethering_status_ind_rsp;
  qcmap_msgr_bt_tethering_wan_ind_register_req_msg_v01 qcmap_bt_tethering_wan_ind_reg;
  qcmap_msgr_bt_tethering_wan_ind_register_resp_msg_v01 qcmap_bt_tethering_wan_ind_rsp;
  qcmap_msgr_indication_register_req_msg_v01 qcmap_ind_reg;
  qcmap_msgr_indication_register_resp_msg_v01 qcmap_ind_rsp;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&qcmap_enable_resp_msg_v01, 0, sizeof(qcmap_msgr_mobile_ap_enable_resp_msg_v01));
  memset(&wwan_status_ind_reg, 0, sizeof(qcmap_msgr_wwan_status_ind_register_req_msg_v01));
  memset(&wwan_status_ind_rsp, 0, sizeof(qcmap_msgr_wwan_status_ind_register_resp_msg_v01));

  wwan_status_ind_reg.register_indication = 1;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                         QMI_QCMAP_MSGR_WWAN_STATUS_IND_REG_REQ_V01,
                                         (void*)&wwan_status_ind_reg,
                                         sizeof(qcmap_msgr_wwan_status_ind_register_req_msg_v01),
                                         (void*)&wwan_status_ind_rsp,
                                         sizeof(qcmap_msgr_wwan_status_ind_register_resp_msg_v01),
                                         QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_ERROR("qmi_client_send_msg_sync: error %d result %d",
                 qmi_error, wwan_status_ind_rsp.resp.result,0);

  if (( qmi_error == QMI_TIMEOUT_ERR ) ||
      ( qmi_error != QMI_NO_ERR ) ||
      ( wwan_status_ind_rsp.resp.result != QMI_NO_ERR ))
  {
    LOG_MSG_ERROR("Can not register for wwan status %d : %d",
                   qmi_error, wwan_status_ind_rsp.resp.error,0);
    *qmi_err_num = wwan_status_ind_rsp.resp.error;
    return false;
  }
  LOG_MSG_INFO1("Registered for wwan status",0,0,0);

  memset(&qcmap_mobile_ap_status_ind_reg, 0, sizeof(qcmap_msgr_mobile_ap_status_ind_register_req_msg_v01));
  memset(&qcmap_mobile_ap_status_ind_rsp, 0, sizeof(qcmap_msgr_mobile_ap_status_ind_register_resp_msg_v01));
  qcmap_mobile_ap_status_ind_reg.register_indication = 1;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_MOBILE_AP_STATUS_IND_REG_REQ_V01,
                                       (void*)&qcmap_mobile_ap_status_ind_reg,
                                       sizeof(qcmap_msgr_mobile_ap_status_ind_register_req_msg_v01),
                                       (void*)&qcmap_mobile_ap_status_ind_rsp,
                                       sizeof(qcmap_msgr_mobile_ap_status_ind_register_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync: error %d result %d",
                 qmi_error, qcmap_mobile_ap_status_ind_rsp.resp.result,0);
  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( qcmap_mobile_ap_status_ind_rsp.resp.result != QMI_NO_ERR ))
  {
    LOG_MSG_ERROR("Can not register for mobile ap status %d : %d",
                   qmi_error, qcmap_mobile_ap_status_ind_rsp.resp.error,0);
    *qmi_err_num = qcmap_mobile_ap_status_ind_rsp.resp.error;
    return false;
  }
  LOG_MSG_INFO1("Registered for mobile ap status",0,0,0);

  memset(&qcmap_station_mode_status_ind_reg, 0, sizeof(qcmap_msgr_station_mode_status_ind_register_req_msg_v01));
  memset(&qcmap_station_mode_status_ind_rsp, 0, sizeof(qcmap_msgr_station_mode_status_ind_register_resp_msg_v01));
  qcmap_station_mode_status_ind_reg.register_indication = 1;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
               QMI_QCMAP_MSGR_STATION_MODE_STATUS_IND_REG_REQ_V01,
               (void*)&qcmap_station_mode_status_ind_reg,
               sizeof(qcmap_msgr_station_mode_status_ind_register_req_msg_v01),
               (void*)&qcmap_station_mode_status_ind_rsp,
               sizeof(qcmap_msgr_station_mode_status_ind_register_resp_msg_v01),
               QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync: error %d result %d",
     qmi_error, qcmap_station_mode_status_ind_rsp.resp.result,0);
  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( qcmap_station_mode_status_ind_rsp.resp.result != QMI_NO_ERR ))
  {
    LOG_MSG_ERROR("Can not register for station mode indications %d : %d",
                   qmi_error, qcmap_station_mode_status_ind_rsp.resp.error,0);
    *qmi_err_num = qcmap_mobile_ap_status_ind_rsp.resp.error;
     return false;
  }
  LOG_MSG_INFO1("Registered for station mode status",0,0,0);

  memset(&qcmap_cradle_mode_status_ind_reg, 0,
         sizeof(qcmap_msgr_cradle_mode_status_ind_register_req_msg_v01));
  memset(&qcmap_cradle_mode_status_ind_rsp, 0,
         sizeof(qcmap_msgr_cradle_mode_status_ind_register_resp_msg_v01));
  qcmap_cradle_mode_status_ind_reg.register_indication =
                                      QCMAP_MSGR_CRADLE_STATUS_IND_REG_V01;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
               QMI_QCMAP_MSGR_CRADLE_MODE_STATUS_IND_REG_REQ_V01,
               (void*)&qcmap_cradle_mode_status_ind_reg,
               sizeof(qcmap_msgr_cradle_mode_status_ind_register_req_msg_v01),
               (void*)&qcmap_cradle_mode_status_ind_rsp,
               sizeof(qcmap_msgr_cradle_mode_status_ind_register_resp_msg_v01),
               QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync: error %d result %d",
     qmi_error, qcmap_cradle_mode_status_ind_rsp.resp.result,0);
  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( qcmap_cradle_mode_status_ind_rsp.resp.result != QMI_NO_ERR ))
  {
    LOG_MSG_ERROR("Can not register for cradle mode indications %d : %d",
                   qmi_error, qcmap_cradle_mode_status_ind_rsp.resp.error,0);
    *qmi_err_num = qcmap_cradle_mode_status_ind_rsp.resp.error;
     return false;
  }
  LOG_MSG_INFO1("Registered for cradle mode status",0,0,0);

  memset(&qcmap_ethernet_mode_status_ind_reg, 0,
         sizeof(qcmap_msgr_ethernet_mode_status_ind_register_req_msg_v01));
  memset(&qcmap_ethernet_mode_status_ind_rsp, 0,
         sizeof(qcmap_msgr_ethernet_mode_status_ind_register_resp_msg_v01));
  qcmap_ethernet_mode_status_ind_reg.register_indication =
                                     QCMAP_MSGR_ETHERNET_STATUS_IND_REG_V01;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
               QMI_QCMAP_MSGR_ETHERNET_MODE_STATUS_IND_REG_REQ_V01,
               (void*)&qcmap_ethernet_mode_status_ind_reg,
           sizeof(qcmap_msgr_ethernet_mode_status_ind_register_req_msg_v01),
               (void*)&qcmap_ethernet_mode_status_ind_rsp,
          sizeof(qcmap_msgr_ethernet_mode_status_ind_register_resp_msg_v01),
               QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync: error %d result %d",
     qmi_error, qcmap_ethernet_mode_status_ind_rsp.resp.result,0);
  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( qcmap_ethernet_mode_status_ind_rsp.resp.result != QMI_NO_ERR ))
  {
    LOG_MSG_ERROR("Cannot register for Ethernet mode indications %d : %d",
                qmi_error, qcmap_ethernet_mode_status_ind_rsp.resp.error,0);
    *qmi_err_num = qcmap_ethernet_mode_status_ind_rsp.resp.error;
     return false;
  }
  LOG_MSG_INFO1("Registered for Ethernet mode status",0,0,0);

  /*Register for BT Tethering Indication*/

  memset(&qcmap_bt_tethering_status_ind_reg, 0,
         sizeof(qcmap_msgr_bt_tethering_status_ind_register_req_msg_v01));
  memset(&qcmap_bt_tethering_status_ind_rsp, 0,
         sizeof(qcmap_msgr_bt_tethering_status_ind_register_resp_msg_v01));
  qcmap_bt_tethering_status_ind_reg.register_indication =
                                      QCMAP_MSGR_BT_TETHERING_STATUS_IND_REG_V01;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
               QMI_QCMAP_MSGR_BT_TETHERING_STATUS_IND_REG_REQ_V01,
               (void*)&qcmap_bt_tethering_status_ind_reg,
               sizeof(qcmap_msgr_bt_tethering_status_ind_register_req_msg_v01),
               (void*)&qcmap_bt_tethering_status_ind_rsp,
               sizeof(qcmap_msgr_bt_tethering_status_ind_register_resp_msg_v01),
               QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync: error %d result %d",
     qmi_error, qcmap_bt_tethering_status_ind_rsp.resp.result,0);
  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( qcmap_bt_tethering_status_ind_rsp.resp.result != QMI_NO_ERR ))
  {
    LOG_MSG_ERROR("Can not register for BT tethering indications %d : %d",
                   qmi_error, qcmap_bt_tethering_status_ind_rsp.resp.error,0);
    *qmi_err_num = qcmap_bt_tethering_status_ind_rsp.resp.error;
     return false;
  }
  LOG_MSG_INFO1("Registered for BT tethering Indication",0,0,0);
  /*End Register for BT Tethering Indication*/

 /*Register for BT Tethering WAN Indication*/

  memset(&qcmap_bt_tethering_wan_ind_reg, 0,
         sizeof(qcmap_msgr_bt_tethering_wan_ind_register_req_msg_v01));
  memset(&qcmap_bt_tethering_wan_ind_rsp, 0,
         sizeof(qcmap_msgr_bt_tethering_wan_ind_register_resp_msg_v01));
  qcmap_bt_tethering_wan_ind_reg.register_indication =
                                      QCMAP_MSGR_BT_TETHERING_WAN_IND_REG_V01;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
               QMI_QCMAP_MSGR_BT_TETHERING_WAN_IND_REG_REQ_V01,
               (void*)&qcmap_bt_tethering_wan_ind_reg,
               sizeof(qcmap_msgr_bt_tethering_wan_ind_register_req_msg_v01),
               (void*)&qcmap_bt_tethering_wan_ind_rsp,
               sizeof(qcmap_msgr_bt_tethering_wan_ind_register_resp_msg_v01),
               QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync: error %d result %d",
     qmi_error, qcmap_bt_tethering_wan_ind_rsp.resp.result,0);
  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( qcmap_bt_tethering_wan_ind_rsp.resp.result != QMI_NO_ERR ))
  {
    LOG_MSG_ERROR("Can not register for BT tethering indications %d : %d",
                   qmi_error, qcmap_bt_tethering_wan_ind_rsp.resp.error,0);
    *qmi_err_num = qcmap_bt_tethering_wan_ind_rsp.resp.error;
     return false;
  }
  LOG_MSG_INFO1("Registered for BT tethering Indication",0,0,0);
  /*End Register for BT Tethering Indication*/

 /*Register for Indication Register */

  memset(&qcmap_ind_reg, 0,
         sizeof(qcmap_msgr_indication_register_req_msg_v01));
  memset(&qcmap_ind_rsp, 0,
         sizeof(qcmap_msgr_indication_register_resp_msg_v01));
  /*Check if atlease one indication is registered or not*/
  if(ind_reg_mask > 0)
  {
    /*Check which ind regsitraion is enabled and include that TLV here*/
    if(ind_reg_mask | BACKHAUL_STATUS_IND)
    {
      /*Register for Backhaul status Indication*/
      qcmap_ind_reg.register_backhaul_status_indication_valid = TRUE;
      qcmap_ind_reg.register_backhaul_status_indication = TRUE;
    }

    if(ind_reg_mask | WWAN_ROAMING_STATUS_IND)
    {
      /*Register for WWAN Roaming status Indication*/
      qcmap_ind_reg.register_wwan_roaming_indication_valid = TRUE;
      qcmap_ind_reg.register_wwan_roaming_indication = TRUE;
    }

    qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                 QMI_QCMAP_MSGR_INDICATION_REGISTER_REQ_V01,
                 (void*)&qcmap_ind_reg,
                 sizeof(qcmap_msgr_indication_register_req_msg_v01),
                 (void*)&qcmap_ind_rsp,
                 sizeof(qcmap_msgr_indication_register_resp_msg_v01),
                 QCMAP_MSGR_QMI_TIMEOUT_VALUE);

    LOG_MSG_INFO1("qmi_client_send_msg_sync: error %d result %d",
       qmi_error, qcmap_ind_rsp.resp.result,0);
    if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
         ( qmi_error != QMI_NO_ERR ) ||
         ( qcmap_ind_rsp.resp.result != QMI_NO_ERR ))
    {
      LOG_MSG_ERROR("Can not register for indications %d : %d",
                     qmi_error, qcmap_ind_rsp.resp.error,0);
      *qmi_err_num = qcmap_ind_rsp.resp.error;
       return false;
    }
    LOG_MSG_INFO1("Registered for Indications",0,0,0);
  }
  /*End Register for Indication Register*/

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_MOBILE_AP_ENABLE_REQ_V01,
                                       NULL,
                                       0,
                                       (void*)&qcmap_enable_resp_msg_v01,
                                       sizeof(qcmap_msgr_mobile_ap_enable_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);
  LOG_MSG_INFO1("qmi_client_send_msg_sync: error %d result %d valid %d",
                 qmi_error, qcmap_enable_resp_msg_v01.resp.result, qcmap_enable_resp_msg_v01.mobile_ap_handle_valid);
  if (( qmi_error == QMI_TIMEOUT_ERR ) ||
      ( qmi_error != QMI_NO_ERR ) ||
      ( qcmap_enable_resp_msg_v01.resp.result != QMI_NO_ERR) ||
      ( qcmap_enable_resp_msg_v01.mobile_ap_handle_valid != TRUE ))
  {
    LOG_MSG_ERROR("Can not enable qcmap %d : %d",
        qmi_error, qcmap_enable_resp_msg_v01.resp.error,0);
    *qmi_err_num = qcmap_enable_resp_msg_v01.resp.error;
    return false;
  }

  if( qcmap_enable_resp_msg_v01.mobile_ap_handle > 0 )
  {
    this->mobile_ap_handle = qcmap_enable_resp_msg_v01.mobile_ap_handle;
    this->qcmap_msgr_enable = true;
    LOG_MSG_INFO1("QCMAP Enabled\n",0,0,0);
    return true;
  }
  else
  {
    LOG_MSG_INFO1("QCMAP Enable Failure\n",0,0,0);
  }

  return false;
}

/*===========================================================================
  FUNCTION DisableMobileAP
  ===========================================================================*/
/*!
  @brief
  Disables the mobileap

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::DisableMobileAP(qmi_error_type_v01 *qmi_err_num)
{
  qcmap_msgr_mobile_ap_disable_req_msg_v01 qcmap_disable_req_msg_v01;
  qcmap_msgr_mobile_ap_disable_resp_msg_v01 qcmap_disable_resp_msg_v01;
  qmi_client_error_type qmi_error = QMI_NO_ERR;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&qcmap_disable_req_msg_v01, 0, sizeof(qcmap_msgr_mobile_ap_disable_req_msg_v01));
  memset(&qcmap_disable_resp_msg_v01, 0, sizeof(qcmap_msgr_mobile_ap_disable_resp_msg_v01));

  if (!this->qcmap_msgr_enable)
  {
    /* QCMAP is not enabled */
    LOG_MSG_INFO1("QCMAP not enabled\n",0,0,0);
    return false;
  }

  qcmap_disable_req_msg_v01.mobile_ap_handle = this->mobile_ap_handle;
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_MOBILE_AP_DISABLE_REQ_V01,
                                       &qcmap_disable_req_msg_v01,
                                       sizeof(qcmap_msgr_mobile_ap_disable_req_msg_v01),
                                       &qcmap_disable_resp_msg_v01,
                                       sizeof(qcmap_msgr_mobile_ap_disable_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( (qcmap_disable_resp_msg_v01.resp.error != QMI_ERR_NO_EFFECT_V01 &&
          qcmap_disable_resp_msg_v01.resp.error != QMI_ERR_NONE_V01)) ||
       ( qcmap_disable_resp_msg_v01.resp.result != QMI_NO_ERR ))
  {
    LOG_MSG_ERROR( "Can not disable qcmap %d : %d",
        qmi_error, qcmap_disable_resp_msg_v01.resp.error,0);
    *qmi_err_num = qcmap_disable_resp_msg_v01.resp.error;
    return false;
  }

  /*.If backhaul is not connected, Mobileap will be disabled instantly. And since
     call back function is being called much before the response pending flag is set to TRUE,
     responses are not sent to the client.
     Hence, we set qcmap_disable_resp_msg_v01.resp.error to QMI_ERR_NO_EFFECT_V01
     So that the caller of this function sends a response back to the client. (Used for IoE 9x25)
    */
  if (qcmap_disable_resp_msg_v01.resp.error == QMI_ERR_NO_EFFECT_V01)
    *qmi_err_num = qcmap_disable_resp_msg_v01.resp.error;

  this->mobile_ap_handle = 0;
  this->qcmap_msgr_enable = false;
  return true;
}

/*===========================================================================
FUNCTION SetWWANProfileHandle()
===========================================================================*/
/*!
  @brief
  Set profile_handle to be used for WWAN API's.

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::SetWWANProfileHandlePreference
(
  profile_handle_type_v01  profile_handle,
  qmi_error_type_v01      *qmi_err_num
)
{
  qcmap_msgr_set_wwan_profile_preference_req_msg_v01   req_msg;
  qcmap_msgr_set_wwan_profile_preference_resp_msg_v01  resp_msg;
  qmi_client_error_type                                qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  BZERO_QMI_MSG(req_msg);
  BZERO_QMI_MSG(resp_msg);
  req_msg.mobile_ap_handle = this->mobile_ap_handle;
  req_msg.profile_handle = profile_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_WWAN_PROFILE_PREFERENCE_REQ_V01,
                                       &req_msg,
                                       sizeof(qcmap_msgr_set_wwan_profile_preference_req_msg_v01),
                                       &resp_msg,
                                       sizeof(qcmap_msgr_set_wwan_profile_preference_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Cannot switch profile(%d) error: %d", profile_handle, resp_msg.resp.error, 0);
    if (qmi_err_num != NULL)
      *qmi_err_num = resp_msg.resp.error;
    return false;
  }

  return true;
}

/*===========================================================================
  FUNCTION EnableIPV4
  ===========================================================================*/
/*!
  @brief
  Enables IPV4 Functionality.

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::EnableIPV4(qmi_error_type_v01 *qmi_err_num)
{
  int qcmap_msgr_errno;
  int ret = QCMAP_CM_SUCCESS;
  qcmap_msgr_enable_ipv4_req_msg_v01 qcmap_enable_ipv4_req_msg;
  qcmap_msgr_enable_ipv4_resp_msg_v01 qcmap_enable_ipv4_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();


  /* Enable IPV4. */
  LOG_MSG_INFO1("Enable IPV4",0,0,0);
  qcmap_enable_ipv4_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_ENABLE_IPV4_REQ_V01,
                                       &qcmap_enable_ipv4_req_msg,
                                       sizeof(qcmap_msgr_enable_ipv4_req_msg_v01),
                                       &qcmap_enable_ipv4_resp_msg,
                                       sizeof(qcmap_msgr_enable_ipv4_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( qcmap_enable_ipv4_resp_msg.resp.result != QMI_NO_ERR ))
  {
    LOG_MSG_ERROR("Can not enable ipv4 %d : %d",
        qmi_error, qcmap_enable_ipv4_resp_msg.resp.error,0);
    *qmi_err_num = qcmap_enable_ipv4_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("Enabled IPV4...",0,0,0);
  return true;
}

/*===========================================================================
  FUNCTION DisableIPV4
  ===========================================================================*/
/*!
  @brief
  Enables IPV4 Functionality.

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::DisableIPV4(qmi_error_type_v01 *qmi_err_num)
{
  int qcmap_msgr_errno;
  int ret = QCMAP_CM_SUCCESS;
  qcmap_msgr_disable_ipv4_req_msg_v01 qcmap_disable_ipv4_req_msg;
  qcmap_msgr_disable_ipv4_resp_msg_v01 qcmap_disable_ipv4_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();


  /* Disable IPV4. */
  LOG_MSG_INFO1("Disable IPV4",0,0,0);
  qcmap_disable_ipv4_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_DISABLE_IPV4_REQ_V01,
                                       &qcmap_disable_ipv4_req_msg,
                                       sizeof(qcmap_msgr_disable_ipv4_req_msg_v01),
                                       &qcmap_disable_ipv4_resp_msg,
                                       sizeof(qcmap_msgr_disable_ipv4_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( qcmap_disable_ipv4_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not disable ipv4 %d : %d",
        qmi_error, qcmap_disable_ipv4_resp_msg.resp.error,0);
    *qmi_err_num = qcmap_disable_ipv4_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("Disabled IPV4...",0,0,0);
  return true;
}

/*===========================================================================
  FUNCTION EnableIPV6
  ===========================================================================*/
/*!
  @brief
  Enables IPV6 Functionality.

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::EnableIPV6(qmi_error_type_v01 *qmi_err_num)
{
  int qcmap_msgr_errno;
  int ret = QCMAP_CM_SUCCESS;
  qcmap_msgr_enable_ipv6_req_msg_v01 qcmap_enable_ipv6_req_msg;
  qcmap_msgr_enable_ipv6_resp_msg_v01 qcmap_enable_ipv6_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();


  /* Enable IPV6. */
  LOG_MSG_INFO1("Enable IPV6",0,0,0);
  qcmap_enable_ipv6_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_ENABLE_IPV6_REQ_V01,
                                       &qcmap_enable_ipv6_req_msg,
                                       sizeof(qcmap_msgr_enable_ipv6_req_msg_v01),
                                       &qcmap_enable_ipv6_resp_msg,
                                       sizeof(qcmap_msgr_enable_ipv6_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( qcmap_enable_ipv6_resp_msg.resp.result != QMI_NO_ERR ))
  {
    LOG_MSG_ERROR("Can not enable ipv6 %d : %d",
        qmi_error, qcmap_enable_ipv6_resp_msg.resp.error,0);
    *qmi_err_num = qcmap_enable_ipv6_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("Enabled IPV6...",0,0,0);
  return true;
}

/*===========================================================================
  FUNCTION DisableIPV6
  ===========================================================================*/
/*!
  @brief
  Enables IPV6 Functionality.

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::DisableIPV6(qmi_error_type_v01 *qmi_err_num)
{
  int qcmap_msgr_errno;
  int ret = QCMAP_CM_SUCCESS;
  qcmap_msgr_disable_ipv6_req_msg_v01 qcmap_disable_ipv6_req_msg;
  qcmap_msgr_disable_ipv6_resp_msg_v01 qcmap_disable_ipv6_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();


  /* Enable IPV6. */
  LOG_MSG_INFO1("Disable IPV6",0,0,0);
  qcmap_disable_ipv6_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_DISABLE_IPV6_REQ_V01,
                                       &qcmap_disable_ipv6_req_msg,
                                       sizeof(qcmap_msgr_disable_ipv6_req_msg_v01),
                                       &qcmap_disable_ipv6_resp_msg,
                                       sizeof(qcmap_msgr_disable_ipv6_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( qcmap_disable_ipv6_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not disable ipv6 %d : %d",
        qmi_error, qcmap_disable_ipv6_resp_msg.resp.error,0);
    *qmi_err_num = qcmap_disable_ipv6_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("Disabled IPV6...",0,0,0);
  return true;
}

/*==========================================================================
 FUNCTION EnableSTAMode()
===========================================================================*/
/*!
  @brief
  Enables WLAN in STA-Only mode.
  This is only to be used internally by eCNE module and other QCMAP Clients
  should not use this API.

  @return
  true  - on Success
  false - on Failure

  qmi_error_type_v01
  QMI_ERR_NO_EFFECT_V01 - WLAN is already Enabled
  QMI_ERR_NONE_V01      - Success
  QMI_ERR_INTERNAL_V01  - Failure.

  @note

  - Dependencies
  - QCMobileAP must be enabled.

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::EnableSTAMode(qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_enable_sta_mode_req_msg_v01 enable_sta_mode_req_msg;
  qcmap_msgr_enable_sta_mode_resp_msg_v01 enable_sta_mode_resp_msg;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&enable_sta_mode_req_msg, 0, sizeof(qcmap_msgr_enable_sta_mode_req_msg_v01));
  memset(&enable_sta_mode_resp_msg, 0, sizeof(qcmap_msgr_enable_sta_mode_resp_msg_v01));

  enable_sta_mode_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_ENABLE_STA_MODE_REQ_V01,
                                       &enable_sta_mode_req_msg,
                                       sizeof(qcmap_msgr_enable_sta_mode_req_msg_v01),
                                       (void *)&enable_sta_mode_resp_msg,
                                       sizeof(qcmap_msgr_enable_sta_mode_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(enable): error %d result %d",
      qmi_error, enable_sta_mode_resp_msg.resp.result,0);

  if ( (qmi_error == QMI_TIMEOUT_ERR) ||
       (qmi_error != QMI_NO_ERR) ||
       (enable_sta_mode_resp_msg.resp.result != QMI_NO_ERR) )
  {
    LOG_MSG_ERROR("Cannot enable WLAN STA Mode %d : %d",
        qmi_error, enable_sta_mode_resp_msg.resp.error,0);
    *qmi_err_num = enable_sta_mode_resp_msg.resp.error;
    return false;
  }
  return true;
}

/*==========================================================================
 FUNCTION DisableSTAMode()
===========================================================================*/
/*!
  @brief
  Disables WLAN in STA-Only mode.
  This is only to be used internally by eCNE module and other QCMAP Clients
  should not use this API.

  @return
  true  - on Success
  false - on Failure

  qmi_error_type_v01
  QMI_ERR_NO_EFFECT_V01 - WLAN is already Enabled
  QMI_ERR_NONE_V01      - Success
  QMI_ERR_INTERNAL_V01  - Failure.

  @note

  - Dependencies
  - QCMobileAP must be enabled.

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::DisableSTAMode(qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_disable_sta_mode_req_msg_v01 disable_sta_mode_req_msg;
  qcmap_msgr_disable_sta_mode_resp_msg_v01 disable_sta_mode_resp_msg;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&disable_sta_mode_req_msg, 0, sizeof(qcmap_msgr_disable_sta_mode_req_msg_v01));
  memset(&disable_sta_mode_resp_msg, 0, sizeof(qcmap_msgr_disable_sta_mode_resp_msg_v01));

  disable_sta_mode_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_DISABLE_STA_MODE_REQ_V01,
                                       &disable_sta_mode_req_msg,
                                       sizeof(qcmap_msgr_disable_sta_mode_req_msg_v01),
                                       (void *)&disable_sta_mode_resp_msg,
                                       sizeof(qcmap_msgr_disable_sta_mode_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(disable sta mode): error %d result %d",
      qmi_error, disable_sta_mode_resp_msg.resp.result,0);

  if ( (qmi_error == QMI_TIMEOUT_ERR) ||
       (qmi_error != QMI_NO_ERR) ||
       (disable_sta_mode_resp_msg.resp.result != QMI_NO_ERR) )
  {
    LOG_MSG_ERROR("Cannot disable WLAN STA Mode %d : %d",
        qmi_error, disable_sta_mode_resp_msg.resp.error,0);
    *qmi_err_num = disable_sta_mode_resp_msg.resp.error;
    return false;
  }
  return true;
}

/*===========================================================================
 FUNCTION RegisterForWLANStatusIND()
 ===========================================================================*/
/*!
  @brief
  This is used to register for WLAN status Indications

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
*/
/*=========================================================================*/
boolean QCMAP_Client::RegisterForWLANStatusIND(qmi_error_type_v01 *qmi_err_num, boolean register_indication)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_wlan_status_ind_register_req_msg_v01 reg_wlan_status_ind_req_msg;
  qcmap_msgr_wlan_status_ind_register_resp_msg_v01 reg_wlan_status_ind_resp_msg;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&reg_wlan_status_ind_req_msg, 0, sizeof(qcmap_msgr_wlan_status_ind_register_req_msg_v01));
  memset(&reg_wlan_status_ind_resp_msg, 0, sizeof(qcmap_msgr_wlan_status_ind_register_req_msg_v01));

  reg_wlan_status_ind_req_msg.register_indication = register_indication;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_WLAN_STATUS_IND_REG_REQ_V01,
                                       &reg_wlan_status_ind_req_msg,
                                       sizeof(qcmap_msgr_wlan_status_ind_register_req_msg_v01),
                                       (void*)&reg_wlan_status_ind_resp_msg,
                                       sizeof(qcmap_msgr_wlan_status_ind_register_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(enable): error %d result %d",
      qmi_error, reg_wlan_status_ind_resp_msg.resp.result,0);

  if ( (qmi_error == QMI_TIMEOUT_ERR) ||
       (qmi_error != QMI_NO_ERR) ||
       (reg_wlan_status_ind_resp_msg.resp.result != QMI_NO_ERR) )
  {
    LOG_MSG_ERROR("Cannot enable Register for WLAN Status IND %d : %d",
        qmi_error, reg_wlan_status_ind_resp_msg.resp.error,0);
    *qmi_err_num = reg_wlan_status_ind_resp_msg.resp.error;
    return false;
  }
  LOG_MSG_INFO1("Successfully Registered for WLAN Status IND",0,0,0);
  return true;
}

/*===========================================================================
  FUNCTION EnableWLAN
  ===========================================================================*/
/*!
  @brief
  Brings up the WLAN interface

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::EnableWLAN(qmi_error_type_v01 *qmi_err_num, boolean privileged_client)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_enable_wlan_req_msg_v01 enable_wlan_req_msg_v01;
  qcmap_msgr_enable_wlan_resp_msg_v01 enable_wlan_resp_msg_v01;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&enable_wlan_req_msg_v01, 0, sizeof(qcmap_msgr_enable_wlan_req_msg_v01));
  memset(&enable_wlan_resp_msg_v01, 0, sizeof(qcmap_msgr_enable_wlan_resp_msg_v01));

  enable_wlan_req_msg_v01.mobile_ap_handle = this->mobile_ap_handle;

  if ( privileged_client )
  {
    enable_wlan_req_msg_v01.privileged_client_valid = TRUE;
    enable_wlan_req_msg_v01.privileged_client = privileged_client;
    LOG_MSG_INFO1("EnableWLAN from privilaged client",0,0,0);
  }

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_ENABLE_WLAN_REQ_V01,
                                       &enable_wlan_req_msg_v01,
                                       sizeof(qcmap_msgr_enable_wlan_req_msg_v01),
                                       (void*)&enable_wlan_resp_msg_v01,
                                       sizeof(qcmap_msgr_enable_wlan_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(enable): error %d result %d",
      qmi_error, enable_wlan_resp_msg_v01.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( enable_wlan_resp_msg_v01.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not enable wlan %d : %d",
        qmi_error, enable_wlan_resp_msg_v01.resp.error,0);
    *qmi_err_num = enable_wlan_resp_msg_v01.resp.error;
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION DisableWLAN
  ===========================================================================*/
/*!
  @brief
  Brings the WLAN interface down

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::DisableWLAN(qmi_error_type_v01 *qmi_err_num, boolean privileged_client)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_disable_wlan_req_msg_v01  disable_wlan_req_msg_v01;
  qcmap_msgr_disable_wlan_resp_msg_v01 disable_wlan_resp_msg_v01;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&disable_wlan_req_msg_v01, 0, sizeof(qcmap_msgr_disable_wlan_req_msg_v01));
  memset(&disable_wlan_resp_msg_v01, 0, sizeof(qcmap_msgr_disable_wlan_resp_msg_v01));

  disable_wlan_req_msg_v01.mobile_ap_handle = this->mobile_ap_handle;

  if ( privileged_client )
  {
    disable_wlan_req_msg_v01.privileged_client_valid = TRUE;
    disable_wlan_req_msg_v01.privileged_client = privileged_client;
    LOG_MSG_INFO1("DisableWLAN from privilaged client",0,0,0);
  }

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_DISABLE_WLAN_REQ_V01,
                                       &disable_wlan_req_msg_v01,
                                       sizeof(qcmap_msgr_disable_wlan_req_msg_v01),
                                       (void*)&disable_wlan_resp_msg_v01,
                                       sizeof(qcmap_msgr_disable_wlan_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(enable): error %d result %d",
      qmi_error, disable_wlan_resp_msg_v01.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( disable_wlan_resp_msg_v01.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not disable wlan %d : %d",
        qmi_error, disable_wlan_resp_msg_v01.resp.error,0);
    *qmi_err_num = disable_wlan_resp_msg_v01.resp.error;
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION ActivateWLAN
  ===========================================================================*/
/*!
  @brief
  Activates the WLAN interface with the current available config.

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::ActivateWLAN
(
  qmi_error_type_v01 *qmi_err_num
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_activate_wlan_resp_msg_v01 activate_wlan_resp_msg_v01;
  qcmap_msgr_activate_wlan_req_msg_v01  activate_wlan_req_msg;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&activate_wlan_resp_msg_v01, 0, sizeof(qcmap_msgr_activate_wlan_resp_msg_v01));
  memset(&activate_wlan_req_msg, 0, sizeof(qcmap_msgr_activate_wlan_req_msg_v01));
  activate_wlan_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_ACTIVATE_WLAN_REQ_V01,
                                       (void *)&activate_wlan_req_msg,
                                       sizeof(qcmap_msgr_activate_wlan_req_msg_v01),
                                       (void*)&activate_wlan_resp_msg_v01,
                                       sizeof(qcmap_msgr_activate_wlan_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(enable): error %d result %d",
      qmi_error, activate_wlan_resp_msg_v01.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR) ||
       ( activate_wlan_resp_msg_v01.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not activate wlan %d : %d",
        qmi_error, activate_wlan_resp_msg_v01.resp.error,0);
    *qmi_err_num = activate_wlan_resp_msg_v01.resp.error;
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION ActivateHostapdConfig
  ===========================================================================*/
/*!
  @brief
  Activates the Hostapd with the current available config.

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::ActivateHostapdConfig
(
  qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type,
  qcmap_msgr_activate_hostapd_action_enum_v01 action_type,
  qmi_error_type_v01 *qmi_err_num,
  boolean privileged_client
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_activate_hostapd_config_resp_msg_v01 activate_hostapd_config_resp_msg;
  qcmap_msgr_activate_hostapd_config_req_msg_v01  activate_hostapd_config_req_msg;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&activate_hostapd_config_resp_msg, 0, sizeof(qcmap_msgr_activate_hostapd_config_resp_msg_v01));
  memset(&activate_hostapd_config_req_msg, 0, sizeof(qcmap_msgr_activate_hostapd_config_req_msg_v01));
  activate_hostapd_config_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  activate_hostapd_config_req_msg.ap_type_valid = TRUE;
  activate_hostapd_config_req_msg.ap_type = ap_type;
  activate_hostapd_config_req_msg.action_type_valid = TRUE;
  activate_hostapd_config_req_msg.action_type = action_type;

  if ( privileged_client )
  {
    activate_hostapd_config_req_msg.privileged_client_valid = TRUE;
    activate_hostapd_config_req_msg.privileged_client = privileged_client;
    LOG_MSG_INFO1("ActivateHostapdConfig from privilaged client", 0, 0, 0);
  }

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_ACTIVATE_HOSTAPD_CONFIG_REQ_V01,
                                       (void *)&activate_hostapd_config_req_msg,
                                       sizeof(qcmap_msgr_activate_hostapd_config_req_msg_v01),
                                       (void*)&activate_hostapd_config_resp_msg,
                                       sizeof(qcmap_msgr_activate_hostapd_config_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(ActivateHostapd): error %d result %d",
      qmi_error, activate_hostapd_config_resp_msg.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR) ||
       ( activate_hostapd_config_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not activate hostapd %d : %d",
        qmi_error, activate_hostapd_config_resp_msg.resp.error,0);
    *qmi_err_num = activate_hostapd_config_resp_msg.resp.error;
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION ActivateSupplicantConfig
  ===========================================================================*/
/*!
  @brief
  Activates the Supplicant with the current available config.

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::ActivateSupplicantConfig
(
  qmi_error_type_v01 *qmi_err_num
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_activate_supplicant_config_resp_msg_v01 activate_supplicant_config_resp_msg;
  qcmap_msgr_activate_supplicant_config_req_msg_v01  activate_supplicant_config_req_msg;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&activate_supplicant_config_resp_msg, 0, sizeof(qcmap_msgr_activate_supplicant_config_resp_msg_v01));
  memset(&activate_supplicant_config_req_msg, 0, sizeof(qcmap_msgr_activate_supplicant_config_req_msg_v01));
  activate_supplicant_config_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_ACTIVATE_SUPPLICANT_CONFIG_REQ_V01,
                                       (void *)&activate_supplicant_config_req_msg,
                                       sizeof(qcmap_msgr_activate_supplicant_config_req_msg_v01),
                                       (void*)&activate_supplicant_config_resp_msg,
                                       sizeof(qcmap_msgr_activate_supplicant_config_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(ActivateSupplicant): error %d result %d",
      qmi_error, activate_supplicant_config_resp_msg.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR) ||
       ( activate_supplicant_config_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not activate supplicant %d : %d",
        qmi_error, activate_supplicant_config_resp_msg.resp.error,0);
    *qmi_err_num = activate_supplicant_config_resp_msg.resp.error;
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION GetActiveWlanIfInfo
  ===========================================================================*/
/*!
  @brief
  Obtains information from active WLAN interfaces.

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - WLAN should be enabled

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::GetActiveWlanIfInfo
(
  qcmap_msgr_wlan_if_info_t *wlan_if_info,
  qmi_error_type_v01 *qmi_err_num
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_get_active_wlan_if_info_resp_msg_v01 active_wlan_if_info_resp_msg;

  if (!wlan_if_info || !qmi_err_num)
  {
    LOG_MSG_ERROR("Invalid parameter passed ",0,0,0);
    return false;
  }

  QCMAP_LOG_FUNC_ENTRY();

  memset(&active_wlan_if_info_resp_msg, 0, sizeof(qcmap_msgr_get_active_wlan_if_info_resp_msg_v01));

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_ACTIVE_WLAN_IF_INFO_REQ_V01,
                                       NULL,
                                       0,
                                       &active_wlan_if_info_resp_msg,
                                       sizeof(qcmap_msgr_get_active_wlan_if_info_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(GetActiveWlanIfInfo): error %d result %d",
      qmi_error, active_wlan_if_info_resp_msg.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR) ||
       ( active_wlan_if_info_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not obtain WLAN IF information %d : %d",
        qmi_error, active_wlan_if_info_resp_msg.resp.error,0);
    *qmi_err_num = active_wlan_if_info_resp_msg.resp.error;
    return false;
  }

  if (active_wlan_if_info_resp_msg.wlan_if_info_valid &&
              (active_wlan_if_info_resp_msg.wlan_if_info_len > 0))
  {
    wlan_if_info->wlan_if_info_len = active_wlan_if_info_resp_msg.active_entries;
    if ((wlan_if_info->wlan_if_info_len > QCMAP_MSGR_MAX_WLAN_IFACE_V01) ||
          (wlan_if_info->wlan_if_info_len == 0))
    {
      *qmi_err_num = active_wlan_if_info_resp_msg.resp.error;
      LOG_MSG_INFO1("Invalid entries %d", wlan_if_info->wlan_if_info_len,0,0);
      return false;
    }

    memcpy(wlan_if_info->wlan_if_info, &active_wlan_if_info_resp_msg.wlan_if_info,
            sizeof(qcmap_msgr_wlan_if_info_v01)*(QCMAP_MSGR_MAX_WLAN_IFACE_V01));
  }

  LOG_MSG_INFO1("Obtained WLAN IF successfully", 0, 0, 0);
  return true;
}

/*===========================================================================
  FUNCTION ActivateLAN
  ===========================================================================*/
/*!
  @brief
  Activates the LAN interface with the current available config.

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::ActivateLAN
(
  qmi_error_type_v01 *qmi_err_num
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_activate_lan_resp_msg_v01 activate_lan_resp_msg_v01;
  qcmap_msgr_activate_lan_req_msg_v01 activate_lan_req_msg;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&activate_lan_resp_msg_v01, 0, sizeof(qcmap_msgr_activate_lan_resp_msg_v01));
  memset(&activate_lan_req_msg, 0, sizeof(qcmap_msgr_activate_lan_req_msg_v01));
  activate_lan_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_ACTIVATE_LAN_REQ_V01,
                                       (void *)&activate_lan_req_msg,
                                       sizeof(qcmap_msgr_activate_lan_req_msg_v01),
                                       (void*)&activate_lan_resp_msg_v01,
                                       sizeof(qcmap_msgr_activate_lan_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(enable): error %d result %d",
      qmi_error, activate_lan_resp_msg_v01.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR) ||
       ( activate_lan_resp_msg_v01.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not activate lan %d : %d",
        qmi_error, activate_lan_resp_msg_v01.resp.error,0);
    *qmi_err_num = activate_lan_resp_msg_v01.resp.error;
    return false;
  }
  return true;
}


/*===========================================================================
  FUNCTION SetWLANConfig
  ===========================================================================*/
/*!
  @brief
  Sets the WLAN mode, guest ap access profile

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::SetWLANConfig(qcmap_msgr_wlan_mode_enum_v01 wlan_mode,
                                             qcmap_msgr_access_profile_v01 guest_ap_access_profile,
                                             qcmap_msgr_station_mode_config_v01 station_config,
                                             qmi_error_type_v01 *qmi_err_num,
                                             qcmap_msgr_guest_profile_config_v01 *guest_profile)
{
  qcmap_msgr_set_wlan_config_req_msg_v01 set_wlan_config_req_msg;
  qcmap_msgr_set_wlan_config_resp_msg_v01 set_wlan_config_resp_msg;
  qmi_client_error_type qmi_error;
  qcmap_msgr_station_mode_config_v01 tmp_sta_null_struct;


  QCMAP_LOG_FUNC_ENTRY();

  memset(&set_wlan_config_req_msg, 0, sizeof(qcmap_msgr_set_wlan_config_req_msg_v01));
  memset(&tmp_sta_null_struct, 0, sizeof(qcmap_msgr_station_mode_config_v01));

  set_wlan_config_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  set_wlan_config_req_msg.wlan_mode_valid = TRUE;
  set_wlan_config_req_msg.wlan_mode = wlan_mode;

  if (guest_profile == NULL )
  {
    if (guest_ap_access_profile != QCMAP_MSGR_ACCESS_PROFILE_MIN_ENUM_VAL_V01)
    {
       set_wlan_config_req_msg.guest_ap_access_profile_valid = TRUE;
       set_wlan_config_req_msg.guest_ap_access_profile = guest_ap_access_profile;
    }
  }
  else
  {
    if (guest_profile->guest_ap_profile[QCMAP_MSGR_GUEST_AP_ONE_V01] !=
         QCMAP_MSGR_ACCESS_PROFILE_MIN_ENUM_VAL_V01)
    {
      set_wlan_config_req_msg.guest_ap_access_profile_valid = TRUE;
      set_wlan_config_req_msg.guest_ap_access_profile =
                            guest_profile->guest_ap_profile[QCMAP_MSGR_GUEST_AP_ONE_V01];
    }

    if (guest_profile->guest_ap_profile[QCMAP_MSGR_GUEST_AP_TWO_V01] !=
         QCMAP_MSGR_ACCESS_PROFILE_MIN_ENUM_VAL_V01)
    {
      set_wlan_config_req_msg.guest_ap_2_access_profile_valid = TRUE;
      set_wlan_config_req_msg.guest_ap_2_access_profile =
                         guest_profile->guest_ap_profile[QCMAP_MSGR_GUEST_AP_TWO_V01];
    }

    if (guest_profile->guest_ap_profile[QCMAP_MSGR_GUEST_AP_THREE_V01] !=
         QCMAP_MSGR_ACCESS_PROFILE_MIN_ENUM_VAL_V01)
    {
      set_wlan_config_req_msg.guest_ap_3_access_profile_valid = TRUE;
      set_wlan_config_req_msg.guest_ap_3_access_profile =
                         guest_profile->guest_ap_profile[QCMAP_MSGR_GUEST_AP_THREE_V01];
    }

  }
  if ( memcmp(&station_config,&tmp_sta_null_struct,sizeof(qcmap_msgr_station_mode_config_v01)) )
  {
    set_wlan_config_req_msg.station_config_valid = TRUE;
    memcpy(&set_wlan_config_req_msg.station_config, &station_config,
            sizeof(qcmap_msgr_station_mode_config_v01));
  }

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_WLAN_CONFIG_REQ_V01,
                                       &set_wlan_config_req_msg,
                                       sizeof(qcmap_msgr_set_wlan_config_req_msg_v01),
                                       &set_wlan_config_resp_msg,
                                       sizeof(qcmap_msgr_set_wlan_config_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR) ||
       ( set_wlan_config_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not set WLAN config %d : %d",
        qmi_error, set_wlan_config_resp_msg.resp.error,0);
    *qmi_err_num = set_wlan_config_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("WLAN Config Set succeeded...", 0, 0, 0);
  return true;

}

/*===========================================================================
  FUNCTION SetLANConfig
  ===========================================================================*/
/*!
  @brief
  Sets the LAN mode.

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::SetLANConfig(qcmap_msgr_lan_config_v01 lan_config,
                                             qmi_error_type_v01 *qmi_err_num)
{

  qcmap_msgr_set_lan_config_req_msg_v01 set_lan_config_req_msg;
  qcmap_msgr_set_lan_config_resp_msg_v01 set_lan_config_resp_msg;
  qcmap_msgr_lan_config_v01  tmp_lan_null_struct;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&set_lan_config_req_msg, 0, sizeof(qcmap_msgr_set_lan_config_req_msg_v01));
  memset(&tmp_lan_null_struct, 0, sizeof(qcmap_msgr_lan_config_v01));

  if ( memcmp(&lan_config, &tmp_lan_null_struct, sizeof(qcmap_msgr_lan_config_v01)) ){
    set_lan_config_req_msg.lan_config_valid = TRUE;
    memcpy(&set_lan_config_req_msg.lan_config, &lan_config, sizeof(qcmap_msgr_lan_config_v01));
  } else {
    LOG_MSG_INFO1("LAN Config- No modification needed", 0, 0, 0);
    return true;
  }

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_LAN_CONFIG_REQ_V01,
                                       &set_lan_config_req_msg,
                                       sizeof(qcmap_msgr_set_lan_config_req_msg_v01),
                                       &set_lan_config_resp_msg,
                                       sizeof(qcmap_msgr_set_lan_config_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR) ||
       ( set_lan_config_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not set LAN config %d : %d",
        qmi_error, set_lan_config_resp_msg.resp.error,0);
    *qmi_err_num = set_lan_config_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("LAN Config Set succeeded...", 0, 0, 0);
  return true;

}

/*===========================================================================
  FUNCTION GetWLANStatus
  ===========================================================================*/
/*!
  @brief
  Gets the current mode in which WLAN is brought up.

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::GetWLANStatus
(
  qcmap_msgr_wlan_mode_enum_v01 *wlan_mode,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_get_wlan_status_resp_msg_v01 get_wlan_status_resp_msg;
  qcmap_msgr_get_wlan_status_req_msg_v01 get_wlan_status_req_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&get_wlan_status_resp_msg, 0, sizeof(qcmap_msgr_get_wlan_status_resp_msg_v01));
  memset(&get_wlan_status_req_msg, 0, sizeof(qcmap_msgr_get_wlan_status_req_msg_v01));
  get_wlan_status_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_WLAN_STATUS_REQ_V01,
                                       &get_wlan_status_req_msg,
                                       sizeof(qcmap_msgr_get_wlan_status_req_msg_v01),
                                       &get_wlan_status_resp_msg,
                                       sizeof(qcmap_msgr_get_wlan_status_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR) ||
       ( get_wlan_status_resp_msg.resp.result != QMI_NO_ERR ))
  {
    LOG_MSG_ERROR("Can not get WLAN status %d : %d",
                   qmi_error, get_wlan_status_resp_msg.resp.error, 0);
    *qmi_err_num = get_wlan_status_resp_msg.resp.error;
    return false;
  }

  if (get_wlan_status_resp_msg.wlan_mode_valid)
  {
    *wlan_mode = get_wlan_status_resp_msg.wlan_mode;
    LOG_MSG_INFO1("Get WLAN Status Succeeded. WLAN Mode:%d", *wlan_mode, 0, 0);
  }

  return true;
}

/*===========================================================================
  FUNCTION GetWLANConfig
  ===========================================================================*/
/*!
  @brief
  Gets the current configured WLan Configuration.

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::GetWLANConfig
(
  qcmap_msgr_wlan_mode_enum_v01 *wlan_mode,
  qcmap_msgr_access_profile_v01 *guest_ap_access_profile,
  qcmap_msgr_station_mode_config_v01 *station_config,
  qmi_error_type_v01 *qmi_err_num,
  qcmap_msgr_guest_profile_config_v01* guest_profile
)
{
  qcmap_msgr_get_wlan_config_resp_msg_v01 get_wlan_config_resp_msg;
  qmi_client_error_type qmi_error;


  memset(&get_wlan_config_resp_msg, 0, sizeof(qcmap_msgr_get_wlan_config_resp_msg_v01));
  QCMAP_LOG_FUNC_ENTRY();

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_WLAN_CONFIG_REQ_V01,
                                       NULL,
                                       0,
                                       &get_wlan_config_resp_msg,
                                       sizeof(qcmap_msgr_get_wlan_config_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_wlan_config_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get WLAN Config %d : %d",
                   qmi_error, get_wlan_config_resp_msg.resp.error, 0);
    *qmi_err_num = get_wlan_config_resp_msg.resp.error;
    return false;
  }

  if (get_wlan_config_resp_msg.wlan_mode_valid)
  {
    *wlan_mode = get_wlan_config_resp_msg.wlan_mode;
  }

  if (guest_profile == NULL)
  {
    if (get_wlan_config_resp_msg.guest_ap_access_profile_valid)
    {
      *guest_ap_access_profile = get_wlan_config_resp_msg.guest_ap_access_profile;
    }
  }
  else
  {
    if (get_wlan_config_resp_msg.guest_ap_access_profile_valid)
    {
      guest_profile->guest_ap_profile[QCMAP_MSGR_GUEST_AP_ONE_V01] =
         get_wlan_config_resp_msg.guest_ap_access_profile;
    }

    if (get_wlan_config_resp_msg.guest_ap_2_access_profile_valid)
    {
      guest_profile->guest_ap_profile[QCMAP_MSGR_GUEST_AP_TWO_V01] =
         get_wlan_config_resp_msg.guest_ap_2_access_profile;
    }

    if (get_wlan_config_resp_msg.guest_ap_3_access_profile_valid)
    {
      guest_profile->guest_ap_profile[QCMAP_MSGR_GUEST_AP_THREE_V01] =
         get_wlan_config_resp_msg.guest_ap_3_access_profile;
    }

  }



  if (get_wlan_config_resp_msg.station_config_valid)
  {
    *station_config = get_wlan_config_resp_msg.station_config;
  }

  LOG_MSG_INFO1("Get WLAN Config Succeeded. WLAN Mode:%d", *wlan_mode, 0, 0);

  return true;
}

/*===========================================================================
  FUNCTION GetLANConfig
  ===========================================================================*/
/*!
  @brief
  Gets the current configured Lan Configuration.

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::GetLANConfig
(
  qcmap_msgr_lan_config_v01 *lan_config,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_get_lan_config_resp_msg_v01 get_lan_config_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_LAN_CONFIG_REQ_V01,
                                       NULL,
                                       0,
                                       &get_lan_config_resp_msg,
                                       sizeof(qcmap_msgr_get_lan_config_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_lan_config_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get WLAN Config %d : %d",
                   qmi_error, get_lan_config_resp_msg.resp.error, 0);
    *qmi_err_num = get_lan_config_resp_msg.resp.error;
    return false;
  }

  if (get_lan_config_resp_msg.lan_config_valid)
  {
    *lan_config = get_lan_config_resp_msg.lan_config;
  }

  LOG_MSG_INFO1("Get LAN Config Succeeded.",0, 0, 0);

  return true;
}

/*===========================================================================
  FUNCTION ConnectBackHaul
  ===========================================================================*/
/*!
  @brief
  Brings up the WWAN interface up

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::ConnectBackHaul
(
  qcmap_msgr_wwan_call_type_v01 call_type,
  qmi_error_type_v01           *qmi_err_num
)
{
  int qcmap_msgr_errno;
  int ret = QCMAP_CM_SUCCESS;
  qcmap_msgr_bring_up_wwan_req_msg_v01  qcmap_bring_up_wwan_req_msg;
  qcmap_msgr_bring_up_wwan_resp_msg_v01 qcmap_bring_up_wwan_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&qcmap_bring_up_wwan_req_msg, 0, sizeof(qcmap_msgr_bring_up_wwan_req_msg_v01));
  memset(&qcmap_bring_up_wwan_resp_msg, 0, sizeof(qcmap_msgr_bring_up_wwan_resp_msg_v01));

  /* Bring up the data call. */
  LOG_MSG_INFO1("Bring up wwan",0,0,0);
  qcmap_bring_up_wwan_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  qcmap_bring_up_wwan_req_msg.call_type_valid = TRUE;

  qcmap_bring_up_wwan_req_msg.call_type = call_type;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_BRING_UP_WWAN_REQ_V01,
                                       &qcmap_bring_up_wwan_req_msg,
                                       sizeof(qcmap_msgr_bring_up_wwan_req_msg_v01),
                                       &qcmap_bring_up_wwan_resp_msg,
                                       sizeof(qcmap_msgr_bring_up_wwan_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( qcmap_bring_up_wwan_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not bring up wwan qcmap %d : %d",
        qmi_error, qcmap_bring_up_wwan_resp_msg.resp.error,0);
    *qmi_err_num = qcmap_bring_up_wwan_resp_msg.resp.error;
    return false;
  }

/*
   If WWAN is already enabled, and we are trying to enable again from a different client,
   set error number to QMI_ERR_NO_EFFECT_V01, so that the correspondingclient can be
   informed. We hit this scenario in the following case:
   1. Start QCMAP_CLI and enable Backhaul.
   2. Start MCM_MOBILEAP_CLI and try enabling backhaul again.
  */
  if (call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01 &&
      qcmap_bring_up_wwan_resp_msg.conn_status ==
      QCMAP_MSGR_WWAN_STATUS_CONNECTED_V01)
  {
    LOG_MSG_INFO1("WWAN is already enabled.",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
  }
  else if (call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01 &&
      qcmap_bring_up_wwan_resp_msg.conn_status ==
      QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTED_V01)
  {
    LOG_MSG_INFO1("IPv6 WWAN is already enabled.",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
  }
  else
    LOG_MSG_INFO1("Bringing up wwan...",0,0,0);
  return true;
}

/*===========================================================================
  FUNCTION DisconnectBackHaul
  ===========================================================================*/
/*!
  @brief
  Brings down the WWAN interface

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/


boolean QCMAP_Client::DisconnectBackHaul
(
  qcmap_msgr_wwan_call_type_v01 call_type,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_tear_down_wwan_req_msg_v01 qcmap_tear_down_wwan_req_msg;
  qcmap_msgr_tear_down_wwan_resp_msg_v01 qcmap_tear_down_wwan_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&qcmap_tear_down_wwan_req_msg, 0, sizeof(qcmap_msgr_tear_down_wwan_req_msg_v01));
  memset(&qcmap_tear_down_wwan_resp_msg, 0, sizeof(qcmap_msgr_tear_down_wwan_resp_msg_v01));

  LOG_MSG_INFO1("Bringing down wwan",0,0,0);
  qcmap_tear_down_wwan_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qcmap_tear_down_wwan_req_msg.call_type_valid = TRUE;
  qcmap_tear_down_wwan_req_msg.call_type = call_type;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_TEAR_DOWN_WWAN_REQ_V01,
                                       &qcmap_tear_down_wwan_req_msg,
                                       sizeof(qcmap_msgr_tear_down_wwan_req_msg_v01),
                                       &qcmap_tear_down_wwan_resp_msg,
                                       sizeof(qcmap_msgr_tear_down_wwan_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR) ||
       ( qcmap_tear_down_wwan_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not tear down wwan qcmap %d : %d",
        qmi_error, qcmap_tear_down_wwan_resp_msg.resp.error,0);
    *qmi_err_num = qcmap_tear_down_wwan_resp_msg.resp.error;
    return false;
  }

  /*
     If WWAN is already disabled, and we are trying to disable again from a different client,
     set error number to QMI_ERR_NO_EFFECT_V01, so that the correspondingclient can be
     informed. We hit this scenario in the following case:
     1. Start QCMAP_CLI and enable Backhaul.
     2. Start MCM_MOBILEAP_CLI and try enabling backhaul again.
     3. Disable backhaul from the 1st client.
     4. Now from the 2nd client.
    */
    if (call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01 &&
        qcmap_tear_down_wwan_resp_msg.conn_status ==
        QCMAP_MSGR_WWAN_STATUS_DISCONNECTED_V01)
    {
      LOG_MSG_INFO1("WWAN is already disabled.",0,0,0);
      *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    }
    else if (call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01 &&
        qcmap_tear_down_wwan_resp_msg.conn_status ==
        QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTED_V01)
    {
      LOG_MSG_INFO1("IPv6 WWAN is already disabled.",0,0,0);
      *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    }
    else
      LOG_MSG_INFO1("Tearing down wwan...",0,0,0);
  return true;
}


/*===========================================================================
  FUNCTION AddStaticNatEntry
  ===========================================================================*/
/*!
  @brief
  Add a static nat entry

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::AddStaticNatEntry(qcmap_msgr_snat_entry_config_v01 *snat_entry, qmi_error_type_v01 *qmi_err_num)
{
  qcmap_msgr_add_static_nat_entry_req_msg_v01 add_snat_req_msg;
  qcmap_msgr_add_static_nat_entry_resp_msg_v01 add_snat_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  add_snat_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  memcpy(&add_snat_req_msg.snat_entry_config, snat_entry, sizeof(qcmap_msgr_snat_entry_config_v01));

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_ADD_STATIC_NAT_ENTRY_REQ_V01,
                                       &add_snat_req_msg,
                                       sizeof(qcmap_msgr_add_static_nat_entry_req_msg_v01),
                                       &add_snat_resp_msg,
                                       sizeof(qcmap_msgr_add_static_nat_entry_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( add_snat_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not add snat entry %d : %d",
        qmi_error, add_snat_resp_msg.resp.error,0);
    *qmi_err_num = add_snat_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("Added SNAT Entry...",0,0,0);
  return true;
}


/*===========================================================================
  FUNCTION DeleteStaticNatEntry
  ===========================================================================*/
/*!
  @brief
  Deletes a static nat entry

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::DeleteStaticNatEntry(qcmap_msgr_snat_entry_config_v01 *snat_entry, qmi_error_type_v01 *qmi_err_num)
{
  qcmap_msgr_delete_static_nat_entry_req_msg_v01 delete_snat_req_msg;
  qcmap_msgr_delete_static_nat_entry_resp_msg_v01 delete_snat_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  delete_snat_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  memcpy(&delete_snat_req_msg.snat_entry_config, snat_entry, sizeof(qcmap_msgr_snat_entry_config_v01));

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_DELETE_STATIC_NAT_ENTRY_REQ_V01,
                                       &delete_snat_req_msg,
                                       sizeof(qcmap_msgr_delete_static_nat_entry_req_msg_v01),
                                       &delete_snat_resp_msg,
                                       sizeof(qcmap_msgr_delete_static_nat_entry_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( delete_snat_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not delete snat entry %d : %d",
        qmi_error, delete_snat_resp_msg.resp.error,0);
    *qmi_err_num = delete_snat_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("Deleted SNAT Entry...",0,0,0);
  return true;
}


/*===========================================================================
  FUNCTION GetStaticNatConfig
  ===========================================================================*/
/*!
  @brief
  Deletes a static nat entry

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::GetStaticNatConfig(qcmap_msgr_snat_entry_config_v01 *snat_config, int *num_entries, qmi_error_type_v01 *qmi_err_num)
{
  qcmap_msgr_get_static_nat_entries_req_msg_v01 get_snat_req_msg;
  qcmap_msgr_get_static_nat_entries_resp_msg_v01 get_snat_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  get_snat_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_STATIC_NAT_ENTRIES_REQ_V01,
                                       &get_snat_req_msg,
                                       sizeof(qcmap_msgr_get_static_nat_entries_req_msg_v01),
                                       &get_snat_resp_msg,
                                       sizeof(qcmap_msgr_get_static_nat_entries_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_snat_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get snat entries %d : %d",
        qmi_error, get_snat_resp_msg.resp.error,0);
    *qmi_err_num = get_snat_resp_msg.resp.error;
    return false;
  }

  if (get_snat_resp_msg.snat_config_valid == true)
  {
    *num_entries = get_snat_resp_msg.snat_config_len;
    LOG_MSG_INFO1("\nNum SNAT entries confged: %d",*num_entries,0,0);
    if (*num_entries <= QCMAP_MSGR_MAX_SNAT_ENTRIES_V01)
      memcpy(snat_config, &get_snat_resp_msg.snat_config, *num_entries * sizeof(qcmap_msgr_snat_entry_config_v01));
    else
    {
      LOG_MSG_INFO1("\nNum SNAT entries confged > QCMAP_MSGR_MAX_SNAT_ENTRIES_V01",0,0,0);
      *qmi_err_num = get_snat_resp_msg.resp.error;
      return false;
    }
  }
  else
  {
    LOG_MSG_INFO1("\nNo SNAT entries configured",0,0,0);
  }
  LOG_MSG_INFO1("Get SNAT Entries Succeeded...",0,0,0);
  return true;
}


/*===========================================================================
  FUNCTION DeleteDMZ
  ===========================================================================*/
/*!
  @brief
  Deletes a DMZ entry

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::DeleteDMZ(qmi_error_type_v01 *qmi_err_num)
{
  qcmap_msgr_delete_dmz_req_msg_v01 delete_dmz_req_msg;
  qcmap_msgr_delete_dmz_resp_msg_v01 delete_dmz_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  delete_dmz_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_DELETE_DMZ_REQ_V01,
                                       &delete_dmz_req_msg,
                                       sizeof(qcmap_msgr_delete_dmz_req_msg_v01),
                                       &delete_dmz_resp_msg,
                                       sizeof(qcmap_msgr_delete_dmz_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( delete_dmz_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not delete dmz %d : %d",
        qmi_error, delete_dmz_resp_msg.resp.error,0);
    *qmi_err_num = delete_dmz_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("Deleted DMZ...",0,0,0);
  return true;
}


/*===========================================================================
  FUNCTION AddDMZ
  ===========================================================================*/
/*!
  @brief
  Adds a DMZ entry

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::AddDMZ(uint32 dmz_ip, qmi_error_type_v01 *qmi_err_num)
{
  qcmap_msgr_set_dmz_req_msg_v01 add_dmz_req_msg;
  qcmap_msgr_set_dmz_resp_msg_v01 add_dmz_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  add_dmz_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  add_dmz_req_msg.dmz_ip_addr = dmz_ip;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_DMZ_REQ_V01,
                                       &add_dmz_req_msg,
                                       sizeof(qcmap_msgr_set_dmz_req_msg_v01),
                                       &add_dmz_resp_msg,
                                       sizeof(qcmap_msgr_set_dmz_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( add_dmz_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not add dmz %d : %d",
        qmi_error, add_dmz_resp_msg.resp.error,0);
    *qmi_err_num = add_dmz_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("Added DMZ...",0,0,0);
  return true;
}


/*===========================================================================
  FUNCTION GetDMZ
  ===========================================================================*/
/*!
  @brief
  Gets a DMZ entry

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::GetDMZ(uint32_t *dmz_ip, qmi_error_type_v01 *qmi_err_num)
{
  qcmap_msgr_get_dmz_req_msg_v01 get_dmz_req_msg;
  qcmap_msgr_get_dmz_resp_msg_v01 get_dmz_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  get_dmz_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_DMZ_REQ_V01,
                                       &get_dmz_req_msg,
                                       sizeof(qcmap_msgr_get_dmz_req_msg_v01),
                                       &get_dmz_resp_msg,
                                       sizeof(qcmap_msgr_get_dmz_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_dmz_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get dmz %d : %d",
        qmi_error, get_dmz_resp_msg.resp.error,0);
    *qmi_err_num = get_dmz_resp_msg.resp.error;
    return false;
  }

  if (get_dmz_resp_msg.dmz_ip_addr_valid)
  {
    LOG_MSG_INFO1("\nDMZ IP got successfully",0,0,0);
    *dmz_ip = get_dmz_resp_msg.dmz_ip_addr;
  }
  else
  {
    LOG_MSG_INFO1("\nNo DMZ IP addr configured",0,0,0);
  }
  LOG_MSG_INFO1("Get DMZ succeeded!!",0,0,0);
  return true;
}

/*===========================================================================
  FUNCTION GetWWANStatistics
  ===========================================================================*/
/*!
  @brief
    Gets the WWAN statistic

  @return
    true  - on Success
    false - on Failure

  @note

  - Dependencies
    - None

  - Side Effects
    - None
 */
/*=========================================================================*/
boolean QCMAP_Client::GetWWANStatistics
(
  qcmap_msgr_ip_family_enum_v01        ip_family,
  qcmap_msgr_wwan_statistics_type_v01 *wwan_stats,
  qmi_error_type_v01                  *qmi_err_num
)
{
  qcmap_msgr_get_wwan_stats_req_msg_v01 get_wwan_stats_req_msg;
  qcmap_msgr_get_wwan_stats_resp_msg_v01 get_wwan_stats_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  get_wwan_stats_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  get_wwan_stats_req_msg.ip_family = ip_family;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_WWAN_STATS_REQ_V01,
                                       &get_wwan_stats_req_msg,
                                       sizeof(qcmap_msgr_get_wwan_stats_req_msg_v01),
                                       &get_wwan_stats_resp_msg,
                                       sizeof(qcmap_msgr_get_wwan_stats_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_wwan_stats_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get wwan stats %d : %d",
        qmi_error, get_wwan_stats_resp_msg.resp.error,0);
    *qmi_err_num = get_wwan_stats_resp_msg.resp.error;
    return false;
  }

  wwan_stats->bytes_rx = get_wwan_stats_resp_msg.wwan_stats.bytes_rx;
  wwan_stats->bytes_tx = get_wwan_stats_resp_msg.wwan_stats.bytes_tx;
  wwan_stats->pkts_rx = get_wwan_stats_resp_msg.wwan_stats.pkts_rx;
  wwan_stats->pkts_tx = get_wwan_stats_resp_msg.wwan_stats.pkts_tx;
  wwan_stats->pkts_dropped_rx = get_wwan_stats_resp_msg.wwan_stats.pkts_dropped_rx;
  wwan_stats->pkts_dropped_tx = get_wwan_stats_resp_msg.wwan_stats.pkts_dropped_tx;
  LOG_MSG_INFO1("Get WWAN Stats succeeded...",0,0,0);
  return true;
}

/*===========================================================================
  FUNCTION ResetWWANStatistics
  ===========================================================================*/
/*!
  @brief
  Resets the WWAN statistics

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::ResetWWANStatistics
(
  qcmap_msgr_ip_family_enum_v01 ip_family,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_reset_wwan_stats_req_msg_v01 reset_wwan_stats_req_msg;
  qcmap_msgr_reset_wwan_stats_resp_msg_v01 reset_wwan_stats_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  reset_wwan_stats_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  reset_wwan_stats_req_msg.ip_family = ip_family;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_RESET_WWAN_STATS_REQ_V01,
                                       &reset_wwan_stats_req_msg,
                                       sizeof(qcmap_msgr_reset_wwan_stats_req_msg_v01),
                                       &reset_wwan_stats_resp_msg,
                                       sizeof(qcmap_msgr_reset_wwan_stats_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( reset_wwan_stats_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not reset wwan stats %d : %d",
        qmi_error, reset_wwan_stats_resp_msg.resp.error,0);
    *qmi_err_num = reset_wwan_stats_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("Reset WWAN Stats succeeded...",0,0,0);
  return true;
}


/*===========================================================================
  FUNCTION SetIPSECVpnPassthrough
  ===========================================================================*/
/*!
  @brief
  Will enable the IpsecVpn Pass through mode

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::SetIPSECVpnPassthrough(boolean enable, qmi_error_type_v01 *qmi_err_num)
{
  qcmap_msgr_set_ipsec_vpn_pass_through_req_msg_v01 set_ipsec_vpn_pt_req_msg;
  qcmap_msgr_set_ipsec_vpn_pass_through_resp_msg_v01 set_ipsec_vpn_pt_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  set_ipsec_vpn_pt_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  set_ipsec_vpn_pt_req_msg.vpn_pass_through_value = enable;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_IPSEC_VPN_PASS_THROUGH_REQ_V01,
                                       &set_ipsec_vpn_pt_req_msg,
                                       sizeof(qcmap_msgr_set_ipsec_vpn_pass_through_req_msg_v01),
                                       &set_ipsec_vpn_pt_resp_msg,
                                       sizeof(qcmap_msgr_set_ipsec_vpn_pass_through_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( set_ipsec_vpn_pt_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not set ipsec vpn passthrough %d : %d",
        qmi_error, set_ipsec_vpn_pt_resp_msg.resp.error,0);
    *qmi_err_num = set_ipsec_vpn_pt_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("IPSEC Vpn Passthrough Set succeeded...",0,0,0);
  return true;
}


/*===========================================================================
  FUNCTION GetIPSECVpnPassthrough
  ===========================================================================*/
/*!
  @brief
  Will get the IpsecVpn Pass through mode

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::GetIPSECVpnPassthrough(boolean *enable, qmi_error_type_v01 *qmi_err_num)
{
  qcmap_msgr_get_ipsec_vpn_pass_through_req_msg_v01 get_ipsec_vpn_pt_req_msg;
  qcmap_msgr_get_ipsec_vpn_pass_through_resp_msg_v01 get_ipsec_vpn_pt_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  get_ipsec_vpn_pt_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_IPSEC_VPN_PASS_THROUGH_REQ_V01,
                                       &get_ipsec_vpn_pt_req_msg,
                                       sizeof(qcmap_msgr_get_ipsec_vpn_pass_through_req_msg_v01),
                                       &get_ipsec_vpn_pt_resp_msg,
                                       sizeof(qcmap_msgr_get_ipsec_vpn_pass_through_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_ipsec_vpn_pt_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get ipsec vpn passthrough %d : %d",
        qmi_error, get_ipsec_vpn_pt_resp_msg.resp.error,0);
    *qmi_err_num = get_ipsec_vpn_pt_resp_msg.resp.error;
    return false;
  }

  if (get_ipsec_vpn_pt_resp_msg.vpn_pass_through_value_valid)
  {
    *enable = get_ipsec_vpn_pt_resp_msg.vpn_pass_through_value;
    LOG_MSG_ERROR("IPSEC Vpn Passthrough Get succeeded...%d", *enable,0,0);
  }

  return true;

}


/*===========================================================================
  FUNCTION SetPPTPVpnPassthrough
  ===========================================================================*/
/*!
  @brief
  Will enable the peer to peer vpn Pass through mode
  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::SetPPTPVpnPassthrough(boolean enable, qmi_error_type_v01 *qmi_err_num)
{
  qcmap_msgr_set_pptp_vpn_pass_through_req_msg_v01 set_pptp_vpn_pt_req_msg;
  qcmap_msgr_set_pptp_vpn_pass_through_resp_msg_v01 set_pptp_vpn_pt_resp_msg;
  qmi_client_error_type qmi_error;


  set_pptp_vpn_pt_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  set_pptp_vpn_pt_req_msg.vpn_pass_through_value = enable;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_PPTP_VPN_PASS_THROUGH_REQ_V01,
                                       &set_pptp_vpn_pt_req_msg,
                                       sizeof(qcmap_msgr_set_pptp_vpn_pass_through_req_msg_v01),
                                       &set_pptp_vpn_pt_resp_msg,
                                       sizeof(qcmap_msgr_set_pptp_vpn_pass_through_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( set_pptp_vpn_pt_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not set pptp vpn passthrough %d : %d",
        qmi_error, set_pptp_vpn_pt_resp_msg.resp.error,0);
    *qmi_err_num = set_pptp_vpn_pt_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("PPTP Vpn Passthrough Set succeeded...",0,0,0);
  return true;
}


/*===========================================================================
  FUNCTION GetPPTPVpnPassthrough
  ===========================================================================*/
/*!
  @brief
  Will get the peer to peer vpn Pass through mode set
  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/


boolean QCMAP_Client::GetPPTPVpnPassthrough(boolean *enable, qmi_error_type_v01 *qmi_err_num)
{
  qcmap_msgr_get_pptp_vpn_pass_through_req_msg_v01 get_pptp_vpn_pt_req_msg;
  qcmap_msgr_get_pptp_vpn_pass_through_resp_msg_v01 get_pptp_vpn_pt_resp_msg;
  qmi_client_error_type qmi_error;


  get_pptp_vpn_pt_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_PPTP_VPN_PASS_THROUGH_REQ_V01,
                                       &get_pptp_vpn_pt_req_msg,
                                       sizeof(qcmap_msgr_get_pptp_vpn_pass_through_req_msg_v01),
                                       &get_pptp_vpn_pt_resp_msg,
                                       sizeof(qcmap_msgr_get_pptp_vpn_pass_through_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_pptp_vpn_pt_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get pptp vpn passthrough %d : %d",
        qmi_error, get_pptp_vpn_pt_resp_msg.resp.error,0);
    *qmi_err_num = get_pptp_vpn_pt_resp_msg.resp.error;
    return false;
  }

  if (get_pptp_vpn_pt_resp_msg.vpn_pass_through_value_valid)
  {
    *enable = get_pptp_vpn_pt_resp_msg.vpn_pass_through_value;
    LOG_MSG_ERROR("PPTP Vpn Passthrough Get succeeded...%d", *enable,0,0);
  }

  return true;
}


/*===========================================================================
  FUNCTION SetL2TPVpnPassthrough
  ===========================================================================*/
/*!
  @brief
  Will set the Layer 2 Tunneling Protocol vpn Pass through mode

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::SetL2TPVpnPassthrough(boolean enable, qmi_error_type_v01 *qmi_err_num)
{
  qcmap_msgr_set_l2tp_vpn_pass_through_req_msg_v01 set_l2tp_vpn_pt_req_msg;
  qcmap_msgr_set_l2tp_vpn_pass_through_resp_msg_v01 set_l2tp_vpn_pt_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  set_l2tp_vpn_pt_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  set_l2tp_vpn_pt_req_msg.vpn_pass_through_value = enable;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_L2TP_VPN_PASS_THROUGH_REQ_V01,
                                       &set_l2tp_vpn_pt_req_msg,
                                       sizeof(qcmap_msgr_set_l2tp_vpn_pass_through_req_msg_v01),
                                       &set_l2tp_vpn_pt_resp_msg,
                                       sizeof(qcmap_msgr_set_l2tp_vpn_pass_through_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( set_l2tp_vpn_pt_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not set l2tp vpn passthrough %d : %d",
        qmi_error, set_l2tp_vpn_pt_resp_msg.resp.error,0);
    *qmi_err_num = set_l2tp_vpn_pt_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("L2TP Vpn Passthrough Set succeeded...",0,0,0);
  return true;

}


/*===========================================================================
  FUNCTION GetL2TPVpnPassthrough
  ===========================================================================*/
/*!
  @brief
  Will get the Layer 2 Tunneling Protocol vpn Pass through mode is enabled or
  disabled

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::GetL2TPVpnPassthrough(boolean *enable, qmi_error_type_v01 *qmi_err_num)
{
  qcmap_msgr_get_l2tp_vpn_pass_through_req_msg_v01 get_l2tp_vpn_pt_req_msg;
  qcmap_msgr_get_l2tp_vpn_pass_through_resp_msg_v01 get_l2tp_vpn_pt_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  get_l2tp_vpn_pt_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_L2TP_VPN_PASS_THROUGH_REQ_V01,
                                       &get_l2tp_vpn_pt_req_msg,
                                       sizeof(qcmap_msgr_get_l2tp_vpn_pass_through_req_msg_v01),
                                       &get_l2tp_vpn_pt_resp_msg,
                                       sizeof(qcmap_msgr_get_l2tp_vpn_pass_through_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_l2tp_vpn_pt_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get l2tp vpn passthrough %d : %d",
        qmi_error, get_l2tp_vpn_pt_resp_msg.resp.error,0);
    *qmi_err_num = get_l2tp_vpn_pt_resp_msg.resp.error;
    return false;
  }

  if (get_l2tp_vpn_pt_resp_msg.vpn_pass_through_value_valid)
  {
    *enable = get_l2tp_vpn_pt_resp_msg.vpn_pass_through_value;
    LOG_MSG_INFO1("L2TP Vpn Passthrough Get succeeded...%d", *enable,0,0);
  }

  return true;
}


/*===========================================================================
  FUNCTION SetNatType
  ===========================================================================*/
/*!
  @brief
  Enables the NAT type
  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::SetNatType
(
  qcmap_msgr_nat_enum_v01 nat_type,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_set_nat_type_req_msg_v01 set_nat_type_req_msg;
  qcmap_msgr_set_nat_type_resp_msg_v01 set_nat_type_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();
  set_nat_type_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  set_nat_type_req_msg.nat_type = nat_type;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_NAT_TYPE_REQ_V01,
                                       &set_nat_type_req_msg,
                                       sizeof(qcmap_msgr_set_nat_type_req_msg_v01),
                                       &set_nat_type_resp_msg,
                                       sizeof(qcmap_msgr_set_nat_type_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( set_nat_type_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not set nat_type %d : %d",
        qmi_error, set_nat_type_resp_msg.resp.error,0);
    *qmi_err_num = set_nat_type_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("NAT Type Set succeeded...",0,0,0);
  return true;
}


/*===========================================================================
  FUNCTION GetNatType
  ===========================================================================*/
/*!
  @brief
  Gets the NAT type enabled

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::GetNatType
(
  qcmap_msgr_nat_enum_v01 *nat_type,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_get_nat_type_req_msg_v01 get_nat_type_req_msg;
  qcmap_msgr_get_nat_type_resp_msg_v01 get_nat_type_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();
  get_nat_type_req_msg.mobile_ap_handle= this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_NAT_TYPE_REQ_V01,
                                       &get_nat_type_req_msg,
                                       sizeof(qcmap_msgr_get_nat_type_req_msg_v01),
                                       &get_nat_type_resp_msg,
                                       sizeof(qcmap_msgr_get_nat_type_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_nat_type_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get nat type %d : %d",
        qmi_error, get_nat_type_resp_msg.resp.error,0);
    *qmi_err_num = get_nat_type_resp_msg.resp.error;
    return false;
  }

  if (get_nat_type_resp_msg.nat_type_valid)
  {
    *nat_type = get_nat_type_resp_msg.nat_type;
    LOG_MSG_INFO1("NAT Type Get succeeded...%d", *nat_type,0,0);
  }

  return true;
}


/*===========================================================================
  FUNCTION SetAutoconnect
  ===========================================================================*/
/*!
  @brief
  Enables the auto connect feature

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::SetAutoconnect(boolean enable, qmi_error_type_v01 *qmi_err_num)
{
  qcmap_msgr_set_auto_connect_req_msg_v01 set_auto_connect_req_msg;
  qcmap_msgr_set_auto_connect_resp_msg_v01 set_auto_connect_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  set_auto_connect_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  set_auto_connect_req_msg.enable = enable;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_AUTO_CONNECT_REQ_V01,
                                       &set_auto_connect_req_msg,
                                       sizeof(qcmap_msgr_set_auto_connect_req_msg_v01),
                                       &set_auto_connect_resp_msg,
                                       sizeof(qcmap_msgr_set_auto_connect_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);
  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( set_auto_connect_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not set auto connect flag %d : %d",
        qmi_error, set_auto_connect_resp_msg.resp.error,0);
    *qmi_err_num = set_auto_connect_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("Auto Connect Mode Set succeeded...",0,0,0);
  return true;
}

/*===========================================================================
  FUNCTION getAutoconnect
  ===========================================================================*/
/*!
  @brief
  Displays the auto connect feature is enabled or not
  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::GetAutoconnect(boolean *enable, qmi_error_type_v01 *qmi_err_num)
{
  qcmap_msgr_get_auto_connect_req_msg_v01 get_auto_connect_req_msg;
  qcmap_msgr_get_auto_connect_resp_msg_v01 get_auto_connect_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();
  get_auto_connect_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_AUTO_CONNECT_REQ_V01,
                                       &get_auto_connect_req_msg,
                                       sizeof(qcmap_msgr_get_auto_connect_req_msg_v01),
                                       &get_auto_connect_resp_msg,
                                       sizeof(qcmap_msgr_get_auto_connect_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_auto_connect_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get Autoconnect mode flag %d : %d",
                   qmi_error, get_auto_connect_resp_msg.resp.error,0);
    *qmi_err_num = get_auto_connect_resp_msg.resp.error;
    return false;
  }

  *enable = get_auto_connect_resp_msg.auto_conn_flag;
  return true;
}


/*===========================================================================
  FUNCTION GetBackhaulStatus
  ===========================================================================*/
/*!
  @brief
  Gets Backahul Status and backhaul type and ip version if connected

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::GetBackhaulStatus
(
  qcmap_backhaul_status_info_type *backhaul_status_info,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_get_backhaul_status_resp_msg_v01 get_backhaul_status_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  if (qmi_err_num == NULL)
  {
    LOG_MSG_ERROR("GetBackhaulStatus Failed: NULL Args",0,0,0);
    return false;
  }

  if (backhaul_status_info == NULL)
  {
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    LOG_MSG_ERROR("GetBackhaulStatus Failed: %d",*qmi_err_num,0,0);
    return false;
  }

  memset(backhaul_status_info, 0x0, sizeof(qcmap_backhaul_status_info_type));
  memset(&get_backhaul_status_resp_msg, 0, sizeof(get_backhaul_status_resp_msg));

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_BACKHAUL_STATUS_REQ_V01,
                                       NULL,
                                       0,
                                       &get_backhaul_status_resp_msg,
                                       sizeof(qcmap_msgr_get_backhaul_status_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_backhaul_status_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get backhaul status %d : %d",
                   qmi_error, get_backhaul_status_resp_msg.resp.error,0);
    *qmi_err_num = get_backhaul_status_resp_msg.resp.error;
    return false;
  }

  if (get_backhaul_status_resp_msg.backhaul_v4_status_valid == true)
    backhaul_status_info->backhaul_v4_available = get_backhaul_status_resp_msg.backhaul_v4_status;
  if (get_backhaul_status_resp_msg.backhaul_v6_status_valid == true)
    backhaul_status_info->backhaul_v6_available = get_backhaul_status_resp_msg.backhaul_v6_status;
  if (get_backhaul_status_resp_msg.backhaul_type_valid == true)
    backhaul_status_info->backhaul_type = get_backhaul_status_resp_msg.backhaul_type;

  LOG_MSG_INFO1("Get Backhaul Status is success , Backhaul type : %d, "
                "V4 status : %d, V6 status : %d", backhaul_status_info->backhaul_type,
                 backhaul_status_info->backhaul_v4_available,
                 backhaul_status_info->backhaul_v6_available);

  return true;
}


/*===========================================================================
  FUNCTION AddFireWallEntry
  ===========================================================================*/
/*!
  @brief
  Encode a firewall configuration into a msgr message and sends the same to
  QCMAP connection manager to add firewall configuration

  @return
   true  on success.
   false on failure
  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::AddFireWallEntry
(
  qcmap_msgr_firewall_conf_t *firewall_conf,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_add_firewall_entry_req_msg_v01 add_extd_firewall_config_req_msg_v01;
  qcmap_msgr_add_firewall_entry_resp_msg_v01 add_extd_firewall_config_resp_msg_v01;
  int next_hdr_prot = 0;
  qmi_client_error_type qmi_error;
  int inc;
  qcmap_msgr_firewall_entry_conf_t *firewall_entry;
  ds_assert(firewall_conf != NULL);
  memset(&add_extd_firewall_config_req_msg_v01, 0,
         sizeof(add_extd_firewall_config_req_msg_v01));
  firewall_entry = &firewall_conf->extd_firewall_entry;
  add_extd_firewall_config_req_msg_v01.mobile_ap_handle = this->mobile_ap_handle;

  LOG_MSG_INFO1("IP family type %d, Direction %d ",firewall_entry->filter_spec.ip_vsn,
                firewall_entry->firewall_direction, 0);

   /* Default firewall direction is DL. */
  add_extd_firewall_config_req_msg_v01.direction_valid = TRUE;
  if (firewall_entry->firewall_direction == QCMAP_MSGR_UL_FIREWALL)
  {
    add_extd_firewall_config_req_msg_v01.direction = QCMAP_MSGR_UL_FIREWALL_V01;
  }
  else
  {
    add_extd_firewall_config_req_msg_v01.direction = QCMAP_MSGR_DL_FIREWALL_V01;
  }

  switch( firewall_entry->filter_spec.ip_vsn )
  {
    case IP_V4:
      add_extd_firewall_config_req_msg_v01.ip_version = QCMAP_MSGR_IP_FAMILY_V4_V01;
      if ( firewall_entry->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_SRC_ADDR )
      {
        add_extd_firewall_config_req_msg_v01.ip4_src_addr_valid = TRUE;
        add_extd_firewall_config_req_msg_v01.ip4_src_addr.addr =
        firewall_entry->filter_spec.ip_hdr.v4.src.addr.ps_s_addr;
        add_extd_firewall_config_req_msg_v01.ip4_src_addr.subnet_mask =
        firewall_entry->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr;
        LOG_MSG_INFO1("IP4 src addr is:", 0, 0, 0);
        IPV4_ADDR_MSG(add_extd_firewall_config_req_msg_v01.ip4_src_addr.addr);
        LOG_MSG_INFO1("IP4 src subnet mask is:", 0, 0, 0);
        IPV4_ADDR_MSG(add_extd_firewall_config_req_msg_v01.ip4_src_addr.subnet_mask);
      }
      if ( firewall_entry->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_TOS )
      {
        add_extd_firewall_config_req_msg_v01.ip4_tos_valid = TRUE;
        add_extd_firewall_config_req_msg_v01.ip4_tos.value =
        firewall_entry->filter_spec.ip_hdr.v4.tos.val;
        add_extd_firewall_config_req_msg_v01.ip4_tos.mask =
        firewall_entry->filter_spec.ip_hdr.v4.tos.mask;
        LOG_MSG_INFO1( "IP4  TOS value %d  mask %d ",
                        add_extd_firewall_config_req_msg_v01.ip4_tos.value,
                        add_extd_firewall_config_req_msg_v01.ip4_tos.mask, 0 );
      }
      if( firewall_entry->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_NEXT_HDR_PROT )
      {
        add_extd_firewall_config_req_msg_v01.next_hdr_prot_valid = TRUE;
        add_extd_firewall_config_req_msg_v01.next_hdr_prot =
        firewall_entry->filter_spec.ip_hdr.v4.next_hdr_prot;
        next_hdr_prot = firewall_entry->filter_spec.ip_hdr.v4.next_hdr_prot;
      }
      break;

    case IP_V6:
      add_extd_firewall_config_req_msg_v01.ip_version = QCMAP_MSGR_IP_FAMILY_V6_V01;
      if ( firewall_entry->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_SRC_ADDR )
      {
        add_extd_firewall_config_req_msg_v01.ip6_src_addr_valid = TRUE;
        memcpy( add_extd_firewall_config_req_msg_v01.ip6_src_addr.addr,
                firewall_entry->filter_spec.ip_hdr.v6.src.addr.in6_u.u6_addr8,
                QCMAP_MSGR_IPV6_ADDR_LEN_V01 * sizeof(uint8) );
        add_extd_firewall_config_req_msg_v01.ip6_src_addr.prefix_len =
        firewall_entry->filter_spec.ip_hdr.v6.src.prefix_len;
        LOG_MSG_INFO1( "IP6 src addr is", 0, 0, 0 );
        IPV6_ADDR_MSG( firewall_entry->filter_spec.ip_hdr.v6.src.addr.in6_u.u6_addr64 );
        LOG_MSG_INFO1( "IPV6 src prefix length %d ",
                        add_extd_firewall_config_req_msg_v01.ip6_src_addr.prefix_len, 0, 0 );
      }

      if( firewall_entry->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_TRAFFIC_CLASS )
      {
        add_extd_firewall_config_req_msg_v01.ip6_trf_cls_valid = TRUE;
        add_extd_firewall_config_req_msg_v01.ip6_trf_cls.value =
        firewall_entry->filter_spec.ip_hdr.v6.trf_cls.val;
        add_extd_firewall_config_req_msg_v01.ip6_trf_cls.mask =
        firewall_entry->filter_spec.ip_hdr.v6.trf_cls.mask;
        LOG_MSG_INFO1( "IPV6 traffic class value %d mask %d",
                        add_extd_firewall_config_req_msg_v01.ip6_trf_cls.value ,
                        add_extd_firewall_config_req_msg_v01.ip6_trf_cls.mask,0 );
      }

      if( firewall_entry->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_NEXT_HDR_PROT )
      {
        add_extd_firewall_config_req_msg_v01.next_hdr_prot_valid = TRUE;
        add_extd_firewall_config_req_msg_v01.next_hdr_prot =
        firewall_entry->filter_spec.ip_hdr.v6.next_hdr_prot;
        next_hdr_prot = firewall_entry->filter_spec.ip_hdr.v6.next_hdr_prot;
      }
      break;
    default:
      LOG_MSG_INFO1( "Unsupported IP Version %d ", firewall_entry->filter_spec.ip_vsn, 0, 0 );
      *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
      return false;
  }
  LOG_MSG_INFO1( "Next header protocol is %d ", next_hdr_prot, 0, 0 );
  if (add_extd_firewall_config_req_msg_v01.next_hdr_prot_valid)
  {
  switch(next_hdr_prot)
  {
    case PS_IPPROTO_TCP:
      LOG_MSG_INFO1("TCP protocol %d ",add_extd_firewall_config_req_msg_v01.next_hdr_prot, 0, 0);
      if(firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_SRC_PORT )
      {
        add_extd_firewall_config_req_msg_v01.tcp_udp_src_valid = TRUE;
        add_extd_firewall_config_req_msg_v01.tcp_udp_src.port =
        firewall_entry->filter_spec.next_prot_hdr.tcp.src.port;
        add_extd_firewall_config_req_msg_v01.tcp_udp_src.range =
        firewall_entry->filter_spec.next_prot_hdr.tcp.src.range;
        LOG_MSG_INFO1("TCP protocol src port %d src range %d",
                       add_extd_firewall_config_req_msg_v01.tcp_udp_src.port,
                       add_extd_firewall_config_req_msg_v01.tcp_udp_src.range,
                       0);
      }
      if(firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT )
      {
        add_extd_firewall_config_req_msg_v01.tcp_udp_dst_valid = TRUE;
        add_extd_firewall_config_req_msg_v01.tcp_udp_dst.port =
        firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port;
        add_extd_firewall_config_req_msg_v01.tcp_udp_dst.range =
        firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range;
        LOG_MSG_INFO1("TCP protocol dst port %d dst range %d",
                       add_extd_firewall_config_req_msg_v01.tcp_udp_dst.port,
                       add_extd_firewall_config_req_msg_v01.tcp_udp_dst.range,
                       0);
      }
      break;

    case PS_IPPROTO_UDP:
      LOG_MSG_INFO1("UDP protocol %d ",add_extd_firewall_config_req_msg_v01.next_hdr_prot, 0, 0);
      if(firewall_entry->filter_spec.next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_SRC_PORT )
      {
        add_extd_firewall_config_req_msg_v01.tcp_udp_src_valid = TRUE;
        add_extd_firewall_config_req_msg_v01.tcp_udp_src.port =
        firewall_entry->filter_spec.next_prot_hdr.udp.src.port;
        add_extd_firewall_config_req_msg_v01.tcp_udp_src.range =
        firewall_entry->filter_spec.next_prot_hdr.udp.src.range;
        LOG_MSG_INFO1("UDP protocol src port %d src range %d",
                       add_extd_firewall_config_req_msg_v01.tcp_udp_src.port,
                       add_extd_firewall_config_req_msg_v01.tcp_udp_src.range,0);
      }
      if(firewall_entry->filter_spec.next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_DST_PORT )
      {
        add_extd_firewall_config_req_msg_v01.tcp_udp_dst_valid = TRUE;
        add_extd_firewall_config_req_msg_v01.tcp_udp_dst.port =
        firewall_entry->filter_spec.next_prot_hdr.udp.dst.port;
        add_extd_firewall_config_req_msg_v01.tcp_udp_dst.range =
        firewall_entry->filter_spec.next_prot_hdr.udp.dst.range;
        LOG_MSG_INFO1("UDP protocol dst port %d dst range %d",
                       add_extd_firewall_config_req_msg_v01.tcp_udp_dst.port,
                       add_extd_firewall_config_req_msg_v01.tcp_udp_dst.range,0);
      }
      break;

    case PS_IPPROTO_ICMP:
    case PS_IPPROTO_ICMP6:
      LOG_MSG_INFO1("ICMP protocol %d ", add_extd_firewall_config_req_msg_v01.next_hdr_prot, 0, 0);
      if(firewall_entry->filter_spec.next_prot_hdr.icmp.field_mask & IPFLTR_MASK_ICMP_MSG_CODE )
      {
        add_extd_firewall_config_req_msg_v01.icmp_code_valid = TRUE;
        add_extd_firewall_config_req_msg_v01.icmp_code =
        firewall_entry->filter_spec.next_prot_hdr.icmp.code;
        LOG_MSG_INFO1("ICMP protocol code %d", add_extd_firewall_config_req_msg_v01.icmp_code, 0, 0);
      }
      if(firewall_entry->filter_spec.next_prot_hdr.icmp.field_mask & IPFLTR_MASK_ICMP_MSG_TYPE )
      {
        add_extd_firewall_config_req_msg_v01.icmp_type_valid = TRUE;
        add_extd_firewall_config_req_msg_v01.icmp_type =
        firewall_entry->filter_spec.next_prot_hdr.icmp.type;
        LOG_MSG_INFO1("ICMP protocol type %d", add_extd_firewall_config_req_msg_v01.icmp_type, 0, 0);
      }
      break;

    case PS_IPPROTO_ESP:
      LOG_MSG_INFO1("ESP protocol %d ", add_extd_firewall_config_req_msg_v01.next_hdr_prot, 0, 0);
      if(firewall_entry->filter_spec.next_prot_hdr.esp.field_mask & IPFLTR_MASK_ESP_SPI )
      {
        add_extd_firewall_config_req_msg_v01.esp_spi_valid = TRUE;
        add_extd_firewall_config_req_msg_v01.esp_spi = firewall_entry->filter_spec.next_prot_hdr.esp.spi;
        LOG_MSG_INFO1("ESP protocol spi %d", add_extd_firewall_config_req_msg_v01.esp_spi, 0, 0);
      }
      break;

    case PS_IPPROTO_TCP_UDP:
      LOG_MSG_INFO1("TCP_UDP protocol %d ", add_extd_firewall_config_req_msg_v01.next_hdr_prot, 0, 0);
      if(firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.field_mask & IPFLTR_MASK_TCP_UDP_SRC_PORT )
      {
        add_extd_firewall_config_req_msg_v01.tcp_udp_src_valid = TRUE;
        add_extd_firewall_config_req_msg_v01.tcp_udp_src.port =
        firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.src.port;
        add_extd_firewall_config_req_msg_v01.tcp_udp_src.range =
        firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.src.range;
        LOG_MSG_INFO1("TCP_UDP protocol src port %d src range",
                       add_extd_firewall_config_req_msg_v01.tcp_udp_src.port,
                       add_extd_firewall_config_req_msg_v01.tcp_udp_src.range,0);
      }

      if(firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.field_mask & IPFLTR_MASK_TCP_UDP_DST_PORT )
      {
        add_extd_firewall_config_req_msg_v01.tcp_udp_dst_valid = TRUE;
        add_extd_firewall_config_req_msg_v01.tcp_udp_dst.port =
        firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.port;
        add_extd_firewall_config_req_msg_v01.tcp_udp_dst.range =
        firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.range;
        LOG_MSG_INFO1("TCP_UDP protocol dst port %d dst range",
                       add_extd_firewall_config_req_msg_v01.tcp_udp_src.port,
                       add_extd_firewall_config_req_msg_v01.tcp_udp_src.range,0);
      }
      break;

    default:
      LOG_MSG_ERROR("Unsupported protocol %d ",next_hdr_prot, 0, 0);
      *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
      return false;
  }
  }

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,QMI_QCMAP_MSGR_ADD_FIREWALL_ENTRY_REQ_V01,
                                       &add_extd_firewall_config_req_msg_v01,
                                       sizeof(add_extd_firewall_config_req_msg_v01),
                                       &add_extd_firewall_config_resp_msg_v01,
                                       sizeof(add_extd_firewall_config_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);
  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( add_extd_firewall_config_resp_msg_v01.resp.result != QMI_NO_ERR ) ||
       ( add_extd_firewall_config_resp_msg_v01.firewall_handle_valid != TRUE) )
  {
    LOG_MSG_ERROR( "Add firewall config failed %d %d", qmi_error,
                   add_extd_firewall_config_resp_msg_v01.resp.error,0);
    *qmi_err_num = add_extd_firewall_config_resp_msg_v01.resp.error;
    if(add_extd_firewall_config_resp_msg_v01.resp.error == QMI_ERR_NO_EFFECT_V01 )
    {
      LOG_MSG_ERROR("Entry ALL Ready present ",0,0,0);
    }
    else if( add_extd_firewall_config_resp_msg_v01.resp.error == QMI_ERR_INSUFFICIENT_RESOURCES_V01)
    {
      LOG_MSG_ERROR("Maximium entry Added",0,0,0);
    }
    return false;
  }
  else
  {
    LOG_MSG_INFO1("\nAdd the firewall entry and handle is =%d \n",
                    add_extd_firewall_config_resp_msg_v01.firewall_handle,0,0);
    firewall_entry->firewall_handle = add_extd_firewall_config_resp_msg_v01.firewall_handle;
  }
  LOG_MSG_INFO1("Added FIREWALL Entry...",0,0,0);
  return true;
}

/*===========================================================================
  FUNCTION UPNPPinholeEntry
  ===========================================================================*/
/*!
  @brief
  Encode a firewall configuration into a msgr message and sends the same to
  QCMAP connection manager to add firewall configuration

  @return
   true  on success.
   false on failure
  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::AddUPNPPinholeEntry
(
  qcmap_msgr_firewall_conf_t *firewall_conf,
  qmi_error_type_v01 *qmi_err_num,
  boolean upnp_pinhole
)
{
  qcmap_msgr_add_firewall_entry_req_msg_v01 add_extd_firewall_config_req_msg_v01;
  qcmap_msgr_add_firewall_entry_resp_msg_v01 add_extd_firewall_config_resp_msg_v01;
  int next_hdr_prot = 0;
  qmi_client_error_type qmi_error;
  int inc;
  qcmap_msgr_firewall_entry_conf_t *firewall_entry;

  ds_assert(firewall_conf != NULL);
  memset(&add_extd_firewall_config_req_msg_v01, 0,
         sizeof(add_extd_firewall_config_req_msg_v01));
  firewall_entry = &firewall_conf->extd_firewall_entry;

  add_extd_firewall_config_req_msg_v01.mobile_ap_handle = this->mobile_ap_handle;

    add_extd_firewall_config_req_msg_v01.upnp_pinhole= upnp_pinhole;
    add_extd_firewall_config_req_msg_v01.upnp_pinhole_valid= TRUE;

  LOG_MSG_INFO1("IP family type %d, Direction %d ",
                firewall_entry->filter_spec.ip_vsn, firewall_entry->firewall_direction, 0);

  switch( firewall_entry->filter_spec.ip_vsn )
  {
    case IP_V4:

     add_extd_firewall_config_req_msg_v01.ip_version = QCMAP_MSGR_IP_FAMILY_V4_V01;

     if ( firewall_entry->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_SRC_ADDR )
      {
        add_extd_firewall_config_req_msg_v01.ip4_src_addr_valid = TRUE;
        add_extd_firewall_config_req_msg_v01.ip4_src_addr.addr =
          firewall_entry->filter_spec.ip_hdr.v4.src.addr.ps_s_addr;
        add_extd_firewall_config_req_msg_v01.ip4_src_addr.subnet_mask =
          firewall_entry->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr;
        LOG_MSG_INFO1("IP4 src addr is:", 0, 0, 0);
        IPV4_ADDR_MSG(add_extd_firewall_config_req_msg_v01.ip4_src_addr.addr);
        LOG_MSG_INFO1("IP4 src subnet mask is:", 0, 0, 0);
        IPV4_ADDR_MSG(add_extd_firewall_config_req_msg_v01.ip4_src_addr.subnet_mask);
      }

      if ( firewall_entry->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_TOS )
      {
        add_extd_firewall_config_req_msg_v01.ip4_tos_valid = TRUE;
        add_extd_firewall_config_req_msg_v01.ip4_tos.value =
          firewall_entry->filter_spec.ip_hdr.v4.tos.val;
        add_extd_firewall_config_req_msg_v01.ip4_tos.mask =
          firewall_entry->filter_spec.ip_hdr.v4.tos.mask;
        LOG_MSG_INFO1( "IP4  TOS value %d  mask %d ",
                       add_extd_firewall_config_req_msg_v01.ip4_tos.value,
                       add_extd_firewall_config_req_msg_v01.ip4_tos.mask, 0 );
      }

      if( firewall_entry->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_NEXT_HDR_PROT )
      {
        add_extd_firewall_config_req_msg_v01.next_hdr_prot_valid = TRUE;
        add_extd_firewall_config_req_msg_v01.next_hdr_prot =
           firewall_entry->filter_spec.ip_hdr.v4.next_hdr_prot;
        next_hdr_prot = firewall_entry->filter_spec.ip_hdr.v4.next_hdr_prot;
      }
      break;

   case IP_V6:

      add_extd_firewall_config_req_msg_v01.ip_version = QCMAP_MSGR_IP_FAMILY_V6_V01;

      if ( firewall_entry->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_SRC_ADDR )
      {
        add_extd_firewall_config_req_msg_v01.ip6_src_addr_valid = TRUE;
        memcpy( add_extd_firewall_config_req_msg_v01.ip6_src_addr.addr,
                firewall_entry->filter_spec.ip_hdr.v6.src.addr.in6_u.u6_addr8,
                QCMAP_MSGR_IPV6_ADDR_LEN_V01 * sizeof(uint8) );
        add_extd_firewall_config_req_msg_v01.ip6_src_addr.prefix_len =
         firewall_entry->filter_spec.ip_hdr.v6.src.prefix_len;
        LOG_MSG_INFO1( "IP6 src addr is", 0, 0, 0 );
        IPV6_ADDR_MSG( firewall_entry->filter_spec.ip_hdr.v6.src.addr.in6_u.u6_addr64 );
        LOG_MSG_INFO1( "IPV6 src prefix length %d ",
                       add_extd_firewall_config_req_msg_v01.ip6_src_addr.prefix_len, 0, 0 );
      }


      if ( firewall_entry->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_DST_ADDR && upnp_pinhole)
      {
        add_extd_firewall_config_req_msg_v01.ip6_dst_addr_valid= TRUE;
        memcpy( add_extd_firewall_config_req_msg_v01.ip6_dst_addr.addr,
                firewall_entry->filter_spec.ip_hdr.v6.dst.addr.in6_u.u6_addr8,
                QCMAP_MSGR_IPV6_ADDR_LEN_V01 * sizeof(uint8) );
        add_extd_firewall_config_req_msg_v01.ip6_dst_addr.prefix_len=
         firewall_entry->filter_spec.ip_hdr.v6.dst.prefix_len;
        LOG_MSG_INFO1( "IP6 dst addr is", 0, 0, 0 );
        IPV6_ADDR_MSG( firewall_entry->filter_spec.ip_hdr.v6.dst.addr.in6_u.u6_addr64);
        LOG_MSG_INFO1( "IPV6 dst prefix length %d ",
                       add_extd_firewall_config_req_msg_v01.ip6_dst_addr.prefix_len, 0, 0 );
      }

      if( firewall_entry->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_TRAFFIC_CLASS )
      {
        add_extd_firewall_config_req_msg_v01.ip6_trf_cls_valid = TRUE;
        add_extd_firewall_config_req_msg_v01.ip6_trf_cls.value =
          firewall_entry->filter_spec.ip_hdr.v6.trf_cls.val;
        add_extd_firewall_config_req_msg_v01.ip6_trf_cls.mask =
          firewall_entry->filter_spec.ip_hdr.v6.trf_cls.mask;
        LOG_MSG_INFO1( "IPV6 traffic class value %d mask %d",
                       add_extd_firewall_config_req_msg_v01.ip6_trf_cls.value ,
                       add_extd_firewall_config_req_msg_v01.ip6_trf_cls.mask, 0 );
      }

      if( firewall_entry->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_NEXT_HDR_PROT )
      {
        add_extd_firewall_config_req_msg_v01.next_hdr_prot_valid = TRUE;
        add_extd_firewall_config_req_msg_v01.next_hdr_prot =
           firewall_entry->filter_spec.ip_hdr.v6.next_hdr_prot;
        next_hdr_prot = firewall_entry->filter_spec.ip_hdr.v6.next_hdr_prot;
      }

      break;

    default:
      LOG_MSG_INFO1( "Unsupported IP Version %d ",
                     firewall_entry->filter_spec.ip_vsn, 0, 0 );
      *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
      return false;
  }



  LOG_MSG_INFO1( "Next header protocol is %d ",
                 next_hdr_prot, 0, 0 );
  if (add_extd_firewall_config_req_msg_v01.next_hdr_prot_valid)
  {
  switch(next_hdr_prot)
  {
    case PS_IPPROTO_TCP:
      LOG_MSG_INFO1("TCP protocol %d ",
                add_extd_firewall_config_req_msg_v01.next_hdr_prot, 0, 0);
      if(firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask &
                                                     IPFLTR_MASK_TCP_SRC_PORT )
      {
        add_extd_firewall_config_req_msg_v01.tcp_udp_src_valid = TRUE;
        add_extd_firewall_config_req_msg_v01.tcp_udp_src.port =
          firewall_entry->filter_spec.next_prot_hdr.tcp.src.port;
        add_extd_firewall_config_req_msg_v01.tcp_udp_src.range =
          firewall_entry->filter_spec.next_prot_hdr.tcp.src.range;
        LOG_MSG_INFO1("TCP protocol src port %d src range %d",
                        add_extd_firewall_config_req_msg_v01.tcp_udp_src.port,
                        add_extd_firewall_config_req_msg_v01.tcp_udp_src.range,
                        0);
      }

      if(firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask &
                                                      IPFLTR_MASK_TCP_DST_PORT )
      {
        add_extd_firewall_config_req_msg_v01.tcp_udp_dst_valid = TRUE;
        add_extd_firewall_config_req_msg_v01.tcp_udp_dst.port =
                         firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port;
        add_extd_firewall_config_req_msg_v01.tcp_udp_dst.range =
                        firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range;
        LOG_MSG_INFO1("TCP protocol dst port %d dst range %d",
                      add_extd_firewall_config_req_msg_v01.tcp_udp_dst.port,
                      add_extd_firewall_config_req_msg_v01.tcp_udp_dst.range,
                      0);
      }
      break;

    case PS_IPPROTO_UDP:
      LOG_MSG_INFO1("UDP protocol %d ",
                      add_extd_firewall_config_req_msg_v01.next_hdr_prot, 0, 0);
      if(firewall_entry->filter_spec.next_prot_hdr.udp.field_mask &
                                                     IPFLTR_MASK_UDP_SRC_PORT )
      {
        add_extd_firewall_config_req_msg_v01.tcp_udp_src_valid = TRUE;
        add_extd_firewall_config_req_msg_v01.tcp_udp_src.port =
                        firewall_entry->filter_spec.next_prot_hdr.udp.src.port;
        add_extd_firewall_config_req_msg_v01.tcp_udp_src.range =
                       firewall_entry->filter_spec.next_prot_hdr.udp.src.range;
        LOG_MSG_INFO1("UDP protocol src port %d src range %d",
                      add_extd_firewall_config_req_msg_v01.tcp_udp_src.port,
                      add_extd_firewall_config_req_msg_v01.tcp_udp_src.range, 0);
      }

      if(firewall_entry->filter_spec.next_prot_hdr.udp.field_mask &
                                                      IPFLTR_MASK_UDP_DST_PORT )
      {
        add_extd_firewall_config_req_msg_v01.tcp_udp_dst_valid = TRUE;
        add_extd_firewall_config_req_msg_v01.tcp_udp_dst.port =
                         firewall_entry->filter_spec.next_prot_hdr.udp.dst.port;
        add_extd_firewall_config_req_msg_v01.tcp_udp_dst.range =
                        firewall_entry->filter_spec.next_prot_hdr.udp.dst.range;
        LOG_MSG_INFO1("UDP protocol dst port %d dst range %d",
                      add_extd_firewall_config_req_msg_v01.tcp_udp_dst.port,
                      add_extd_firewall_config_req_msg_v01.tcp_udp_dst.range, 0);
      }
      break;

    case PS_IPPROTO_ICMP:
    case PS_IPPROTO_ICMP6:
      LOG_MSG_INFO1("ICMP protocol %d ",
                add_extd_firewall_config_req_msg_v01.next_hdr_prot, 0, 0);
      if(firewall_entry->filter_spec.next_prot_hdr.icmp.field_mask &
                                                      IPFLTR_MASK_ICMP_MSG_CODE )
      {
        add_extd_firewall_config_req_msg_v01.icmp_code_valid = TRUE;
        add_extd_firewall_config_req_msg_v01.icmp_code =
                             firewall_entry->filter_spec.next_prot_hdr.icmp.code;
        LOG_MSG_INFO1("ICMP protocol code %d",
                add_extd_firewall_config_req_msg_v01.icmp_code, 0, 0);
      }

      if(firewall_entry->filter_spec.next_prot_hdr.icmp.field_mask &
                                                      IPFLTR_MASK_ICMP_MSG_TYPE )
      {
        add_extd_firewall_config_req_msg_v01.icmp_type_valid = TRUE;
        add_extd_firewall_config_req_msg_v01.icmp_type =
                             firewall_entry->filter_spec.next_prot_hdr.icmp.type;
        LOG_MSG_INFO1("ICMP protocol type %d",
                add_extd_firewall_config_req_msg_v01.icmp_type, 0, 0);
      }
      break;

    case PS_IPPROTO_ESP:
      LOG_MSG_INFO1("ESP protocol %d ",
                add_extd_firewall_config_req_msg_v01.next_hdr_prot, 0, 0);
      if(firewall_entry->filter_spec.next_prot_hdr.esp.field_mask & IPFLTR_MASK_ESP_SPI )
      {
        add_extd_firewall_config_req_msg_v01.esp_spi_valid = TRUE;
        add_extd_firewall_config_req_msg_v01.esp_spi =
                               firewall_entry->filter_spec.next_prot_hdr.esp.spi;
        LOG_MSG_INFO1("ESP protocol spi %d",
                add_extd_firewall_config_req_msg_v01.esp_spi, 0, 0);
      }
      break;

    case PS_IPPROTO_TCP_UDP:
      LOG_MSG_INFO1("TCP_UDP protocol %d ",
                add_extd_firewall_config_req_msg_v01.next_hdr_prot, 0, 0);
      if(firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.field_mask &
                                                     IPFLTR_MASK_TCP_UDP_SRC_PORT )
      {
        add_extd_firewall_config_req_msg_v01.tcp_udp_src_valid = TRUE;
        add_extd_firewall_config_req_msg_v01.tcp_udp_src.port =
             firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.src.port;
        add_extd_firewall_config_req_msg_v01.tcp_udp_src.range =
            firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.src.range;
        LOG_MSG_INFO1("TCP_UDP protocol src port %d src range",
                      add_extd_firewall_config_req_msg_v01.tcp_udp_src.port,
                      add_extd_firewall_config_req_msg_v01.tcp_udp_src.range, 0);
      }

      if(firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.field_mask &
                                                     IPFLTR_MASK_TCP_UDP_DST_PORT )
      {
        add_extd_firewall_config_req_msg_v01.tcp_udp_dst_valid = TRUE;
        add_extd_firewall_config_req_msg_v01.tcp_udp_dst.port =
             firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.port;
        add_extd_firewall_config_req_msg_v01.tcp_udp_dst.range =
            firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.range;
        LOG_MSG_INFO1("TCP_UDP protocol dst port %d dst range",
                      add_extd_firewall_config_req_msg_v01.tcp_udp_src.port,
                      add_extd_firewall_config_req_msg_v01.tcp_udp_src.range, 0);
      }
      break;

    default:
      LOG_MSG_ERROR("Unsupported protocol %d ",next_hdr_prot, 0, 0);
      *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
      return false;
  }
  }

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_ADD_FIREWALL_ENTRY_REQ_V01,
                                       &add_extd_firewall_config_req_msg_v01,
                                       sizeof(add_extd_firewall_config_req_msg_v01),
                                       &add_extd_firewall_config_resp_msg_v01,
                                       sizeof(add_extd_firewall_config_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);
  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( add_extd_firewall_config_resp_msg_v01.resp.result != QMI_NO_ERR ) ||
       ( add_extd_firewall_config_resp_msg_v01.firewall_handle_valid != TRUE ) )
  {
    LOG_MSG_ERROR( "Add firewall config failed %d %d", qmi_error,
                   add_extd_firewall_config_resp_msg_v01.resp.error,0);
    *qmi_err_num = add_extd_firewall_config_resp_msg_v01.resp.error;

    if(add_extd_firewall_config_resp_msg_v01.resp.error == QMI_ERR_NO_EFFECT_V01 )
   {
     LOG_MSG_ERROR("Entry ALL Ready present ",0,0,0);
   }else if( add_extd_firewall_config_resp_msg_v01.resp.error == QMI_ERR_INSUFFICIENT_RESOURCES_V01)
   {
     LOG_MSG_ERROR("Maximium entry Added",0,0,0);
   }
   LOG_MSG_INFO1("\nAdd when backhaul down =%d \n", add_extd_firewall_config_resp_msg_v01.firewall_handle,0,0);
   firewall_conf->extd_firewall_entry.firewall_handle = add_extd_firewall_config_resp_msg_v01.firewall_handle;
   return false;
  }
  else
  {
    LOG_MSG_INFO1("\nAdd the firewall entry and handle is =%d \n", add_extd_firewall_config_resp_msg_v01.firewall_handle,0,0);
    firewall_conf->extd_firewall_entry.firewall_handle = add_extd_firewall_config_resp_msg_v01.firewall_handle;
  }
  LOG_MSG_INFO1("Added FIREWALL Entry...",0,0,0);
  return true;
}


/*===========================================================================
  FUNCTION GetFireWallHandleList
  ===========================================================================*/
/*!
  @brief
  Gets the firewall handle list
  @return
   true  on success.
   false on failure
  @note
  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::GetFireWallHandlesList
(
  qcmap_msgr_get_firewall_handle_list_conf_t *handlelist,
  qmi_error_type_v01 *qmi_err_num
)
{
  int ret = QCMAP_CM_SUCCESS;
  qmi_client_error_type qmi_error;
  qcmap_msgr_get_firewall_entries_handle_list_req_msg_v01 get_firewall_config_handle_list_req;
  qcmap_msgr_get_firewall_entries_handle_list_resp_msg_v01 get_firewall_config_handle_list_resp;
  uint32 i;
  uint32 mask,domain;
  get_firewall_config_handle_list_req.mobile_ap_handle =this->mobile_ap_handle;

  if (  handlelist->ip_family == IP_V4 )
   get_firewall_config_handle_list_req.ip_version = QCMAP_MSGR_IP_FAMILY_V4_V01;
  else if ( handlelist->ip_family == IP_V6 )
   get_firewall_config_handle_list_req.ip_version = QCMAP_MSGR_IP_FAMILY_V6_V01;
  else
  {
    LOG_MSG_ERROR("Unsupported IP version %d ",handlelist->ip_family, 0, 0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_FIREWALL_ENTRIES_HANDLE_LIST_REQ_V01,
                                       &get_firewall_config_handle_list_req,
                                       sizeof(get_firewall_config_handle_list_req),
                                       &get_firewall_config_handle_list_resp,
                                       sizeof(get_firewall_config_handle_list_resp),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);
  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_firewall_config_handle_list_resp.resp.result != QMI_NO_ERR ) )
  {
    ret = QCMAP_CM_ERROR;
    LOG_MSG_ERROR( "Get firewall config failed %d %d", qmi_error,
       get_firewall_config_handle_list_resp.resp.error,0);
    *qmi_err_num = get_firewall_config_handle_list_resp.resp.error;
    return false;
  }
  else
  {
    LOG_MSG_INFO1("\nGet Firewall handles list \n",0,0,0);
    handlelist->num_of_entries=get_firewall_config_handle_list_resp.firewall_handle_list_len;
    if( handlelist->num_of_entries > 0 )
    {
      for( i=0; i < handlelist->num_of_entries ; i++ )
      {
        handlelist->handle_list[i]=get_firewall_config_handle_list_resp.firewall_handle_list[i];
      }
    }
    else
    {
      LOG_MSG_INFO1("\nNo Firewall rules present\n",0,0,0);
    }
    return true;
  }
}


/*===========================================================================
  FUNCTION GetFireWallEntry
  ===========================================================================*/
/*!
  @brief
  Gets the Firewall Configuration from  QCMAP connection manager  and
  decodes the message .

  @return
   true  on success.
   false on failure
  @note
  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::GetFireWallEntry
(
  qcmap_msgr_firewall_entry_conf_t *firewall_entry,
  qmi_error_type_v01 *qmi_err_num
)
{
  int ret = QCMAP_CM_SUCCESS;
  qmi_client_error_type qmi_error;
  qcmap_msgr_get_firewall_entry_req_msg_v01 get_firewall_req;
  qcmap_msgr_get_firewall_entry_resp_msg_v01 get_firewall_resp;
  uint32 i;
  int next_hdr_prot;
  memset(&get_firewall_req,0,sizeof(get_firewall_req));
  memset(&get_firewall_resp,0,sizeof(get_firewall_resp));

  get_firewall_req.mobile_ap_handle =this->mobile_ap_handle;
  get_firewall_req.firewall_handle=firewall_entry->firewall_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_FIREWALL_ENTRY_REQ_V01,
                                       &get_firewall_req,
                                       sizeof(get_firewall_req),
                                       &get_firewall_resp,
                                       sizeof(get_firewall_resp),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);
  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_firewall_resp.resp.result != QMI_NO_ERR ) )
  {
    ret = QCMAP_CM_ERROR;
    LOG_MSG_ERROR( "Get firewall config failed %d %d", qmi_error,get_firewall_resp.resp.error,0);
    *qmi_err_num = get_firewall_resp.resp.error;
    return false;
  }
  else
  {
    LOG_MSG_INFO1("\nGet FireWallEntries .... \n",0,0,0);
    LOG_MSG_INFO1("IP family type = %d, Direction = %d",
                   get_firewall_resp.ip_version, get_firewall_resp.direction, 0);
    firewall_entry->firewall_direction = (qcmap_msgr_firewall_direction)get_firewall_resp.direction;
    switch(get_firewall_resp.ip_version)
    {
      case IP_V4:
        firewall_entry->filter_spec.ip_vsn = (ip_version_enum_type)get_firewall_resp.ip_version;
        if(get_firewall_resp.ip4_src_addr_valid)
        {
          firewall_entry->filter_spec.ip_hdr.v4.field_mask |=
            IPFLTR_MASK_IP4_SRC_ADDR;
          firewall_entry->filter_spec.ip_hdr.v4.src.addr.ps_s_addr =
            get_firewall_resp.ip4_src_addr.addr;
          firewall_entry->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr =
            get_firewall_resp.ip4_src_addr.subnet_mask;

          LOG_MSG_INFO1("IPV4 src addr is:", 0, 0, 0);
          IPV4_ADDR_MSG(firewall_entry->filter_spec.ip_hdr.v4.src.addr.ps_s_addr);

          LOG_MSG_INFO1("IPV4 src subnet mask", 0, 0, 0);
          IPV4_ADDR_MSG(firewall_entry->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr);
        }

        if(get_firewall_resp.ip4_dst_addr_valid)
        {
          firewall_entry->filter_spec.ip_hdr.v4.field_mask |=
            IPFLTR_MASK_IP4_DST_ADDR;
          firewall_entry->filter_spec.ip_hdr.v4.dst.addr.ps_s_addr =
            get_firewall_resp.ip4_dst_addr.addr;
          firewall_entry->filter_spec.ip_hdr.v4.dst.subnet_mask.ps_s_addr =
            get_firewall_resp.ip4_dst_addr.subnet_mask;

          LOG_MSG_INFO1("IPV4 dst addr is:", 0, 0, 0);
          IPV4_ADDR_MSG(firewall_entry->filter_spec.ip_hdr.v4.dst.addr.ps_s_addr);

          LOG_MSG_INFO1("IPV4 dst subnet mask", 0, 0, 0);
          IPV4_ADDR_MSG(firewall_entry->filter_spec.ip_hdr.v4.dst.subnet_mask.ps_s_addr);
        }

        if(get_firewall_resp.ip4_tos_valid)
        {
          firewall_entry->filter_spec.ip_hdr.v4.field_mask |=
            IPFLTR_MASK_IP4_TOS;
          firewall_entry->filter_spec.ip_hdr.v4.tos.val =
            get_firewall_resp.ip4_tos.value;
          firewall_entry->filter_spec.ip_hdr.v4.tos.mask =
            get_firewall_resp.ip4_tos.mask;
          LOG_MSG_INFO1("tos val %d mask %d",
              firewall_entry->filter_spec.ip_hdr.v4.tos.val,
              firewall_entry->filter_spec.ip_hdr.v4.tos.mask, 0);
        }
        break;

      case IP_V6:
        firewall_entry->filter_spec.ip_vsn = (ip_version_enum_type)get_firewall_resp.ip_version;
        if(get_firewall_resp.ip6_src_addr_valid)
        {
          firewall_entry->filter_spec.ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_SRC_ADDR;
          memcpy(firewall_entry->filter_spec.ip_hdr.v6.src.addr.in6_u.u6_addr8,
              get_firewall_resp.ip6_src_addr.addr,
              QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));

          firewall_entry->filter_spec.ip_hdr.v6.src.prefix_len =
            get_firewall_resp.ip6_src_addr.prefix_len;
          LOG_MSG_INFO1("IPV6 src addr is:", 0, 0, 0);
          IPV6_ADDR_MSG(firewall_entry->filter_spec.ip_hdr.v6.src.addr.in6_u.u6_addr64);
          LOG_MSG_INFO1("IPV6 src prefix length %d ",
              firewall_entry->filter_spec.ip_hdr.v6.src.prefix_len, 0, 0);
        }

        if(get_firewall_resp.ip6_dst_addr_valid)
        {
          firewall_entry->filter_spec.ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_DST_ADDR;

          memcpy(firewall_entry->filter_spec.ip_hdr.v6.dst.addr.in6_u.u6_addr8,
              get_firewall_resp.ip6_dst_addr.addr,
              QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));

          firewall_entry->filter_spec.ip_hdr.v6.dst.prefix_len = get_firewall_resp.ip6_dst_addr.prefix_len;

          LOG_MSG_INFO1("IPV6 dst addr is:", 0, 0, 0);
          IPV6_ADDR_MSG(firewall_entry->filter_spec.ip_hdr.v6.dst.addr.in6_u.u6_addr64);

          LOG_MSG_INFO1("IPV6 src prefix length %d ",
              firewall_entry->filter_spec.ip_hdr.v6.dst.prefix_len, 0, 0);
        }

        if(get_firewall_resp.ip6_trf_cls_valid)
        {
          firewall_entry->filter_spec.ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_TRAFFIC_CLASS;
          firewall_entry->filter_spec.ip_hdr.v6.trf_cls.val = get_firewall_resp.ip6_trf_cls.value;
          firewall_entry->filter_spec.ip_hdr.v6.trf_cls.mask = get_firewall_resp.ip6_trf_cls.mask;
          LOG_MSG_INFO1("ip6 trf class value %d mask %d",
              firewall_entry->filter_spec.ip_hdr.v6.trf_cls.val, firewall_entry->filter_spec.ip_hdr.v6.trf_cls.mask, 0);
        }
        break;

      default:
        LOG_MSG_ERROR("BAD IP protocol", 0, 0, 0);
        return QCMAP_CM_ERROR;
    }

    if(get_firewall_resp.next_hdr_prot_valid)
    {
      LOG_MSG_INFO1("Next header protocol is %d ",
          get_firewall_resp.next_hdr_prot, 0, 0);
      switch(get_firewall_resp.next_hdr_prot)
      {
        case PS_IPPROTO_TCP:
          if(get_firewall_resp.tcp_udp_src_valid)
          {
            firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask |=
              IPFLTR_MASK_TCP_SRC_PORT;
            firewall_entry->filter_spec.next_prot_hdr.tcp.src.port =
              get_firewall_resp.tcp_udp_src.port;
            firewall_entry->filter_spec.next_prot_hdr.tcp.src.range =
              get_firewall_resp.tcp_udp_src.range;
            LOG_MSG_INFO1("TCP src port %d range %d",
                firewall_entry->filter_spec.next_prot_hdr.tcp.src.port,
                firewall_entry->filter_spec.next_prot_hdr.tcp.src.range,
                0);
          }

          if(get_firewall_resp.tcp_udp_dst_valid)
          {
            firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask |=
              IPFLTR_MASK_TCP_DST_PORT;
            firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port =
              get_firewall_resp.tcp_udp_dst.port;
            firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range =
              get_firewall_resp.tcp_udp_dst.range;
            LOG_MSG_INFO1("TCP dst port %d range %d",
                firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port,
                firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range,
                0);
          }
          break;

        case PS_IPPROTO_UDP:
          if(get_firewall_resp.tcp_udp_src_valid)
          {
            firewall_entry->filter_spec.next_prot_hdr.udp.field_mask |=
              IPFLTR_MASK_UDP_SRC_PORT;
            firewall_entry->filter_spec.next_prot_hdr.udp.src.port =
              get_firewall_resp.tcp_udp_src.port;
            firewall_entry->filter_spec.next_prot_hdr.udp.src.range =
              get_firewall_resp.tcp_udp_src.range;
            LOG_MSG_INFO1("UDP src port %d range %d",
                firewall_entry->filter_spec.next_prot_hdr.udp.src.port,
                firewall_entry->filter_spec.next_prot_hdr.udp.src.range,
                0);
          }

          if(get_firewall_resp.tcp_udp_dst_valid)
          {
            firewall_entry->filter_spec.next_prot_hdr.udp.field_mask |=
              IPFLTR_MASK_UDP_DST_PORT;
            firewall_entry->filter_spec.next_prot_hdr.udp.dst.port =
              get_firewall_resp.tcp_udp_dst.port;
            firewall_entry->filter_spec.next_prot_hdr.udp.dst.range =
              get_firewall_resp.tcp_udp_dst.range;
            LOG_MSG_INFO1("UDP dst port %d range %d",
                firewall_entry->filter_spec.next_prot_hdr.udp.dst.port,
                firewall_entry->filter_spec.next_prot_hdr.udp.dst.range,
                0);
          }
          break;

        case PS_IPPROTO_ICMP:
        case PS_IPPROTO_ICMP6:
          if(get_firewall_resp.icmp_code_valid)
          {
            firewall_entry->filter_spec.next_prot_hdr.icmp.field_mask |=
              IPFLTR_MASK_ICMP_MSG_CODE;
            firewall_entry->filter_spec.next_prot_hdr.icmp.code =
              get_firewall_resp.icmp_code;
            LOG_MSG_INFO1("ICMP code %d",
                firewall_entry->filter_spec.next_prot_hdr.icmp.code, 0, 0);
          }

          if(get_firewall_resp.icmp_type_valid)
          {
            firewall_entry->filter_spec.next_prot_hdr.icmp.field_mask |=
              IPFLTR_MASK_ICMP_MSG_TYPE;
            firewall_entry->filter_spec.next_prot_hdr.icmp.type =
              get_firewall_resp.icmp_type;
            LOG_MSG_INFO1("ICMP type %d",
                firewall_entry->filter_spec.next_prot_hdr.icmp.type, 0, 0);
          }
          break;

        case PS_IPPROTO_ESP:
          if(get_firewall_resp.esp_spi_valid)
          {
            firewall_entry->filter_spec.next_prot_hdr.esp.field_mask |=
              IPFLTR_MASK_ESP_SPI;
            firewall_entry->filter_spec.next_prot_hdr.esp.spi =
              get_firewall_resp.esp_spi;
            LOG_MSG_INFO1("ESP SPI %d",
                firewall_entry->filter_spec.next_prot_hdr.esp.spi, 0, 0);
          }
          break;

        case PS_IPPROTO_TCP_UDP:
          if(get_firewall_resp.tcp_udp_src_valid)
          {
            firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.field_mask |=
              IPFLTR_MASK_TCP_UDP_SRC_PORT;
            firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.src.port =
              get_firewall_resp.tcp_udp_src.port;
            firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.src.range =
              get_firewall_resp.tcp_udp_src.range;
            LOG_MSG_INFO1("TCP_UDP src port %d range %d",
                firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.src.port,
                firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.src.range,
                0);
          }

          if(get_firewall_resp.tcp_udp_dst_valid)
          {
            firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.field_mask |=
              IPFLTR_MASK_TCP_UDP_DST_PORT;
            firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.port =
              get_firewall_resp.tcp_udp_dst.port;
            firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.range =
              get_firewall_resp.tcp_udp_dst.range;
            LOG_MSG_INFO1("TCP_UDP dst port %d range %d",
                firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.port,
                firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.range,
                0);
          }
          break;

        default:
          LOG_MSG_INFO1("Bad next header protocol %d",
              get_firewall_resp.next_hdr_prot , 0, 0);
      }

      LOG_MSG_INFO1("Next header protocol is %d ",
          next_hdr_prot, 0, 0);

      if( (firewall_entry->filter_spec.ip_vsn == IP_V4) && (get_firewall_resp.next_hdr_prot_valid) )
      {
        firewall_entry->filter_spec.ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_NEXT_HDR_PROT;
        firewall_entry->filter_spec.ip_hdr.v4.next_hdr_prot = get_firewall_resp.next_hdr_prot;
      }
      else if( (firewall_entry->filter_spec.ip_vsn = IP_V6) && ( get_firewall_resp.next_hdr_prot_valid ) )
      {
        firewall_entry->filter_spec.ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_NEXT_HDR_PROT;
        firewall_entry->filter_spec.ip_hdr.v6.next_hdr_prot = get_firewall_resp.next_hdr_prot;
      }
    }
  }
  return true;
}


/*==========================================================================
  FUNCTION DeleteFireWallEntry
  ===========================================================================*/
/*!
  @brief
  Sends delete message for firewall entry identified by the handle
  @return
   true  on success.
   false on failure
  @note
  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
int QCMAP_Client::DeleteFireWallEntry(int handle, qmi_error_type_v01  *qmi_err_num)
{
  qcmap_msgr_delete_firewall_entry_req_msg_v01 delete_firewall_entry_req_msg;
  qcmap_msgr_delete_firewall_entry_resp_msg_v01 delete_firewall_entry_resp_msg;

  qmi_client_error_type qmi_error;
  QCMAP_LOG_FUNC_ENTRY();
  delete_firewall_entry_req_msg.mobile_ap_handle =this->mobile_ap_handle;
  delete_firewall_entry_req_msg.firewall_handle=handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_DELETE_FIREWALL_ENTRY_REQ_V01,
                                       &delete_firewall_entry_req_msg,
                                       sizeof(qcmap_msgr_delete_firewall_entry_req_msg_v01),
                                       &delete_firewall_entry_resp_msg,
                                       sizeof(qcmap_msgr_delete_firewall_entry_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( delete_firewall_entry_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Deleted FIREWALL Entry failed",0,0,0);
    *qmi_err_num = delete_firewall_entry_resp_msg.resp.error;
    return false;
  }
  else
  {
    LOG_MSG_ERROR("Deleted FIREWALL Entry success",0,0,0);
  }
  return true;
}

/*===========================================================================
  FUNCTION GetNetworkConfiguration()
  ===========================================================================*/
/*!
  @brief
  Gets the Network configuration

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::GetNetworkConfiguration
(
  qcmap_msgr_ip_family_enum_v01 ip_family,
  qcmap_nw_params_t            *qcmap_nw_params,
  qmi_error_type_v01           *qmi_err_num
)
{
  if ( qmi_err_num == NULL || qcmap_nw_params == NULL )
  {
    LOG_MSG_ERROR(" Null argument passed ",0,0,0);
    return FALSE;
  }

  if ((ip_family != QCMAP_MSGR_IP_FAMILY_V4_V01) &&
      (ip_family != QCMAP_MSGR_IP_FAMILY_V6_V01))
  {
    LOG_MSG_ERROR("GetNetworkConfiguration(): Invalid IP family specified",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_IP_FAMILY_PREF_V01;
    return FALSE;
  }

  *qmi_err_num = QMI_ERR_NONE_V01;

  if (ip_family == QCMAP_MSGR_IP_FAMILY_V4_V01)
  {
    if(!this->GetIPv4NetworkConfiguration(
                              &(qcmap_nw_params->v4_conf.public_ip.s_addr),
                              &(qcmap_nw_params->v4_conf.primary_dns.s_addr),
                              &(qcmap_nw_params->v4_conf.secondary_dns.s_addr),
                              qmi_err_num))
    {
      LOG_MSG_ERROR("Failed to get IPv4 Network Configuration - error = 0x%x",
                    *qmi_err_num,0,0);
      return FALSE;
    }
  }

  if (ip_family == QCMAP_MSGR_IP_FAMILY_V6_V01)
  {
    if(!this->GetIPv6NetworkConfiguration(
                                &qcmap_nw_params->v6_conf.public_ip_v6,
                                &qcmap_nw_params->v6_conf.primary_dns_v6,
                                &qcmap_nw_params->v6_conf.secondary_dns_v6,
                                qmi_err_num))
    {
      LOG_MSG_ERROR("Failed to get IPv6 Network Configuration - error = 0x%x",
                    *qmi_err_num,0,0);
      return FALSE;
    }
  }

  return TRUE;
}

/*===========================================================================
  FUNCTION GetIPv4NetworkConfiguration()
  ===========================================================================*/
/*!
  @brief
  Gets the configuration

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::GetIPv4NetworkConfiguration
(
  in_addr_t          *public_ip,
  uint32             *primary_dns,
  in_addr_t          *secondary_dns,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_get_wwan_config_req_msg_v01 get_wwan_config_req_msg;
  qcmap_msgr_get_wwan_config_resp_msg_v01 get_wwan_config_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  get_wwan_config_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  get_wwan_config_req_msg.addr_type_op = QCMAP_MSGR_MASK_V4_ADDR_V01 |
                                         QCMAP_MSGR_MASK_V4_DNS_ADDR_V01;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_WWAN_CONFIG_REQ_V01,
                                       &get_wwan_config_req_msg,
                                       sizeof(qcmap_msgr_get_wwan_config_req_msg_v01),
                                       &get_wwan_config_resp_msg,
                                       sizeof(qcmap_msgr_get_wwan_config_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_wwan_config_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get Network config %d : %d",
        qmi_error, get_wwan_config_resp_msg.resp.error,0);
    *qmi_err_num = get_wwan_config_resp_msg.resp.error;
    return false;
  }

  if (get_wwan_config_resp_msg.v4_addr_valid)
    *public_ip = get_wwan_config_resp_msg.v4_addr;
  if (get_wwan_config_resp_msg.v4_prim_dns_addr_valid)
    *primary_dns = get_wwan_config_resp_msg.v4_prim_dns_addr;
  if (get_wwan_config_resp_msg.v4_sec_dns_addr_valid)
    *secondary_dns = get_wwan_config_resp_msg.v4_sec_dns_addr;

  LOG_MSG_INFO1("Get Network Config succeeded...",0,0,0);
  return true;
}

/*===========================================================================
  FUNCTION GetIPv6NetworkConfiguration()
  ===========================================================================*/
/*!
  @brief
  Gets the IPv6 configuration

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::GetIPv6NetworkConfiguration
(
  struct in6_addr    *public_ip,
  struct in6_addr    *primary_dns,
  struct in6_addr    *secondary_dns,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_get_wwan_config_req_msg_v01 get_wwan_config_req_msg;
  qcmap_msgr_get_wwan_config_resp_msg_v01 get_wwan_config_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&get_wwan_config_resp_msg, 0,
                         sizeof(qcmap_msgr_get_wwan_config_resp_msg_v01));

  get_wwan_config_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  get_wwan_config_req_msg.addr_type_op = QCMAP_MSGR_MASK_V6_ADDR_V01 |
                                         QCMAP_MSGR_MASK_V6_DNS_ADDR_V01;

  qmi_error = qmi_client_send_msg_sync(
                               this->qmi_qcmap_msgr_handle,
                               QMI_QCMAP_MSGR_GET_WWAN_CONFIG_REQ_V01,
                               &get_wwan_config_req_msg,
                               sizeof(qcmap_msgr_get_wwan_config_req_msg_v01),
                               &get_wwan_config_resp_msg,
                               sizeof(qcmap_msgr_get_wwan_config_resp_msg_v01),
                               QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_wwan_config_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get network config %d : %d",
        qmi_error, get_wwan_config_resp_msg.resp.error,0);
    *qmi_err_num = get_wwan_config_resp_msg.resp.error;
    return false;
  }

  if (get_wwan_config_resp_msg.v6_addr_valid)
    memcpy(&public_ip->s6_addr, &get_wwan_config_resp_msg.v6_addr,
           QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
  if (get_wwan_config_resp_msg.v6_prim_dns_addr_valid)
    memcpy(&primary_dns->s6_addr, &get_wwan_config_resp_msg.v6_prim_dns_addr,
           QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
  if (get_wwan_config_resp_msg.v6_sec_dns_addr_valid)
    memcpy(&secondary_dns->s6_addr, &get_wwan_config_resp_msg.v6_sec_dns_addr,
           QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));

  LOG_MSG_INFO1("Get network Config succeeded...",0,0,0);
  return true;
}

/*===========================================================================
  FUNCTION Get Mobile AP status
  ===========================================================================*/
/*!
  @brief
  Gets Mobile AP status

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::GetMobileAPStatus(qcmap_msgr_mobile_ap_status_enum_v01 *status, qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_mobile_ap_status_req_v01 mobileap_status_req;
  qcmap_msgr_mobile_ap_status_resp_v01 mobileap_status_resp;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&mobileap_status_resp, 0, sizeof(qcmap_msgr_mobile_ap_status_resp_v01));
  mobileap_status_req.mobile_ap_handle = this->mobile_ap_handle;
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_MOBILE_AP_STATUS_REQ_V01,
                                       &mobileap_status_req,
                                       sizeof(qcmap_msgr_mobile_ap_status_req_v01),
                                       (void*)&mobileap_status_resp,
                                       sizeof(qcmap_msgr_mobile_ap_status_resp_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(enable): error %d result %d",
      qmi_error, mobileap_status_resp.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( mobileap_status_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not disable wlan %d : %d",
        qmi_error,mobileap_status_resp.resp.error,0);
    *qmi_err_num = mobileap_status_resp.resp.error;
    return false;
  }

  if(mobileap_status_resp.mobile_ap_status_valid ==1)
  {
    *status = mobileap_status_resp.mobile_ap_status;
    if(mobileap_status_resp.mobile_ap_status == QCMAP_MSGR_MOBILE_AP_STATUS_CONNECTED_V01)
    {
      LOG_MSG_INFO1(" Mobile AP is Connected \n",0,0,0);
    }
    else if(mobileap_status_resp.mobile_ap_status == QCMAP_MSGR_MOBILE_AP_STATUS_DISCONNECTED_V01)
    {
      LOG_MSG_INFO1(" Mobile AP is Disconnected \n",0,0,0);
   }
  }
  return true;
}

/*===========================================================================
  FUNCTION Get WAN status
  ===========================================================================*/
/*!
  @brief
    Gets WAN status

  @return
    true  - on Success
    false - on Failure

  @note

  - Dependencies
    - None

  - Side Effects
    - None
 */
/*=========================================================================*/

boolean QCMAP_Client::GetWWANStatus
(
  qcmap_msgr_wwan_status_enum_v01 *v4_status,
  qcmap_msgr_wwan_status_enum_v01 *v6_status,
  qmi_error_type_v01 *qmi_err_num
  )
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_wwan_status_req_msg_v01 wan_status_req;
  qcmap_msgr_wwan_status_resp_msg_v01 wan_status_resp;
  QCMAP_LOG_FUNC_ENTRY();

  memset(&wan_status_resp, 0, sizeof(qcmap_msgr_wwan_status_resp_msg_v01));
  wan_status_req.mobile_ap_handle = this->mobile_ap_handle;
  wan_status_req.call_type_valid = 1;
  wan_status_req.call_type = QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_WWAN_STATUS_REQ_V01,
                                       &wan_status_req,
                                       sizeof(qcmap_msgr_wwan_status_req_msg_v01),
                                       (void*)&wan_status_resp,
                                       sizeof(qcmap_msgr_wwan_status_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(enable): error %d result %d",
                    qmi_error, wan_status_resp.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( wan_status_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get IPV4 WAN status  %d : %d",
        qmi_error, wan_status_resp.resp.error,0);
    *qmi_err_num = wan_status_resp.resp.error;
    return false;
  }

  if(wan_status_resp.conn_status_valid == 1)
  {
    *v4_status=wan_status_resp.conn_status;
    if(wan_status_resp.conn_status == QCMAP_MSGR_WWAN_STATUS_CONNECTING_V01)
    {
      LOG_MSG_INFO1(" IPV4 WWAN is Connecting \n",0,0,0);
    }
    else if(wan_status_resp.conn_status == QCMAP_MSGR_WWAN_STATUS_CONNECTED_V01)
    {
      LOG_MSG_INFO1(" IPV4 WWAN is connected \n",0,0,0);
    }
    else if(wan_status_resp.conn_status == QCMAP_MSGR_WWAN_STATUS_DISCONNECTING_V01)
    {
      LOG_MSG_INFO1(" IPV4 WWAN is Disconnecting \n",0,0,0);
    }
    else if(wan_status_resp.conn_status == QCMAP_MSGR_WWAN_STATUS_DISCONNECTED_V01)
    {
      LOG_MSG_INFO1(" IPV4 WWAN is Disconnected \n",0,0,0);
    }
  }

  memset(&wan_status_resp, 0, sizeof(qcmap_msgr_wwan_status_resp_msg_v01));
  wan_status_req.mobile_ap_handle = this->mobile_ap_handle;
  wan_status_req.call_type_valid = 1;
  wan_status_req.call_type = QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_WWAN_STATUS_REQ_V01,
                                       &wan_status_req,
                                       sizeof(qcmap_msgr_wwan_status_req_msg_v01),
                                       (void*)&wan_status_resp,
                                       sizeof(qcmap_msgr_wwan_status_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(enable): error %d result %d",
      qmi_error, wan_status_resp.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( wan_status_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get IPV6 WAN status %d : %d",
        qmi_error, wan_status_resp.resp.error,0);
    *qmi_err_num = wan_status_resp.resp.error;
    return false;
  }

  if(wan_status_resp.conn_status_valid == 1)
  {
    *v6_status=wan_status_resp.conn_status;
    if(wan_status_resp.conn_status == QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTING_V01)
    {
      LOG_MSG_INFO1(" IPV6 WWAN is Connecting \n",0,0,0);
    }
    else if(wan_status_resp.conn_status == QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTED_V01)
    {
      LOG_MSG_INFO1(" IPV6 WWAN is connected \n",0,0,0);
    }
    else if(wan_status_resp.conn_status == QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTING_V01)
    {
      LOG_MSG_INFO1(" IPV6 WWAN is Disconnecting \n",0,0,0);
    }
    else if(wan_status_resp.conn_status == QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTED_V01)
    {
      LOG_MSG_INFO1(" IPV6 WWAN is Disconnected \n",0,0,0);
    }
  }

  return true;
}

/*===========================================================================
  FUNCTION Get Station Mode status
  ===========================================================================*/
/*!
  @brief
  Gets Station Mode status

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::GetStationModeStatus(qcmap_msgr_station_mode_status_enum_v01 *status, qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_get_station_mode_status_resp_msg_v01 station_mode_status_resp;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&station_mode_status_resp, 0, sizeof(qcmap_msgr_get_station_mode_status_resp_msg_v01));
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_STATION_MODE_STATUS_REQ_V01,
                                       NULL,
                                       0,
                                       (void*)&station_mode_status_resp,
                                       sizeof(qcmap_msgr_get_station_mode_status_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(GetStationModeStatus): error %d result %d",
      qmi_error, station_mode_status_resp.resp.result, 0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( station_mode_status_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get station mode status %d : %d",
        qmi_error, station_mode_status_resp.resp.error, 0);
    *qmi_err_num = station_mode_status_resp.resp.error;
    return false;
  }

  if ( station_mode_status_resp.conn_status_valid == 1 )
  {
    *status = station_mode_status_resp.conn_status;
    if ( *status == QCMAP_MSGR_STATION_MODE_CONNECTED_V01 )
    {
      LOG_MSG_INFO1(" Mobile AP Station Mode is Connected \n", 0, 0, 0);
    }
    else if ( *status == QCMAP_MSGR_STATION_MODE_DISCONNECTED_V01 )
    {
      LOG_MSG_INFO1(" Mobile AP Station Mode is Disconnected \n", 0, 0, 0);
    }
    else
    {
      LOG_MSG_ERROR(" Invalid station mode status returned: 0x%x \n", *status, 0, 0);
    }
    return true;
  }
  return false;
}

/*===========================================================================
  FUNCTION SetRoaming
  ===========================================================================*/
/*!
  @brief
  Enables the Roaming feature

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::SetRoaming(boolean enable, qmi_error_type_v01 *qmi_err_num)
{
  qcmap_msgr_set_roaming_pref_req_msg_v01 set_roaming_req_msg;
  qcmap_msgr_set_roaming_pref_resp_msg_v01 set_roaming_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  set_roaming_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  set_roaming_req_msg.allow_wwan_calls_while_roaming = enable;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_ROAMING_PREF_REQ_V01,
                                       &set_roaming_req_msg,
                                       sizeof(qcmap_msgr_set_roaming_pref_req_msg_v01),
                                       &set_roaming_resp_msg,
                                       sizeof(qcmap_msgr_set_roaming_pref_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);
  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( set_roaming_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not set auto connect flag %d : %d",
        qmi_error, set_roaming_resp_msg.resp.error,0);
    *qmi_err_num = set_roaming_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("Roaming is Set succesfully...",0,0,0);
  return true;
}

/*===========================================================================
  FUNCTION GetRoaming
  ===========================================================================*/
/*!
  @brief
  Enables the Roaming feature

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::GetRoaming(boolean *enable, qmi_error_type_v01 *qmi_err_num)
{
  qcmap_msgr_get_roaming_pref_req_msg_v01 get_roaming_req_msg;
  qcmap_msgr_get_roaming_pref_resp_msg_v01 get_roaming_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  get_roaming_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_ROAMING_PREF_REQ_V01,
                                       &get_roaming_req_msg,
                                       sizeof(qcmap_msgr_get_roaming_pref_req_msg_v01),
                                       &get_roaming_resp_msg,
                                       sizeof(qcmap_msgr_get_roaming_pref_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);
  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_roaming_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not set auto connect flag %d : %d",
        qmi_error, get_roaming_resp_msg.resp.error,0);
    *qmi_err_num = get_roaming_resp_msg.resp.error;
    return false;
  }

  if(get_roaming_resp_msg.allow_wwan_calls_while_roaming_valid)
  {
    *enable = get_roaming_resp_msg.allow_wwan_calls_while_roaming;
  }
  return true;
}

/*===========================================================================
  FUNCTION SetNatTimeout
  ===========================================================================*/
/*!
  @brief
  Sets the timeout for the corresponding timeout type.

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::SetNatTimeout
(
  qcmap_msgr_nat_timeout_enum_v01 timeout_type,
  uint32 timeout_value,
  qmi_error_type_v01 *qmi_err_num
)
{

  qcmap_msgr_set_nat_timeout_req_msg_v01 set_nat_timeout_req_msg;
  qcmap_msgr_set_nat_timeout_resp_msg_v01 set_nat_timeout_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  set_nat_timeout_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  set_nat_timeout_req_msg.nat_timeout_type = timeout_type;
  set_nat_timeout_req_msg.timeout_value = timeout_value;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_NAT_TIMEOUT_REQ_V01,
                                       &set_nat_timeout_req_msg,
                                       sizeof(qcmap_msgr_set_nat_timeout_req_msg_v01),
                                       &set_nat_timeout_resp_msg,
                                       sizeof(qcmap_msgr_set_nat_timeout_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( set_nat_timeout_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not set nat_type %d : %d",
        qmi_error, set_nat_timeout_resp_msg.resp.error,0);
    *qmi_err_num = set_nat_timeout_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("NAT Timeout Set succeeded...", 0, 0, 0);
  return true;

}

/*===========================================================================
  FUNCTION GetNatTimeout
  ===========================================================================*/
/*!
  @brief
  Gets the timeout for the corresponding timeout type.

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::GetNatTimeout
(
  qcmap_msgr_nat_timeout_enum_v01 timeout_type,
  uint32 *timeout_value,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_get_nat_timeout_req_msg_v01 get_nat_timeout_req_msg;
  qcmap_msgr_get_nat_timeout_resp_msg_v01 get_nat_timeout_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  get_nat_timeout_req_msg.nat_timeout_type = timeout_type;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_NAT_TIMEOUT_REQ_V01,
                                       &get_nat_timeout_req_msg,
                                       sizeof(qcmap_msgr_get_nat_timeout_req_msg_v01),
                                       &get_nat_timeout_resp_msg,
                                       sizeof(qcmap_msgr_get_nat_timeout_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_nat_timeout_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get nat timeout %d : %d",
                   qmi_error, get_nat_timeout_resp_msg.resp.error, 0);
    *qmi_err_num = get_nat_timeout_resp_msg.resp.error;
    return false;
  }

  if (get_nat_timeout_resp_msg.timeout_value_valid)
  {
    *timeout_value = get_nat_timeout_resp_msg.timeout_value;
    LOG_MSG_INFO1("NAT Timeout for type:%d Get succeeded...%d", timeout_type, *timeout_value, 0);
  }

  return true;
}


/*===========================================================================
  FUNCTION SetFirewall
  ===========================================================================*/
/*!
  @brief
  Sets the Firewall Config with all params

  @return
  true  - on Success
  false - on Failure

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::SetFirewall(
  boolean             enable_firewall,
  boolean             pkts_allowed,
  qmi_error_type_v01  *qmi_err_num
)
{
 qcmap_msgr_set_firewall_config_req_msg_v01 set_firewall_req_msg;
 qcmap_msgr_set_firewall_config_resp_msg_v01 set_firewall_resp_msg;
 qmi_client_error_type qmi_error;

 memset(&set_firewall_req_msg, 0, sizeof(qcmap_msgr_set_firewall_config_req_msg_v01));
 memset(&set_firewall_resp_msg, 0, sizeof(qcmap_msgr_set_firewall_config_resp_msg_v01));

 set_firewall_req_msg.mobile_ap_handle = this->mobile_ap_handle;
 set_firewall_req_msg.firewall_enabled = enable_firewall;

 if(enable_firewall)
 {
   set_firewall_req_msg.pkts_allowed_valid = true;
   set_firewall_req_msg.pkts_allowed = pkts_allowed;
 }
 else
 {
  /*By default when Firewall is disabled, upnp_pinholde will be disabled*/
   set_firewall_req_msg.upnp_pinhole_flag_valid = true;
   set_firewall_req_msg.upnp_pinhole_flag = false;
 }

 LOG_MSG_INFO1("\n firewall_enabled =%d pkts_allowed =%d ",enable_firewall,pkts_allowed,0);
 qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_FIREWALL_CONFIG_REQ_V01,
                                       &set_firewall_req_msg,
                                       sizeof(qcmap_msgr_set_firewall_config_req_msg_v01),
                                       &set_firewall_resp_msg,
                                       sizeof(qcmap_msgr_set_firewall_config_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);
  if (( qmi_error == QMI_TIMEOUT_ERR ) ||
      ( qmi_error != QMI_NO_ERR) ||
      ( set_firewall_resp_msg.resp.result != QMI_NO_ERR ))
  {
    LOG_MSG_ERROR("Can not set firewall config %d : %d",
        qmi_error, set_firewall_resp_msg.resp.error,0);
    *qmi_err_num = set_firewall_resp_msg.resp.error;
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION GetFirewall
  ===========================================================================*/
/*!
  @brief
  Gets the Firewall Config

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::GetFirewall
(
  boolean *enable_firewall,
  boolean *pkts_allowed,
  qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_LOG_FUNC_ENTRY();

  qcmap_msgr_get_firewall_config_req_msg_v01 get_firewall_req_msg;
  qcmap_msgr_get_firewall_config_resp_msg_v01 get_firewall_resp_msg;
  qmi_client_error_type qmi_error;

  memset(&get_firewall_req_msg, 0, sizeof(qcmap_msgr_get_firewall_config_req_msg_v01));
  memset(&get_firewall_resp_msg, 0, sizeof(qcmap_msgr_get_firewall_config_resp_msg_v01));

  get_firewall_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_FIREWALL_CONFIG_REQ_V01,
                                       &get_firewall_req_msg,
                                       sizeof(qcmap_msgr_get_firewall_config_req_msg_v01),
                                       &get_firewall_resp_msg,
                                       sizeof(qcmap_msgr_get_firewall_config_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);
  if (( qmi_error == QMI_TIMEOUT_ERR ) ||
      ( qmi_error != QMI_NO_ERR) ||
      ( get_firewall_resp_msg.resp.result != QMI_NO_ERR ))
  {
    LOG_MSG_ERROR("Can not get firewall config %d : %d",
        qmi_error, get_firewall_resp_msg.resp.error,0);
    *qmi_err_num = get_firewall_resp_msg.resp.error;
    return false;
  }
  /* Need to add check for optional value */
  if ( get_firewall_resp_msg.firewall_enabled_valid )
  {
    *enable_firewall = get_firewall_resp_msg.firewall_enabled;
  }
  if ( get_firewall_resp_msg.pkts_allowed_valid )
  {
    *pkts_allowed = get_firewall_resp_msg.pkts_allowed ;
  }
  return true;
}


/*===========================================================================
  FUNCTION GetIPv4State
  ===========================================================================*/
/*!
  @brief
  Gets the IPv4 state.

  @return
  true  - on Enable
  false - on Disbale

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::GetIPv4State (boolean *ipv4_state , qmi_error_type_v01 *qmi_err_num)
{
  qcmap_msgr_get_ipv4_state_resp_msg_v01 get_ipv4_state_resp_msg;
  qmi_client_error_type qmi_error;

  memset(&get_ipv4_state_resp_msg, 0, sizeof(qcmap_msgr_get_ipv4_state_resp_msg_v01));

  QCMAP_LOG_FUNC_ENTRY();

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_IPV4_STATE_REQ_V01,
                                       NULL,
                                       0,
                                       &get_ipv4_state_resp_msg,
                                       sizeof(qcmap_msgr_get_ipv4_state_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_ipv4_state_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get IPv4  state %d : %d",
    qmi_error, get_ipv4_state_resp_msg.resp.error,0);
    *qmi_err_num = get_ipv4_state_resp_msg.resp.error;
    return false;
  }

  if (get_ipv4_state_resp_msg.ipv4_state_valid)
  {
    *ipv4_state = get_ipv4_state_resp_msg.ipv4_state;
    LOG_MSG_INFO1("Get IPv4 State succeeded. State: %d", *ipv4_state, 0, 0);
  }
  return true;

}

/*===========================================================================
  FUNCTION GetIPv6State
  ===========================================================================*/
/*!
  @brief
  Gets the IPv6 state.

  @return
  true  - on Enable
  false - on Disbale

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::GetIPv6State (boolean *ipv6_state , qmi_error_type_v01 *qmi_err_num)
{
  qcmap_msgr_get_ipv6_state_resp_msg_v01 get_ipv6_state_resp_msg;
  qmi_client_error_type qmi_error;

  memset(&get_ipv6_state_resp_msg, 0, sizeof(qcmap_msgr_get_ipv6_state_resp_msg_v01));

  QCMAP_LOG_FUNC_ENTRY();

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_IPV6_STATE_REQ_V01,
                                       NULL,
                                       0,
                                       &get_ipv6_state_resp_msg,
                                       sizeof(qcmap_msgr_get_ipv6_state_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_ipv6_state_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get IPv6  state %d : %d",
    qmi_error, get_ipv6_state_resp_msg.resp.error,0);
    *qmi_err_num = get_ipv6_state_resp_msg.resp.error;
    return false;
  }

  if (get_ipv6_state_resp_msg.ipv6_state_valid)
  {
    *ipv6_state = get_ipv6_state_resp_msg.ipv6_state;
    LOG_MSG_INFO1("Get IPv6 State succeeded. State: %d", *ipv6_state, 0, 0);
  }
  return true;

}

/*===========================================================================
  FUNCTION GetWWANPolicy
  ===========================================================================*/
/*!
  @brief
  Gets the WWAN Policy.

  @return
  V4/V6 profile number for UMTS and 3GPP2 along with tech preference.
  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::GetWWANPolicy(qcmap_msgr_net_policy_info_v01 *WWAN_policy, qmi_error_type_v01 *qmi_err_num )
{
  qcmap_msgr_get_wwan_policy_req_msg_v01  get_wwan_policy_req_msg;
  qcmap_msgr_get_wwan_policy_resp_msg_v01 get_wwan_policy_resp_msg;
  qmi_client_error_type qmi_error;

  memset(&get_wwan_policy_req_msg, 0, sizeof(qcmap_msgr_get_wwan_policy_req_msg_v01));
  memset(&get_wwan_policy_resp_msg, 0, sizeof(qcmap_msgr_get_wwan_policy_resp_msg_v01));

  QCMAP_LOG_FUNC_ENTRY();

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_WWAN_POLICY_REQ_V01,
                                       &get_wwan_policy_req_msg,
                                       sizeof(qcmap_msgr_get_wwan_policy_req_msg_v01),
                                       &get_wwan_policy_resp_msg,
                                       sizeof(qcmap_msgr_get_wwan_policy_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR) ||
       ( get_wwan_policy_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get WWAN Config %d : %d",
                   qmi_error, get_wwan_policy_resp_msg.resp.error, 0);
    *qmi_err_num = get_wwan_policy_resp_msg.resp.error;
    return false;
  }
  if(get_wwan_policy_resp_msg.wwan_policy_valid)
  {
    *WWAN_policy = get_wwan_policy_resp_msg.wwan_policy;
    LOG_MSG_INFO1("Get WWAN POLICY Succeeded. WWAN policy", 0, 0, 0);
  }

  return true;
}

 /*===========================================================================
   FUNCTION GetWWANPolicyList
   ===========================================================================*/
 /*!
   @brief
     Gets all WWAN Policy for a specific profile handle.

    @return
     V4/V6 profile number for UMTS and 3GPP2 along with tech preference.

   @note

   - Dependencies
     - None

   - Side Effects
     - None
  */
 /*=========================================================================*/
 boolean QCMAP_Client::GetWWANPolicyList
 (
   qcmap_msgr_wwan_policy_list_resp_msg_v01  *WWAN_policy,
   qmi_error_type_v01                        *qmi_err_num
  )
 {
   qmi_client_error_type qmi_error;

   memset(WWAN_policy, 0, sizeof(qcmap_msgr_wwan_policy_list_resp_msg_v01));

   QCMAP_LOG_FUNC_ENTRY();

   qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                        QMI_QCMAP_MSGR_WWAN_POLICY_LIST_REQ_V01,
                                        NULL,
                                        0,
                                        WWAN_policy,
                                        sizeof(qcmap_msgr_wwan_policy_list_resp_msg_v01),
                                        QCMAP_MSGR_QMI_TIMEOUT_VALUE);

   if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
        ( qmi_error != QMI_NO_ERR) ||
        ( WWAN_policy->resp.result != QMI_NO_ERR ) )
   {
     LOG_MSG_ERROR("Can not get WWAN Config %d : %d",
                    qmi_error, WWAN_policy->resp.error, 0);
     *qmi_err_num = WWAN_policy->resp.error;
     return false;
   }

   return true;
 }

/*===========================================================================
  FUNCTION SetWWANPolicy
  ===========================================================================*/
/*!
  @brief
    Sets the WWAN profile.

  @return
    true  - on Success
    false - on Failure

  @note

  - Dependencies
    - None

  - Side Effects
    - None
 */
/*=========================================================================*/
boolean QCMAP_Client::SetWWANPolicy
(
  qcmap_msgr_net_policy_info_v01 WWAN_policy,
  qmi_error_type_v01            *qmi_err_num
)
{
  qcmap_msgr_set_wwan_policy_req_msg_v01   set_wwan_policy_req_msg;
  qcmap_msgr_set_wwan_policy_resp_msg_v01  set_wwan_policy_resp_msg;
  qmi_client_error_type                    qmi_error;
  QCMAP_LOG_FUNC_ENTRY();
  set_wwan_policy_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  set_wwan_policy_req_msg.wwan_policy= WWAN_policy;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_WWAN_POLICY_REQ_V01,
                                       &set_wwan_policy_req_msg,
                                       sizeof(qcmap_msgr_set_wwan_policy_req_msg_v01),
                                       &set_wwan_policy_resp_msg,
                                       sizeof(qcmap_msgr_set_wwan_policy_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( set_wwan_policy_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not set WWAN Config %d : %d",
                   qmi_error, set_wwan_policy_resp_msg.resp.error, 0);
    *qmi_err_num = set_wwan_policy_resp_msg.resp.error;
    return false;
  }

  return true;
 }

/*===========================================================================
 FUNCTION CreateWWANPolicy
 ===========================================================================*/
/*!
 @brief
   Create's a WWAN policy.

 @return
   true  - on Success
   false - on Failure
 @note

 - Dependencies
   QCMobileAP must be enabled.

 - Side Effects
   - None
*/
/*=========================================================================*/
boolean QCMAP_Client::CreateWWANPolicy
(
  qcmap_msgr_net_policy_info_v01   WWAN_policy,
  qmi_error_type_v01              *qmi_err_num
)
{
  qcmap_msgr_create_wwan_policy_req_msg_v01   create_wwan_policy_req_msg;
  qcmap_msgr_create_wwan_policy_resp_msg_v01  create_wwan_policy_resp_msg;
  qmi_client_error_type                       qmi_error;

  QCMAP_LOG_FUNC_ENTRY();
  memset(&create_wwan_policy_req_msg , 0, sizeof(qcmap_msgr_create_wwan_policy_req_msg_v01));
  memset(&create_wwan_policy_resp_msg , 0, sizeof(qcmap_msgr_create_wwan_policy_resp_msg_v01));
  create_wwan_policy_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  create_wwan_policy_req_msg.wwan_policy = WWAN_policy;
  //setting Wan policy ip family type as V4V6
  create_wwan_policy_req_msg.wwan_policy.ip_family = QCMAP_MSGR_IP_FAMILY_V4V6_V01;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_CREATE_WWAN_POLICY_REQ_V01,
                                       &create_wwan_policy_req_msg,
                                       sizeof(qcmap_msgr_create_wwan_policy_req_msg_v01),
                                       &create_wwan_policy_resp_msg,
                                       sizeof(qcmap_msgr_create_wwan_policy_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( create_wwan_policy_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Cannot create profile : %d",
                   create_wwan_policy_resp_msg.resp.error, 0, 0);
    if (qmi_err_num != NULL)
      *qmi_err_num = create_wwan_policy_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("Sync Create WWAN Policy, valid=%d, profile=%d", create_wwan_policy_resp_msg.profile_handle_valid, create_wwan_policy_resp_msg.profile_handle, 0);

  return true;
}


/*===========================================================================
 FUNCTION UpdateWWANPolicy
 ===========================================================================*/
/*!
 @brief
   Updates WWAN policy.

 @return
   true  - on Success
   false - on Failure
 @note

 - Dependencies
   QCMobileAP must be enabled.

 - Side Effects
   - None
*/
/*=========================================================================*/
boolean QCMAP_Client::UpdateWWANPolicy
(
  qcmap_msgr_update_profile_enum_v01 update_req,
  qcmap_msgr_net_policy_info_v01     WWAN_policy,
  qmi_error_type_v01                *qmi_err_num
)

{
  qcmap_msgr_update_wwan_policy_req_msg_v01   update_wwan_policy_req_msg;
  qcmap_msgr_update_wwan_policy_resp_msg_v01  update_wwan_policy_resp_msg;
  qmi_client_error_type                       qmi_error;

  QCMAP_LOG_FUNC_ENTRY();
  memset(&update_wwan_policy_resp_msg , 0, sizeof(qcmap_msgr_update_wwan_policy_resp_msg_v01));
  memset(&update_wwan_policy_req_msg , 0, sizeof(qcmap_msgr_update_wwan_policy_req_msg_v01));
  update_wwan_policy_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  update_wwan_policy_req_msg.update_req_valid = TRUE;
  update_wwan_policy_req_msg.update_req = update_req;
  update_wwan_policy_req_msg.wwan_policy_valid = TRUE;
  update_wwan_policy_req_msg.wwan_policy = WWAN_policy;

  LOG_MSG_INFO1("Update_req=%d", update_req, 0,0);
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_UPDATE_WWAN_POLICY_REQ_V01,
                                       &update_wwan_policy_req_msg,
                                       sizeof(qcmap_msgr_update_wwan_policy_req_msg_v01),
                                       &update_wwan_policy_resp_msg,
                                       sizeof(qcmap_msgr_update_wwan_policy_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( update_wwan_policy_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Cannot update profile %d : %d",
                   qmi_error, update_wwan_policy_resp_msg.resp.error, 0);
    *qmi_err_num = update_wwan_policy_resp_msg.resp.error;
    return false;
  }

  return true;
}

/*===========================================================================
 FUNCTION DeleteWWANPolicy
 ===========================================================================*/
/*!
 @brief
   Delete's WWAN policy (only secondary profiles can be deleted).

 @return
   true  - on Success
   false - on Failure
 @note

 - Dependencies
   QCMobileAP must be enabled.

 - Side Effects
   - None
*/
/*=========================================================================*/

boolean QCMAP_Client::DeleteWWANPolicy
(
  qmi_error_type_v01      *qmi_err_num
)
{
  qcmap_msgr_delete_wwan_policy_req_msg_v01   delete_wwan_policy_req_msg;
  qcmap_msgr_delete_wwan_policy_resp_msg_v01  delete_wwan_policy_resp_msg;
  qmi_client_error_type                       qmi_error;

  QCMAP_LOG_FUNC_ENTRY();
  delete_wwan_policy_resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  delete_wwan_policy_resp_msg.resp.error = QMI_ERR_NONE_V01;
  delete_wwan_policy_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_DELETE_WWAN_POLICY_REQ_V01,
                                       &delete_wwan_policy_req_msg,
                                       sizeof(qcmap_msgr_delete_wwan_policy_req_msg_v01),
                                       &delete_wwan_policy_resp_msg,
                                       sizeof(qcmap_msgr_delete_wwan_policy_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( delete_wwan_policy_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Cannot delete profile %d : %d",
                   qmi_error, delete_wwan_policy_resp_msg.resp.error, 0);
    *qmi_err_num = delete_wwan_policy_resp_msg.resp.error;
    return false;
  }

  return true;
}

/*===========================================================================
  FUNCTION EnableUPNP
  ===========================================================================*/
/*!
  @brief
  Starts the UPNP daemon

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::EnableUPNP(qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_enable_upnp_resp_msg_v01 enable_upnp_resp_msg_v01;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&enable_upnp_resp_msg_v01, 0, sizeof(qcmap_msgr_enable_upnp_resp_msg_v01));

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_ENABLE_UPNP_REQ_V01,
                                       NULL,
                                       0,
                                       (void*)&enable_upnp_resp_msg_v01,
                                       sizeof(qcmap_msgr_enable_upnp_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(enable): error %d result %d",
      qmi_error, enable_upnp_resp_msg_v01.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( enable_upnp_resp_msg_v01.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not enable upnp %d : %d",
        qmi_error, enable_upnp_resp_msg_v01.resp.error,0);
    *qmi_err_num = enable_upnp_resp_msg_v01.resp.error;
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION DisableUPNP
  ===========================================================================*/
/*!
  @brief
  Stops the UPNP daemon

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::DisableUPNP(qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_disable_upnp_resp_msg_v01 disable_upnp_resp_msg_v01;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&disable_upnp_resp_msg_v01, 0, sizeof(qcmap_msgr_disable_upnp_resp_msg_v01));

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_DISABLE_UPNP_REQ_V01,
                                       NULL,
                                       0,
                                       (void*)&disable_upnp_resp_msg_v01,
                                       sizeof(qcmap_msgr_disable_upnp_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(disable): error %d result %d",
      qmi_error, disable_upnp_resp_msg_v01.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( disable_upnp_resp_msg_v01.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not disable upnp %d : %d",
        qmi_error, disable_upnp_resp_msg_v01.resp.error,0);
    *qmi_err_num = disable_upnp_resp_msg_v01.resp.error;
    return false;
  }
  return true;
}


/*===========================================================================
  FUNCTION GetUPNPStatus
  ===========================================================================*/
/*!
  @brief
  Returns the status of UPNP

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::GetUPNPStatus(qcmap_msgr_upnp_mode_enum_v01 *upnp_status,
                                    qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_get_upnp_status_resp_msg_v01  status_resp;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&status_resp, 0, sizeof(qcmap_msgr_get_upnp_status_resp_msg_v01));
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_UPNP_STATUS_REQ_V01,
                                       NULL,
                                       0,
                                       (void*)&status_resp,
                                       sizeof(qcmap_msgr_get_upnp_status_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(enable): error %d result %d",
      qmi_error, status_resp.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( status_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get UPnP status %d : %d",
        qmi_error, status_resp.resp.error, 0);
    *qmi_err_num = status_resp.resp.error;
    return false;
  }

  *upnp_status = status_resp.upnp_mode;
  if(status_resp.upnp_mode == QCMAP_MSGR_UPNP_MODE_UP_V01)
  {
    LOG_MSG_INFO1("UPnP is enabled \n",0,0,0);
  }
  else if(status_resp.upnp_mode == QCMAP_MSGR_UPNP_MODE_DOWN_V01)
  {
    LOG_MSG_INFO1("UPnP is disabled \n",0,0,0);
  }

  return true;
}


/*===========================================================================
  FUNCTION EnableDLNA
  ===========================================================================*/
/*!
  @brief
  Starts the DLNA daemon

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::EnableDLNA(qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_enable_dlna_resp_msg_v01 enable_dlna_resp_msg_v01;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&enable_dlna_resp_msg_v01, 0, sizeof(qcmap_msgr_enable_dlna_resp_msg_v01));

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_ENABLE_DLNA_REQ_V01,
                                       NULL,
                                       0,
                                       (void*)&enable_dlna_resp_msg_v01,
                                       sizeof(qcmap_msgr_enable_dlna_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(enable): error %d result %d",
      qmi_error, enable_dlna_resp_msg_v01.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( enable_dlna_resp_msg_v01.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not enable dlna %d : %d",
        qmi_error, enable_dlna_resp_msg_v01.resp.error,0);
    *qmi_err_num = enable_dlna_resp_msg_v01.resp.error;
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION DisableDLNA
  ===========================================================================*/
/*!
  @brief
  Stops the DLNA daemon

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::DisableDLNA(qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_disable_dlna_resp_msg_v01 disable_dlna_resp_msg_v01;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&disable_dlna_resp_msg_v01, 0, sizeof(qcmap_msgr_disable_dlna_resp_msg_v01));

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_DISABLE_DLNA_REQ_V01,
                                       NULL,
                                       0,
                                       (void*)&disable_dlna_resp_msg_v01,
                                       sizeof(qcmap_msgr_disable_dlna_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(disable): error %d result %d",
      qmi_error, disable_dlna_resp_msg_v01.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( disable_dlna_resp_msg_v01.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not disable dlna %d : %d",
        qmi_error, disable_dlna_resp_msg_v01.resp.error,0);
    *qmi_err_num = disable_dlna_resp_msg_v01.resp.error;
    return false;
  }
  return true;
}


/*===========================================================================
  FUNCTION GetDLNAStatus
  ===========================================================================*/
/*!
  @brief
  Returns the status of DLNA

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::GetDLNAStatus(qcmap_msgr_dlna_mode_enum_v01 *dlna_status,
                                    qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_get_dlna_status_resp_msg_v01  status_resp;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&status_resp, 0, sizeof(qcmap_msgr_get_dlna_status_resp_msg_v01));
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_DLNA_STATUS_REQ_V01,
                                       NULL,
                                       0,
                                       (void*)&status_resp,
                                       sizeof(qcmap_msgr_get_dlna_status_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(enable): error %d result %d",
      qmi_error, status_resp.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( status_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get DLNA status %d : %d",
        qmi_error, status_resp.resp.error, 0);
    *qmi_err_num = status_resp.resp.error;
    return false;
  }

  *dlna_status = status_resp.dlna_mode;
  if(status_resp.dlna_mode == QCMAP_MSGR_DLNA_MODE_UP_V01)
  {
    LOG_MSG_INFO1("DLNA is enabled \n",0,0,0);
  }
  else if(status_resp.dlna_mode == QCMAP_MSGR_DLNA_MODE_DOWN_V01)
  {
    LOG_MSG_INFO1("DLNA is disabled \n",0,0,0);
  }

  return true;
}


/*===========================================================================
  FUNCTION SetDLNAMediaDir
  ===========================================================================*/
/*!
  @brief
  Changes the DLNA media directory

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::SetDLNAMediaDir(char media_dir[],
                                      qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_set_dlna_media_dir_resp_msg_v01  status_resp;
  qcmap_msgr_set_dlna_media_dir_req_msg_v01   status_req;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&status_resp, 0, sizeof(qcmap_msgr_set_dlna_media_dir_resp_msg_v01));
  memset(&status_req, 0, sizeof(qcmap_msgr_set_dlna_media_dir_req_msg_v01));

  strlcpy(status_req.media_dir, media_dir, sizeof(status_req.media_dir));
  status_req.media_dir_len = strlen(media_dir);

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_DLNA_MEDIA_DIR_REQ_V01,
                                       (void*)&status_req,
                                       sizeof(qcmap_msgr_set_dlna_media_dir_req_msg_v01),
                                       (void*)&status_resp,
                                       sizeof(qcmap_msgr_set_dlna_media_dir_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(enable): error %d result %d",
      qmi_error, status_resp.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( status_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not set DLNA media directory %d : %d",
        qmi_error, status_resp.resp.error, 0);
    *qmi_err_num = status_resp.resp.error;
    return false;
  }

  return true;
}


/*===========================================================================
  FUNCTION GetDLNAMediaDir
  ===========================================================================*/
/*!
  @brief
  Returns the DLNA media directory

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::GetDLNAMediaDir(char media_dir[],
                                      qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_get_dlna_media_dir_resp_msg_v01  status_resp;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&status_resp, 0, sizeof(qcmap_msgr_get_dlna_media_dir_resp_msg_v01));
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_DLNA_MEDIA_DIR_REQ_V01,
                                       NULL,
                                       0,
                                       (void*)&status_resp,
                                       sizeof(qcmap_msgr_get_dlna_media_dir_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(enable): error %d result %d",
      qmi_error, status_resp.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( status_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not fetch DLNA media directory %d : %d",
        qmi_error, status_resp.resp.error, 0);
    *qmi_err_num = status_resp.resp.error;
    return false;
  }
  strlcpy(media_dir, status_resp.media_dir, QCMAP_MSGR_MAX_DLNA_DIR_LEN_V01);

  return true;
}


/*===========================================================================
  FUNCTION Enable M-DNS
  ===========================================================================*/
/*!
  @brief
  Starts the M-DNS daemon

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::EnableMDNS(qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_enable_multicast_dns_resp_msg_v01 enable_mdns_resp_msg_v01;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&enable_mdns_resp_msg_v01, 0, sizeof(qcmap_msgr_enable_multicast_dns_resp_msg_v01));

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_ENABLE_MULTICAST_DNS_RESPONDER_REQ_V01,
                                       NULL,
                                       0,
                                       (void*)&enable_mdns_resp_msg_v01,
                                       sizeof(qcmap_msgr_enable_multicast_dns_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(enable): error %d result %d",
      qmi_error, enable_mdns_resp_msg_v01.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( enable_mdns_resp_msg_v01.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not enable mdns %d : %d",
        qmi_error, enable_mdns_resp_msg_v01.resp.error,0);
    *qmi_err_num = enable_mdns_resp_msg_v01.resp.error;
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION DisableMDNS
  ===========================================================================*/
/*!
  @brief
  Stops the M-DNS daemon

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::DisableMDNS(qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_disable_multicast_dns_resp_msg_v01 disable_mdns_resp_msg_v01;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&disable_mdns_resp_msg_v01, 0, sizeof(qcmap_msgr_disable_multicast_dns_resp_msg_v01));

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_DISABLE_MULTICAST_DNS_RESPONDER_REQ_V01,
                                       NULL,
                                       0,
                                       (void*)&disable_mdns_resp_msg_v01,
                                       sizeof(qcmap_msgr_disable_multicast_dns_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(enable): error %d result %d",
      qmi_error, disable_mdns_resp_msg_v01.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( disable_mdns_resp_msg_v01.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not disable wlan %d : %d",
        qmi_error, disable_mdns_resp_msg_v01.resp.error,0);
    *qmi_err_num = disable_mdns_resp_msg_v01.resp.error;
    return false;
  }
  return true;
}


/*===========================================================================
  FUNCTION GetMDNSStatus
  ===========================================================================*/
/*!
  @brief
  Returns the status of MDNS

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::GetMDNSStatus(qcmap_msgr_mdns_mode_enum_v01 *mdns_status,
                                    qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_get_multicast_dns_status_resp_msg_v01  status_resp;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&status_resp, 0, sizeof(qcmap_msgr_get_multicast_dns_status_resp_msg_v01));
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_MULTICAST_DNS_STATUS_REQ_V01,
                                       NULL,
                                       0,
                                       (void*)&status_resp,
                                       sizeof(qcmap_msgr_get_multicast_dns_status_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(enable): error %d result %d",
      qmi_error, status_resp.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( status_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get MDNS status %d : %d",
        qmi_error, status_resp.resp.error, 0);
    *qmi_err_num = status_resp.resp.error;
    return false;
  }

  *mdns_status = status_resp.mdns_mode;
  if(status_resp.mdns_mode == QCMAP_MSGR_MDNS_MODE_UP_V01)
  {
    LOG_MSG_INFO1("MDNS is enabled \n",0,0,0);
  }
  else if(status_resp.mdns_mode == QCMAP_MSGR_MDNS_MODE_DOWN_V01)
  {
    LOG_MSG_INFO1("MDNS is disabled \n",0,0,0);
  }

  return true;
}

/*===========================================================================
  FUNCTION SetQCMAPBootupCfg
  ===========================================================================*/
/*!
  @brief
  Set QCMAP bootup configuration for MobileAP and WLAN

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::SetQCMAPBootupCfg(qcmap_msgr_bootup_flag_v01 mobileap_enable, qcmap_msgr_bootup_flag_v01 wlan_enable, qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_set_qcmap_bootup_cfg_req_msg_v01    qcmap_bootup_cfg_req_msg;
  qcmap_msgr_set_qcmap_bootup_cfg_resp_msg_v01   qcmap_bootup_cfg_resp_msg;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&qcmap_bootup_cfg_req_msg, 0, sizeof(qcmap_msgr_set_qcmap_bootup_cfg_req_msg_v01));
  memset(&qcmap_bootup_cfg_resp_msg, 0, sizeof(qcmap_msgr_set_qcmap_bootup_cfg_resp_msg_v01));

  if ((mobileap_enable == QCMAP_MSGR_BOOTUP_FLAG_MIN_ENUM_VAL_V01) && (wlan_enable == QCMAP_MSGR_BOOTUP_FLAG_MIN_ENUM_VAL_V01))
  {
    LOG_MSG_INFO1(" No Change required in Bootup Parameters",0,0,0);
    return true;
  }

  if (mobileap_enable != QCMAP_MSGR_BOOTUP_FLAG_MIN_ENUM_VAL_V01) {
    qcmap_bootup_cfg_req_msg.mobileap_bootup_flag_valid = true;
    qcmap_bootup_cfg_req_msg.mobileap_bootup_flag  = mobileap_enable;
  }

  if (wlan_enable != QCMAP_MSGR_BOOTUP_FLAG_MIN_ENUM_VAL_V01) {
    qcmap_bootup_cfg_req_msg.wlan_bootup_flag_valid = true;
    qcmap_bootup_cfg_req_msg.wlan_bootup_flag  = wlan_enable;
  }

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_QCMAP_BOOTUP_CFG_REQ_V01,
                                       (void *)&qcmap_bootup_cfg_req_msg,
                                       sizeof(qcmap_msgr_set_qcmap_bootup_cfg_req_msg_v01),
                                       (void*)&qcmap_bootup_cfg_resp_msg,
                                       sizeof(qcmap_msgr_set_qcmap_bootup_cfg_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(enable): error %d result %d",
      qmi_error,qcmap_bootup_cfg_resp_msg.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( qcmap_bootup_cfg_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Cannot Set Bootup Configuration of QCMAP Components %d : %d",
        qmi_error, qcmap_bootup_cfg_resp_msg.resp.error,0);
    *qmi_err_num = qcmap_bootup_cfg_resp_msg.resp.error;
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION GetQCMAPBootupCfg
  ===========================================================================*/
/*!
  @brief
  Get QCMAP bootup configuration for MobileAP and WLAN

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::GetQCMAPBootupCfg(qcmap_msgr_bootup_flag_v01 *mobileap_enable, qcmap_msgr_bootup_flag_v01 *wlan_enable, qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_get_qcmap_bootup_cfg_resp_msg_v01   qcmap_bootup_cfg_resp_msg;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&qcmap_bootup_cfg_resp_msg, 0, sizeof(qcmap_msgr_get_qcmap_bootup_cfg_resp_msg_v01));

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_QCMAP_BOOTUP_CFG_REQ_V01,
                                       NULL,
                                       0,
                                       (void*)&qcmap_bootup_cfg_resp_msg,
                                       sizeof(qcmap_msgr_get_qcmap_bootup_cfg_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(enable): error %d result %d",
      qmi_error,qcmap_bootup_cfg_resp_msg.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( qcmap_bootup_cfg_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Cannot Set Bootup Configuration of QCMAP Components %d : %d",
        qmi_error, qcmap_bootup_cfg_resp_msg.resp.error,0);
    *qmi_err_num = qcmap_bootup_cfg_resp_msg.resp.error;
    return false;
  }

  *mobileap_enable = qcmap_bootup_cfg_resp_msg.mobileap_bootup_flag;
  *wlan_enable = qcmap_bootup_cfg_resp_msg.wlan_bootup_flag;

  return true;
}


/*===========================================================================
  FUNCTION GetDataRate
  ===========================================================================*/
/*!
  @brief
  Get current data bitrate

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::GetDataRate(qcmap_msgr_data_bitrate_v01 *data_rate, qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_get_data_bitrate_resp_msg_v01   qcmap_data_rate_resp_msg;
  qcmap_msgr_get_data_bitrate_req_msg_v01    qcmap_data_rate_req_msg;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&qcmap_data_rate_resp_msg, 0, sizeof(qcmap_msgr_get_data_bitrate_resp_msg_v01));

  qcmap_data_rate_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_DATA_BITRATE_REQ_V01,
                                       &qcmap_data_rate_req_msg,
                                       sizeof(qcmap_msgr_get_data_bitrate_req_msg_v01),
                                       (void*)&qcmap_data_rate_resp_msg,
                                       sizeof(qcmap_msgr_get_data_bitrate_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(enable): error %d result %d",
      qmi_error, qcmap_data_rate_resp_msg.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( qcmap_data_rate_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Cannot get data rates %d : %d",
        qmi_error, qcmap_data_rate_resp_msg.resp.error,0);
    *qmi_err_num = qcmap_data_rate_resp_msg.resp.error;
    return false;
  }

  if(qcmap_data_rate_resp_msg.data_rate_valid)
  {
    *data_rate = qcmap_data_rate_resp_msg.data_rate;
    LOG_MSG_INFO1("Get Data Bitrate Succeeded.", 0, 0, 0);
  }
  return true;
}


/*===========================================================================
  FUNCTION SetUPNPNotifyInterval
  ===========================================================================*/
/*!
  @brief
  Changes the UPnP notify interval

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::SetUPNPNotifyInterval(int notify_int,
                                      qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_set_upnp_notify_interval_resp_msg_v01  notify_interval_resp;
  qcmap_msgr_set_upnp_notify_interval_req_msg_v01   notify_interval_req;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&notify_interval_resp, 0, sizeof(qcmap_msgr_set_upnp_notify_interval_resp_msg_v01));
  memset(&notify_interval_req, 0, sizeof(qcmap_msgr_set_upnp_notify_interval_req_msg_v01));

  notify_interval_req.notify_interval_valid = true;
  notify_interval_req.notify_interval = notify_int;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_UPNP_NOTIFY_INTERVAL_REQ_V01,
                                       (void*)&notify_interval_req,
                                       sizeof(qcmap_msgr_set_upnp_notify_interval_req_msg_v01),
                                       (void*)&notify_interval_resp,
                                       sizeof(qcmap_msgr_set_upnp_notify_interval_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(enable): error %d result %d",
      qmi_error, notify_interval_resp.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( notify_interval_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not set UPnP notify interval %d : %d",
        qmi_error, notify_interval_resp.resp.error, 0);
    *qmi_err_num = notify_interval_resp.resp.error;
    return false;
  }

  return true;
}


/*===========================================================================
  FUNCTION GetUPnPNotifyInterval
  ===========================================================================*/
/*!
  @brief
  Returns the UPnP notify interval

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::GetUPNPNotifyInterval(int *notify_int,
                                      qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_get_upnp_notify_interval_resp_msg_v01  notify_interval_resp;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&notify_interval_resp, 0, sizeof(qcmap_msgr_get_upnp_notify_interval_resp_msg_v01));
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_UPNP_NOTIFY_INTERVAL_REQ_V01,
                                       NULL,
                                       0,
                                       (void*)&notify_interval_resp,
                                       sizeof(qcmap_msgr_get_upnp_notify_interval_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(enable): error %d result %d",
      qmi_error, notify_interval_resp.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( notify_interval_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not fetch UPnP notify interval %d : %d",
        qmi_error, notify_interval_resp.resp.error, 0);
    *qmi_err_num = notify_interval_resp.resp.error;
    return false;
  }
  if (!notify_interval_resp.notify_interval_valid)
  {
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  *notify_int = notify_interval_resp.notify_interval;
  return true;
}


/*===========================================================================
  FUNCTION SetDLNANotifyInterval
  ===========================================================================*/
/*!
  @brief
  Changes the DLNA notify interval

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::SetDLNANotifyInterval(int notify_int,
                                      qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_set_dlna_notify_interval_resp_msg_v01  notify_interval_resp;
  qcmap_msgr_set_dlna_notify_interval_req_msg_v01   notify_interval_req;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&notify_interval_resp, 0, sizeof(qcmap_msgr_set_dlna_notify_interval_resp_msg_v01));
  memset(&notify_interval_req, 0, sizeof(qcmap_msgr_set_dlna_notify_interval_req_msg_v01));

  notify_interval_req.notify_interval_valid = true;
  notify_interval_req.notify_interval = notify_int;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_DLNA_NOTIFY_INTERVAL_REQ_V01,
                                       (void*)&notify_interval_req,
                                       sizeof(qcmap_msgr_set_dlna_notify_interval_req_msg_v01),
                                       (void*)&notify_interval_resp,
                                       sizeof(qcmap_msgr_set_dlna_notify_interval_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(enable): error %d result %d",
      qmi_error, notify_interval_resp.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( notify_interval_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not set DLNA notify interval %d : %d",
        qmi_error, notify_interval_resp.resp.error, 0);
    *qmi_err_num = notify_interval_resp.resp.error;
    return false;
  }

  return true;
}


/*===========================================================================
  FUNCTION GetDLNANotifyInterval
  ===========================================================================*/
/*!
  @brief
  Returns the DLNA notify interval

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::GetDLNANotifyInterval(int *notify_int,
                                      qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_get_dlna_notify_interval_resp_msg_v01  notify_interval_resp;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&notify_interval_resp, 0, sizeof(qcmap_msgr_get_dlna_notify_interval_resp_msg_v01));
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_DLNA_NOTIFY_INTERVAL_REQ_V01,
                                       NULL,
                                       0,
                                       (void*)&notify_interval_resp,
                                       sizeof(qcmap_msgr_get_dlna_notify_interval_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(enable): error %d result %d",
      qmi_error, notify_interval_resp.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( notify_interval_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not fetch DLNA notify interval %d : %d",
        qmi_error, notify_interval_resp.resp.error, 0);
    *qmi_err_num = notify_interval_resp.resp.error;
    return false;
  }
  if (!notify_interval_resp.notify_interval_valid){
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  *notify_int = notify_interval_resp.notify_interval;
  return true;
}
/*===========================================================================
  FUNCTION AddDHCPReservRecord
  ===========================================================================*/
/*!
  @brief
  Add a DHCP Reservation Record

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::AddDHCPReservRecord
(qcmap_msgr_dhcp_reservation_v01 *dhcp_reserv_record,
 qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_add_dhcp_reservation_record_req_msg_v01 add_dhcp_reserv_record_req_msg;
  qcmap_msgr_add_dhcp_reservation_record_resp_msg_v01 add_dhcp_reserv_record_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  if ( !dhcp_reserv_record )
  {
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    LOG_MSG_ERROR("Add DHCP Recrod Failed: %d ",*qmi_err_num ,0,0);
    return false;
  }

  memset(&add_dhcp_reserv_record_req_msg,0,\
         sizeof(qcmap_msgr_add_dhcp_reservation_record_req_msg_v01));
  add_dhcp_reserv_record_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  memcpy(&add_dhcp_reserv_record_req_msg.dhcp_reservation_record, dhcp_reserv_record,\
         sizeof(qcmap_msgr_dhcp_reservation_v01));
 qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_ADD_DHCP_RESERVATION_RECORD_REQ_V01,
                                       &add_dhcp_reserv_record_req_msg,
                                       sizeof(qcmap_msgr_add_dhcp_reservation_record_req_msg_v01),
                                       &add_dhcp_reserv_record_resp_msg,
                                       sizeof(qcmap_msgr_add_dhcp_reservation_record_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( add_dhcp_reserv_record_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not add dhcp reservation record %d : %d",
        qmi_error, add_dhcp_reserv_record_resp_msg.resp.error,0);
    *qmi_err_num = add_dhcp_reserv_record_resp_msg.resp.error;
    return false;
  }
  LOG_MSG_INFO1("Added DHCP Reservation Record...",0,0,0);
  return true;
}

/*===========================================================================
  FUNCTION GetDHCPReservRecords
  ===========================================================================*/
/*!
  @brief
  Display DHCP Reservation Records

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::GetDHCPReservRecords
(qcmap_msgr_dhcp_reservation_v01 *dhcp_reserv_records,
 uint32_t* num_entries,
 qmi_error_type_v01 *qmi_err_num
 )
{
  qcmap_msgr_get_dhcp_reservation_records_req_msg_v01 get_dhcp_reserv_records_req_msg;
  qcmap_msgr_get_dhcp_reservation_records_resp_msg_v01 get_dhcp_reserv_records_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  /*Check for valid pointers*/
  if ( dhcp_reserv_records == NULL|| num_entries == NULL)
  {
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    LOG_MSG_ERROR("Get DHCP Records Failed: %d ",*qmi_err_num ,0,0);
    return false;
  }
  memset(&get_dhcp_reserv_records_req_msg,0,\
         sizeof(qcmap_msgr_get_dhcp_reservation_records_req_msg_v01));
  get_dhcp_reserv_records_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  qmi_error = qmi_client_send_msg_sync(
                            this->qmi_qcmap_msgr_handle,
                            QMI_QCMAP_MSGR_GET_DHCP_RESERVATION_RECORDS_REQ_V01,
                            &get_dhcp_reserv_records_req_msg,
                            sizeof(qcmap_msgr_get_dhcp_reservation_records_req_msg_v01),
                            &get_dhcp_reserv_records_resp_msg,
                            sizeof(qcmap_msgr_get_dhcp_reservation_records_resp_msg_v01),
                            QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_dhcp_reserv_records_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get dhcp reservation records %d : %d",
        qmi_error, get_dhcp_reserv_records_resp_msg.resp.error,0);
    *qmi_err_num = get_dhcp_reserv_records_resp_msg.resp.error;
    return false;
  }
  if (get_dhcp_reserv_records_resp_msg.dhcp_reservation_records_valid == true)
  {
    *num_entries = get_dhcp_reserv_records_resp_msg.dhcp_reservation_records_len;
    LOG_MSG_INFO1("\nNum DHCP Reservation Records: %d",*num_entries,0,0);
    if (*num_entries <= QCMAP_MSGR_MAX_DHCP_RESERVATION_ENTRIES_V01)
    {
      memcpy(dhcp_reserv_records, get_dhcp_reserv_records_resp_msg.dhcp_reservation_records,\
             *num_entries * sizeof(qcmap_msgr_dhcp_reservation_v01));
    }
  }
  else
  {
    LOG_MSG_INFO1("\n Get DHCP Reservervation Records failed!!",0,0,0);
    return false;
  }
  LOG_MSG_INFO1("Exiting  GetDHCPReservRecords...",0,0,0);
  return true;
}

/*===========================================================================
  FUNCTION EditDHCPReservRecord
  ===========================================================================*/
/*!
  @brief
  Edit a DHCP record based on MAC or IP address as index

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::EditDHCPReservRecord
(
   uint32_t *addr,
   qcmap_msgr_dhcp_reservation_v01 *dhcp_reserv_record,
   qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_edit_dhcp_reservation_record_req_msg_v01 edit_dhcp_reserv_record_req_msg;
  qcmap_msgr_edit_dhcp_reservation_record_resp_msg_v01 edit_dhcp_reserv_record_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  /*Check for valid pointers*/
  if ( !dhcp_reserv_record || !addr)
  {
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    LOG_MSG_ERROR("Edit DHCP Records Failed: %d ",*qmi_err_num ,0,0);
    return false;
  }
  memset(&edit_dhcp_reserv_record_req_msg,0,\
         sizeof(qcmap_msgr_edit_dhcp_reservation_record_req_msg_v01));
  edit_dhcp_reserv_record_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  edit_dhcp_reserv_record_req_msg.client_reserved_ip = *addr;
  edit_dhcp_reserv_record_req_msg.client_reserved_ip_valid  = true;
  memcpy(&edit_dhcp_reserv_record_req_msg.dhcp_reservation_record,dhcp_reserv_record,\
          sizeof(qcmap_msgr_dhcp_reservation_v01));
  edit_dhcp_reserv_record_req_msg.dhcp_reservation_record_valid  = true;

  qmi_error = qmi_client_send_msg_sync(
                            this->qmi_qcmap_msgr_handle,
                            QMI_QCMAP_MSGR_EDIT_DHCP_RESERVATION_RECORD_REQ_V01,
                            &edit_dhcp_reserv_record_req_msg,
                            sizeof(qcmap_msgr_edit_dhcp_reservation_record_req_msg_v01),
                            &edit_dhcp_reserv_record_resp_msg,
                            sizeof(qcmap_msgr_edit_dhcp_reservation_record_resp_msg_v01),
                            QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( edit_dhcp_reserv_record_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not edit DHCP recrod %d : %d",
        qmi_error, edit_dhcp_reserv_record_resp_msg.resp.error,0);
    *qmi_err_num = edit_dhcp_reserv_record_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("Edited DHCP Record Successfully...",0,0,0);
  return true;
}

/*===========================================================================
  FUNCTION DeleteDHCPReservRecord
  ===========================================================================*/
/*!
  @brief
  delete a DHCP record based on MAC or IP address as index

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::DeleteDHCPReservRecord(uint32_t *addr, qmi_error_type_v01 *qmi_err_num)
{
  qcmap_msgr_delete_dhcp_reservation_record_req_msg_v01 delete_dhcp_reserv_record_req_msg;
  qcmap_msgr_delete_dhcp_reservation_record_resp_msg_v01 delete_dhcp_reserv_record_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  if ( !addr)
  {
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    LOG_MSG_ERROR("Delete DHCP Records Failed: %d ",*qmi_err_num ,0,0);
    return false;
  }
  memset(&delete_dhcp_reserv_record_req_msg,0,\
         sizeof(qcmap_msgr_delete_dhcp_reservation_record_req_msg_v01));
  delete_dhcp_reserv_record_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  delete_dhcp_reserv_record_req_msg.client_reserved_ip = *addr;
  delete_dhcp_reserv_record_req_msg.client_reserved_ip_valid = true;

  qmi_error = qmi_client_send_msg_sync(
                            this->qmi_qcmap_msgr_handle,
                            QMI_QCMAP_MSGR_DELETE_DHCP_RESERVATION_RECORD_REQ_V01,
                            &delete_dhcp_reserv_record_req_msg,
                            sizeof(qcmap_msgr_delete_dhcp_reservation_record_req_msg_v01),
                            &delete_dhcp_reserv_record_resp_msg,
                            sizeof(qcmap_msgr_delete_dhcp_reservation_record_resp_msg_v01),
                            QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( delete_dhcp_reserv_record_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not delete DHCP Reservation Record %d : %d",
        qmi_error, delete_dhcp_reserv_record_resp_msg.resp.error,0);
        *qmi_err_num = delete_dhcp_reserv_record_resp_msg.resp.error;
    return false;
  }
  LOG_MSG_INFO1("deleted DHCP Record...",0,0,0);
  return true;
}
/*===========================================================================
  FUNCTION SetWebserverWWANAccess
  ===========================================================================*/
/*!
  @brief
  Will set the webserver wwan access flag.

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::SetWebserverWWANAccess(boolean enable, qmi_error_type_v01 *qmi_err_num)
{

  qcmap_msgr_set_webserver_wwan_access_req_msg_v01 req_msg;
  qcmap_msgr_set_webserver_wwan_access_resp_msg_v01 resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  req_msg.mobile_ap_handle = this->mobile_ap_handle;
  req_msg.webserver_wwan_access = enable;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_WEBSERVER_WWAN_ACCESS_REQ_V01,
                                       &req_msg,
                                       sizeof(qcmap_msgr_set_webserver_wwan_access_req_msg_v01),
                                       &resp_msg,
                                       sizeof(qcmap_msgr_set_webserver_wwan_access_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not set webserver wwan access %d : %d",
        qmi_error, resp_msg.resp.error,0);
    *qmi_err_num = resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("Webserver WWAN Access Set succeeded...",0,0,0);
  return true;

}


/*===========================================================================
  FUNCTION GetWebserverWWANAccess
  ===========================================================================*/
/*!
  @brief
  Will get whether webserver is accessible from WWAN.

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::GetWebserverWWANAccess(boolean *enable, qmi_error_type_v01 *qmi_err_num)
{
  qcmap_msgr_get_webserver_wwan_access_req_msg_v01 req_msg;
  qcmap_msgr_get_webserver_wwan_access_resp_msg_v01 resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_WEBSERVER_WWAN_ACCESS_REQ_V01,
                                       &req_msg,
                                       sizeof(qcmap_msgr_get_webserver_wwan_access_req_msg_v01),
                                       &resp_msg,
                                       sizeof(qcmap_msgr_get_webserver_wwan_access_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get webserver wwan access %d : %d",
        qmi_error, resp_msg.resp.error,0);
    *qmi_err_num = resp_msg.resp.error;
    return false;
  }

  if (resp_msg.webserver_wwan_access_valid)
  {
    *enable = resp_msg.webserver_wwan_access;
    LOG_MSG_INFO1("Webserver WWAN Access Get succeeded...%d", *enable,0,0);
  }

  return true;
}
/*===========================================================================
  FUNCTION EnableAlg
  ===========================================================================*/
/*!
  @brief
  Enables ALGs Functionality.

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::EnableAlg
(
  qcmap_msgr_alg_type_mask_v01 alg_types,
  qmi_error_type_v01 *qmi_err_num
)
{
  int qcmap_msgr_errno;
  int ret = QCMAP_CM_SUCCESS;
  qcmap_msgr_enable_alg_req_msg_v01 qcmap_enable_alg_req_msg;
  qcmap_msgr_enable_alg_resp_msg_v01 qcmap_enable_alg_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();


  /* Enable ALGs. */
  LOG_MSG_INFO1("Enable ALGs mask %d", alg_types, 0, 0);
  qcmap_enable_alg_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  qcmap_enable_alg_req_msg.alg_type_op = alg_types;
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_ENABLE_ALG_REQ_V01,
                                       &qcmap_enable_alg_req_msg,
                                       sizeof(qcmap_msgr_enable_alg_req_msg_v01),
                                       &qcmap_enable_alg_resp_msg,
                                       sizeof(qcmap_msgr_enable_alg_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( qcmap_enable_alg_resp_msg.resp.result != QMI_NO_ERR ))
  {
    LOG_MSG_ERROR("Can not enable algs %d : %d",
        qmi_error, qcmap_enable_alg_resp_msg.resp.error, 0);
    *qmi_err_num = qcmap_enable_alg_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("Enabled ALGs...",0,0,0);
  return true;
}

/*===========================================================================
  FUNCTION DisableAlg
  ===========================================================================*/
/*!
  @brief
  Disables ALGs Functionality.

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::DisableAlg
(
  qcmap_msgr_alg_type_mask_v01 alg_types,
  qmi_error_type_v01 *qmi_err_num
)
{
  int qcmap_msgr_errno;
  int ret = QCMAP_CM_SUCCESS;
  qcmap_msgr_disable_alg_req_msg_v01 qcmap_disable_alg_req_msg;
  qcmap_msgr_disable_alg_resp_msg_v01 qcmap_disable_alg_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();


  /* Disable  ALGs. */
  LOG_MSG_INFO1("Disable ALGs mask: %d", alg_types, 0, 0);
  qcmap_disable_alg_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  qcmap_disable_alg_req_msg.alg_type_op = alg_types;
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_DISABLE_ALG_REQ_V01,
                                       &qcmap_disable_alg_req_msg,
                                       sizeof(qcmap_msgr_disable_alg_req_msg_v01),
                                       &qcmap_disable_alg_resp_msg,
                                       sizeof(qcmap_msgr_disable_alg_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( qcmap_disable_alg_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not disable ALGs %d : %d",
        qmi_error, qcmap_disable_alg_resp_msg.resp.error, 0);
    *qmi_err_num = qcmap_disable_alg_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("Disabled  ALGs...",0,0,0);
  return true;
}

/*=============================================================================
  FUNCTION SetSIPServerInfo
==============================================================================*/
/*!
  @brief
  - Populates the necessary fields in the QMI_QCMAP_MSGR_SET_SIP_SERVER_INFO_REQ
    message
  - Sends a QMI message to QCMAP server to set the SIP server information

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=============================================================================*/
boolean QCMAP_Client::SetSIPServerInfo
(
  qcmap_msgr_sip_server_info_v01 *sip_server_info,
  qmi_error_type_v01 *qmi_err_num
)
{
  int                   qcmap_msgr_errno;
  int                   ret = QCMAP_CM_SUCCESS;
  qcmap_msgr_set_sip_server_info_req_msg_v01
                        qcmap_set_sip_server_info_req_msg;
  qcmap_msgr_set_sip_server_info_resp_msg_v01
                        qcmap_set_sip_server_info_resp_msg;
  qmi_client_error_type qmi_error;
/*-----------------------------------------------------------------------------*/

  QCMAP_LOG_FUNC_ENTRY();

  if (sip_server_info == NULL ||
      qmi_err_num == NULL)
  {
    LOG_MSG_ERROR("Invalid parameters sent", 0, 0, 0);
    return false;
  }

  LOG_MSG_INFO1("SIP Server Info type %d", sip_server_info->pcscf_info_type, 0, 0);
  memset(&qcmap_set_sip_server_info_req_msg,
         0,
         sizeof(qcmap_msgr_set_sip_server_info_req_msg_v01));
  memset(&qcmap_set_sip_server_info_resp_msg,
         0,
         sizeof(qcmap_msgr_set_sip_server_info_resp_msg_v01));

  qcmap_set_sip_server_info_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  memcpy(&qcmap_set_sip_server_info_req_msg.default_sip_server_info,
         sip_server_info,
         sizeof(qcmap_msgr_sip_server_info_v01));

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_SIP_SERVER_INFO_REQ_V01,
                                       &qcmap_set_sip_server_info_req_msg,
                                       sizeof(qcmap_msgr_set_sip_server_info_req_msg_v01),
                                       &qcmap_set_sip_server_info_resp_msg,
                                       sizeof(qcmap_msgr_set_sip_server_info_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( qcmap_set_sip_server_info_resp_msg.resp.result != QMI_NO_ERR ))
  {
    LOG_MSG_ERROR("Can not set SIP server info %d : %d",
        qmi_error, qcmap_set_sip_server_info_resp_msg.resp.error, 0);
    *qmi_err_num = qcmap_set_sip_server_info_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("Completed setting SIP server info",0,0,0);
  return true;
}


/*=============================================================================
  FUNCTION GetSIPServerInfo
  ============================================================================*/
/*!
  @brief
  - Populates the necessary fields of QMI_QCMAP_MSGR_GET_SIP_SERVER_INFO_REQ
  - Sends a QMI message to QCMAP server to get the SIP server information

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=============================================================================*/
boolean QCMAP_Client::GetSIPServerInfo
(
   qcmap_msgr_sip_server_info_v01 *default_sip_info,
   qcmap_msgr_sip_server_info_v01 *network_sip_info,
   int *count_network_sip_info,
   qmi_error_type_v01 *qmi_err_num
)
{
  int                    qcmap_msgr_errno;
  int                    ret = QCMAP_CM_SUCCESS;
  qcmap_msgr_get_sip_server_info_req_msg_v01
                         qcmap_get_sip_server_info_req_msg;
  qcmap_msgr_get_sip_server_info_resp_msg_v01
                         qcmap_get_sip_server_info_resp_msg;
  qmi_client_error_type  qmi_error;
/*-----------------------------------------------------------------------------*/
  QCMAP_LOG_FUNC_ENTRY();

  if (default_sip_info == NULL || network_sip_info == NULL ||
      count_network_sip_info == NULL || qmi_err_num == NULL)
  {
    LOG_MSG_ERROR("Invalid parameter sent", 0, 0, 0);
    return false;
  }

  LOG_MSG_INFO1("Getting SIP Server Info", 0, 0, 0);
  memset(&qcmap_get_sip_server_info_req_msg,
         0,
         sizeof(qcmap_msgr_get_sip_server_info_req_msg_v01));
  memset(&qcmap_get_sip_server_info_resp_msg,
         0,
         sizeof(qcmap_msgr_get_sip_server_info_resp_msg_v01));

  qcmap_get_sip_server_info_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_SIP_SERVER_INFO_REQ_V01,
                                       &qcmap_get_sip_server_info_req_msg,
                                       sizeof(qcmap_msgr_get_sip_server_info_req_msg_v01),
                                       &qcmap_get_sip_server_info_resp_msg,
                                       sizeof(qcmap_msgr_get_sip_server_info_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( qcmap_get_sip_server_info_resp_msg.resp.result != QMI_NO_ERR ))
  {
    LOG_MSG_ERROR("Can not get SIP server info %d : %d",
        qmi_error, qcmap_get_sip_server_info_resp_msg.resp.error, 0);
    *qmi_err_num = qcmap_get_sip_server_info_resp_msg.resp.error;
    return false;
  }

  if (qcmap_get_sip_server_info_resp_msg.default_sip_server_info_valid)
  {
    memcpy(default_sip_info,
           &(qcmap_get_sip_server_info_resp_msg.default_sip_server_info),
           sizeof(qcmap_get_sip_server_info_resp_msg.default_sip_server_info));
  }

  if (qcmap_get_sip_server_info_resp_msg.network_sip_server_info_valid)
  {
    LOG_MSG_INFO1("There are %d network SIP servers",
                   qcmap_get_sip_server_info_resp_msg.network_sip_server_info_len,
                   0, 0);
    *count_network_sip_info = qcmap_get_sip_server_info_resp_msg.network_sip_server_info_len;
    if (qcmap_get_sip_server_info_resp_msg.network_sip_server_info_len > 0)
    {
        memcpy(network_sip_info,
             &qcmap_get_sip_server_info_resp_msg.network_sip_server_info,
             qcmap_get_sip_server_info_resp_msg.network_sip_server_info_len*sizeof(qcmap_msgr_sip_server_info_v01));
    }
  }

  LOG_MSG_INFO1("Completed obtaining SIP server info",0,0,0);
  return true;
}

/*=============================================================================
  FUNCTION GetV6SIPServerInfo
  ============================================================================*/
/*!
  @brief
  - Populates the necessary fields of QMI_QCMAP_MSGR_GET_IPV6_SIP_SERVER_INFO_REQ
  - Sends a QMI message to QCMAP server to get the V6SIP server information

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=============================================================================*/
boolean QCMAP_Client::GetV6SIPServerInfo
(
   qcmap_msgr_ipv6_sip_server_info_v01 *network_v6_sip_info,
   int *count_network_sip_info,
   qmi_error_type_v01 *qmi_err_num
)
{
  int                    qcmap_msgr_errno;
  int                    ret = QCMAP_CM_SUCCESS;
  qcmap_msgr_get_ipv6_sip_server_info_req_msg_v01
                         qcmap_get_ipv6_sip_server_info_req_msg;
  qcmap_msgr_get_ipv6_sip_server_info_resp_msg_v01
                         qcmap_get_ipv6_sip_server_info_resp_msg;
  qmi_client_error_type  qmi_error;
/*-----------------------------------------------------------------------------*/
  QCMAP_LOG_FUNC_ENTRY();

  if ( network_v6_sip_info == NULL ||
       count_network_sip_info == NULL || qmi_err_num == NULL)
  {
    LOG_MSG_ERROR("Invalid parameter sent", 0, 0, 0);
    return false;
  }

  LOG_MSG_INFO1("Getting IPV6 SIP Server Info", 0, 0, 0);
  memset(&qcmap_get_ipv6_sip_server_info_req_msg,
         0,
         sizeof(qcmap_msgr_get_ipv6_sip_server_info_req_msg_v01));
  memset(&qcmap_get_ipv6_sip_server_info_resp_msg,
         0,
         sizeof(qcmap_msgr_get_ipv6_sip_server_info_resp_msg_v01));

  qcmap_get_ipv6_sip_server_info_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_IPV6_SIP_SERVER_INFO_REQ_V01,
                                       &qcmap_get_ipv6_sip_server_info_req_msg,
                                       sizeof(qcmap_msgr_get_ipv6_sip_server_info_req_msg_v01),
                                       &qcmap_get_ipv6_sip_server_info_resp_msg,
                                       sizeof(qcmap_msgr_get_ipv6_sip_server_info_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( qcmap_get_ipv6_sip_server_info_resp_msg.resp.result != QMI_NO_ERR ))
  {
    LOG_MSG_ERROR("Can not get IPV6 SIP server info %d : %d",
    qmi_error, qcmap_get_ipv6_sip_server_info_resp_msg.resp.error, 0);
    *qmi_err_num = qcmap_get_ipv6_sip_server_info_resp_msg.resp.error;
    return false;
  }

  if (qcmap_get_ipv6_sip_server_info_resp_msg.network_ipv6_sip_server_info_valid)
  {
    LOG_MSG_INFO1("There are %d network SIP servers",
                   qcmap_get_ipv6_sip_server_info_resp_msg.\
                   network_ipv6_sip_server_info_len,
                   0, 0);
    *count_network_sip_info = qcmap_get_ipv6_sip_server_info_resp_msg.\
                              network_ipv6_sip_server_info_len;
    if (qcmap_get_ipv6_sip_server_info_resp_msg.network_ipv6_sip_server_info_len > 0)
    {
      memset(network_v6_sip_info,0,
             qcmap_get_ipv6_sip_server_info_resp_msg.network_ipv6_sip_server_info_len*
             sizeof(qcmap_msgr_ipv6_sip_server_info_v01));
      memcpy(network_v6_sip_info,
             &qcmap_get_ipv6_sip_server_info_resp_msg.network_ipv6_sip_server_info,
             qcmap_get_ipv6_sip_server_info_resp_msg.network_ipv6_sip_server_info_len*
             sizeof(qcmap_msgr_ipv6_sip_server_info_v01));
    }
  }

  LOG_MSG_INFO1("Completed obtaining IPV6 SIP server info", 0, 0, 0);
  return true;
}

/*===========================================================================
  FUNCTION RestoreFactoryConfig
  ===========================================================================*/
/*!
  @brief
  RestoreFactoryConfig will load the factory default configuration
  and reboot the device.

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::RestoreFactoryConfig(qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_restore_factory_config_req_msg_v01 qcmap_restore_factory_config_req_msg;
  qcmap_msgr_restore_factory_config_resp_msg_v01 qcmap_restore_factory_config_resp_msg;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&qcmap_restore_factory_config_req_msg,0,sizeof(qcmap_restore_factory_config_req_msg));
  memset(&qcmap_restore_factory_config_resp_msg,0,sizeof(qcmap_restore_factory_config_resp_msg));
  qcmap_restore_factory_config_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync( this->qmi_qcmap_msgr_handle,
                                        QMI_QCMAP_MSGR_RESTORE_FACTORY_CONFIG_REQ_V01,
                                        &qcmap_restore_factory_config_req_msg,
                                        sizeof(qcmap_restore_factory_config_req_msg),
                                        &qcmap_restore_factory_config_resp_msg,
                                        sizeof(qcmap_msgr_restore_factory_config_resp_msg_v01),
                                        QCMAP_MSGR_QMI_TIMEOUT_VALUE );

  LOG_MSG_INFO1( "qmi_client_send_msg_sync(RestoreFactoryConfig): error %d result %d",
                 qmi_error,qcmap_restore_factory_config_resp_msg.resp.result,0 );

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( qcmap_restore_factory_config_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR( "Cannot Reset to factory configuration %d : %d",
                   qmi_error, qcmap_restore_factory_config_resp_msg.resp.error,0 );
    *qmi_err_num = qcmap_restore_factory_config_resp_msg.resp.error;
    return false;
  }
  return true;
}

/*=============================================================================
  FUNCTION GetConnectedDevicesInfo
  ============================================================================*/
/*!
  @brief
  - This function fetches the information of the devices connected to SoftAP device

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=============================================================================*/
boolean QCMAP_Client::GetConnectedDevicesInfo
(
  qcmap_msgr_connected_device_info_v01 *conn_dev_info,
  int *num_entries,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_get_connected_devices_info_req_msg_v01
                         qcmap_get_connected_devices_info_req_msg;
  qcmap_msgr_get_connected_devices_info_resp_msg_v01
                         qcmap_get_connected_devices_info_resp_msg;
  qmi_client_error_type  qmi_error;
/*-----------------------------------------------------------------------------*/
  QCMAP_LOG_FUNC_ENTRY();

  qcmap_get_connected_devices_info_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync
              (this->qmi_qcmap_msgr_handle,
               QMI_QCMAP_MSGR_GET_CONNECTED_DEVICES_INFO_REQ_V01,
               &qcmap_get_connected_devices_info_req_msg,
               sizeof(qcmap_get_connected_devices_info_req_msg),
               &qcmap_get_connected_devices_info_resp_msg,
               sizeof(qcmap_get_connected_devices_info_resp_msg),
               QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( qcmap_get_connected_devices_info_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Cannot get Connected Devices info %d : %d",
                  qmi_error, qcmap_get_connected_devices_info_resp_msg.resp.error,
                  0);
    *qmi_err_num = qcmap_get_connected_devices_info_resp_msg.resp.error;
    return false;
  }

  if ((qcmap_get_connected_devices_info_resp_msg.connected_devices_info_valid) ==
      true)
  {
    *num_entries = qcmap_get_connected_devices_info_resp_msg.connected_devices_info_len;
    LOG_MSG_INFO1("\nNum of Connected Devices Entries: %d vlan_id:%d",*num_entries,
                  qcmap_get_connected_devices_info_resp_msg.connected_devices_info[0].vlan_id,
                  0);

    if (*num_entries <= QCMAP_MSGR_MAX_CONNECTED_DEVICES_V01)
    {
      memcpy(conn_dev_info,
             &qcmap_get_connected_devices_info_resp_msg.connected_devices_info,
             *num_entries * sizeof(qcmap_msgr_connected_device_info_v01));
    }
    else
    {
      LOG_MSG_INFO1("\nNum Connected Devices > QCMAP_MSGR_MAX_CONNECTED_DEVICES"
                    "Will be displaying max %d connected devices info",
                    QCMAP_MSGR_MAX_CONNECTED_DEVICES_V01,0,0);
      memcpy(conn_dev_info,
             &qcmap_get_connected_devices_info_resp_msg.connected_devices_info,
             (QCMAP_MSGR_MAX_CONNECTED_DEVICES_V01 *
             sizeof(qcmap_msgr_connected_device_info_v01)));
    }
  }
  else
  {
    LOG_MSG_INFO1("\nNo Connected Devices found",0,0,0);
    return false;
  }
  LOG_MSG_INFO1("Get Connected Devices Info Succeeded...",0,0,0);
  return true;
}

/*=============================================================================
  FUNCTION EnablePacketStats
  ============================================================================*/
/*!
  @brief
  - This function Enables Packet Stats Feature

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=============================================================================*/
boolean QCMAP_Client::EnablePacketStats(qmi_error_type_v01 *qmi_err_num)
{

  qcmap_msgr_packet_stats_status_ind_register_req_msg_v01 packet_stats_ind_req;
  qcmap_msgr_packet_stats_status_ind_register_resp_msg_v01 packet_stats_ind_rsp;
  qcmap_msgr_enable_packet_stats_resp_msg_v01 enable_packet_stats_resp_msg_v01;
  qcmap_msgr_enable_packet_stats_req_msg_v01 enable_packet_stats_req_msg_v01;
  qmi_client_error_type  qmi_error;
/*-----------------------------------------------------------------------------*/
  QCMAP_LOG_FUNC_ENTRY();

    /* Packet Stats is  enabled */


  memset(&enable_packet_stats_req_msg_v01, 0, sizeof(qcmap_msgr_enable_packet_stats_req_msg_v01));
  memset(&enable_packet_stats_resp_msg_v01, 0, sizeof(qcmap_msgr_enable_packet_stats_resp_msg_v01));
  enable_packet_stats_req_msg_v01.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_ENABLE_PACKET_STATS_REQ_V01,
                                       (void*)&enable_packet_stats_req_msg_v01,
                                       sizeof(qcmap_msgr_enable_packet_stats_req_msg_v01),
                                       (void*)&enable_packet_stats_resp_msg_v01,
                                       sizeof(qcmap_msgr_enable_packet_stats_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);
  LOG_MSG_INFO1("qmi_client_send_msg_sync: error %d result %d ",
                qmi_error, enable_packet_stats_resp_msg_v01.resp.result,0);
  if (( qmi_error == QMI_TIMEOUT_ERR ) ||
      ( qmi_error != QMI_NO_ERR ) ||
      ( enable_packet_stats_resp_msg_v01.resp.result != QMI_NO_ERR))
  {
    LOG_MSG_ERROR("Can not enable packet stats %d : %d",
        qmi_error, enable_packet_stats_resp_msg_v01.resp.error,0);
    *qmi_err_num = enable_packet_stats_resp_msg_v01.resp.error;
    return false;
  }




  memset(&packet_stats_ind_req, 0, sizeof(qcmap_msgr_packet_stats_status_ind_register_req_msg_v01));
  memset(&packet_stats_ind_rsp, 0, sizeof(qcmap_msgr_packet_stats_status_ind_register_resp_msg_v01));
  packet_stats_ind_req.register_indication = 1;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                         QMI_QCMAP_MSGR_PACK_STATS_STATUS_IND_REG_REQ_V01,
                                         (void*)&packet_stats_ind_req,
                                         sizeof(qcmap_msgr_packet_stats_status_ind_register_req_msg_v01),
                                         (void*)&packet_stats_ind_rsp,
                                         sizeof(qcmap_msgr_packet_stats_status_ind_register_resp_msg_v01),
                                         QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_ERROR("qmi_client_send_msg_sync: error %d result %d",
                 qmi_error, packet_stats_ind_rsp.resp.result,0);

  if (( qmi_error == QMI_TIMEOUT_ERR ) ||
      ( qmi_error != QMI_NO_ERR ) ||
      ( packet_stats_ind_rsp.resp.result != QMI_NO_ERR ))
  {
    LOG_MSG_ERROR("Can not register for packet stats status %d : %d",
                   qmi_error, packet_stats_ind_rsp.resp.error,0);
    *qmi_err_num = packet_stats_ind_rsp.resp.error;
    return false;
  }
  LOG_MSG_INFO1("Registered for packet stats status",0,0,0);

    return true;
}


/*=============================================================================
  FUNCTION DisablePacketStats
  ============================================================================*/
/*!
  @brief
  - This function Disables Packet Stats Feature

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=============================================================================*/
boolean QCMAP_Client::DisablePacketStats(qmi_error_type_v01 *qmi_err_num)
{

  qcmap_msgr_packet_stats_status_ind_register_req_msg_v01 packet_stats_ind_req;
  qcmap_msgr_packet_stats_status_ind_register_resp_msg_v01 packet_stats_ind_rsp;
  qcmap_msgr_disable_packet_stats_req_msg_v01 disable_packet_stats_req_msg_v01;
  qcmap_msgr_disable_packet_stats_resp_msg_v01 disable_packet_stats_resp_msg_v01;
  qmi_client_error_type qmi_error = QMI_NO_ERR;

  QCMAP_LOG_FUNC_ENTRY();

   // de register indications
  memset(&packet_stats_ind_req, 0, sizeof(qcmap_msgr_packet_stats_status_ind_register_req_msg_v01));
  memset(&packet_stats_ind_rsp, 0, sizeof(qcmap_msgr_packet_stats_status_ind_register_resp_msg_v01));
  packet_stats_ind_req.register_indication = 0;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                         QMI_QCMAP_MSGR_PACK_STATS_STATUS_IND_REG_REQ_V01,
                                         (void*)&packet_stats_ind_req,
                                         sizeof(qcmap_msgr_packet_stats_status_ind_register_req_msg_v01),
                                         (void*)&packet_stats_ind_rsp,
                                         sizeof(qcmap_msgr_packet_stats_status_ind_register_resp_msg_v01),
                                         QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_ERROR("qmi_client_send_msg_sync: error %d result %d",
                 qmi_error, packet_stats_ind_rsp.resp.result,0);

  if (( qmi_error == QMI_TIMEOUT_ERR ) ||
      ( qmi_error != QMI_NO_ERR ) ||
      ( packet_stats_ind_rsp.resp.result != QMI_NO_ERR ))
  {
    LOG_MSG_ERROR("Can not de-register for packet stats status %d : %d",
                   qmi_error, packet_stats_ind_rsp.resp.error,0);
    *qmi_err_num = packet_stats_ind_rsp.resp.error;
    return false;
  }
  LOG_MSG_INFO1("De-Registered for packet stats status",0,0,0);

  memset(&disable_packet_stats_req_msg_v01, 0, sizeof(qcmap_msgr_disable_packet_stats_req_msg_v01));
  memset(&disable_packet_stats_resp_msg_v01, 0, sizeof(qcmap_msgr_disable_packet_stats_resp_msg_v01));
  disable_packet_stats_req_msg_v01.mobile_ap_handle = this->mobile_ap_handle;
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_DISABLE_PACKET_STATS_REQ_V01,
                                       &disable_packet_stats_req_msg_v01,
                                       sizeof(disable_packet_stats_req_msg_v01),
                                       &disable_packet_stats_resp_msg_v01,
                                       sizeof(qcmap_msgr_disable_packet_stats_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( (disable_packet_stats_resp_msg_v01.resp.error != QMI_ERR_NO_EFFECT_V01 &&
          disable_packet_stats_resp_msg_v01.resp.error != QMI_ERR_NONE_V01)) ||
       ( disable_packet_stats_resp_msg_v01.resp.result != QMI_NO_ERR ))
  {
    LOG_MSG_ERROR( "Can not disable packet stats %d : %d",
        qmi_error, disable_packet_stats_resp_msg_v01.resp.error,0);
    *qmi_err_num = disable_packet_stats_resp_msg_v01.resp.error;
    return false;
  }
  return true;
}

/*=============================================================================
  FUNCTION ResetPacketStats
  ============================================================================*/
/*!
  @brief
  - This function resets statistics of all connected clients.

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=============================================================================*/
boolean QCMAP_Client::ResetPacketStats(qmi_error_type_v01 *qmi_err_num)
{
  qcmap_msgr_reset_packet_stats_req_msg_v01 reset_packet_stats_req_msg_v01;
  qcmap_msgr_reset_packet_stats_resp_msg_v01 reset_packet_stats_resp_msg_v01;
  qmi_client_error_type  qmi_error;

  memset(&reset_packet_stats_req_msg_v01, 0, sizeof(qcmap_msgr_reset_packet_stats_req_msg_v01));
  memset(&reset_packet_stats_resp_msg_v01, 0, sizeof(qcmap_msgr_reset_packet_stats_resp_msg_v01));
  reset_packet_stats_req_msg_v01.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_RESET_PACKET_STATS_REQ_V01,
                                       &reset_packet_stats_req_msg_v01,
                                       sizeof(qcmap_msgr_reset_packet_stats_req_msg_v01),
                                       &reset_packet_stats_resp_msg_v01,
                                       sizeof(qcmap_msgr_reset_packet_stats_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( (reset_packet_stats_resp_msg_v01.resp.error != QMI_ERR_NO_EFFECT_V01 &&
          reset_packet_stats_resp_msg_v01.resp.error != QMI_ERR_NONE_V01)) ||
       ( reset_packet_stats_resp_msg_v01.resp.result != QMI_NO_ERR ))
  {
    LOG_MSG_ERROR( "Can not reset packet stats %d : %d",
        qmi_error, reset_packet_stats_resp_msg_v01.resp.error,0);
    *qmi_err_num = reset_packet_stats_resp_msg_v01.resp.error;
    return false;
  }

  return true;
}

/*===========================================================================
FUNCTION GetPacketStatsStatus()
===========================================================================*/
/*! @ingroup  qcmap_msgr_packet_stats_status

  Obtain Packet Stats Status.

  This API is called  by the QCMAP client to get  packet
  statistics state.

  @datatypes
  qcmap_msgr_packet_stats_status_enum_v01\n
  qmi_error_type_v01

  @param[out] status           packet stats status.
  @param[out] qmi_err_num      Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  none

  @dependencies
  None.
*/
boolean QCMAP_Client::GetPacketStatsStatus(qcmap_msgr_packet_stats_status_enum_v01* status,
                             qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_packet_stats_status_req_msg_v01 status_req;
  qcmap_msgr_packet_stats_status_resp_msg_v01 status_resp;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&status_resp, 0, sizeof(qcmap_msgr_packet_stats_status_resp_msg_v01));
  status_req.mobile_ap_handle = this->mobile_ap_handle;
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_PACKET_STATS_STATUS_REQ_V01,
                                       &status_req,
                                       sizeof(qcmap_msgr_packet_stats_status_req_msg_v01),
                                       (void*)&status_resp,
                                       sizeof(qcmap_msgr_packet_stats_status_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(enable): error %d result %d",
      qmi_error, status_resp.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( status_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR(" QMI failure  %d : %d",
        qmi_error,status_resp.resp.error,0);
    *qmi_err_num = status_resp.resp.error;
    return false;
  }

  if (status_resp.status_valid == 1)
  {
    *status = status_resp.status;
    if (status_resp.status == QCMAP_MSGR_PACKET_STATS_STATUS_ENABLED_V01)
    {
      LOG_MSG_INFO1(" Packet stats is Enabled",0,0,0);
    }
    else if (status_resp.status == QCMAP_MSGR_PACKET_STATS_STATUS_DISABLED_V01)
    {
      LOG_MSG_INFO1(" Packet stats is Disabled",0,0,0);
   }
  }
  return true;
}

/*===========================================================================
  FUNCTION SetSupplicantConfig
  ===========================================================================*/
/*!
  @brief
  Activate/Deactivate the wpa_supplicant based on the status flag.

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::SetSupplicantConfig
(
  boolean status,
  qmi_error_type_v01 *qmi_err_num
)
{
  int qcmap_msgr_errno;
  int ret = QCMAP_CM_SUCCESS;
  qcmap_msgr_set_supplicant_config_req_msg_v01
     qcmap_set_supplicant_config_req_msg_v01;
  qcmap_msgr_set_supplicant_config_resp_msg_v01
     qcmap_set_supplicant_config_resp_msg_v01;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  if (qmi_err_num == NULL)
  {
    LOG_MSG_ERROR("Invalid parameter sent", 0, 0, 0);
    return false;
  }

  memset(&qcmap_set_supplicant_config_resp_msg_v01, 0,
         sizeof(qcmap_set_supplicant_config_resp_msg_v01));

  memset(&qcmap_set_supplicant_config_req_msg_v01, 0,
         sizeof(qcmap_set_supplicant_config_req_msg_v01));

  LOG_MSG_INFO1("Set Supplicant status: %d", status, 0, 0);
  qcmap_set_supplicant_config_req_msg_v01.mobile_ap_handle = this->mobile_ap_handle;
  qcmap_set_supplicant_config_req_msg_v01.supplicant_config_status = status;
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_SUPPLICANT_CONFIG_REQ_V01,
                                       &qcmap_set_supplicant_config_req_msg_v01,
                                       sizeof(qcmap_msgr_set_supplicant_config_req_msg_v01),
                                       &qcmap_set_supplicant_config_resp_msg_v01,
                                       sizeof(qcmap_msgr_set_supplicant_config_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( qcmap_set_supplicant_config_resp_msg_v01.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not set supplicant config %d : %d",
        qmi_error, qcmap_set_supplicant_config_resp_msg_v01.resp.error, 0);
    *qmi_err_num = qcmap_set_supplicant_config_resp_msg_v01.resp.error;
    return false;
  }

  LOG_MSG_INFO1("Supplicant config applied successfully.",0,0,0);
  return true;
}

/*===========================================================================
  FUNCTION Get Cradle Mode
  ===========================================================================*/
/*!
  @brief
  Gets Cradle Mode

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::GetCradleMode
(
   qcmap_msgr_cradle_mode_v01 *mode,
   qmi_error_type_v01 *qmi_err_num
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_get_cradle_mode_resp_msg_v01 cradle_mode_resp;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&cradle_mode_resp, 0, sizeof(qcmap_msgr_get_cradle_mode_resp_msg_v01));
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_CRADLE_MODE_REQ_V01,
                                       NULL,
                                       0,
                                       &cradle_mode_resp,
                                       sizeof(qcmap_msgr_get_cradle_mode_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(GetCradleModeStatus): error %d result %d",
      qmi_error, cradle_mode_resp.resp.result, 0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( cradle_mode_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get cradle mode status %d : %d",
        qmi_error, cradle_mode_resp.resp.error, 0);
    *qmi_err_num = cradle_mode_resp.resp.error;
    return false;
  }

  *mode = cradle_mode_resp.mode;

  return true;
}

/*===========================================================================
  FUNCTION Set Cradle Mode
  ===========================================================================*/
/*!
  @brief
  Sets Cradle Mode

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::SetCradleMode
(
   qcmap_msgr_cradle_mode_v01 mode,
   qmi_error_type_v01 *qmi_err_num
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_set_cradle_mode_req_msg_v01 cradle_mode_req;
  qcmap_msgr_set_cradle_mode_resp_msg_v01 cradle_mode_resp;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&cradle_mode_req, 0, sizeof(qcmap_msgr_set_cradle_mode_req_msg_v01));
  memset(&cradle_mode_resp, 0, sizeof(qcmap_msgr_set_cradle_mode_resp_msg_v01));

  cradle_mode_req.mode = mode;
  cradle_mode_req.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_CRADLE_MODE_REQ_V01,
                                       &cradle_mode_req,
                                       sizeof(qcmap_msgr_set_cradle_mode_req_msg_v01),
                                       &cradle_mode_resp,
                                       sizeof(qcmap_msgr_set_cradle_mode_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(SetCradleModeStatus): error %d result %d",
      qmi_error, cradle_mode_resp.resp.result, 0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( cradle_mode_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not set cradle mode status %d : %d",
        qmi_error, cradle_mode_resp.resp.error, 0);
    *qmi_err_num = cradle_mode_resp.resp.error;
    return false;
  }

  return true;
}

/*===========================================================================
  FUNCTION Get Prefix Delegation Config
  ===========================================================================*/
/*!
  @brief
  Gets Prefix Delegation Config

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::GetPrefixDelegationConfig
(
   boolean *pd_mode,
   qmi_error_type_v01 *qmi_err_num
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_get_prefix_delegation_config_resp_msg_v01 pd_config_resp;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&pd_config_resp, 0, sizeof(qcmap_msgr_get_prefix_delegation_config_resp_msg_v01));
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_PREFIX_DELEGATION_CONFIG_REQ_V01,
                                       NULL,
                                       0,
                                       &pd_config_resp,
                                       sizeof(qcmap_msgr_get_prefix_delegation_config_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(GetPrefixDelegationConfig): error %d result %d",
      qmi_error, pd_config_resp.resp.result, 0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( pd_config_resp.resp.result != QMI_NO_ERR ) ||
       !pd_config_resp.prefix_delegation_valid )
  {
    LOG_MSG_ERROR("Can not get Prefix Delegation config %d : %d",
        qmi_error, pd_config_resp.resp.error, 0);
    *qmi_err_num = pd_config_resp.resp.error;
    return false;
  }

  *pd_mode = pd_config_resp.prefix_delegation;
  return true;
}

/*===========================================================================
  FUNCTION Set Prefix Delegation Config
  ===========================================================================*/
/*!
  @brief
  Enable/Disable Prefix Delegation Config

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::SetPrefixDelegationConfig
(
   boolean pd_mode,
   qmi_error_type_v01 *qmi_err_num
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_set_prefix_delegation_config_req_msg_v01 pd_config_req;
  qcmap_msgr_set_prefix_delegation_config_resp_msg_v01 pd_config_resp;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&pd_config_req, 0, sizeof(qcmap_msgr_set_prefix_delegation_config_req_msg_v01));
  memset(&pd_config_resp, 0, sizeof(qcmap_msgr_set_prefix_delegation_config_resp_msg_v01));

  pd_config_req.prefix_delegation = pd_mode;
  pd_config_req.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_PREFIX_DELEGATION_CONFIG_REQ_V01,
                                       &pd_config_req,
                                       sizeof(qcmap_msgr_set_prefix_delegation_config_req_msg_v01),
                                       &pd_config_resp,
                                       sizeof(qcmap_msgr_set_prefix_delegation_config_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(SetPrefixDelegationConfig): error %d result %d",
      qmi_error, pd_config_resp.resp.result, 0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( pd_config_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not set Prefix Delegation config %d : %d",
        qmi_error, pd_config_resp.resp.error, 0);
    *qmi_err_num = pd_config_resp.resp.error;
    return false;
  }

  return true;
}

/*===========================================================================
  FUNCTION Get Prefix Delegation Status
  ===========================================================================*/
/*!
  @brief
  Gets the current Prefix Delegation mode

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::GetPrefixDelegationStatus
(
   boolean *pd_mode,
   qmi_error_type_v01 *qmi_err_num
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_get_prefix_delegation_status_resp_msg_v01 pd_status_resp;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&pd_status_resp, 0, sizeof(qcmap_msgr_get_prefix_delegation_config_resp_msg_v01));
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_PREFIX_DELEGATION_STATUS_REQ_V01,
                                       NULL,
                                       0,
                                       &pd_status_resp,
                                       sizeof(qcmap_msgr_get_prefix_delegation_status_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(GetPrefixDelegationMode): error %d result %d",
      qmi_error, pd_status_resp.resp.result, 0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( pd_status_resp.resp.result != QMI_NO_ERR ) ||
       !pd_status_resp.prefix_delegation_valid )
  {
    LOG_MSG_ERROR("Can not get Prefix Delegation mode status %d : %d",
        qmi_error, pd_status_resp.resp.error, 0);
    *qmi_err_num = pd_status_resp.resp.error;
    return false;
  }

  *pd_mode = pd_status_resp.prefix_delegation;
  return true;
}

/*===========================================================================
  FUNCTION Set Gateway URL
  ===========================================================================*/
/*!
  @brief
  Set gateway URL

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::SetGatewayUrl
(
   uint8_t *url,
   uint32_t url_len,
   qmi_error_type_v01 *qmi_err_num
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_set_gateway_url_req_msg_v01 set_url_req;
  qcmap_msgr_set_gateway_url_resp_msg_v01 set_url_resp;

  QCMAP_LOG_FUNC_ENTRY();

  if(!url || (url_len > QCMAP_MSGR_MAX_GATEWAY_URL_V01))
  {
    LOG_MSG_ERROR("Invalid parameter passed ",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  memset(&set_url_req, 0, sizeof(qcmap_msgr_set_gateway_url_req_msg_v01));
  memset(&set_url_resp, 0, sizeof(qcmap_msgr_set_gateway_url_resp_msg_v01));

  set_url_req.mobile_ap_handle = this->mobile_ap_handle;
  memcpy(set_url_req.gateway_url,url,url_len);
  set_url_req.gateway_url_len = url_len;
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_GATEWAY_URL_REQ_V01,
                                       &set_url_req,
                                       sizeof(qcmap_msgr_set_gateway_url_req_msg_v01),
                                       &set_url_resp,
                                       sizeof(qcmap_msgr_set_gateway_url_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(SetGatewayUrl): error %d result %d",
      qmi_error, set_url_resp.resp.result, 0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( set_url_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not set SetGatewayUrl %d : %d",
                   qmi_error, set_url_resp.resp.error, 0);
    *qmi_err_num = set_url_resp.resp.error;
    return false;
  }

  return true;
}

/*===========================================================================
  FUNCTION Get Gateway URL
  ===========================================================================*/
/*!
  @brief
  get gateway URL

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::GetGatewayUrl
(
   uint8_t *url,
   uint32_t *url_len,
   qmi_error_type_v01 *qmi_err_num
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_get_gateway_url_req_msg_v01 get_url_req;
  qcmap_msgr_get_gateway_url_resp_msg_v01 get_url_resp;

  QCMAP_LOG_FUNC_ENTRY();

  if(!url || !url_len)
  {
    LOG_MSG_ERROR("Invalid parameter passed ",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }
  memset(&get_url_resp, 0, sizeof(qcmap_msgr_get_gateway_url_resp_msg_v01));
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_GATEWAY_URL_REQ_V01,
                                       &get_url_req,
                                       sizeof(qcmap_msgr_get_gateway_url_req_msg_v01),
                                       &get_url_resp,
                                       sizeof(qcmap_msgr_get_gateway_url_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(GetGatewayUrl): error %d result %d",
      qmi_error, get_url_resp.resp.result, 0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_url_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get GetGatewayUrl %d : %d",
        qmi_error, get_url_resp.resp.error, 0);
    *qmi_err_num = get_url_resp.resp.error;
    return false;
  }
  memcpy(url,get_url_resp.gateway_url,get_url_resp.gateway_url_len);
  url[QCMAP_MSGR_MAX_GATEWAY_URL_V01 -1]='\0';
  *url_len = strlen(url);

  return true;
}

/*===========================================================================
  FUNCTION EnableDDNS
  ===========================================================================*/
/*!
  @brief
  Enables Dynamic DNS

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::EnableDDNS(qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_enable_dynamic_dns_req_msg_v01 enable_ddns_req_msg_v01;
  qcmap_msgr_enable_dynamic_dns_resp_msg_v01 enable_ddns_resp_msg_v01;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&enable_ddns_req_msg_v01, 0, sizeof(enable_ddns_req_msg_v01));
  memset(&enable_ddns_resp_msg_v01, 0, sizeof(enable_ddns_resp_msg_v01));

  enable_ddns_req_msg_v01.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_ENABLE_DYNAMIC_DNS_REQ_V01,
                                       &enable_ddns_req_msg_v01,
                                       sizeof(enable_ddns_req_msg_v01),
                                       (void*)&enable_ddns_resp_msg_v01,
                                       sizeof(enable_ddns_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(EnableDDNS): error %d result %d",
      qmi_error, enable_ddns_resp_msg_v01.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( enable_ddns_resp_msg_v01.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not Enable DDNS %d : %d",
        qmi_error, enable_ddns_resp_msg_v01.resp.error,0);
    *qmi_err_num = enable_ddns_resp_msg_v01.resp.error;
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION DisableDDNS
  ===========================================================================*/
/*!
  @brief
  Disables Dynamic Dns

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::DisableDDNS(qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_disable_dynamic_dns_req_msg_v01 disable_ddns_req_msg_v01;
  qcmap_msgr_disable_dynamic_dns_resp_msg_v01 disable_ddns_resp_msg_v01;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&disable_ddns_req_msg_v01, 0, sizeof(disable_ddns_req_msg_v01));
  memset(&disable_ddns_resp_msg_v01, 0, sizeof(disable_ddns_resp_msg_v01));

  disable_ddns_req_msg_v01.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_DISABLE_DYNAMIC_DNS_REQ_V01,
                                       &disable_ddns_req_msg_v01,
                                       sizeof(disable_ddns_req_msg_v01),
                                       (void*)&disable_ddns_resp_msg_v01,
                                       sizeof(disable_ddns_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(DisableDDNS): error %d result %d",
      qmi_error, disable_ddns_resp_msg_v01.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( disable_ddns_resp_msg_v01.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not enable Dynamic DNS %d : %d",
    qmi_error, disable_ddns_resp_msg_v01.resp.error,0);
    *qmi_err_num = disable_ddns_resp_msg_v01.resp.error;
    return false;
  }
  return true;
}


/*===========================================================================
  FUNCTION qmi_qcmap_msgr_set_dynamic_dns_config()

  DESCRIPTION
   Set ddns

  PARAMETERS

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
boolean QCMAP_Client::SetDDNSConfig( qcmap_msgr_set_dynamic_dns_config_req_msg_v01
                               *setddns_cfg_req, qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_set_dynamic_dns_config_resp_msg_v01 setddns_cfg_resp;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&setddns_cfg_resp, 0, sizeof(qcmap_msgr_set_dynamic_dns_config_resp_msg_v01));

  if(setddns_cfg_req == NULL)
  {
    LOG_MSG_ERROR("Invalid parameter passed ",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  setddns_cfg_req->mobile_ap_handle = this->mobile_ap_handle;

  if(setddns_cfg_req->timeout > 0)
  {
    setddns_cfg_req->timeout_valid = true;
  }
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_DYNAMIC_DNS_CONFIG_REQ_V01,
                                       setddns_cfg_req,
                                       sizeof(qcmap_msgr_set_dynamic_dns_config_req_msg_v01),
                                       &setddns_cfg_resp,
                                       sizeof(qcmap_msgr_set_dynamic_dns_config_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(SetDDNSConfig): error %d result %d",
                qmi_error, setddns_cfg_resp.resp.result, 0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( setddns_cfg_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not set DDNS config %d : %d",
                   qmi_error, setddns_cfg_resp.resp.error, 0);
    *qmi_err_num = setddns_cfg_resp.resp.error;
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION GetDDNS
  ===========================================================================*/
/*!
  @brief
  Will get whether webserver is accessible from WWAN.

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::GetDDNSConfig(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01 *ddns_server, qmi_error_type_v01 *qmi_err_num)
{
  qcmap_msgr_get_dynamic_dns_config_req_msg_v01 get_ddns_req_msg;
  qcmap_msgr_get_dynamic_dns_config_resp_msg_v01 get_ddns_resp_msg;
  qmi_client_error_type qmi_error;
  int num_entries,i;
  QCMAP_LOG_FUNC_ENTRY();

  memset(&get_ddns_resp_msg, 0, sizeof(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01));
  memset(&get_ddns_req_msg, 0, sizeof(qcmap_msgr_get_dynamic_dns_config_req_msg_v01));

  if(ddns_server == NULL)
  {
    LOG_MSG_ERROR("Invalid parameter passed ",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_DYNAMIC_DNS_CONFIG_REQ_V01,
                                       &get_ddns_req_msg,
                                       sizeof(qcmap_msgr_get_dynamic_dns_config_req_msg_v01),
                                       &get_ddns_resp_msg,
                                       sizeof(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_ddns_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get DDNS entries %d : %d",
                  qmi_error, get_ddns_resp_msg.resp.error,0);
    *qmi_err_num = get_ddns_resp_msg.resp.error;
    return false;
  }

  if ( get_ddns_resp_msg.ddns_config_len > 0)
  {
    num_entries = get_ddns_resp_msg.ddns_config_len;
    ddns_server->ddns_config_len = num_entries;
    LOG_MSG_INFO1("Num ddns entries confged: %d",num_entries,0,0);
    if ( num_entries > QCMAP_MSGR_MAX_DDNS_SERVER_ENTRIES_V01 && num_entries < 0)
    {
        LOG_MSG_INFO1("Invalid number of ddns config entries %d",num_entries,0,0);
        *qmi_err_num = get_ddns_resp_msg.resp.error;
        return false;
    }
    for( i=0; i < num_entries; i++)
    {
       memcpy( &(ddns_server->ddns_config[i].server_url), &(get_ddns_resp_msg.ddns_config[i].server_url), QCMAP_MSGR_DDNS_URL_LENGTH_V01);
    }

   if(get_ddns_resp_msg.hostname_valid)
    strlcpy(ddns_server->hostname,get_ddns_resp_msg.hostname,sizeof(ddns_server->hostname));

   if(get_ddns_resp_msg.timeout_valid)
    ddns_server->timeout = get_ddns_resp_msg.timeout;

    ddns_server->enable = get_ddns_resp_msg.enable;
  }
  else
  {
    LOG_MSG_INFO1("No ddns server configured configured",0,0,0);
    return false;
  }

  LOG_MSG_INFO1("Get ddns Configuration Succeeded...",0,0,0);
  return true;
}

/*===========================================================================
  FUNCTION GetTinyProxyStatus
  ===========================================================================*/
/*!
  @brief
  Returns the status of Tinyproxy

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::GetTinyProxyStatus(qcmap_msgr_tiny_proxy_mode_enum_v01 *tiny_proxy_status,
                                    qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_get_tiny_proxy_status_resp_msg_v01  status_resp;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&status_resp, 0, sizeof(qcmap_msgr_get_tiny_proxy_status_resp_msg_v01));
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_TINY_PROXY_STATUS_REQ_V01,
                                       NULL,
                                       0,
                                       (void*)&status_resp,
                                       sizeof(qcmap_msgr_get_tiny_proxy_status_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(enable): error %d result %d",
                 qmi_error, status_resp.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( status_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get TinyProxy status %d : %d",
        qmi_error, status_resp.resp.error, 0);
    *qmi_err_num = status_resp.resp.error;
    return false;
  }

  *tiny_proxy_status = status_resp.tiny_proxy_mode;
      LOG_MSG_ERROR("client .cpp  =%d",status_resp.tiny_proxy_mode,0,0);
  if(status_resp.tiny_proxy_mode == QCMAP_MSGR_TINY_PROXY_MODE_UP_V01)
  {
    LOG_MSG_INFO1("TinyProxy is enabled \n",0,0,0);
  }
  else if(status_resp.tiny_proxy_mode == QCMAP_MSGR_TINY_PROXY_MODE_DOWN_V01)
  {
    LOG_MSG_INFO1("TinyProxy is disabled \n",0,0,0);
  }

  return true;
}




/*===========================================================================
  FUNCTION EnableTinyProxy
  ===========================================================================*/
/*!
  @brief
  Enables TinyProxy

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::EnableTinyProxy(qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_enable_tiny_proxy_req_msg_v01 enable_tiny_proxy_req_msg_v01;
  qcmap_msgr_enable_tiny_proxy_resp_msg_v01 enable_tiny_proxy_resp_msg_v01;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&enable_tiny_proxy_req_msg_v01, 0, sizeof(enable_tiny_proxy_req_msg_v01));
  memset(&enable_tiny_proxy_resp_msg_v01, 0, sizeof(enable_tiny_proxy_resp_msg_v01));

  enable_tiny_proxy_req_msg_v01.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_ENABLE_TINY_PROXY_REQ_V01,
                                       &enable_tiny_proxy_req_msg_v01,
                                       sizeof(enable_tiny_proxy_req_msg_v01),
                                       (void*)&enable_tiny_proxy_resp_msg_v01,
                                       sizeof(enable_tiny_proxy_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(EnableTinyProxy): error %d result %d",
      qmi_error, enable_tiny_proxy_resp_msg_v01.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( enable_tiny_proxy_resp_msg_v01.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not Enable Tiny Proxy %d : %d",
        qmi_error, enable_tiny_proxy_resp_msg_v01.resp.error,0);
    *qmi_err_num = enable_tiny_proxy_resp_msg_v01.resp.error;
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION DisableTinyProxy
  ===========================================================================*/
/*!
  @brief
  Disables TinyProxy

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::DisableTinyProxy(qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_disable_tiny_proxy_req_msg_v01 disable_tiny_proxy_req_msg_v01;
  qcmap_msgr_disable_tiny_proxy_resp_msg_v01 disable_tiny_proxy_resp_msg_v01;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&disable_tiny_proxy_req_msg_v01, 0, sizeof(disable_tiny_proxy_req_msg_v01));
  memset(&disable_tiny_proxy_resp_msg_v01, 0, sizeof(disable_tiny_proxy_resp_msg_v01));

  disable_tiny_proxy_req_msg_v01.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_DISABLE_TINY_PROXY_REQ_V01,
                                       &disable_tiny_proxy_req_msg_v01,
                                       sizeof(disable_tiny_proxy_req_msg_v01),
                                       (void*)&disable_tiny_proxy_resp_msg_v01,
                                       sizeof(disable_tiny_proxy_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(DisableTinyProxy): error %d result %d",
      qmi_error, disable_tiny_proxy_resp_msg_v01.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( disable_tiny_proxy_resp_msg_v01.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not enable Dynamic Tiny Proxy %d : %d",
    qmi_error, disable_tiny_proxy_resp_msg_v01.resp.error,0);
    *qmi_err_num = disable_tiny_proxy_resp_msg_v01.resp.error;
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION SetDLNAWhitelisting
  ===========================================================================*/
/*!
  @brief
  Sets the DLNA Whitelisting

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::SetDLNAWhitelisting(boolean dlna_whitelisting_allow, qmi_error_type_v01 *qmi_err_num)
{
 qcmap_msgr_set_dlna_whitelisting_req_msg_v01 set_dlna_whitelist_req_msg;
 qcmap_msgr_set_dlna_whitelisting_resp_msg_v01 set_dlna_whitelist_resp_msg;
 qmi_client_error_type qmi_error;

 set_dlna_whitelist_req_msg.mobile_ap_handle = this->mobile_ap_handle;
 set_dlna_whitelist_req_msg.dlna_whitelist_allow= dlna_whitelisting_allow;
 LOG_MSG_INFO1("SET dlna whitelisting Client side %d", dlna_whitelisting_allow,0,0);

 qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_DLNA_WHITELISTING_REQ_V01,
                                       &set_dlna_whitelist_req_msg,
                                       sizeof(qcmap_msgr_set_dlna_whitelisting_req_msg_v01),
                                       &set_dlna_whitelist_resp_msg,
                                       sizeof(qcmap_msgr_set_dlna_whitelisting_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);
  if (( qmi_error == QMI_TIMEOUT_ERR ) ||
      ( qmi_error != QMI_NO_ERR) ||
      ( set_dlna_whitelist_resp_msg.resp.result != QMI_NO_ERR ))
  {
    LOG_MSG_ERROR("Can not set dlna whitelisting %d : %d",
        qmi_error, set_dlna_whitelist_resp_msg.resp.error,0);
    *qmi_err_num = set_dlna_whitelist_resp_msg.resp.error;
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION GetDLNAWhitelisting
  ===========================================================================*/
/*!
  @brief
  Gets the GetDLNAWhitelisting Config

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::GetDLNAWhitelisting(boolean *dlna_whitelisting_allow,qmi_error_type_v01 *qmi_err_num)
{
 qcmap_msgr_get_dlna_whitelisting_req_msg_v01 get_dlna_whitelist_req_msg;
 qcmap_msgr_get_dlna_whitelisting_resp_msg_v01 get_dlna_whitelist_resp_msg;
 qmi_client_error_type qmi_error;
 get_dlna_whitelist_req_msg.mobile_ap_handle = this->mobile_ap_handle;

 qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_DLNA_WHITELISTING_REQ_V01,
                                       &get_dlna_whitelist_req_msg,
                                       sizeof(qcmap_msgr_get_dlna_whitelisting_req_msg_v01),
                                       &get_dlna_whitelist_resp_msg,
                                       sizeof(qcmap_msgr_get_dlna_whitelisting_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);
  if (( qmi_error == QMI_TIMEOUT_ERR ) ||
      ( qmi_error != QMI_NO_ERR) ||
      ( get_dlna_whitelist_resp_msg.resp.result != QMI_NO_ERR ))
  {
    LOG_MSG_ERROR("Can not get firewall config %d : %d",
        qmi_error, get_dlna_whitelist_resp_msg.resp.error,0);
    *qmi_err_num = get_dlna_whitelist_resp_msg.resp.error;
    return false;
  }
  /* Need to add check for optional value */
  if ( get_dlna_whitelist_resp_msg.dlna_whitelist_allow_valid)
  {
    *dlna_whitelisting_allow = get_dlna_whitelist_resp_msg.dlna_whitelist_allow;
  }
 return true;
}


/*===========================================================================
  FUNCTION AddDLNAWhitelistIP
  ===========================================================================*/
/*!
  @brief
  Adds a AddDLNAWhitelistIP entry

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::AddDLNAWhitelistIP(uint32 dlna_whitelisting_ip , qmi_error_type_v01 *qmi_err_num)
{
  qcmap_msgr_add_dlna_whitelistip_req_msg_v01 add_dlna_whitelist_ip_req_msg;
  qcmap_msgr_add_dlna_whitelistip_resp_msg_v01 add_dlna_whitelist_ip_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  add_dlna_whitelist_ip_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  add_dlna_whitelist_ip_req_msg.dlna_whitelist_ip_addr= dlna_whitelisting_ip;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_ADD_DLNA_WHITELISTIP_REQ_V01,
                                       &add_dlna_whitelist_ip_req_msg,
                                       sizeof(qcmap_msgr_add_dlna_whitelistip_req_msg_v01),
                                       &add_dlna_whitelist_ip_resp_msg,
                                       sizeof(qcmap_msgr_add_dlna_whitelistip_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( add_dlna_whitelist_ip_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not add dlna whitelist ip %d : %d",
        qmi_error, add_dlna_whitelist_ip_resp_msg.resp.error,0);
    *qmi_err_num = add_dlna_whitelist_ip_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("Added DLNA Whitelist IP...",0,0,0);
  return true;
}

/*===========================================================================
  FUNCTION DeleteDLNAWhitelistIP
  ===========================================================================*/
/*!
  @brief
  Deletes a DeleteDLNAWhitelistIP entry

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::DeleteDLNAWhitelistIP(uint32 dlna_whitelisting_ip,qmi_error_type_v01 *qmi_err_num)
{
  qcmap_msgr_delete_dlna_whitelist_ip_req_msg_v01 delete_dlna_whitelist_ip_req_msg;
  qcmap_msgr_delete_dlna_whitelist_ip_resp_msg_v01 delete_dlna_whitelist_ip_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  delete_dlna_whitelist_ip_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  delete_dlna_whitelist_ip_req_msg.dlna_whitelist_ip_addr = dlna_whitelisting_ip;
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_DELETE_DLNA_WHITELIST_IP_REQ_V01,
                                       &delete_dlna_whitelist_ip_req_msg,
                                       sizeof(qcmap_msgr_delete_dlna_whitelist_ip_req_msg_v01),
                                       &delete_dlna_whitelist_ip_resp_msg,
                                       sizeof(qcmap_msgr_delete_dlna_whitelist_ip_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( delete_dlna_whitelist_ip_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not delete dlna whitelist ip %d : %d",
        qmi_error, delete_dlna_whitelist_ip_resp_msg.resp.error,0);
    *qmi_err_num = delete_dlna_whitelist_ip_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("Deleted DLNA whitelist ip...",0,0,0);
  return true;
}

/*===========================================================================
  FUNCTION SetUPNPState
  ===========================================================================*/
/*!
  @brief
  Sets the UPNP State

  @return
  true  - on Success
  false - on Failure

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::SetUPNPState(boolean firewall_state, boolean upnp_pinhole_flag,
                                   qmi_error_type_v01 *qmi_err_num)
{
 boolean upnp_pinhole_flag_valid = true;
 boolean enable_firewall, pkts_allowed = false;
 qcmap_msgr_set_firewall_config_req_msg_v01 set_firewall_req_msg;
 qcmap_msgr_set_firewall_config_resp_msg_v01 set_firewall_resp_msg;
 qmi_client_error_type qmi_error;
 *qmi_err_num = QMI_ERR_NONE_V01;

 memset(&set_firewall_req_msg, 0, sizeof(qcmap_msgr_set_firewall_config_req_msg_v01));
 memset(&set_firewall_resp_msg, 0, sizeof(qcmap_msgr_set_firewall_config_resp_msg_v01));

 GetFirewall(&enable_firewall, &pkts_allowed, qmi_err_num);
 if(*qmi_err_num != QMI_ERR_NONE_V01)
 {
   LOG_MSG_INFO1("Failed to get current firewall state. Error: %d ",*qmi_err_num,0,0);
   return false;
 }
 if(enable_firewall != firewall_state)
 {
   LOG_MSG_INFO1("SetUPNPState(): Setting firewall state to = %d ",enable_firewall,0,0);
 }

 set_firewall_req_msg.mobile_ap_handle = this->mobile_ap_handle;
 set_firewall_req_msg.firewall_enabled = firewall_state;

 if(firewall_state)
 {
   set_firewall_req_msg.pkts_allowed_valid = true;
   set_firewall_req_msg.pkts_allowed = pkts_allowed;

   set_firewall_req_msg.upnp_pinhole_flag_valid = true;
   set_firewall_req_msg.upnp_pinhole_flag = upnp_pinhole_flag;
 }

 LOG_MSG_INFO1("\n SetUPNPState(): firewall_enabled =%d pkts_allowed =%d & upnp_pinhole_flag:%d ",
                enable_firewall, pkts_allowed, upnp_pinhole_flag);
 qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_FIREWALL_CONFIG_REQ_V01,
                                       &set_firewall_req_msg,
                                       sizeof(qcmap_msgr_set_firewall_config_req_msg_v01),
                                       &set_firewall_resp_msg,
                                       sizeof(qcmap_msgr_set_firewall_config_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);
  if (( qmi_error == QMI_TIMEOUT_ERR ) ||
      ( qmi_error != QMI_NO_ERR) ||
      ( set_firewall_resp_msg.resp.result != QMI_NO_ERR ))
  {
    LOG_MSG_ERROR("Can not set firewall config %d : %d",
        qmi_error, set_firewall_resp_msg.resp.error,0);
    *qmi_err_num = set_firewall_resp_msg.resp.error;
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION GetUPNPStatus
  ===========================================================================*/
/*!
  @brief
  Gets the UPNP state

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::GetUPNPState(boolean *upnp_pinhole_flag,qmi_error_type_v01 *qmi_err_num)
{
 qcmap_msgr_get_firewall_config_req_msg_v01 get_firewall_req_msg;
 qcmap_msgr_get_firewall_config_resp_msg_v01 get_firewall_resp_msg;
 qmi_client_error_type qmi_error;
 get_firewall_req_msg.mobile_ap_handle = this->mobile_ap_handle;

 qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_FIREWALL_CONFIG_REQ_V01,
                                       &get_firewall_req_msg,
                                       sizeof(qcmap_msgr_get_firewall_config_req_msg_v01),
                                       &get_firewall_resp_msg,
                                       sizeof(qcmap_msgr_get_firewall_config_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);
  if (( qmi_error == QMI_TIMEOUT_ERR ) ||
      ( qmi_error != QMI_NO_ERR) ||
      ( get_firewall_resp_msg.resp.result != QMI_NO_ERR ))
  {
    LOG_MSG_ERROR("Can not get firewall config %d : %d",
        qmi_error, get_firewall_resp_msg.resp.error,0);
    *qmi_err_num = get_firewall_resp_msg.resp.error;
    return false;
  }
  /* Need to add check for optional value */
  if ( get_firewall_resp_msg.upnp_pinhole_flag_valid)
  {
    *upnp_pinhole_flag = get_firewall_resp_msg.upnp_pinhole_flag;
  }

 return true;
}

/*===========================================================================
  FUNCTION SetActiveBackhaulPref
  ===========================================================================*/
/*!
  @brief
  Sets Backhaul Preference

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::SetActiveBackhaulPref(backhaul_pref_t *backhaul_pref_req, qmi_error_type_v01 *qmi_err_num)
{
  qcmap_msgr_set_backhaul_pref_req_msg_v01 set_backhaul_pref_req_msg;
  qcmap_msgr_set_backhaul_pref_resp_msg_v01 set_backhaul_pref_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();
  memset(&set_backhaul_pref_req_msg,0,sizeof(qcmap_msgr_set_backhaul_pref_req_msg_v01));
  memset(&set_backhaul_pref_resp_msg,0,sizeof(qcmap_msgr_set_backhaul_pref_resp_msg_v01));

  if(backhaul_pref_req == NULL)
  {
    LOG_MSG_ERROR("Invalid parameter passed ",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  set_backhaul_pref_req_msg.first_priority_backhaul = backhaul_pref_req->first;
  set_backhaul_pref_req_msg.second_priority_backhaul = backhaul_pref_req->second;
  set_backhaul_pref_req_msg.third_priority_backhaul = backhaul_pref_req->third;
  set_backhaul_pref_req_msg.fourth_priority_backhaul = backhaul_pref_req->fourth;
  set_backhaul_pref_req_msg.fifth_priority_backhaul = backhaul_pref_req->fifth;
  set_backhaul_pref_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_BACKHAUL_PREF_REQ_V01,
                                       &set_backhaul_pref_req_msg,
                                       sizeof(qcmap_msgr_set_backhaul_pref_req_msg_v01),
                                       &set_backhaul_pref_resp_msg,
                                       sizeof(qcmap_msgr_set_backhaul_pref_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);
  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( set_backhaul_pref_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not set backhaul pref %d : %d",
        qmi_error, set_backhaul_pref_resp_msg.resp.error,0);
    *qmi_err_num = set_backhaul_pref_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("Backhaul Preference is Set succesfully...",0,0,0);
  return true;
}

/*===========================================================================
  FUNCTION GetBackhaulPref
  ===========================================================================*/
/*!
  @brief
  Gets Current Backhaul Preference

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::GetBackhaulPref
(
  backhaul_pref_t *resp,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_get_backhaul_pref_resp_msg_v01 get_backhaul_resp_msg;
  qmi_client_error_type qmi_error = QMI_NO_ERR;

  QCMAP_LOG_FUNC_ENTRY();

  if(resp == NULL)
  {
    LOG_MSG_ERROR("Invalid parameter passed ",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  memset(&get_backhaul_resp_msg,0,sizeof(qcmap_msgr_get_backhaul_pref_resp_msg_v01));

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_BACKHAUL_PREF_REQ_V01,
                                       NULL,
                                       0,
                                       &get_backhaul_resp_msg,
                                       sizeof(qcmap_msgr_get_backhaul_pref_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_backhaul_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get backhaul pref %d : %d",
        qmi_error, get_backhaul_resp_msg.resp.error,0);
    *qmi_err_num = get_backhaul_resp_msg.resp.error;
    return false;
  }

  if (get_backhaul_resp_msg.first_priority_backhaul_valid)
  {
    LOG_MSG_INFO1("First  = %d",get_backhaul_resp_msg.first_priority_backhaul,0,0);
    resp->first = get_backhaul_resp_msg.first_priority_backhaul;
  }
  if (get_backhaul_resp_msg.second_priority_backhaul_valid)
  {

    LOG_MSG_INFO1("Second  = %d",get_backhaul_resp_msg.second_priority_backhaul,0,0);
    resp->second = get_backhaul_resp_msg.second_priority_backhaul;
  }
  if (get_backhaul_resp_msg.third_priority_backhaul_valid)
  {

    LOG_MSG_INFO1("Third  = %d",get_backhaul_resp_msg.third_priority_backhaul,0,0);
    resp->third = get_backhaul_resp_msg.third_priority_backhaul;
  }
  if (get_backhaul_resp_msg.fourth_priority_backhaul_valid)
  {

    LOG_MSG_INFO1("Fourth  = %d",get_backhaul_resp_msg.fourth_priority_backhaul,0,0);
    resp->fourth = get_backhaul_resp_msg.fourth_priority_backhaul;
  }
  if (get_backhaul_resp_msg.fifth_priority_backhaul_valid)
  {

    LOG_MSG_INFO1("Fifth  = %d",get_backhaul_resp_msg.fifth_priority_backhaul,0,0);
    resp->fifth= get_backhaul_resp_msg.fifth_priority_backhaul;
  }
  LOG_MSG_INFO1("Got Backhaul Preference succesfully...",0,0,0);
  return true;
}
/*===========================================================================
  FUNCTION GetEthernetMode
  ===========================================================================*/
/*!
  @brief
  Gets Ethernet Tethering Mode

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::GetEthernetMode
(
   qcmap_msgr_ethernet_mode_v01 *mode,
   qmi_error_type_v01 *qmi_err_num
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_get_ethernet_mode_resp_msg_v01 eth_mode_resp;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&eth_mode_resp, 0, sizeof(
                           qcmap_msgr_get_ethernet_mode_resp_msg_v01));
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                          QMI_QCMAP_MSGR_GET_ETHERNET_MODE_REQ_V01,
                          NULL,
                          0,
                          &eth_mode_resp,
                          sizeof(qcmap_msgr_get_ethernet_mode_resp_msg_v01),
                          QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(GetEthernetMode):"
                " error %d result %d",
      qmi_error, eth_mode_resp.resp.result, 0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( eth_mode_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get ethernet mode %d : %d",
        qmi_error, eth_mode_resp.resp.error, 0);
    *qmi_err_num = eth_mode_resp.resp.error;
    return false;
  }

  *mode = eth_mode_resp.mode;

  return true;
}

/*===========================================================================
  FUNCTION SetEthernetMode
  ===========================================================================*/
/*!
  @brief
  Sets Ethernet Tethering Mode

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::SetEthernetMode
(
   qcmap_msgr_ethernet_mode_v01 mode,
   qmi_error_type_v01 *qmi_err_num
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_set_ethernet_mode_req_msg_v01 eth_mode_req;
  qcmap_msgr_set_ethernet_mode_resp_msg_v01 eth_mode_resp;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&eth_mode_req, 0, sizeof(qcmap_msgr_set_ethernet_mode_req_msg_v01));
  memset(&eth_mode_resp, 0, sizeof(qcmap_msgr_set_ethernet_mode_resp_msg_v01));

  eth_mode_req.mode = mode;
  eth_mode_req.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                           QMI_QCMAP_MSGR_SET_ETHERNET_MODE_REQ_V01,
                           &eth_mode_req,
                           sizeof(qcmap_msgr_set_ethernet_mode_req_msg_v01),
                           &eth_mode_resp,
                          sizeof(qcmap_msgr_set_ethernet_mode_resp_msg_v01),
                           QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(SetEthernetMode):"
                " error %d result %d",
      qmi_error, eth_mode_resp.resp.result, 0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( eth_mode_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not set ethernet mode %d : %d",
        qmi_error, eth_mode_resp.resp.error, 0);
    *qmi_err_num = eth_mode_resp.resp.error;
    return false;
  }

  return true;
}
/*===========================================================================
  FUNCTION Set IP Passthrough Configuration
  ===========================================================================*/
/** @ingroup qcmap_set_ip_passthrough_config

  Set the IP Passthrough configuration for tethered client.

  @datatypes
  qcmap_msgr_ip_passthrough_mode_enum_v01
  qcmap_msgr_ip_passthrough_config_v01
  qmi_error_type_v01

  @param[in] enable_state                IP passthrough enable state.
  @param[in] new_config                  Use New/Existing config
  @param[in] ip_passthrough_config       IP passthrough Configuration.
  @param[out] qmi_err_num                Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/

/*=========================================================================*/
boolean QCMAP_Client::SetIPPassthroughConfig
(
  qcmap_msgr_ip_passthrough_mode_enum_v01 enable_state,
  bool new_config,
  qcmap_msgr_ip_passthrough_config_v01 *ip_passthrough_config,
  qmi_error_type_v01 *qmi_err_num
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_set_ip_passthrough_flag_req_msg_v01 set_ip_passthrough_flag_req_msg;
  qcmap_msgr_set_ip_passthrough_flag_resp_msg_v01 set_ip_passthrough_flag_resp_msg;

  QCMAP_LOG_FUNC_ENTRY();

  if ( !qmi_err_num )
  {
    LOG_MSG_ERROR("SetIPPassthroughConfig Failed: Invalid Args", 0,0,0);
    return false;
  }

  /* if new config is to be set and passthrough is not present. */
  if ( new_config && !ip_passthrough_config )
  {
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    LOG_MSG_ERROR("SetIPPassthroughConfig Failed: %d ",*qmi_err_num ,0,0);
    return false;
  }

  memset(&set_ip_passthrough_flag_req_msg, 0, sizeof(set_ip_passthrough_flag_req_msg));
  memset(&set_ip_passthrough_flag_resp_msg, 0, sizeof(set_ip_passthrough_flag_resp_msg));

  set_ip_passthrough_flag_req_msg.enable_state = enable_state;
  set_ip_passthrough_flag_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  if (new_config)
  {
    set_ip_passthrough_flag_req_msg.passthrough_config_valid = true;
    memcpy(&set_ip_passthrough_flag_req_msg.passthrough_config, ip_passthrough_config, \
           sizeof(qcmap_msgr_ip_passthrough_config_v01));
  }

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_IP_PASSTHROUGH_FLAG_REQ_V01,
                                       &set_ip_passthrough_flag_req_msg,
                                       sizeof(set_ip_passthrough_flag_req_msg),
                                       (void*)&set_ip_passthrough_flag_resp_msg,
                                       sizeof(set_ip_passthrough_flag_resp_msg),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(SetIPPassthrough): error %d result %d",
      qmi_error, set_ip_passthrough_flag_resp_msg.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( set_ip_passthrough_flag_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Set IP Passthrough failed %d : %d",
                  qmi_error, set_ip_passthrough_flag_resp_msg.resp.error,0);
    *qmi_err_num = set_ip_passthrough_flag_resp_msg.resp.error;
    return false;
  }
  return true;
}

/*===========================================================================
FUNCTION GetIPPassthroughConfig()
===========================================================================*/
/** @ingroup qcmap_get_ip_passthrough_flag

  Get the IP Passthrough configuration.

  @datatypes
  qcmap_msgr_ip_passthrough_mode_enum_v01
  qcmap_msgr_ip_passthrough_config_v01
  qmi_error_type_v01

  @param[out] enable_state                Current state of IP Passthrough
  @param[out] ip_passthrough_config       IP Passthrough Configuration.
  @param[out] qmi_err_num                 Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/

/*=========================================================================*/
boolean QCMAP_Client::GetIPPassthroughConfig
(
  qcmap_msgr_ip_passthrough_mode_enum_v01 *enable_state,
  qcmap_msgr_ip_passthrough_config_v01 *ip_passthrough_config,
  qmi_error_type_v01 *qmi_err_num
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_get_ip_passthrough_flag_req_msg_v01 get_ip_passthrough_flag_req_msg;
  qcmap_msgr_get_ip_passthrough_flag_resp_msg_v01 get_ip_passthrough_flag_resp_msg;

  QCMAP_LOG_FUNC_ENTRY();

  if ( !qmi_err_num )
  {
    LOG_MSG_ERROR("GetIPPassthroughConfig Failed: Invalid Args", 0,0,0);
    return false;
  }

  if ( !ip_passthrough_config )
  {
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    LOG_MSG_ERROR("GetIPPassthroughConfig Failed: %d ",*qmi_err_num ,0,0);
    return false;
  }

  memset(&get_ip_passthrough_flag_req_msg, 0, sizeof(get_ip_passthrough_flag_req_msg));
  memset(&get_ip_passthrough_flag_resp_msg, 0, sizeof(get_ip_passthrough_flag_resp_msg));

  get_ip_passthrough_flag_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_IP_PASSTHROUGH_FLAG_REQ_V01,
                                       &get_ip_passthrough_flag_req_msg,
                                       sizeof(get_ip_passthrough_flag_req_msg),
                                       (void*)&get_ip_passthrough_flag_resp_msg,
                                       sizeof(get_ip_passthrough_flag_resp_msg),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(GetIPPassthroughConfig): error %d result %d",
      qmi_error, get_ip_passthrough_flag_resp_msg.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_ip_passthrough_flag_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Get IP Passthrough Flag failed %d : %d",
        qmi_error, get_ip_passthrough_flag_resp_msg.resp.error,0);
    *qmi_err_num = get_ip_passthrough_flag_resp_msg.resp.error;
    return false;
  }

  *enable_state = get_ip_passthrough_flag_resp_msg.enable_state;
  memcpy(ip_passthrough_config, &get_ip_passthrough_flag_resp_msg.passthrough_config,\
         sizeof(qcmap_msgr_ip_passthrough_config_v01));
  return true;
}

/*===========================================================================
FUNCTION GetIPPassthroughState()
===========================================================================*/
/** @ingroup qcmap_get_ip_passthrough_state

  Get the IP Passthrough active state.

  @datatypes
  qmi_error_type_v01

  @param[out] active_state                Current active state of IP Passthrough
  @param[out] qmi_err_num                 Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/

/*=========================================================================*/
boolean QCMAP_Client::GetIPPassthroughState
(
  boolean *state,
  qmi_error_type_v01 *qmi_err_num
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_get_ip_passthrough_state_req_msg_v01
  get_ip_passthrough_state_req_msg;
  qcmap_msgr_get_ip_passthrough_state_resp_msg_v01
  get_ip_passthrough_state_resp_msg;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&get_ip_passthrough_state_req_msg, 0, sizeof(get_ip_passthrough_state_req_msg));
  memset(&get_ip_passthrough_state_resp_msg, 0, sizeof(get_ip_passthrough_state_resp_msg));

  get_ip_passthrough_state_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_IP_PASSTHROUGH_STATE_REQ_V01,
                                       &get_ip_passthrough_state_req_msg,
                                       sizeof(get_ip_passthrough_state_req_msg),
                                       (void*)&get_ip_passthrough_state_resp_msg,
                                       sizeof(get_ip_passthrough_state_resp_msg),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(GetIPPassthroughState): error %d result %d",
                             qmi_error, get_ip_passthrough_state_resp_msg.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_ip_passthrough_state_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Get IP Passthrough State failed %d : %d",
                               qmi_error, get_ip_passthrough_state_resp_msg.resp.error,0);
    *qmi_err_num = get_ip_passthrough_state_resp_msg.resp.error;
    return false;
  }

  *state = get_ip_passthrough_state_resp_msg.state;
  return true;
}


/*===========================================================================
  FUNCTION BringupBTTethering
  ===========================================================================*/
/*!
  @brief
  Brings up the BT Tethering

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::BringupBTTethering
(
  qmi_error_type_v01 *qmi_err_num,
  qcmap_bt_tethering_mode_enum_v01 bt_tethering_mode
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_bring_up_bt_tethering_req_msg_v01 bring_up_bt_tethering_req_msg_v01;
  qcmap_msgr_bring_up_bt_tethering_resp_msg_v01 bring_up_bt_tethering_resp_msg_v01;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&bring_up_bt_tethering_req_msg_v01,
         0, sizeof(qcmap_msgr_bring_up_bt_tethering_req_msg_v01));
  memset(&bring_up_bt_tethering_resp_msg_v01,
         0, sizeof(qcmap_msgr_bring_up_bt_tethering_resp_msg_v01));

  bring_up_bt_tethering_req_msg_v01.mobile_ap_handle = this->mobile_ap_handle;

  if (bt_tethering_mode == QCMAP_MSGR_BT_MODE_LAN_V01 ||
      bt_tethering_mode == QCMAP_MSGR_BT_MODE_WAN_V01)
  {
    bring_up_bt_tethering_req_msg_v01.bt_mode_valid = true;
    bring_up_bt_tethering_req_msg_v01.bt_mode = bt_tethering_mode;
  }

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_BRING_UP_BT_TETHERING_REQ_V01,
                                       &bring_up_bt_tethering_req_msg_v01,
                                       sizeof(qcmap_msgr_bring_up_bt_tethering_req_msg_v01),
                                       (void*)&bring_up_bt_tethering_resp_msg_v01,
                                       sizeof(qcmap_msgr_bring_up_bt_tethering_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(BringupBTTethering): error %d result %d",
      qmi_error, bring_up_bt_tethering_resp_msg_v01.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( bring_up_bt_tethering_resp_msg_v01.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not bring up BT tethering %d : %d",
        qmi_error, bring_up_bt_tethering_resp_msg_v01.resp.error,0);
    *qmi_err_num = bring_up_bt_tethering_resp_msg_v01.resp.error;
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION BringdownBTTethering
  ===========================================================================*/
/*!
  @brief
  Brings down the BT Tethering

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::BringdownBTTethering(qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_bring_down_bt_tethering_req_msg_v01 bring_down_bt_tethering_req_msg_v01;
  qcmap_msgr_bring_down_bt_tethering_resp_msg_v01 bring_down_bt_tethering_resp_msg_v01;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&bring_down_bt_tethering_req_msg_v01, 0, sizeof(qcmap_msgr_bring_down_bt_tethering_req_msg_v01));
  memset(&bring_down_bt_tethering_resp_msg_v01, 0, sizeof(qcmap_msgr_bring_down_bt_tethering_resp_msg_v01));

  bring_down_bt_tethering_req_msg_v01.mobile_ap_handle = this->mobile_ap_handle;


  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_BRING_DOWN_BT_TETHERING_REQ_V01,
                                       &bring_down_bt_tethering_req_msg_v01,
                                       sizeof(qcmap_msgr_bring_down_bt_tethering_req_msg_v01),
                                       (void*)&bring_down_bt_tethering_resp_msg_v01,
                                       sizeof(qcmap_msgr_bring_down_bt_tethering_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(BringdownBTTethering): error %d result %d",
      qmi_error, bring_down_bt_tethering_resp_msg_v01.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( bring_down_bt_tethering_resp_msg_v01.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not bring down BT tethering %d : %d",
        qmi_error, bring_down_bt_tethering_resp_msg_v01.resp.error,0);
    *qmi_err_num = bring_down_bt_tethering_resp_msg_v01.resp.error;
    return false;
  }
  return true;
}


/*===========================================================================
  FUNCTION GetBTTetheringStatus
  ===========================================================================*/
/*!
  @brief
  Displays the BT Tethering current status & mode

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::GetBTTetheringStatus
(
  qcmap_msgr_bt_tethering_status_enum_v01 *bt_teth_status,
  qmi_error_type_v01 *qmi_err_num,
  qcmap_bt_tethering_mode_enum_v01 *bt_teth_mode
)
{
  qcmap_msgr_get_bt_tethering_status_req_msg_v01 get_bt_tethering_status_req_msg;
  qcmap_msgr_get_bt_tethering_status_resp_msg_v01 get_bt_tethering_status_resp_msg;
  qmi_client_error_type qmi_error;
  QCMAP_LOG_FUNC_ENTRY();
  get_bt_tethering_status_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_BT_TETHERING_STATUS_REQ_V01,
                                       &get_bt_tethering_status_req_msg,
                                       sizeof(qcmap_msgr_get_bt_tethering_status_req_msg_v01),
                                       &get_bt_tethering_status_resp_msg,
                                       sizeof(qcmap_msgr_get_bt_tethering_status_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_bt_tethering_status_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get Bt tethering status %d : %d",
                   qmi_error, get_bt_tethering_status_resp_msg.resp.error,0);
    *qmi_err_num = get_bt_tethering_status_resp_msg.resp.error;
    return false;
  }
  if(bt_teth_status && get_bt_tethering_status_resp_msg.bt_tethering_status_valid)
    *bt_teth_status = get_bt_tethering_status_resp_msg.bt_tethering_status;
  if(bt_teth_mode && get_bt_tethering_status_resp_msg.bt_tethering_mode_valid)
    *bt_teth_mode = get_bt_tethering_status_resp_msg.bt_tethering_mode;
  return true;
}
/*===========================================================================
  FUNCTION Set Initial Packet Threshold
  ===========================================================================*/
/*!
  @brief
  Set the packet threshold count to delay the time for the initial
  packets to flow through HW or SFE path. Until the packet threshold is reached
  packets will take the software path


  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::SetInitialPacketLimit
(
   uint32 pkt_limit,
   qmi_error_type_v01 *qmi_err_num
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_set_initial_packet_threshold_req_msg_v01 set_initial_pkt_threshold_req;
  qcmap_msgr_set_initial_packet_threshold_resp_msg_v01 set_initial_pkt_threshold_resp;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&set_initial_pkt_threshold_req, 0, sizeof(qcmap_msgr_set_initial_packet_threshold_req_msg_v01));
  memset(&set_initial_pkt_threshold_resp, 0, sizeof(qcmap_msgr_set_initial_packet_threshold_resp_msg_v01));

  set_initial_pkt_threshold_req.mobile_ap_handle = this->mobile_ap_handle;
  set_initial_pkt_threshold_req.packet_count = pkt_limit;
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_INITIAL_PACKET_THRESHOLD_REQ_V01,
                                       &set_initial_pkt_threshold_req,
                                       sizeof(qcmap_msgr_set_initial_packet_threshold_req_msg_v01),
                                       &set_initial_pkt_threshold_resp,
                                       sizeof(qcmap_msgr_set_initial_packet_threshold_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(SetInitialPacketLimit): error %d result %d",
      qmi_error, set_initial_pkt_threshold_resp.resp.result, 0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( set_initial_pkt_threshold_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not set SetInitialPacketLimit %d : %d",
                   qmi_error, set_initial_pkt_threshold_resp.resp.error, 0);
    *qmi_err_num = set_initial_pkt_threshold_resp.resp.error;
    return false;
  }

  return true;
}

/*===========================================================================
  FUNCTION Get Initial Packet Threshold
  ===========================================================================*/
/*!
  @brief
  Gets the packet threshold count to delay the time for the initial
  packets to flow through HW path. Until the packet threshold is reached
  packets will take the software path


  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::GetInitialPacketLimit
(
   uint32 *pkt_limit,
   qmi_error_type_v01 *qmi_err_num
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_get_initial_packet_threshold_req_msg_v01 get_initial_pkt_threshold_req;
  qcmap_msgr_get_initial_packet_threshold_resp_msg_v01 get_initial_pkt_threshold_resp;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&get_initial_pkt_threshold_req, 0, sizeof(qcmap_msgr_get_initial_packet_threshold_req_msg_v01));
  memset(&get_initial_pkt_threshold_resp, 0, sizeof(qcmap_msgr_get_initial_packet_threshold_resp_msg_v01));

  get_initial_pkt_threshold_req.mobile_ap_handle = this->mobile_ap_handle;
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_INITIAL_PACKET_THRESHOLD_REQ_V01,
                                       &get_initial_pkt_threshold_req,
                                       sizeof(qcmap_msgr_get_initial_packet_threshold_req_msg_v01),
                                       &get_initial_pkt_threshold_resp,
                                       sizeof(qcmap_msgr_get_initial_packet_threshold_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(GetInitialPacketLimit): error %d result %d",
      qmi_error, get_initial_pkt_threshold_resp.resp.result, 0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_initial_pkt_threshold_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not set GetInitialPacketLimit %d : %d",
                   qmi_error, get_initial_pkt_threshold_resp.resp.error, 0);
    *qmi_err_num = get_initial_pkt_threshold_resp.resp.error;
    return false;
  }
  *pkt_limit = get_initial_pkt_threshold_resp.packet_count;

  return true;
}

/*===========================================================================
  FUNCTION EnableSOCKSv5Proxy
  ===========================================================================*/
/*!
  @brief
  Enables SOCKSv5 Proxy Daemon

  SOCKSv5 proxy daemon remains running in background even on reboot if
  EnableSOCKSv5Proxy tag is 1.

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::EnableSOCKSv5Proxy(qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_enable_socksv5_proxy_req_msg_v01 enable_socksv5_proxy_req_msg_v01;
  qcmap_msgr_enable_socksv5_proxy_resp_msg_v01 enable_socksv5_proxy_resp_msg_v01;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&enable_socksv5_proxy_req_msg_v01, 0, sizeof(enable_socksv5_proxy_req_msg_v01));
  memset(&enable_socksv5_proxy_resp_msg_v01, 0, sizeof(enable_socksv5_proxy_resp_msg_v01));

  enable_socksv5_proxy_req_msg_v01.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
               QMI_QCMAP_MSGR_ENABLE_SOCKSV5_PROXY_REQ_V01,
               &enable_socksv5_proxy_req_msg_v01,
               sizeof(enable_socksv5_proxy_req_msg_v01),
               (void*)&enable_socksv5_proxy_resp_msg_v01,
               sizeof(enable_socksv5_proxy_resp_msg_v01),
               QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(EnableSOCKSv5Proxy): error %d result %d",
      qmi_error, enable_socksv5_proxy_resp_msg_v01.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( enable_socksv5_proxy_resp_msg_v01.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not Enable SOCKSv5 Proxy %d : %d",
  qmi_error, enable_socksv5_proxy_resp_msg_v01.resp.error,0);
    *qmi_err_num = enable_socksv5_proxy_resp_msg_v01.resp.error;
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION DisableSOCKSv5Proxy
  ===========================================================================*/
/*!
  @brief
  Disables SOCKSv5 Proxy

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::DisableSOCKSv5Proxy(qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_disable_socksv5_proxy_req_msg_v01 disable_socksv5_proxy_req_msg_v01;
  qcmap_msgr_disable_socksv5_proxy_resp_msg_v01 disable_socksv5_proxy_resp_msg_v01;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&disable_socksv5_proxy_req_msg_v01, 0, sizeof(disable_socksv5_proxy_req_msg_v01));
  memset(&disable_socksv5_proxy_resp_msg_v01, 0, sizeof(disable_socksv5_proxy_resp_msg_v01));

  disable_socksv5_proxy_req_msg_v01.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
               QMI_QCMAP_MSGR_DISABLE_SOCKSV5_PROXY_REQ_V01,
               &disable_socksv5_proxy_req_msg_v01,
               sizeof(disable_socksv5_proxy_req_msg_v01),
               (void*)&disable_socksv5_proxy_resp_msg_v01,
               sizeof(disable_socksv5_proxy_resp_msg_v01),
               QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(DisableSOCKSv5Proxy): error %d result %d",
      qmi_error, disable_socksv5_proxy_resp_msg_v01.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( disable_socksv5_proxy_resp_msg_v01.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not Disable SOCKSv5 Proxy %d : %d",
    qmi_error, disable_socksv5_proxy_resp_msg_v01.resp.error,0);
    *qmi_err_num = disable_socksv5_proxy_resp_msg_v01.resp.error;
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION GetSOCKSv5Config
  ===========================================================================*/
/*!
  @brief
  Gets SOCKSv5 Proxy config

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::GetSOCKSv5Config
(
socksv5_configuration *configuration,
qmi_error_type_v01 *qmi_err_num
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_get_socksv5_proxy_config_req_msg_v01 get_socksv5_proxy_config_req_msg_v01;
  qcmap_msgr_get_socksv5_proxy_config_resp_msg_v01 get_socksv5_proxy_config_resp_msg_v01;

  memset(&get_socksv5_proxy_config_req_msg_v01, 0, sizeof(get_socksv5_proxy_config_req_msg_v01));
  memset(&get_socksv5_proxy_config_resp_msg_v01, 0, sizeof(get_socksv5_proxy_config_resp_msg_v01));

  get_socksv5_proxy_config_req_msg_v01.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
         QMI_QCMAP_MSGR_GET_SOCKSV5_PROXY_CONFIG_REQ_V01,
         &get_socksv5_proxy_config_req_msg_v01,
         sizeof(get_socksv5_proxy_config_req_msg_v01),
         (void*)&get_socksv5_proxy_config_resp_msg_v01,
         sizeof(get_socksv5_proxy_config_resp_msg_v01),
         QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(GetSOCKSv5Config): error %d result %d",
      qmi_error, get_socksv5_proxy_config_resp_msg_v01.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_socksv5_proxy_config_resp_msg_v01.resp.result != QMI_NO_ERR ) ||
       ( NULL == configuration ) )
  {
    LOG_MSG_ERROR("Can not get SOCKSv5 Proxy Config %d : %d",
                  qmi_error, get_socksv5_proxy_config_resp_msg_v01.resp.error, 0);
    *qmi_err_num = get_socksv5_proxy_config_resp_msg_v01.resp.error;
    return false;
  } else
  {
    //get conf file path
    if(sizeof(configuration->config_file_paths.conf_file) <=
       strlen(get_socksv5_proxy_config_resp_msg_v01.config_file_paths.conf_file))
    {
      LOG_MSG_INFO1("Preventing buffer overflow %d <= %d\n",
                    sizeof(configuration->config_file_paths.conf_file),
                    strlen(get_socksv5_proxy_config_resp_msg_v01.config_file_paths.conf_file), 0);
      return false;
    }
    memcpy(configuration->config_file_paths.conf_file,
           get_socksv5_proxy_config_resp_msg_v01.config_file_paths.conf_file,
           strlen(get_socksv5_proxy_config_resp_msg_v01.config_file_paths.conf_file));

    //get auth file path
    if(sizeof(configuration->config_file_paths.auth_file) <=
       strlen(get_socksv5_proxy_config_resp_msg_v01.config_file_paths.auth_file))
    {
      LOG_MSG_INFO1("Preventing buffer overflow %d <= %d\n",
                    sizeof(configuration->config_file_paths.auth_file),
                    strlen(get_socksv5_proxy_config_resp_msg_v01.config_file_paths.auth_file), 0);
      return false;
    }
    memcpy(configuration->config_file_paths.auth_file,
           get_socksv5_proxy_config_resp_msg_v01.config_file_paths.auth_file,
           strlen(get_socksv5_proxy_config_resp_msg_v01.config_file_paths.auth_file));

    //get auth method
    configuration->auth_method = get_socksv5_proxy_config_resp_msg_v01.auth_method;

    //get lan iface
    if(sizeof(configuration->lan_iface) <= strlen(get_socksv5_proxy_config_resp_msg_v01.lan_iface))
    {
      LOG_MSG_INFO1("Preventing buffer overflow %d <= %d\n",
                    sizeof(configuration->lan_iface),
                    strlen(get_socksv5_proxy_config_resp_msg_v01.lan_iface), 0);
      return false;
    }
    memcpy(configuration->lan_iface, get_socksv5_proxy_config_resp_msg_v01.lan_iface,
           strlen(get_socksv5_proxy_config_resp_msg_v01.lan_iface));

    //get wan_ifaces with corresponding profile/service no
    for(int i = 0; i < QCMAP_MAX_NUM_BACKHAULS_V01; i++)
    {
      memcpy(&configuration->wan_service[i],
             &get_socksv5_proxy_config_resp_msg_v01.socksv5_wan_service[i],
             sizeof(qcmap_msgr_socksv5_wan_config_v01));
    }
  }

  return true;
}

/*===========================================================================
  FUNCTION SetSOCKSv5Config
  ===========================================================================*/
/*!
  @brief
  Sets SOCKSv5 Proxy configuration

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::SetSOCKSv5Config
(
void *config,
qcmap_socksv5_config_type_v01 config_type,
qmi_error_type_v01 *qmi_err_num
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_set_socksv5_proxy_config_req_msg_v01 set_socksv5_proxy_config_req_msg_v01;
  qcmap_msgr_set_socksv5_proxy_config_resp_msg_v01 set_socksv5_proxy_config_resp_msg_v01;

  QCMAP_LOG_FUNC_ENTRY();

  if(NULL == config)
  {
    LOG_MSG_INFO1("Failed to SetSOCKSv5Config: config ptr NULL\n", 0, 0, 0);
    return false;
  }

  memset(&set_socksv5_proxy_config_req_msg_v01, 0, sizeof(set_socksv5_proxy_config_req_msg_v01));
  memset(&set_socksv5_proxy_config_resp_msg_v01, 0, sizeof(set_socksv5_proxy_config_resp_msg_v01));

  set_socksv5_proxy_config_req_msg_v01.mobile_ap_handle = this->mobile_ap_handle;
  set_socksv5_proxy_config_req_msg_v01.config_type = config_type;

  switch(config_type)
  {
    case QCMAP_MSGR_SOCKSV5_SET_CONFIG_FILE_PATH_V01:
    {
      qcmap_msgr_socksv5_config_file_paths_v01 *config_file_paths =
                                                  (qcmap_msgr_socksv5_config_file_paths_v01*)config;

      if(sizeof(set_socksv5_proxy_config_req_msg_v01.config_file_paths.conf_file) <=
         strlen(config_file_paths->conf_file))
      {
        LOG_MSG_INFO1("Preventing buffer overflow %d <= %d\n",
                      sizeof(set_socksv5_proxy_config_req_msg_v01.config_file_paths.conf_file),
                      strlen(config_file_paths->conf_file), 0);
        return false;
      }
      memcpy(set_socksv5_proxy_config_req_msg_v01.config_file_paths.conf_file,
              config_file_paths->conf_file, strlen(config_file_paths->conf_file));

      if(sizeof(set_socksv5_proxy_config_req_msg_v01.config_file_paths.auth_file) <=
         strlen(config_file_paths->auth_file))
      {
        LOG_MSG_INFO1("Preventing buffer overflow %d <= %d\n",
                      sizeof(set_socksv5_proxy_config_req_msg_v01.config_file_paths.auth_file),
                      strlen(config_file_paths->auth_file), 0);
        return false;
      }
      memcpy(set_socksv5_proxy_config_req_msg_v01.config_file_paths.auth_file,
              config_file_paths->auth_file, strlen(config_file_paths->auth_file));

      set_socksv5_proxy_config_req_msg_v01.config_file_paths_valid = 1;

      break;
    }
    case QCMAP_MSGR_SOCKSV5_SET_AUTH_METHOD_V01:
    {
      unsigned char *auth_method = (unsigned char*)config;
      set_socksv5_proxy_config_req_msg_v01.auth_method = *auth_method;
      set_socksv5_proxy_config_req_msg_v01.auth_method_valid = 1;
      break;
    }
    case QCMAP_MSGR_SOCKSV5_EDIT_LAN_IFACE_V01:
    {
      char *lan_iface = (char*)config;

      if(sizeof(set_socksv5_proxy_config_req_msg_v01.lan_iface) <= strlen(lan_iface))
      {
        LOG_MSG_INFO1("Preventing buffer overflow %d <= %d\n",
                      sizeof(set_socksv5_proxy_config_req_msg_v01.lan_iface),
                      strlen(lan_iface), 0);
        return false;
      }
      memcpy(set_socksv5_proxy_config_req_msg_v01.lan_iface, lan_iface, strlen(lan_iface));
      set_socksv5_proxy_config_req_msg_v01.lan_iface_valid = 1;
      break;
    }
    case QCMAP_MSGR_SOCKSV5_ADD_UNAME_ASSOC_V01:
    {
      qcmap_msgr_socksv5_uname_assoc_v01 *uname_assoc = (qcmap_msgr_socksv5_uname_assoc_v01*)config;

      if(sizeof(set_socksv5_proxy_config_req_msg_v01.uname_assoc.uname) <=
         strlen(uname_assoc->uname))
      {
        LOG_MSG_INFO1("Preventing buffer overflow %d <= %d\n",
                      sizeof(set_socksv5_proxy_config_req_msg_v01.uname_assoc.uname),
                      strlen(uname_assoc->uname), 0);
        return false;
      }
      memcpy(set_socksv5_proxy_config_req_msg_v01.uname_assoc.uname, uname_assoc->uname,
             strlen(uname_assoc->uname));
      set_socksv5_proxy_config_req_msg_v01.uname_assoc.service_no = uname_assoc->service_no;
      set_socksv5_proxy_config_req_msg_v01.uname_assoc_valid = 1;
      break;
    }
    case QCMAP_MSGR_SOCKSV5_DELETE_UNAME_ASSOC_V01:
    {
      qcmap_msgr_socksv5_uname_assoc_v01 *uname_assoc = (qcmap_msgr_socksv5_uname_assoc_v01*)config;
      if(sizeof(set_socksv5_proxy_config_req_msg_v01.uname_assoc.uname) <=
         strlen(uname_assoc->uname))
      {
        LOG_MSG_INFO1("Preventing buffer overflow %d <= %d\n",
                      sizeof(set_socksv5_proxy_config_req_msg_v01.uname_assoc.uname),
                      strlen(uname_assoc->uname), 0);
        return false;
      }
      memcpy(set_socksv5_proxy_config_req_msg_v01.uname_assoc.uname, uname_assoc->uname,
             strlen(uname_assoc->uname));
      set_socksv5_proxy_config_req_msg_v01.uname_assoc_valid = 1;
      break;
    }
    default:
    {
      LOG_MSG_INFO1("Invalid SetSOCKSv5Config type %d", config_type, 0, 0);
      return false;
      break;
    }
  }

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
               QMI_QCMAP_MSGR_SET_SOCKSV5_PROXY_CONFIG_REQ_V01,
               &set_socksv5_proxy_config_req_msg_v01,
               sizeof(set_socksv5_proxy_config_req_msg_v01),
               (void*)&set_socksv5_proxy_config_resp_msg_v01,
               sizeof(set_socksv5_proxy_config_resp_msg_v01),
               QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(SetSOCKSv5ProxyConfig): error %d result %d",
      qmi_error, set_socksv5_proxy_config_resp_msg_v01.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( set_socksv5_proxy_config_resp_msg_v01.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not Set SOCKSv5 Config %d : %d",
    qmi_error, set_socksv5_proxy_config_resp_msg_v01.resp.error,0);
    *qmi_err_num = set_socksv5_proxy_config_resp_msg_v01.resp.error;
    return false;
  }
  return true;
}


/*===========================================================================
  FUNCTION SetVLANConfig
  ===========================================================================*/
/*!
  @brief
  Sets VLAN Config

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::SetVLANConfig
(
  qcmap_msgr_vlan_config_v01 vlan_config,
  qmi_error_type_v01 *qmi_err_num,
  bool *is_ipa_offloaded
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_set_vlan_config_req_msg_v01 vlan_config_req;
  qcmap_msgr_set_vlan_config_resp_msg_v01 vlan_config_resp;

  if (!qmi_err_num)
  {
    LOG_MSG_ERROR("Null parameter passed ",0,0,0);
    return false;
  }
  if (!is_ipa_offloaded)
  {
    LOG_MSG_ERROR("Invalid parameter passed ",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  QCMAP_LOG_FUNC_ENTRY();

  memset(&vlan_config_req, 0, sizeof(qcmap_msgr_set_vlan_config_req_msg_v01));
  memset(&vlan_config_resp, 0, sizeof(qcmap_msgr_set_vlan_config_resp_msg_v01));

  vlan_config_req.mobile_ap_handle = this->mobile_ap_handle;
  memcpy(&vlan_config_req.config, &vlan_config, sizeof(qcmap_msgr_vlan_config_v01));

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                           QMI_QCMAP_MSGR_SET_VLAN_CONFIG_REQ_V01,
                           &vlan_config_req,
                           sizeof(qcmap_msgr_set_vlan_config_req_msg_v01),
                           &vlan_config_resp,
                          sizeof(qcmap_msgr_set_vlan_config_resp_msg_v01),
                           QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(SetVLANConfig):"
                " error %d result %d",
      qmi_error, vlan_config_resp.resp.result, 0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( vlan_config_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Cannot set VLAN Config %d : %d",
        qmi_error, vlan_config_resp.resp.error, 0);
    *qmi_err_num = vlan_config_resp.resp.error;
    return false;
  }

  *qmi_err_num = vlan_config_resp.resp.error;
  if (vlan_config_resp.is_ipa_offload_enabled_valid == TRUE)
  {
    *is_ipa_offloaded = vlan_config_resp.is_ipa_offload_enabled;
  }

  return true;
}
/*===========================================================================
  FUNCTION GetVLANConfig
  ===========================================================================*/
/*!
  @brief
  Gets VLAN Configuration

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::GetVLANConfig
(
  qcmap_msgr_vlan_conf_t *vlan_config,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_get_vlan_config_req_msg_v01 get_vlan_config_req_msg;
  qcmap_msgr_get_vlan_config_resp_msg_v01 get_vlan_config_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  /*Check for valid pointers*/
  if ( vlan_config == NULL)
  {
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    LOG_MSG_ERROR("Get VLAN Config Failed: %d ",*qmi_err_num ,0,0);
    return false;
  }
  memset(&get_vlan_config_req_msg,0,\
         sizeof(qcmap_msgr_get_vlan_config_req_msg_v01));
  memset(&get_vlan_config_resp_msg,0,\
         sizeof(qcmap_msgr_get_vlan_config_resp_msg_v01));

  get_vlan_config_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  qmi_error = qmi_client_send_msg_sync(
                            this->qmi_qcmap_msgr_handle,
                            QMI_QCMAP_MSGR_GET_VLAN_CONFIG_REQ_V01,
                            &get_vlan_config_req_msg,
                            sizeof(qcmap_msgr_get_vlan_config_req_msg_v01),
                            &get_vlan_config_resp_msg,
                            sizeof(qcmap_msgr_get_vlan_config_resp_msg_v01),
                            QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_vlan_config_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get VLAN config %d : %d",
        qmi_error, get_vlan_config_resp_msg.resp.error,0);
    *qmi_err_num = get_vlan_config_resp_msg.resp.error;
    return false;
  }
  if (get_vlan_config_resp_msg.vlan_config_list_valid == true)
  {

    vlan_config->vlan_config_list_len = get_vlan_config_resp_msg.
                                                   vlan_config_list_len;

    LOG_MSG_INFO1("\nNum VLAN Configs: %d",vlan_config->vlan_config_list_len,\
                  0,0);
    if (vlan_config->vlan_config_list_len <=\
         QCMAP_MSGR_MAX_VLAN_ENTRIES_V01)
    {
      memcpy(vlan_config->vlan_config_list,
             get_vlan_config_resp_msg.vlan_config_list,\
             vlan_config->vlan_config_list_len *\
                    sizeof(qcmap_msgr_vlan_config_v01));
    }
  }
  else
  {
    LOG_MSG_INFO1("\n Get VLAN Config failed!!",0,0,0);
    return false;
  }
  LOG_MSG_INFO1("Exiting  GetVLANConfig...",0,0,0);
  return true;
}
/*===========================================================================
  FUNCTION DeleteVLANConfig
  ===========================================================================*/
/*!
  @brief
  Deletes VLAN Config

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::DeleteVLANConfig
(
   qcmap_msgr_vlan_config_v01 vlan_config,
   qmi_error_type_v01 *qmi_err_num
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_delete_vlan_config_req_msg_v01 vlan_config_req;
  qcmap_msgr_delete_vlan_config_resp_msg_v01 vlan_config_resp;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&vlan_config_req, 0, sizeof(qcmap_msgr_delete_vlan_config_req_msg_v01));
  memset(&vlan_config_resp, 0, sizeof(qcmap_msgr_delete_vlan_config_resp_msg_v01));

  vlan_config_req.mobile_ap_handle = this->mobile_ap_handle;
  memcpy(&vlan_config_req.config, &vlan_config, sizeof(qcmap_msgr_vlan_config_v01));

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                           QMI_QCMAP_MSGR_DELETE_VLAN_CONFIG_REQ_V01,
                           &vlan_config_req,
                           sizeof(qcmap_msgr_delete_vlan_config_req_msg_v01),
                           &vlan_config_resp,
                          sizeof(qcmap_msgr_delete_vlan_config_resp_msg_v01),
                           QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(DeleteVLANConfig):"
                " error %d result %d",
      qmi_error, vlan_config_resp.resp.result, 0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( vlan_config_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Cannot Delete VLAN Config %d : %d",
        qmi_error, vlan_config_resp.resp.error, 0);
    *qmi_err_num = vlan_config_resp.resp.error;
    return false;
  }

  return true;
}
/*===========================================================================
  FUNCTION SetUnmanagedL2TPState
  ===========================================================================*/
/*!
  @brief
  Set to enable/disable L2TP config for unmanaged tunnels.

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::SetUnmanagedL2TPState
(
  qcmap_msgr_set_unmanaged_l2tp_state_config_v01 l2tp_enable_config,
  qcmap_msgr_l2tp_mtu_config_v01     MTU_config,
  qcmap_msgr_l2tp_TCP_MSS_config_v01 TCP_MSS_config,
  qmi_error_type_v01 *qmi_err_num
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_set_unmanaged_l2tp_state_req_msg_v01 l2tp_enable_config_req;
  qcmap_msgr_set_unmanaged_l2tp_state_resp_msg_v01 l2tp_enable_config_resp;
  qcmap_msgr_set_MTU_for_l2tp_config_req_msg_v01 l2tp_mtu_config_req;
  qcmap_msgr_set_MTU_for_l2tp_config_resp_msg_v01 l2tp_mtu_config_resp;
  qcmap_msgr_set_TCP_MSS_for_l2tp_config_req_msg_v01 l2tp_mss_config_req;
  qcmap_msgr_set_TCP_MSS_for_l2tp_config_resp_msg_v01 l2tp_mss_config_resp;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&l2tp_enable_config_req, 0,
                       sizeof(qcmap_msgr_set_unmanaged_l2tp_state_req_msg_v01));
  memset(&l2tp_enable_config_resp, 0,
                      sizeof(qcmap_msgr_set_unmanaged_l2tp_state_resp_msg_v01));

  l2tp_enable_config_req.mobile_ap_handle = this->mobile_ap_handle;
  memcpy(&l2tp_enable_config_req.config,&l2tp_enable_config,
                       sizeof(qcmap_msgr_set_unmanaged_l2tp_state_config_v01));

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                           QMI_QCMAP_MSGR_SET_UNMANAGED_L2TP_STATE_REQ_V01,
                           &l2tp_enable_config_req,
                        sizeof(qcmap_msgr_set_unmanaged_l2tp_state_req_msg_v01),
                           &l2tp_enable_config_resp,
                       sizeof(qcmap_msgr_set_unmanaged_l2tp_state_resp_msg_v01),
                           QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(SetL2TPState):"
                " error %d result %d",
      qmi_error, l2tp_enable_config_resp.resp.result, 0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( l2tp_enable_config_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("CannotSet L2TP State %d : %d",
        qmi_error, l2tp_enable_config_resp.resp.error, 0);
    *qmi_err_num = l2tp_enable_config_resp.resp.error;
    return false;
  }

  if (TCP_MSS_config.enable)
  {
    memset(&l2tp_mss_config_req,0,
           sizeof(qcmap_msgr_set_TCP_MSS_for_l2tp_config_req_msg_v01));
    memset(&l2tp_mss_config_resp, 0,
                 sizeof(qcmap_msgr_set_TCP_MSS_for_l2tp_config_resp_msg_v01));

    l2tp_mss_config_req.mobile_ap_handle = this->mobile_ap_handle;
    l2tp_mss_config_req.config.enable = TCP_MSS_config.enable;

    qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                       QMI_QCMAP_MSGR_SET_TCP_MSS_FOR_L2TP_CONFIG_REQ_V01,
                       &l2tp_mss_config_req,
                       sizeof(qcmap_msgr_set_TCP_MSS_for_l2tp_config_req_msg_v01),
                       &l2tp_mss_config_resp,
                       sizeof(qcmap_msgr_set_TCP_MSS_for_l2tp_config_resp_msg_v01),
                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

    LOG_MSG_INFO1("qmi_client_send_msg_sync(SetTCPMSSforL2TPConfig):"
                  " error %d result %d",
        qmi_error, l2tp_mss_config_resp.resp.result, 0);

    if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
         ( qmi_error != QMI_NO_ERR ) ||
         ( l2tp_mss_config_resp.resp.result != QMI_NO_ERR ) )
{
      LOG_MSG_ERROR("Cannot set L2TP TCP MSSConfig %d : %d",
          qmi_error, l2tp_mss_config_resp.resp.error, 0);
    }

  }
  if (MTU_config.enable)
  {
    memset(&l2tp_mtu_config_req,0,
           sizeof(qcmap_msgr_set_MTU_for_l2tp_config_req_msg_v01));
    memset(&l2tp_mtu_config_resp, 0,
                     sizeof(qcmap_msgr_set_MTU_for_l2tp_config_resp_msg_v01));

    l2tp_mtu_config_req.mobile_ap_handle = this->mobile_ap_handle;
    l2tp_mtu_config_req.config.enable = MTU_config.enable;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                       QMI_QCMAP_MSGR_SET_MTU_FOR_L2TP_CONFIG_REQ_V01,
                       &l2tp_mtu_config_req,
                       sizeof(qcmap_msgr_set_MTU_for_l2tp_config_req_msg_v01),
                       &l2tp_mtu_config_resp,
                       sizeof(qcmap_msgr_set_MTU_for_l2tp_config_resp_msg_v01),
                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

    LOG_MSG_INFO1("qmi_client_send_msg_sync(SetMTUforL2TPConfig):"
                " error %d result %d",
        qmi_error, l2tp_mtu_config_resp.resp.result, 0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( l2tp_mtu_config_resp.resp.result != QMI_NO_ERR ) )
  {
      LOG_MSG_ERROR("Cannot set L2TP MTU Config %d : %d",
          qmi_error, l2tp_mtu_config_resp.resp.error, 0);
    }
  }

  return true;
}

/*===========================================================================
  FUNCTION SetL2TPConfig
  ===========================================================================*/
/*!
  @brief
  Sets L2TP Tunnel Config and also sets MTU size option. TCP MSS option for all
  L2TP tunnels

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::SetL2TPConfig
(
  qcmap_msgr_l2tp_mode_enum_v01 mode,
  qcmap_msgr_l2tp_config_v01 l2tp_config,
  qmi_error_type_v01 *qmi_err_num
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_set_l2tp_config_req_msg_v01 l2tp_config_req;
  qcmap_msgr_set_l2tp_config_resp_msg_v01 l2tp_config_resp;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&l2tp_config_req, 0,
                   sizeof(qcmap_msgr_set_l2tp_config_req_msg_v01));
  memset(&l2tp_config_resp, 0,
                     sizeof(qcmap_msgr_set_l2tp_config_resp_msg_v01));

  l2tp_config_req.mobile_ap_handle = this->mobile_ap_handle;
  l2tp_config_req.mode = mode;
  memcpy(&l2tp_config_req.config,&l2tp_config,
                           sizeof(qcmap_msgr_l2tp_config_v01));

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                       QMI_QCMAP_MSGR_SET_L2TP_CONFIG_REQ_V01,
                   &l2tp_config_req,
                       sizeof(qcmap_msgr_set_l2tp_config_req_msg_v01),
                   &l2tp_config_resp,
                       sizeof(qcmap_msgr_set_l2tp_config_resp_msg_v01),
                   QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(SetL2TPConfig):"
                " error %d result %d",
      qmi_error, l2tp_config_resp.resp.result, 0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( l2tp_config_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Cannot set L2TP Config %d : %d",
        qmi_error, l2tp_config_resp.resp.error, 0);
    *qmi_err_num = l2tp_config_resp.resp.error;
    return false;
  }

  return true;
}
/*===========================================================================
  FUNCTION GetL2TPConfig
  ===========================================================================*/
/*!
  @brief
  Gets L2TP Configuration

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::GetL2TPConfig
(
  qcmap_msgr_l2tp_conf_t *l2tp_config,
  qmi_error_type_v01 *qmi_err_num
 )
{
  qcmap_msgr_get_l2tp_config_req_msg_v01 get_l2tp_config_req_msg;
  qcmap_msgr_get_l2tp_config_resp_msg_v01 get_l2tp_config_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  /*Check for valid pointers*/
  if ( l2tp_config == NULL)
  {
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    LOG_MSG_ERROR("Get L2TP Config Failed: %d ",*qmi_err_num ,0,0);
    return false;
  }
  memset(&get_l2tp_config_req_msg,0,\
         sizeof(qcmap_msgr_get_l2tp_config_req_msg_v01));
  memset(&get_l2tp_config_resp_msg,0,\
         sizeof(qcmap_msgr_get_l2tp_config_resp_msg_v01));

  qmi_error = qmi_client_send_msg_sync(
                            this->qmi_qcmap_msgr_handle,
                            QMI_QCMAP_MSGR_GET_L2TP_CONFIG_REQ_V01,
                            &get_l2tp_config_req_msg,
                            sizeof(qcmap_msgr_get_l2tp_config_req_msg_v01),
                            &get_l2tp_config_resp_msg,
                            sizeof(qcmap_msgr_get_l2tp_config_resp_msg_v01),
                            QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_l2tp_config_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get L2TP config %d : %d",
        qmi_error, get_l2tp_config_resp_msg.resp.error,0);
    *qmi_err_num = get_l2tp_config_resp_msg.resp.error;
    return false;
  }

  l2tp_config->mode = get_l2tp_config_resp_msg.mode;
  l2tp_config->l2tp_mtu_config.enable =
                            get_l2tp_config_resp_msg.mtu_config.enable;
  l2tp_config->l2tp_mss_config.enable =
                            get_l2tp_config_resp_msg.tcp_mss_config.enable;
  if (get_l2tp_config_resp_msg.l2tp_config_list_valid == true)
  {

    l2tp_config->l2tp_config_list_len =
                  get_l2tp_config_resp_msg.l2tp_config_list_len;

    LOG_MSG_INFO1("\nNum L2TP Configs: %d",l2tp_config->l2tp_config_list_len,\
                  0,0);
    if (l2tp_config->l2tp_config_list_len <=\
         QCMAP_MSGR_L2TP_MAX_TUNNELS_V01)
    {
      memcpy(&l2tp_config->l2tp_config_list,
             get_l2tp_config_resp_msg.l2tp_config_list,\
             l2tp_config->l2tp_config_list_len *\
                    sizeof(qcmap_msgr_l2tp_config_v01));
    }
  }
  else
  {
    LOG_MSG_INFO1("\n Get L2TP Config failed!!",0,0,0);
    return false;
  }
  LOG_MSG_INFO1("Exiting  GetL2TPConfig...",0,0,0);
  return true;
}

/*===========================================================================
  FUNCTION DeleteL2TPTunnelConfig
  ===========================================================================*/
/*!
  @brief
  Deletes L2TP Tunnel Config

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::DeleteL2TPTunnelConfig
(
  qcmap_msgr_delete_l2tp_config_v01 l2tp_config,
  qmi_error_type_v01 *qmi_err_num
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_delete_l2tp_tunnel_config_req_msg_v01 l2tp_config_req;
  qcmap_msgr_delete_l2tp_tunnel_config_resp_msg_v01 l2tp_config_resp;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&l2tp_config_req, 0, sizeof(qcmap_msgr_delete_l2tp_tunnel_config_req_msg_v01));
  memset(&l2tp_config_resp, 0, sizeof(qcmap_msgr_delete_l2tp_tunnel_config_resp_msg_v01));

  l2tp_config_req.mobile_ap_handle = this->mobile_ap_handle;
  memcpy(&l2tp_config_req.config,&l2tp_config,
                           sizeof(qcmap_msgr_delete_l2tp_config_v01));

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                       QMI_QCMAP_MSGR_DELETE_L2TP_TUNNEL_CONFIG_REQ_V01,
                       &l2tp_config_req,
                       sizeof(qcmap_msgr_delete_l2tp_tunnel_config_req_msg_v01),
                       &l2tp_config_resp,
                      sizeof(qcmap_msgr_delete_l2tp_tunnel_config_resp_msg_v01),
                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(DeleteL2TPConfig):"
                " error %d result %d",
      qmi_error, l2tp_config_resp.resp.result, 0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( l2tp_config_resp.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Cannot Delete L2TP Config %d : %d",
        qmi_error, l2tp_config_resp.resp.error, 0);
    *qmi_err_num = l2tp_config_resp.resp.error;
    return false;
  }

  return true;
}

/*===========================================================================
  FUNCTION AddPDNToVLANMapping
  ===========================================================================*/
/*!
  @brief
  Adds a PDN to VLAN mapping pair

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::AddPDNToVLANMapping(int16_t vlan_id, profile_handle_type_v01 profile_handle,
                                          qmi_error_type_v01 *qmi_err_num)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_add_pdn_to_vlan_mapping_req_msg_v01 add_pdn_to_vlan_mapping_req_msg_v01;
  qcmap_msgr_add_pdn_to_vlan_mapping_resp_msg_v01 add_pdn_to_vlan_mapping_resp_msg_v01;

  if(NULL == qmi_err_num)
  {
    LOG_MSG_INFO1("Given NULL arg", 0, 0, 0);
    return false;
  }

  QCMAP_LOG_FUNC_ENTRY();

  memset(&add_pdn_to_vlan_mapping_req_msg_v01, 0, sizeof(add_pdn_to_vlan_mapping_req_msg_v01));
  memset(&add_pdn_to_vlan_mapping_resp_msg_v01, 0, sizeof(add_pdn_to_vlan_mapping_resp_msg_v01));

  add_pdn_to_vlan_mapping_req_msg_v01.mobile_ap_handle = this->mobile_ap_handle;
  add_pdn_to_vlan_mapping_req_msg_v01.profile_handle = profile_handle;
  add_pdn_to_vlan_mapping_req_msg_v01.vlan_id = vlan_id;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
               QMI_QCMAP_MSGR_ADD_PDN_TO_VLAN_MAPPING_REQ_V01,
               &add_pdn_to_vlan_mapping_req_msg_v01,
               sizeof(add_pdn_to_vlan_mapping_req_msg_v01),
               (void*)&add_pdn_to_vlan_mapping_resp_msg_v01,
               sizeof(add_pdn_to_vlan_mapping_resp_msg_v01),
               QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(%s): error %d result %d", __func__,
      qmi_error, add_pdn_to_vlan_mapping_resp_msg_v01.resp.result);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( add_pdn_to_vlan_mapping_resp_msg_v01.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not Add PDN To VLAN Mapping %d : %d",
    qmi_error, add_pdn_to_vlan_mapping_resp_msg_v01.resp.error,0);
    *qmi_err_num = add_pdn_to_vlan_mapping_resp_msg_v01.resp.error;
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION DeletePDNToVLANMapping
  ===========================================================================*/
/*!
  @brief
  Deletes a PDN to VLAN mapping pair

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::DeletePDNToVLANMapping
(
int16_t vlan_id,
profile_handle_type_v01 profile_handle,
qmi_error_type_v01 *qmi_err_num
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_delete_pdn_to_vlan_mapping_req_msg_v01 delete_pdn_to_vlan_mapping_req_msg_v01;
  qcmap_msgr_delete_pdn_to_vlan_mapping_resp_msg_v01 delete_pdn_to_vlan_mapping_resp_msg_v01;

  if(NULL == qmi_err_num)
  {
    LOG_MSG_INFO1("Given NULL arg", 0, 0, 0);
    return false;
  }

  QCMAP_LOG_FUNC_ENTRY();

  memset(&delete_pdn_to_vlan_mapping_req_msg_v01, 0,
         sizeof(delete_pdn_to_vlan_mapping_req_msg_v01));
  memset(&delete_pdn_to_vlan_mapping_resp_msg_v01, 0,
         sizeof(delete_pdn_to_vlan_mapping_resp_msg_v01));

  delete_pdn_to_vlan_mapping_req_msg_v01.mobile_ap_handle = this->mobile_ap_handle;
  delete_pdn_to_vlan_mapping_req_msg_v01.profile_handle = profile_handle;
  delete_pdn_to_vlan_mapping_req_msg_v01.vlan_id = vlan_id;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
               QMI_QCMAP_MSGR_DELETE_PDN_TO_VLAN_MAPPING_REQ_V01,
               &delete_pdn_to_vlan_mapping_req_msg_v01,
               sizeof(delete_pdn_to_vlan_mapping_req_msg_v01),
               (void*)&delete_pdn_to_vlan_mapping_resp_msg_v01,
               sizeof(delete_pdn_to_vlan_mapping_resp_msg_v01),
               QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(%s): error %d result %d", __func__,
      qmi_error, delete_pdn_to_vlan_mapping_resp_msg_v01.resp.result);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( delete_pdn_to_vlan_mapping_resp_msg_v01.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not Delete PDN To VLAN Mapping %d : %d",
    qmi_error, delete_pdn_to_vlan_mapping_resp_msg_v01.resp.error,0);
    *qmi_err_num = delete_pdn_to_vlan_mapping_resp_msg_v01.resp.error;
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION GetPDNtoVLANMappings
  ===========================================================================*/
/*!
  @brief
  Retrieves all the PDN to VLAN mapping pairs

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean QCMAP_Client::GetPDNtoVLANMappings
(
  qcmap_msgr_pdn_to_vlan_mapping_v01 *pdn_vlan_mappings,
  int *num_entries,
  qmi_error_type_v01 *qmi_err_num
)
{
  qmi_client_error_type qmi_error = QMI_NO_ERR;
  qcmap_msgr_get_pdn_to_vlan_mappings_resp_msg_v01 resp_msg;

  QCMAP_LOG_FUNC_ENTRY();

  if(!pdn_vlan_mappings || !num_entries)
  {
    LOG_MSG_ERROR("Invalid parameter passed ",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  memset(&resp_msg, 0, sizeof(qcmap_msgr_get_pdn_to_vlan_mappings_resp_msg_v01));

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
               QMI_QCMAP_MSGR_GET_PDN_TO_VLAN_MAPPINGS_REQ_V01,
               NULL,
               0,
               (void*)&resp_msg,
               sizeof(qcmap_msgr_get_pdn_to_vlan_mappings_resp_msg_v01),
               QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(GetPDNtoVLANMapping): error %d result %d",
                qmi_error, resp_msg.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not Get PDN to VLAN mappings %d : %d",
    qmi_error, resp_msg.resp.error,0);
    *qmi_err_num = resp_msg.resp.error;
    return false;
  }

  if (!resp_msg.pdn_to_vlan_mappings_valid)
  {
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    LOG_MSG_INFO1("No valid mappings",0,0,0);
    return false;
  }

  *num_entries = resp_msg.pdn_to_vlan_mappings_len;

  if (*num_entries > QCMAP_MAX_NUM_BACKHAULS_V01 || *num_entries == 0)
  {
    *qmi_err_num = resp_msg.resp.error;
    LOG_MSG_INFO1("Too many entries or none: %d", *num_entries,0,0);
    return false;
  }

  memcpy(pdn_vlan_mappings, &resp_msg.pdn_to_vlan_mappings, *num_entries * sizeof(qcmap_msgr_pdn_to_vlan_mapping_v01));
  return true;
}

/*===========================================================================
FUNCTION EnableGSB()
===========================================================================*/
/** @ingroup qcmap_enable_gsb

  Enables the GSB. This function loads generic software bridge kernel module
  and sends the configuration to GSB

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
QCMAP_Client::EnableGSB
(
  qmi_error_type_v01 *qmi_err_num
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_enable_gsb_req_msg_v01 enable_gsb_req_msg_v01;
  qcmap_msgr_enable_gsb_resp_msg_v01 enable_gsb_resp_msg_v01;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&enable_gsb_req_msg_v01, 0, sizeof(qcmap_msgr_enable_gsb_req_msg_v01));
  memset(&enable_gsb_resp_msg_v01, 0, sizeof(qcmap_msgr_enable_gsb_resp_msg_v01));

  enable_gsb_req_msg_v01.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_ENABLE_GSB_REQ_V01,
                                       &enable_gsb_req_msg_v01,
                                       sizeof(qcmap_msgr_enable_gsb_req_msg_v01),
                                       (void*)&enable_gsb_resp_msg_v01,
                                       sizeof(qcmap_msgr_enable_gsb_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(enable): error %d result %d",
      qmi_error, enable_gsb_resp_msg_v01.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( enable_gsb_resp_msg_v01.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not enable gsb %d : %d",
    qmi_error, enable_gsb_resp_msg_v01.resp.error,0);
    *qmi_err_num = enable_gsb_resp_msg_v01.resp.error;
    return false;
  }
  return true;
}

/*===========================================================================
FUNCTION DisableGSB()
===========================================================================*/
/** @ingroup qcmap_disable_gsb

  Disables the GSB. This function unloads generic software bridge kernel module.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
QCMAP_Client::DisableGSB
(
  qmi_error_type_v01 *qmi_err_num
)
{
  qmi_client_error_type qmi_error, qmi_err_code = QMI_NO_ERR;
  qcmap_msgr_disable_gsb_req_msg_v01  disable_gsb_req_msg_v01;
  qcmap_msgr_disable_gsb_resp_msg_v01 disable_gsb_resp_msg_v01;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&disable_gsb_req_msg_v01, 0, sizeof(qcmap_msgr_disable_gsb_req_msg_v01));
  memset(&disable_gsb_resp_msg_v01, 0, sizeof(qcmap_msgr_disable_gsb_resp_msg_v01));

  disable_gsb_req_msg_v01.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_DISABLE_GSB_REQ_V01,
                                       &disable_gsb_req_msg_v01,
                                       sizeof(qcmap_msgr_disable_gsb_req_msg_v01),
                                       (void*)&disable_gsb_resp_msg_v01,
                                       sizeof(qcmap_msgr_disable_gsb_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(enable): error %d result %d",
      qmi_error, disable_gsb_resp_msg_v01.resp.result,0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( disable_gsb_resp_msg_v01.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not disable gsb %d : %d",
    qmi_error, disable_gsb_resp_msg_v01.resp.error,0);
    *qmi_err_num = disable_gsb_resp_msg_v01.resp.error;
    return false;
  }
  return true;
}

/*===========================================================================
FUNCTION SetGSBConfig()
===========================================================================*/
/** @ingroup qcmap_set_gsb_config

  Sets GSB configuration.

  @datatypes
  qcmap_msgr_gsb_config_v01
  qmi_error_type_v01

  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
QCMAP_Client::SetGSBConfig
(
  qcmap_msgr_gsb_config_v01 *config,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_set_gsb_config_req_msg_v01 set_gsb_config_req_msg;
  qcmap_msgr_set_gsb_config_resp_msg_v01 set_gsb_config_resp_msg;
  qmi_client_error_type qmi_error;

  if (!config || (config->if_name == NULL))
  {
    LOG_MSG_ERROR("Invalid parameter passed ",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  /* validating input range */
  if (config->bw_reqd_in_mb > 900 || config->if_high_watermark > 600 ||
        config->if_low_watermark > 599 ||
          config->if_type < QCMAP_MSGR_INTERFACE_TYPE_ENUM_MIN_ENUM_VAL_V01 ||
            config->if_type > QCMAP_MSGR_INTERFACE_TYPE_ENUM_MAX_ENUM_VAL_V01)
  {
    LOG_MSG_ERROR("Invalid range detected",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  QCMAP_LOG_FUNC_ENTRY();

  memset(&set_gsb_config_req_msg, 0, sizeof(qcmap_msgr_set_gsb_config_req_msg_v01));
  memset(&set_gsb_config_resp_msg, 0, sizeof(qcmap_msgr_set_gsb_config_resp_msg_v01));


  set_gsb_config_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  set_gsb_config_req_msg.config_valid = TRUE;
  memcpy(&set_gsb_config_req_msg.config, config,sizeof(qcmap_msgr_gsb_config_v01));


  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_GSB_CONFIG_REQ_V01,
                                       &set_gsb_config_req_msg,
                                       sizeof(qcmap_msgr_set_gsb_config_req_msg_v01),
                                       &set_gsb_config_resp_msg,
                                       sizeof(qcmap_msgr_set_gsb_config_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR) ||
       ( set_gsb_config_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not set GSB config %d : %d",
        qmi_error, set_gsb_config_resp_msg.resp.error,0);
    *qmi_err_num = set_gsb_config_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("GSB Config Set succeeded", 0, 0, 0);
  return true;

}

/*===========================================================================
FUNCTION GetGSBConfig()
===========================================================================*/
/** @ingroup qcmap_get_gsb_config

  Gets GSB configuration.

  @datatypes
  qcmap_msgr_gsb_config_v01
  num_of_entries
  qmi_error_type_v01

  @param[out] qmi_err_num  Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
QCMAP_Client::GetGSBConfig
(
  qcmap_msgr_gsb_config_v01 *config,
  uint8 *num_of_entries,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_get_gsb_config_req_msg_v01 get_gsb_config_req_msg;
  qcmap_msgr_get_gsb_config_resp_msg_v01 get_gsb_config_resp_msg;
  qmi_client_error_type qmi_error;

  if (!config || !num_of_entries)
  {
    LOG_MSG_ERROR("Invalid parameter passed ",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  memset(&get_gsb_config_resp_msg, 0, sizeof(qcmap_msgr_get_gsb_config_resp_msg_v01));
  QCMAP_LOG_FUNC_ENTRY();

  get_gsb_config_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_GSB_CONFIG_REQ_V01,
                                       &get_gsb_config_req_msg,
                                       sizeof(qcmap_msgr_get_gsb_config_req_msg_v01),
                                       &get_gsb_config_resp_msg,
                                       sizeof(qcmap_msgr_get_gsb_config_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_gsb_config_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get GSB Config %d : %d",
                   qmi_error, get_gsb_config_resp_msg.resp.error, 0);
    *qmi_err_num = get_gsb_config_resp_msg.resp.error;
    return false;
  }

  if (get_gsb_config_resp_msg.config_valid && get_gsb_config_resp_msg.config_len > 0)
  {
    *num_of_entries = get_gsb_config_resp_msg.config_len;
    if (*num_of_entries > QCMAP_MSGR_MAX_IF_SUPPORTED_V01 || *num_of_entries == 0)
    {
          *qmi_err_num = get_gsb_config_resp_msg.resp.error;
          LOG_MSG_INFO1("Too many entries or none: %d", *num_of_entries,0,0);
          return false;
    }

    memcpy(config, &get_gsb_config_resp_msg.config,
            sizeof(qcmap_msgr_gsb_config_v01)*(*num_of_entries));
  }

  LOG_MSG_INFO1("Get GSB Config Succeeded", 0, 0, 0);

  return true;
}

/*===========================================================================
FUNCTION DeleteGSBConfig()
===========================================================================*/
/** @ingroup qcmap_delete_gsb_config

  Deletes configuration stored for GSB

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
QCMAP_Client::DeleteGSBConfig
(
  char* if_name,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_delete_gsb_config_resp_msg_v01 delete_gsb_config_resp_msg;
  qcmap_msgr_delete_gsb_config_req_msg_v01 delete_gsb_config_req_msg;

  qmi_client_error_type qmi_error;

  if (!if_name)
  {
    LOG_MSG_ERROR("Invalid parameter passed ",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }


  memset(&delete_gsb_config_resp_msg, 0, sizeof(qcmap_msgr_delete_gsb_config_resp_msg_v01));
  memset(&delete_gsb_config_req_msg, 0, sizeof(qcmap_msgr_delete_gsb_config_req_msg_v01));
  strlcpy(delete_gsb_config_req_msg.if_name, if_name, QCMAP_MAX_IFACE_NAME_SIZE_V01);


  delete_gsb_config_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  QCMAP_LOG_FUNC_ENTRY();
  LOG_MSG_INFO1("requesting to delete %s %s IFACE", delete_gsb_config_req_msg.if_name, if_name, 0);

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_DELETE_GSB_CONFIG_REQ_V01,
                                       &delete_gsb_config_req_msg,
                                       sizeof(qcmap_msgr_delete_gsb_config_req_msg_v01),
                                       &delete_gsb_config_resp_msg,
                                       sizeof(qcmap_msgr_delete_gsb_config_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( delete_gsb_config_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Cannot delete GSB Config %d : %d",
                   qmi_error, delete_gsb_config_resp_msg.resp.error, 0);
    *qmi_err_num = delete_gsb_config_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("Delete GSB Config Succeeded", 0, 0, 0);

  return true;
}

/*===========================================================================
  FUNCTION SetDunDongleMode
  ===========================================================================*/
/*!
  @brief
  Enable the Dun Dongle mode feature

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - QCMobileAP must be enabled.

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::SetDunDongleMode
(
  boolean dun_dongle_mode_state,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_set_dun_dongle_mode_req_msg_v01 set_dun_dongle_mode_req_msg;
  qcmap_msgr_set_dun_dongle_mode_resp_msg_v01 set_dun_dongle_mode_resp_msg;
  qmi_client_error_type qmi_error = QMI_NO_ERR;

  if (!qmi_err_num)
  {
    LOG_MSG_ERROR("Invalid parameter passed ",0,0,0);
    return false;
  }

  QCMAP_LOG_FUNC_ENTRY();

  memset(&set_dun_dongle_mode_resp_msg, 0, sizeof(set_dun_dongle_mode_resp_msg));
  set_dun_dongle_mode_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  set_dun_dongle_mode_req_msg.enable_dun_dongle_mode = dun_dongle_mode_state;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_DUN_DONGLE_MODE_REQ_V01,
                                       &set_dun_dongle_mode_req_msg,
                                       sizeof(qcmap_msgr_set_dun_dongle_mode_req_msg_v01),
                                       &set_dun_dongle_mode_resp_msg,
                                       sizeof(qcmap_msgr_set_dun_dongle_mode_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);
  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( set_dun_dongle_mode_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not set DUN Dongle Mode state %d : %d",
        qmi_error, set_dun_dongle_mode_resp_msg.resp.error,0);
    *qmi_err_num = set_dun_dongle_mode_resp_msg.resp.error;
    return false;
  }
  LOG_MSG_INFO1("DUN Dongle Mode state Set succeeded DUNDongleMode is:%d",
                dun_dongle_mode_state,0,0);
  return true;
}

/*===========================================================================
  FUNCTION GetDunDongleMode
  ===========================================================================*/
/*!
  @brief
  Display the Dun Dongle mode feature is enabled or not
  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::GetDunDongleMode
(
  boolean *dun_dongle_mode_status,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_get_dun_dongle_mode_resp_msg_v01 get_dun_dongle_mode_resp_msg;
  qmi_client_error_type qmi_error = QMI_NO_ERR;

  if (!qmi_err_num)
  {
    LOG_MSG_ERROR("Invalid parameter passed ",0,0,0);
    return false;
  }
  if (!dun_dongle_mode_status)
  {
    LOG_MSG_ERROR("Invalid parameter passed ",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  QCMAP_LOG_FUNC_ENTRY();
  memset(&get_dun_dongle_mode_resp_msg, 0, sizeof(get_dun_dongle_mode_resp_msg));

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_DUN_DONGLE_MODE_REQ_V01,
                                       NULL,
                                       0,
                                       &get_dun_dongle_mode_resp_msg,
                                       sizeof(qcmap_msgr_get_dun_dongle_mode_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_dun_dongle_mode_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get DUN Dongle Mode %d : %d",
                   qmi_error, get_dun_dongle_mode_resp_msg.resp.error,0);
    *qmi_err_num = get_dun_dongle_mode_resp_msg.resp.error;
    return false;
  }

  *dun_dongle_mode_status = get_dun_dongle_mode_resp_msg.dun_dongle_mode;
  return true;
}

/*===========================================================================
FUNCTION GetDataPathOptStatus()
===========================================================================*/
/**
  Use to know the status of data path optimization whether enabled/disabled.
  @datatypes
  qmi_error_type_v01

  @param [in] data_path_opt_status      Status of Data Path Optimizer
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
/*=========================================================================*/
boolean
QCMAP_Client::GetDataPathOptStatus
(
  boolean *data_path_opt_status,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_get_data_path_opt_status_req_msg_v01  req_msg;
  qcmap_msgr_get_data_path_opt_status_resp_msg_v01  resp_msg;

  memset(&req_msg, 0 , sizeof(qcmap_msgr_get_data_path_opt_status_req_msg_v01));
  memset(&resp_msg, 0 , sizeof(qcmap_msgr_get_data_path_opt_status_req_msg_v01));

  qmi_client_error_type qmi_error;
  QCMAP_LOG_FUNC_ENTRY();
  LOG_MSG_INFO1("requesting to get data path opt status",0,0,0);
  if (!qmi_err_num)
  {
    LOG_MSG_ERROR("Invalid parameter passed ",0,0,0);
    return false;
  }

  if(data_path_opt_status == NULL)
  {
    LOG_MSG_ERROR("GetDataPahtOptStatus: data_path_opt_status returned NULL",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                      QMI_QCMAP_MSGR_GET_DATA_PATH_OPT_STATUS_REQ_V01,
                                      &req_msg,
                                      sizeof(qcmap_msgr_get_data_path_opt_status_req_msg_v01),
                                      &resp_msg,
                                      sizeof(qcmap_msgr_get_data_path_opt_status_resp_msg_v01),
                                      QCMAP_MSGR_QMI_TIMEOUT_VALUE);
  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Getting Data Path opt status Failed %d : %d",
                     qmi_error,resp_msg.resp.error,0);
    *qmi_err_num = resp_msg.resp.error;
    return false;
  }

  *data_path_opt_status= resp_msg.data_path_opt_status;
  LOG_MSG_INFO1("Got Data Path Opt status success = %d",*data_path_opt_status,0,0);
  return true;
}

/*===========================================================================
FUNCTION SetDataPathOptStatus()
===========================================================================*/
/**
  Use to enable/disable data path optimization.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
/*=========================================================================*/
boolean
QCMAP_Client::SetDataPathOptStatus
(
  boolean data_path_opt_status,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_set_data_path_opt_status_req_msg_v01   req_msg;
  qcmap_msgr_set_data_path_opt_status_resp_msg_v01   resp_msg;

  qmi_client_error_type qmi_error;
  /* -------------------------------------------------------------*/

  if (!qmi_err_num)
  {
    LOG_MSG_ERROR("Invalid parameter passed ",0,0,0);
    return false;
  }

  memset(&req_msg, 0, sizeof(qcmap_msgr_set_data_path_opt_status_req_msg_v01));
  memset(&resp_msg, 0, sizeof(qcmap_msgr_set_data_path_opt_status_resp_msg_v01));

  req_msg.mobile_ap_handle = this->mobile_ap_handle;
  req_msg.data_path_opt_status = data_path_opt_status;
  QCMAP_LOG_FUNC_ENTRY();
  LOG_MSG_INFO1("requesting to set data path optimization status",0,0,0);

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_DATA_PATH_OPT_STATUS_REQ_V01,
                                       &req_msg,
                                       sizeof(qcmap_msgr_set_data_path_opt_status_req_msg_v01),
                                       &resp_msg,
                                       sizeof(qcmap_msgr_set_data_path_opt_status_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Setting Data Path Optimization  status Failed %d : %d",
                                      qmi_error, resp_msg.resp.error,0);
    *qmi_err_num = resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("Setting  Success  Data Path Optimization  status",0,0,0);

  return true;
}

/*===========================================================================
FUNCTION GetPMIPMode()
===========================================================================*/
/**

  Gets Pmip configuration.

  @datatypes
  qcmap_msgr_get_pmip_mode_resp_msg_v01
  qmi_error_type_v01

  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

*/
boolean QCMAP_Client::GetPMIPMode
(
  qcmap_msgr_get_pmip_mode_resp_msg_v01 * get_pmip_mode_resp_msg,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_get_pmip_mode_req_msg_v01  get_pmip_mode_req_msg;
  memset(get_pmip_mode_resp_msg, 0x0, sizeof(qcmap_msgr_get_pmip_mode_resp_msg_v01));

  qmi_client_error_type qmi_error;
  QCMAP_LOG_FUNC_ENTRY();
  get_pmip_mode_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_PMIP_MODE_REQ_V01,
                                       &get_pmip_mode_req_msg,
                                       sizeof(qcmap_msgr_get_pmip_mode_req_msg_v01),
                                       get_pmip_mode_resp_msg,
                                       sizeof(qcmap_msgr_get_pmip_mode_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_pmip_mode_resp_msg->resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get PMIP mode %d : %d",
                   qmi_error, get_pmip_mode_resp_msg->resp.error,0);
    *qmi_err_num = get_pmip_mode_resp_msg->resp.error;
    return false;
  }
  *qmi_err_num = get_pmip_mode_resp_msg->resp.error;
  return true;
}

/*===========================================================================
FUNCTION SetPMIPMode()
===========================================================================*/
/**

  Set Pmip configuration.

  @datatypes
  qcmap_msgr_set_pmip_mode_req_msg_v01
  qmi_error_type_v01

  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

*/

boolean QCMAP_Client::SetPMIPMode
(
  qcmap_msgr_set_pmip_mode_req_msg_v01 * set_pmip_mode_req_msg,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_set_pmip_mode_resp_msg_v01 set_pmip_mode_resp_msg;
  memset(&set_pmip_mode_resp_msg, 0x0, sizeof(qcmap_msgr_set_pmip_mode_resp_msg_v01));
  set_pmip_mode_req_msg->mobile_ap_handle = this->mobile_ap_handle;

  qmi_client_error_type qmi_error;
  QCMAP_LOG_FUNC_ENTRY();
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_PMIP_MODE_REQ_V01,
                                       set_pmip_mode_req_msg,
                                       sizeof(qcmap_msgr_set_pmip_mode_req_msg_v01),
                                       &set_pmip_mode_resp_msg,
                                       sizeof(qcmap_msgr_set_pmip_mode_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( set_pmip_mode_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not set PMIP mode %d : %d",
                   qmi_error, set_pmip_mode_resp_msg.resp.error,0);
    *qmi_err_num = set_pmip_mode_resp_msg.resp.error;
    return false;
  }
  *qmi_err_num = set_pmip_mode_resp_msg.resp.error;
  return true;
}

/*===========================================================================
  FUNCTION GetWWANRoamStatus
  ==========================================================================*/
/*!
  @brief
  Displays the current WWAN roaming status

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean
QCMAP_Client::GetWWANRoamStatus
(
  uint8_t *wwan_roam_status,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_get_wwan_roaming_status_req_msg_v01 wwan_roaming_status_req_msg;
  qcmap_msgr_get_wwan_roaming_status_resp_msg_v01 wwan_roaming_status_resp_msg;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_FUNC_ENTRY();

  if ( !qmi_err_num )
  {
    LOG_MSG_ERROR("GetWWANRoamStatus Failed: Invalid Args", 0,0,0);
    return false;
  }

  if ( !wwan_roam_status )
  {
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    LOG_MSG_ERROR("GetWWANRoamStatus Failed: %d ",*qmi_err_num ,0,0);
    return false;
  }

  memset(&wwan_roaming_status_resp_msg, 0, sizeof(wwan_roaming_status_resp_msg));

  wwan_roaming_status_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_WWAN_ROAMING_STATUS_REQ_V01,
                                       &wwan_roaming_status_req_msg,
                                       sizeof(qcmap_msgr_get_wwan_roaming_status_req_msg_v01),
                                       &wwan_roaming_status_resp_msg,
                                       sizeof(qcmap_msgr_get_wwan_roaming_status_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( wwan_roaming_status_resp_msg.resp.result != QMI_NO_ERR))
  {
    LOG_MSG_ERROR("Can not get wwan roaming status %d : %d",
                   qmi_error, wwan_roaming_status_resp_msg.resp.error,0);
    *qmi_err_num = wwan_roaming_status_resp_msg.resp.error;
    return false;
  }
  if (wwan_roam_status)
    *wwan_roam_status = wwan_roaming_status_resp_msg.wwan_roaming_status;
  return true;
}

/*===========================================================================
FUNCTION ConnectBackHaulAsync()
===========================================================================*/
/** @ingroup section_qcmap_backhaul_wwan
    @xreflabel{sec:connect_back_haul}

  Connects the Wireless Wide Area Network (WWAN) backhaul asynchronously.This
  function connects the WWAN based on the configuration provided.

  @datatypes
  qcmap_msgr_wwan_call_type_v01\n
  qmi_error_type_v01

  @param[in] call_type          Identifies call type like IPv4, IPv6 or both
  @param[in] profile_handle     Modem Profile number to be used for BackHaul
  @param[in] resp_cb            Asynchronous response callback for this request
  @param[in] user_data          Cookie user data value supplied by the client.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
QCMAP_Client::ConnectBackHaulAsync
(
  qcmap_msgr_wwan_call_type_v01 call_type,
  profile_handle_type_v01  profile_handle,
  qmi_client_recv_msg_async_cb resp_cb,
  void *user_data
)
{
  int qcmap_msgr_errno;
  int ret = QCMAP_CM_SUCCESS;
  qcmap_msgr_bring_up_wwan_req_msg_v01  qcmap_bring_up_wwan_req_msg;
  qcmap_msgr_bring_up_wwan_resp_msg_v01 qcmap_bring_up_wwan_resp_msg;
  qmi_client_error_type qmi_error;
  qmi_txn_handle txn_handle;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&qcmap_bring_up_wwan_req_msg, 0, sizeof(qcmap_msgr_bring_up_wwan_req_msg_v01));
  memset(&qcmap_bring_up_wwan_resp_msg, 0, sizeof(qcmap_msgr_bring_up_wwan_resp_msg_v01));

  /* Bring up the data call. */
  LOG_MSG_INFO1("Bring up wwan",0,0,0);
  qcmap_bring_up_wwan_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  // Call Type
  qcmap_bring_up_wwan_req_msg.call_type_valid = TRUE;
  qcmap_bring_up_wwan_req_msg.call_type = call_type;

  // Profile Index
  qcmap_bring_up_wwan_req_msg.profile_handle_valid = TRUE;
  qcmap_bring_up_wwan_req_msg.profile_handle = profile_handle;

  LOG_MSG_INFO1("Bringing up wwan call_type = %d, profile_handle = %d",
                call_type, profile_handle, 0);

  qmi_error = qmi_client_send_msg_async(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_BRING_UP_WWAN_REQ_V01,
                                       &qcmap_bring_up_wwan_req_msg,
                                       sizeof(qcmap_msgr_bring_up_wwan_req_msg_v01),
                                       &qcmap_bring_up_wwan_resp_msg,
                                       sizeof(qcmap_msgr_bring_up_wwan_resp_msg_v01),
                                       resp_cb,
                                       user_data,
                                       &txn_handle);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not bring up wwan qcmap %d",
        qmi_error, 0,0);
    return false;
  }

  LOG_MSG_INFO1("Bringing up wwan...",0,0,0);
  return true;
}

/*===========================================================================
FUNCTION DisconnectBackHaulAsync()
===========================================================================*/
/** @ingroup section_qcmap_backhaul_wwan
    @xreflabel{sec:disconnect_back_haul}

  Disconnects the WWAN backhaul asynchronously.

  @datatypes
  qcmap_msgr_wwan_call_type_v01\n
  qmi_error_type_v01

  @param[in] call_type         Identifies call type.
  @param[in] profile_handle    Modem Profile number to be used for Disconnect BackHaul
  @param[in] resp_cb           Asynchronous response callback for this request
  @param[in] user_data         Cookie user data value supplied by the client.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
QCMAP_Client::DisconnectBackHaulAsync
(
  qcmap_msgr_wwan_call_type_v01 call_type,
  profile_handle_type_v01  profile_handle,
  qmi_client_recv_msg_async_cb resp_cb,
  void *user_data
)
{
  qcmap_msgr_tear_down_wwan_req_msg_v01 qcmap_tear_down_wwan_req_msg;
  qcmap_msgr_tear_down_wwan_resp_msg_v01 qcmap_tear_down_wwan_resp_msg;
  qmi_client_error_type qmi_error;
  qmi_txn_handle txn_handle;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&qcmap_tear_down_wwan_req_msg, 0, sizeof(qcmap_msgr_tear_down_wwan_req_msg_v01));
  memset(&qcmap_tear_down_wwan_resp_msg, 0, sizeof(qcmap_msgr_tear_down_wwan_resp_msg_v01));

  LOG_MSG_INFO1("Bringing down wwan",0,0,0);
  qcmap_tear_down_wwan_req_msg.mobile_ap_handle = this->mobile_ap_handle;

  qcmap_tear_down_wwan_req_msg.call_type_valid = TRUE;
  qcmap_tear_down_wwan_req_msg.call_type = call_type;

  qcmap_tear_down_wwan_req_msg.profile_handle_valid = TRUE;
  qcmap_tear_down_wwan_req_msg.profile_handle = profile_handle;

  LOG_MSG_INFO1("Bringing down wwan call_type = %d, profile_handle = %d",
                call_type, profile_handle, 0);

  qmi_error = qmi_client_send_msg_async(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_TEAR_DOWN_WWAN_REQ_V01,
                                       &qcmap_tear_down_wwan_req_msg,
                                       sizeof(qcmap_msgr_tear_down_wwan_req_msg_v01),
                                       &qcmap_tear_down_wwan_resp_msg,
                                       sizeof(qcmap_msgr_tear_down_wwan_resp_msg_v01),
                                       resp_cb,
                                       user_data,
                                       &txn_handle);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR))
  {
    LOG_MSG_ERROR("Can not tear down wwan qcmap %d",
        qmi_error, 0,0);
    return false;
  }

  LOG_MSG_INFO1("Tearing down wwan...",0,0,0);
  return true;
}

/*===========================================================================
FUNCTION CreateWWANPolicyAsync()
===========================================================================*/
/** @ingroup section_qcmap_backhaul_wwan
    @xreflabel{sec:create_wwan_policy}

  Creates WWAN policy asynchronously.

  @datatypes
  qcmap_msgr_net_policy_info_v01\n
  qmi_error_type_v01

  @param[in] WWAN_policy       WWAN policy information to be configured.
  @param[in] resp_cb           Asynchronous response callback for this request
  @param[in] user_data         Cookie user data value supplied by the client.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
QCMAP_Client::CreateWWANPolicyAsync
(
  qcmap_msgr_net_policy_info_v01   WWAN_policy,
  qmi_client_recv_msg_async_cb resp_cb,
  void *user_data
)
{
  qcmap_msgr_create_wwan_policy_req_msg_v01   create_wwan_policy_req_msg;
  qcmap_msgr_create_wwan_policy_resp_msg_v01  create_wwan_policy_resp_msg;// = NULL;
  qmi_client_error_type                       qmi_error;
  qmi_txn_handle                              txn_handle;

  QCMAP_LOG_FUNC_ENTRY();
  memset(&create_wwan_policy_req_msg , 0, sizeof(qcmap_msgr_create_wwan_policy_req_msg_v01));
  memset(&create_wwan_policy_resp_msg , 0, sizeof(qcmap_msgr_create_wwan_policy_resp_msg_v01));

  create_wwan_policy_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  create_wwan_policy_req_msg.wwan_policy = WWAN_policy;
  //setting Wan policy ip family type as V4V6
  create_wwan_policy_req_msg.wwan_policy.ip_family = QCMAP_MSGR_IP_FAMILY_V4V6_V01;

  qmi_error = qmi_client_send_msg_async(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_CREATE_WWAN_POLICY_REQ_V01,
                                       &create_wwan_policy_req_msg,
                                       sizeof(qcmap_msgr_create_wwan_policy_req_msg_v01),
                                       &create_wwan_policy_resp_msg,
                                       sizeof(qcmap_msgr_create_wwan_policy_resp_msg_v01),
                                       resp_cb,
                                       user_data,
                                       &txn_handle);

  LOG_MSG_INFO1("Async Create WWAN Policy, valid=%d, profile=%d",
                  create_wwan_policy_resp_msg.profile_handle_valid,
                  create_wwan_policy_resp_msg.profile_handle, 0);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Cannot create profile : %d",
                   qmi_error, 0, 0);
    return false;
  }

  return true;
}

/*===========================================================================
FUNCTION GetWWANPolicyListAsync()
===========================================================================*/
/** @ingroup section_qcmap_backhaul_wwan
    @xreflabel{sec:get_allwwan_policy}

  Gets all configured WWAN policy asynchronously.

  @datatypes
  qcmap_msgr_wwan_policy_list_resp_msg_v01\n
  qmi_error_type_v01

  @param[in] resp_cb           Asynchronous response callback for this request
  @param[in] user_data         Cookie user data value supplied by the client.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
QCMAP_Client::GetWWANPolicyListAsync
(
  qmi_client_recv_msg_async_cb resp_cb,
  void *user_data
)
{
  qcmap_msgr_wwan_policy_list_req_msg_v01   wwan_policy_list_req_msg;
  qcmap_msgr_wwan_policy_list_resp_msg_v01  WWAN_policy_list_resp;
  qmi_client_error_type                     qmi_error;
  qmi_txn_handle                            txn_handle;

  QCMAP_LOG_FUNC_ENTRY();

  memset(&wwan_policy_list_req_msg , 0, sizeof(qcmap_msgr_wwan_policy_list_req_msg_v01));

  memset(&WWAN_policy_list_resp, 0, sizeof(qcmap_msgr_wwan_policy_list_resp_msg_v01));

  qmi_error = qmi_client_send_msg_async(this->qmi_qcmap_msgr_handle,
                                        QMI_QCMAP_MSGR_WWAN_POLICY_LIST_REQ_V01,
                                        &wwan_policy_list_req_msg,
                                        sizeof(qcmap_msgr_wwan_policy_list_req_msg_v01),
                                        &WWAN_policy_list_resp,
                                        sizeof(qcmap_msgr_wwan_policy_list_resp_msg_v01),
                                        resp_cb,
                                        user_data,
                                        &txn_handle);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
      ( qmi_error != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get WWAN Config %d ",
                   qmi_error,0, 0);
    return false;
  }
  return true;
}

/*===========================================================================
 FUNCTION GetLANBridges
 ===========================================================================*/
/*!
 @brief
   Gets all configured LAN Bridges for VLAN

  @return
   true  - on Success
   false - on Failure

 @note

 - Dependencies
   - None

 - Side Effects
   - None
*/
/*=========================================================================*/
boolean QCMAP_Client::GetLANBridges
(
 qcmap_msgr_bridge_list_v01* bridge_list,
 qmi_error_type_v01                        *qmi_err_num
)
{
  qcmap_msgr_get_lan_bridges_resp_msg_v01 get_lan_bridges_resp_msg_v01;
  qcmap_msgr_get_lan_bridges_req_msg_v01 get_lan_bridges_req_msg_v01;

  if(NULL == bridge_list)
  {
    LOG_MSG_ERROR("Given NULL arg: bridge_list", 0, 0, 0);
    return false;
  } else if(NULL == qmi_err_num) {
    LOG_MSG_ERROR("Given NULL arg: qmi_err_num", 0, 0, 0);
    return false;
  }

  memset(&get_lan_bridges_req_msg_v01, 0, sizeof(get_lan_bridges_req_msg_v01));
  memset(&get_lan_bridges_resp_msg_v01, 0, sizeof(get_lan_bridges_resp_msg_v01));

  *qmi_err_num = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
         QMI_QCMAP_MSGR_GET_LAN_BRIDGES_REQ_V01,
         &get_lan_bridges_req_msg_v01,
         sizeof(get_lan_bridges_req_msg_v01),
         (void*)&get_lan_bridges_resp_msg_v01,
         sizeof(qcmap_msgr_get_lan_bridges_resp_msg_v01),
         QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(GetLANBridges): error %d result %d",
      *qmi_err_num, get_lan_bridges_resp_msg_v01.resp.result,0);

  if ( ( *qmi_err_num == QMI_TIMEOUT_ERR ) ||
       ( *qmi_err_num != QMI_NO_ERR ) ||
       ( get_lan_bridges_resp_msg_v01.resp.result != QMI_NO_ERR ))
  {
    LOG_MSG_ERROR("Can not get LAN Bridges %d : %d",
                  *qmi_err_num, get_lan_bridges_resp_msg_v01.resp.error, 0);
    *qmi_err_num = get_lan_bridges_resp_msg_v01.resp.error;
    return false;
  }

  memcpy(bridge_list, &(get_lan_bridges_resp_msg_v01.bridge_list),
         sizeof(qcmap_msgr_bridge_list_v01));
  return true;
}

/*===========================================================================
 FUNCTION SelectLANBridge
 ===========================================================================*/
/*!
 @brief
   Sets the bridges context for LAN configuration menu items

  @return
   true  - on Success
   false - on Failure

 @note

 - Dependencies
   - None

 - Side Effects
   - None
*/
/*=========================================================================*/
boolean QCMAP_Client::SelectLANBridge(int16_t bridge_vlan_id, qmi_error_type_v01 *qmi_err_num)
{
  qcmap_msgr_select_lan_bridge_req_msg_v01 select_lan_bridge_req_msg_v01;
  qcmap_msgr_select_lan_bridge_resp_msg_v01 select_lan_bridge_resp_msg_v01;

  if(NULL == qmi_err_num)
  {
    LOG_MSG_ERROR("Given NULL arg", 0, 0, 0);
    return false;
  }

  memset(&select_lan_bridge_req_msg_v01, 0, sizeof(select_lan_bridge_req_msg_v01));
  memset(&select_lan_bridge_resp_msg_v01, 0, sizeof(qcmap_msgr_select_lan_bridge_resp_msg_v01));

  select_lan_bridge_req_msg_v01.mobile_ap_handle = this->mobile_ap_handle;
  select_lan_bridge_req_msg_v01.bridge_vlan_id = bridge_vlan_id;

  *qmi_err_num = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
         QMI_QCMAP_MSGR_SELECT_LAN_BRIDGE_REQ_V01,
         &select_lan_bridge_req_msg_v01,
         sizeof(select_lan_bridge_req_msg_v01),
         (void*)&select_lan_bridge_resp_msg_v01,
         sizeof(qcmap_msgr_select_lan_bridge_resp_msg_v01),
         QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync(SelectLANBridge): error %d result %d",
      *qmi_err_num, select_lan_bridge_resp_msg_v01.resp.result,0);

  if ( ( *qmi_err_num == QMI_TIMEOUT_ERR ) ||
       ( *qmi_err_num != QMI_NO_ERR ) ||
       ( select_lan_bridge_resp_msg_v01.resp.result != QMI_NO_ERR ))
  {
    LOG_MSG_ERROR("Can not Select LAN Bridge %d : %d",
                  *qmi_err_num, select_lan_bridge_resp_msg_v01.resp.error, 0);
    *qmi_err_num = select_lan_bridge_resp_msg_v01.resp.error;
    return false;
  }

 return true;
}

/*===========================================================================
  FUNCTION SetAlwaysOnWLAN
  ===========================================================================*/
/*!
  @brief
  Enable the Always On WLAN feature

  @return
  true  - on Succes
  false - on Failure

  @note

  - Dependencies
  - QCMobileAP must be enabled.

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::SetAlwaysOnWLAN
(
  boolean always_on_wlan_state,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_set_always_on_wlan_req_msg_v01 set_always_on_wlan_req_msg;
  qcmap_msgr_set_always_on_wlan_resp_msg_v01 set_always_on_wlan_resp_msg;
  qmi_client_error_type qmi_error = QMI_NO_ERR;

  if (!qmi_err_num)
  {
    LOG_MSG_ERROR("Null parameter passed ",0,0,0);
    return false;
  }

  QCMAP_LOG_FUNC_ENTRY();

  memset(&set_always_on_wlan_req_msg,0x0, sizeof(qcmap_msgr_set_always_on_wlan_req_msg_v01));
  memset(&set_always_on_wlan_resp_msg,0x0, sizeof(qcmap_msgr_set_always_on_wlan_resp_msg_v01));

  set_always_on_wlan_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  set_always_on_wlan_req_msg.enable_always_on_wlan = always_on_wlan_state;

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_AlWAYS_ON_WLAN_REQ_V01,
                                       &set_always_on_wlan_req_msg,
                                       sizeof(qcmap_msgr_set_always_on_wlan_req_msg_v01),
                                       &set_always_on_wlan_resp_msg,
                                       sizeof(qcmap_msgr_set_always_on_wlan_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);
  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( set_always_on_wlan_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not set Always on WLAN state %d : %d",
        qmi_error, set_always_on_wlan_resp_msg.resp.error,0);
    *qmi_err_num = set_always_on_wlan_resp_msg.resp.error;
    return false;
  }
  LOG_MSG_INFO1("Always on WLAN Set succeeded Always on WLAN state is:%d",
                always_on_wlan_state,0,0);
  return true;
}

/*===========================================================================
  FUNCTION GetAlwaysOnWLAN
  ===========================================================================*/
/*!
  @brief
  Display the Always On WLAN feature is enabled or not

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::GetAlwaysOnWLAN
(
  boolean *always_on_wlan_status,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_get_always_on_wlan_resp_msg_v01 get_always_on_wlan_resp_msg;
  qmi_client_error_type qmi_error = QMI_NO_ERR;

  if (!qmi_err_num)
  {
    LOG_MSG_ERROR("Null parameter passed ",0,0,0);
    return false;
  }
  if (!always_on_wlan_status)
  {
    LOG_MSG_ERROR("Invalid parameter passed ",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }
  QCMAP_LOG_FUNC_ENTRY();

  memset(&get_always_on_wlan_resp_msg, 0x0, sizeof(qcmap_msgr_get_always_on_wlan_resp_msg_v01));

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_AlWAYS_ON_WLAN_REQ_V01,
                                       NULL,
                                       0,
                                       &get_always_on_wlan_resp_msg,
                                       sizeof(qcmap_msgr_get_always_on_wlan_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_always_on_wlan_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get Always on WLAN %d : %d",
                   qmi_error, get_always_on_wlan_resp_msg.resp.error,0);
    *qmi_err_num = get_always_on_wlan_resp_msg.resp.error;
    return false;
  }

  *always_on_wlan_status = get_always_on_wlan_resp_msg.always_on_wlan_status;
  LOG_MSG_INFO1("Get Always on WLAN Status succeeded. Status: %d", *always_on_wlan_status, 0, 0);
  return true;
}

/*===========================================================================
  FUNCTION set_p2p_role
  ===========================================================================*/
/*!
  @brief
  Set p2p_role if p2p_status is enabled

  @return
  true  - on Succes
  false - on Failure

  @note

  - Dependencies
  - QCMobileAP must be enabled.

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::set_p2p_role
(
  qcmap_p2p_config p2p_config,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_set_p2p_role_req_msg_v01 set_p2p_role_req_msg;
  qcmap_msgr_set_p2p_role_resp_msg_v01 set_p2p_role_resp_msg;
  qmi_client_error_type qmi_error = QMI_NO_ERR;

  if (!qmi_err_num)
  {
    LOG_MSG_ERROR("Null parameter passed ",0,0,0);
    return false;
  }

  QCMAP_LOG_FUNC_ENTRY();

  memset(&set_p2p_role_req_msg,0x0, sizeof(qcmap_msgr_set_p2p_role_req_msg_v01));
  memset(&set_p2p_role_resp_msg,0x0, sizeof(qcmap_msgr_set_p2p_role_resp_msg_v01));

  set_p2p_role_req_msg.mobile_ap_handle = this->mobile_ap_handle;
  set_p2p_role_req_msg.p2p_status = p2p_config.p2p_status;

  if(p2p_config.p2p_role_valid)
  {
    set_p2p_role_req_msg.p2p_role_valid = TRUE;
    set_p2p_role_req_msg.p2p_role = p2p_config.p2p_role;
  }

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_SET_P2P_ROLE_REQ_V01,
                                       &set_p2p_role_req_msg,
                                       sizeof(qcmap_msgr_set_p2p_role_req_msg_v01),
                                       &set_p2p_role_resp_msg,
                                       sizeof(qcmap_msgr_set_p2p_role_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);
  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( set_p2p_role_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR(" Failed to set peer-to-peer role %d : %d",
        qmi_error, set_p2p_role_resp_msg.resp.error,0);
    *qmi_err_num = set_p2p_role_resp_msg.resp.error;
    return false;
  }
  LOG_MSG_INFO1("set peer-to-peer role succeeded. peer-to-peer role is:%d p2p_status: %d",
                p2p_config.p2p_role, p2p_config.p2p_status, 0);
  return true;
}

/*===========================================================================
  FUNCTION get_p2p_role
  ===========================================================================*/
/*!
  @brief
  Get p2p_role if p2p_status is enabled

  @return
  true  - on Succes
  false - on Failure


  @note

  - Dependencies
  - QCMobileAP must be enabled.

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::get_p2p_role
(
  qcmap_p2p_config* p2p_config,
  qmi_error_type_v01* qmi_err_num

)
{
  QCMAP_LOG_FUNC_ENTRY();
  qcmap_msgr_get_p2p_role_resp_msg_v01 get_p2p_role_resp_msg;
  qmi_client_error_type qmi_error = QMI_NO_ERR;

  if (!qmi_err_num || !p2p_config)
  {
    LOG_MSG_ERROR("Null parameter passed p2p_config:%p qmi_err_num:%p",p2p_config, qmi_err_num, 0);
    return false;
  }

  memset(&get_p2p_role_resp_msg, 0x0, sizeof(qcmap_msgr_get_p2p_role_resp_msg_v01));

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_P2P_ROLE_REQ_V01,
                                       NULL,
                                       0,
                                       &get_p2p_role_resp_msg,
                                       sizeof(qcmap_msgr_get_p2p_role_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_p2p_role_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get P2P role  %d : %d",
                   qmi_error, get_p2p_role_resp_msg.resp.error,0);
    *qmi_err_num = get_p2p_role_resp_msg.resp.error;
    return false;
  }

  p2p_config->p2p_status = get_p2p_role_resp_msg.p2p_status;
  if(get_p2p_role_resp_msg.p2p_role_valid)
  {
    p2p_config->p2p_role_valid = TRUE;
    p2p_config->p2p_role = get_p2p_role_resp_msg.p2p_role;
  }
  LOG_MSG_INFO1("p2p_status: %d p2p_role: %d qmi_err_num: %d",
                 p2p_config->p2p_status, p2p_config->p2p_role, *qmi_err_num);
  return true;
}

/*===========================================================================
  FUNCTION GetWWANProfilePreference
  ===========================================================================*/
/*!
  @brief
  Gets WWAN Profile Preference

  @return
  true  - on Success
  false - on Failure

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/

boolean QCMAP_Client::GetWWANProfilePreference
(
  profile_handle_type_v01 *current_profile_handle,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_get_wwan_profile_preference_resp_msg_v01 get_wwan_profile_preference_resp_msg;
  qmi_client_error_type qmi_error = QMI_NO_ERR;

  if (!qmi_err_num)
  {
    LOG_MSG_ERROR("Null parameter passed ",0,0,0);
    return false;
  }
  if (!current_profile_handle)
  {
    LOG_MSG_ERROR("Invalid parameter passed ",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }
  QCMAP_LOG_FUNC_ENTRY();

  memset(&get_wwan_profile_preference_resp_msg, 0x0,
         sizeof(qcmap_msgr_get_wwan_profile_preference_resp_msg_v01));

  qmi_error = qmi_client_send_msg_sync(this->qmi_qcmap_msgr_handle,
                                       QMI_QCMAP_MSGR_GET_WWAN_PROFILE_PREFERENCE_REQ_V01,
                                       NULL,
                                       0,
                                       &get_wwan_profile_preference_resp_msg,
                                       sizeof(qcmap_msgr_get_wwan_profile_preference_resp_msg_v01),
                                       QCMAP_MSGR_QMI_TIMEOUT_VALUE);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_wwan_profile_preference_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get WWAN profile preference %d : %d",
                   qmi_error, get_wwan_profile_preference_resp_msg.resp.error,0);
    *qmi_err_num = get_wwan_profile_preference_resp_msg.resp.error;
    return false;
  }

  *current_profile_handle = get_wwan_profile_preference_resp_msg.current_profile_handle;
  LOG_MSG_INFO1("Get WWAN Profile Preference succeeded. profile handle: %d", *current_profile_handle, 0, 0);
  return true;
}

