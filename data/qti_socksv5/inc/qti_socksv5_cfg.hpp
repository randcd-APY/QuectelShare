#ifndef QTI_SOCKSV5_CFG_HPP_
#define QTI_SOCKSV5_CFG_HPP_

/*==========================================================================

  FILE:  qti_socksv5_cfg.hpp

  SERVICES:

  SOCKSv5 header file for configuration setting and parsing.

==========================================================================*/

/*==========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

==========================================================================*/

/*==========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        ----------------------------------------------------
  03/20/17   jt         SOCKSv5 support.
==========================================================================*/

#include <iostream>
#include <map>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#include "pugixml/pugixml.hpp"

//0b0001 0000 means IPV4 support, 0b0100 0000 means IPV6 support
#define IPV4_SUPPORTED 0x10
#define IPV6_SUPPORTED 0x40
#define IPV4V6_SUPPORTED (IPV4_SUPPORTED | IPV6_SUPPORTED)

//Linux sysctl tcp keepalive cmds
#define INTVL_GET_CMD "sysctl -n net.ipv4.tcp_keepalive_intvl"
#define PROBES_GET_CMD "sysctl -n net.ipv4.tcp_keepalive_probes"
#define TIME_GET_CMD "sysctl -n net.ipv4.tcp_keepalive_time"
#define INTVL_SET_CMD "sysctl -w net.ipv4.tcp_keepalive_intvl="
#define PROBES_SET_CMD "sysctl -w net.ipv4.tcp_keepalive_probes="
#define TIME_SET_CMD "sysctl -w net.ipv4.tcp_keepalive_time="

//Linux defaults for keepalive
#define LINUX_DEFAULT_KEEPALIVE_INTVL 75
#define LINUX_DEFAULT_KEEPALIVE_PROBES 9
#define LINUX_DEFAULT_KEEPALIVE_TIME 7200

enum Pref_Auth_Method
{
  NO_AUTH        = 0x00,
  GSSAPI         = 0x01,
  UNAME_PASSWD   = 0x02,
  MIN_PRIVATE    = 0x80,
  MAX_PRIVATE    = 0xFE,
  NOT_ACCEPTABLE = 0xFF,
};

struct tcp_keepalive
{
  unsigned char intvl;
  unsigned char probes;
  unsigned short time;
};

struct wan_service
{
  unsigned char ip_ver;
  char* wan_iface;
  char* pri_dns_ipv4_addr;
  char* sec_dns_ipv4_addr;
  char* pri_dns_ipv6_addr;
  char* sec_dns_ipv6_addr;
};

struct lan_service
{
  char* lan_iface;
};

//QC_SOCKSv5 proxy config object
class QC_SOCKSv5_Proxy_Configuration
{
  private:
    //0x00 means NO AUTH prefered, 0x01 means GSSAPI prefered
    //and 0x02 means USERNAME/PASSWORD preferred
    unsigned char pref_auth_method;
    struct tcp_keepalive keepalive_cfg;
    struct lan_service lan_service; //only one LAN service allowed at this point in time
    std::map<int, struct wan_service*>* wan_service_map; //key = service number, value = wan_service
    std::map<std::string, std::string>* uname_passwd_map; //key = uname, value = passwd
    std::map<std::string, int>* uname_wan_assoc_map; //key = uname, value = service_number

  public:
    QC_SOCKSv5_Proxy_Configuration(void);
    ~QC_SOCKSv5_Proxy_Configuration(void);

    unsigned char getPrefAuthMethod(void) const;
    bool setPrefAuthMethod(unsigned char pref_auth_method);

    struct tcp_keepalive getTCPKeepAlive(void);
    void setTCPKeepAliveIntvl(unsigned char intvl);
    void setTCPKeepAliveProbes(unsigned char probes);
    void setTCPKeepAliveTime(unsigned short time);

    const char* getLANIface(void) const;
    void setLANIface(const char* lan_iface);
    void deleteLANIface(void);
    int getLANIfaceIndex(void) const;

    unsigned char getNumOfWANServices(void) const;
    const struct wan_service* getWANService(int service_no) const;
    const struct wan_service* getWANService(const char* wan_iface) const;
    void insertWANService(int service_no, const char* wan_iface, const char* pri_dns_ipv4_addr,
                          const char* sec_dns_ipv4_addr, const char* pri_dns_ipv6_addr,
                          const char* sec_dns_ipv6_addr);
    void setWANIPVer(int service_no, unsigned char ip_ver);
    void setWANIPVer(const char* wan_iface, unsigned char ip_ver);
    void deleteAllWANServices(void);

    bool validateUnamePasswd(const char* uname, const char* passwd) const;
    void insertUnamePasswd(const char* uname, const char* passwd);
    void deleteAllUnamePasswds(void);

    /* For the private authentication method */
    int getUnameWANAssoc(const char* uname) const;
    void insertUnameWANAssoc(const char* uname, const int service_no);
    void deleteAllUnameWANAssocs(void);

    void printConfig(void);

};

//namepsace for config parser
namespace QC_SOCKSv5_Cfg_Parser
{
  bool validateConfigFile(char* conf_file, QC_SOCKSv5_Proxy_Configuration* configuration);
  bool validateAuthFile(char* conf_file, QC_SOCKSv5_Proxy_Configuration* configuration);
}

//static helper methods for config parser
static void extractLANIfaceConfig(pugi::xml_node node,
                                  QC_SOCKSv5_Proxy_Configuration* configuration);
static void extractWANIfaceConfig(pugi::xml_node node,
                                  QC_SOCKSv5_Proxy_Configuration* configuration);
static void extractPrefAuthMethodConfig(pugi::xml_node node,
                                        QC_SOCKSv5_Proxy_Configuration* configuration);
static void extractUnamePasswdConfig(pugi::xml_node node,
                                     QC_SOCKSv5_Proxy_Configuration* configuration);
static void extractUnameWANAssocConfig(pugi::xml_node node,
                                       QC_SOCKSv5_Proxy_Configuration* configuration);
static void extractTCPKeepAliveConfig(pugi::xml_node node,
                                      QC_SOCKSv5_Proxy_Configuration* configuration);
#endif
