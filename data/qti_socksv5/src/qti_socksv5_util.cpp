/*==========================================================================

  FILE:  qti_socksv5_util.cpp

  SERVICES:

  Utility functions QC_SOCKSv5 Proxy implementation following RFC 1928 protocol

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

#include <sys/ioctl.h>
#include <net/if.h>

#include "qti_socksv5_pkt_factory.hpp"
#include "qti_socksv5_cfg.hpp"
#include "qti_socksv5_util.hpp"
#include "qti_socksv5_log_msg.h"

namespace QC_SOCKSv5_Util
{

  /*==========================================================================
    FUNCTION handleUnamePasswdAuth
  ==========================================================================*/
  /*!
  @brief
    checks the given username password

  @parameters
    buffer - SOCKSv5 payload
    cli_sock - ptr to the socket connected with SOCKSv5 client
    configuration - configuration object

  @return
    0 - valid username password and succesfully assigned the egress wan iface
    -1 - otherwise
  */
  /*========================================================================*/
  int handleUnamePasswdAuth
  (
    unsigned char* buffer,
    int cli_sock,
    const QC_SOCKSv5_Proxy_Configuration* configuration
  )
  {
    struct socksv5_packet* uname_passwd_reply;
    unsigned char ulen, plen;
    char* uname = NULL;
    char* passwd = NULL;

    if(NULL == buffer)
    {
      LOG_MSG_INFO1("buffer is null in handle uname wan map auth", 0, 0, 0);
      return -1;
    } else if(NULL == configuration)
    {
      LOG_MSG_INFO1("configuration is null in handle uname wan map auth", 0, 0, 0);
      return -1;
    }

    //check version
    if(buffer[0] != 1)
    {
      LOG_MSG_INFO1("Received a non standard username password auth version!", 0, 0, 0);
      goto invalid_auth;
    }

    //must have a uname
    ulen = buffer[1];
    if(ulen == 0)
    {
      goto invalid_auth;
    } else {

      //get uname
      if((uname = (char*)calloc(1, ulen * sizeof(char) + 1)) == NULL)
      {
        LOG_MSG_INFO1("Failed to calloc for uname", 0, 0, 0);
        goto invalid_auth;
      }
      for(int i = 0; i < ulen; i++)
      {
        uname[i] = buffer[2 + i];
      }
    }

    //must have a passwd
    plen = buffer[ulen + 2];
    if(plen == 0)
    {
      goto invalid_auth;
    } else {

      //get passwd
      if((passwd = (char*)calloc(1, plen * sizeof(char) + 1)) == NULL)
      {
        LOG_MSG_INFO1("Failed to calloc for passwd", 0, 0, 0);
        goto invalid_auth;
      }
      for(int i = 0; i < plen; i++)
      {
        passwd[i] = buffer[ulen + 3 + i];
      }
    }

    //validate UNAME/PASSWD
    if(configuration->validateUnamePasswd(uname, passwd))
    {
      //send success
      if((uname_passwd_reply = QC_SOCKSv5_Pkt_Factory::createUnamePasswdReply(0x00)) == NULL)
      {
        LOG_MSG_INFO1("failure to create uname passwd reply", 0, 0, 0);
        free(uname);
        free(passwd);
        return -1;
      }

      if(send(cli_sock, uname_passwd_reply->payload, uname_passwd_reply->payload_size, 0) <= -1)
      {
        LOG_MSG_INFO1("error writing to client socket", 0, 0, 0);
      }
      QC_SOCKSv5_Pkt_Factory::freeSocksv5Pkt(uname_passwd_reply);

      free(uname);
      free(passwd);
      return 0;

    }

    invalid_auth:
        if((uname_passwd_reply = QC_SOCKSv5_Pkt_Factory::createUnamePasswdReply(0xff)) == NULL)
        {
          LOG_MSG_INFO1("failure to create uname passwd reply", 0, 0, 0);
          free(uname);
          free(passwd);
          return -1;
        }
        if(send(cli_sock, uname_passwd_reply->payload, uname_passwd_reply->payload_size, 0) <= -1)
        {
          LOG_MSG_INFO1("error writing to client socket", 0, 0, 0);
        }
        QC_SOCKSv5_Pkt_Factory::freeSocksv5Pkt(uname_passwd_reply);
        free(uname);
        free(passwd);
        return -1;
  }

  /*==========================================================================
    FUNCTION handleUnameWANMapAuth
  ==========================================================================*/
  /*!
  @brief
    checks the given username and then assigns the appropriate
    egress wan iface for connecting with remote

  @parameters
    buffer - SOCKSv5 payload
    cli_sock - ptr to the socket connected with SOCKSv5 client
    egress_wan_iface - wan iface name
    configuration - configuration object

  @return
    0 - valid username password and succesfully assigned the egress wan iface
    -1 - otherwise
  */
  /*========================================================================*/
  int handleUnameWANMapAuth
  (
    unsigned char* buffer,
    int cli_sock,
    char** egress_wan_iface,
    const QC_SOCKSv5_Proxy_Configuration* configuration
  )
  {
    struct socksv5_packet* uname_wan_map_reply;
    unsigned char ulen;
    char* uname = NULL;
    struct wan_service* egress_wan_service = NULL;

    if(NULL == buffer)
    {
      LOG_MSG_INFO1("buffer is null in handle uname wan map auth", 0, 0, 0);
      return -1;
    } else if(NULL == configuration)
    {
      LOG_MSG_INFO1("configuration is null in handle uname wan map auth", 0, 0, 0);
      return -1;
    } else if(NULL == egress_wan_iface)
    {
      LOG_MSG_INFO1("egress wan iface is null in handle uname wan map auth", 0, 0, 0);
      return -1;
    }

    //check version
    if(buffer[0] != 1)
    {
      LOG_MSG_INFO1("Received a non standard private auth version!", 0, 0 ,0);
      goto invalid_auth;
    }

    //must have a uname
    ulen = buffer[1];
    if(ulen == 0)
    {
      goto invalid_auth;
    } else {

      //get uname
      if((uname = (char*)calloc(1, ulen * sizeof(char) + 1)) == NULL)
      {
        LOG_MSG_INFO1("error with calloc in unameWANMapAuth", 0, 0, 0);
        goto invalid_auth;
      }
      for(int i = 0; i < ulen; i++)
      {
        uname[i] = buffer[2 + i];
      }
    }

    //get uname association to service
    int service_no;
    if((service_no = configuration->getUnameWANAssoc(uname)) >= 0)
    {
      //std::cout << "Client selected service no: " << service_no << std::endl;
      //update the egress_wan_iface appropriately
      egress_wan_service = (struct wan_service*)configuration->getWANService(service_no);
      if((NULL != egress_wan_service) && (NULL != egress_wan_service->wan_iface))
      {
        *egress_wan_iface = egress_wan_service->wan_iface;
      } else {
        LOG_MSG_INFO1("getWANService invalid for: %d", service_no, 0, 0);
        free(uname);
        return -1;
      }

      //send success
      if((uname_wan_map_reply = QC_SOCKSv5_Pkt_Factory::createUnamePasswdReply(0x00)) == NULL)
      {
        LOG_MSG_INFO1("failure to create uname passwd reply", 0, 0, 0);
        free(uname);
        return -1;
      }
      if(send(cli_sock, uname_wan_map_reply->payload, uname_wan_map_reply->payload_size, 0) <= -1)
      {
        LOG_MSG_INFO1("error writing to client socket", 0, 0, 0);
      }
      QC_SOCKSv5_Pkt_Factory::freeSocksv5Pkt(uname_wan_map_reply);

      free(uname);
      return 0;
    }

    invalid_auth:
        if((uname_wan_map_reply = QC_SOCKSv5_Pkt_Factory::createUnamePasswdReply(0xff)) == NULL)
        {
          LOG_MSG_INFO1("failure to create uname passwd reply", 0, 0, 0);
          free(uname);
          return -1;
        }
        if(send(cli_sock, uname_wan_map_reply->payload,
                 uname_wan_map_reply->payload_size, 0) <= -1)
        {
          LOG_MSG_INFO1("error writing to client socket", 0, 0, 0);
        }
        QC_SOCKSv5_Pkt_Factory::freeSocksv5Pkt(uname_wan_map_reply);
        free(uname);
        return -1;
  }

  /*==========================================================================
    FUNCTION handleConnectRequest
  ==========================================================================*/
  /*!
  @brief
    connects with requested remote server per RFC 1928 following the
    connect request

  @parameters
    buffer - SOCKSv5 payload
    cli_sock - socket fd connected with SOCKSv5 client

    wan_iface - wan iface name sock_remote shall bind to

  @return
    sock_remote - socket fd to be used for remote server
    -1 - otherwise
  */
  /*========================================================================*/
  int handleConnectRequest
  (
    unsigned char* buffer,
    int cli_sock,
    const char* wan_iface,
    const QC_SOCKSv5_Proxy_Configuration* configuration
  )
  {
    if(NULL == buffer)
    {
      LOG_MSG_INFO1("buffer is null", 0, 0, 0);
      return -1;
    } else if(NULL == configuration)
    {
      LOG_MSG_INFO1("configuration is null", 0, 0, 0);
      return -1;
    }

    int sock_remote;
    struct socksv5_packet* cmd_reply = NULL;
    struct sockaddr_in req_addr_v4;
    struct sockaddr_in6 req_addr_v6;
    unsigned int lan_af_family = AF_UNSPEC;
    unsigned int cli_mss = 0;
    bool setMSS = false;
    socklen_t len;
    const unsigned char auth_method = configuration->getPrefAuthMethod();
    char bind_addr[INET6_ADDRSTRLEN]; //should cover both IPv4 and IPv6 addr length
    int wan_ip_ver;
    char* resolved_addr = NULL;
    //we know how long the name will be + 1 for null terminating character
    char name_to_resolve[buffer[4] + 1];

    if((NULL == wan_iface) && (auth_method != Pref_Auth_Method::NO_AUTH))
    {
      LOG_MSG_INFO1("egress wan iface is null", 0, 0, 0);
      return -1;
    }

    len = sizeof(cli_mss);
    if(getsockopt(cli_sock, IPPROTO_TCP, TCP_MAXSEG, (void*)&cli_mss, &len) <= -1)
    {
      LOG_MSG_INFO1("Failed to get client socket mss: %s", strerror(errno), 0, 0);
      cli_mss = MAX_MSS_IPV6_IPV4_SPLICE_SESSION;
    }

    //Is cli socket AF_INET6?
    len = sizeof(lan_af_family);
    if(getsockopt(cli_sock, SOL_SOCKET, SO_DOMAIN, (void*)&lan_af_family, &len) <= -1)
    {
      LOG_MSG_INFO1("Failed to get client socket family: %s", strerror(errno), 0, 0);
      return -1;
    }

    //check address type
    switch(buffer[3])
    {
      case(Addr_Type::IPV4):
      {
        //Here we make another socket connection for the requested IP
        if((sock_remote = socket(AF_INET, SOCK_STREAM, 0)) <= -1)
        {
          LOG_MSG_INFO1("error creating requested socket", 0, 0, 0);
          return -1;
        }

        //bind socket to wan_iface
        switch(auth_method)
        {
          case(Pref_Auth_Method::UNAME_PASSWD):
          {
            if(configuration->getWANService(wan_iface) &&
               !(configuration->getWANService(wan_iface)->ip_ver & IPV4_SUPPORTED))
            {
              LOG_MSG_INFO1("WAN IPv4 is not supported for iface: %s : 0x%x", wan_iface,
                            configuration->getWANService(wan_iface)->ip_ver, 0);
              LOG_MSG_INFO1("Will respond ENETUNREACH", 0, 0, 0);
              respondToConnectRequest(cli_sock, ENETUNREACH, Addr_Type::IPV4);
              goto cleanup_1;
            }
            if(setsockopt(sock_remote, SOL_SOCKET, SO_BINDTODEVICE, (void*)wan_iface,
                          strlen((char*)wan_iface)) <= -1)
            {
              LOG_MSG_INFO1("error binding req socket to iface: \"%s\":%s", wan_iface,
                            strerror(errno), 0);
              goto cleanup_1;
            }
            break;
          }
          default:
          {
            break;
          }
        }

        //Is cli socket AF_INET6?
        if(AF_INET6 == lan_af_family)
        {
          setMSS = true;
        }

        //TCP handshake with the requested address
        memset(&req_addr_v4, 0x00, sizeof(struct sockaddr_in));
        req_addr_v4.sin_family = AF_INET;
        req_addr_v4.sin_port = (buffer[9] << 8) | buffer[8];
        req_addr_v4.sin_addr.s_addr = (buffer[7] << 24) | (buffer[6] << 16) |
                                   (buffer[5] << 8) | buffer[4];

        if(respondToConnectRequest(cli_sock, connectToWAN(sock_remote, &req_addr_v4, AF_INET,
                                   setMSS, cli_mss), Addr_Type::IPV4) != 0)
        {
          goto cleanup_1;
        }

        break;
      }

      case(Addr_Type::DOMAIN_NAME): //We will resolve domain name on behalf of client here
      {
        //get the domain name
        for(int i = 0; i < buffer[4]; i++)
        {
          name_to_resolve[i] = buffer[5 + i];
        }
        name_to_resolve[buffer[4]] = '\0';  //NULL terminate the string

        LOG_MSG_INFO1("Resolving domain name on behalf of client: %s", name_to_resolve, 0, 0);

        if(configuration->getWANService(wan_iface) != NULL)
        {
          //what does the wan iface support?
          switch((configuration->getWANService(wan_iface))->ip_ver)
          {
            case(IPV6_SUPPORTED):
            {
              wan_ip_ver = Addr_Type::IPV6;

              //execute dig
              if((resolved_addr = executeDig(name_to_resolve, wan_ip_ver,
                  configuration->getWANService(wan_iface)->pri_dns_ipv6_addr, auth_method)) == NULL)
              {
                if((resolved_addr = executeDig(name_to_resolve, wan_ip_ver,
                    configuration->getWANService(wan_iface)->sec_dns_ipv6_addr,
                    auth_method)) == NULL)
                {
                  LOG_MSG_INFO1("error executing dig command", 0, 0, 0);
                  return -1;
                }
              }
              break;
            }

            case(IPV4_SUPPORTED):
            {
              wan_ip_ver = Addr_Type::IPV4;

              //execute dig
              if((resolved_addr = executeDig(name_to_resolve, wan_ip_ver,
                  configuration->getWANService(wan_iface)->pri_dns_ipv4_addr, auth_method)) == NULL)
              {
                if((resolved_addr = executeDig(name_to_resolve, wan_ip_ver,
                    configuration->getWANService(wan_iface)->sec_dns_ipv4_addr,
                    auth_method)) == NULL)
                {
                  LOG_MSG_INFO1("error executing dig command", 0, 0, 0);
                  return -1;
                }
              }
              break;

            }

            case(IPV4V6_SUPPORTED):
            default: //default to try IPv4 first, then IPv6 if both IPv6 and IPv4 supported
            {
              wan_ip_ver = Addr_Type::IPV4;

              //execute dig
              if((resolved_addr = executeDig(name_to_resolve, wan_ip_ver,
                  configuration->getWANService(wan_iface)->pri_dns_ipv4_addr, auth_method)) == NULL)
              {
                if((resolved_addr = executeDig(name_to_resolve, wan_ip_ver,
                    configuration->getWANService(wan_iface)->sec_dns_ipv4_addr,
                    auth_method)) == NULL)
                {
                  LOG_MSG_INFO1("error executing dig command", 0, 0, 0);

                  //try IPv6
                  wan_ip_ver = Addr_Type::IPV6;
                  //execute dig
                  if((resolved_addr = executeDig(name_to_resolve, wan_ip_ver,
                      configuration->getWANService(wan_iface)->pri_dns_ipv6_addr, auth_method))
                      == NULL)
                  {
                    if((resolved_addr = executeDig(name_to_resolve, wan_ip_ver,
                        configuration->getWANService(wan_iface)->sec_dns_ipv6_addr,
                        auth_method)) == NULL)
                    {
                      LOG_MSG_INFO1("error executing dig command", 0, 0, 0);
                      return -1;
                    }
                  }
                }
              }
              break;
            }
          }

          LOG_MSG_INFO1("domain name: %s resolved to: %s", name_to_resolve, resolved_addr, 0);

          //TCP handshake with remote now
          switch(wan_ip_ver)
          {
            case(Addr_Type::IPV4):
            {
              //make the wan socket
              if((sock_remote = socket(AF_INET, SOCK_STREAM, 0)) <= -1)
              {
                LOG_MSG_INFO1("error creating requested socket", 0, 0, 0);
                free(resolved_addr);
                return -1;
              }

              //bind socket to wan_iface
              switch(auth_method)
              {
                case(Pref_Auth_Method::UNAME_PASSWD):
                {
                  if(configuration->getWANService(wan_iface) &&
                     !(configuration->getWANService(wan_iface)->ip_ver & IPV4_SUPPORTED))
                  {
                    LOG_MSG_INFO1("WAN IPv4 is not supported for iface: %s : 0x%x", wan_iface,
                            configuration->getWANService(wan_iface)->ip_ver, 0);
                    LOG_MSG_INFO1("Will respond ENETUNREACH", 0, 0, 0);
                    respondToConnectRequest(cli_sock, ENETUNREACH, Addr_Type::IPV4);
                    free(resolved_addr);
                    goto cleanup_1;
                  }
                  if(setsockopt(sock_remote, SOL_SOCKET, SO_BINDTODEVICE, (void*)wan_iface,
                                strlen((char*)wan_iface)) <= -1)
                  {
                    LOG_MSG_INFO1("error binding req socket to iface: \"%s\":%s", wan_iface,
                                  strerror(errno), 0);
                    free(resolved_addr);
                    goto cleanup_1;
                  }
                  break;
                }
                default:
                {
                  break;
                }
              }

              memset(&req_addr_v4, 0x00, sizeof(struct sockaddr_in));
              req_addr_v4.sin_family = AF_INET;
              req_addr_v4.sin_port = (buffer[buffer[4] + 6] << 8) | buffer[buffer[4] + 5];
              if(inet_pton(AF_INET, resolved_addr, &req_addr_v4.sin_addr.s_addr) != 1)
              {
                LOG_MSG_INFO1("inet_pton failed: %s : %d", resolved_addr, wan_ip_ver, 0);
                free(resolved_addr);
                goto cleanup_1;
              }

              if(AF_INET6 == lan_af_family)
              {
                setMSS = true;
              }

              if(respondToConnectRequest(cli_sock, connectToWAN(sock_remote, &req_addr_v4, AF_INET,
                                         setMSS, cli_mss), Addr_Type::IPV4) != 0)
              {
                free(resolved_addr);
                goto cleanup_1;
              }

              break;
            }

            case(Addr_Type::IPV6):
            {
              //make the wan socket
              if((sock_remote = socket(AF_INET6, SOCK_STREAM, 0)) <= -1)
              {
                LOG_MSG_INFO1("error creating requested socket", 0, 0, 0);
                free(resolved_addr);
                return -1;
              }

              //bind socket to wan_iface
              switch(auth_method)
              {
                case(Pref_Auth_Method::UNAME_PASSWD):
                {
                  if(configuration->getWANService(wan_iface) &&
                     !(configuration->getWANService(wan_iface)->ip_ver & IPV6_SUPPORTED))
                  {
                    LOG_MSG_INFO1("WAN IPv6 is not supported for iface: %s : 0x%x", wan_iface,
                            configuration->getWANService(wan_iface)->ip_ver, 0);
                    LOG_MSG_INFO1("Will respond ENETUNREACH", 0, 0, 0);
                    respondToConnectRequest(cli_sock, ENETUNREACH, Addr_Type::IPV6);
                    free(resolved_addr);
                    goto cleanup_1;
                  }
                  if(setsockopt(sock_remote, SOL_SOCKET, SO_BINDTODEVICE, (void*)wan_iface,
                                strlen((char*)wan_iface)) <= -1)
                  {
                    LOG_MSG_INFO1("error binding req socket to iface: \"%s\":%s", wan_iface,
                                  strerror(errno), 0);
                    free(resolved_addr);
                    goto cleanup_1;
                  }
                  break;
                }
                default:
                {
                  break;
                }
              }

              memset(&req_addr_v6, 0x00, sizeof(struct sockaddr_in6));
              req_addr_v6.sin6_family = AF_INET6;
              req_addr_v6.sin6_port = (buffer[buffer[4] + 6] << 8) | buffer[buffer[4] + 5];
              if(inet_pton(AF_INET6, resolved_addr, &req_addr_v6.sin6_addr) != 1)
              {
                LOG_MSG_INFO1("inet_pton failed: %s : %d", resolved_addr, wan_ip_ver, 0);
                free(resolved_addr);
                goto cleanup_1;
              }

              if(respondToConnectRequest(cli_sock, connectToWAN(sock_remote, &req_addr_v6,
                                         AF_INET6, false, cli_mss), Addr_Type::IPV6) != 0)
              {
                free(resolved_addr);
                goto cleanup_1;
              }

              break;
            }

            default:
            {
              LOG_MSG_INFO1("Invalid wan_ip_ver: %d", wan_ip_ver, 0, 0);
              free(resolved_addr);
              return -1;
              break;
            }
          }

        } else if(Pref_Auth_Method::NO_AUTH == auth_method) {

          //try IPv4 first
          ipv4_attempt:
            if((resolved_addr = executeDig(name_to_resolve, Addr_Type::IPV4, "", auth_method)) ==
                NULL)
            {
                LOG_MSG_INFO1("error executing dig command", 0, 0, 0);
                goto ipv6_attempt;
            }

            if((sock_remote = socket(AF_INET, SOCK_STREAM, 0)) <= -1) //make the wan socket
            {
              LOG_MSG_INFO1("error creating requested socket", 0, 0, 0);
              free(resolved_addr);
              return -1;
            }

            memset(&req_addr_v4, 0x00, sizeof(struct sockaddr_in));
            req_addr_v4.sin_family = AF_INET;
            req_addr_v4.sin_port = (buffer[buffer[4] + 6] << 8) | buffer[buffer[4] + 5];
            if(inet_pton(AF_INET, resolved_addr, &req_addr_v4.sin_addr.s_addr) != 1)
            {
              LOG_MSG_INFO1("inet_pton failed: %s : %d", resolved_addr, Addr_Type::IPV4, 0);
              free(resolved_addr);
              goto cleanup_1;
            }

            if(AF_INET6 == lan_af_family)
            {
              setMSS = true;
            }

            if(connectToWAN(sock_remote, &req_addr_v4, AF_INET, setMSS, cli_mss))
            {
              free(resolved_addr);
              if(close(sock_remote))
              {
                LOG_MSG_INFO1("error closing sock_remote: %s", strerror(errno), 0, 0);
              }
              goto ipv6_attempt;
            }

            if(respondToConnectRequest(cli_sock, 0, Addr_Type::IPV4) != 0)
            {
              free(resolved_addr);
              goto cleanup_1;
            }

            goto resolved_addr_label;

          //try IPv6 second
          ipv6_attempt:
            setMSS = false;

            if((resolved_addr = executeDig(name_to_resolve, Addr_Type::IPV6, "", auth_method)) ==
                NULL)
            {
                LOG_MSG_INFO1("error executing dig command", 0, 0, 0);
                goto enetunreach;
            }

            if((sock_remote = socket(AF_INET6, SOCK_STREAM, 0)) <= -1) //make the wan socket
            {
              LOG_MSG_INFO1("error creating requested socket", 0, 0, 0);
              free(resolved_addr);
              return -1;
            }

            memset(&req_addr_v6, 0x00, sizeof(struct sockaddr_in6));
            req_addr_v6.sin6_family = AF_INET6;
            req_addr_v6.sin6_port = (buffer[buffer[4] + 6] << 8) | buffer[buffer[4] + 5];
            if(inet_pton(AF_INET6, resolved_addr, &req_addr_v6.sin6_addr.s6_addr) != 1)
            {
              LOG_MSG_INFO1("inet_pton failed: %s : %d", resolved_addr, Addr_Type::IPV6, 0);
              free(resolved_addr);
              goto cleanup_1;
            }

            if(respondToConnectRequest(cli_sock, connectToWAN(sock_remote, &req_addr_v6, AF_INET6,
                                       false, cli_mss), Addr_Type::IPV6) != 0)
            {
              free(resolved_addr);
              goto cleanup_1;
            }

            goto resolved_addr_label;

          enetunreach:
            //respond ENETUNREACH
            LOG_MSG_INFO1("Will respond ENETUNREACH", 0, 0, 0);
            respondToConnectRequest(cli_sock, ENETUNREACH, Addr_Type::DOMAIN_NAME);
            return -1;


          resolved_addr_label:
            LOG_MSG_INFO1("domain name: %s resolved to: %s", name_to_resolve, resolved_addr, 0);

        } else {
          LOG_MSG_INFO1("WAN Service is NULL", wan_iface, 0, 0);
          return -1;
        }

        free(resolved_addr);

        break;
      }

      case(Addr_Type::IPV6):
      {
        //Here we make another socket connection for the requested IP
        if((sock_remote = socket(AF_INET6, SOCK_STREAM, 0)) <= -1)
        {
          LOG_MSG_INFO1("error creating requested socket", 0, 0, 0);
          return -1;
        }

        //bind socket to wan_iface
        switch(auth_method)
        {
          case(Pref_Auth_Method::UNAME_PASSWD):
          {
            if(configuration->getWANService(wan_iface) &&
               !(configuration->getWANService(wan_iface)->ip_ver & IPV6_SUPPORTED))
            {
              LOG_MSG_INFO1("WAN IPv6 is not supported for iface: %s : 0x%x", wan_iface,
                            configuration->getWANService(wan_iface)->ip_ver, 0);
              LOG_MSG_INFO1("Will respond ENETUNREACH", 0, 0, 0);
              respondToConnectRequest(cli_sock, ENETUNREACH, Addr_Type::IPV6);
              goto cleanup_1;
            }
            if(setsockopt(sock_remote, SOL_SOCKET, SO_BINDTODEVICE, (void*)wan_iface,
                          strlen((char*)wan_iface)) <= -1)
            {
              LOG_MSG_INFO1("error binding req socket to iface: \"%s\":%s", wan_iface,
                            strerror(errno), 0);
              goto cleanup_1;
            }
            break;
          }
          default:
          {
            break;
          }
        }

        //Is cli socket AF_INET6?
        if(AF_INET6 == lan_af_family)
        {
          setMSS = true;
        }


        //TCP handshake with the requested address
        memset(&req_addr_v6, 0x00, sizeof(struct sockaddr_in6));
        req_addr_v6.sin6_family = AF_INET6;
        req_addr_v6.sin6_port = (buffer[21] << 8) | buffer[20];
        for(int i = 0; i < 16; i++)
        {
          req_addr_v6.sin6_addr.s6_addr[i] = buffer[i + 4];
        }

        if(respondToConnectRequest(cli_sock, connectToWAN(sock_remote, &req_addr_v6, AF_INET6,
                                   setMSS, cli_mss), Addr_Type::IPV6) != 0)
        {
          goto cleanup_1;
        }

        break;
      }

      default:
      {
        LOG_MSG_INFO1("Client requested an invalid address type", 0, 0, 0);
        if((cmd_reply = QC_SOCKSv5_Pkt_Factory::createCmdReply(
                        Server_Reply::ADDR_TYPE_UNSUPPORTED, Addr_Type::IPV6, "::", 0x00)) == NULL)
        {
          LOG_MSG_INFO1("failure to create cmd reply: %u",
                        Server_Reply::ADDR_TYPE_UNSUPPORTED, 0, 0);
          return -1;
        }
        if(send(cli_sock, cmd_reply->payload, cmd_reply->payload_size, 0) <= -1)
        {
          LOG_MSG_INFO1("error writing to client socket", 0, 0, 0);
        }
        QC_SOCKSv5_Pkt_Factory::freeSocksv5Pkt(cmd_reply);
        return -1;
      }
    }

    return sock_remote;

    cleanup_1:
      if(close(sock_remote))
      {
        LOG_MSG_INFO1("error closing socket: %s", strerror(errno), 0, 0);
      }
      return -1;
  }

  /*==========================================================================
    FUNCTION handleUnsupportedRequest
  ==========================================================================*/
  /*!
  @brief
    replies to client that the requested command is not supported

  @parameters
    buffer - SOCKSv5 payload
    cli_sock - ptr to the socket connected with SOCKSv5 client

  @return
  */
  /*========================================================================*/
  void handleUnsupportedRequest(unsigned char* buffer, int cli_sock)
  {
    struct socksv5_packet* cmd_reply;

    if(NULL == buffer)
    {
      LOG_MSG_INFO1("buffer is null in handle unsupported", 0, 0, 0);
      return;
    }

    if((cmd_reply = QC_SOCKSv5_Pkt_Factory::createCmdReply(
                    Server_Reply::COMMAND_NOT_SUPPORTED, Addr_Type::IPV4, "0.0.0.0", 0x00)) == NULL)
    {
      LOG_MSG_INFO1("failure to create cmd reply: %u", Server_Reply::COMMAND_NOT_SUPPORTED, 0, 0);
      return;
    }
    if(send(cli_sock, cmd_reply->payload, cmd_reply->payload_size, 0) <= -1)
    {
      LOG_MSG_INFO1("error writing to client socket", 0, 0, 0);
    }
    QC_SOCKSv5_Pkt_Factory::freeSocksv5Pkt(cmd_reply);

    return;
  }
}

/*==========================================================================
  FUNCTION connectToWAN
==========================================================================*/
/*!
@brief
  TCP handshake/connect with requested remote server

@parameters
  sock_remote - socket fd to use for WAN connections
  req_addr - ptr to struct sockaddr_in or sockaddr_in6 for remote address
  af_family - AF_INET or AF_INET6 intended WAN connection
  setMSS - bool to set mss on WAN connection
  cli_mss - MSS of CLI side TCP connection

@return
  0 - success
 -1 - failure
 errno - failure
*/
/*========================================================================*/
static int connectToWAN(int sock_remote, void* req_addr, int af_family, bool setMSS,
                        unsigned int cli_mss)
{
  unsigned int mss_clamp = cli_mss;
  char buffer[INET6_ADDRSTRLEN] = {0};

  if(NULL == req_addr)
  {
    LOG_MSG_INFO1("req_addr NULL", 0, 0, 0);
    return -1;
  }

  //MSS clamp
  if(setMSS)
  {
    LOG_MSG_INFO1("Cli MSS: %d", cli_mss, 0, 0);
    if(setsockopt(sock_remote, IPPROTO_TCP, TCP_MAXSEG, (void*)&mss_clamp, sizeof(mss_clamp)) <= -1)
    {
      LOG_MSG_INFO1("Error setting MSS for sock_remote: %s", strerror(errno), 0, 0);
      return errno;
    }
  }

  switch(af_family)
  {
    case(AF_INET):
    {
      if(connect(sock_remote, (struct sockaddr*)req_addr, sizeof(struct sockaddr_in)) <= -1)
      {
        LOG_MSG_INFO1("error connecting to req_addr: %s port 0x%x",
                      inet_ntop(AF_INET, &(((struct sockaddr_in*)req_addr)->sin_addr.s_addr),
                                buffer, INET_ADDRSTRLEN),
                                ntohs(((struct sockaddr_in*)req_addr)->sin_port), 0);
        return errno;
      }
      break;
    }
    case(AF_INET6):
    {
      if(connect(sock_remote, (struct sockaddr*)req_addr, sizeof(struct sockaddr_in6)) <= -1)
      {
        LOG_MSG_INFO1("error connecting to req_addr: %s port 0x%x",
                      inet_ntop(AF_INET6, ((struct sockaddr_in6*)req_addr)->sin6_addr.s6_addr,
                                buffer, INET6_ADDRSTRLEN),
                                ntohs(((struct sockaddr_in6*)req_addr)->sin6_port), 0);
        return errno;
      }
      break;
    }
    default:
    {
      LOG_MSG_INFO1("not a valid af_family for req: %d", af_family, 0, 0);
      return -1;
      break;
    }
  }

  return 0;
}

/*==========================================================================
  FUNCTION respondToConnectRequest
==========================================================================*/
/*!
@brief
  Respond to SOCKSv5 Client on Connect response

@parameters
  cli_sock - ptr to socket used for LAN
  reply_errno - errno reply or 0 if success
  wan_req_type - IPv4 or IPv6 intended WAN connection

@return
  0 - success
 -1 - failure
 errno - failure
*/
/*========================================================================*/
static int respondToConnectRequest(int cli_sock, int reply_errno, int wan_req_type)
{
  struct socksv5_packet* cmd_reply = NULL;
  int ret = reply_errno;

  switch(wan_req_type)
  {
    case(Addr_Type::IPV4):
    {
      switch(reply_errno)
      {
        //Sucess case
        case 0:
        {
          if((cmd_reply = QC_SOCKSv5_Pkt_Factory::createCmdReply(
                          Server_Reply::SUCCEEDED, Addr_Type::IPV4, "0.0.0.0", 0x00)) == NULL)
          {
            LOG_MSG_INFO1("failure to create cmd reply: %u", Server_Reply::SUCCEEDED, 0, 0);
            ret = -1;
            break;
          }
          if(send(cli_sock, cmd_reply->payload, cmd_reply->payload_size, 0) <= -1)
          {
            LOG_MSG_INFO1("error writing to client socket: %s", strerror(errno), 0, 0);
            ret = -1;
            break;
          }

          break;
        }
        case ECONNREFUSED:
        {
          LOG_MSG_INFO1("ECONNREFUSED", 0, 0, 0);
          if((cmd_reply = QC_SOCKSv5_Pkt_Factory::createCmdReply(
                   Server_Reply::CONNECTION_REFUSED, Addr_Type::IPV4, "0.0.0.0", 0x00)) == NULL)
          {
            LOG_MSG_INFO1("failure to create cmd reply: %u",
                          Server_Reply::CONNECTION_REFUSED, 0, 0);
            break;
          }
          if(send(cli_sock, cmd_reply->payload, cmd_reply->payload_size, 0) <= -1)
          {
            LOG_MSG_INFO1("error writing to client socket", 0, 0, 0);
          }
          break;
        }
        case ENETUNREACH:
        {
          LOG_MSG_INFO1("ENETUNREACH", 0, 0, 0);
          if((cmd_reply = QC_SOCKSv5_Pkt_Factory::createCmdReply(
                  Server_Reply::NETWORK_UNREACHABLE, Addr_Type::IPV4, "0.0.0.0", 0x00)) == NULL)
          {
            LOG_MSG_INFO1("failure to create cmd reply: %u",
                          Server_Reply::NETWORK_UNREACHABLE, 0, 0);
            break;
          }
          if(send(cli_sock, cmd_reply->payload, cmd_reply->payload_size, 0) <= -1)
          {
            LOG_MSG_INFO1("error writing to client socket", 0, 0, 0);
          }
          break;
        }
        case ETIMEDOUT:
        {
          LOG_MSG_INFO1("ETIMEDOUT", 0, 0, 0);
          if((cmd_reply = QC_SOCKSv5_Pkt_Factory::createCmdReply(
                     Server_Reply::HOST_UNREACHABLE, Addr_Type::IPV4, "0.0.0.0", 0x00)) == NULL)
          {
            LOG_MSG_INFO1("failure to create cmd reply: %u",
                          Server_Reply::HOST_UNREACHABLE, 0, 0);
            break;
          }
          if(send(cli_sock, cmd_reply->payload, cmd_reply->payload_size, 0) <= -1)
          {
            LOG_MSG_INFO1("error writing to client socket", 0, 0, 0);
          }
          break;
        }
        default:
        {
          LOG_MSG_INFO1("wan connect error: %d", reply_errno, 0, 0);
          ret = -1;
          if((cmd_reply = QC_SOCKSv5_Pkt_Factory::createCmdReply(Server_Reply::SERVER_FAILURE,
                          Addr_Type::IPV4, "0.0.0.0", 0x00)) == NULL)
          {
            LOG_MSG_INFO1("failure to create cmd reply: %u",
                          Server_Reply::SERVER_FAILURE, 0, 0);
            break;
          }
          if(send(cli_sock, cmd_reply->payload, cmd_reply->payload_size, 0) <= -1)
          {
            LOG_MSG_INFO1("error writing to client socket", 0, 0, 0);
          }
          break;
        }
      }
      break;
    }

    case(Addr_Type::DOMAIN_NAME):
    {
      switch(reply_errno)
      {
        //Sucess case
        case 0:
        {
          if((cmd_reply = QC_SOCKSv5_Pkt_Factory::createCmdReply(
                          Server_Reply::SUCCEEDED, Addr_Type::DOMAIN_NAME, "", 0x00)) == NULL)
          {
            LOG_MSG_INFO1("failure to create cmd reply: %u", Server_Reply::SUCCEEDED, 0, 0);
            ret = -1;
            break;
          }
          if(send(cli_sock, cmd_reply->payload, cmd_reply->payload_size, 0) <= -1)
          {
            LOG_MSG_INFO1("error writing to client socket", 0, 0, 0);
            ret = -1;
            break;
          }

          ret = 0;
          break;
        }
        case ECONNREFUSED:
        {
          LOG_MSG_INFO1("ECONNREFUSED", 0, 0, 0);
          if((cmd_reply = QC_SOCKSv5_Pkt_Factory::createCmdReply(
                   Server_Reply::CONNECTION_REFUSED, Addr_Type::DOMAIN_NAME, "", 0x00)) == NULL)
          {
            LOG_MSG_INFO1("failure to create cmd reply: %u",
                          Server_Reply::CONNECTION_REFUSED, 0, 0);
            break;
          }
          if(send(cli_sock, cmd_reply->payload, cmd_reply->payload_size, 0) <= -1)
          {
            LOG_MSG_INFO1("error writing to client socket", 0, 0, 0);
          }
          break;
        }
        case ENETUNREACH:
        {
          LOG_MSG_INFO1("ENETUNREACH", 0, 0, 0);
          if((cmd_reply = QC_SOCKSv5_Pkt_Factory::createCmdReply(
                  Server_Reply::NETWORK_UNREACHABLE, Addr_Type::DOMAIN_NAME, "", 0x00)) == NULL)
          {
            LOG_MSG_INFO1("failure to create cmd reply: %u",
                          Server_Reply::NETWORK_UNREACHABLE, 0, 0);
            break;
          }
          if(send(cli_sock, cmd_reply->payload, cmd_reply->payload_size, 0) <= -1)
          {
            LOG_MSG_INFO1("error writing to client socket", 0, 0, 0);
          }
          break;
        }
        case ETIMEDOUT:
        {
          LOG_MSG_INFO1("ETIMEDOUT", 0, 0, 0);
          if((cmd_reply = QC_SOCKSv5_Pkt_Factory::createCmdReply(
                     Server_Reply::HOST_UNREACHABLE, Addr_Type::DOMAIN_NAME, "", 0x00)) == NULL)
          {
            LOG_MSG_INFO1("failure to create cmd reply: %u",
                          Server_Reply::HOST_UNREACHABLE, 0, 0);
            break;
          }
          if(send(cli_sock, cmd_reply->payload, cmd_reply->payload_size, 0) <= -1)
          {
            LOG_MSG_INFO1("error writing to client socket", 0, 0, 0);
          }
          break;
        }
        default:
        {
          LOG_MSG_INFO1("wan connect error: %d", reply_errno, 0, 0);
          ret = -1;
          if((cmd_reply = QC_SOCKSv5_Pkt_Factory::createCmdReply(Server_Reply::SERVER_FAILURE,
                          Addr_Type::DOMAIN_NAME, "", 0x00)) == NULL)
          {
            LOG_MSG_INFO1("failure to create cmd reply: %u",
                          Server_Reply::SERVER_FAILURE, 0, 0);
            break;
          }
          if(send(cli_sock, cmd_reply->payload, cmd_reply->payload_size, 0) <= -1)
          {
            LOG_MSG_INFO1("error writing to client socket", 0, 0, 0);
          }
          break;
        }
      }

      break;
    }

    case(Addr_Type::IPV6):
    {
      switch(reply_errno)
      {
        //success case
        case 0:
        {
          if((cmd_reply = QC_SOCKSv5_Pkt_Factory::createCmdReply(
                          Server_Reply::SUCCEEDED, Addr_Type::IPV6, "::", 0x00)) == NULL)
          {
            LOG_MSG_INFO1("failure to create cmd reply: %u", Server_Reply::SUCCEEDED, 0, 0);
            ret = -1;
            break;
          }
          if(send(cli_sock, cmd_reply->payload, cmd_reply->payload_size, 0) <= -1)
          {
            LOG_MSG_INFO1("error writing to client socket", 0, 0, 0);
            ret = -1;
            break;
          }

          ret = 0;
          break;
        }

        case ECONNREFUSED:
        {
          LOG_MSG_INFO1("ECONNREFUSED", 0, 0, 0);
          if((cmd_reply = QC_SOCKSv5_Pkt_Factory::createCmdReply(
                        Server_Reply::CONNECTION_REFUSED, Addr_Type::IPV6, "::", 0x00)) == NULL)
          {
            LOG_MSG_INFO1("failure to create cmd reply: %u",
                          Server_Reply::CONNECTION_REFUSED, 0, 0);
            break;
          }
          if(send(cli_sock, cmd_reply->payload, cmd_reply->payload_size, 0) <= -1)
          {
            LOG_MSG_INFO1("error writing to client socket", 0, 0, 0);
          }
          break;
        }
        case ENETUNREACH:
        {
          LOG_MSG_INFO1("ENETUNREACH", 0, 0, 0);
          if((cmd_reply = QC_SOCKSv5_Pkt_Factory::createCmdReply(
                       Server_Reply::NETWORK_UNREACHABLE, Addr_Type::IPV6, "::", 0x00)) == NULL)
          {
            LOG_MSG_INFO1("failure to create cmd reply: %u",
                          Server_Reply::NETWORK_UNREACHABLE, 0, 0);
            break;
          }
          if(send(cli_sock, cmd_reply->payload, cmd_reply->payload_size, 0) <= -1)
          {
            LOG_MSG_INFO1("error writing to client socket", 0, 0, 0);
          }
          break;
        }
        case ETIMEDOUT:
        {
          LOG_MSG_INFO1("ETIMEDOUT", 0, 0, 0);
          if((cmd_reply = QC_SOCKSv5_Pkt_Factory::createCmdReply(
                          Server_Reply::HOST_UNREACHABLE, Addr_Type::IPV6, "::", 0x00)) == NULL)
          {
            LOG_MSG_INFO1("failure to create cmd reply: %u",
                          Server_Reply::HOST_UNREACHABLE, 0, 0);
            break;
          }
          if(send(cli_sock, cmd_reply->payload, cmd_reply->payload_size, 0) <= -1)
          {
            LOG_MSG_INFO1("error writing to client socket", 0, 0, 0);
          }
          break;
        }
        default:
        {
          LOG_MSG_INFO1("wan connect error: %d", reply_errno, 0, 0);
          ret = -1;
          if((cmd_reply = QC_SOCKSv5_Pkt_Factory::createCmdReply(Server_Reply::SERVER_FAILURE,
                          Addr_Type::IPV6, "::", 0x00)) == NULL)
          {
            LOG_MSG_INFO1("failure to create cmd reply: %u",
                          Server_Reply::SERVER_FAILURE, 0, 0);
            break;
          }
          if(send(cli_sock, cmd_reply->payload, cmd_reply->payload_size, 0) <= -1)
          {
            LOG_MSG_INFO1("error writing to client socket", 0, 0, 0);
          }
          break;
        }
      }

      break;
    }

    default:
    {
      ret = -1;
      LOG_MSG_INFO1("family: %d , not handled", wan_req_type, 0, 0);
      break;
    }
  }

  if(0 != ret)
  {
    if(close(cli_sock))
    {
      LOG_MSG_INFO1("error with close: %s", strerror(errno), 0, 0);
    }
  }

  if(cmd_reply != NULL)
  {
    QC_SOCKSv5_Pkt_Factory::freeSocksv5Pkt(cmd_reply);
  }

  return ret;
}

/*==========================================================================
  FUNCTION executeDig
==========================================================================*/
/*!
@brief
  Executes dig DNS resolution over WAN iface

@parameters
  name_to_resolve - domain name to resolve
  wan_ip_ver - WAN side IP version
  dns_ip_addr - the DNS server IP addr
  auth_method - the authentication mode

@return
  NULL - failure
  char* - the resolved addr
*/
/*========================================================================*/
static char* executeDig
(
const char* name_to_resolve,
unsigned char wan_ip_ver,
const char* dns_ip_addr,
unsigned char auth_method
)
{
  char* resolved_addr = NULL;
  FILE* fp = NULL;
  char line[256] = {0}; //no domain name request can be larger than 255 bytes, RFC 1928
  struct sockaddr_in temp_addr_v4;
  struct sockaddr_in6 temp_addr_v6;
  const char* base_dig = "dig +short +retry=10 ";
  char* dig_cmd = NULL;
  std::string resolved_addr_str;

  if(NULL == name_to_resolve)
  {
    LOG_MSG_INFO1("name_to_resolve str is NULL", 0, 0, 0);
    return NULL;
  }

  if((NULL == dns_ip_addr) && (Pref_Auth_Method::NO_AUTH != auth_method))
  {
    LOG_MSG_INFO1("dns_ip_addr is NULL", 0, 0, 0);
    return NULL;
  }

  //allocate some heap for worst case scenario
  if(NULL == dns_ip_addr)
  {
    if((dig_cmd = (char*)calloc(1, strlen(base_dig) + strlen("@") + strlen(" ") +
                                   strlen(name_to_resolve) + strlen(" -tAAAA") + 1)) == NULL)
    {
      LOG_MSG_INFO1("error with calloc: %s", strerror(errno), 0, 0);
      return NULL;
    }
  } else {
    if((dig_cmd = (char*)calloc(1, strlen(base_dig) + strlen("@") + strlen(dns_ip_addr) +
                                   strlen(" ") + strlen(name_to_resolve) + strlen(" -tAAAA") + 1))
                                   == NULL)
    {
      LOG_MSG_INFO1("error with calloc: %s", strerror(errno), 0, 0);
      return NULL;
    }
  }

  switch(auth_method)
  {
    case(Pref_Auth_Method::UNAME_PASSWD):
    {
      if(snprintf(dig_cmd, strlen(base_dig) + strlen("@") + strlen(dns_ip_addr) + strlen(" ") + 1,
                           "dig +short +retry=10 @%s ", dns_ip_addr) < 0)
      {
        LOG_MSG_INFO1("error with snprintf: %s", strerror(errno), 0, 0);
        goto dns_failure;
      }
      break;
    }
    default:
    {
      if(snprintf(dig_cmd, strlen(base_dig) + 1, "%s", base_dig) < 0)
      {
        LOG_MSG_INFO1("error with snprintf: %s", strerror(errno), 0, 0);
        goto dns_failure;
      }
      break;
    }
  }

  if(snprintf(dig_cmd + strlen(dig_cmd), strlen(name_to_resolve) + 1, "%s", name_to_resolve) < 0)
  {
    LOG_MSG_INFO1("error with snprintf: %s", strerror(errno), 0, 0);
    goto dns_failure;
  }

  switch(wan_ip_ver)
  {
    case(Addr_Type::IPV4):
    {
      if(snprintf(dig_cmd + strlen(dig_cmd), strlen(" -tA") + 1, " -tA") < 0)
      {
        LOG_MSG_INFO1("error with snprintf: %s", strerror(errno), 0, 0);
        goto dns_failure;
      }
      break;
    }
    case(Addr_Type::IPV6):
    {
      if(snprintf(dig_cmd + strlen(dig_cmd), strlen(" -tAAAA") + 1, " -tAAAA") < 0)
      {
        LOG_MSG_INFO1("error with snprintf: %s", strerror(errno), 0, 0);
        goto dns_failure;
      }
      break;
    }
    default:
    {
      LOG_MSG_INFO1("Invalid wan_ip_ver: %d", wan_ip_ver, 0, 0);
      goto dns_failure;
      break;
    }
  }

  LOG_MSG_INFO1("%s", dig_cmd, 0, 0);

  if((fp = popen(dig_cmd, "r")) == NULL)
  {
    LOG_MSG_INFO1("Error with popen: %s", strerror(errno), 0, 0);
    goto dns_failure;
  }

  while(!(resolved_addr_str.length()) && (fgets(line, sizeof(line), fp) != NULL))
  {
    //get rid of the new line character
    if(line[strlen(line) - 1] == '\n')
    {
      line[strlen(line) - 1] = '\0';
    }

    switch(wan_ip_ver)
    {
      case(Addr_Type::IPV4):
      {
        if(inet_pton(AF_INET, line, &temp_addr_v4.sin_addr.s_addr) != 1)
        {
          LOG_MSG_INFO1("inet_pton failed: %s : %d", line, wan_ip_ver, 0);
        } else {
          resolved_addr_str = line;
        }
        break;
      }
      case(Addr_Type::IPV6):
      {
        if(inet_pton(AF_INET6, line, &temp_addr_v6.sin6_addr) != 1)
        {
          LOG_MSG_INFO1("inet_pton failed: %s : %d", line, wan_ip_ver, 0);
        } else {
          resolved_addr_str = line;
        }
        break;
      }
      default:
      {
        LOG_MSG_INFO1("Invalid wan_ip_ver: %d", wan_ip_ver, 0, 0);
        if(pclose(fp))
        {
          LOG_MSG_INFO1("error with pclose: %s", strerror(errno), 0, 0);
        }
        goto dns_failure;
        break;
      }
    }
  }
  if(pclose(fp))
  {
    LOG_MSG_INFO1("error with pclose: %s", strerror(errno), 0, 0);
  }

  if(!(resolved_addr_str.length()))
  {
    goto dns_failure;
  }

  if((resolved_addr = (char*)calloc(1, strlen(resolved_addr_str.c_str()) + 1)) == NULL)
  {
    LOG_MSG_INFO1("Failed to calloc", 0, 0, 0);
    goto dns_failure;
  }

  memcpy(resolved_addr, resolved_addr_str.c_str(), strlen(resolved_addr_str.c_str()));

  dns_success:
    free(dig_cmd);
    return resolved_addr;

  dns_failure:
    free(dig_cmd);
    return NULL;
}
