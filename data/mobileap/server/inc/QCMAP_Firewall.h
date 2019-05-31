#ifndef _QCMAP_FIREWALL_H_
#define _QCMAP_FIREWALL_H_


/*====================================================

FILE:  QCMAP_Firewall.h

SERVICES:
   QCMAP Connection Manager Backhaul AP-STA Class

=====================================================

  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
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
#include "pugixml/pugixml.hpp"

#ifndef QCMAP_Backhaul
class QCMAP_Backhaul;
#endif

typedef enum {
  CONFIG_FIREWALL_ENABLED = 1,
  CONFIG_FIREWALL_PACKETS_ALLOWED,
}qcmap_firewall_config_enum;


#define SEARCH_FIREWALL_ENTRY -1
#define CONNTRACK_ENTRIES "/tmp/conntrack_entries.txt"
#define IPv6_CONNTRACK_FILTER_PATH "/tmp/v6conntrack.txt"



class QCMAP_Firewall
{
  private:

  public:
    //parent backhaul object that create firewall object
    QCMAP_Backhaul* QcMapBackhaul;
    qcmap_cm_firewall_conf_t firewall_config;

    QCMAP_Firewall(QCMAP_Backhaul* QcMapBackhaulMgr);
     ~QCMAP_Firewall();

    void Init();
    profile_handle_type_v01 GetProfileHandle();

    /*Add an Extended Firewall rule to the configuration*/
    int AddFireWallEntry( qcmap_msgr_firewall_conf_t* firewall_entry,
                                       qmi_error_type_v01 *qmi_err_num);

    int  SetFirewall( qcmap_msgr_firewall_entry_conf_t* firewall_entry,
                         boolean add_rule, qmi_error_type_v01 *qmi_err_num );

    bool DeleteConntrackEntryForDropIPv4FirewallEntries( qcmap_msgr_firewall_entry_conf_t *firewall_entry,
                                      uint8 protocol);

    bool DeleteConntrackEntryForAcceptIPv4FirewallEntries( qcmap_msgr_firewall_entry_conf_t *firewall_entry,
                                                               uint8 protocol_num);

    int  SetFirewallV4( qcmap_msgr_firewall_entry_conf_t* firewall_entry,
                            boolean add_rule, qmi_error_type_v01 *qmi_err_num );

    int  SetFirewallV6( qcmap_msgr_firewall_entry_conf_t* firewall_entry,
                            boolean add_rule, qmi_error_type_v01 *qmi_err_num );

    /*Get an Extended Firewall rule from the configuration*/
    boolean GetFireWallEntry(qcmap_msgr_get_firewall_entry_resp_msg_v01* resp,
                             uint32_t handle, qmi_error_type_v01 *qmi_err_num);

    /*Delete extended firewall rule from the configuration*/
    boolean DeleteFireWallEntry(qcmap_msgr_firewall_entry_conf_t* firewall_entry,
                                int firewall_handle, qmi_error_type_v01 *qmi_err_num );

    /*Get Firewall rule handles from the configuration*/
    boolean GetFireWallHandleList(qcmap_msgr_firewall_conf_t* firewall_entry,
                                  qmi_error_type_v01 *qmi_err_num);


    boolean GetSetFirewallConfigFromXML(qcmap_action_type,boolean*, boolean*, boolean*);
    boolean ReadFirewallXML();
    boolean GetFirewallEntryFromXML( pugi::xml_node,
                                               qcmap_msgr_firewall_entry_conf_t * );

    pugi::xml_node searchFirewallByHandleInXML(pugi::xml_document*,
                                               qcmap_msgr_firewall_entry_conf_t*, int);

    pugi::xml_node searchFirewallByIndexInXML(pugi::xml_document*,
                                              qcmap_msgr_firewall_entry_conf_t*, int);

    int addFirewallEntryToXML(pugi::xml_document*, qcmap_msgr_firewall_entry_conf_t*);

    boolean addFirewallEntryToList( qcmap_msgr_firewall_entry_conf_t*,
                                           qcmap_msgr_firewall_entry_conf_t* );


    /*Read QCMAP extended firewall config from XML */
    boolean ReadConfigFromFirewallXML(void);

    /*Write QCMAP extended firewall config to XML */
    boolean WriteConfigToFirewallXML(void);

    /* Delete QCMAP extended firewall config from XML */
    boolean DeleteConfigFromFirewallXML(void);

    static void WriteDefaultsToFirewallXML();

    boolean SetDefaultFirewallRule( qcmap_msgr_ip_family_enum_v01 interface );

    void SetDefaultFirewall(void);

    boolean EnableIPV6DefaultFirewall(void);

    boolean SetFirewallConfig( boolean, boolean,  boolean , qmi_error_type_v01*, boolean, boolean);

    boolean GetFirewallConfig( boolean*, boolean *,boolean *, qmi_error_type_v01* );

    boolean EnableIPV6Firewall(void);

    void CleanIPv6MangleTable(void);

    void UpdateIPv6FirewallDefaultRules(char *devname, boolean del);

    void CleanIPv4MangleTable(void);

    int GetFirewallEntryCountFromXML( pugi::xml_document * );

    void Dump_firewall_conf( qcmap_msgr_firewall_entry_conf_t *firewall_entry );

    void DeleteConntrackEntryForDropIPv6FirewallEntries(qcmap_msgr_firewall_entry_conf_t *firewall_entry,
                                                  uint8 protocol_num);

    void DeleteConntrackEntryForAcceptIPv6FirewallEntries(qcmap_msgr_firewall_entry_conf_t *firewall_entry,
                                                  uint8 protocol_num);

    boolean DeleteExternalIpv4Conntracks();

    boolean DeleteExternalIpv6Conntracks();

    pugi::xml_node GetPdnRootXML(pugi::xml_document *xml_file);

    void SwapFirewallWithDefault();
    void UpdateNetDevInXML(const char *devname);
    static void ResetNetDevInXML();
};

#endif

