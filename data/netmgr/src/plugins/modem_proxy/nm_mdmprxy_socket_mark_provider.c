/******************************************************************************

     N M _ M D M P R X Y _ S O C K E T _ M A R K _ P R O V I D E R . C

******************************************************************************/

/******************************************************************************

  @file    nm_mdmprxy_socket_mark_provider.c
  @brief   Modem proxy plugin

  DESCRIPTION
  Modem proxy plugin module is responsible for managing routing rules
  and packet marking rules for forwarding packets to modem.

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include <pthread.h>
#include <errno.h>

/* QMI DFS Includes */
#include "qmi_client.h"
#include "qmi_cci_target.h"
#include "qmi_cci_common.h"
#include "qmi_client.h"
#include "qmi.h"
#include "data_filter_service_v01.h"

/* Socket includes */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/udp.h>
#ifndef NETMGR_OFFTARGET
#include <netinet/in6.h>
#endif

/* Standard Linux includes */
#include <unistd.h>
#include <error.h>
#include <time.h>

/* Netmgr core includes */
#include "ds_util.h"
#include "netmgr.h"
#include "netmgr_util.h"
#include "netmgr_config.h"

#include "nm_mdmprxy.h"
#include "nm_mdmprxy_mark_provider_factory.h"
#include "nm_mdmprxy_iptables_helper.h"


/*===========================================================================
                     LOCAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

#define MAX_OPEN_SOCKETS 600
#define NM_MDMPRXY_MAX_BIND_RETRIES 10

#define NM_MDMPRXY_QMI_TIMEOUT  (10000)

typedef struct
{
  boolean is_v4_marking_rule_installed;
  boolean is_v6_marking_rule_installed;
} nm_mdmprxy_cfg_t;

static nm_mdmprxy_cfg_t global_config;

#define NM_MDMPRXY_GET_MARKING_RULES_INSTALLED(family)                 \
  ((AF_INET == family) ?                                               \
   global_config.is_v4_marking_rule_installed :                        \
   global_config.is_v6_marking_rule_installed)

#define NM_MDMPRXY_SET_MARKING_RULES_INSTALLED(family, status)         \
  *(((AF_INET == family) ?                                             \
    &global_config.is_v4_marking_rule_installed :                      \
    &global_config.is_v6_marking_rule_installed)) = status

#define MAX_SOCKET_LOGS (5000)
static int logging_counter;

#define NM_MDMPRXY_SOCKET_LOG(level,...)                                     \
  do {                                                                       \
    if (logging_fd)                                                          \
    {                                                                        \
      char timstamp[MAX_TIMESTAMP_LEN];                                      \
                                                                             \
      /* We need to prevent the log file from growing beyond a */            \
      /* certain point. Short term solution would be to empty the */         \
      /* file and restart logging. */                                        \
                                                                             \
      /* TODO: Proper solution would be to implement a circular log */       \
      /* buffer*/                                                            \
      if (MAX_SOCKET_LOGS == logging_counter)                                \
      {                                                                      \
        if(fclose(logging_fd))                                               \
        {                                                                    \
          /* Failed to close the file and we have exceeded the */            \
          /* the limit */                                                    \
          perror("Failed to close log file");                                \
          logging_fd = NULL;                                                 \
          break;                                                             \
        }                                                                    \
                                                                             \
        /* Re-open log file with w+ mode so that old contents */             \
        /* are cleared */                                                    \
        logging_fd = fopen(NETMGR_LOG_FILE_LOCATION, "w+");                  \
        if (!logging_fd)                                                     \
        {                                                                    \
          log_to_file = FALSE;                                               \
          perror("failed to re-open log file");                              \
          break;                                                             \
        }                                                                    \
                                                                             \
        fprintf(logging_fd, "Reached maximum logging limit....restarting");  \
        fprintf(logging_fd,"\n");                                            \
        /* Reset logging counter */                                          \
        logging_counter = 0;                                                 \
      }                                                                      \
                                                                             \
      TIMESTAMP(timstamp);                                                   \
      fprintf(logging_fd,"%25s %70s %40s():%3u (INF):",                      \
              timstamp, __FILE__, __func__, __LINE__);                       \
      fprintf(logging_fd,__VA_ARGS__);                                       \
      fprintf(logging_fd,"\n");                                              \
      fflush(logging_fd);                                                    \
      logging_counter++;                                                     \
    }                                                                        \
    ds_log_##level(__VA_ARGS__);                                             \
} while (0);

/* QMI Handle information */
int                    pr_qmi_state;
qmi_client_type        pr_qmi_dfs_clnt_hndl;
qmi_cci_os_signal_type pr_qcci_os_params;
#define PR_QMI_STATE_UNINITIALIZED 0
#define PR_QMI_STATE_INITIALIZED   1

/* List of open handles. Caching these for SSR / cleanup only */
static int             pr_fds[MAX_OPEN_SOCKETS];
static int             pr_encap_handle;

/* Synchronize the v-table callbacks with QMI callbacks */
static pthread_mutex_t nm_mdmprxy_mark_lock = PTHREAD_MUTEX_INITIALIZER;

static const struct nm_mdmprxy_cmd static_rule_list[] =
{
  {"%s " NM_MDMPRXY_FLAGS " -t mangle -N nm_mdmprxy_pkt_skmark",
   "%s " NM_MDMPRXY_FLAGS " -t mangle -X nm_mdmprxy_pkt_skmark",
   nm_mdmprxy_basic_formatter},

  {"%s " NM_MDMPRXY_FLAGS " -t mangle -A nm_mdmprxy_pkt_skmark -j RETURN",
   "%s " NM_MDMPRXY_FLAGS " -t mangle -D nm_mdmprxy_pkt_skmark -j RETURN",
   nm_mdmprxy_basic_formatter},

  {"%s " NM_MDMPRXY_FLAGS " -t mangle -A nm_mdmprxy_mark_prov_chain -p tcp"
                          " --dport " strstr(NM_MDMPRXY_IMS_PORT_START) ":" strstr(NM_MDMPRXY_IMS_PORT_END)
                          " -j MARK --set-mark " strstr(NM_MDMPRXY_MARK),
   "%s " NM_MDMPRXY_FLAGS " -t mangle -D nm_mdmprxy_mark_prov_chain -p tcp"
                          " --dport " strstr(NM_MDMPRXY_IMS_PORT_START) ":" strstr(NM_MDMPRXY_IMS_PORT_END)
                          " -j MARK --set-mark " strstr(NM_MDMPRXY_MARK),
   nm_mdmprxy_basic_formatter },

  {"%s " NM_MDMPRXY_FLAGS " -t mangle -A nm_mdmprxy_mark_prov_chain -p udp"
                          " --dport " strstr(NM_MDMPRXY_IMS_PORT_START) ":" strstr(NM_MDMPRXY_IMS_PORT_END)
                          " -j MARK --set-mark " strstr(NM_MDMPRXY_MARK),
   "%s " NM_MDMPRXY_FLAGS " -t mangle -D nm_mdmprxy_mark_prov_chain -p udp"
                          " --dport " strstr(NM_MDMPRXY_IMS_PORT_START) ":" strstr(NM_MDMPRXY_IMS_PORT_END)
                          " -j MARK --set-mark " strstr(NM_MDMPRXY_MARK),
   nm_mdmprxy_basic_formatter },


  {"%s " NM_MDMPRXY_FLAGS " -t mangle -A nm_mdmprxy_mark_prov_chain -p tcp"
                          " --dport " strstr(NM_MDMPRXY_EX_IMS_PORT_START) ":" strstr(NM_MDMPRXY_EX_IMS_PORT_END)
                          " -j MARK --set-mark " strstr(NM_MDMPRXY_MARK),
   "%s " NM_MDMPRXY_FLAGS " -t mangle -D nm_mdmprxy_mark_prov_chain -p tcp"
                          " --dport " strstr(NM_MDMPRXY_EX_IMS_PORT_START) ":" strstr(NM_MDMPRXY_EX_IMS_PORT_END)
                          " -j MARK --set-mark " strstr(NM_MDMPRXY_MARK),
   nm_mdmprxy_basic_formatter },


  {"%s " NM_MDMPRXY_FLAGS " -t mangle -A nm_mdmprxy_mark_prov_chain -p udp -m socket --transparent --nowildcard --restore-skmark -j nm_mdmprxy_pkt_skmark",
   "%s " NM_MDMPRXY_FLAGS " -t mangle -D nm_mdmprxy_mark_prov_chain -p udp -m socket --transparent --nowildcard --restore-skmark -j nm_mdmprxy_pkt_skmark",
   nm_mdmprxy_basic_formatter},

  {"%s " NM_MDMPRXY_FLAGS " -t mangle -A nm_mdmprxy_mark_prov_chain -p tcp -m socket --transparent --nowildcard --restore-skmark -j nm_mdmprxy_pkt_skmark",
   "%s " NM_MDMPRXY_FLAGS " -t mangle -D nm_mdmprxy_mark_prov_chain -p tcp -m socket --transparent --nowildcard --restore-skmark -j nm_mdmprxy_pkt_skmark",
   nm_mdmprxy_basic_formatter},

  /********************************************************************************/
  /* TODO: Common junk which needs to be de-duplicated from legacy provider       */
  /********************************************************************************/

  /* Insert a jump rule to IMS audio port-forwarding based rules */
  /* TODO: Figure out how to handle dynamic addition and deletion of audio
           port-forwarding based rules */
  /* save any packet mark on to the connection (for handling fragmented IPv6 pkts) */
  { "%s" NM_MDMPRXY_FLAGS " -t mangle -I nm_mdmprxy_pkt_marker -j CONNMARK --save-mark",
    "%s" NM_MDMPRXY_FLAGS " -t mangle -D nm_mdmprxy_pkt_marker -j CONNMARK --save-mark",
    nm_mdmprxy_connmark_save_formatter },

  { "%s" NM_MDMPRXY_FLAGS " -t mangle -I nm_mdmprxy_pkt_marker -j nm_mdmprxy_mngl_pre_ex",
    "%s" NM_MDMPRXY_FLAGS " -t mangle -D nm_mdmprxy_pkt_marker -j nm_mdmprxy_mngl_pre_ex",
    nm_mdmprxy_basic_formatter },

  /* Mark IMS SIP packets arriving on port 5060 */
  { "%s" NM_MDMPRXY_FLAGS " -t mangle -I nm_mdmprxy_pkt_marker -p udp"
                          " --dport " strstr(NM_MDMPRXY_IMS_SIP_PORT) " -j MARK --set-mark " strstr(NM_MDMPRXY_MARK),
    "%s" NM_MDMPRXY_FLAGS " -t mangle -D nm_mdmprxy_pkt_marker -p udp"
                          " --dport " strstr(NM_MDMPRXY_IMS_SIP_PORT) " -j MARK --set-mark " strstr(NM_MDMPRXY_MARK),
    nm_mdmprxy_basic_formatter },

  { "%s" NM_MDMPRXY_FLAGS " -t mangle -I nm_mdmprxy_pkt_marker -p tcp"
                          " --dport " strstr(NM_MDMPRXY_IMS_SIP_PORT) " -j MARK --set-mark " strstr(NM_MDMPRXY_MARK),
    "%s" NM_MDMPRXY_FLAGS " -t mangle -D nm_mdmprxy_pkt_marker -p tcp"
                          " --dport " strstr(NM_MDMPRXY_IMS_SIP_PORT) " -j MARK --set-mark " strstr(NM_MDMPRXY_MARK),
    nm_mdmprxy_basic_formatter },

  /* Insert a jump rule to the SPI matching table from pkt_marker table */
  { "%s" NM_MDMPRXY_FLAGS " -t mangle -I nm_mdmprxy_pkt_marker -j nm_mdmprxy_mngl_pre_spi",
    "%s" NM_MDMPRXY_FLAGS " -t mangle -D nm_mdmprxy_pkt_marker -j nm_mdmprxy_mngl_pre_spi",
    nm_mdmprxy_basic_formatter },

  /* Jump to mark prov chain which hosts the modem port-range rules */
  { "%s" NM_MDMPRXY_FLAGS " -t mangle -I nm_mdmprxy_pkt_marker -j nm_mdmprxy_mark_prov_chain",
    "%s" NM_MDMPRXY_FLAGS " -t mangle -D nm_mdmprxy_pkt_marker -j nm_mdmprxy_mark_prov_chain",
    nm_mdmprxy_basic_formatter },

  /* Restore mark on icmp packets originating from modem using connection tracking
     framework to enable pings originiating from modem to be successful */
  { "%s" NM_MDMPRXY_FLAGS " -t mangle -I nm_mdmprxy_pkt_marker -j CONNMARK --restore-mark",
    "%s" NM_MDMPRXY_FLAGS " -t mangle -D nm_mdmprxy_pkt_marker -j CONNMARK --restore-mark",
    nm_mdmprxy_basic_formatter }
};

/* All IKE in UDP packets destined to the Modem NATT port must be sent to the
 * Modem and other IKE/ESP in UDP to the AP network stack for decryption and
 * subsequent forwarding based on innner packet contents. This is how we can
 * differentiate between the two: the first 4 bytes of the UDP payload of
 * IKE in UDP is always 0 and non-zero for ESP in UDP packets (contains SPI)
 *
 * Start at the IP header, extract IHL and add the offset to get to the
 * start of UDP header and then offset 8 (UDP header) bytes to get to the
 * payload. If the 4 bytes are 0 then mark the packet so that it gets
 * forwarded to the Modem */

static const struct nm_mdmprxy_cmd udp_encap_rule_list[] =
{
  {"%s " NM_MDMPRXY_FLAGS " -t mangle -I nm_mdmprxy_mark_prov_chain -p udp --sport 500 --dport %d -j MARK --set-mark %u",
   "%s " NM_MDMPRXY_FLAGS " -t mangle -D nm_mdmprxy_mark_prov_chain -p udp --sport 500 --dport %d -j MARK --set-mark %u",
   nm_mdmprxy_encaprule_dport_formatter},

  {"%s " NM_MDMPRXY_FLAGS " -t mangle -I nm_mdmprxy_mark_prov_chain -p udp --dport %d -m u32 --u32 0>>22&0x3C@8=0 -j MARK --set-mark %u",
   "%s " NM_MDMPRXY_FLAGS " -t mangle -D nm_mdmprxy_mark_prov_chain -p udp --dport %d -m u32 --u32 0>>22&0x3C@8=0 -j MARK --set-mark %u",
   nm_mdmprxy_encaprule_dport_formatter}
};

extern int
nm_mdmprxy_common_mark_prov_install_spi_rule(sa_family_t,unsigned int);

extern int
nm_mdmprxy_common_mark_prov_remove_spi_rule(sa_family_t,unsigned int);

extern int
nm_mdmprxy_common_mark_prov_install_portfwd_rule(sa_family_t, boolean);

extern int
nm_mdmprxy_common_mark_prov_remove_portfwd_rule(sa_family_t, boolean);

extern boolean
nm_mdmprxy_common_mark_prov_query_portfwd_status(sa_family_t);

extern int
nm_mdmprxy_common_mark_prov_install_icmp_rules(sa_family_t, char*);

extern int
nm_mdmprxy_common_mark_prov_remove_icmp_rules(sa_family_t, char*);

extern int
nm_mdmprxy_common_mark_prov_oos_cleanup(void);

static int
nm_mdmprxy_socket_mark_prov_init(void);

static int
nm_mdmprxy_socket_mark_prov_is_hdlr(void);

static int
nm_mdmprxy_socket_mark_prov_oos_hdlr(void);

static void
nm_mdmprxy_socket_mark_prov_status(void);

extern void
nm_mdmprxy_common_mark_prov_setup_iptables(struct mark_prov_iptables *);

nm_mdmprxy_mark_prov_t nm_mdmprxy_socket_mark_provider =
{
  .install_rules        = 0,
  .remove_rules         = 0,

  .install_spi_rule     = nm_mdmprxy_common_mark_prov_install_spi_rule,
  .remove_spi_rule      = nm_mdmprxy_common_mark_prov_remove_spi_rule,
  .install_portfwd_rule = nm_mdmprxy_common_mark_prov_install_portfwd_rule,
  .remove_portfwd_rule  = nm_mdmprxy_common_mark_prov_remove_portfwd_rule,
  .query_portfwd_status = nm_mdmprxy_common_mark_prov_query_portfwd_status,
  .install_icmp_rule    = nm_mdmprxy_common_mark_prov_install_icmp_rules,
  .remove_icmp_rule     = nm_mdmprxy_common_mark_prov_remove_icmp_rules,

  .init                 = nm_mdmprxy_socket_mark_prov_init,
  .is_hdlr              = nm_mdmprxy_socket_mark_prov_is_hdlr,
  .oos_hdlr             = nm_mdmprxy_socket_mark_prov_oos_hdlr,
  .sigusr1_hdlr         = nm_mdmprxy_socket_mark_prov_status,
};

#define nm_mdmprxy_socket_mark_lock() \
  pthread_mutex_lock(&nm_mdmprxy_mark_lock); \
  netmgr_log_low("%s(): locked nm_mdmprxy_mark_lock", __func__);

#define nm_mdmprxy_socket_mark_unlock() \
  pthread_mutex_unlock(&nm_mdmprxy_mark_lock); \
  netmgr_log_low("%s(): unlocked nm_mdmprxy_mark_lock", __func__);

static struct mark_prov_iptables socket_mark_prov_cmd_tbl;

/*===========================================================================
                     Socket fanciness
===========================================================================*/
#define for_each_fd(X) \
  for (X = 0; X < MAX_OPEN_SOCKETS; X++) \
    if (pr_fds[i] != 0)

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_set_reserved_ports
===========================================================================*/
/*!
@brief
  Sets the 'reserved_port_bind' proc entry to block explicit bind
  to a specific range of ports
*/
/*=========================================================================*/
static void
nm_mdmprxy_socket_mark_set_reserved_ports(void)
{
  FILE *fp = NULL;
  const char* enable_blocking_bind = "0";
  const char* port_range = strstr(NM_MDMPRXY_IMS_PORT_START) "-" strstr(NM_MDMPRXY_IMS_PORT_END) ","
                          strstr(NM_MDMPRXY_EX_IMS_PORT_START) "-" strstr(NM_MDMPRXY_EX_IMS_PORT_END);

  /* Check if we can access the reserved_port_bind */
  if (access(FILEPATH_RESERVED_PORT_BIND, F_OK))
  {
    /* We are unable to access the file */
    netmgr_log_err("%s(): unable access reserved_port_bind /proc entry [%s]",
                   __func__, strerror(errno));
    return;
  }

  fp = fopen(FILEPATH_RESERVED_PORT_BIND, "w");
  if (!fp)
  {
    netmgr_log_err("%s(): unable to open reserved_port_bind /proc file [%s]",
                   __func__, strerror(errno));
    return;
  }

  fprintf(fp, "%s", enable_blocking_bind);
  fclose(fp);
  fp = NULL;

  /* Check if we can access the ip_local_reserved_ports */
  if (access(FILEPATH_IP_LOCAL_RESERVED_PORTS, F_OK))
  {
    netmgr_log_err("%s(): unable to access reserved_port_bind /proc entry [%s]",
                   __func__, strerror(errno));
    return;
  }

  fp = fopen(FILEPATH_IP_LOCAL_RESERVED_PORTS, "w");
  if (!fp)
  {
    netmgr_log_err("%s(): unable to open ip_local_reserved_ports /proc file [%s]",
                   __func__, strerror(errno));
    return;
  }

  fprintf(fp, "%s", port_range);
  fclose(fp);
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_get_socket_port
===========================================================================*/
/*!
@brief
  Query the kernel to get the port number of a socket.

@arg handle - pr_fds handle, not fd

@return
  0 on failure
  port number otherwise

*/
/*=========================================================================*/
static int
nm_mdmprxy_socket_mark_get_socket_port
(
  unsigned int handle
)
{
  struct sockaddr_storage addr;
  socklen_t len = sizeof(addr);
  int rc;

  rc = getsockname(pr_fds[handle], (struct sockaddr *)&addr, &len);
  if (rc < 0)
  {
    netmgr_log_err("%s(): Unable to get socket name for handle %u",
                   __func__, handle);
    return 0;
  }

  switch(addr.ss_family)
  {
    case AF_INET:
      return ntohs(((struct sockaddr_in *)&addr)->sin_port);

    case AF_INET6:
      return ntohs(((struct sockaddr_in6 *)&addr)->sin6_port);

    default:
      netmgr_log_err("%s(): Unkown AF: %d", __func__, addr.ss_family);
      return 0;
  }
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_get_socket_af
===========================================================================*/
/*!
@brief
  Query the kernel to get the address family of a socket.

@arg handle - pr_fds handle, not fd

@return
  0 on failure
  address family otherwise (AF_INET/AF_INET6)

*/
/*=========================================================================*/
static int
nm_mdmprxy_socket_mark_get_socket_af
(
  unsigned int handle
)
{
  struct sockaddr_storage addr;
  socklen_t len = sizeof(addr);
  int rc;

  rc = getsockname(pr_fds[handle], (struct sockaddr *)&addr, &len);
  if (rc < 0)
  {
    netmgr_log_err("%s(): Unable to get socket name for handle %u",
                   __func__, handle);
    return 0;
  }

  return addr.ss_family;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_get_socket_type
===========================================================================*/
/*!
@brief
  Query the kernel to get the type of a socket.

@arg handle - pr_fds handle, not fd

@return
  0 on failure
  socket type otherwise

*/
/*=========================================================================*/
static int
nm_mdmprxy_socket_mark_get_socket_type
(
  unsigned int handle
)
{
  int type, rc;
  socklen_t optlen = sizeof(type);

  rc = getsockopt(pr_fds[handle], SOL_SOCKET, SO_PROTOCOL, &type, &optlen);
  if (rc < 0)
  {
    netmgr_log_err("%s(): Unable to getsockopt for handle %u",
                   __func__, handle);
    return 0;
  }
  /* TODO: Check optlen */
  return type;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_print_ports
===========================================================================*/
/*!
@brief
  Print all ports opened by netmgr port reserver to the logs

*/
/*=========================================================================*/
static void
nm_mdmprxy_socket_mark_prov_status()
{
  int i;
  nm_mdmprxy_socket_mark_lock();
  for_each_fd(i)
  {
    netmgr_log_low("sockets[%d]: handle:%d af:%d type:%d port:%d",
                   i,
                   pr_fds[i],
                   nm_mdmprxy_socket_mark_get_socket_af(i),
                   nm_mdmprxy_socket_mark_get_socket_type(i),
                   nm_mdmprxy_socket_mark_get_socket_port(i));
  }
  nm_mdmprxy_socket_mark_unlock();
}

/*===========================================================================
  FUNCTION nm_mdmprxy_socket_mark_close_socket
===========================================================================*/
/*!
@brief
  Check if the handle is a UDP ENCAP. If it is, do some cleanup.
  Then close the socket.

@arg handle - pr_fds handle to close

*/
/*=========================================================================*/
static void
nm_mdmprxy_socket_mark_close_socket
(
  unsigned int handle
)
{
  int port, rc;

  if (handle >= MAX_OPEN_SOCKETS)
  {
    netmgr_log_err("%s(): Error: handle out of range (%u)", __func__, handle);
    return;
  }

  if (pr_fds[handle] == 0)
  {
    /* Nothing to do */
    netmgr_log_med("%s(): Could not find reference to handle [%u]", __func__, handle);
    return;
  }

  port = nm_mdmprxy_socket_mark_get_socket_port(handle);

  if (pr_encap_handle >= 0 && handle == pr_encap_handle)
  {
    pr_encap_handle = -1;
    netmgr_main_cfg_set_ike_port(0);
    rc = nm_mdmprxy_uninstall_udpencap_rules(port,
                                             NM_MDMPRXY_IKE_REMOTE_PORT,
                                             NM_MDMPRXY_MARK);
    if (rc != NETMGR_SUCCESS) {
      netmgr_log_err("%s(): Failed to uninstall UDP Encap rules", __func__);
    }

    /* Log the message to file */
    NM_MDMPRXY_SOCKET_LOG(err, "Destroy UDP encap socket handle: %u port:%u",
                          handle, port);
  }
  else
  {
    NM_MDMPRXY_SOCKET_LOG(err, "Close socket handle: %u port:%u", handle, port);
  }

  close(pr_fds[handle]);
  pr_fds[handle] = 0;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_close_all_ports
===========================================================================*/
/*!
@brief
  Close all all ports opened by netmgr port reserver

*/
/*=========================================================================*/
static void
nm_mdmprxy_socket_mark_close_all_ports()
{
  int i;

  for_each_fd(i)
  {
    nm_mdmprxy_socket_mark_close_socket(i);
  }
  netmgr_log_low("%s(): Closed all ports", __func__);
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_find_empty_port_index
===========================================================================*/
/*!
@brief
  Simple linear search over pr_fds[] to find an unused index
  It is the caller's responsibility to ensure that the return value
  is within range.

@return
  a positive value from 0 to MAX_OPEN_SOCKETS

*/
/*=========================================================================*/
static unsigned int
nm_mdmprxy_socket_mark_find_empty_port_index()
{
  unsigned int i;

  for (i = 0; i < MAX_OPEN_SOCKETS; i++)
  {
    if (pr_fds[i] == 0)
      return i;
  }

  return i;
}

/*===========================================================================
  FUNCTION  _nm_mdmprxy_socket_mark_socket_bind
===========================================================================*/
/*!
@brief
  Finds an empty slot in pr_fds[], allocates a socket,
  sets it to transparent mode, and binds

@arg type - SOCK_DGRAM/SOCK_STREAM
@arg source - AF + Port to bind to. Can be sockaddr_in or sockaddr_in6
@arg slen - size of source struct

@return
  <0 on failure
  pr_fds index otherwise

*/
/*=========================================================================*/
static int
_nm_mdmprxy_socket_mark_socket_bind
(
  int type,
  struct sockaddr_storage *source,
  ssize_t slen,
  uint32_t mark
)
{
  unsigned int idx;
  int fd, rc, val;

  idx = nm_mdmprxy_socket_mark_find_empty_port_index();

  if (idx >= MAX_OPEN_SOCKETS)
  {
    netmgr_log_err("%s(): Too many sockets, close some before requesting more",
                   __func__);
    return NETMGR_FAILURE;
  }

  if (source->ss_family == AF_UNSPEC)
    fd = socket(AF_INET6, type, 0);
  else
    fd = socket(source->ss_family, type, 0);

  if (fd < 0)
  {
    netmgr_log_err("%s(): Unable to open socket: %d-%s\n",
                   __func__, errno, strerror(errno));
    return fd;
  }

  val = 1;
  if (source->ss_family == AF_INET6)
  {
    rc = setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &val, sizeof(val));
    if (rc < 0)
    {
      close(fd);
      netmgr_log_err("Unable to set IPV6_V6ONLY: %d-%s\n",
                     errno, strerror(errno));
      return rc;
    }
  }

  val = 1;
  rc = setsockopt(fd, SOL_IP, IP_TRANSPARENT, &val, sizeof(val));
  if (rc < 0)
  {
    close(fd);
    netmgr_log_err("Unable to set IP_TRANSPARENT: %d-%s\n",
                   errno, strerror(errno));
    return rc;
  }

  val = mark;
  rc = setsockopt(fd, SOL_SOCKET, SO_MARK, &val, sizeof(val));
  if (rc < 0)
  {
    close(fd);
    netmgr_log_err("Unable to set SO_MARK: %d-%s\n", errno, strerror(errno));
    return rc;
  }

  rc = bind(fd, source, slen);
  if (rc < 0)
  {
    close(fd);
    netmgr_log_err("Unable to bind socket: %d-%s\n",
                   errno, strerror(errno));
    return rc;
  }

  if (type == SOCK_STREAM)
  {
    rc = listen(fd, SOMAXCONN);
    if (rc < 0)
    {
      close(fd);
      netmgr_log_err("Unable to listen on socket: %d-%s\n",
                     errno, strerror(errno));
      return rc;
    }
  }

  pr_fds[idx] = fd;

  return (int)idx;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_socket_bind
===========================================================================*/
/*!
@brief
  Convenience wrapper around _nm_mdmprxy_socket_mark_socket_bind. Formats a sockaddr_storage
  based on the arguments then proceeds to call _nm_mdmprxy_socket_mark_socket_bind. If the
  specified port is 0, an random number is chosen between NM_MDMPRXY_MODEM_PORT_START
  and NM_MDMPRXY_PORT_RANGE_MAX. If bind fails, a new port is chosen in the same range
  and bind is retried. Will attempt NM_MDMPRXY_MAX_BIND_RETRIES times.

@arg af - AF_INET/AF_INET6
@arg type - SOCK_DGRAM/SOCK_STREAM
@arg port - a number between 0 and 65535

@return
  <0 on failure
  pr_fds index otherwise

*/
/*=========================================================================*/
static int
nm_mdmprxy_socket_mark_socket_bind
(
  int af,
 int type,
 int port,
 uint32_t mark
)
{
  int idx = -1;
  int retries = 0;
  int r_port;

  do
  {
    if (port == 0)
      r_port = NM_MDMPRXY_MODEM_PORT_RANGE_START + (rand() % (NM_MDMPRXY_PORT_RANGE_MAX - NM_MDMPRXY_MODEM_PORT_RANGE_START));
    else
      r_port = port;

    if (af == AF_INET)
    {
      struct sockaddr_in source;
      memset(&source, 0, sizeof(source));
      source.sin_family = af;
      source.sin_port = htons(r_port & 0xFFFF);
      idx = _nm_mdmprxy_socket_mark_socket_bind(type,
                                   (struct sockaddr_storage *)&source,
                                   sizeof(source), mark);
    }
    else if (af == AF_INET6)
    {
      struct sockaddr_in6 source;
      memset(&source, 0, sizeof(source));
      source.sin6_family = af;
      source.sin6_port = htons(r_port & 0xFFFF);
      idx = _nm_mdmprxy_socket_mark_socket_bind(type,
                                   (struct sockaddr_storage *)&source,
                                   sizeof(source), mark);
    }
    else
    {
      netmgr_log_err("%s(): unknown AF: %d", __func__, af);
      return -1;
    }

    if (idx >= 0)
    {
      NM_MDMPRXY_SOCKET_LOG(low, "Successfully allocated socket %d, af:%d, type:%d port:%d",
                            idx, af, type, r_port);
    }
    else if (port == 0)
    {
      NM_MDMPRXY_SOCKET_LOG(med, "Failed to bind ephemeral af:%d type:%d port: %d try:%d",
                            af, type, r_port, retries);
    }
    else
    {
      NM_MDMPRXY_SOCKET_LOG(med, "Failed to bind af:%d type:%d port: %d",
                            af, type, r_port);
    }

    retries ++;
  } while (idx < 0 && retries < NM_MDMPRXY_MAX_BIND_RETRIES && port == 0);

  return idx;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_set_udpencap
===========================================================================*/
/*!
@brief
  Set/unset a socket in UDP encapsulation mode. Uses encapsulation mode
  UDP_ENCAP_ESPINUDP. When encapsulation mode is enabled, IP_TRANSPARENT
  mode is disabled. Likewise when encapsulation mode is disabled,
  IP_TRANSPARENT mode is re-applied.

@arg handle - pr_fds[] handle
@arg should_set - TRUE or 1 means set encap mode; FALSE of 0 disabled it


@return
  NETMGR_FAILURE / NETMGR_SUCCESS

*/
/*=========================================================================*/
static int
nm_mdmprxy_socket_mark_set_udpencap
(
  unsigned int handle,
  int should_set
)
{
  int rc, val;
  int type = UDP_ENCAP_ESPINUDP;

  if (handle >= MAX_OPEN_SOCKETS)
  {
    netmgr_log_err("%s(): Error: handle out of range (%u)", __func__, handle);
    return NETMGR_FAILURE;
  }

  if (pr_fds[handle] == 0)
  {
    netmgr_log_err("%s(): Tried to change modes on an unused handle (%u)",
                   __func__, handle);
    return NETMGR_FAILURE;
  }

  /* unset transparent */
  if (should_set)
    val = 0;
  else
    val = 1;

  rc = setsockopt(pr_fds[handle], SOL_IP, IP_TRANSPARENT, &val, sizeof(val));
  if (rc < 0)
  {
    netmgr_log_err("%s(): Unable to set IP_TRANSPARENT: %d-%s\n",
                   __func__, errno, strerror(errno));
    return NETMGR_FAILURE;
  }

  /* Set UDP Encapsulation
   * As of Kernel 3.14, there is no way to un-set UDP_ENCAP once it has
   * been set. As such, if UDP_ENCAP unset is requested, we just ignore
   * the next step. In theory, the TRANSPARENT flag above should take
   * effect earlier in the stack than UDP_ENCAP. As such the packets should
   * still be proxied. This behavior is untested as it is not an anticipated
   * use case */
  if (should_set)
  {
    rc = setsockopt(pr_fds[handle], SOL_UDP, UDP_ENCAP, &type, sizeof(type));
    if (rc < 0)
    {
      netmgr_log_err("%s(): Unable to set UDP_ENCAP: %d-%s\n",
                     __func__, errno, strerror(errno));
      return NETMGR_FAILURE;
    }
  }

  NM_MDMPRXY_SOCKET_LOG(low, "Set{%d} UDP_ENCAP on handle:%d port:%d",
                        should_set, handle,
                        nm_mdmprxy_socket_mark_get_socket_port(handle));

  return NETMGR_SUCCESS;
}
/*===========================================================================
                     QMI DFS Client
===========================================================================*/


void nm_mdmprxy_socket_mark_qmi_do_request_resp
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                           *resp_c_struct,
  unsigned int                   resp_c_struct_len,
  void                           *resp_cb_data,
  qmi_client_error_type          transp_err
)
{
  NM_MDMPRXY_VAR_UNUSED(user_handle);
  NM_MDMPRXY_VAR_UNUSED(msg_id);
  NM_MDMPRXY_VAR_UNUSED(resp_c_struct);
  NM_MDMPRXY_VAR_UNUSED(resp_c_struct_len);
  NM_MDMPRXY_VAR_UNUSED(resp_cb_data);
  NM_MDMPRXY_VAR_UNUSED(transp_err);

  netmgr_log_low("%s(): Pointless function that does nothing", __func__);
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_qmi_do_request_ind
===========================================================================*/
/*!
@brief
  Callback to handle QMI_DFS_REMOTE_SOCKET_REQUEST_IND_V01 indication.
  Converts modem request into a socket bind request. Then sends a
  QMI_DFS_REMOTE_SOCKET_ALLOCATED_REQ_V01 to the modem with the appropriate
  handles.

*/
/*=========================================================================*/
static void
nm_mdmprxy_socket_mark_qmi_do_request_ind
(
  qmi_client_type    user_handle,
  unsigned int       msg_id,
  void               *ind_buf,
  unsigned int       ind_buf_len,
  void               *ind_cb_data
)
{
  qmi_client_error_type qmi_err;
  dfs_remote_socket_request_ind_msg_v01 req_msg;
  dfs_remote_socket_allocated_req_msg_v01  resp_req_msg;
  dfs_remote_socket_allocated_resp_msg_v01 resp_resp_msg;
  qmi_txn_handle txn_handle;
  int i, af, type, port, handle, rc;

  NM_MDMPRXY_VAR_UNUSED(ind_cb_data);

  qmi_err = qmi_client_message_decode(user_handle,
                                      QMI_IDL_INDICATION,
                                      msg_id,
                                      ind_buf,
                                      ind_buf_len,
                                      &req_msg,
                                      sizeof(req_msg));

  if(QMI_NO_ERR != qmi_err)
  {
    netmgr_log_err("%s(): failed qmi_client_message_decode with error %d", __func__, qmi_err);
    return;
  }

  resp_req_msg.socket_list_len = req_msg.request_socket_list_len;
  for (i = 0; i < req_msg.request_socket_list_len && i < QMI_DFS_MAX_ALLOCATED_SOCKETS_V01; i++)
  {
    memset(&(resp_req_msg.socket_list[i]),
           0,
           sizeof(dfs_allocated_socket_info_type_v01));

    memcpy(&(resp_req_msg.socket_list[i].socket_info),
           &(req_msg.request_socket_list[i]),
           sizeof(dfs_request_socket_info_type_v01));

    port = req_msg.request_socket_list[i].port_no;
    if (port == 0)
      resp_req_msg.socket_list[i].is_ephemeral = 1;

    switch(req_msg.request_socket_list[i].xport_prot)
    {
      case DFS_PROTO_TCP_V01:
        type = SOCK_STREAM;
        break;

      case DFS_PROTO_UDP_V01:
        type = SOCK_DGRAM;
        break;

      default:
        netmgr_log_err("%s(): Modem requested a socket of unknown type: %d",
                       __func__, req_msg.request_socket_list[i].xport_prot);
        resp_req_msg.socket_list[i].status = DFS_REMOTE_SOCKET_ALLOC_GENERAL_FAILURE_V01;
        continue;
    }

    switch(req_msg.request_socket_list[i].ip_family)
    {
      case DFS_IP_FAMILY_IPV4_V01:
        af = AF_INET;
        break;

      case DFS_IP_FAMILY_IPV6_V01:
        af = AF_INET6;
        break;

      default:
        netmgr_log_err("%s(): Modem requested a socket of unknown IP family: %d",
                       __func__, req_msg.request_socket_list[i].ip_family);
        resp_req_msg.socket_list[i].status = DFS_REMOTE_SOCKET_ALLOC_GENERAL_FAILURE_V01;
        continue;
    }

    handle = nm_mdmprxy_socket_mark_socket_bind(af, type, port, NM_MDMPRXY_MARK);
    if (handle < 0)
    {
      /* TODO: Parse errno to see if this is an IN_USE error and pass the correct
       * error code back to the modem */
      resp_req_msg.socket_list[i].status = DFS_REMOTE_SOCKET_ALLOC_GENERAL_FAILURE_V01;
    }
    else
    {
      resp_req_msg.socket_list[i].socket_handle = (uint32_t) handle;
      resp_req_msg.socket_list[i].status = DFS_REMOTE_SOCKET_ALLOC_SUCCESS_V01;
      resp_req_msg.socket_list[i].socket_info.port_no = nm_mdmprxy_socket_mark_get_socket_port(handle);
    }
  }

  rc = qmi_client_send_msg_async(pr_qmi_dfs_clnt_hndl,
                                 QMI_DFS_REMOTE_SOCKET_ALLOCATED_REQ_V01,
                                 &resp_req_msg,
                                 sizeof(dfs_remote_socket_allocated_req_msg_v01),
                                 &resp_resp_msg,
                                 sizeof(dfs_remote_socket_allocated_resp_msg_v01),
                                 nm_mdmprxy_socket_mark_qmi_do_request_resp,
                                 0,
                                 &txn_handle);

  if (QMI_NO_ERR != rc)
  {
    netmgr_log_err("%s(): Error sending QMI_DFS_REMOTE_SOCKET_ALLOCATED_REQ_V01 message: %d",
                   __func__,
                   rc);
  }
  else
  {
    netmgr_log_low("%s(): Sent port request handles back to modem", __func__);
  }

  netmgr_log_low("%s(): Successfully processed QMI_DFS_REMOTE_SOCKET_REQUEST_IND_V01",
                 __func__);
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_qmi_do_release_ind
===========================================================================*/
/*!
@brief
  Callback to handle QMI_DFS_REMOTE_SOCKET_RELEASE_IND_V01 indication.
  Closes the socket specified by the modem.

*/
/*=========================================================================*/
static void
nm_mdmprxy_socket_mark_qmi_do_release_ind
(
  qmi_client_type    user_handle,
  unsigned int       msg_id,
  void               *ind_buf,
  unsigned int       ind_buf_len,
  void               *ind_cb_data
)
{
  qmi_client_error_type qmi_err;
  dfs_remote_socket_release_ind_msg_v01 req_msg;

  NM_MDMPRXY_VAR_UNUSED(ind_cb_data);

  qmi_err = qmi_client_message_decode(user_handle,
                                      QMI_IDL_INDICATION,
                                      msg_id,
                                      ind_buf,
                                      ind_buf_len,
                                      &req_msg,
                                      sizeof(req_msg));

  if(QMI_NO_ERR != qmi_err)
  {
    netmgr_log_err("%s(): failed qmi_client_message_decode with error %d", __func__, qmi_err);
    return;
  }

  if (!req_msg.socket_handles_valid)
  {
    nm_mdmprxy_socket_mark_close_all_ports();
    netmgr_log_low("%s(): Closed all socket handles", __func__);
  }
  else
  {
    int i;
    for (i = 0; i < req_msg.socket_handles_len && i < QMI_DFS_MAX_ALLOCATED_SOCKETS_V01; i++)
    {
      if(req_msg.socket_handles[i] >= MAX_OPEN_SOCKETS)
      {
        netmgr_log_err("%s(): Modem requested invalid socket handle (%u) be closed; skipping",
                       __func__,
                       req_msg.socket_handles[i]);
        continue;
      }
      nm_mdmprxy_socket_mark_close_socket(req_msg.socket_handles[i]);
      netmgr_log_low("%s(): Modem requested socket handle (%u) be closed",
                     __func__,
                     req_msg.socket_handles[i]);
    }
  }

  netmgr_log_low("%s(): Successfully processed QMI_DFS_REMOTE_SOCKET_RELEASE_IND_V01",
                 __func__);
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_qmi_do_release_ind
===========================================================================*/
/*!
@brief
  Callback to handle QMI_DFS_REMOTE_SOCKET_SET_OPTION_IND_V01 indication.
  Sets any options specified by the modem on a socket. As of now, only
  UDP_ENCAP is handled.

*/
/*=========================================================================*/
static void
nm_mdmprxy_socket_mark_qmi_do_set_option_ind
(
  qmi_client_type    user_handle,
  unsigned int       msg_id,
  void               *ind_buf,
  unsigned int       ind_buf_len,
  void               *ind_cb_data
)
{
  dfs_remote_socket_set_option_ind_msg_v01 req_msg;
  qmi_client_error_type qmi_err;
  int handle, rc, type;
  unsigned int port;

  NM_MDMPRXY_VAR_UNUSED(ind_cb_data);

  qmi_err = qmi_client_message_decode(user_handle,
                                      QMI_IDL_INDICATION,
                                      msg_id,
                                      ind_buf,
                                      ind_buf_len,
                                      &req_msg,
                                      sizeof(req_msg));
  if(QMI_NO_ERR != qmi_err)
  {
    netmgr_log_err("%s(): failed qmi_client_message_decode with error %d",
                   __func__, qmi_err);
    return;
  }

  handle = req_msg.socket_handle;

  if (handle >= MAX_OPEN_SOCKETS)
  {
    netmgr_log_err("%s(): Bad socket handle (%u) from modem",
                   __func__, handle);
    return;
  }

  if (pr_fds[handle] == 0)
  {
    netmgr_log_err("%s(): No matching socket found for handle (%u)",
                   __func__, handle);
    return;
  }

  type = nm_mdmprxy_socket_mark_get_socket_type(handle);
  port = nm_mdmprxy_socket_mark_get_socket_port(handle);

  if (req_msg.is_udp_encaps_valid)
  {
    netmgr_main_cfg_set_ike_port(0);

    if(req_msg.is_udp_encaps == 1)
    {
      if (pr_encap_handle >= 0)
      {
        netmgr_log_err("%s(): Tried to set more than one encap port at a time"
                       ". Old handle:%d Requested handle:%d",
                       __func__, pr_encap_handle, handle);
        return;
      }

      rc = nm_mdmprxy_socket_mark_set_udpencap(handle, 1);
      if (rc != NETMGR_SUCCESS) {
        netmgr_log_err("%s(): Failed to set UDP Encap mode", __func__);
        return;
      }

      rc = nm_mdmprxy_install_udpencap_rules(port,
                                             NM_MDMPRXY_IKE_REMOTE_PORT,
                                             NM_MDMPRXY_MARK);
      if (rc != NETMGR_SUCCESS) {
        netmgr_log_err("%s(): Failed to install UDP Encap rules", __func__);
        return;
      }

      netmgr_main_cfg_set_ike_port(port);
      pr_encap_handle = handle;
    }
    else
    {
      rc = nm_mdmprxy_socket_mark_set_udpencap(handle, 0);
      if (rc != NETMGR_SUCCESS) {
        netmgr_log_err("%s(): Failed to unset UDP Encap mode", __func__);
      }

      rc = nm_mdmprxy_uninstall_udpencap_rules(port,
                                               NM_MDMPRXY_IKE_REMOTE_PORT,
                                               NM_MDMPRXY_MARK);
      if (rc != NETMGR_SUCCESS) {
        netmgr_log_err("%s(): Failed to uninstall UDP Encap rules", __func__);
      }

      pr_encap_handle = -1;
    }
  }
  else
  {
    netmgr_log_med("%s(): No action specified", __func__);
  }

  netmgr_log_low("%s(): Successfully processed QMI_DFS_REMOTE_SOCKET_SET_OPTION_IND_V01",
                 __func__);
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_qmi_do_release_ind
===========================================================================*/
/*!
@brief
  Primary callback registered with QCCI framework. Based on the msg_id,
  the appropriate indication handler is invoked above.

*/
/*=========================================================================*/
static void
nm_mdmprxy_socket_mark_qmi_callback
(
  qmi_client_type    user_handle,
  unsigned int       msg_id,
  void               *ind_buf,
  unsigned int       ind_buf_len,
  void               *ind_cb_data
)
{
  qmi_client_error_type   qmi_err;
  dfs_low_latency_traffic_ind_msg_v01 filterModeStatus;

  (void) ind_cb_data;

  if (ind_buf == NULL)
  {
    netmgr_log_err("%s(): Called with null pointers!\n", __func__);
    return;
  }

  nm_mdmprxy_socket_mark_lock();
  switch(msg_id)
  {
    case QMI_DFS_REMOTE_SOCKET_REQUEST_IND_V01:
      nm_mdmprxy_socket_mark_qmi_do_request_ind(user_handle, msg_id, ind_buf, ind_buf_len, ind_cb_data);
      break;

    case QMI_DFS_REMOTE_SOCKET_RELEASE_IND_V01:
      nm_mdmprxy_socket_mark_qmi_do_release_ind(user_handle, msg_id, ind_buf, ind_buf_len, ind_cb_data);
      break;

    case QMI_DFS_REMOTE_SOCKET_SET_OPTION_IND_V01:
      nm_mdmprxy_socket_mark_qmi_do_set_option_ind(user_handle, msg_id, ind_buf, ind_buf_len, ind_cb_data);
      break;

    default:
      netmgr_log_err("%s(): Unhandled msg_id: %u", __func__, msg_id);
  }
  nm_mdmprxy_socket_mark_unlock();
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_release_qmi_clients
===========================================================================*/
/*!
@brief
  Cleanup the QMI DFS clients.

*/
/*=========================================================================*/
static void
nm_mdmprxy_socket_mark_release_qmi_clients()
{
  if (pr_qmi_state == PR_QMI_STATE_UNINITIALIZED)
  {
    netmgr_log_low("%s(): QMI Client is already uninitialized; skipping", __func__);
    return;
  }

  qmi_client_release(pr_qmi_dfs_clnt_hndl);
  pr_qmi_state = PR_QMI_STATE_UNINITIALIZED;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_socket_mark_init_qmi_clients
===========================================================================*/
/*!
@brief
  Initialize the QMI DFS client and register for all of the port reservation
  indications.

*/
/*=========================================================================*/
static void
nm_mdmprxy_socket_mark_init_qmi_clients()
{
  int rc;
  dfs_indication_register_req_msg_v01 request;
  dfs_indication_register_resp_msg_v01 response;

  /* Initialize the QCCI client */

  if (pr_qmi_state == PR_QMI_STATE_INITIALIZED)
  {
    netmgr_log_med("%s(): QMI Client is already initialized; skipping", __func__);
    return;
  }

  rc = qmi_client_init_instance(dfs_get_service_object_v01(),
                                QMI_CLIENT_INSTANCE_ANY,
                                nm_mdmprxy_socket_mark_qmi_callback,
                                NULL,
                                &pr_qcci_os_params,
                                NM_MDMPRXY_QMI_TIMEOUT,
                                &pr_qmi_dfs_clnt_hndl);

  if (QMI_NO_ERR != rc)
  {
    netmgr_log_err("%s(): failed on qmi_client_init_instance with rc=%d!\n", __func__, rc);
    return;
  }

  /* Register for socket indications */
  memset(&request, 0, sizeof(request));
  memset(&response, 0, sizeof(response));
  request.remote_socket_handling = TRUE;
  request.remote_socket_handling_valid = TRUE;

  rc = qmi_client_send_msg_sync(pr_qmi_dfs_clnt_hndl,
                                QMI_DFS_INDICATION_REGISTER_REQ_V01,
                                &request,
                                sizeof(dfs_indication_register_req_msg_v01),
                                &response,
                                sizeof(dfs_indication_register_resp_msg_v01),
                                NM_MDMPRXY_QMI_TIMEOUT);

  if (QMI_NO_ERR != rc)
  {
    netmgr_log_err("%s(): Error sending QMI_DFS_INDICATION_REGISTER_REQ_V01 message: %d",
                   __func__,
                   rc);
    goto bail;
  }

  pr_qmi_state = PR_QMI_STATE_INITIALIZED;
  return;

bail:
  nm_mdmprxy_socket_mark_release_qmi_clients();
}



/*===========================================================================
  FUNCTION  nm_mdmprxy_legacy_mark_prov_remove_rules
===========================================================================*/
/*!
@brief
  Remove iptables marking rules

@return
  NETMGR_SUCCESS on successful command execution
  NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
static int
nm_mdmprxy_socket_mark_uninstall_static_rules
(
  sa_family_t af,
  int start
)
{
  int num_rules = 0, i = 0;
  int rc = NETMGR_FAILURE;

  if (AF_INET != af && AF_INET6 != af)
  {
    netmgr_log_err("%s(): Unknown IP family!", __func__);
    goto bail;
  }

  for (i = start; i >= 0; i--)
  {
    if (!static_rule_list[i].rule)
    {
      continue;
    }

    rc = nm_mdmprxy_process_rule(&static_rule_list[i], NM_MDMPRXY_RUN_ANTIRULE, af, 0);
    if (NETMGR_SUCCESS != rc)
    {
      netmgr_log_err("%s(): Failed to apply rule [%d]", __func__, i);
      goto bail;
    }
  }

  netmgr_log_low("%s(): Successfully removed marking rules for ip_family [%d]", __func__, af);
  NM_MDMPRXY_SET_MARKING_RULES_INSTALLED(af, FALSE);

bail:
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_legacy_mark_prov_install_rules
===========================================================================*/
/*!
@brief
  Install iptables marking rules

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static int
nm_mdmprxy_socket_mark_install_static_rules
(
  sa_family_t af
)
{
  int num_rules = 0, i = 0;
  int rc = NETMGR_FAILURE;


  if (AF_INET != af && AF_INET6 != af)
  {
    netmgr_log_err("%s(): Unknown IP family!", __func__);
    goto bail;
  }

  num_rules = nm_mdmprxy_rules_len(static_rule_list);

  for (i = 0; i < num_rules; i++)
  {
    if (!static_rule_list[i].rule)
    {
      continue;
    }

    rc = nm_mdmprxy_process_rule(&static_rule_list[i], NM_MDMPRXY_RUN_RULE, af, 0);
    if (NETMGR_SUCCESS != rc)
    {
      netmgr_log_err("%s(): Failed to apply rule [%d]", __func__, i);
      nm_mdmprxy_socket_mark_uninstall_static_rules(af, i);
      goto bail;
    }
  }

  netmgr_log_low("%s(): Successfully installed marking rules for ip_family [%d]", __func__, af);
  NM_MDMPRXY_SET_MARKING_RULES_INSTALLED(af, TRUE);

bail:
  return rc;
}

/*===========================================================================
  FUNCTION nm_mdmprxy_socket_mark_iptables_init
===========================================================================*/
/*!
@brief
  Reset the iiface handles and install the static fules for AF_INET
  and AF_INET6
*/
/*=========================================================================*/
void
nm_mdmprxy_socket_mark_iptables_init()
{
  nm_mdmprxy_socket_mark_install_static_rules(AF_INET);
  nm_mdmprxy_socket_mark_install_static_rules(AF_INET6);

  netmgr_log_low("%s(): Initialization finished", __func__);
}

/*===========================================================================
  FUNCTION nm_mdmprxy_socket_mark_iptables_uninit
===========================================================================*/
/*!
@brief
  Clean up all iptables rules
*/
/*=========================================================================*/
void
nm_mdmprxy_socket_mark_iptables_uninit()
{
  nm_mdmprxy_socket_mark_uninstall_static_rules(AF_INET,
                                                nm_mdmprxy_rules_len(static_rule_list) - 1);

  nm_mdmprxy_socket_mark_uninstall_static_rules(AF_INET6,
                                                nm_mdmprxy_rules_len(static_rule_list) - 1);

  /* Remove common rules */
  nm_mdmprxy_common_mark_prov_oos_cleanup();
}

static int
nm_mdmprxy_socket_mark_prov_init(void)
{
  socket_mark_prov_cmd_tbl.udp_encap_rule_list       = udp_encap_rule_list;
  socket_mark_prov_cmd_tbl.udp_encap_rule_list_len   = (int) nm_mdmprxy_rules_len(udp_encap_rule_list);
  nm_mdmprxy_common_mark_prov_setup_iptables(&socket_mark_prov_cmd_tbl);
  (void) nm_mdmprxy_register_mark_prov_cmd_tbl(&socket_mark_prov_cmd_tbl);

  /* Begin protected section */
  nm_mdmprxy_socket_mark_lock();
  pr_encap_handle = -1;
  nm_mdmprxy_socket_mark_set_reserved_ports();
  nm_mdmprxy_socket_mark_iptables_init();
  nm_mdmprxy_socket_mark_unlock();
  netmgr_log_low("%s(): Socket-mark provider init complete", __func__);

  return 0;
}

static int
nm_mdmprxy_socket_mark_prov_is_hdlr(void)
{
  nm_mdmprxy_socket_mark_lock();
  nm_mdmprxy_socket_mark_init_qmi_clients();

  /* Install IMS audio port-forwarding rules, set the preference to TRUE indicating that we
     want to install the rules since this is an SSR in-service scenario */
  nm_mdmprxy_common_mark_prov_install_portfwd_rule(AF_INET, TRUE);
  nm_mdmprxy_common_mark_prov_install_portfwd_rule(AF_INET6, TRUE);

  nm_mdmprxy_socket_mark_unlock();
  netmgr_log_low("%s(): Socket-mark provider IS handler complete", __func__);
  return 0;
}

static int
nm_mdmprxy_socket_mark_prov_oos_hdlr(void)
{
  /* Begin protected section */
  nm_mdmprxy_socket_mark_lock();
  nm_mdmprxy_socket_mark_release_qmi_clients();
  nm_mdmprxy_socket_mark_close_all_ports();
  /* Remove common rules */
  nm_mdmprxy_common_mark_prov_oos_cleanup();
  nm_mdmprxy_socket_mark_unlock();
  netmgr_log_low("%s(): Socket-mark provider OOS handler complete", __func__);

  return 0;
}

