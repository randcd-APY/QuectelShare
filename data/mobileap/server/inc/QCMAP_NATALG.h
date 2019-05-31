#ifndef _QCMAP_NATALG_H_
#define _QCMAP_NATALG_H_


/*====================================================

FILE:  QCMAP_Firewall.h

SERVICES:
   QCMAP Connection Manager Backhaul AP-STA Class

=====================================================

  Copyright (c) 2011-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*=====================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.
    when       who        what, where, why
  --------   ---        -------------------------------------------------------
  08/19/14   vm           Created
======================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/select.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include "stringl.h"
#include "ds_util.h"
#include "comdef.h"
#include "qcmap_cm_api.h"
#include "QCMAP_SOCKSv5_Configure.h"

#ifndef QCMAP_Backhaul
class QCMAP_Backhaul;
#endif

#define KERNEL_VERSION_4_9 "4.9"

typedef enum {
  CONFIG_NAT_ENTRY_GENERIC_TIMEOUT = 1,
  CONFIG_NAT_ENTRY_ICMP_TIMEOUT,
  CONFIG_NAT_ENTRY_TCP_ESTABLISHED_TIMEOUT,
  CONFIG_NAT_ENTRY_UDP_TIMEOUT,
  CONFIG_DMZ_IP,
  CONFIG_PPTP_VPN_PASSTHROUGH,
  CONFIG_L2TP_VPN_PASSTHROUGH,
  CONFIG_IPSEC_VPN_PASSTHROUGH,
  CONFIG_SOCKSv5_PROXY,
  CONFIG_SOCKSv5_CONF_FILE,
  CONFIG_SOCKSv5_AUTH_FILE,
  CONFIG_RTSP_ALG,
  CONFIG_SIP_ALG,
  CONFIG_NAT_TYPE,
  CONFIG_PKT_LIMIT,
  CONFIG_SNAT_ENTERIES,

  CONFIG_NAT_CONFIG_MAX //Last item
}qcmap_nat_config_enum;

class QCMAP_NATALG
{
  private:
    /* SOCKSv5 private fcns */
    static boolean InvokeSOCKSv5Proxy(qcmap_msgr_socksv5_config_file_paths_v01 *socksv5_file_path);
    static boolean KillSOCKSv5Proxy(void);

  public:
    //parent backhaul object that create firewall object
    QCMAP_Backhaul* QcMapBackhaul;
    static qcmap_cm_nat_conf_common_t nat_config_common;
    qcmap_cm_nat_conf_t nat_config;
    // Data structure to store mask for ALGs Enabled/ Disabled.
    qcmap_msgr_alg_type_mask_v01 alg_enabled_disabled_mask;

    QCMAP_NATALG(QCMAP_Backhaul* QcMapBackhaul);
    ~QCMAP_NATALG();

    void Init();
    profile_handle_type_v01 GetProfileHandle();
    boolean AddSNATEntryOnApps( qcmap_cm_port_fwding_entry_conf_t* nat_entry,
                                qmi_error_type_v01 *qmi_err_num);

    boolean DeleteSNATEntryOnApps( qcmap_cm_port_fwding_entry_conf_t* nat_entry,
                                    qmi_error_type_v01 *qmi_err_num);


    boolean AddDMZOnApps(uint32 dmz_ip, qmi_error_type_v01 *qmi_err_num);
    boolean DeleteDMZOnApps(uint32 dmz_ip, qmi_error_type_v01 *qmi_err_num);

    boolean SetIPSECVpnPassThroughOnApps( boolean enable,
                                          qmi_error_type_v01 *qmi_err_num);

    boolean SetPPTPVpnPassThroughOnApps( boolean enable,
                                         qmi_error_type_v01 *qmi_err_num);

    boolean SetL2TPVpnPassThroughOnApps( boolean enable,
                                         qmi_error_type_v01 *qmi_err_num);

    /* NAT Type */
    boolean SetNatType( qcmap_msgr_nat_enum_v01 nat_type,
                        qmi_error_type_v01 *qmi_err_num);

    boolean GetNatType( qcmap_msgr_nat_enum_v01 * cur_nat_type,
                        qmi_error_type_v01 *qmi_err_num);

    /* NAT Timeout */
    static boolean SetNatTimeout( qcmap_msgr_nat_timeout_enum_v01 timeout_type,
                                  uint32 timeout_value,
                                  qmi_error_type_v01 *qmi_err_num);

    static boolean GetNatTimeout( qcmap_msgr_nat_timeout_enum_v01 timeout_type,
                                  uint32 *timeout_value,
                                  qmi_error_type_v01 *qmi_err_num);

    static boolean SetNatTimeoutOnApps( qcmap_msgr_nat_timeout_enum_v01 timeout_type,
                                 uint32 timeout_value,
                                 qmi_error_type_v01 *qmi_err_num);

    static boolean GetNatTimeoutOnApps( qcmap_msgr_nat_timeout_enum_v01 timeout_type,
                                 uint32 *timeout_value,
                                 qmi_error_type_v01 *qmi_err_num);

    boolean GetSetPassthroughFlag( qcmap_nat_config_enum,
                                          qcmap_action_type, uint8 *,
                                          qmi_error_type_v01 *);


    /* Add a Static Nat Entry to the configuration and update XML file. */
    boolean AddStaticNatEntry( qcmap_cm_port_fwding_entry_conf_t* nat_entry,
                                      qmi_error_type_v01 *qmi_err_num);

    /* Delete a Static Nat Entry from the configuration and update XML file. */
    boolean DeleteStaticNatEntry( qcmap_cm_port_fwding_entry_conf_t* nat_entry,
                                         qmi_error_type_v01 *qmi_err_num);

    /* Get a Static Nat Entries from the configuration. */
    boolean GetStaticNatEntries(qcmap_msgr_snat_entry_config_v01 *snat_config,
                                unsigned int* num_entries,
                                qmi_error_type_v01 *qmi_err_num);

    /* Add a DMZ IP address to the configuration and update XML file. */
    boolean AddDMZ(uint32 dmz_ip, qmi_error_type_v01 *qmi_err_num);

    /* Get the DMZ IP address from the configuration . */
    boolean GetDMZ(uint32_t *dmz_ip, qmi_error_type_v01 *qmi_err_num);

    /* Delete a DMZ IP address from the configuration and update XML file. */
    boolean DeleteDMZ(qmi_error_type_v01 *qmi_err_num);

    /* Set NAT Timeout in the configuration and update XML file. */
    static boolean SetNATEntryTimeout(uint16 timeout);

    /* Get NAT Timeout from the configuration and update XML file. */
    static uint16 GetNATEntryTimeout(void);

    /* Enable/Disable NAT on A5 (In Station mode) */
    boolean EnableNATonApps(void);
    boolean DisableNATonApps(void);


    /*Misc NAT tasks to be done after NAT types is set */
    boolean EnableMiscNATTasks(void);

    boolean GetSetNATConfigFromXML(qcmap_nat_config_enum conf,
                                   qcmap_action_type action,
                                   char *data,
                                   int data_len);

    boolean AddNATEntryToList(qcmap_cm_port_fwding_entry_conf_t*);

    boolean GetStaticNATEntriesFromXML(qcmap_msgr_snat_entry_config_v01*,
                                       unsigned int*);

    pugi::xml_node searchNATEntryInXML(pugi::xml_document*,
                                       qcmap_cm_port_fwding_entry_conf_t*);

    boolean ReadNATConfigFromXML(char *);
    int addNATEntryToXML( pugi::xml_document*, qcmap_cm_port_fwding_entry_conf_t*);

    /* Enable Algs */
    static boolean EnableAlg( qcmap_msgr_alg_type_mask_v01 alg_type,
                              qmi_error_type_v01 *qmi_err_num);
    /* Disable Algs */
    static boolean DisableAlg( qcmap_msgr_alg_type_mask_v01 alg_type,
                               qmi_error_type_v01 *qmi_err_num);
    /* Enable RTSP Alg */
    static boolean EnableRTSPAlg(qmi_error_type_v01 *qmi_err_num);
    /* Disable RTSP Alg */
    static boolean DisableRTSPAlg(qmi_error_type_v01 *qmi_err_num);

    int GetNATEntryCountFromXML(pugi::xml_document*);
    static boolean EnableSIPAlg(qmi_error_type_v01*);
    static boolean DisableSIPAlg(qmi_error_type_v01*);
    static boolean SetInitialPacketThreshold
                                 ( uint32 timeout_value,
                                   qmi_error_type_v01 *qmi_err_num);
    static boolean GetInitialPacketThreshold
                                 ( uint32 *packet_count,
                                   qmi_error_type_v01 *qmi_err_num);

    boolean CleanIPv4FilterTableFwdChain();
    boolean CleanIPv4FilterTableInChain();
    boolean CleanIPv4NatTable();
    boolean CleanIPv6FilterTableFwdChain();

    /* SOCKSv5 fcns */
    static unsigned char GetSOCKSv5ProxyEnableCfg(qmi_error_type_v01 *qmi_err_num);
    static boolean EnableSOCKSv5Proxy(qmi_error_type_v01 *qmi_err_num);
    static boolean DisableSOCKSv5Proxy(qmi_error_type_v01 *qmi_err_num);
    static boolean GetSOCKSv5ConfigFilePathFromXML(qcmap_msgr_socksv5_config_file_paths_v01 *config_file_path);
    static boolean SetSOCKSv5ConfigFilePath(const char *conf_file, const char *auth_file);
    boolean SetSOCKSv5Backhaul(unsigned int service_no, int wan_ip_ver, const char* wan_iface,
                               const char* pri_dns_ip_addr, const char* sec_dns_ip_addr);
    boolean DeleteSOCKSv5Backhaul(const char* wan_iface);
    boolean SetSOCKSv5WANIPVer(const char* wan_iface, int wan_ip_ver);
};

long int qcmap_compare_nat_entries(const void *first,const void *second);

#endif
