/*==========================================================================

  FILE:  qti_socksv5_cfg.cpp

  SERVICES:

  SOCKSv5 implementation for configuration setting and parsing.

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

#include <net/if.h>

#include "qti_socksv5_cfg.hpp"
#include "qti_socksv5_log_msg.h"


/*==========================================================================
  FUNCTION Constructor
==========================================================================*/
/*!
@brief
  Constructs QC_SOCKSv5_Proxy_Configuration object

@parameters
  none

@return
  none
*/
/*========================================================================*/
QC_SOCKSv5_Proxy_Configuration::QC_SOCKSv5_Proxy_Configuration(void)
{
  //0x00 means NO AUTH prefered, 0x01 means GSSAPI prefered and 0x02 means USERNAME/PASSWORD
  //preferred
  this->pref_auth_method = Pref_Auth_Method::NO_AUTH;
  this->keepalive_cfg = this->getTCPKeepAlive();
  this->lan_service.lan_iface = NULL;
  this->wan_service_map = new std::map<int, struct wan_service*>();
  this->uname_passwd_map = NULL; //only instantiate if username/password is selected
  this->uname_wan_assoc_map = NULL; //only instantiate if private auth method is selected
}

/*==========================================================================
  FUNCTION Destructor
==========================================================================*/
/*!
@brief
  Destroys QC_SOCKSv5_Proxy_Configuration object

@parameters
  none

@return
  none
*/
/*========================================================================*/
QC_SOCKSv5_Proxy_Configuration::~QC_SOCKSv5_Proxy_Configuration(void)
{
  if(this->lan_service.lan_iface != NULL)
  {
    free(this->lan_service.lan_iface);
    this->lan_service.lan_iface = NULL;
  }
  if(this->wan_service_map != NULL)
  {
    this->deleteAllWANServices();
    delete this->wan_service_map;
    this->wan_service_map = NULL;
  }
  if(this->uname_passwd_map != NULL)
  {
    this->deleteAllUnamePasswds();
    delete this->uname_passwd_map;
    this->uname_passwd_map = NULL;
  }
  if(this->uname_wan_assoc_map != NULL)
  {
    this->deleteAllUnameWANAssocs();
    delete this->uname_wan_assoc_map;
    this->uname_passwd_map = NULL;
  }
}

/*==========================================================================
  FUNCTION getPrefAuthMethod
==========================================================================*/
/*!
@brief
  Gets this QC_SOCKSv5_Proxy_Configuration object's preferred authentication
  method.

@parameters
  none

@return
  preferred SOCKSv5 authentication method
*/
/*========================================================================*/
unsigned char QC_SOCKSv5_Proxy_Configuration::getPrefAuthMethod(void) const
{
  return this->pref_auth_method;
}

/*==========================================================================
  FUNCTION setPrefAuthMethod
==========================================================================*/
/*!
@brief
  Sets this QC_SOCKSv5_Proxy_Configuration object's preferred authentication
  method.

@parameters
  preferred SOCKSv5 authentication method

@return
  true - if set successful
  false - otherwise
*/
/*========================================================================*/
bool QC_SOCKSv5_Proxy_Configuration::setPrefAuthMethod(unsigned char pref_auth_method)
{
  switch(pref_auth_method)
  {
    case(Pref_Auth_Method::NO_AUTH):
    {
      this->pref_auth_method = pref_auth_method;
      return true;
    }
    case(Pref_Auth_Method::GSSAPI):
    {
      //TODO
      return false;
    }
    case(Pref_Auth_Method::UNAME_PASSWD):
    {
      this->pref_auth_method = pref_auth_method;
      return true;
    }
    default:
    {
      //RFC 1928 check for private auth method values
      if((pref_auth_method >= Pref_Auth_Method::MIN_PRIVATE) &&
         (pref_auth_method <= Pref_Auth_Method::MAX_PRIVATE))
      {
        this->pref_auth_method = pref_auth_method;
        return true;
      }
    }
  }

  return false;
}

/*==========================================================================
  FUNCTION getTCPKeepAlive
==========================================================================*/
/*!
@brief
  Gets sysctl TCP keepalive parameters.

@parameters

@return
  struct containing the tcp_keepalive parameters
*/
/*========================================================================*/
struct tcp_keepalive QC_SOCKSv5_Proxy_Configuration::getTCPKeepAlive(void)
{
  FILE* fp;
  char buffer[5]; //2^16 - 1 = 65535 so max num of characters for a short is 5

  //get interval
  if((fp = popen(INTVL_GET_CMD, "r")) <= 0)
  {
    LOG_MSG_INFO1("Error getting TCP Keepalive params: %s", strerror(errno), 0, 0);
  } else {

    if(fgets(buffer, 3, fp) == NULL)
    {
      this->keepalive_cfg.intvl = LINUX_DEFAULT_KEEPALIVE_INTVL;
    } else {
      this->keepalive_cfg.intvl = (unsigned char)atoi(buffer);
    }
    if(pclose(fp))
    {
      LOG_MSG_INFO1("error with pclose: %s", strerror(errno), 0, 0);
    }
  }


  //get probes
  if((fp = popen(PROBES_GET_CMD, "r")) <= 0)
  {
    LOG_MSG_INFO1("Error getting TCP Keepalive params: %s", strerror(errno), 0, 0);
  } else {

    if(fgets(buffer, 3, fp) == NULL)
    {
      this->keepalive_cfg.probes = LINUX_DEFAULT_KEEPALIVE_PROBES;
    } else {
      this->keepalive_cfg.probes = (unsigned char)atoi(buffer);
    }
    if(pclose(fp))
    {
      LOG_MSG_INFO1("error with pclose: %s", strerror(errno), 0, 0);
    }
  }



  //get time
  if((fp = popen(TIME_GET_CMD, "r")) <= 0)
  {
    LOG_MSG_INFO1("Error getting TCP Keepalive params: %s", strerror(errno), 0, 0);
  } else {

    if(fgets(buffer, 5, fp) == NULL)
    {
      this->keepalive_cfg.time = LINUX_DEFAULT_KEEPALIVE_TIME;
    } else {
      this->keepalive_cfg.time = (unsigned short)atoi(buffer);
    }
    if(pclose(fp))
    {
      LOG_MSG_INFO1("error with pclose: %s", strerror(errno), 0, 0);
    }
  }

  return this->keepalive_cfg;
}

/*==========================================================================
  FUNCTION setTCPKeepAliveIntvl
==========================================================================*/
/*!
@brief
  Sets sysctl TCP keepalive interval parameter.

@parameters
  TCP keepalive interval to set

@return

*/
/*========================================================================*/
void QC_SOCKSv5_Proxy_Configuration::setTCPKeepAliveIntvl(unsigned char intvl)
{
  FILE* fp;
  unsigned char intv1_arr[2] = {0};
  char* intv1_cmd = (char *) calloc(strlen(INTVL_SET_CMD)+sizeof(intv1_arr)+1,sizeof(char));

  if(!intv1_cmd)
   return;

  strlcat(intv1_cmd, INTVL_SET_CMD, sizeof(intv1_cmd));
  intv1_arr[0] = intvl;
  strlcat(intv1_cmd, intv1_arr, sizeof(intv1_cmd));

  //get interval
  if((fp = popen(intv1_cmd, "w")) <= 0)
  {
    LOG_MSG_INFO1("Error setting TCP Keepalive params: %s", strerror(errno), 0, 0);
  } else {
    if(pclose(fp))
    {
      LOG_MSG_INFO1("error with pclose: %s", strerror(errno), 0, 0);
    }
  }


  this->keepalive_cfg.intvl = intvl;
}

/*==========================================================================
  FUNCTION setTCPKeepAliveProbes
==========================================================================*/
/*!
@brief
  Sets sysctl TCP keepalive probe parameter.

@parameters
  Number of TCP keepalive probes to set

@return

*/
/*========================================================================*/
void QC_SOCKSv5_Proxy_Configuration::setTCPKeepAliveProbes(unsigned char probes)
{
  FILE* fp;
  unsigned char probes_arr[2] = {0};
  char* probes_cmd = (char *) calloc(strlen(PROBES_SET_CMD)+sizeof(probes_arr)+1,sizeof(char));

  if(!probes_cmd)
    return;

  strlcat(probes_cmd, PROBES_SET_CMD, sizeof(probes_cmd));
  probes_arr[0] = probes;
  strlcat(probes_cmd, probes_arr, sizeof(probes_cmd));

  //get interval
  if((fp = popen(probes_cmd, "w")) <= 0)
  {
    LOG_MSG_INFO1("Error setting TCP Keepalive params: %s", strerror(errno), 0, 0);
  } else {
    if(pclose(fp))
    {
      LOG_MSG_INFO1("error with pclose: %s", strerror(errno), 0, 0);
    }
  }

  this->keepalive_cfg.probes = probes;
}

/*==========================================================================
  FUNCTION setTCPKeepAliveTime
==========================================================================*/
/*!
@brief
  Sets sysctl TCP keepalive time parameter.

@parameters
  Time of TCP keepalive to set

@return

*/
/*========================================================================*/
void QC_SOCKSv5_Proxy_Configuration::setTCPKeepAliveTime(unsigned short time)
{
  FILE* fp;
  unsigned char time_arr[2] = {0};
  char* time_cmd = (char *) calloc(strlen(TIME_SET_CMD)+sizeof(time_arr)+1,sizeof(char));

   if(!time_cmd)
     return;

   strlcat(time_cmd, TIME_SET_CMD, sizeof(time_cmd));
   time_arr[0] = time;
   strlcat(time_cmd, time_arr, sizeof(time_cmd));

  //get interval
  if((fp = popen(time_cmd, "w")) <= 0)
  {
    LOG_MSG_INFO1("Error setting TCP Keepalive params: %s", strerror(errno), 0, 0);
  } else {
    if(pclose(fp))
    {
      LOG_MSG_INFO1("error with pclose: %s", strerror(errno), 0, 0);
    }
  }

  this->keepalive_cfg.time = time;
}

/*==========================================================================
  FUNCTION getLANIface
==========================================================================*/
/*!
@brief
  gets this QC_SOCKSv5_Proxy_Configuratin's lan iface

@parameters

@return
  lan iface name that this SOCKSv5 proxy binds to
*/
/*========================================================================*/
const char* QC_SOCKSv5_Proxy_Configuration::getLANIface(void) const
{
  return this->lan_service.lan_iface;
}

/*==========================================================================
  FUNCTION setLANIface
==========================================================================*/
/*!
@brief
  sets this QC_SOCKSv5_Proxy_Configuratin's lan iface

@parameters
  lan iface name that this SOCKSv5 proxy binds to

@return

*/
/*========================================================================*/
void QC_SOCKSv5_Proxy_Configuration::setLANIface(const char* lan_iface)
{
  if(strlen(lan_iface) > 0)
  {
    //plus 1 for \0 character
    if((this->lan_service.lan_iface = (char*)calloc(1, (strlen(lan_iface) * sizeof(char)) + 1))
       == NULL)
    {
      LOG_MSG_INFO1("Error with calloc setLANIface", 0, 0, 0);
      return;
    }

    //do not change this to strcpy, strcpy does not work here
    for(int i = 0; i < strlen(lan_iface); i++)
    {
      this->lan_service.lan_iface[i] = lan_iface[i];
    }
  }
}

void QC_SOCKSv5_Proxy_Configuration::deleteLANIface(void)
{
  free(this->lan_service.lan_iface);
  this->lan_service.lan_iface = NULL;
  //this->lan_service.ip_ver = 0;
}

int QC_SOCKSv5_Proxy_Configuration::getLANIfaceIndex(void) const
{
  if(NULL == this->lan_service.lan_iface)
  {
    return 0;
  } else {
    return if_nametoindex(this->lan_service.lan_iface);
  }

  return 0;
}

/*==========================================================================
  FUNCTION getNumOfWANServices
==========================================================================*/
/*!
@brief
  gets number of WAN services for this QC_SOCKSv5_Proxy_Configuration

@parameters

@return
  size of the pdn_map
*/
/*========================================================================*/
unsigned char QC_SOCKSv5_Proxy_Configuration::getNumOfWANServices(void) const
{
  return this->wan_service_map->size();
}

/*==========================================================================
  FUNCTION getWANService
==========================================================================*/
/*!
@brief
  get WAN service struct by service_no

@parameters

@return
  ptr to the wan service or NULL if service no doesn't exist
*/
/*========================================================================*/
const struct wan_service* QC_SOCKSv5_Proxy_Configuration::getWANService(int service_no) const
{
  std::map<int, struct wan_service*>::iterator it;

  if((it = this->wan_service_map->find(service_no)) == this->wan_service_map->end())
  {
    LOG_MSG_INFO1("WAN Service # %d doesn't exist", service_no, 0, 0);
    return NULL;
  } else {

    return it->second;
  }

  return NULL;
}

/*==========================================================================
  FUNCTION getWANService
==========================================================================*/
/*!
@brief
  get WAN service struct by wan_iface name

@parameters

@return
  ptr to the wan service or NULL if wan_iface name doesn't exist
*/
/*========================================================================*/
const struct wan_service* QC_SOCKSv5_Proxy_Configuration::getWANService(const char* wan_iface) const
{
  if(NULL == wan_iface)
  {
    LOG_MSG_INFO1("null while getWANService", 0, 0, 0);
    return NULL;
  }

  //only returning the first macthing wan iface name
  for(std::map<int, struct wan_service*>::iterator it = this->wan_service_map->begin();
      it != this->wan_service_map->end(); ++it)
  {
    if(strcmp(it->second->wan_iface, wan_iface) == 0)
    {
      return it->second;
    }
  }
  return NULL;
}

/*==========================================================================
  FUNCTION insertWANService
==========================================================================*/
/*!
@brief
  inserts WAN service struct into pdn_map

@parameters
  service_no
  wan_iface name
  pri_dns_ipv4_addr for the wan service
  sec_dns_ipv4_addr for the wan service
  pri_dns_ipv6_addr for the wan service
  sec_dns_ipv6_addr for the wan service

@return

*/
/*========================================================================*/
void QC_SOCKSv5_Proxy_Configuration::insertWANService
(
int service_no,
const char* wan_iface,
const char* pri_dns_ipv4_addr,
const char* sec_dns_ipv4_addr,
const char* pri_dns_ipv6_addr,
const char* sec_dns_ipv6_addr
)
{
  struct wan_service* pdn;
  const char* temp_pri_dns_ipv4_addr = "";
  const char* temp_sec_dns_ipv4_addr = "";
  const char* temp_pri_dns_ipv6_addr = "";
  const char* temp_sec_dns_ipv6_addr = "";
  std::pair<std::map<int, struct wan_service*>::iterator, bool> ret;

  if(NULL == wan_iface)
  {
    LOG_MSG_INFO1("error with inserting wan service, wan_iface is NULL!", 0, 0, 0);
    return;
  }

  if(NULL == pri_dns_ipv4_addr)
  {
    LOG_MSG_INFO1("pri_dns_ipv4_addr is empty", 0, 0, 0);
  } else {
    temp_pri_dns_ipv4_addr = pri_dns_ipv4_addr;
  }

  if(NULL == sec_dns_ipv4_addr)
  {
    LOG_MSG_INFO1("sec_dns_ipv4_addr is empty", 0, 0, 0);
  } else {
    temp_sec_dns_ipv4_addr = sec_dns_ipv4_addr;
  }

  if(NULL == pri_dns_ipv6_addr)
  {
    LOG_MSG_INFO1("pri_dns_ipv6_addr is empty", 0, 0, 0);
  } else {
    temp_pri_dns_ipv6_addr = pri_dns_ipv6_addr;
  }

  if(NULL == sec_dns_ipv6_addr)
  {
    LOG_MSG_INFO1("sec_dns_ipv6_addr is empty", 0, 0, 0);
  } else {
    temp_sec_dns_ipv6_addr = sec_dns_ipv6_addr;
  }

  //calloc takes care of setting ip_ver to 0
  if((pdn = (struct wan_service*)calloc(1, sizeof(struct wan_service))) == NULL)
  {
    LOG_MSG_INFO1("error with calloc insertWANService", 0, 0, 0);
    return;
  }

  //allocate on heap the wan_iface name
  if((pdn->wan_iface = (char*)calloc(1, (strlen(wan_iface) * sizeof(char)) + 1)) == NULL)
  {
    LOG_MSG_INFO1("error with calloc insertWANService", 0, 0, 0);
    free(pdn);
    return;
  }

  //do not change this to strcpy, don't trust it
  for(int i = 0; i < strlen(wan_iface); i++)
  {
    pdn->wan_iface[i] = wan_iface[i];
  }

  //allocate on heap the dns server ip for this wan service
  if((pdn->pri_dns_ipv4_addr = (char*)calloc(1,
     (strlen(temp_pri_dns_ipv4_addr) * sizeof(char)) + 1)) == NULL)
  {
    LOG_MSG_INFO1("error with calloc insertWANService", 0, 0, 0);
    free(pdn->wan_iface);
    free(pdn);
    return;
  }

  //do not change this to strcpy, don't trust it
  for(int i = 0; i < strlen(temp_pri_dns_ipv4_addr); i++)
  {
    pdn->pri_dns_ipv4_addr[i] = temp_pri_dns_ipv4_addr[i];
  }

  //allocate on heap the dns server ip for this wan service
  if((pdn->sec_dns_ipv4_addr = (char*)calloc(1,
     (strlen(temp_sec_dns_ipv4_addr) * sizeof(char)) + 1)) == NULL)
  {
    LOG_MSG_INFO1("error with calloc insertWANService", 0, 0, 0);
    free(pdn->pri_dns_ipv4_addr);
    free(pdn->wan_iface);
    free(pdn);
    return;
  }

  //do not change this to strcpy, don't trust it
  for(int i = 0; i < strlen(temp_sec_dns_ipv4_addr); i++)
  {
    pdn->sec_dns_ipv4_addr[i] = temp_sec_dns_ipv4_addr[i];
  }

  //allocate on heap the dns server ip for this wan service
  if((pdn->pri_dns_ipv6_addr = (char*)calloc(1,
     (strlen(temp_pri_dns_ipv6_addr) * sizeof(char)) + 1)) == NULL)
  {
    LOG_MSG_INFO1("error with calloc insertWANService", 0, 0, 0);
    free(pdn->sec_dns_ipv4_addr);
    free(pdn->pri_dns_ipv4_addr);
    free(pdn->wan_iface);
    free(pdn);
    return;
  }

  //do not change this to strcpy, don't trust it
  for(int i = 0; i < strlen(temp_pri_dns_ipv6_addr); i++)
  {
    pdn->pri_dns_ipv6_addr[i] = temp_pri_dns_ipv6_addr[i];
  }

  //allocate on heap the dns server ip for this wan service
  if((pdn->sec_dns_ipv6_addr = (char*)calloc(1,
     (strlen(temp_sec_dns_ipv6_addr) * sizeof(char)) + 1)) == NULL)
  {
    LOG_MSG_INFO1("error with calloc insertWANService", 0, 0, 0);
    free(pdn->pri_dns_ipv6_addr);
    free(pdn->sec_dns_ipv4_addr);
    free(pdn->pri_dns_ipv4_addr);
    free(pdn->wan_iface);
    free(pdn);
    return;
  }

  //do not change this to strcpy, don't trust it
  for(int i = 0; i < strlen(temp_sec_dns_ipv6_addr); i++)
  {
    pdn->sec_dns_ipv6_addr[i] = temp_sec_dns_ipv6_addr[i];
  }

  ret = this->wan_service_map->insert(std::pair<int, struct wan_service*>(service_no, pdn));

  //check if a previous wan service was already assigned to the service_no
  if(!ret.second)
  {
    LOG_MSG_INFO1("Already assigned Service # %u to %s\n", service_no,
                  ret.first->second->wan_iface, 0);
    free(pdn->pri_dns_ipv6_addr);
    free(pdn->sec_dns_ipv6_addr);
    free(pdn->pri_dns_ipv4_addr);
    free(pdn->sec_dns_ipv4_addr);
    free(pdn->wan_iface);
    free(pdn);
  }
}

/*==========================================================================
  FUNCTION setWANIPVer
==========================================================================*/
/*!
@brief
  sets WAN service ip version by service_no

@parameters
  service_no
  ip version to set (either ipv4 or ipv6)

@return

*/
/*========================================================================*/
void QC_SOCKSv5_Proxy_Configuration::setWANIPVer(int service_no, unsigned char ip_ver)
{
  std::map<int, struct wan_service*>::iterator it;

  if((it = this->wan_service_map->find(service_no)) == this->wan_service_map->end())
  {
    LOG_MSG_INFO1("WAN Service # %d doesn't exit", service_no, 0, 0);
    return;

  } else {
    it->second->ip_ver |= ip_ver;
  }
}

/*==========================================================================
  FUNCTION setWANIPVer
==========================================================================*/
/*!
@brief
  sets WAN service ip version by wan iface name

@parameters
  wan_iface
  ip version to set (either ipv4 or ipv6)

@return

*/
/*========================================================================*/
void QC_SOCKSv5_Proxy_Configuration::setWANIPVer(const char* wan_iface, unsigned char ip_ver)
{
  if(NULL == wan_iface)
  {
    LOG_MSG_INFO1("wan_iface given is null", 0, 0, 0);
    return;
  }

  for(std::map<int, struct wan_service*>::iterator it = this->wan_service_map->begin();
      it != this->wan_service_map->end(); ++it)
  {
    if(strcmp(it->second->wan_iface, wan_iface) == 0)
    {
      it->second->ip_ver |= ip_ver;
    }
  }
}

/*==========================================================================
  FUNCTION deleteAllWANServices
==========================================================================*/
/*!
@brief
  deletes everything in pdn_map

@parameters

@return

*/
/*========================================================================*/
void QC_SOCKSv5_Proxy_Configuration::deleteAllWANServices(void)
{
  //free and then clear
  for(std::map<int, struct wan_service*>::iterator it = this->wan_service_map->begin();
      it != this->wan_service_map->end(); ++it)
  {
    if(it->second->wan_iface != NULL)
    {
      free(it->second->wan_iface);
      it->second->wan_iface = NULL;
    }
    if(it->second->pri_dns_ipv4_addr != NULL)
    {
      free(it->second->pri_dns_ipv4_addr);
      it->second->pri_dns_ipv4_addr = NULL;
    }
    if(it->second->sec_dns_ipv4_addr != NULL)
    {
      free(it->second->sec_dns_ipv4_addr);
      it->second->sec_dns_ipv4_addr = NULL;
    }
    if(it->second->pri_dns_ipv6_addr != NULL)
    {
      free(it->second->pri_dns_ipv6_addr);
      it->second->pri_dns_ipv6_addr = NULL;
    }
    if(it->second->sec_dns_ipv6_addr != NULL)
    {
      free(it->second->sec_dns_ipv6_addr);
      it->second->sec_dns_ipv6_addr = NULL;
    }
    free(it->second);
    it->second = NULL;
  }
  this->wan_service_map->clear();
}

/*==========================================================================
  FUNCTION validateUnamePasswd
==========================================================================*/
/*!
@brief
  validates the given username password with configuration

@parameters
  uname
  passwd
@return
  true - validated uname and passwd
  false - otherwise
*/
/*========================================================================*/
bool QC_SOCKSv5_Proxy_Configuration::validateUnamePasswd
(
const char* uname,
const char* passwd
) const
{
  std::map<std::string, std::string>::iterator it;

  if(NULL == uname)
  {
    LOG_MSG_INFO1("given null uname", 0, 0, 0);
    return false;
  } else if(NULL == passwd)
  {
    LOG_MSG_INFO1("given null passwd", 0, 0, 0);
    return false;
  }

  if((it = this->uname_passwd_map->find(std::string(uname))) == this->uname_passwd_map->end())
  {
    LOG_MSG_INFO1("Invalid UNAME/PASSWD", 0, 0, 0);
    return false;
  } else if(it->second.compare(passwd) == 0) {

    return true;
  }

  LOG_MSG_INFO1("Invalid UNAME/PASSWD", 0, 0, 0);
  return false;
}

/*==========================================================================
  FUNCTION insertUnamePasswdAssoc
==========================================================================*/
/*!
@brief
  inserts the given username password into configuration

@parameters
  uname
  passwd
@return
*/
/*========================================================================*/
void QC_SOCKSv5_Proxy_Configuration::insertUnamePasswd(const char* uname, const char* passwd)
{

  if(NULL == uname)
  {
    LOG_MSG_INFO1("given null uname", 0, 0, 0);
    return;
  } else if(NULL == passwd)
  {
    LOG_MSG_INFO1("given null passwd", 0, 0, 0);
    return;
  }

  if(NULL == this->uname_passwd_map)
  {
    this->uname_passwd_map = new std::map<std::string, std::string>();
  }

  if(strcmp(uname, "") && strcmp(passwd, ""))
  {
    this->uname_passwd_map->insert(std::pair<std::string, std::string>(std::string(uname),
                                   std::string(passwd)));
  }
}

/*==========================================================================
  FUNCTION deleteAllUnamePasswds
==========================================================================*/
/*!
@brief
  deletes all username password mappings

@parameters

@return

*/
/*========================================================================*/
void QC_SOCKSv5_Proxy_Configuration::deleteAllUnamePasswds(void)
{
  if(this->uname_passwd_map != NULL)
  {
    this->uname_passwd_map->clear();
  }
}

/*==========================================================================
  FUNCTION getUnameWANAssoc
==========================================================================*/
/*!
@brief
  gets service no mapping based off username

@parameters
  uname

@return
  service_no
  -1, if invalid uname

*/
/*========================================================================*/
int QC_SOCKSv5_Proxy_Configuration::getUnameWANAssoc(const char* uname) const
{
  std::map<std::string, int>::iterator it;

  if(NULL == uname)
  {
    LOG_MSG_INFO1("given null uname", 0, 0, 0);
    return -1;
  }

  if((it = this->uname_wan_assoc_map->find(std::string(uname))) == this->uname_wan_assoc_map->end())
  {
    LOG_MSG_INFO1("Invalid UNAME", 0, 0, 0);
    return -1;
  }

  return it->second;

}

/*==========================================================================
  FUNCTION insertUnameWANAssoc
==========================================================================*/
/*!
@brief
  gets service no mapping based off username

@parameters
  uname
  service_no

@return

*/
/*========================================================================*/
void QC_SOCKSv5_Proxy_Configuration::insertUnameWANAssoc(const char* uname, const int service_no)
{

  if(NULL == uname)
  {
    LOG_MSG_INFO1("given null uname", 0, 0, 0);
    return;
  }

  if(NULL == this->uname_wan_assoc_map)
  {
    this->uname_wan_assoc_map = new std::map<std::string, int>();
  }

  if(strcmp(uname, "") && (service_no >= 0))
  {
    this->uname_wan_assoc_map->insert(std::pair<std::string, int>(std::string(uname), service_no));
  }
}

/*==========================================================================
  FUNCTION deleteAllUnameWANAssocs
==========================================================================*/
/*!
@brief
  deletes all username wan service mappings

@parameters

@return

*/
/*========================================================================*/
void QC_SOCKSv5_Proxy_Configuration::deleteAllUnameWANAssocs(void)
{
  if(this->uname_wan_assoc_map != NULL)
  {
    this->uname_wan_assoc_map->clear();
  }
}

/*==========================================================================
  FUNCTION printConfig
==========================================================================*/
/*!
@brief
  prints to diag logs the SOCKSv5 Proxy configuration

@parameters

@return

*/
/*========================================================================*/
void QC_SOCKSv5_Proxy_Configuration::printConfig(void)
{
  LOG_MSG_INFO1("LAN Iface: %s", this->lan_service.lan_iface, 0, 0);

  LOG_MSG_INFO1("Preferred Auth Method:  ", 0, 0, 0);
  switch(this->pref_auth_method)
  {
    case(Pref_Auth_Method::NO_AUTH):
    {
      LOG_MSG_INFO1("NO_AUTHENTICATION", 0, 0, 0);
      break;
    }
    case(Pref_Auth_Method::GSSAPI):
    {
      LOG_MSG_INFO1("GSSAPI", 0, 0, 0);
      break;
    }
    case(Pref_Auth_Method::UNAME_PASSWD):
    {
      LOG_MSG_INFO1("USERNAME/PASSWORD", 0, 0, 0);
      break;
    }
    default:
    {
      if((this->pref_auth_method >= Pref_Auth_Method::MIN_PRIVATE) &&
         (this->pref_auth_method <= Pref_Auth_Method::MAX_PRIVATE))
      {
        LOG_MSG_INFO1("PRIVATE_METHOD: %u", this->pref_auth_method, 0, 0);

      } else {
        LOG_MSG_INFO1("NOT SUPPORTED!", 0, 0, 0);
      }

      break;
    }
  }

  for(std::map<int, struct wan_service*>::iterator it = this->wan_service_map->begin();
      it != this->wan_service_map->end(); ++it)
  {
    LOG_MSG_INFO1("Service # %u assigned to WAN iface: %s\n", it->first, it->second->wan_iface, 0);

    if((NULL != it->second->wan_iface) && (strlen(it->second->wan_iface) > 0))
    {
      switch(it->second->ip_ver)
      {
        case(IPV4_SUPPORTED):
        {
          LOG_MSG_INFO1("WAN IP Version: IPV4", 0, 0, 0);
          break;
        }
        case(IPV6_SUPPORTED):
        {
          LOG_MSG_INFO1("WAN IP Version: IPV6", 0, 0, 0);
          break;
        }
        case(IPV4V6_SUPPORTED):
        {
          LOG_MSG_INFO1("WAN IP Version: IPV4V6", 0, 0, 0);
          break;
        }
        default:
        {
          LOG_MSG_INFO1("No valid IP version provided", 0, 0, 0);
          break;
        }
      }

      if((NULL != it->second->pri_dns_ipv4_addr) && (strlen(it->second->pri_dns_ipv4_addr) > 0))
      {
        LOG_MSG_INFO1("WAN iface: %s using Primary DNS IPv4 Addr %s\n", it->second->wan_iface,
                      it->second->pri_dns_ipv4_addr, 0);
      }
      if((NULL != it->second->sec_dns_ipv4_addr) && (strlen(it->second->sec_dns_ipv4_addr) > 0))
      {
        LOG_MSG_INFO1("WAN iface: %s using Secondary DNS IPv4 Addr %s\n", it->second->wan_iface,
                      it->second->sec_dns_ipv4_addr, 0);
      }
      if((NULL != it->second->pri_dns_ipv6_addr) && (strlen(it->second->pri_dns_ipv6_addr) > 0))
      {
        LOG_MSG_INFO1("WAN iface: %s using Primary DNS IPv6 Addr %s\n", it->second->wan_iface,
                      it->second->pri_dns_ipv6_addr, 0);
      }
      if((NULL != it->second->sec_dns_ipv6_addr) && (strlen(it->second->sec_dns_ipv6_addr) > 0))
      {
        LOG_MSG_INFO1("WAN iface: %s using Secondary DNS IPv6 Addr %s\n", it->second->wan_iface,
                      it->second->sec_dns_ipv6_addr, 0);
      }
    }
  }

  LOG_MSG_INFO1("TCP Keepalive Parameters: ", 0, 0, 0);
  LOG_MSG_INFO1("Interval: %u\n", this->keepalive_cfg.intvl, 0, 0);
  LOG_MSG_INFO1("Probes: %u\n", this->keepalive_cfg.probes, 0, 0);
  LOG_MSG_INFO1("Time: %u\n", this->keepalive_cfg.time, 0, 0);
}

/*==========================================================================
  Namespace definitions for the SOCKSv5 XML parser
==========================================================================*/
namespace QC_SOCKSv5_Cfg_Parser
{

  /*==========================================================================
    FUNCTION validateConfigFile
  ==========================================================================*/
  /*!
  @brief
    validates the SOCKSv5 Proxy configuration to have all necessary parameters
    to start SOCKSv5 sessions

  @parameters
    configuration file path
    QC_SOCKSv5_Proxy_Configuration object

  @return
    true - valid configuration
    false - otherwise

  */
  /*========================================================================*/
  bool validateConfigFile(char* conf_file, QC_SOCKSv5_Proxy_Configuration* configuration)
  {
    pugi::xml_document doc;
    pugi::xml_node node;

    if(!doc.load_file(conf_file))
    {
      LOG_MSG_INFO1("Error loading the config file: %s", conf_file, 0, 0);
      return false;
    }

    if(NULL == configuration)
    {
      LOG_MSG_INFO1("give null configuration object", 0, 0, 0);
      return false;
    }

    //extract lan iface configuration
    node = doc.child("PROXY_CONFIG").child("LAN_CFG");
    extractLANIfaceConfig(node, configuration);
    if(configuration->getLANIface() == NULL) //check at least the LAN iface was given
    {
      LOG_MSG_INFO1("Error in config file, no LAN iface specified", 0, 0, 0);
      return false;
    }

    //extract wan iface configuration
    node = doc.child("PROXY_CONFIG").child("WAN_CFG");
    extractWANIfaceConfig(node, configuration);
    if(configuration->getNumOfWANServices() == 0) //check at least one WAN iface given
    {
      LOG_MSG_INFO1("No WAN iface specified", 0, 0, 0);
    }

    //extract TCP Keep Alive Timeout configuration
    node = doc.child("PROXY_CONFIG").child("TCP_KEEP_ALIVE_TIME_CFG");
    extractTCPKeepAliveConfig(node, configuration);

    //configuration->printConfig();

    return true;
  }

  /*==========================================================================
    FUNCTION validateAuthFile
  ==========================================================================*/
  /*!
  @brief
    validates the SOCKSv5 Proxy authentication file to have all necessary parameters
    to start SOCKSv5 sessions

  @parameters
    authentication file path
    QC_SOCKSv5_Proxy_Configuration object

  @return
    true - valid configuration
    false - otherwise

  */
  /*========================================================================*/
  bool validateAuthFile(char* auth_file, QC_SOCKSv5_Proxy_Configuration* configuration)
  {
    pugi::xml_document doc;
    pugi::xml_node node;

    if(!doc.load_file(auth_file))
    {
      LOG_MSG_INFO1("Error loading the auth file: %s", auth_file, 0, 0);
      return false;
    }

    if(NULL == configuration)
    {
      LOG_MSG_INFO1("give null configuration object", 0, 0, 0);
      return false;
    }

    //extract SOCKS auth method configuration
    node = doc.child("PROXY_CONFIG").child("SOCKSV5_AUTH_METHOD_CFG");
    extractPrefAuthMethodConfig(node, configuration);

    //special case of auth method
    if(configuration->getPrefAuthMethod() == Pref_Auth_Method::UNAME_PASSWD)
    {
      //extract UNAME WAN Service Mapping
      node = doc.child("PROXY_CONFIG").child("USERNAME_PASSWORD_CFG");
      extractUnameWANAssocConfig(node, configuration);
    }

    return true;
  }
}


/*==========================================================================
  Static helper functions for the SOCKSv5 XML config/auth file parser
==========================================================================*/

/*==========================================================================
  FUNCTION extractLANIfaceConfig
==========================================================================*/
/*!
@brief
  parses xml file for lan iface given the node and updates SOCKSv5 Proxy configuration

@parameters
  xml node of LAN_CFG
  SOCKSv5 configuration object

@return

*/
/*========================================================================*/
static void extractLANIfaceConfig
(
pugi::xml_node node,
QC_SOCKSv5_Proxy_Configuration* configuration
)
{
  if(NULL == configuration)
  {
    LOG_MSG_INFO1("give null configuration object", 0, 0, 0);
    return;
  }

  configuration->setLANIface(node.child("LAN_IFACE").child_value());
}

/*==========================================================================
  FUNCTION extractWANIfaceConfig
==========================================================================*/
/*!
@brief
  parses xml file for wan ifaces given the node and updates SOCKSv5 Proxy configuration

@parameters
  xml node of WAN_CFG
  SOCKSv5 configuration object

@return

*/
/*========================================================================*/
static void extractWANIfaceConfig
(
pugi::xml_node node,
QC_SOCKSv5_Proxy_Configuration* configuration
)
{
  unsigned char temp_ip_ver;
  char* temp_name;
  char* temp_profile;
  char* temp_pri_dns_ipv4;
  char* temp_sec_dns_ipv4;
  char* temp_pri_dns_ipv6;
  char* temp_sec_dns_ipv6;

  if(NULL == configuration)
  {
    LOG_MSG_INFO1("give null configuration object", 0, 0, 0);
    return;
  }

  for(pugi::xml_node child = node.first_child(); child != NULL; child = child.next_sibling())
  {
    temp_ip_ver = 0;
    temp_name = NULL;
    temp_profile = NULL;
    temp_pri_dns_ipv4 = NULL;
    temp_sec_dns_ipv4 = NULL;
    temp_pri_dns_ipv6 = NULL;
    temp_sec_dns_ipv6 = NULL;

    if(strcmp(child.name(), "WAN_IFACE") == 0)
    {
      for(pugi::xml_node grand_child = child.first_child(); grand_child != NULL;
          grand_child = grand_child.next_sibling())
      {

        if((strcmp(grand_child.name(), "NAME") == 0) && (NULL == temp_name))
        {
          if((temp_name = (char*)calloc(1, (strlen(grand_child.child_value()) * sizeof(char)) + 1))
              == NULL)
          {
            LOG_MSG_INFO1("error with calloc extractWANIfaceConfig", 0, 0, 0);
            break;
          }
          for(int i = 0; i < strlen(grand_child.child_value()); i++)
          {
            temp_name[i] = (grand_child.child_value())[i];
          }
        }
        if((strcmp(grand_child.name(), "PROFILE") == 0) && (NULL == temp_profile))
        {
          if((temp_profile = (char*)calloc(1,
                                   (strlen(grand_child.child_value()) * sizeof(char)) + 1)) == NULL)
          {
            LOG_MSG_INFO1("error with calloc extractWANIfaceConfig", 0, 0, 0);
            break;
          }
          for(int i = 0; i < strlen(grand_child.child_value()); i++)
          {
            temp_profile[i] = (grand_child.child_value())[i];
          }
        }
        if((strcmp(grand_child.name(), "PRI_DNS_IPV4_ADDR") == 0) && (NULL == temp_pri_dns_ipv4))
        {
          if((temp_pri_dns_ipv4 = (char*)calloc(1,
                                   (strlen(grand_child.child_value()) * sizeof(char)) + 1)) == NULL)
          {
            LOG_MSG_INFO1("error with calloc extractWANIfaceConfig", 0, 0, 0);
            break;
          }
          for(int i = 0; i < strlen(grand_child.child_value()); i++)
          {
            temp_pri_dns_ipv4[i] = (grand_child.child_value())[i];
          }
        }
        if((strcmp(grand_child.name(), "SEC_DNS_IPV4_ADDR") == 0) && (NULL == temp_sec_dns_ipv4))
        {
          if((temp_sec_dns_ipv4 = (char*)calloc(1,
                                   (strlen(grand_child.child_value()) * sizeof(char)) + 1)) == NULL)
          {
            LOG_MSG_INFO1("error with calloc extractWANIfaceConfig", 0, 0, 0);
            break;
          }
          for(int i = 0; i < strlen(grand_child.child_value()); i++)
          {
            temp_sec_dns_ipv4[i] = (grand_child.child_value())[i];
          }
        }
        if((strcmp(grand_child.name(), "PRI_DNS_IPV6_ADDR") == 0) && (NULL == temp_pri_dns_ipv6))
        {
          if((temp_pri_dns_ipv6 = (char*)calloc(1,
                                   (strlen(grand_child.child_value()) * sizeof(char)) + 1)) == NULL)
          {
            LOG_MSG_INFO1("error with calloc extractWANIfaceConfig", 0, 0, 0);
            break;
          }
          for(int i = 0; i < strlen(grand_child.child_value()); i++)
          {
            temp_pri_dns_ipv6[i] = (grand_child.child_value())[i];
          }
        }
        if((strcmp(grand_child.name(), "SEC_DNS_IPV6_ADDR") == 0) && (NULL == temp_sec_dns_ipv6))
        {
          if((temp_sec_dns_ipv6 = (char*)calloc(1,
                                   (strlen(grand_child.child_value()) * sizeof(char)) + 1)) == NULL)
          {
            LOG_MSG_INFO1("error with calloc extractWANIfaceConfig", 0, 0, 0);
            break;
          }
          for(int i = 0; i < strlen(grand_child.child_value()); i++)
          {
            temp_sec_dns_ipv6[i] = (grand_child.child_value())[i];
          }
        }
        if((strcmp(grand_child.name(), "IP") == 0) &&
           (strcmp(grand_child.child_value(), "IPV4") == 0))
        {
          temp_ip_ver = IPV4_SUPPORTED;
        }
        if((strcmp(grand_child.name(), "IP") == 0) &&
           (strcmp(grand_child.child_value(), "IPV6") == 0))
        {
          temp_ip_ver = IPV6_SUPPORTED;
        }
        if((strcmp(grand_child.name(), "IP") == 0) &&
           (strcmp(grand_child.child_value(), "IPV4V6") == 0))
        {
          temp_ip_ver = IPV4V6_SUPPORTED;
        }
      }

      if((temp_name != NULL) && (strlen(temp_name) > 0) && (strlen(temp_name) < IFNAMSIZ))
      {
        //given service no?
        if(temp_profile != NULL)
        {
          configuration->insertWANService((unsigned char)atoi(temp_profile), temp_name,
                                            temp_pri_dns_ipv4, temp_sec_dns_ipv4,
                                            temp_pri_dns_ipv6, temp_sec_dns_ipv6);
          configuration->setWANIPVer((unsigned char)atoi(temp_profile), temp_ip_ver);
        } else {
          LOG_MSG_INFO1("Not given a service no for wan iface: %s", temp_name, 0, 0);
        }
      }

      free(temp_profile);
      free(temp_name);
      free(temp_pri_dns_ipv4);
      free(temp_sec_dns_ipv4);
      free(temp_pri_dns_ipv6);
      free(temp_sec_dns_ipv6);
    }
  }
}

/*==========================================================================
  FUNCTION extractPrefAuthMethod
==========================================================================*/
/*!
@brief
  parses xml file for SOCKSv5 pref auth method and updates SOCKSv5 Proxy configuration

@parameters
  xml node of SOCKSV5_AUTH_METHOD_CFG
  SOCKSv5 configuration object

@return

*/
/*========================================================================*/
static void extractPrefAuthMethodConfig
(
  pugi::xml_node node,
  QC_SOCKSv5_Proxy_Configuration* configuration
)
{
  if(NULL == configuration)
  {
    LOG_MSG_INFO1("give null configuration object", 0, 0, 0);
    return;
  }

  if(strcmp(node.child("SOCKSV5_AUTH_METHOD").child_value(), "NO_AUTHENTICATION") == 0)
  {
    configuration->setPrefAuthMethod(Pref_Auth_Method::NO_AUTH);
  } else if(strcmp(node.child("SOCKSV5_AUTH_METHOD").child_value(), "USERNAME_PASSWORD") == 0) {
    configuration->setPrefAuthMethod(Pref_Auth_Method::UNAME_PASSWD);
  }
}

/*==========================================================================
  FUNCTION extractUnamePasswdConfig
==========================================================================*/
/*!
@brief
  parses xml file for SOCKSv5 uname and passwds and updates SOCKSv5 Proxy configuration

@parameters
  xml node of USERNAME_PASSWORD_CFG
  SOCKSv5 configuration object

@return

*/
/*========================================================================*/
static void extractUnamePasswdConfig
(
  pugi::xml_node node,
  QC_SOCKSv5_Proxy_Configuration* configuration
)
{
  char* temp_uname;
  char* temp_passwd;

  if(NULL == configuration)
  {
    LOG_MSG_INFO1("give null configuration object", 0, 0, 0);
    return;
  }

  for(pugi::xml_node child = node.first_child(); child != NULL; child = child.next_sibling())
  {
    temp_uname = NULL;
    temp_passwd = NULL;

    if(strcmp(child.name(), "USER") == 0)
    {
      for(pugi::xml_node grand_child = child.first_child(); grand_child != NULL;
          grand_child = grand_child.next_sibling())
      {
        if((strcmp(grand_child.name(), "NAME") == 0) && (NULL == temp_uname))
        {
          if((temp_uname =
                  (char*)calloc(1, (strlen(grand_child.child_value()) * sizeof(char)) + 1)) == NULL)
          {
            LOG_MSG_INFO1("error with calloc extractUnamePasswdConfig", 0, 0, 0);
            break;
          }
          for(int i = 0; i < strlen(grand_child.child_value()); i++)
          {
            temp_uname[i] = (grand_child.child_value())[i];
          }
        }
        if((strcmp(grand_child.name(), "PASSWORD") == 0) && (NULL == temp_passwd))
        {
          if((temp_passwd = (char*)calloc(1,
                                   (strlen(grand_child.child_value()) * sizeof(char)) + 1)) == NULL)
          {
            LOG_MSG_INFO1("error with calloc extractUnamePasswdConfig", 0, 0, 0);
            break;
          }
          for(int i = 0; i < strlen(grand_child.child_value()); i++)
          {
            temp_passwd[i] = (grand_child.child_value())[i];
          }
        }
      }

      if((temp_uname != NULL) && (temp_passwd != NULL))
      {
        configuration->insertUnamePasswd(temp_uname, temp_passwd);
      }

      free(temp_passwd);
      free(temp_uname);
    }
  }
}

/*==========================================================================
  FUNCTION extractUnameWANAssocConfig
==========================================================================*/
/*!
@brief
  parses xml file for SOCKSv5 uname and wan service mapping and updates
  SOCKSv5 Proxy configuration

@parameters
  xml node of PRIVATE_METHOD_CFG
  SOCKSv5 configuration object

@return

*/
/*========================================================================*/
static void extractUnameWANAssocConfig
(
pugi::xml_node node,
QC_SOCKSv5_Proxy_Configuration* configuration
)
{
  char* temp_uname;
  char* temp_service_no;

  if(NULL == configuration)
  {
    LOG_MSG_INFO1("give null configuration object", 0, 0, 0);
    return;
  }

  for(pugi::xml_node child = node.first_child(); child != NULL; child = child.next_sibling())
  {
    temp_uname = NULL;
    temp_service_no = NULL;

    if(strcmp(child.name(), "USER") == 0)
    {
      for(pugi::xml_node grand_child = child.first_child(); grand_child != NULL;
          grand_child = grand_child.next_sibling())
      {
        if((strcmp(grand_child.name(), "NAME") == 0) && (NULL == temp_uname))
        {
          if((temp_uname = (char*)calloc(1, (strlen(grand_child.child_value()) * sizeof(char)) + 1))
              == NULL)
          {
            LOG_MSG_INFO1("error calloc extractUnameWANAssocConfig", 0, 0 ,0);
            break;
          }
          for(int i = 0; i < strlen(grand_child.child_value()); i++)
          {
            temp_uname[i] = (grand_child.child_value())[i];
          }
        }
        if((strcmp(grand_child.name(), "PROFILE") == 0) && (NULL == temp_service_no))
        {
          if((temp_service_no = (char*)calloc(1, (strlen(
                                           grand_child.child_value()) * sizeof(char)) + 1)) == NULL)
          {
            LOG_MSG_INFO1("error calloc extractUnameWANAssocConfig", 0, 0 ,0);
            break;
          }
          for(int i = 0; i < strlen(grand_child.child_value()); i++)
          {
            temp_service_no[i] = (grand_child.child_value())[i];
          }
        }
      }

      if((temp_uname != NULL) && (temp_service_no != NULL))
      {
        configuration->insertUnameWANAssoc(temp_uname, (int)atoi(temp_service_no));
      }

      free(temp_service_no);
      free(temp_uname);
    }
  }

}

/*==========================================================================
  FUNCTION extractTCPKeepAliveConfig
==========================================================================*/
/*!
@brief
  parses xml file for TCP Keep Alive parameters and updates SOCKSv5 Proxy configuration

@parameters
  xml node of TCP_KEEP_ALIVE_TIME_CFG
  SOCKSv5 configuration object

@return

*/
/*========================================================================*/
static void extractTCPKeepAliveConfig
(
pugi::xml_node node,
QC_SOCKSv5_Proxy_Configuration* configuration
)
{

  if(NULL == configuration)
  {
    LOG_MSG_INFO1("give null configuration object", 0, 0, 0);
    return;
  }

  configuration->setTCPKeepAliveIntvl((unsigned char)atoi(node.child("INTVL").child_value()));
  configuration->setTCPKeepAliveProbes((unsigned char)atoi(node.child("PROBES").child_value()));
  configuration->setTCPKeepAliveTime((unsigned short)atoi(node.child("TIME").child_value()));

}
