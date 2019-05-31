/**
 * @file ql_mgmt_client.h
 * @brief Quectel manager client interface function declarations.
 *
 * @note
 */
/*=============================================================================
  Copyright (c) 2018 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=============================================================================*/

/*============================================================================

                          EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  WHEN             WHO         WHAT, WHERE, WHY
  ------------     -------     ----------------------------------------
  11/20/2017       Mike        Initial creation.
  04/10/2018       Mike        Thread resource release
  07/17/2018       Mike        Adds WiFi ap station connected/disconnected event.
  08/10/2018       Mike        Adds WiFi restore defualt configuration.
  08/11/2018       Mike        Adds WiFi Mac address acl.
  10/10/2018       Mike        Adds WiFi station connected/disconnected event.
  10/22/2018       Mike        Adds ethernet mode.
============================================================================*/
#ifndef __QL_MGMT_CLIENT_H__
#define __QL_MGMT_CLIENT_H__

#include <stdbool.h>
#include <netinet/in.h>

#include "ql_manager_access_msgr_v01.h"

#ifdef FEATURE_QL_WIFI
#include "ql_mgmt_wifi.h"
#endif  /* end of FEATURE_QL_WIFI */

#ifdef FEATURE_QL_DATA_CALL
#include "ql_mgmt_data_call.h"
#endif /* end of FEATURE_QL_DATA_CALL */

#ifdef FEATURE_QL_NETWORK
#include "ql_mgmt_network.h"
#endif /* end of FEATURE_QL_NETWORK */

#define QL_MGMT_CLIENT_ERR_INVALID_HANDLE                    (-1)
#define QL_MGMT_CLIENT_ERR_COMMAND_NOT_FOUND                 (-2)

#define COMMAND_OFFSET_SIZE 16

#define QL_MGMT_CLIENT_COMMAND_WIFI        0x0001
#define QL_MGMT_CLIENT_COMMAND_ETH         0x0002
#define QL_MGMT_CLIENT_COMMAND_NETWORK     0x0003
#define QL_MGMT_CLIENT_COMMAND_FIREWALL    0x0004
#define QL_MGMT_CLIENT_COMMAND_PLATFORM    0x0005
#define QL_MGMT_CLIENT_COMMAND_DATA_CALL   0x0006

#define COMMAND_OFFSET(client, msg) (client) << COMMAND_OFFSET_SIZE | msg

typedef void* ql_mgmt_client_type;

typedef enum {
#ifdef FEATURE_QL_WIFI
	/* WiFI commands list */
	QL_COMMAND_WIFI_WORK_MODE = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_WIFI, QL_MGMT_CLIENT_COMMAND_WIFI_WORK_MODE),
	QL_COMMAND_WIFI_ENABLE = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_WIFI, QL_MGMT_CLIENT_COMMAND_WIFI_ENABLE),
	QL_COMMAND_WIFI_COUNTRY_CODE = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_WIFI, QL_MGMT_CLIENT_COMMAND_WIFI_COUNTRY_CODE),
	QL_COMMAND_WIFI_MODE = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_WIFI, QL_MGMT_CLIENT_COMMAND_WIFI_MODE),
	QL_COMMAND_WIFI_CHANNEL = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_WIFI, QL_MGMT_CLIENT_COMMAND_WIFI_CHANNEL),
	QL_COMMAND_WIFI_SSID = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_WIFI, QL_MGMT_CLIENT_COMMAND_WIFI_SSID),
	QL_COMMAND_WIFI_SSID_HIDE = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_WIFI, QL_MGMT_CLIENT_COMMAND_WIFI_SSID_HIDE),
	QL_COMMAND_WIFI_BANDWIDTH = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_WIFI, QL_MGMT_CLIENT_COMMAND_WIFI_BANDWIDTH),
	QL_COMMAND_WIFI_MAX_STA = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_WIFI, QL_MGMT_CLIENT_COMMAND_WIFI_MAX_STA),
	QL_COMMAND_WIFI_11N_ONLY = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_WIFI, QL_MGMT_CLIENT_COMMAND_WIFI_11N_ONLY),
	QL_COMMAND_WIFI_11AC_ONLY = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_WIFI, QL_MGMT_CLIENT_COMMAND_WIFI_11AC_ONLY),
	QL_COMMAND_WIFI_AP_ISOLATE = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_WIFI, QL_MGMT_CLIENT_COMMAND_WIFI_AP_ISOLATE),
	QL_COMMAND_WIFI_DFS = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_WIFI, QL_MGMT_CLIENT_COMMAND_WIFI_DFS),
	QL_COMMAND_WIFI_AUTH = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_WIFI, QL_MGMT_CLIENT_COMMAND_WIFI_AUTH),
	QL_COMMAND_WIFI_RSSI = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_WIFI, QL_MGMT_CLIENT_COMMAND_WIFI_RSSI),
	QL_COMMAND_WIFI_STA_INFO = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_WIFI, QL_MGMT_CLIENT_COMMAND_WIFI_STA_INFO),
	QL_COMMAND_WIFI_RESTORE_DEFAULT = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_WIFI, QL_MGMT_CLIENT_COMMAND_WIFI_RESTORE_DEFAULT),
	QL_COMMAND_WIFI_MAC_ACL_RULE = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_WIFI, QL_MGMT_CLIENT_COMMAND_WIFI_MAC_ACL_RULE),
	QL_COMMAND_WIFI_MAC_ACL_ADD = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_WIFI, QL_MGMT_CLIENT_COMMAND_WIFI_MAC_ACL_ADD),
	QL_COMMAND_WIFI_MAC_ACL_DEL = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_WIFI, QL_MGMT_CLIENT_COMMAND_WIFI_MAC_ACL_DEL),
	QL_COMMAND_WIFI_MAC_ACL_INFO = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_WIFI, QL_MGMT_CLIENT_COMMAND_WIFI_MAC_ACL_INFO),
	QL_COMMAND_WIFI_ACTIVE = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_WIFI, QL_MGMT_CLIENT_COMMAND_WIFI_ACTIVE),
	QL_COMMAND_WIFI_STATION_STATUS = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_WIFI, QL_MGMT_CLIENT_COMMAND_WIFI_STATION_STATUS),
	
#endif /* end of FEATURE_QL_WIFI */

#ifdef FEATURE_QL_DATA_CALL
	/* Data_Call commands list */
	QL_COMMAND_DATA_CALL_UP = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_DATA_CALL, QL_MGMT_CLIENT_COMMAND_DATA_CALL_UP),
	QL_COMMAND_DATA_CALL_DOWN = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_DATA_CALL, QL_MGMT_CLIENT_COMMAND_DATA_CALL_DOWN),
	QL_COMMAND_DATA_CALL_INFO = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_DATA_CALL, QL_MGMT_CLIENT_COMMAND_DATA_CALL_INFO),
	QL_COMMAND_DATA_CALL_APN = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_DATA_CALL, QL_MGMT_CLIENT_COMMAND_DATA_CALL_APN),
	QL_COMMAND_DATA_CALL_APN_PROFILE_LIST = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_DATA_CALL, QL_MGMT_CLIENT_COMMAND_DATA_CALL_APN_PROFILE_LIST),
	QL_COMMAND_DATA_CALL_DEL_APN = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_DATA_CALL, QL_MGMT_CLIENT_COMMAND_DATA_CALL_APN_DEL),
	QL_COMMAND_DATA_CALL_ADD_APN = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_DATA_CALL, QL_MGMT_CLIENT_COMMAND_DATA_CALL_APN_ADD),
	QL_COMMAND_DATA_CALL_DEFAULT_PROFILE = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_DATA_CALL, QL_MGMT_CLIENT_COMMAND_DATA_CALL_DEFAULT_PROFILE),

#endif /* end of FEATURE_QL_DATA_CALL */

#ifdef FEATURE_QL_NETWORK
	/* Network commands list */
	QL_COMMAND_NETWORK_DMZ_ADD = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_NETWORK, QL_MGMT_CLIENT_COMMAND_NETWORK_DMZ_ADD),
	QL_COMMAND_NETWORK_DMZ_DEL = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_NETWORK, QL_MGMT_CLIENT_COMMAND_NETWORK_DMZ_DEL),
	QL_COMMAND_NETWORK_DMZ_GET = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_NETWORK, QL_MGMT_CLIENT_COMMAND_NETWORK_DMZ_GET),
	QL_COMMAND_NETWORK_SGMII_ENABLE = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_NETWORK, QL_MGMT_CLIENT_COMMAND_NETWORK_SGMII_ENABLE),
	QL_COMMAND_NETWORK_SGMII_CONFIG = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_NETWORK, QL_MGMT_CLIENT_COMMAND_NETWORK_SGMII_CONFIG),
	QL_COMMAND_NETWORK_SGMII_INFO = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_NETWORK, QL_MGMT_CLIENT_COMMAND_NETWORK_SGMII_INFO),
	QL_COMMAND_NETWORK_LAN_CONFIG = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_NETWORK, QL_MGMT_CLIENT_COMMAND_NETWORK_LAN_CONFIG),
	QL_COMMAND_NETWORK_SNAT_ADD = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_NETWORK, QL_MGMT_CLIENT_COMMAND_NETWORK_SNAT_ADD),
	QL_COMMAND_NETWORK_SNAT_DEL = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_NETWORK, QL_MGMT_CLIENT_COMMAND_NETWORK_SNAT_DEL),
	QL_COMMAND_NETWORK_SNAT_GET = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_NETWORK, QL_MGMT_CLIENT_COMMAND_NETWORK_SNAT_GET),
	QL_COMMAND_NETWORK_ETHERNET_MODE = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_NETWORK, QL_MGMT_CLIENT_COMMAND_NETWORK_ETHERNET_MODE),
#endif /* end of FEATURE_QL_NETWORK */

	QL_COMMAND_INVALID = 0xFFFFFFFF,
} ql_mgmt_client_command_e;

typedef enum {
#ifdef FEATURE_QL_DATA_CALL
	/* Data Call indication list */
	QL_COMMAND_IND_DATA_CALL_STATE = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_DATA_CALL, QL_MGMT_CLIENT_IND_DATA_CALL_STATE),
#endif /* end of FEATURE_QL_DATA_CALL */

#ifdef FEATURE_QL_WIFI
	QL_COMMAND_IND_WIFI_ENABLE = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_WIFI, QL_MGMT_CLIENT_COMMAND_WIFI_ENABLE),
	QL_COMMAND_IND_WIFI_AP_STA_INFO = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_WIFI, QL_MGMT_CLIENT_COMMAND_WIFI_STA_INFO),
	QL_COMMAND_IND_WIFI_STA_STATUS = COMMAND_OFFSET(QL_MGMT_CLIENT_COMMAND_WIFI, QL_MGMT_CLIENT_COMMAND_WIFI_STATION_STATUS),
#endif /* end of FEATURE_QL_WIFI */
	QL_COMMAND_IND_INVALD = 0xFFFFFFFF,
} ql_mgmt_client_ind_e;

typedef void (*ql_manager_ind_cb)(ql_mgmt_client_ind_e ind_cmd, void *ind_data, int ind_size);

/*========================================================================
  FUNCTION: ql_mgmt_client_set
=========================================================================*/
/*! @brief 
     Sets configuration.

    @param[in] handle          Handle used by the infrastructure to
                               identify different clients.
    @param[in] command         the commands of manager client.
    @param[in] opt             the get or set flags.
    @param[in] val             the configuration of value.
    @param[in] size            the value of size.

    @return
       On success, 0 is return. On error, -1 is return.
*/
/*=======================================================================*/
extern int ql_mgmt_client_set(ql_mgmt_client_type handle, ql_mgmt_client_command_e command, void *val, int size);

/*========================================================================
  FUNCTION: ql_mgmt_client_get
=========================================================================*/
/*! @brief 
     Gets configuration.

    @param[in] handle          Handle used by the infrastructure to
                               identify different clients.
    @param[in] command         the commands of manager client.
    @param[in] opt             the get or set flags.
    @param[in] val             the configuration of value.
    @param[in] size            the value of size.

    @return
       On success, 0 is return. On error, -1 is return.
*/
/*=======================================================================*/
extern int ql_mgmt_client_get(ql_mgmt_client_type handle, ql_mgmt_client_command_e command, void *val, int size);

/*========================================================================
  FUNCTION: ql_mgmt_client_open
=========================================================================*/
/*! @brief 
      Connect with the QL_MANAGER_ACCESS service and registers the client callback
  function for any asynchronous indications received from the QL_MANAGER_ACCESS
  service.

    @param[in] handle          Handle used by the infrastructure to
                               identify different clients.
    @param[in] ind_cb          Callback of indications message.

    @return
       On success, 0 is return. On error, -1 is return.
*/
/*=======================================================================*/
extern int ql_mgmt_client_open(ql_mgmt_client_type *handle, ql_manager_ind_cb ind_cb);

/*========================================================================
  FUNCTION: ql_mgmt_client_close
=========================================================================*/
/*! @brief 
      Close a manager qmi client.

    @param[in] handle          Handle used by the infrastructure to
                               identify different clients.
    @param[in] ind_cb          Callback of indient message.

    @return
       On success, 0 is return. On error, -1 is return.
*/
/*=======================================================================*/
extern void ql_mgmt_client_close(ql_mgmt_client_type handle);

/*========================================================================
  FUNCTION: ql_mgmt_get_errno
=========================================================================*/
/*! @brief 
      Gets errno.

    @return
       the errno is return.
*/
/*=======================================================================*/
extern int ql_mgmt_get_errno(void);

#endif /* end of __QL_MGMT_CLIENT_H__ */
