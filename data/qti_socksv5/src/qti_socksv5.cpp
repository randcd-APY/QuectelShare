/*==========================================================================

  FILE:  qti_socksv5.cpp

  SERVICES:

  main controller for QC_SOCKSv5 Proxy implementation.

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

#include "qti_socksv5_pkt_factory.hpp"
#include "qti_socksv5_cfg.hpp"
#include "qti_socksv5_util.hpp"
#include "qti_socksv5_garbage_collector.hpp"
#include "qti_socksv5.hpp"
#include "qti_socksv5_log_msg.h"

std::mutex cli_req_sock_pair_map_mutex;
static sem_t socks_conn_req_sem; //semaphore to restrict memory resources for SOCKS connect requests
static std::map<int, int> cli_req_sock_pair_map; //key = cli_sock, value = sock_remote
static QC_SOCKSv5_Proxy_Configuration* configuration;
static std::atomic<unsigned int> num_of_critical_threads(0); //for keeping track critical sections
static std::atomic<bool> tcp_splice_mode(true); //false means fallback to socket splice
static std::atomic<bool> lan_server_up(false); //for keeping track of LAN server thread
static char* conf_file = NULL;
static char* auth_file = NULL;
static pthread_t server_thread_v4, server_thread_v6, inotify_thread, lan_monitor_thread;


/*==========================================================================
  FUNCTION main
==========================================================================*/
/*!
@brief
  main controller of SOCKSv5 proxy. Continually listens for netlink
  messages from kernel when lan iface goes up/down.

@parameters
  -L <lan iface name>
  -W <wan iface name>
  -c <configuration file>
  -u <authentication file>

@return
  -1 - error in given configuration files or -L -W options
  errno - error creating netlink socket
  0 - otherwise
*/
/*========================================================================*/
int main(int argc, char* argv[])
{
  unsigned char config_file_flag = 0;
  unsigned char auth_file_flag = 0;
  int opt;
  int nl_sock;
  int num_bytes_recv;
  struct sockaddr_nl kernel_addr;
  struct sockaddr_nl user_addr;
  struct sockaddr_nl netlink_addr;
  struct relay_session_tcp_splice* t_session;
  struct nlmsghdr nlmhdr;
  struct iovec iov;
  struct msghdr msg;
  fd_set sock_read_fd_set; //for select to monitor file descriptors
  void* res;

  //clear the cli_req_sock_pair table
  cli_req_sock_pair_map_mutex.lock();
  cli_req_sock_pair_map.clear();
  cli_req_sock_pair_map_mutex.unlock();

  //will not delete unless reloading a new configuration
  //will rely on OS to free heap after process terminates
  configuration = new QC_SOCKSv5_Proxy_Configuration();

  if(!Diag_LSM_Init(NULL))
  {
     printf("Diag_LSM_Init failed\n");
  }

  //parse options
  while((opt = getopt(argc, argv, "c:u:")) != -1)
  {
    switch(opt)
    {
      case 'c':
      {
        if(config_file_flag != 1)
        {
          LOG_MSG_INFO1("Will parse config file: %s", optarg, 0, 0);

          //config file takes precedence, will ignore all previous L option
          if(configuration->getLANIface() != NULL)
          {
            configuration->deleteLANIface();
          }

          //config file takes precedence, will ignore all previous W option
          if(configuration->getNumOfWANServices() != 0)
          {
            configuration->deleteAllWANServices();
          }

          //parse the given config file
          if(!QC_SOCKSv5_Cfg_Parser::validateConfigFile(optarg, configuration))
          {
            LOG_MSG_INFO1("Error in config file", 0, 0, 0);
            if(NULL != auth_file)
            {
              free(auth_file);
            }
            delete configuration;
            return -1;
          }

          //calloc takes care of '\0' character
          //never free'd, need for lifetime of process
          if((conf_file = (char*)calloc(1, strlen(optarg) * sizeof(char) + 1)) == NULL)
          {
            LOG_MSG_INFO1("Error calloc for config file location", 0, 0, 0);
            if(NULL != auth_file)
            {
              free(auth_file);
            }
            delete configuration;
            return -1;
          }

          //don't trust strcpy
          for(int i = 0; i < strlen(optarg); i++)
          {
            conf_file[i] = optarg[i];
          }

          if(pthread_create(&inotify_thread, NULL, monitorConfFile, (void*)conf_file) != 0)
          {
            LOG_MSG_INFO1("error creating inotify thread: %s", optarg, 0, 0);
            if(NULL != conf_file)
            {
              free(conf_file);
            }
            if(NULL != auth_file)
            {
              free(auth_file);
            }
            delete configuration;
            return -1;
          }

          config_file_flag = 1;
        }

        break;
      }
      case 'u':
      {
        if(auth_file_flag != 1)
        {
          LOG_MSG_INFO1("Will parse auth file: %s", optarg, 0, 0);

          //parse the given config file
          if(!QC_SOCKSv5_Cfg_Parser::validateAuthFile(optarg, configuration))
          {
            LOG_MSG_INFO1("Error in auth file", 0, 0, 0);
            if(NULL != conf_file)
            {
              free(conf_file);
            }
            delete configuration;
            return -1;
          }

          //calloc takes care of '\0' character
          //never free'd, need for lifetime of process
          if((auth_file = (char*)calloc(1, strlen(optarg) * sizeof(char) + 1)) == NULL)
          {
            LOG_MSG_INFO1("Error calloc for auth file location", 0, 0, 0);
            if(NULL != conf_file)
            {
              free(conf_file);
            }
            delete configuration;
            return -1;
          }

          //don't trust strcpy
          for(int i = 0; i < strlen(optarg); i++)
          {
            auth_file[i] = optarg[i];
          }

          auth_file_flag = 1;
        }
        break;
      }
      default:
      {
        printUsage();
        if(NULL != conf_file)
        {
          free(conf_file);
        }
        if(NULL != auth_file)
        {
          free(auth_file);
        }
        delete configuration;
        return -1;
      }
    }
  }

  if(configuration->getLANIface() == NULL)
  {
    printUsage();
    if(NULL != conf_file)
    {
      free(conf_file);
    }
    if(NULL != auth_file)
    {
      free(auth_file);
    }
    delete configuration;
    return -1;
  }

  //if not using config file we will try to support IPv4 and IPv6 LAN
  if(config_file_flag != auth_file_flag) //XOR
  {
    printUsage();
    if(NULL != conf_file)
    {
      free(conf_file);
    }
    if(NULL != auth_file)
    {
      free(auth_file);
    }
    delete configuration;
    return -1;
  }

  configuration->printConfig();

  //check lan iface up/down here, only once
  checkLANIface(configuration, &server_thread_v4, &server_thread_v6);

  //SIGPIPE sent when KEEPALIVE probe does not receive a probe reply
  signal(SIGPIPE, handleSigPipe);

  if(sem_init(&socks_conn_req_sem, 0, MAX_SOCKS_CONN_REQ))
  {
    LOG_MSG_INFO1("error creating semaphore: %s", strerror(errno), 0, 0);
    return errno;
  }

  //spawn netlink monitor LAN iface up/down pthread
  if(pthread_create(&lan_monitor_thread, NULL, monitorLANIface, NULL) != 0)
  {
    LOG_MSG_INFO1("error creating netlink monitor LAN thread: %s", strerror(errno), 0, 0);
    if(NULL != conf_file)
    {
      free(conf_file);
    }
    if(NULL != auth_file)
    {
      free(auth_file);
    }
    delete configuration;
    return errno;
  }

  if((nl_sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_QC_SOCKSIFY)) <= -1)
  {
    LOG_MSG_INFO1("Problem creating netlink socket: %s\n", strerror(errno), 0, 0);
    LOG_MSG_INFO1("Falling back to socket splice\n", 0, 0, 0);
    tcp_splice_mode = false;

    //wait for the lan_monitor thread to finish, which hopefully is never
    if(pthread_join(lan_monitor_thread, &res) != 0)
    {
      LOG_MSG_INFO1("monitorLANIface pthread join error", 0, 0, 0);
    }

    LOG_MSG_INFO1("monitorLANIface suddenly stopped!", 0, 0, 0);

    free(res);

  } else
  {
    tcp_splice_mode = true;

    //clears
    memset(&user_addr, 0, sizeof(struct sockaddr_nl));
    memset(&kernel_addr, 0, sizeof(struct sockaddr_nl));

    //fill out addresses
    user_addr.nl_family = AF_NETLINK;
    user_addr.nl_pid = getpid(); //this process's PID
    user_addr.nl_groups = RTNLGRP_NONE; //unicast
    kernel_addr.nl_family = AF_NETLINK;
    kernel_addr.nl_pid = 0; //default PID 0 means to kernel space
    kernel_addr.nl_groups = RTNLGRP_NONE; //unicast

    //bind
    if(bind(nl_sock, (struct sockaddr*)&user_addr, sizeof(struct sockaddr_nl)) < 0)
    {
      LOG_MSG_INFO1("Failed to bind netlink socket: %s\n", strerror(errno), 0, 0);
      if(close(nl_sock))
      {
        LOG_MSG_INFO1("error closing socket: %s", strerror(errno), 0, 0);
      }
      if(pthread_cancel(lan_monitor_thread))
      {
        LOG_MSG_INFO1("pthread_cancel error: %s", strerror(errno), 0, 0);
      }
      if(pthread_cancel(inotify_thread))
      {
        LOG_MSG_INFO1("pthread_cancel error: %s", strerror(errno), 0, 0);
      }

      if(pthread_join(lan_monitor_thread, NULL))
      {
        LOG_MSG_INFO1("pthread_join error: %s", strerror(errno), 0, 0);
      }
      if(pthread_join(inotify_thread, NULL))
      {
        LOG_MSG_INFO1("pthread_join error: %s", strerror(errno), 0, 0);
      }
      if(NULL != conf_file)
      {
        free(conf_file);
      }
      if(NULL != auth_file)
      {
        free(auth_file);
      }
      delete configuration;
      return errno;
    }

    //notify tcp splice module of our pid
    notifyTCPSpliceModule(nl_sock, &kernel_addr, user_addr.nl_pid);

    //main control loop to listen for netlink messages from kernel module
    while(1)
    {
      //clear, yes we do need to do this every loop iteration
      memset(&nlmhdr, 0, sizeof(struct iovec));
      memset(&iov, 0, sizeof(struct iovec));
      memset(&msg, 0, sizeof(struct msghdr));

      nlmhdr.nlmsg_len = NLMSG_LENGTH(sizeof(struct relay_session_tcp_splice));
      nlmhdr.nlmsg_pid = getpid();

      //fill out iov and msg
      iov.iov_base = (void*)&nlmhdr;
      iov.iov_len  = nlmhdr.nlmsg_len;

      msg.msg_name = (void*)&kernel_addr;
      msg.msg_namelen = sizeof(kernel_addr);
      msg.msg_iov = &iov;
      msg.msg_iovlen = 1;

      //setup file descriptors for select call
      FD_ZERO(&sock_read_fd_set);
      FD_SET(nl_sock, &sock_read_fd_set);

      //block until we recv something over the sock
      if(select(nl_sock + 1, &sock_read_fd_set, NULL, NULL, NULL) < 0)
      {
        LOG_MSG_INFO1("select call failed for receiving from netlink socket\n", 0, 0, 0);
        continue;
      } else {

        if(recvmsg(nl_sock, &msg, 0) > 0)
        {
          //extract msg
          t_session = (struct relay_session_tcp_splice*)NLMSG_DATA(&nlmhdr);

          cli_req_sock_pair_map_mutex.lock();
          if(cli_req_sock_pair_map.find(t_session->cli_sock) != cli_req_sock_pair_map.end())
          {
            //clean up here
            if(close(cli_req_sock_pair_map[t_session->cli_sock]))//closes the sock_remote
            {
              LOG_MSG_INFO1("error with closing socket: %s", strerror(errno), 0, 0);
            }
            if(close(t_session->cli_sock)) //closes the cli_sock
            {
              LOG_MSG_INFO1("error with closing socket: %s", strerror(errno), 0, 0);
            }
            cli_req_sock_pair_map.erase(t_session->cli_sock); //erase hash entry
          }
          cli_req_sock_pair_map_mutex.unlock();
        }
      }

      //yield the CPU and place this thread at the end of priority queue
      if(sched_yield() != 0)
      {
        printf("error yielding main loop!\n");
        continue;
      }
    }

  }

  if(pthread_cancel(lan_monitor_thread))
  {
    LOG_MSG_INFO1("pthread_cancel error: %s", strerror(errno), 0, 0);
  }
  if(pthread_cancel(inotify_thread))
  {
    LOG_MSG_INFO1("pthread_cancel error: %s", strerror(errno), 0, 0);
  }

  if(pthread_join(lan_monitor_thread, NULL))
  {
    LOG_MSG_INFO1("pthread_join error: %s", strerror(errno), 0, 0);
  }
  if(pthread_join(inotify_thread, NULL))
  {
    LOG_MSG_INFO1("pthread_join error: %s", strerror(errno), 0, 0);
  }

  if(sem_destroy(&socks_conn_req_sem))
  {
    LOG_MSG_INFO1("semaphore destroy error: %s", strerror(errno), 0, 0);
  }

  return 0;
}

/*==========================================================================
  FUNCTION spawnLANServerThreads
==========================================================================*/
/*!
@brief
  spawn LAN pthreads based on configuration

@parameters
  SOCKSv5 configuration object
  IPv4 LAN pthread
  IPv6 LAN pthread

@return
*/
/*========================================================================*/
void spawnLANServerThreads
(
QC_SOCKSv5_Proxy_Configuration* configuration,
pthread_t* server_thread_v4,
pthread_t* server_thread_v6
)
{
    if(NULL == configuration)
    {
      LOG_MSG_INFO1("give null configuration", 0, 0, 0);
      return;
    } else if(NULL == server_thread_v4)
    {
      LOG_MSG_INFO1("give null v4 thread", 0, 0, 0);
      return;
    } else if(NULL == server_thread_v6)
    {
      LOG_MSG_INFO1("give null v6 thread", 0, 0, 0);
      return;
    }

    if(pthread_create(server_thread_v4, NULL, listenForSocks5Clientv4,
                      (void*)configuration->getLANIface()) != 0)
    {
      LOG_MSG_INFO1("error creating server pthread v4 on iface: %s, %s",
                    configuration->getLANIface(), strerror(errno), 0);
    } else {
      lan_server_up = true;
    }

    if(pthread_create(server_thread_v6, NULL, listenForSocks5Clientv6,
                      (void*)configuration->getLANIface()) != 0)
    {
      LOG_MSG_INFO1("error creating server pthread v6 on iface: %s, %s",
                    configuration->getLANIface(), strerror(errno), 0);
    } else {
      lan_server_up = true;
    }
}

/*==========================================================================
  FUNCTION checkLANIface
==========================================================================*/
/*!
@brief
  ioctl to light weight check LAN iface status

@parameters
  SOCKSv5 configuration object
  IPv4 LAN pthread
  IPv6 LAN pthread

@return
*/
/*========================================================================*/
void checkLANIface
(
QC_SOCKSv5_Proxy_Configuration* configuration,
pthread_t* server_thread_v4,
pthread_t* server_thread_v6
)
{
  int ioctl_sock;
  struct ifreq ifr;

  if(NULL == configuration)
  {
    LOG_MSG_INFO1("give null configuration", 0, 0, 0);
    return;
  } else if(NULL == server_thread_v4)
  {
    LOG_MSG_INFO1("give null v4 thread", 0, 0, 0);
    return;
  } else if(NULL == server_thread_v6)
  {
    LOG_MSG_INFO1("give null v6 thread", 0, 0, 0);
    return;
  }

  if((ioctl_sock = socket(AF_UNIX, SOCK_DGRAM, 0)) <= -1)
  {
    LOG_MSG_INFO1("error creating ioctl socket: %s", strerror(errno), 0, 0);
    return;
  }

  if(strlen(configuration->getLANIface()) >= IFNAMSIZ)
  {
    LOG_MSG_INFO1("error LAN Iface name too long: %s", configuration->getLANIface(), 0, 0);
    if(close(ioctl_sock))
    {
      LOG_MSG_INFO1("error with closing socket: %s", strerror(errno), 0, 0);
    }
    return;
  }

  memset(&ifr, 0, sizeof(struct ifreq));
  for(int i = 0; i < strlen(configuration->getLANIface()); i++)
  {
    ifr.ifr_name[i] = (configuration->getLANIface())[i];
  }

  if(ioctl(ioctl_sock, SIOCGIFFLAGS, &ifr) == -1)
  {
    LOG_MSG_INFO1("error with ioctl: %s", strerror(errno), 0 ,0);
    if(close(ioctl_sock))
    {
      LOG_MSG_INFO1("error with closing socket: %s", strerror(errno), 0, 0);
    }
    return;
  }
  if(close(ioctl_sock))
  {
    LOG_MSG_INFO1("error with closing socket: %s", strerror(errno), 0, 0);
  }

  //userspace ioctl ifr_flags is a short and thus cannot post events for IFF_LOWER_UP
  //relying on main netlink thread for IFF_LOWER_UP detection
  if(!(ifr.ifr_flags & IFF_UP))
  {
    LOG_MSG_INFO1("LAN iface down: %s : 0x%x", configuration->getLANIface(), ifr.ifr_flags, 0);
    LOG_MSG_INFO1("Killing LAN server threads...", 0, 0, 0);
    if(lan_server_up)
    {
      if(pthread_cancel(*server_thread_v4))
      {
        LOG_MSG_INFO1("pthread_cancel error: %s", strerror(errno), 0, 0);
      }
      if(pthread_cancel(*server_thread_v6))
      {
        LOG_MSG_INFO1("pthread_cancel error: %s", strerror(errno), 0, 0);
      }

      if(pthread_join(*server_thread_v4, NULL))
      {
        LOG_MSG_INFO1("pthread_join error: %s", strerror(errno), 0, 0);
      }
      if(pthread_join(*server_thread_v6, NULL))
      {
        LOG_MSG_INFO1("pthread_join error: %s", strerror(errno), 0, 0);
      }
    }
    lan_server_up = false;
  } else if(!lan_server_up) {
    LOG_MSG_INFO1("LAN iface up: %s", configuration->getLANIface(), 0, 0);
    spawnLANServerThreads(configuration, server_thread_v4, server_thread_v6);
  }

}

/*==========================================================================
  FUNCTION listenForSocks5Clientv4
==========================================================================*/
/*!
@brief
  LAN server pthread for IPv4 clients

@parameters
  LAN iface name

@return
*/
/*========================================================================*/
void* listenForSocks5Clientv4(void* iface_name)
{
  struct sockaddr_in serv_addr, cli_addr;
  int cli_addr_len;
  fd_set sock_read_fd_set;
  unsigned int enable = 1; //for setting sock options
  pthread_t t;
  //std::pair listening serv_sock* (first), cli_sock* (second)
  std::pair<int*, int*>* serv_cli_sock_fd_pair = NULL;

  if(NULL == iface_name)
  {
    LOG_MSG_INFO1("give null iface_name", 0, 0, 0);
    pthread_exit(0);
  }

  serv_cli_sock_fd_pair = new std::pair<int*, int*>(NULL, NULL);

  pthread_cleanup_push(cleanupLANThread, serv_cli_sock_fd_pair);

  if((serv_cli_sock_fd_pair->first = (int*)malloc(sizeof(int))) == NULL)
  {
    LOG_MSG_INFO1("error malloc serverv4 socket: %s", strerror(errno), 0, 0);
    pthread_exit(0);
  }

  if((*serv_cli_sock_fd_pair->first = socket(AF_INET, SOCK_STREAM, 0)) <= -1)
  {
    LOG_MSG_INFO1("error creating serverv4 socket: %s", strerror(errno), 0, 0);
    pthread_exit(0);
  }

  //clears
  memset(&serv_addr, 0, sizeof(struct sockaddr_in));

  //fill out address info
  //Note: no need to specify IP addr because we will BINDTODEVICE
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PROXY_PORT); //proxy server port

  //setting sock options
  if(setsockopt(*serv_cli_sock_fd_pair->first, SOL_SOCKET, SO_REUSEADDR, &enable,
                sizeof(socklen_t)) <= -1)
  {
    LOG_MSG_INFO1("error setting socket options: %s", strerror(errno), 0, 0);
    pthread_exit(0);
  }
  if(setsockopt(*serv_cli_sock_fd_pair->first, SOL_SOCKET, SO_REUSEPORT, &enable,
                sizeof(socklen_t)) <= -1)
  {
    LOG_MSG_INFO1("error setting socket options: %s", strerror(errno), 0, 0);
    pthread_exit(0);
  }
  if(setsockopt(*serv_cli_sock_fd_pair->first, SOL_SOCKET, SO_BINDTODEVICE, iface_name,
                strlen((char*)iface_name)) <= -1)
  {
    LOG_MSG_INFO1("error binding socket to iface %s: %s", (char*)iface_name, strerror(errno), 0);
    pthread_exit(0);
  }

  //bind, to specify AF_INET and PROXY_PORT to listen and accept
  if(bind(*serv_cli_sock_fd_pair->first, (struct sockaddr*)&serv_addr,
          sizeof(struct sockaddr_in)) <= -1)
  {
    LOG_MSG_INFO1("error binding IPv4 to socket: %s", strerror(errno), 0, 0);
    pthread_exit(0);
  }

  //print LAN iface name
  LOG_MSG_INFO1("LAN iface v4: %s", (char*)iface_name, 0, 0);
  LOG_MSG_INFO1("Listening for client v4 connections on %s:1080...", (char*)iface_name, 0, 0);

  //Up to 5 clients
  if(listen(*serv_cli_sock_fd_pair->first, MAX_LISTEN_CLIENTS) <= -1)
  {
    LOG_MSG_INFO1("error when listening on socket: %s", strerror(errno), 0, 0);
    pthread_exit(0);
  }

  cli_addr_len = sizeof(cli_addr);

  //main loop
  while(1)
  {
    //freed in handleSocks5Client or cleanupHandleSocks5Client
    if((serv_cli_sock_fd_pair->second = (int*)malloc(sizeof(int))) == NULL)
    {
      LOG_MSG_INFO1("Error with malloc", 0, 0, 0);
      continue;
    }

    //setup file descriptors for select call
    FD_ZERO(&sock_read_fd_set);
    FD_SET(*serv_cli_sock_fd_pair->first, &sock_read_fd_set);

    //block until we recv something over the sock
    if(select(*serv_cli_sock_fd_pair->first + 1, &sock_read_fd_set, NULL, NULL, NULL) < 0)
    {
      LOG_MSG_INFO1("select call failed for receiving from client socket", 0, 0, 0);
      free(serv_cli_sock_fd_pair->second);
      continue;
    } else {

      if((*serv_cli_sock_fd_pair->second = accept(*serv_cli_sock_fd_pair->first,
          (struct sockaddr*) &cli_addr, (socklen_t *)&cli_addr_len)) < 0)
      {
        LOG_MSG_INFO1("problem accepting a client TCP handshake", 0, 0, 0);
        free(serv_cli_sock_fd_pair->second);
        continue;
      }

      //insert a new sock pair, sock_remote set to -1 and will be filled out later
      cli_req_sock_pair_map_mutex.lock();
      cli_req_sock_pair_map.insert(std::pair<int, int>(*serv_cli_sock_fd_pair->second, -1));
      cli_req_sock_pair_map_mutex.unlock();

      //create a pthread to handle the client
      if(pthread_create(&t, NULL, handleSocks5Client, serv_cli_sock_fd_pair->second) != 0)
      {
        LOG_MSG_INFO1("error creating pthread! %s", strerror(errno), 0, 0);
      }
    }
  }

  pthread_cleanup_pop(1);

  pthread_exit(0);
}

/*==========================================================================
  FUNCTION listenForSocks5Clientv6
==========================================================================*/
/*!
@brief
  LAN server pthread for IPv6 clients

@parameters
  LAN iface name

@return
*/
/*========================================================================*/
void* listenForSocks5Clientv6(void* iface_name)
{
  struct sockaddr_in6 serv_addr, cli_addr;
  int cli_addr_len;
  fd_set sock_read_fd_set;
  unsigned int enable = 1; //for setting sock options
  pthread_t t;
  //std::pair listening serv_sock* (first), cli_sock* (second)
  std::pair<int*, int*>* serv_cli_sock_fd_pair = NULL;

  if(NULL == iface_name)
  {
    LOG_MSG_INFO1("give null iface_name", 0, 0, 0);
    pthread_exit(0);
  }

  serv_cli_sock_fd_pair = new std::pair<int*, int*>(NULL, NULL);

  pthread_cleanup_push(cleanupLANThread, serv_cli_sock_fd_pair);

  if((serv_cli_sock_fd_pair->first = (int*)malloc(sizeof(int))) == NULL)
  {
    LOG_MSG_INFO1("error malloc serverv6 socket: %s", strerror(errno), 0, 0);
    pthread_exit(0);
  }

  if((*serv_cli_sock_fd_pair->first = socket(AF_INET6, SOCK_STREAM, 0)) <= -1)
  {
    LOG_MSG_INFO1("error creating serverv6 socket: %s", strerror(errno), 0, 0);
    pthread_exit(0);
  }

  //clears
  memset(&serv_addr, 0, sizeof(struct sockaddr_in6));

  //fill out address info
  //Note: no need to specify IP addr because we will BINDTODEVICE
  serv_addr.sin6_family = AF_INET6;
  serv_addr.sin6_port = htons(PROXY_PORT); //proxy server port

  if(setsockopt(*serv_cli_sock_fd_pair->first, SOL_SOCKET, SO_REUSEADDR, &enable,
     sizeof(socklen_t)) <= -1)
  {
    LOG_MSG_INFO1("error setting socket options: %s", strerror(errno), 0, 0);
    pthread_exit(0);
  }
  if(setsockopt(*serv_cli_sock_fd_pair->first, SOL_SOCKET, SO_REUSEPORT, &enable,
     sizeof(socklen_t)) <= -1)
  {
    LOG_MSG_INFO1("error setting socket options: %s", strerror(errno), 0, 0);
    pthread_exit(0);
  }
  if(setsockopt(*serv_cli_sock_fd_pair->first, SOL_SOCKET, SO_BINDTODEVICE, iface_name,
                strlen((char*)iface_name)) <= -1)
  {
    LOG_MSG_INFO1("error binding socket to iface %s: %s", (char*)iface_name, strerror(errno), 0);
    pthread_exit(0);
  }

  //bind, to specify AF_INET and PROXY_PORT to listen and accept
  if(bind(*serv_cli_sock_fd_pair->first, (struct sockaddr*)&serv_addr,
          sizeof(struct sockaddr_in6)) <= -1)
  {
    LOG_MSG_INFO1("error binding IPv6 to socket: %s", strerror(errno), 0, 0);
    pthread_exit(0);
  }

  //print LAN iface name
  LOG_MSG_INFO1("LAN iface v6: %s", (char*)iface_name, 0, 0);
  LOG_MSG_INFO1("Listening for client v6 connections on %s:1080...", (char*)iface_name, 0, 0);

  //Up to 5 clients
  if(listen(*serv_cli_sock_fd_pair->first, MAX_LISTEN_CLIENTS) <= -1)
  {
    LOG_MSG_INFO1("error when listening on socket: %s", errno, 0, 0);
    pthread_exit(0);
  }

  cli_addr_len = sizeof(cli_addr);

  //main loop
  while(1)
  {
    //freed in handleSocks5Client or cleanupHandleSocks5Client
    if((serv_cli_sock_fd_pair->second = (int*)malloc(sizeof(int))) == NULL)
    {
      LOG_MSG_INFO1("Error with malloc", 0, 0, 0);
      continue;
    }

    //setup file descriptors for select call
    FD_ZERO(&sock_read_fd_set);
    FD_SET(*serv_cli_sock_fd_pair->first, &sock_read_fd_set);

    //block until we recv something over the sock
    if(select(*serv_cli_sock_fd_pair->first + 1, &sock_read_fd_set, NULL, NULL, NULL) < 0)
    {
      LOG_MSG_INFO1("select call failed for receiving from client socket", 0, 0, 0);
      free(serv_cli_sock_fd_pair->second);
      continue;
    } else {

      if((*serv_cli_sock_fd_pair->second = accept(*serv_cli_sock_fd_pair->first,
         (struct sockaddr*) &cli_addr, (socklen_t *)&cli_addr_len)) < 0)
      {
        LOG_MSG_INFO1("problem accepting a client TCP handshake", 0, 0, 0);
        free(serv_cli_sock_fd_pair->second);
        continue;
      }

      //insert a new sock pair, sock_remote set to -1 and will be filled out later
      cli_req_sock_pair_map_mutex.lock();
      cli_req_sock_pair_map.insert(std::pair<int, int>(*serv_cli_sock_fd_pair->second, -1));
      cli_req_sock_pair_map_mutex.unlock();

      //create a pthread to handle the client
      if(pthread_create(&t, NULL, handleSocks5Client, serv_cli_sock_fd_pair->second) != 0)
      {
        LOG_MSG_INFO1("error creating pthread! %s", strerror(errno), 0, 0);
      }

    }
  }

  pthread_cleanup_pop(1);

  pthread_exit(0);
}

/*==========================================================================
  FUNCTION handleSocks5Client
==========================================================================*/
/*!
@brief
  client handling pthread following SOCKSv5 RFC 1928 protocol

@parameters
  pair of sockets for relaying between remote and client

@return
*/
/*========================================================================*/
void* handleSocks5Client(void* arg)
{
  unsigned char buffer[SOCKSV5_MAX_PAYLOAD]; //buffer will be used for reading from sockets
  unsigned char auth_method = NOT_ACCEPTABLE; //default to not acceptable
  int sock_remote;
  int cli_sock;
  char* egress_wan_iface = NULL;
  struct socksv5_packet* id_reply = NULL;
  fd_set cli_read_fd_set; //for select to monitor file descriptors
  int cli_to_req_pipe[2];
  int num_bytes_recv;
  struct linger linger_opt = {1, 0};
  unsigned int enable = 1;
  struct relay_session_tcp_splice t_splice_session;

  /* Socket Splice fallback variables */
  struct relay_session_socket_splice socket_splice_session;
  int ep_fd, num_of_events;
  struct epoll_event read_event;
  struct epoll_event events[MAX_NUM_EPOLL_EVENTS];

  if(NULL == arg)
  {
    LOG_MSG_INFO1("given null sock_pair_index", 0, 0, 0);
    pthread_exit(0);
  }
  if(NULL == configuration)
  {
    LOG_MSG_INFO1("given null configuration", 0, 0, 0);
    free(arg);
    pthread_exit(0);
  }

  cli_sock = *((int*)arg);

  //push cleanup routine for this pthread
  pthread_cleanup_push(cleanupHandleSocks5Client, arg);

  //independent thread, free after we terminate this
  if(pthread_detach(pthread_self()) != 0)
  {
    LOG_MSG_INFO1("problem with detaching pthread!", 0, 0, 0);
    pthread_exit(0);
  }

  num_of_critical_threads++;

  /*** Step 1: Select a method the client supports for authentication ***/
  if(recv(cli_sock, buffer, SOCKSV5_MAX_PAYLOAD_METHODS * sizeof(unsigned char), 0) <= -1)
  {
    LOG_MSG_INFO1("error receiving from client socket: %s", strerror(errno), 0, 0);
    pthread_exit(0);
  }

  //check the version number
  if(buffer[0] != SOCKSV5_VERSION)
  {
    LOG_MSG_INFO1("anything not SOCKSv5 is not supported!", 0, 0, 0);
    pthread_exit(0);
  }

  //Check number of methods field
  //printf("Client NMETHODS: %u\n", buffer[1]);

  //either the preferred auth method, or not
  for(int i = 0; i < buffer[1]; i++)
  {
    if(buffer[i + 2] == configuration->getPrefAuthMethod())
    {
      auth_method = buffer[i + 2];
      break;
    }
  }

  //std::cout << "Selected authentication method: ";

  //handle the authentication method
  switch(auth_method)
  {
    case(Pref_Auth_Method::NO_AUTH): //no authentication required
    {
      //std::cout << "NO AUTHENTICATION REQUIRED" << std::endl;

      if((id_reply =
         QC_SOCKSv5_Pkt_Factory::createMethodSelectionReply(Pref_Auth_Method::NO_AUTH)) == NULL)
      {
        LOG_MSG_INFO1("error creating method selection reply: %u",
                      Pref_Auth_Method::NO_AUTH, 0, 0);
        pthread_exit(0);
      }

      if(send(cli_sock, id_reply->payload, id_reply->payload_size, 0) <= -1)
      {
        LOG_MSG_INFO1("error writing to client socket", 0, 0, 0);
        QC_SOCKSv5_Pkt_Factory::freeSocksv5Pkt(id_reply);
        pthread_exit(0);
      }
      QC_SOCKSv5_Pkt_Factory::freeSocksv5Pkt(id_reply);
      break;
    }
    case(Pref_Auth_Method::GSSAPI):
    {
      LOG_MSG_INFO1("GSSAPI auth is not supported...", 0, 0, 0);

      if((id_reply =
      QC_SOCKSv5_Pkt_Factory::createMethodSelectionReply(Pref_Auth_Method::NOT_ACCEPTABLE)) == NULL)
      {
        LOG_MSG_INFO1("error creating method selection reply: %u",
                      Pref_Auth_Method::NOT_ACCEPTABLE, 0, 0);
        pthread_exit(0);
      }

      if(send(cli_sock, id_reply->payload, id_reply->payload_size, 0) <= -1)
      {
        LOG_MSG_INFO1("error writing to client socket", 0, 0, 0);
      }
      QC_SOCKSv5_Pkt_Factory::freeSocksv5Pkt(id_reply);
      pthread_exit(0);
      break;
    }
    case(Pref_Auth_Method::UNAME_PASSWD):  //USERNAME/PASSWORD RFC 1929
    {
      //std::cout << "USERNAME/PASSWORD" << std::endl;
      if((id_reply =
        QC_SOCKSv5_Pkt_Factory::createMethodSelectionReply(Pref_Auth_Method::UNAME_PASSWD)) == NULL)
      {
        LOG_MSG_INFO1("error creating method selection reply: %u",
                      Pref_Auth_Method::UNAME_PASSWD, 0, 0);
        pthread_exit(0);
      }

      if(send(cli_sock, id_reply->payload, id_reply->payload_size, 0) <= -1)
      {
        LOG_MSG_INFO1("error writing to client socket", 0, 0, 0);
        QC_SOCKSv5_Pkt_Factory::freeSocksv5Pkt(id_reply);
        pthread_exit(0);
      }
      QC_SOCKSv5_Pkt_Factory::freeSocksv5Pkt(id_reply);

      //recv username password from client
      memset(&buffer, 0x00, SOCKSV5_MAX_PAYLOAD * sizeof(unsigned char)); //clear buffer
      if(recv(cli_sock, &buffer, SOCKSV5_MAX_PAYLOAD * sizeof(unsigned char), 0) <= -1)
      {
        LOG_MSG_INFO1("error writing to client socket", 0, 0, 0);
        pthread_exit(0);
      }

      //Note: we are customizing this specific package for a customer
      //will be doing uname multi-pdn routing
      //egress_wan_iface shall be pointing to a wan_iface string
      if(QC_SOCKSv5_Util::handleUnameWANMapAuth(buffer, cli_sock, &egress_wan_iface,
                                                configuration) != 0)
      {
        pthread_exit(0);
      }

      LOG_MSG_INFO1("Will use egress wan iface: %s", egress_wan_iface, 0, 0);

      break;
    }
    default:
    {
      //NOTE: if in the range of 0x03 - 0x7F -> IANA assigned internet assigned
      //numbers authority
      //if 0x80 - 0xfe -> we define can define our own way of server-client auth
      //std::cout << "NO ACCEPTABLE METHODS" << std::endl;
      if((id_reply =
      QC_SOCKSv5_Pkt_Factory::createMethodSelectionReply(Pref_Auth_Method::NOT_ACCEPTABLE)) == NULL)
      {
        LOG_MSG_INFO1("error creating method selection reply: %u",
                      Pref_Auth_Method::NOT_ACCEPTABLE, 0, 0);
        pthread_exit(0);
      }
      if(send(cli_sock, id_reply->payload, id_reply->payload_size, 0) <= -1)
      {
        LOG_MSG_INFO1("error writing to client socket", 0, 0, 0);
      }
      QC_SOCKSv5_Pkt_Factory::freeSocksv5Pkt(id_reply);
      pthread_exit(0);
      break;
    }
  }

  //std::cout << "Will use egress_wan_iface: " << egress_wan_iface << std::endl;

  /*** Step 2: Proceed to receiving the client's request to reach an external destination. ***/

  memset(&buffer, 0x00, SOCKSV5_MAX_PAYLOAD_REQUESTS * sizeof(unsigned char)); //clear buffer
  if(recv(cli_sock, &buffer, SOCKSV5_MAX_PAYLOAD_REQUESTS * sizeof(unsigned char), 0) <= -1)
  {
    LOG_MSG_INFO1("error writing to client socket", 0, 0, 0);
    pthread_exit(0);
  }

  //Check request code
  switch(buffer[1])
  {
    case(Cmd_Request::CONNECT):
    {
      if(sem_wait(&socks_conn_req_sem))
      {
        LOG_MSG_INFO1("error with semaphore wait: %s", strerror(errno), 0, 0);
        pthread_exit(0);
      }

      sock_remote = QC_SOCKSv5_Util::handleConnectRequest(buffer, cli_sock, egress_wan_iface,
                                                          configuration);

      if(sem_post(&socks_conn_req_sem))
      {
        LOG_MSG_INFO1("error with semaphore post: %s", strerror(errno), 0, 0);
        pthread_exit(0);
      }

      if(sock_remote <= 0)
      {
        LOG_MSG_INFO1("error with handle connect request", 0, 0, 0);
        pthread_exit(0);
      }

      break;
    }

    case(Cmd_Request::BIND):
    {
      LOG_MSG_INFO1("Not handling Bind request yet", 0, 0, 0);
      pthread_exit(0);
      break;
    }

    case(Cmd_Request::UDP_ASSOCIATE):
    {
      LOG_MSG_INFO1("Not handling UDP Associate request yet", 0, 0, 0);
      pthread_exit(0);
      break;
    }

    default:
    {
      QC_SOCKSv5_Util::handleUnsupportedRequest(buffer, cli_sock);
      pthread_exit(0);
    }
  }

  cli_req_sock_pair_map_mutex.lock();
  cli_req_sock_pair_map[cli_sock] = sock_remote;
  cli_req_sock_pair_map_mutex.unlock();

  /*** Next Steps: Relay back and forth between client and requested destination ***/

  if(tcp_splice_mode)
  {
    //setup file descriptors for select call
    FD_ZERO(&cli_read_fd_set);
    FD_SET(cli_sock, &cli_read_fd_set);

    //wait for the first packet to relay
    //client sent something to proxy server
    if(select(cli_sock + 1, &cli_read_fd_set, NULL, NULL, NULL) < 0)
    {
      LOG_MSG_INFO1("select call failed for receiving from client socket\n", 0, 0, 0);
      pthread_exit(0);
    }

    /* Set up TCP splicing */
    t_splice_session = {cli_sock, sock_remote};
    //setsockopt for splicing here
    if(setsockopt(cli_sock, IPPROTO_IP, SO_TCP_SPLICE, &t_splice_session,
                  sizeof(struct relay_session_tcp_splice)) <= -1)
    {
      LOG_MSG_INFO1("error setting tcp splice\n", 0, 0, 0);
      pthread_exit(0);
    }

    /* Set up Socket splicing */
    /* Only for the first packet to be relayed */
    if(pipe(cli_to_req_pipe) == -1)
    {
      LOG_MSG_INFO1("cli_to_req: issue with creating pipe...\n", 0, 0, 0);
      pthread_exit(0);
    }

    if(likely((num_bytes_recv = splice(cli_sock, NULL, cli_to_req_pipe[1], NULL,
              MAX_SOCKET_BUFFER_SIZE * sizeof(unsigned char), SPLICE_F_NONBLOCK)) > 0))
    {
      if(splice(cli_to_req_pipe[0], NULL, sock_remote, NULL, num_bytes_recv, SPLICE_F_NONBLOCK) < 0)
      {
        LOG_MSG_INFO1("error relaying cli_to_req pipe...\n", 0, 0, 0);
        if(close(cli_to_req_pipe[0]))
        {
          LOG_MSG_INFO1("error with closing socket: %s", strerror(errno), 0, 0);
        }
        if(close(cli_to_req_pipe[1]))
        {
          LOG_MSG_INFO1("error with closing socket: %s", strerror(errno), 0, 0);
        }
        pthread_exit(0);
      }
    }

    if(close(cli_to_req_pipe[0]))
    {
      LOG_MSG_INFO1("error with closing socket: %s", strerror(errno), 0, 0);
    }
    if(close(cli_to_req_pipe[1]))
    {
      LOG_MSG_INFO1("error with closing socket: %s", strerror(errno), 0, 0);
    }

    //setsockopt to don't LINGER
    if(setsockopt(cli_sock, SOL_SOCKET, SO_LINGER, &linger_opt, sizeof(struct linger)) <= -1)
    {
      LOG_MSG_INFO1("error setting linger timeout on client socket", 0, 0, 0);
      pthread_exit(0);
    }
    if(setsockopt(sock_remote, SOL_SOCKET, SO_LINGER, &linger_opt, sizeof(struct linger)) <= -1)
    {
      LOG_MSG_INFO1("error setting linger timeout on req socket", 0, 0, 0);
      pthread_exit(0);
    }

    //setsockopt for TCP KEEPALIVE
    if(setsockopt(cli_sock, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(socklen_t)) <= -1)
    {
      LOG_MSG_INFO1("error setting keep alive on client socket", 0, 0, 0);
      pthread_exit(0);
    }
    if(setsockopt(sock_remote, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(socklen_t)) <= -1)
    {
      LOG_MSG_INFO1("error setting keep alive on req socket", 0, 0, 0);
      pthread_exit(0);
    }

  } else {

    /* Set up Socket splicing */

    //setsockopt to don't LINGER
    if(setsockopt(cli_sock, SOL_SOCKET, SO_LINGER, &linger_opt, sizeof(struct linger)) <= -1)
    {
      LOG_MSG_INFO1("error setting linger timeout on client socket: %s", strerror(errno), 0, 0);
      pthread_exit(0);
    }
    if(setsockopt(sock_remote, SOL_SOCKET, SO_LINGER, &linger_opt, sizeof(struct linger)) <= -1)
    {
      LOG_MSG_INFO1("error setting linger timeout on req socket: %s", strerror(errno), 0, 0);
      pthread_exit(0);
    }

    //setsockopt for TCP KEEPALIVE
    if(setsockopt(cli_sock, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(socklen_t)) <= -1)
    {
      LOG_MSG_INFO1("error setting keep alive on client socket: %s", strerror(errno), 0, 0);
      pthread_exit(0);
    }
    if(setsockopt(sock_remote, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(socklen_t)) <= -1)
    {
      LOG_MSG_INFO1("error setting keep alive on req socket: %s", strerror(errno), 0, 0);
      pthread_exit(0);
    }

    //assign socket fds and pipes
    socket_splice_session.cli_sock = cli_sock;
    socket_splice_session.sock_remote = sock_remote;
    socket_splice_session.done = false;

    if(pipe(socket_splice_session.cli_to_req_pipe) == -1)
    {
      LOG_MSG_INFO1("cli_to_req: issue with creating pipe: %s", strerror(errno), 0, 0);
      pthread_exit(0);
    }

    if(pipe(socket_splice_session.req_to_cli_pipe) == -1)
    {
      LOG_MSG_INFO1("req_to_cli: issue with creating pipe: %s", strerror(errno), 0, 0);
      if(close(socket_splice_session.cli_to_req_pipe[0]))
      {
        LOG_MSG_INFO1("Error with close: %s", strerror(errno), 0, 0);
      }
      if(close(socket_splice_session.cli_to_req_pipe[1]))
      {
        LOG_MSG_INFO1("Error with close: %s", strerror(errno), 0, 0);
      }
      pthread_exit(0);
    }

    pthread_cleanup_push(cleanupSocketSplice, &socket_splice_session);

    //get ready for epoll events
    if((ep_fd = epoll_create(1)) < 0)
    {
      LOG_MSG_INFO1("error creating epoll fd: %s\n", strerror(errno), 0, 0);
      pthread_exit(0);
    }

    //add cli sock
    memset(&read_event, 0, sizeof(struct epoll_event));
    read_event.events = EPOLLIN;
    read_event.data.fd = socket_splice_session.cli_sock;
    if(epoll_ctl(ep_fd, EPOLL_CTL_ADD, socket_splice_session.cli_sock, &read_event) < 0)
    {
      LOG_MSG_INFO1("error adding client socket fd to epoll fd: %s\n", strerror(errno), 0, 0);
      if(close(ep_fd))
      {
        LOG_MSG_INFO1("error closing epoll fd: %s", strerror(errno), 0, 0);
      }
      pthread_exit(0);
    }

    //add sock remote
    memset(&read_event, 0, sizeof(struct epoll_event));
    read_event.events = EPOLLIN;
    read_event.data.fd = socket_splice_session.sock_remote;
    if(epoll_ctl(ep_fd, EPOLL_CTL_ADD, socket_splice_session.sock_remote, &read_event) < 0)
    {
      LOG_MSG_INFO1("error adding remote socket fd to epoll fd: %s\n", strerror(errno), 0, 0);
      if(epoll_ctl(ep_fd, EPOLL_CTL_DEL, socket_splice_session.cli_sock, NULL) < 0)
      {
        LOG_MSG_INFO1("error deleting client socket fd from epoll fd: %s\n", strerror(errno), 0, 0);
      }
      if(close(ep_fd))
      {
        LOG_MSG_INFO1("error closing epoll fd: %s", strerror(errno), 0, 0);
      }
      pthread_exit(0);
    }

    while(!socket_splice_session.done)
    {
      num_of_events = 0;
      if((num_of_events = epoll_wait(ep_fd, events, MAX_NUM_EPOLL_EVENTS, -1)) < 0)
      {
        LOG_MSG_INFO1("error with epoll_wait: %s\n", strerror(errno), 0, 0);
        socket_splice_session.done = true;
        break;
      }

      for(int i = 0; i < num_of_events; i++)
      {
        if(events[i].data.fd == socket_splice_session.cli_sock)
        {

          if(likely((num_bytes_recv = splice(socket_splice_session.cli_sock, NULL,
                                             socket_splice_session.cli_to_req_pipe[1], NULL,
                                             sizeof(buffer), SPLICE_F_NONBLOCK)) > 0))
          {
            if(splice(socket_splice_session.cli_to_req_pipe[0], NULL,
                      socket_splice_session.sock_remote, NULL, num_bytes_recv,
                      SPLICE_F_NONBLOCK) < 0)
            {
              LOG_MSG_INFO1("error relaying cli_to_req pipe: %s", strerror(errno), 0, 0);
              socket_splice_session.done = true;
            }
          } else if(num_bytes_recv == 0) {
            socket_splice_session.done = true;
          } else {
            LOG_MSG_INFO1("error relaying cli_to_req pipe: %s", strerror(errno), 0, 0);
            socket_splice_session.done = true;
          }

        } else if(events[i].data.fd == socket_splice_session.sock_remote) {

          if(likely((num_bytes_recv = splice(socket_splice_session.sock_remote, NULL,
                                             socket_splice_session.req_to_cli_pipe[1],
                                             NULL, sizeof(buffer), SPLICE_F_NONBLOCK)) > 0))
          {
            if(splice(socket_splice_session.req_to_cli_pipe[0], NULL,
                      socket_splice_session.cli_sock, NULL, num_bytes_recv,
                      SPLICE_F_NONBLOCK) < 0)
            {
              LOG_MSG_INFO1("error relaying req_to_cli pipe: %s", strerror(errno), 0, 0);
              socket_splice_session.done = true;
            }
          } else if(num_bytes_recv == 0) {
            socket_splice_session.done = true;
          } else {
            LOG_MSG_INFO1("error relaying req_to_cli pipe: %s", strerror(errno), 0, 0);
            socket_splice_session.done = true;
          }
        }
      }
    }

    if(epoll_ctl(ep_fd, EPOLL_CTL_DEL, socket_splice_session.cli_sock, NULL) < 0)
    {
      LOG_MSG_INFO1("error deleting client socket fd from epoll fd: %s\n", strerror(errno), 0, 0);
    }
    if(epoll_ctl(ep_fd, EPOLL_CTL_DEL, socket_splice_session.sock_remote, NULL) < 0)
    {
      LOG_MSG_INFO1("error deleting remote socket fd from epoll fd: %s\n", strerror(errno), 0, 0);
    }
    if(close(ep_fd))
    {
      LOG_MSG_INFO1("error closing epoll fd: %s", strerror(errno), 0, 0);
    }

    pthread_cleanup_pop(1);

  }

  pthread_cleanup_pop(0); //will cleanup later either in relay thread or via garbage collector

  num_of_critical_threads--;

  free(arg);

  pthread_exit(0);
}

/*==========================================================================
  FUNCTION cleanupSocketSplice
==========================================================================*/
/*!
@brief
  cleans up lan server thread

@parameters
  std::pair listening serv_sock* (first), cli_sock* (second)

@return
*/
/*========================================================================*/
void cleanupSocketSplice(void* arg)
{
  struct relay_session_socket_splice* r_session = NULL;

  if(NULL == arg)
  {
    LOG_MSG_INFO1("given null socket splice session info %s", __func__, 0, 0);
    return;
  }

  r_session = (struct relay_session_socket_splice*)arg;

  if(close(r_session->cli_to_req_pipe[0]))
  {
    LOG_MSG_INFO1("Error with close: %s", strerror(errno), 0, 0);
  }
  if(close(r_session->cli_to_req_pipe[1]))
  {
    LOG_MSG_INFO1("Error with close: %s", strerror(errno), 0, 0);
  }
  if(close(r_session->req_to_cli_pipe[0]))
  {
    LOG_MSG_INFO1("Error with close: %s", strerror(errno), 0, 0);
  }
  if(close(r_session->req_to_cli_pipe[1]))
  {
    LOG_MSG_INFO1("Error with close: %s", strerror(errno), 0, 0);
  }
  if(close(r_session->cli_sock))
  {
    LOG_MSG_INFO1("Error with close: %s", strerror(errno), 0, 0);
  }
  if(close(r_session->sock_remote))
  {
    LOG_MSG_INFO1("Error with close: %s", strerror(errno), 0, 0);
  }

  //erase from map
  cli_req_sock_pair_map_mutex.lock();
  if(cli_req_sock_pair_map.find(r_session->cli_sock) != cli_req_sock_pair_map.end())
  {
    cli_req_sock_pair_map.erase(r_session->cli_sock); //erase from hash table
  }
  cli_req_sock_pair_map_mutex.unlock();

  return;
}

/*==========================================================================
  FUNCTION cleanupLANThread
==========================================================================*/
/*!
@brief
  cleans up lan server thread

@parameters
  std::pair listening serv_sock* (first), cli_sock* (second)

@return
*/
/*========================================================================*/
void cleanupLANThread(void* arg)
{
  std::pair<int*, int*>* serv_cli_sock_fd_pair = NULL;

  if(NULL == arg)
  {
    LOG_MSG_INFO1("given null std::pair in %s", __func__, 0, 0);
    return;
  }

  serv_cli_sock_fd_pair = (std::pair<int*, int*>*)arg;

  if(serv_cli_sock_fd_pair->first)
  {
    if(close(*serv_cli_sock_fd_pair->first))
    {
      LOG_MSG_INFO1("error with closing socket: %s", strerror(errno), 0, 0);
    } else {
      LOG_MSG_INFO1("closed a lan server socket", 0, 0, 0);
    }
    free(serv_cli_sock_fd_pair->first);
  }

  if(serv_cli_sock_fd_pair->second)
  {
    if(close(*serv_cli_sock_fd_pair->second)) //close cli_sock
    {
      LOG_MSG_INFO1("error with closing socket: %s", strerror(errno), 0, 0);
    }
    cli_req_sock_pair_map_mutex.lock();
    if(cli_req_sock_pair_map.find(*serv_cli_sock_fd_pair->second) != cli_req_sock_pair_map.end())
    {
      cli_req_sock_pair_map.erase(*serv_cli_sock_fd_pair->second); //erase from hash table
    }
    cli_req_sock_pair_map_mutex.unlock();

    free(serv_cli_sock_fd_pair->second);
  }

  delete serv_cli_sock_fd_pair;

  return;
}

/*==========================================================================
  FUNCTION cleanupHandleSocks5Client
==========================================================================*/
/*!
@brief
  cleans up client handling pthread following SOCKSv5 RFC 1928 protocol

@parameters
  pair of sockets for relaying between remote and client

@return
*/
/*========================================================================*/
void cleanupHandleSocks5Client(void* arg)
{
  int* cli_sock = (int*)arg;

  if(NULL == arg)
  {
    LOG_MSG_INFO1("given null sock in %s", __func__, 0, 0);
    return;
  }

  cli_req_sock_pair_map_mutex.lock();
  if(cli_req_sock_pair_map.find(*cli_sock) != cli_req_sock_pair_map.end())
  {
    if(-1 != cli_req_sock_pair_map[*cli_sock])
    {
      if(close(cli_req_sock_pair_map[*cli_sock])) //close sock_remote
      {
        LOG_MSG_INFO1("error with closing socket: %s", strerror(errno), 0, 0);
      }
    }
    if(close(*cli_sock)) //close cli_sock
    {
      LOG_MSG_INFO1("error with closing socket: %s", strerror(errno), 0, 0);
    }
    cli_req_sock_pair_map.erase(*cli_sock); //erase from hash table
  }
  cli_req_sock_pair_map_mutex.unlock();

  free(arg);

  num_of_critical_threads--;

  return;
}

/*==========================================================================
  FUNCTION printUsage
==========================================================================*/
/*!
@brief
  prints usage of main controller

@parameters

@return
*/
/*========================================================================*/
void printUsage(void)
{
  printf("\nUsage:   ./qti_socksv5 -c <config_file> -u <auth_file>\n");
  printf("\t\t\t-c, use config file\n");
  printf("\t\t\t-u, use auth file\n");

  return;
}

/*==========================================================================
  FUNCTION handleSigPipe
==========================================================================*/
/*!
@brief
  handles SIGPIPE signal from kernel when send or recv is called on a socket
  connection that is dead by calling garbage collector

@parameters
  the signal

@return
*/
/*========================================================================*/
void handleSigPipe(int sig)
{
  //garbage collect dead sockets
  QC_SOCKSv5_Garbage_Collector::collectGarbage(&cli_req_sock_pair_map);
  return;
}

/*==========================================================================
  FUNCTION monitorConfFile
==========================================================================*/
/*!
@brief
  monitor config file via inotify to know when to update the configuration

@parameters
  path to config file

@return
*/
/*========================================================================*/
void* monitorConfFile(void* conf_file)
{
  int fd, wd; //file descriptor and watch descriptor
  fd_set read_fd_set;
  struct inotify_event event;

  if(NULL == conf_file)
  {
    LOG_MSG_INFO1("given a null conf_file", 0, 0, 0);
    pthread_exit(0);
  }

  while(1)
  {
    if((fd = inotify_init()) == -1)
    {
      LOG_MSG_INFO1("Error intializing inotify: %s", strerror(errno), 0, 0);
      continue;
    }

    //watch descriptor for writing and then closing of file events
    if((wd = inotify_add_watch(fd, (char*)conf_file, IN_CLOSE_WRITE)) < 0)
    {
      LOG_MSG_INFO1("Error adding inotify watch descriptor: %s", strerror(errno), 0, 0);
      if(close(fd))
      {
        LOG_MSG_INFO1("error with closing socket: %s", strerror(errno), 0, 0);
      }
      continue;
    }

    //setup file descriptors for select call
    FD_ZERO(&read_fd_set);
    FD_SET(fd, &read_fd_set);

    //block until we recv an event
    if(select(fd + 1, &read_fd_set, NULL, NULL, NULL) < 0)
    {
      LOG_MSG_INFO1("select call failed for receiving inotify events", 0, 0, 0);
      goto remove_watch_fd;
    } else {

      if(read(fd, &event, sizeof(struct inotify_event)) < 0)
      {
        LOG_MSG_INFO1("Error reading the inotify event", 0, 0, 0);
        goto remove_watch_fd;
      } else {

        LOG_MSG_INFO1("Config file modified, will reload config file...", 0, 0, 0);

        //no need to check for LAN version support, just kill everything
        if(lan_server_up)
        {
          if(pthread_cancel(server_thread_v4))
          {
            LOG_MSG_INFO1("pthread_cancel error: %s", strerror(errno), 0, 0);
          }
          if(pthread_cancel(server_thread_v6))
          {
            LOG_MSG_INFO1("pthread_cancel error: %s", strerror(errno), 0, 0);
          }

          if(pthread_join(server_thread_v4, NULL))
          {
            LOG_MSG_INFO1("pthread_join error: %s", strerror(errno), 0, 0);
          }
          if(pthread_join(server_thread_v6, NULL))
          {
            LOG_MSG_INFO1("pthread_join error: %s", strerror(errno), 0, 0);
          }
        }
        lan_server_up = false;
        LOG_MSG_INFO1("Cancelled LAN pthreads", 0, 0, 0);

        //block until critical sections are finished
        while(num_of_critical_threads > 0)
        {
          sleep(TIME_TO_WAIT_FOR_CRITICAL_THREADS);
        }

        LOG_MSG_INFO1("About to erase config...", 0, 0, 0);

        //erase current configuration
        configuration->deleteAllWANServices();
        configuration->deleteLANIface();

        //parse configuration again
        if(QC_SOCKSv5_Cfg_Parser::validateConfigFile((char*)conf_file, configuration))
        {
          configuration->printConfig();
          //spawn LAN pthreads based on new config
          checkLANIface(configuration, &server_thread_v4, &server_thread_v6);
        }
      }
    }

    remove_watch_fd:
      inotify_rm_watch(fd, wd);
      if(close(fd))
      {
        LOG_MSG_INFO1("error with closing socket: %s", strerror(errno), 0, 0);
      }
  }

  if(close(fd))
  {
    LOG_MSG_INFO1("error with closing socket: %s", strerror(errno), 0, 0);
  }
  pthread_exit(0);
}

/*==========================================================================
  FUNCTION monitorLANIface
==========================================================================*/
/*!
@brief
  Continually listens for netlink messages from kernel when lan iface goes
  up/down.

@parameters
  void* arg, expected to be NULL since we don't use the argument

@return
*/
/*========================================================================*/
void* monitorLANIface(void* arg)
{
  //netlink monitor LAN iface up/down
  int netlink_sock;
  struct sockaddr_nl netlink_addr;
  fd_set netlink_sock_read_fd_set;
  unsigned char buffer[MAX_NL_PAYLOAD];
  struct nlmsghdr* nlmhdr;
  struct iovec iov;
  struct msghdr msg;
  int num_bytes_recv;
  struct ifinfomsg* iface_info;

  memset(&netlink_addr, 0, sizeof(struct sockaddr_nl));
  netlink_addr.nl_family = AF_NETLINK;
  netlink_addr.nl_groups = RTMGRP_LINK;

  if((netlink_sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE)) <= -1)
  {
    LOG_MSG_INFO1("error creating netlink socket: %s", strerror(errno), 0, 0);
    pthread_exit(0);
  }
  if(bind(netlink_sock, (struct sockaddr*)&netlink_addr, sizeof(struct sockaddr_nl)) <= -1)
  {
    LOG_MSG_INFO1("error binding netlink socket: %s", strerror(errno), 0, 0);
    if(close(netlink_sock))
    {
      LOG_MSG_INFO1("error with closing socket: %s", strerror(errno), 0, 0);
    }
    pthread_exit(0);
  }

  while(1)
  {
    //clear, yes we do need to do this every loop iteration
    memset(&iov, 0, sizeof(struct iovec));
    memset(&msg, 0, sizeof(struct msghdr));

    nlmhdr = (struct nlmsghdr*)buffer;
    nlmhdr->nlmsg_len = MAX_NL_PAYLOAD;
    //fill out iov and msg
    iov.iov_base = (void*)nlmhdr;
    iov.iov_len  = nlmhdr->nlmsg_len;

    msg.msg_name = (void*)&netlink_addr;
    msg.msg_namelen = sizeof(struct sockaddr_nl);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    //setup file descriptors for select call
    FD_ZERO(&netlink_sock_read_fd_set);
    FD_SET(netlink_sock, &netlink_sock_read_fd_set);

    //block until we recv something over the sock
    if(select(netlink_sock + 1, &netlink_sock_read_fd_set, NULL, NULL, NULL) < 0)
    {
      LOG_MSG_INFO1("select call failed for receiving from netlink socket", 0, 0, 0);
      nlmhdr = NULL;
      continue;
    } else {

      if((num_bytes_recv = recvmsg(netlink_sock, &msg, 0)) <= -1)
      {
        LOG_MSG_INFO1("error receiving from netlink socket", 0, 0, 0);
        nlmhdr = NULL;
        continue;
      }

      while(NLMSG_OK(nlmhdr, num_bytes_recv))
      {
        iface_info = (struct ifinfomsg*)NLMSG_DATA(nlmhdr);

        if(NULL == iface_info)
        {
          LOG_MSG_INFO1("iface info from nlmsg is NULL", 0, 0, 0);
          nlmhdr = NLMSG_NEXT(nlmhdr, num_bytes_recv);
          continue;
        }

        //check if msg is about LAN iface
        if(iface_info->ifi_index == configuration->getLANIfaceIndex())
        {
          switch(nlmhdr->nlmsg_type)
          {
            case(RTM_NEWLINK):
            case(RTM_DELLINK):
            {
              if(!(iface_info->ifi_flags & IFF_UP) || !(iface_info->ifi_flags & IFF_LOWER_UP))
              {
                LOG_MSG_INFO1("LAN iface down: %s : 0x%x",
                              configuration->getLANIface(), iface_info->ifi_flags, 0);

                if(lan_server_up)
                {
                  LOG_MSG_INFO1("Killing LAN server threads...", 0, 0, 0);

                  if(pthread_cancel(server_thread_v4))
                  {
                    LOG_MSG_INFO1("pthread_cancel error: %s", strerror(errno), 0, 0);
                  }
                  if(pthread_cancel(server_thread_v6))
                  {
                    LOG_MSG_INFO1("pthread_cancel error: %s", strerror(errno), 0, 0);
                  }

                  if(pthread_join(server_thread_v4, NULL))
                  {
                    LOG_MSG_INFO1("pthread_join error: %s", strerror(errno), 0, 0);
                  }
                  if(pthread_join(server_thread_v6, NULL))
                  {
                    LOG_MSG_INFO1("pthread_join error: %s", strerror(errno), 0, 0);
                  }

                  QC_SOCKSv5_Garbage_Collector::shutdownAllConnections(&cli_req_sock_pair_map);

                  lan_server_up = false;
                }
              } else {

                LOG_MSG_INFO1("LAN iface up: %s", configuration->getLANIface(), 0, 0);

                if(!lan_server_up)
                {
                  spawnLANServerThreads(configuration, &server_thread_v4, &server_thread_v6);
                }
              }
              break;
            }

            default:
            {
              break;
            }
          }
        }
        nlmhdr = NLMSG_NEXT(nlmhdr, num_bytes_recv);
      }
    }
  }

  if(close(netlink_sock))
  {
    LOG_MSG_INFO1("error with closing socket: %s", strerror(errno), 0, 0);
  }
  pthread_exit(0);
}

/*==========================================================================
  FUNCTION notifyTCPSpliceModule
==========================================================================*/
/*!
@brief
  Notifies TCP splice module of our pid

@parameters
  int nl_sock, netlink socket fd
  struct sockaddr_nl* kernel_addr, kernel nelink addr
  pid_t pid, this process's pid

@return
*/
/*========================================================================*/
void notifyTCPSpliceModule(int nl_sock, struct sockaddr_nl* kernel_addr, pid_t pid)
{
  struct nlmsghdr nlmhdr;
  struct iovec iov;
  struct msghdr msg;

  if(NULL == kernel_addr)
  {
    LOG_MSG_INFO1("given null kernel nl addr\n", 0, 0, 0);
    return;
  }

  memset(&nlmhdr, 0, sizeof(struct nlmsghdr));
  memset(&iov, 0, sizeof(struct iovec));
  memset(&msg, 0, sizeof(struct msghdr));

  //construct the nl packet to send
  nlmhdr.nlmsg_len = NLMSG_LENGTH(0);
  nlmhdr.nlmsg_type = NLMSG_DONE;
  nlmhdr.nlmsg_pid = pid;

  //fill out iov and msg
  iov.iov_base = (void *)&nlmhdr;
  iov.iov_len  = nlmhdr.nlmsg_len;

  msg.msg_name = (void*)kernel_addr;
  msg.msg_namelen = sizeof(struct sockaddr_nl);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  //send the netlink msg
  if(sendmsg(nl_sock, &msg, 0) < 0)
  {
    LOG_MSG_INFO1("Failed to send pid to tcp splice module: %s", strerror(errno), 0, 0);
  } else {
    LOG_MSG_INFO1("pid sent to tcp splice module: %u\n", pid, 0, 0);
  }

  return;
}
