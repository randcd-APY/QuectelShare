/*!
  @file
  qcmap_cli_main.cpp

  @brief
  basic QCMAP Command Line Module Client Main

  Copyright (c) 2011-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
*/
/*=========================================================================*/

/*=========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/11/12   gk      Created module.
10/26/12   cp      Added support for Dual AP and different types of NAT.
02/27/13   cp      Added support to get IPV6 WAN status.
04/17/13   mp      Added support to get IPv6 WWAN/STA mode configuration.
06/12/13   sg      Added support for DHCP reservation.
09/17/13   at      Added support to Enable/Disable ALGs
01/11/14   sr      Added support for connected devices in SoftAP
03/25/17   spr     Added support for Multi-PDN.
05/20/17   gs      Added GSB Support
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <string>
#include "comdef.h"
#include "QCMAP_Client.h"
#include "limits.h"
#include "ds_util.h"
/* The original delay for dss_init was 6 seconds. Allowing this value to be
   set at runtime to determine if delay still necessary. */
#define QCMAP_DEFAULT_DSS_INIT_TIME    6
#define MAX_PORT_VALUE           65535
#define MIN_DHCP_LEASE 120 /*Lease time in seconds */
#define MIN_NOTIFY_INTERVAL 30
#define MAX_NOTIFY_INTERVAL 60000
#define MAC_HEX_STRING "0123456789abcdefABCDEF" /*MAC hex check*/
#define MAC_NULL_STRING "00:00:00:00:00:00" /*MAC Null String*/
#define INET_ADDRSTRLEN        16
#define INET6_ADDRSTRLEN       46

#define CLI_DEBUG_OPTION             500
#define ENABLE_STA_ONLY_DEBUG_MODE   CLI_DEBUG_OPTION+1
#define DISABLE_STA_ONLY_DEBUG_MODE  CLI_DEBUG_OPTION+2
#define REGISTER_FOR_WLAN_STATUS_IND CLI_DEBUG_OPTION+3

#define MAX_BACKHAUL_SUPPORTED       5
#define MAX_UINT32_VAL               4294967295   /*2^32 - 2*/
#define MAX_UINT16_VAL               65535        /*2^16 - 1*/
#define MAX_VLAN_ID                  4094/*vlan 4095 is max and it is reserved*/
#define MAX_BACKHAUL_TYPE_LENGTH     128

uint8 mac_addr_int[QCMAP_MSGR_MAC_ADDR_LEN_V01]; /*byte array of mac address*/
char mac_addr_str[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01]; /*char array of mac address*/

#define QCMAP_CLI_LOG(...)                         \
  fprintf( stderr, "\n%s %d:", __FILE__, __LINE__); \
  fprintf( stderr, __VA_ARGS__ )


#define WLAN_CARD_TYPE(type) ((type == QCMAP_MSGR_WLAN_DEV_TUF_V01)? "TUFFELO" : \
                            ((type == QCMAP_MSGR_WLAN_DEV_ROME_V01)? "ROME": "INVALID"))
#define WLAN_AP_TYPE(type) \
  (((type == QCMAP_MSGR_WLAN_IFACE_PRIMARY_AP_V01)? "Primary" : \
    ((type == QCMAP_MSGR_WLAN_IFACE_GUEST_AP_ONE_V01) ? "Guest" : \
      (type == QCMAP_MSGR_WLAN_IFACE_GUEST_AP_TWO_V01) ? "Guest" :"Station")))

#define WLAN_STATE_TYPE(type) ((type == QCMAP_MSGR_WLAN_IFACE_ACTIVE_V01)? "Enabled" : "Disabled")

boolean read_firewall_conf(qcmap_msgr_firewall_conf_t *extd_firewall_add);
void Dump_firewall_conf( qcmap_msgr_firewall_entry_conf_t *firewall_entry);

/* CLI restructure function declarations */
void mobileApConfig(int);
void lanConfig(int);
void nat_alg_vpn_config(int);
void wlanConfig(int);
void firewallConfig(int);
void backhaulConfig(int);
void backhaulCommConfig(int);
profile_handle_type_v01 ChooseWWANProfileHandle();
int16_t ChooseLANBridge(void);
void backhaulWWANConfig(int);
void backhaulWWANUpdateConfig( int );
void tetheringConfig(int);
void mediaServiceConfig(int);

/* private static helper functions for SOCKSv5 Configuration */
static boolean EnableDisableSOCKSv5(qmi_error_type_v01 *qmi_err_num);
static boolean SetSOCKSv5Config(qmi_error_type_v01 *qmi_err_num);
static boolean PromptUserSOCKSv5UnameAssoc(qmi_error_type_v01 *qmi_err_num);
static boolean GetUserInputSOCKSv5AuthMethod(unsigned char *auth_method);
static boolean GetUserInputSOCKSv5LANIface(char *lan_iface);
static boolean GetUserInputSOCKSv5ConfigFilePath(char* conf_file, char* auth_file);
static boolean GetUserInputSOCKSv5Uname(char *uname);
static boolean GetUserInputSOCKSv5ServiceNo(unsigned int *service_no);
static boolean CheckSOCKSv5UnameLen(char *str);


QCMAP_Client *QcMapClient = NULL;

const char* options_list[]=
{
"1. MobileAP Configuration                                    ",
"2. LAN Configuration                                         ",
"3. NAT/ALG/VPN Configuration                                 ",
"4. WLAN Configuration                                        ",
"5. Firewall Configuration                                    ",
"6. Backhaul Configuration                                    ",
"7. Tethering Configuration                                   ",
"8. Media Service Configuration                               ",
"9. Generic Software Bridge Configuration                     "
};


const char* mobileAp_configuration_list[]=
{
"1.  Display Current Config                                   ",
"2.  Enable/Disable mobileap                                  ",
"3.  Get MobileAP status                                      ",
"4.  Get Connected Device info                                ",
"5.  Enable/Disable/Reset Packet Stats                        ",
"6.  Get Packet Stats Status                                  ",
"7.  Restore Factory Default Settings(** Will Reboot Device ) ",
"8.  Teardown/Disable and Exit                                ",
"9.  Set Data Path Optimization Flag                          ",
"10. Get Data Path Optimization Flag                          ",
};

const char* lan_configuration_list[]=
{
"1.  Set  LAN Config                                           ",
"2.  Get  LAN Config                                           ",
"3.  Activate  LAN                                             ",
"4.  Add DHCP Reservation Record                               ",
"5.  Get DHCP Reservation Records                              ",
"6.  Edit DHCP Reservation Record                              ",
"7.  Delete DHCP Reservation Record                            ",
"8.  Set/Get Gateway URL                                       ",
"9.  Add VLAN Interface                                        ",
"10. Get VLAN Interfaces                                       ",
"11. Delete VLAN Interface                                     ",
"12. Set L2TP Unmnaged Tunnel state                            ",
"13. Set L2TP Config                                           ",
"14. Get L2TP Config                                           ",
"15. Delete L2TP Config                                        ",
"16. Set Bridge-VLAN Context                                   ",
"17. Get Bridge-VLAN Context                                   ",
};

const char* nat_alg_vpn_configuration_list[]=
{
"1.  Add SNAT Entry                                           ",
"2.  Delete SNAT Entry                                        ",
"3.  Get SNAT Config                                          ",
"4.  Set NAT Type                                             ",
"5.  Get NAT Type                                             ",
"6.  Set NAT Timeout                                          ",
"7.  Get NAT Timeout                                          ",
"8.  Add DMZ IP                                               ",
"9.  Get DMZ IP                                               ",
"10. Delete DMZ IP                                            ",
"11. Set IPSEC VPN Passthrough                                ",
"12. Get IPSEC VPN Passthrough                                ",
"13. Set PPTP VPN Passthrough                                 ",
"14. Get PPTP VPN Passthrough                                 ",
"15. Set L2TP VPN Passthrough                                 ",
"16. Get L2TP VPN Passthrough                                 ",
"17. Enable/Disable ALG                                       ",
"18. Set SIP server info                                      ",
"19. Get SIP server info                                      ",
"20. Set Initial Packet Threshold                             ",
"21. Get Initial Packet Threshold                             ",
"22. Enable/Disable SOCKSv5Proxy                              ",
"23. Set SOCKSv5 Proxy Config                                 ",
"24. Get SOCKSv5 Proxy Config                                 ",
};

const char* wlan_configuration_list[]
{
"1.  Enable/Disable WLAN                                      ",
"2.  Activate WLAN                                            ",
"3.  Set WLAN Config                                          ",
"4.  Get WLAN Config                                          ",
"5.  Get WLAN Status                                          ",
"6.  Set MobileAP/WLAN Bootup Config                          ",
"7.  Get MobileAP/WLAN Bootup Config                          ",
"8.  Get Station Mode Status                                  ",
"9.  Activate Hostapd Config                                  ",
"10. Activate Supplicant Config                               ",
"11. Get WLAN IF information                                  ",
"12. Set Always on WLAN                                       ",
"13. Get Always on WLAN                                       ",
"14. Set Peer to Peer Role                                    ",
"15. Get Peer to Peer Role                                    "
};

const char* firewall_configuration_list[]
{
"1. Add Firewall Entry                                        ",
"2. Set Firewall Config                                       ",
"3. Get Firewall Config                                       ",
"4. Display Firewalls                                         ",
"5. Delete Firewall Entry                                     "
};

const char* backhaul_configuration_list[]
{
"1. Backhaul Common                                           ",
"2. Backhaul WWAN                                             "
};

const char* backhaul_common_configuration_list[]
{
"1.  Get Network Configuration         ",
"2.  Configure Active Backhaul Priority",
"3.  Get Backhaul Priority             ",
"4.  Get Data Bitrate                  ",
"5.  Enable/Disable IPV4               ",
"6.  Enable/Disable IPV6               ",
"7.  Get IPv4 State                    ",
"8.  Get IPv6 State                    ",
"9.  Get Backhaul Status               "
};

const char* backhaul_wwan_configuration_list[]
{
"1.  Connect/Disconnect Backhaul       ",
"2.  Get WWAN status                    ",
"3.  Get WWAN Statistics               ",
"4.  Reset WWAN Statistics             ",
"5.  Set Webserver WWAN access flag    ",
"6.  Get Webserver WWAN access flag    ",
"7.  Set WWAN Profile                  ",
"8.  Get WWAN Profile                  ",
"9.  Set Prefix Delegation Config      ",
"10. Get Prefix Delegation Config      ",
"11. Get Prefix Delegation Status      ",
"12. Enable/Disable TinyProxy          ",
"13. Get TinyProxy Status              ",
"14. Set IP Passthrough Config         ",
"15. Get IP Passthrough Config         ",
"16. Get IP Passthrough State          ",
"17. Set Autoconnect Config            ",
"18. Get Autoconnect Config            ",
"19. Set Roaming                       ",
"20. Get Roaming                       ",
"21. Enable/Disable DDNS               ",
"22. Set DDNS Config                   ",
"23. Get DDNS Config                   ",
"24. Switch WWAN Profile               ",
"25. Create WWAN Profile               ",
"26. Update WWAN Profile               ",
"27. Delete WWAN Profile               ",
"28. Add/Delete PDN to VLAN Mapping    ",
"29. Get All PDN to VLAN Mappings      ",
"30. Set PMIP mode                     ",
"31. Get PMIP mode                     ",
"32. Get WWAN Roaming status           ",
"33. Get Current Profile Handle        "
};

const char *backhaul_wwan_update_configuration_list[]
{
"1. Update Tech Type",
"2. Update 3GPP (V4) Profile",
"3. Update 3GPP2 (V4) Profile",
"4. Update 3GPP (V6) Profile",
"5. Update 3GPP2 (V6) Profile",
"6. Update All (3GPP/3GPP2 (V4/V6) Profile",
"7. Set as default Profile",
};

const char* tethering_configuration_list[]
{
"1. Set Cradle Mode                                           ",
"2. Get Cradle Mode                                           ",
"3. Set Ethernet mode                                         ",
"4. Get Ethernet mode                                         ",
"5. Get BT Tethering Status                                   ",
"6. Set Dun Dongle Mode                                       ",
"7. Get Dun Dongle Mode                                       "
};

const char* media_service_configuration_list[]
{
"1.  Enable/Disable UPnP                                      ",
"2.  Get UPnP Status                                          ",
"3.  Set UPnP Notify Interval                                 ",
"4.  Get UPnP Notify Interval                                 ",
"5.  Set UPNPPinhole State                                    ",
"6.  Get UPNPPinhole State                                    ",
"7.  Enable/Disable DLNA                                      ",
"8.  Set DLNA Notify Interval                                 ",
"9.  Get DLNA Notify Interval                                 ",
"10. Get DLNA Status                                          ",
"11. Set DLNA Media Directory                                 ",
"12. Get DLNA Media Directory                                 ",
"13. Set DLNAWhitelisting                                     ",
"14. Get DLNAWhitelisting                                     ",
"15. Add DLNAWhitelistingIP                                   ",
"16. Delete DLNAWhitelistingIP                                ",
"17. Enable/Disable M-DNS                                     ",
"18. Get MDNS Status                                          "
};

const char* software_bridge_config_list[]=
{
"1. Set Generic Software Bridge Config                         ",
"2. Enable/Disable(1/0) Generic Software Bridge                ",
"3. Get Generic Software Bridge Config                         ",
"4. Delete Generic Software Bridge Config                         ",
};


/* Teardown/Disable and Exit should always be the last option.
  Always keep an even number of elements in array.  If array size is odd,
  add an emtpy placeholder to the end of the array.  When adding new options
  always replace empty options first.*/

/*===========================================================================
  FUNCTION  DisplayClientInfo
  ===========================================================================*/
DisplayClientInfo(qcmap_msgr_packet_stats_status_ind_msg_v01 ind_data)
{
  char tmpIPv4[INET_ADDRSTRLEN];
  in6_addr allipv6[QCMAP_MSGR_MAX_IPV6_ADDR_V01];
  in6_addr tmpipv6;
  uint8 zero_buff[QCMAP_MSGR_IPV6_ADDR_LEN_V01] = {0};
  char ip6_addr_buf[INET6_ADDRSTRLEN];
  int connDevCount=0;
  char mac_addr_str[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01]; /*char array of mac address*/
  memset(tmpIPv4,0,INET_ADDRSTRLEN);
  if (ind_data.number_of_entries != 0)
  {
    // Displaying the information in appropriate fashion
    for (connDevCount=0; connDevCount<ind_data.number_of_entries; connDevCount++)
    {

      ds_mac_addr_ntop(ind_data.info[connDevCount].client_mac_addr,
                       mac_addr_str);
      printf("MAC Address : %s \n",mac_addr_str);
      if(inet_ntop(AF_INET,
                   (void *)&ind_data.info[connDevCount].ipv4_addr,tmpIPv4,
                   INET_ADDRSTRLEN))
      {
        printf("IPv4 Address : %s \n",tmpIPv4);
      }
      memset(&tmpipv6, 0, sizeof(tmpipv6));
      memcpy(&tmpipv6.s6_addr,
             ind_data.info[connDevCount].ll_ipv6_addr,
             QCMAP_MSGR_IPV6_ADDR_LEN_V01);
      if(inet_ntop(AF_INET6,
                   (void*)&tmpipv6, ip6_addr_buf, sizeof(ip6_addr_buf)))
      {
        printf("Link Local IPv6 Address : %s\n",ip6_addr_buf);
      }

     for (int i = 0; i < QCMAP_MSGR_MAX_IPV6_ADDR_V01; i++)
     {
       memset(&allipv6[i], 0, sizeof(in6_addr));
       memcpy(&allipv6[i].s6_addr,
              ind_data.info[connDevCount].ipv6[i].addr,
               QCMAP_MSGR_IPV6_ADDR_LEN_V01);
       if (!memcmp(&allipv6[i].s6_addr, zero_buff,QCMAP_MSGR_IPV6_ADDR_LEN_V01))
         break;
       if(inet_ntop(AF_INET6,
                    (void*)&allipv6[i], ip6_addr_buf, sizeof(ip6_addr_buf)))
       {
         printf("IPv6 Address %d: %s\n",i, ip6_addr_buf);
       }
     }
     switch (ind_data.info[connDevCount].device_type)
     {
       case QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01:
         printf("Device Type : Primary AP\n");
         break;
       case QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01:
         printf("Device Type :Guest AP1\n");
         break;
       case QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01:
         printf("Device Type :Guest AP2\n");
         break;
       case QCMAP_MSGR_DEVICE_TYPE_USB_V01:
         printf("Device Type :USB\n");
         break;
       case QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01:
         printf("Device Type :Ethernet\n");
         break;
       default:
         printf("Device Type : Invalid\n");
         break;
     }
     printf("Host Name : %s\n",
             ind_data.info[connDevCount].host_name);
     printf("rx bytes : %llu\n",
             ind_data.info[connDevCount].bytes_rx);
     printf("tx bytes : %llu\n",
             ind_data.info[connDevCount].bytes_tx);
     printf("Lease Expiry Time (in minutes) : %d\n\n\n",
             ind_data.info[connDevCount].lease_expiry_time);
    }
  }
  else
  {
    printf("\n CLI:No Connected Device to this Access Point \n");
  }
}

/*===========================================================================
  FUNCTION  convert_backhaul_enum_to_string
  ===========================================================================*/
/*!
  @brief
  Converts backhaul enum to string.

  @return
  void

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
boolean convert_backhaul_enum_to_string
(
  qcmap_msgr_backhaul_type_enum_v01 backhaul_type,
  char *backhaul_type_string
)
{
  if (backhaul_type_string == NULL)
  {
    LOG_MSG_ERROR("NULL Args", 0, 0, 0);
    return false;
  }
  switch (backhaul_type)
  {
    case QCMAP_MSGR_WWAN_BACKHAUL_V01:
      snprintf(backhaul_type_string,MAX_BACKHAUL_TYPE_LENGTH,"WWAN");
      break;
    case QCMAP_MSGR_USB_CRADLE_BACKHAUL_V01:
      snprintf(backhaul_type_string,MAX_BACKHAUL_TYPE_LENGTH,"USB CRADLE");
      break;
    case QCMAP_MSGR_WLAN_BACKHAUL_V01:
      snprintf(backhaul_type_string,MAX_BACKHAUL_TYPE_LENGTH,"WLAN");
      break;
    case QCMAP_MSGR_ETHERNET_BACKHAUL_V01:
      snprintf(backhaul_type_string,MAX_BACKHAUL_TYPE_LENGTH,"ETHERNET");
      break;
    case QCMAP_MSGR_BT_BACKHAUL_V01:
      snprintf(backhaul_type_string,MAX_BACKHAUL_TYPE_LENGTH,"BT");
      break;
    default:
      LOG_MSG_ERROR("Invalid Backhaul type", 0, 0, 0);
      return false;
      break;
  }
  return true;
}

/*===========================================================================
  FUNCTION  qcmap_msgr_qmi_qcmap_ind
  ===========================================================================*/
/*!
  @brief
  Processes an incoming QMI QCMAP Indication.

  @return
  void

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
void qcmap_msgr_qmi_qcmap_ind
(
 qmi_client_type user_handle,                    /* QMI user handle       */
 unsigned int    msg_id,                         /* Indicator message ID  */
 void           *ind_buf,                        /* Raw indication data   */
 unsigned int    ind_buf_len,                    /* Raw data length       */
 void           *ind_cb_data                     /* User call back handle */
 )
{
  qmi_client_error_type qmi_error;
  profile_handle_type_v01 profile_handle;
  char command[MAX_COMMAND_STR_LEN] = {0}, buffer[MAX_BACKHAUL_TYPE_LENGTH] = {0};

  QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: user_handle %X msg_id %d ind_buf_len %d.",
          user_handle, msg_id, ind_buf_len);

  switch (msg_id)
  {
    case QMI_QCMAP_MSGR_PACKET_STATS_STATUS_IND_V01:
    {
      qcmap_msgr_packet_stats_status_ind_msg_v01 ind_data;

      qmi_error = qmi_client_message_decode(user_handle,
                                            QMI_IDL_INDICATION,
                                            msg_id,
                                            ind_buf,
                                            ind_buf_len,
                                            &ind_data,
                                            sizeof(qcmap_msgr_packet_stats_status_ind_msg_v01));
      if (qmi_error != QMI_NO_ERR)
      {
        QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: qmi_client_message_decode error %d\n",qmi_error);
        break;
     }
      /* Process packet service status indication for packet stats for QCMAP*/
     switch (ind_data.conn_status)
     {
     case QCMAP_MSGR_PACKET_STATS_CLIENT_CONNECTED_V01:
       QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: A new client is Connected\n\n");
         break;
     case QCMAP_MSGR_PACKET_STATS_CLIENT_DISCONNECTED_V01:
       QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: Client is disconnected\n\n");
         break;
     case QCMAP_MSGR_PACKET_STATS_IPV4_UPDATED_V01:
       QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: IPV4 Updated\n\n");
         break;
     case QCMAP_MSGR_PACKET_STATS_IPV6_UPDATED_V01:
       QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: IPV6 Updated\n\n");
         break;
     case QCMAP_MSGR_PACKET_STATS_WLAN_DISABLED_V01:
       QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: WLAN Disabled\n\n");
         break;
     case QCMAP_MSGR_PACKET_STATS_MOBILEAP_DISABLED_V01:
       QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: MOBILEAP Teardown\n\n");
         break;
     case QCMAP_MSGR_PACKET_STATS_USB_DISCONNECTED_V01:
       QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: USB Disconnected\n\n");
         break;
     case QCMAP_MSGR_PACKET_STATS_IPV4_WWAN_DISCONNECTED_V01:
       QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: IPV4 BH Disconnected\n\n");
         break;
     case QCMAP_MSGR_PACKET_STATS_IPV6_WWAN_DISCONNECTED_V01:
       QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: IPV6 BH Disconnected\n\n");
         break;
     case QCMAP_MSGR_PACKET_STATS_BH_SWITCHED_V01:
       QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: Backhaul is Switched \n\n");
         break;
      }
      DisplayClientInfo(ind_data);
      break;
    }

    case QMI_QCMAP_MSGR_BRING_UP_WWAN_IND_V01:
    {
      qcmap_msgr_bring_up_wwan_ind_msg_v01 ind_data;

      qmi_error = qmi_client_message_decode(user_handle,
                                            QMI_IDL_INDICATION,
                                            msg_id,
                                            ind_buf,
                                            ind_buf_len,
                                            &ind_data,
                                            sizeof(qcmap_msgr_bring_up_wwan_ind_msg_v01));
      if (qmi_error != QMI_NO_ERR)
      {
        QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: qmi_client_message_decode error %d",qmi_error);
        break;
      }

      profile_handle = (ind_data.profile_handle_valid == TRUE) ? ind_data.profile_handle: 0xFFFF;
      /* Process packet service status indication for WWAN for QCMAP*/
      if (ind_data.conn_status == QCMAP_MSGR_WWAN_STATUS_CONNECTED_V01)
      {
        if (ind_data.mobile_ap_handle == QcMapClient->mobile_ap_handle)
        {
          QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: PDN=%p IPV4 WWAN Connected", profile_handle);
          return;
        }
      }
      else if (ind_data.conn_status == QCMAP_MSGR_WWAN_STATUS_CONNECTING_FAIL_V01)
      {
        if (ind_data.mobile_ap_handle == QcMapClient->mobile_ap_handle)
        {
          QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: PDN=%p IPV4 WWAN Connecting Failed...WAN CallendType=%d CallendCode=%d",
                            profile_handle,
                            ind_data.wwan_call_end_reason.wwan_call_end_reason_type,
                            ind_data.wwan_call_end_reason.wwan_call_end_reason_code);
          return;
        }
      }
      else if (ind_data.conn_status == QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTED_V01)
      {
        if (ind_data.mobile_ap_handle == QcMapClient->mobile_ap_handle)
        {
          QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: PDN=%p IPV6 WWAN Connected", profile_handle);
          return;
        }
      }
      else if (ind_data.conn_status == QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTING_FAIL_V01)
      {
        if (ind_data.mobile_ap_handle == QcMapClient->mobile_ap_handle)
        {
          QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: PDN=%p IPV6 WWAN Connecting Failed...WAN CallendType=%d CallendCode=%d",
                                profile_handle,
                                ind_data.wwan_call_end_reason.wwan_call_end_reason_type,
                                ind_data.wwan_call_end_reason.wwan_call_end_reason_code);

          return;
        }
      }

      break;
    }
    case QMI_QCMAP_MSGR_TEAR_DOWN_WWAN_IND_V01:
    {
      qcmap_msgr_tear_down_wwan_ind_msg_v01 ind_data;

      qmi_error = qmi_client_message_decode(user_handle,
                                            QMI_IDL_INDICATION,
                                            msg_id,
                                            ind_buf,
                                            ind_buf_len,
                                            &ind_data,
                                            sizeof(qcmap_msgr_tear_down_wwan_ind_msg_v01));
      if (qmi_error != QMI_NO_ERR)
      {
        QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: qmi_client_message_decode error %d",qmi_error);
        break;
      }

      profile_handle = (ind_data.profile_handle_valid == TRUE) ? ind_data.profile_handle: 0xFFFF;
      if (ind_data.conn_status == QCMAP_MSGR_WWAN_STATUS_DISCONNECTED_V01)
      {
        if (ind_data.mobile_ap_handle == QcMapClient->mobile_ap_handle)
        {
          QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: PDN=%p IPV4 WWAN Disconnected...WAN CallendType=%d CallendCode=%d",
                            profile_handle,
                            ind_data.wwan_call_end_reason.wwan_call_end_reason_type,
                            ind_data.wwan_call_end_reason.wwan_call_end_reason_code);

          return;
        }
      }
      else if (ind_data.conn_status == QCMAP_MSGR_WWAN_STATUS_DISCONNECTING_FAIL_V01)
      {
        if (ind_data.mobile_ap_handle == QcMapClient->mobile_ap_handle)
        {
          QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: PDN=%p IPV4 WWAN Disconnecting Failed...WAN CallendType=%d CallendCode=%d",
                            profile_handle,
                            ind_data.wwan_call_end_reason.wwan_call_end_reason_type,
                            ind_data.wwan_call_end_reason.wwan_call_end_reason_code);
          return;
        }
      }
      else if (ind_data.conn_status == QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTED_V01)
      {
        if (ind_data.mobile_ap_handle == QcMapClient->mobile_ap_handle)
        {
          QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: PDN=%p IPV6 WWAN Disconnected...WAN CallendType=%d CallendCode=%d",
                            profile_handle,
                            ind_data.wwan_call_end_reason.wwan_call_end_reason_type,
                            ind_data.wwan_call_end_reason.wwan_call_end_reason_code);

          return;
        }
      }
      else if (ind_data.conn_status == QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTING_FAIL_V01)
      {
        if (ind_data.mobile_ap_handle == QcMapClient->mobile_ap_handle)
        {
          QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: PDN=%p IPV6 WWAN Disconnecting Failed...WAN CallendType=%d CallendCode=%d",
                            profile_handle,
                            ind_data.wwan_call_end_reason.wwan_call_end_reason_type,
                            ind_data.wwan_call_end_reason.wwan_call_end_reason_code);
          return;
        }
      }

      break;
    }
    case QMI_QCMAP_MSGR_WWAN_STATUS_IND_V01:
    {
      qcmap_msgr_wwan_status_ind_msg_v01 ind_data;

      qmi_error = qmi_client_message_decode(user_handle,
                                            QMI_IDL_INDICATION,
                                            msg_id,
                                            ind_buf,
                                            ind_buf_len,
                                            &ind_data,
                                            sizeof(qcmap_msgr_wwan_status_ind_msg_v01));
      if (qmi_error != QMI_NO_ERR)
      {
        QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: qmi_client_message_decode error %d",qmi_error);
        break;
      }

      profile_handle = (ind_data.profile_handle_valid == TRUE) ? ind_data.profile_handle: 0xFFFF;
      if (ind_data.wwan_status == QCMAP_MSGR_WWAN_STATUS_DISCONNECTED_V01)
      {
        QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: PDN=%p IPV4 WWAN Disconnected...WAN CallendType=%d, CallendCode=%d",
                            profile_handle,
                            ind_data.wwan_call_end_reason.wwan_call_end_reason_type,
                            ind_data.wwan_call_end_reason.wwan_call_end_reason_code);
        return;
      }
      else if (ind_data.wwan_status == QCMAP_MSGR_WWAN_STATUS_DISCONNECTING_FAIL_V01)
      {
        QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: PDN=%p IPV4 WWAN Disconnecting Failed...WAN CallendType=%d CallendCode=%d",
                            profile_handle,
                            ind_data.wwan_call_end_reason.wwan_call_end_reason_type,
                            ind_data.wwan_call_end_reason.wwan_call_end_reason_code);
        return;
      }
      else if (ind_data.wwan_status == QCMAP_MSGR_WWAN_STATUS_CONNECTED_V01)
      {
        QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: PDN=%p IPv4 WWAN Connected...", profile_handle);
        return;
      }
      else if (ind_data.wwan_status == QCMAP_MSGR_WWAN_STATUS_CONNECTING_FAIL_V01)
      {
        QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: PDN=%p IPV4 WWAN Connecting Failed...WAN CallendType=%d CallendCode=%d",
                            profile_handle,
                            ind_data.wwan_call_end_reason.wwan_call_end_reason_type,
                            ind_data.wwan_call_end_reason.wwan_call_end_reason_code);
        return;
      }
      else if (ind_data.wwan_status == QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTED_V01)
      {
        QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: PDN=%p IPV6 WWAN Disconnected...WAN CallendType=%d CallendCode=%d",
                            profile_handle,
                            ind_data.wwan_call_end_reason.wwan_call_end_reason_type,
                            ind_data.wwan_call_end_reason.wwan_call_end_reason_code);
        return;
      }
      else if (ind_data.wwan_status == QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTING_FAIL_V01)
      {
        QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: PDN=%p IPV6 WWAN Disconnecting Failed...WAN CallendType=%d CallendCode=%d",
                            profile_handle,
                            ind_data.wwan_call_end_reason.wwan_call_end_reason_type,
                            ind_data.wwan_call_end_reason.wwan_call_end_reason_code);
        return;
      }
      else if (ind_data.wwan_status == QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTED_V01)
      {
        QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: PDN=%p IPV6 WWAN Connected...", profile_handle);
        return;
      }
      else if (ind_data.wwan_status == QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTING_FAIL_V01)
      {
        QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: PDN=%p IPV6 WWAN Connecting Failed...WAN CallendType=%d CallendCode=%d",
                            profile_handle,
                            ind_data.wwan_call_end_reason.wwan_call_end_reason_type,
                            ind_data.wwan_call_end_reason.wwan_call_end_reason_code);
        return;
      }

      break;
    }
  case QMI_QCMAP_MSGR_MOBILE_AP_STATUS_IND_V01:
    {
      qcmap_msgr_mobile_ap_status_ind_msg_v01 ind_data;

      qmi_error = qmi_client_message_decode(user_handle,
                                            QMI_IDL_INDICATION,
                                            msg_id,
                                            ind_buf,
                                            ind_buf_len,
                                            &ind_data,
                                            sizeof(qcmap_msgr_mobile_ap_status_ind_msg_v01));
      if (qmi_error != QMI_NO_ERR)
      {
        QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: qmi_client_message_decode error %d",qmi_error);
        break;
      }

      if (ind_data.mobile_ap_status == QCMAP_MSGR_MOBILE_AP_STATUS_CONNECTED_V01)
      {
        QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: Mobile AP Connected...");
        return;
      }
      else if (ind_data.mobile_ap_status == QCMAP_MSGR_MOBILE_AP_STATUS_DISCONNECTED_V01)
      {
        QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: Mobile AP Disconnected...");
        return;
      }
      break;
    }

  case QMI_QCMAP_MSGR_STATION_MODE_STATUS_IND_V01:
    {
      qcmap_msgr_station_mode_status_ind_msg_v01 ind_data;

      qmi_error = qmi_client_message_decode(user_handle,
                                            QMI_IDL_INDICATION,
                                            msg_id,
                                            ind_buf,
                                            ind_buf_len,
                                            &ind_data,
                                            sizeof(qcmap_msgr_station_mode_status_ind_msg_v01));
      if (qmi_error != QMI_NO_ERR)
      {
        QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: qmi_client_message_decode error %d",qmi_error);
        break;
      }

      if (ind_data.station_mode_status == QCMAP_MSGR_STATION_MODE_CONNECTED_V01)
      {
        QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: Mobile AP Station mode Connected...");
        return;
      }
      else if (ind_data.station_mode_status == QCMAP_MSGR_STATION_MODE_DISCONNECTED_V01)
      {
        QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: Mobile AP Station mode Disconnected...");
        return;
      }
      else if (ind_data.station_mode_status == QCMAP_MSGR_STATION_MODE_ASSOCIATION_FAIL_V01)
      {
        QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: Mobile AP Station mode Association Failed. Going back to AP+STA Router Mode");
        return;
      }
      else if (ind_data.station_mode_status == QCMAP_MSGR_STATION_MODE_DHCP_IP_ASSIGNMENT_FAIL_V01)
      {
        QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: Mobile AP Station mode IP Assignment via DHCP Failed. Will switch to Static IP if available");
        return;
      }
      break;
    }

  case QMI_QCMAP_MSGR_CRADLE_MODE_STATUS_IND_V01:
    {
      qcmap_msgr_cradle_mode_status_ind_msg_v01 ind_data;

      qmi_error = qmi_client_message_decode(user_handle,
                           QMI_IDL_INDICATION,
                           msg_id,
                           ind_buf,
                           ind_buf_len,
                           &ind_data,
                           sizeof(qcmap_msgr_cradle_mode_status_ind_msg_v01));
      if (qmi_error != QMI_NO_ERR)
      {
        QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: "
                      "qmi_client_message_decode error %d",qmi_error);
        break;
      }

      if (ind_data.cradle_status == QCMAP_MSGR_CRADLE_CONNECTED_V01)
      {
        QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: "
                      "Mobile AP Cradle mode Connected...");
        return;
      }
      else if (ind_data.cradle_status == QCMAP_MSGR_CRADLE_DISCONNECTED_V01)
      {
        QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: "
                      "Mobile AP Cradle mode Disconnected...");
        return;
      }
      break;
    }

  case QMI_QCMAP_MSGR_WLAN_STATUS_IND_V01:
  {
     qcmap_msgr_wlan_status_ind_msg_v01 ind_data;
     int i=0;
     in_addr ip4_addr;
     char ip6_addr[INET6_ADDRSTRLEN];

     qmi_error = qmi_client_message_decode(user_handle,
                                           QMI_IDL_INDICATION,
                                           msg_id,
                                           ind_buf,
                                           ind_buf_len,
                                           &ind_data,
                                           sizeof(qcmap_msgr_wlan_status_ind_msg_v01));
      if (qmi_error != QMI_NO_ERR)
      {
        printf("qcmap_msgr_qmi_qcmap_ind: qmi_client_message_decode error %d",qmi_error);
        break;
      }

      if (ind_data.scm_ind_valid)
      {
        switch (ind_data.scm_ind)
        {
        case QCMAP_MSGR_SCM_DYNAMIC_RECONFIG_IND_V01:
          printf("QCMAP_MSGR_SCM_DYNAMIC_RECONFIG_IND_V01 SCM indication received\n");
          break;
        case QCMAP_MSGR_SCM_STATION_STATE_IND_V01:
          printf("QCMAP_MSGR_SCM_STATION_STATE_IND_V01 SCM indication received\n");
          break;
        case QCMAP_MSGR_SCM_SYS_CONTROL_IND_V01:
          printf("QCMAP_MSGR_SCM_SYS_CONTROL_IND_V01 SCM indication received\n");
          break;
        default:
          break;
        }
      }
      else
      {
        if (ind_data.wlan_status == QCMAP_MSGR_WLAN_ENABLED_V01)
        {
         printf("\n qcmap_msgr_qmi_qcmap_ind: WLAN is ENABLED...\n");
        }
        else if (ind_data.wlan_status == QCMAP_MSGR_WLAN_DISABLED_V01)
        {
         printf("\n qcmap_msgr_qmi_qcmap_ind: WLAN is DISABLED...\n");
        }
        else
        {
         printf("\n Invalid wlan status %d \n",ind_data.wlan_status);
        }

        if (ind_data.wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_V01)
        {
         printf("qcmap_msgr_qmi_qcmap_ind: WLAN is in AP Mode...\n");
        }
        else if (ind_data.wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_V01)
        {
         printf("qcmap_msgr_qmi_qcmap_ind: WLAN is AP+AP Mode...\n");
        }
        else if (ind_data.wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01)
        {
         printf("qcmap_msgr_qmi_qcmap_ind: WLAN is AP+AP+AP Mode...\n");
        }
        else if (ind_data.wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01)
        {
         printf("qcmap_msgr_qmi_qcmap_ind: WLAN is AP+STA Mode...\n");
         printf("STA is in %s mode...\n", (ind_data.bridge_mode ? "Bridge" : "Router"));
        }
        else if (ind_data.wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01)
        {
         printf("qcmap_msgr_qmi_qcmap_ind: WLAN is AP+AP+STA Mode...\n");
         printf("STA is in %s mode...\n", (ind_data.bridge_mode ? "Bridge" : "Router"));
        }
        else if (ind_data.wlan_mode == QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01)
        {
         printf("qcmap_msgr_qmi_qcmap_ind: WLAN is STA Mode...\n");
         printf("STA is in %s mode...\n", (ind_data.bridge_mode ? "Bridge" : "Router"));
        }
        else if (ind_data.wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01)
        {
         printf("qcmap_msgr_qmi_qcmap_ind: WLAN is AP+AP+AP+AP Mode...\n");
        }
        else
        {
         printf(" Invalid wlan mode %d ...\n",ind_data.wlan_mode);
        }

        for (i=0; i<ind_data.wlan_state_len; i++)
        {
         printf("\n WLAN State for Iface %s \n", ind_data.wlan_state[i].wlan_iface_name);
         printf("IP type %d \n",ind_data.wlan_state[i].ip_type);
         printf("Iface type %d \n",ind_data.wlan_state[i].wlan_iface_type);

         if (ind_data.wlan_state[i].wlan_iface_state == QCMAP_MSGR_WLAN_CONNECTED_V01)
         {
           printf("qcmap_msgr_qmi_qcmap_ind: Iface state is connected...\n");
         }
         else if (ind_data.wlan_state[i].wlan_iface_state == QCMAP_MSGR_WLAN_DISCONNECTED_V01)
         {
           printf("qcmap_msgr_qmi_qcmap_ind: Iface state is disconnected...\n");
         }
         else if (ind_data.wlan_state[i].wlan_iface_state == QCMAP_MSGR_WLAN_CONNECTING_V01)
         {
           printf("qcmap_msgr_qmi_qcmap_ind: Iface state is connecting...\n");
         }
         else
         {
           printf("qcmap_msgr_qmi_qcmap_ind: Incorrect Iface state %d \n",ind_data.wlan_state[i].wlan_iface_state);
         }

         ip4_addr.s_addr = ind_data.wlan_state[i].ip4_addr;
         printf("IP4 address of the iface: %s\n",inet_ntoa(ip4_addr));

         inet_ntop(AF_INET6,(void*)&ind_data.wlan_state[i].ip6_addr, ip6_addr, sizeof(ip6_addr));
         printf("IPv6 Address of the iface: %s\n",ip6_addr);
        }
      }
      break;
   }
   case QMI_QCMAP_MSGR_ETHERNET_MODE_STATUS_IND_V01:
    {
      qcmap_msgr_ethernet_mode_status_ind_msg_v01 ind_data;

      qmi_error = qmi_client_message_decode(user_handle,
                           QMI_IDL_INDICATION,
                           msg_id,
                           ind_buf,
                           ind_buf_len,
                           &ind_data,
                           sizeof(qcmap_msgr_ethernet_mode_status_ind_msg_v01));
      if (qmi_error != QMI_NO_ERR)
      {
        QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: "
                      "qmi_client_message_decode error %d",qmi_error);
        break;
      }

      if (ind_data.eth_status == QCMAP_MSGR_ETH_BACKHAUL_CONNECTED_V01)
      {
        QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: "
                      "Mobile AP ETHERNET Backhaul Connected...");
        return;
      }
      else if (ind_data.eth_status == QCMAP_MSGR_ETH_BACKHAUL_DISCONNECTED_V01)
      {
        QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: "
                      "Mobile AP ETHERNET Backhaul Disconnected...");
        return;
      }
      break;
    }
   case QMI_QCMAP_MSGR_BACKHAUL_STATUS_IND_V01:
     {
       qcmap_msgr_backhaul_status_ind_msg_v01 ind_data;

       qmi_error = qmi_client_message_decode(user_handle,
                           QMI_IDL_INDICATION,
                           msg_id,
                           ind_buf,
                           ind_buf_len,
                           &ind_data,
                           sizeof(qcmap_msgr_backhaul_status_ind_msg_v01));
       if (qmi_error != QMI_NO_ERR)
       {
         QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: "
                      "qmi_client_message_decode error %d",qmi_error);
         break;
       }
       if (ind_data.backhaul_type_valid == TRUE)
       {
         if (convert_backhaul_enum_to_string(ind_data.backhaul_type, buffer))
         {
           if (ind_data.backhaul_v4_status_valid == TRUE)
           {
             snprintf(command,MAX_COMMAND_STR_LEN,"IPV4 %s Backhaul %s...",buffer,(ind_data.backhaul_v4_status)?"connected":"disconneted");
           }
           if (ind_data.backhaul_v6_status_valid == TRUE)
           {
             snprintf(command,MAX_COMMAND_STR_LEN,"IPV6 %s Backhaul %s...",buffer,(ind_data.backhaul_v6_status)?"connected":"disconneted");
           }
           QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: %s",command);
         }
       }
       break;
     }
   case QMI_QCMAP_MSGR_WWAN_ROAMING_STATUS_IND_V01:
    {
      qcmap_msgr_wwan_roaming_status_ind_msg_v01 ind_data;

      qmi_error = qmi_client_message_decode(user_handle,
                           QMI_IDL_INDICATION,
                           msg_id,
                           ind_buf,
                           ind_buf_len,
                           &ind_data,
                           sizeof(qcmap_msgr_wwan_roaming_status_ind_msg_v01));
      if (qmi_error != QMI_NO_ERR)
      {
        QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind: "
                      "qmi_client_message_decode error %d",qmi_error);
        break;
      }

      QCMAP_CLI_LOG("qcmap_msgr_qmi_qcmap_ind Roaming status changed to %d",
                     ind_data.wwan_roaming_status);
      printf("qcmap_msgr_qmi_qcmap_ind Roaming status changed to %d",ind_data.wwan_roaming_status);
      return;
    }

   default:
     break;
  }

  return;
}


/*===========================================================================
  FUNCTION sighandler
  ===========================================================================
  @brief
  Signal Handler
  @input
  signal- signal number
  @return
  void
  @dependencies
  Under lying os to generate the signal
  @sideefects
  None
  =========================================================================*/
void sighandler(int signal)
{
  qmi_error_type_v01 qmi_err_num;
  switch (signal)
  {
    case SIGTERM:
      if (QcMapClient)
        QcMapClient->DisableMobileAP(&qmi_err_num);

      exit(0);
      break;

    default:
      printf("Received unexpected signal %s\n", signal);
      break;
  }
}
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
int16
check_port (uint32 sport)
{
  if((sport > MAX_PORT_VALUE) || (sport < 1) )
  {
    printf(" port value should be between 1 - %d\n",MAX_PORT_VALUE);
    return -1;
  }
  else
    return 0;
}

/*===========================================================================
  FUNCTION  check_proto
===========================================================================
 @brief
   protocol value is validated against the range 1 - MAX_PROTO_VALUE
 @input
   sport - protocol value
 @return
   0  - success
   -1 - failure
 @dependencies
   None
 @sideefects
   None
=========================================================================*/
int16
check_proto (uint8 sport)
{
  if( sport > MAX_PROTO_VALUE )
  {
    printf(" port value should be between 1 - %d\n",MAX_PROTO_VALUE);
    return -1;
  }
  else
    return 0;
}

/*===========================================================================
  FUNCTION  check_tos
===========================================================================
 @brief
   Tos value is validated against the range 1 - MAX_TOS_VALUE
 @input
   tos - port value
 @return
   0  - success
   -1 - failure
 @dependencies
   None
 @sideefects
   None
=========================================================================*/
int16
check_tos (uint8 tos)
{
  if( tos > MAX_TOS_VALUE )
  {
    printf(" Tos value should be between 0 - %d\n",MAX_TOS_VALUE);
    return -1;
  }
  else
    return 0;
}

/*===========================================================================
  FUNCTION read_addr
  ===========================================================================
  @brief
  Read address funtion will read the address from the user
  @input
  domain - identifies ipv4 or ipv6 domain
  addr   - contains the numeric address, it's an output value.
  @return
  0  - success
  -1 - failure
  @dependencies
  It depends on inet_pton()
  @sideefects
  None
  =========================================================================*/
int read_addr(int domain,uint8 *addr)
{
  unsigned char buf[50]={0};
  char *ptr=NULL;

  bzero(buf,sizeof(buf));

  read_again:
  if ( domain == AF_INET )
  {
    printf("\n Please Enter V4 Address Value =>");
  }
  else
  if(domain == AF_INET6)
  {
     printf("\n Please Enter V6 Address Value =>");
  }
  else
  printf("\n Please Enter Address Mask Value =>");

  if ( fgets(buf, sizeof(buf), stdin) != NULL )
  {
    ptr = strchr((char *)&buf,'\n');

    if( ptr )
      *ptr = NULL;

    if ( inet_pton(domain,buf,addr) <=0 ) {
      printf("\n Address not in presentation format\n");
      goto read_again;
    }
  }
  return 0;
}

/*===========================================================================
  FUNCTION read_uint8
  ===========================================================================
  @brief
  Read uint8 value from user.
  @input
  void
  @return
  int value read from user.
  @dependencies
  usr to provide input
  @sideefects
  None
  =========================================================================*/

uint8 read_uint8()
{
  char  scan_string[50];
  uint8 result;
  uint32 src_addr[4]={0};
  bzero(scan_string,sizeof(scan_string));
  fgets(scan_string, sizeof(scan_string), stdin);
  result = atoi(scan_string);
  return result;
}

/*===========================================================================
  FUNCTION read_uint32
  ===========================================================================
  @brief
  Read integer value from user.
  @input
  void
  @return
  int value read from user.
  @dependencies
  usr to provide input
  @sideefects
  None
  =========================================================================*/
int read_uint32()
{
  char  scan_string[50];
  int result;
  bzero(scan_string,sizeof(scan_string));
  uint32 src_addr[4]={0};
  fgets(scan_string, sizeof(scan_string), stdin);
  result = atoi(scan_string);
  return result;
}

#define GET_MAC_ADDR(scan_string,mac_addr_int) {\
    memset(scan_string,0,QCMAP_MSGR_MAX_FILE_PATH_LEN);\
    while(TRUE)\
    {\
          printf("Please input the MAC address  :");\
          if ( fgets(scan_string,sizeof(scan_string),stdin) != NULL )\
          {\
              if ( ds_mac_addr_pton(scan_string,mac_addr_int))\
                break; \
          }\
          printf("Invalid Mac Address entered %s\n", scan_string);\
        }\
 }

#define GET_IP_ADDR(scan_string, ip_addr) {\
    memset(scan_string,0,QCMAP_MSGR_MAX_FILE_PATH_LEN);\
    while(TRUE)\
    {\
         printf("Please input the client reserved IP(xxx.xxx.xxx.xxx)  :");\
         if ( fgets(scan_string,sizeof(scan_string),stdin) != NULL )\
         {\
              if ( (inet_aton(scan_string, &ip_addr)) && \
                   ( ip_addr.s_addr != 0 ) &&\
                   ( ip_addr.s_addr != 0xffffffff ))\
               break; \
          }\
          printf("Invalid IPv4 Address entered: %s\n", scan_string);\
     }\
 }


/*===========================================================================
  FUNCTION read_firewall_conf
  ===========================================================================
  @brief
  Read firewall configuration value from user and calls add fireall entry.
  @input
  void
  @return
  true  - success
  false - failure
  @dependencies
  usr to provide input
  @sideefects
  None
  =========================================================================*/
boolean read_firewall_conf(qcmap_msgr_firewall_conf_t *extd_firewall_add)
{
  unsigned char buf[50];
  int val,icmp_proto_val = 0;
  memset(extd_firewall_add, 0, sizeof(qcmap_msgr_firewall_conf_t));

  while(1)
  {
    printf("\nPlease input Direction [1-UL/2-DL]:");
    val = read_uint32();
    if( val == QCMAP_MSGR_UL_FIREWALL || val == QCMAP_MSGR_DL_FIREWALL )
     break;
  }
  extd_firewall_add->extd_firewall_entry.firewall_direction = val;

  while(1)
  {
    printf("\nPlease input IP family [4-IPV4/6-IPV6]:");
    val = read_uint32();
    if( val == IP_V4 || val== IP_V6 )
    break;
  }
  extd_firewall_add->extd_firewall_entry.filter_spec.ip_vsn = val;

  if(extd_firewall_add->extd_firewall_entry.filter_spec.ip_vsn == IP_V4)
  {
    printf("\nDo you want to enter IPV4 source address\n");
    printf("  and subnet mask: [1-YES 0-NO]:");

    if(read_uint32() == 1)

    {
      printf("\nPlease enter Src Addr [xxx.xxx.xxx.xxx]:");
      read_addr(AF_INET,(char *)&extd_firewall_add->extd_firewall_entry.filter_spec.ip_hdr.v4.src.addr.ps_s_addr);
      extd_firewall_add->extd_firewall_entry.filter_spec.ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_SRC_ADDR;
      printf("\nPlease enter IPV4 src subnet mask [xxx.xxx.xxx.xxx]:");
      read_addr(AF_INET,(uint8* )&extd_firewall_add->extd_firewall_entry.filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr);
    }

    printf("\nDo you want to enter IPV4 TOS value\n");
    printf("and TOS mask: [1-YES 0-NO]:");
    if(read_uint32() == 1)
    {
      while(1)
      {
        printf("\nPlease enter the TOS value [0 to 255]:");
        val = read_uint32();
        if( check_tos(val)!= -1)
          break;
      }
      extd_firewall_add->extd_firewall_entry.filter_spec.ip_hdr.v4.tos.val =val;
      while(1)
      {
        printf("\nPlease enter the TOS mask [0 to 255]:");
        val = read_uint32();
        if( check_tos( val )!= -1)
          break;
      }
      extd_firewall_add->extd_firewall_entry.filter_spec.ip_hdr.v4.tos.mask = val;
      extd_firewall_add->extd_firewall_entry.filter_spec.ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_TOS;
    }
  }
  else
  {
    printf("\nDo you want to enter IPV6 source address\n"
           "and subnet mask: [1-YES 0-NO]:");
    if(read_uint32() == 1)
    {
      printf("\nPlease enter Src Addr [xxxx:xxxx::xxxx:xxxx:xxxx]:");
      read_addr(AF_INET6,(uint8 *)&extd_firewall_add->extd_firewall_entry.filter_spec.ip_hdr.v6.src.addr.in6_u.u6_addr8);
      extd_firewall_add->extd_firewall_entry.filter_spec.ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_SRC_ADDR;
      printf("\nPlease enter IPV6 src prefixlen:");
      extd_firewall_add->extd_firewall_entry.filter_spec.ip_hdr.v6.src.prefix_len = read_uint32();
    }

    printf("\nDo you want to enter IPV6 Traffic Class value\n"
           "and subnet mask: [1-YES 0-NO]:");
    if(read_uint32() == 1)
    {
      printf("\nPlease enter IPV6 Class value:");
      extd_firewall_add->extd_firewall_entry.filter_spec.ip_hdr.v6.trf_cls.val=read_uint32();
      extd_firewall_add->extd_firewall_entry.filter_spec.ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_TRAFFIC_CLASS;
      printf("\nPlease enter IPV6 class mask value:");
      extd_firewall_add->extd_firewall_entry.filter_spec.ip_hdr.v6.trf_cls.mask = read_uint32();
    }
  }

  printf("\nDo you want to enter Protocol [1-YES 0-NO]:");

  if(read_uint32() == 1)
  {
    read_proto_again:
    printf("\nPlease input IPV4 next header protocol\n");
    if( extd_firewall_add->extd_firewall_entry.filter_spec.ip_vsn == IP_V4 )
    {
      printf("TCP=%d, UDP=%d,ICMP=%d,TCP_UDP=%d,ESP=%d ",
                   PS_IPPROTO_TCP, PS_IPPROTO_UDP, PS_IPPROTO_ICMP,\
                   PS_IPPROTO_TCP_UDP, PS_IPPROTO_ESP);
      icmp_proto_val = PS_IPPROTO_ICMP;
    }
    else if (extd_firewall_add->extd_firewall_entry.filter_spec.ip_vsn == IP_V6)
    {
      printf("TCP=%d, UDP=%d,ICMP6=%d,TCP_UDP=%d,ESP=%d ",
                   PS_IPPROTO_TCP, PS_IPPROTO_UDP, PS_IPPROTO_ICMP6,\
                   PS_IPPROTO_TCP_UDP, PS_IPPROTO_ESP);
      icmp_proto_val = PS_IPPROTO_ICMP6;
    }
    val = read_uint32();
    if( val == PS_IPPROTO_TCP || val == PS_IPPROTO_UDP || val == icmp_proto_val\
        || val == PS_IPPROTO_ESP || val == PS_IPPROTO_TCP_UDP )
    {
      if( extd_firewall_add->extd_firewall_entry.filter_spec.ip_vsn == IP_V4 )
      {
        extd_firewall_add->extd_firewall_entry.filter_spec.ip_hdr.v4.next_hdr_prot = val;
        extd_firewall_add->extd_firewall_entry.filter_spec.ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_NEXT_HDR_PROT;
      }
      else if(extd_firewall_add->extd_firewall_entry.filter_spec.ip_vsn == IP_V6 )
      {
        extd_firewall_add->extd_firewall_entry.filter_spec.ip_hdr.v6.next_hdr_prot = val;
        extd_firewall_add->extd_firewall_entry.filter_spec.ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_NEXT_HDR_PROT;
      }
    }else
    {
      printf("Invalid selection. Please select protocol again.\n");
      goto read_proto_again;
    }
  }

  if(val == PS_IPPROTO_TCP)
  {
    printf("\nDo you want to enter Source Port and Range [1-YES 0-NO]:");
    if(read_uint32() == 1)
    {
      read_tcp_src_single_port:
      printf("\nPlease input TCP source port:");
      val = read_uint32();
      if( check_port (val) == -1)
        goto read_tcp_src_single_port;
      extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.tcp.src.port = val;
      extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.tcp.field_mask |= IPFLTR_MASK_TCP_SRC_PORT;

      read_tcp_start_port:
      printf("\nPlease enter TCP source port range:");
      val = read_uint32();
      if( check_port ((extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.tcp.src.port+val)) == -1 )
        goto read_tcp_start_port;
      extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.tcp.src.range = val;
    }

    printf("\nDo you want to enter Destination Port and Range [1-YES 0-NO]:");
    val = read_uint32();
    if(val ==  1)
    {
      read_tcp_dst_port:
      printf("\nPlease input TCP destination port:");
      val = read_uint32();
      if( check_port (val) == -1)
        goto read_tcp_dst_port;
      extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.tcp.dst.port = val;
      extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.tcp.field_mask |= IPFLTR_MASK_TCP_DST_PORT;

      read_tcp_dst_port_range:
      printf("\nPlease enter TCP destination port range:");
      val = read_uint32();
      if( check_port ((extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.tcp.dst.port + val)) == -1 )
        goto read_tcp_dst_port_range;
      extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.tcp.dst.range = val;
    }
  }else if( val == PS_IPPROTO_UDP)
  {
    printf("\nDo you want to enter Source Port and Range [1-YES 0-NO]:");
    val = read_uint32();
    if(val ==  1)
    {
      read_udp_src_single_port:
      printf("\nPlease input udp source port:");
      val = read_uint32();
      if( check_port (val) == -1)
        goto read_udp_src_single_port;
      extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.udp.src.port = val;
      extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.udp.field_mask |= IPFLTR_MASK_UDP_SRC_PORT;

      read_udp_start_port:
      printf("\nPlease enter udp source port range:");
      val = read_uint32();
      if( check_port ((extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.udp.src.port+val)) == -1 )
        goto read_udp_start_port;
      extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.udp.src.range = val;
    }
    printf("\nDo you want to enter Destination Port and Range [1-YES 0-NO]:");
    val = read_uint32();
    if( val == 1)
    {
      read_udp_dst_port:
      printf("\nPlease input udp destination port:");
      val = read_uint32();
      if( check_port (val) == -1)
        goto read_udp_dst_port;
      extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.udp.dst.port = val;
      extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.udp.field_mask |= IPFLTR_MASK_UDP_DST_PORT;

      read_udp_dst_end_port:
      printf("\nPlease enter udp destination port range:");
      val = read_uint32();
      if( check_port ((extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.udp.dst.port+val)) == -1 )
        goto read_udp_dst_end_port;
      extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.udp.dst.range = val;
    }
  }
  else if (val == PS_IPPROTO_TCP_UDP)
  {
    printf("\nDo you want to enter Source Port and Range [1-YES 0-NO]:");
    if(read_uint32() == 1)
    {
     read_tcp_udp_port_range_src_single_port:
     printf("\nPlease input tcp_udp source port:");
     val = read_uint32();
     if( check_port (val) == -1)
     goto read_tcp_udp_port_range_src_single_port;
     extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.tcp_udp_port_range.src.port = val;
     extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.tcp_udp_port_range.field_mask |= IPFLTR_MASK_TCP_UDP_SRC_PORT;

    read_tcp_udp_port_range_start_port:
     printf("\nPlease enter tcp_udp source port range:");
     val = read_uint32();
     if( check_port ((extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.tcp_udp_port_range.src.port+val)) == -1 )
     goto read_tcp_udp_port_range_start_port;
     extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.tcp_udp_port_range.src.range = val;
    }
    printf("\nDo you want to enter Destination Port and Range [1-YES 0-NO]:");
    if(read_uint32() == 1)
    {
      read_tcp_udp_port_range_dst_port:
      printf("\nPlease input tcp_udp destination port:");
      val = read_uint32();
      if( check_port (val) == -1)
        goto read_tcp_udp_port_range_dst_port;
      extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.tcp_udp_port_range.dst.port = val;
      extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.tcp_udp_port_range.field_mask |= IPFLTR_MASK_TCP_UDP_DST_PORT;
      read_tcp_udp_port_range_dst_end_port:
      printf("\nPlease enter tcp_udp destination port range:");
      val = read_uint32();
      if( check_port ((extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.tcp_udp_port_range.dst.port + val)) == -1 )
        goto read_tcp_udp_port_range_dst_end_port;
      extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.tcp_udp_port_range.dst.range = val;
    }
  }else if ( val == PS_IPPROTO_ICMP || val == PS_IPPROTO_ICMP6)
  {
    printf("\nDo you want to enter ICMP Type [1-YES 0-NO]:");
    if(read_uint32() == 1)
    {
      printf("\nPlease enter the ICMP Type value:");
      extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.icmp.type = read_uint32();
      extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.icmp.field_mask |= IPFLTR_MASK_ICMP_MSG_TYPE;
    }
    printf("\nDo you want to enter ICMP code [1-YES 0-NO]:");
    if(read_uint32() == 1)
    {
      printf("\nPlease enter ICMP code value:");
      extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.icmp.code = read_uint32();
      extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.icmp.field_mask |= IPFLTR_MASK_ICMP_MSG_CODE;
    }
  }else if( val == PS_IPPROTO_ESP)
  {
    printf("\nDo you want to enter ESP SPI value [1-YES 0-NO]:");
    if(read_uint32() == 1)
    {
      printf("\nPlease enter the ESP SPI value:");
      extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.esp.spi = read_uint32();
      extd_firewall_add->extd_firewall_entry.filter_spec.next_prot_hdr.esp.field_mask |= IPFLTR_MASK_ESP_SPI;
    }
  }
  return true;
}


/*===========================================================================
  FUNCTION readable_addr
  ===========================================================================
  @brief
    convert a numeric address into a text string suitable
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
int readable_addr(int domain,uint32_t *addr, char *str)
{
  if (inet_ntop(domain, addr, str, INET6_ADDRSTRLEN) == NULL)
  {
    printf("\n Not in presentation format \n");
    return -1;
  }

  return 0;
}


/*===========================================================================
  FUNCTION DisplayIPv6State
  ===========================================================================
  @brief
    Prints the state of IPv6 (enabled or disabled)

  @input
    void

  @return
    void

  @dependencies
    none

  @sideefects
    None
  =========================================================================*/
void DisplayIPv6State()
{
  boolean ipv6_state;
  qmi_error_type_v01  qmi_err_num;

  memset((void *)&ipv6_state, 0, sizeof(uint8_t));

  if (QcMapClient->GetIPv6State( &ipv6_state, &qmi_err_num))
  {
    printf("\nIPV6 is: %s.\n",(ipv6_state)?"Enabled":"Disabled");
  }
  else
  {
    printf("\nGetIPV6State returns Error: 0x%x", qmi_err_num);
  }
}


/*===========================================================================
  FUNCTION DisplayWWANPolicy
  ===========================================================================
  @brief
    Displays the current WWAN network policy

  @input
    void

  @return
    void

  @dependencies
    usr to provide input

  @sideefects
    None
  =========================================================================*/
void DisplayWWANPolicy()
{
  qcmap_msgr_net_policy_info_v01 wwan_policy;
  memset(&wwan_policy,0,sizeof(qcmap_msgr_net_policy_info_v01));
  qmi_error_type_v01      qmi_err_num;

  if ( QcMapClient->GetWWANPolicy(&wwan_policy, &qmi_err_num))
  {
    printf("\nTech preference is :");
    switch (wwan_policy.tech_pref)
    {
      case 0:
        printf("ANY \n");
        break;

      case 1:
        printf("UMTS \n");
        break;

      case 2:
        printf("CDMA \n");
        break;

      default:
        printf("Error: invalid tech preference:- %d\n",wwan_policy.tech_pref);
        break;
    }
    printf("\nV4 3gpp profile id is: %d.\n",
            wwan_policy.v4_profile_id_3gpp);
    printf("\nV4 3gpp2 profile id is: %d.\n",
            wwan_policy.v4_profile_id_3gpp2);
    printf("\nV6 3gpp pofile id is: %d.\n",
            wwan_policy.v6_profile_id_3gpp);
    printf("\nV6 3gpp2 pofile id is: %d.\n",
            wwan_policy.v6_profile_id_3gpp2);
  }
  else
  {
    printf("\nGet WWAN policy failed Error 0x%x.\n", qmi_err_num);
  }
}


/*===========================================================================
  FUNCTION DisplayFirewall
  ===========================================================================
  @brief
    Displays the firewall configuration rules for Mobile AP

  @input
    handle_list_length - handle list length
    extd_firewall_handle_list -  firewall configuration list

  @return
    void

  @dependencies
    usr to provide input

  @sideefects
    None
  =========================================================================*/
boolean DisplayFirewall(int handle_list_len,
                        qcmap_msgr_firewall_conf_t extd_firewall_handle_list)
{
  char str[INET6_ADDRSTRLEN];
  qcmap_msgr_firewall_conf_t extd_firewall_get;
  int   index;
  int   result;
  int next_hdr_prot = 0;
  qcmap_msgr_firewall_entry_conf_t *firewall_entry;
  qmi_error_type_v01  qmi_err_num;

  if(handle_list_len > 0)
  {
    for(index =0; index < handle_list_len; index++)
    {
      memset(&extd_firewall_get, 0, sizeof(qcmap_msgr_firewall_conf_t));
      extd_firewall_get.extd_firewall_entry.filter_spec.ip_vsn =
        extd_firewall_handle_list.extd_firewall_handle_list.ip_family;
      extd_firewall_get.extd_firewall_entry.firewall_handle =
        extd_firewall_handle_list.extd_firewall_handle_list.handle_list[index];

      if(QcMapClient->GetFireWallEntry(&extd_firewall_get.extd_firewall_entry, &qmi_err_num))
      {
        firewall_entry=&extd_firewall_get.extd_firewall_entry;
        printf("\n### Start Displaying firewall configuration of handle =%d ###",
               extd_firewall_get.extd_firewall_entry.firewall_handle);
        if(firewall_entry!=NULL)
        {
          if (firewall_entry->firewall_direction == QCMAP_MSGR_UL_FIREWALL)
            printf("\nUL Firewall Rule");
          else
            printf("\nDL Firewall Rule");
          if( firewall_entry->filter_spec.ip_vsn == IP_V4 )
          {
            printf("\nIp version : IPv4");
            if( firewall_entry->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_SRC_ADDR)
            {
              readable_addr(AF_INET,&firewall_entry->filter_spec.ip_hdr.v4.src.addr.ps_s_addr,(char *)&str);
              printf("\nSRC Addr : %s",str);
              readable_addr(AF_INET,&firewall_entry->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr,(char *)&str);
              printf("\nSRC Addr Mask : %s",str);
            }
            else
              printf("\nSRC Addr : Any");

            if(  firewall_entry->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_TOS)
            {
              printf("\nTos value : %x ",firewall_entry->filter_spec.ip_hdr.v4.tos.val);
              printf("\nTos Mask : %x ",firewall_entry->filter_spec.ip_hdr.v4.tos.mask);
            }
            else
              printf("\nTos value : Any");

            if( firewall_entry->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_NEXT_HDR_PROT )
            {
              next_hdr_prot = firewall_entry->filter_spec.ip_hdr.v4.next_hdr_prot;
            }
          }
          else
          {
            printf("\nIp version : Ipv6");
            if( firewall_entry->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_SRC_ADDR)
            {
              readable_addr(AF_INET6,(uint32 *)&firewall_entry->filter_spec.ip_hdr.v6.src.addr.in6_u.u6_addr32,(char *)&str);
              printf("\nSrc Addr : %s ",str);
              printf("\nSrc Prefixlen : %d ",firewall_entry->filter_spec.ip_hdr.v6.src.prefix_len);
            }
            else
              printf("\nSRC Addr : Any");

            if( extd_firewall_get.extd_firewall_entry.filter_spec.ip_hdr.v6.field_mask &
                IPFLTR_MASK_IP6_TRAFFIC_CLASS )
            {
              printf("\n IPV6 Traffic class value: %d",
                  extd_firewall_get.extd_firewall_entry.filter_spec.ip_hdr.v6.trf_cls.val);
              printf("\n IPV6 Traffic class mask: %d",
                  extd_firewall_get.extd_firewall_entry.filter_spec.ip_hdr.v6.trf_cls.mask);
            }

            if( firewall_entry->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_NEXT_HDR_PROT )
            {
              next_hdr_prot = firewall_entry->filter_spec.ip_hdr.v6.next_hdr_prot;
            }
          }
          switch(next_hdr_prot)
          {
            case PS_IPPROTO_TCP:
              printf("\nProtocol : TCP");
              if( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_SRC_PORT )
              {
                printf("\nSrc port : %d",firewall_entry->filter_spec.next_prot_hdr.tcp.src.port);
                printf("\nSrc portrange : %d",firewall_entry->filter_spec.next_prot_hdr.tcp.src.range);
              }
              if( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT )
              {
                printf("\nDst port : %d",firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port);
                printf("\nDst portrange : %d",firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range);
              }
              break;
            case PS_IPPROTO_UDP:
              printf("\nProtocol: UDP");
              if( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_SRC_PORT )
              {
                printf("\nSrc port : %d",firewall_entry->filter_spec.next_prot_hdr.udp.src.port);
                printf("\nDst portrange : %d",firewall_entry->filter_spec.next_prot_hdr.udp.src.range);
              }
              if( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_DST_PORT )
              {
                printf("\nDst port : %d",firewall_entry->filter_spec.next_prot_hdr.udp.dst.port);
                printf("\nDst portrange : %d",firewall_entry->filter_spec.next_prot_hdr.udp.dst.range);
              }
              break;
            case PS_IPPROTO_TCP_UDP:
              printf("\nProtocol: TCP_UDP");
              if( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask & IPFLTR_MASK_TCP_UDP_SRC_PORT )
              {
                printf("\nSrc port : %d",firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.src.port);
                printf("\nSrc portrange : %d",firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.src.range);

              }
              if( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask & IPFLTR_MASK_TCP_UDP_DST_PORT )
              {
                printf("\nDst port : %d",firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.port);
                printf("\nDst portrange : %d",firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.range);
              }
              break;
            case PS_IPPROTO_ICMP:
              printf("\nProtocol : ICMP");
              if( firewall_entry->filter_spec.next_prot_hdr.icmp.field_mask & IPFLTR_MASK_ICMP_MSG_TYPE )
              {
                printf("\nIcmp Type: %d ",firewall_entry->filter_spec.next_prot_hdr.icmp.type);
              }
              if( firewall_entry->filter_spec.next_prot_hdr.icmp.field_mask & IPFLTR_MASK_ICMP_MSG_CODE )
              {
                printf("\nIcmp Code: %d ",firewall_entry->filter_spec.next_prot_hdr.icmp.code);
              }
              break;
            case PS_IPPROTO_ICMP6:
              printf("\nProtocol : ICMP6");
              if( firewall_entry->filter_spec.next_prot_hdr.icmp.field_mask & IPFLTR_MASK_ICMP_MSG_TYPE )
              {
                printf("\nICMPv6 Type: %d ",firewall_entry->filter_spec.next_prot_hdr.icmp.type);
              }
              if( firewall_entry->filter_spec.next_prot_hdr.icmp.field_mask & IPFLTR_MASK_ICMP_MSG_CODE )
              {
                printf("\nICMPv6 Code: %d ",firewall_entry->filter_spec.next_prot_hdr.icmp.code);
              }
              break;
            case PS_IPPROTO_ESP:
              printf("\nProtocol : ESP");
              if( firewall_entry->filter_spec.next_prot_hdr.esp.field_mask & IPFLTR_MASK_ESP_SPI )
              {
                printf("\nESP spi : %d",firewall_entry->filter_spec.next_prot_hdr.esp.spi);

              }
              break;
            default:
              printf("\nProtocol : Any");
              break;
          }
          printf("\n### End of Firewall configuration of handle =%d ###",
               extd_firewall_get.extd_firewall_entry.firewall_handle);
        }
      }
      else
      {
        printf("\nFirewall entry get failed, Error: 0x%x", qmi_err_num);
        break;
      }
     }
   }
   else
   {
      printf("\nNo Firewall Rules to Display \n" );
   }

   return true;
}


/*===========================================================================
  FUNCTION DisplayConfig
  ===========================================================================
  @brief
    Displays the current configuration of MobileAP

  @input
    void

  @return
    void

  @dependencies
    usr to provide input

@sideefects
    None
  =========================================================================*/
void DisplayConfig()
{
  qcmap_msgr_firewall_conf_t extd_firewall_get[50];
  qcmap_msgr_firewall_conf_t extd_firewall_handle_list;
  qcmap_msgr_firewall_entry_conf_t *firewall_entry;
  qcmap_msgr_nat_enum_v01 nat_type;
  qcmap_msgr_wlan_mode_enum_v01 wlan_mode;
  qcmap_msgr_lan_config_v01 lan_config;
  qcmap_msgr_station_mode_config_v01 station_config;
  qcmap_msgr_bootup_flag_v01 mobileap_enable, wlan_enable;
  int   handle_list_len;
  int   index;
  int   result;
  in_addr tmpIP;
  int status;
  boolean roaming,pptp,ltp,autocon,webserver_wwan_access;
  uint32 dmz_ip=0;
  boolean vpn;
  int p_error=0;
  int i=0, num_entries=0;
  qcmap_nw_params_t qcmap_nw_params;
  char ipv6_addr_buf[INET6_ADDRSTRLEN];
  char mac_addr_str[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01]; /*char array of mac address*/
  qcmap_msgr_snat_entry_config_v01 snat_config[QCMAP_MSGR_MAX_SNAT_ENTRIES_V01];
  char str[INET6_ADDRSTRLEN];
  in_addr addr;
  in_addr start,gateway,subnet_mask;
  uint32 leasetime=0;
  qmi_error_type_v01  qmi_err_num;
  qcmap_msgr_access_profile_v01 guest_ap_profile;
  uint32_t num_dhcp_entries=0;
  qcmap_msgr_dhcp_reservation_v01 dhcp_reserv_record[QCMAP_MSGR_MAX_DHCP_RESERVATION_ENTRIES_V01];
  boolean pd_mode;

  memset(snat_config, 0, QCMAP_MSGR_MAX_SNAT_ENTRIES_V01*sizeof(qcmap_msgr_snat_entry_config_v01));
  QcMapClient->GetStaticNatConfig(snat_config, &num_entries, &qmi_err_num);
  QcMapClient->GetRoaming(&roaming, &qmi_err_num);
  QcMapClient->GetDMZ(&dmz_ip, &qmi_err_num);
  QcMapClient->GetIPSECVpnPassthrough(&vpn, &qmi_err_num);
  QcMapClient->GetPPTPVpnPassthrough(&pptp, &qmi_err_num);
  QcMapClient->GetL2TPVpnPassthrough(&ltp, &qmi_err_num);
  QcMapClient->GetAutoconnect(&autocon, &qmi_err_num);
  QcMapClient->GetWebserverWWANAccess(&webserver_wwan_access, &qmi_err_num);
 // QcMapClient->GetUSBConfig(&gateway.s_addr,&start.s_addr,&subnet_mask.s_addr,&leasetime);
  memset(&extd_firewall_handle_list, 0, sizeof(qcmap_msgr_firewall_conf_t));
  handle_list_len=0;
  extd_firewall_handle_list.extd_firewall_handle_list.ip_family = IP_V4;
  if(QcMapClient->GetFireWallHandlesList(&extd_firewall_handle_list.extd_firewall_handle_list,&qmi_err_num))
  {
    handle_list_len = extd_firewall_handle_list.extd_firewall_handle_list.num_of_entries;
  }
  if(handle_list_len > 0)
  {
    for(index =0; index < handle_list_len; index++)
    {
      memset(&extd_firewall_get[index], 0, sizeof(qcmap_msgr_firewall_conf_t));
      extd_firewall_get[index].extd_firewall_entry.filter_spec.ip_vsn =
        extd_firewall_handle_list.extd_firewall_handle_list.ip_family;
      extd_firewall_get[index].extd_firewall_entry.firewall_handle =
        extd_firewall_handle_list.extd_firewall_handle_list.handle_list[index];
      QcMapClient->GetFireWallEntry(&extd_firewall_get[index].extd_firewall_entry, &qmi_err_num);
    }
  }

  memset(&qcmap_nw_params,0,sizeof(qcmap_nw_params_t));

  QcMapClient->GetNatType(&nat_type, &qmi_err_num);
  uint32 timeout_value[5];
  uint32 timeout_type;
  p_error=0;
  timeout_type = QCMAP_MSGR_NAT_TIMEOUT_GENERIC_V01;
  while(timeout_type <= QCMAP_MSGR_NAT_TIMEOUT_UDP_V01)
  {
    QcMapClient->GetNatTimeout(timeout_type,(uint32 *)&timeout_value[timeout_type], &qmi_err_num);
    timeout_type++;
  }
  memset(&lan_config,0,sizeof(qcmap_msgr_lan_config_v01));
  memset(&station_config,0,sizeof(qcmap_msgr_station_mode_config_v01));
  QcMapClient->GetLANConfig(&lan_config,&qmi_err_num);
  QcMapClient->GetWLANConfig(&wlan_mode,&guest_ap_profile,&station_config,&qmi_err_num);
  printf("\nSNAT configuration:\n");
  for (i=0; i<num_entries; i++)
  {
    printf("Entry %d:\n",i);
    tmpIP.s_addr = ntohl(snat_config[i].private_ip_addr);
    printf("private ip: %s\n", inet_ntoa(tmpIP));
    printf("private port: %d\n", snat_config[i].private_port);
    printf("global port: %d\n", snat_config[i].global_port);
    printf("protocol: %d\n", snat_config[i].protocol);
  }
  printf("\nRoaming Mode: %s.\n",(roaming)?"Enabled":"Disabled");
  printf("\nDMZ IP:");
  if ( dmz_ip == 0 )
    printf(" No DMZ Configured!");
  else
  {
    tmpIP.s_addr = ntohl(dmz_ip);
    printf(" %s",inet_ntoa(tmpIP));
  }
  printf("\nIPSEC Passthrough Enable Flag : %d",vpn);
  printf("\nPPTP Passthrough Enable Flag : %d",pptp);
  printf("\nL2TP Passthrough Enable Flag : %d", ltp);
  printf("\nWebserver WWAN Access Flag: %d", webserver_wwan_access);
  printf("\nAuto Connect Mode: %s.\n",(autocon)?"Enabled":"Disabled");
  printf("\nFirewall Entries for Ipv4:");
  DisplayFirewall( handle_list_len, extd_firewall_handle_list );
  memset(&extd_firewall_handle_list, 0, sizeof(qcmap_msgr_firewall_conf_t));
  handle_list_len=0;
  extd_firewall_handle_list.extd_firewall_handle_list.ip_family = IP_V6;
  if(QcMapClient->GetFireWallHandlesList(&extd_firewall_handle_list.extd_firewall_handle_list,&qmi_err_num))
  {
    handle_list_len = extd_firewall_handle_list.extd_firewall_handle_list.num_of_entries;
  }
  if(handle_list_len > 0)
  {
    for(index =0; index < handle_list_len; index++)
    {
      memset(&extd_firewall_get[index], 0, sizeof(qcmap_msgr_firewall_conf_t));
      extd_firewall_get[index].extd_firewall_entry.filter_spec.ip_vsn =
        extd_firewall_handle_list.extd_firewall_handle_list.ip_family;
      extd_firewall_get[index].extd_firewall_entry.firewall_handle =
        extd_firewall_handle_list.extd_firewall_handle_list.handle_list[index];
      QcMapClient->GetFireWallEntry(&extd_firewall_get[index].extd_firewall_entry, &qmi_err_num);
    }
  }
  printf("\nFirewall Entries for Ipv6:");
  DisplayFirewall( handle_list_len, extd_firewall_handle_list );

  DisplayWWANPolicy();
  DisplayIPv6State();

  if (QcMapClient->GetNetworkConfiguration(
                                           QCMAP_MSGR_IP_FAMILY_V4_V01,
                                           &qcmap_nw_params,
                                           &qmi_err_num))
  {
    if(qmi_err_num == QMI_ERR_NONE_V01)
    {
      printf("\nIPv4 WWAN NetworkConfiguration:\n");
      qcmap_nw_params.v4_conf.public_ip.s_addr =
                        htonl(qcmap_nw_params.v4_conf.public_ip.s_addr);
      qcmap_nw_params.v4_conf.primary_dns.s_addr =
                        htonl(qcmap_nw_params.v4_conf.primary_dns.s_addr);
      qcmap_nw_params.v4_conf.secondary_dns.s_addr =
                        htonl(qcmap_nw_params.v4_conf.secondary_dns.s_addr);

      printf("Public IP for WWAN: %s \n",
             inet_ntoa(qcmap_nw_params.v4_conf.public_ip));
      printf("Primary DNS IP address: %s \n",
             inet_ntoa(qcmap_nw_params.v4_conf.primary_dns));
      printf("Secondary DNS IP address: %s \n",
             inet_ntoa(qcmap_nw_params.v4_conf.secondary_dns));
    }
    else
      printf("\nError in IPv4 config - 0x%x\n\n", qmi_err_num);
  }
  else
    printf("\n GetNetworkConfiguration fails\n");

  if (QcMapClient->GetNetworkConfiguration(
                                           QCMAP_MSGR_IP_FAMILY_V6_V01,
                                           &qcmap_nw_params,
                                           &qmi_err_num ))
  {
    if(qmi_err_num == QMI_ERR_NONE_V01)
    {
      /* Print this if user specified IPv6 or IPv4v6 */
      printf("\nIPv6 WWAN NetworkConfiguration:\n");
      if( inet_ntop(AF_INET6,&qcmap_nw_params.v6_conf.public_ip_v6,
          ipv6_addr_buf,sizeof(ipv6_addr_buf)))
      {
        printf("Public IP for WWAN: %s \n",ipv6_addr_buf);
      }
      if( inet_ntop(AF_INET6,&qcmap_nw_params.v6_conf.primary_dns_v6,
          ipv6_addr_buf,sizeof(ipv6_addr_buf)))
      {
        printf("Primary DNS IP address: %s \n",ipv6_addr_buf);
      }
      if( inet_ntop(AF_INET6,&qcmap_nw_params.v6_conf.secondary_dns_v6,
          ipv6_addr_buf,sizeof(ipv6_addr_buf)))
      {
        printf("Secondary DNS IP address: %s \n\n",ipv6_addr_buf);
      }
    }
    else
      printf("\nError in IPv6 config - 0x%x\n\n", qmi_err_num);
  }
  else
    printf("\n GetNetworkConfiguration fails \n\n");

  printf("Nat type configured: \n");
  if(nat_type == QCMAP_MSGR_NAT_SYMMETRIC_NAT_V01)
  {
    printf("Symmetric NAT");
  }
  else if(nat_type == QCMAP_MSGR_NAT_PORT_RESTRICTED_CONE_NAT_V01)
  {
    printf("Port Restricted Cone NAT");
  }
  else if (nat_type == QCMAP_MSGR_NAT_FULL_CONE_NAT_V01)
  {
    printf("Full Cone NAT");
  }
  else if(nat_type == QCMAP_MSGR_NAT_ADDRESS_RESTRICTED_NAT_V01)
  {
    printf("Address Restricted Cone NAT");
  }
  else
  {
    printf("Default NAT configured");
  }
  printf("\nNAT Timeout Values Configured:\n");
  timeout_type = QCMAP_MSGR_NAT_TIMEOUT_GENERIC_V01;
  while(timeout_type <= QCMAP_MSGR_NAT_TIMEOUT_UDP_V01)
  {
    if(timeout_type == QCMAP_MSGR_NAT_TIMEOUT_GENERIC_V01)
    {
      printf("GENRIC NAT Timeout: %d\n", timeout_value[timeout_type]);
    }
    else if(timeout_type == QCMAP_MSGR_NAT_TIMEOUT_ICMP_V01)
    {
      printf("ICMP NAT Timeout: %d\n", timeout_value[timeout_type]);
    }
    else if(timeout_type == QCMAP_MSGR_NAT_TIMEOUT_TCP_ESTABLISHED_V01)
    {
      printf("TCP ESTABLISHED NAT Timeout: %d\n", timeout_value[timeout_type]);
    }
    else if(timeout_type == QCMAP_MSGR_NAT_TIMEOUT_UDP_V01)
    {
      printf("UDP NAT Timeout: %d\n", timeout_value[timeout_type]);
    }
    timeout_type++;
  }
  printf("\nWlan Configuration :");
  if(wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_V01)
  {
    printf(" WLAN Mode is AP.\n");
  }
  else if(wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_V01)
  {
    printf(" WLAN Mode is AP-AP.\n");
  }
  else if(wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01)
  {
    printf(" WLAN Mode is AP-AP-AP.\n");
  }
  else if(wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01)
  {
    printf(" WLAN Mode is AP-STA.\n");
  }
  else if(wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01)
  {
    printf(" WLAN Mode is AP-AP-STA.\n");
  }
  else if(wlan_mode == QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01)
  {
    printf(" WLAN Mode is STA.\n");
  }
  else if(wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01)
  {
    printf(" WLAN Mode is AP-AP-AP-AP.\n");
  }
  else if(wlan_mode == QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01)
  {
    printf(" WLAN is not configured.\n");
  }

  // Guest AP Access profile
  printf("\n Guest AP is setup to be in '%s' mode \n", (guest_ap_profile == QCMAP_MSGR_PROFILE_FULL_ACCESS_V01)?"Full Access" : "Internet Only Access");

  /* Print LAN Configuration. */
  printf("\n LAN Configuration.\n");
  addr.s_addr = htonl(lan_config.gw_ip);
  printf("\nGateway IP: %s\n", inet_ntoa(addr));
  addr.s_addr = htonl(lan_config.netmask);
  printf("\nNetmask : %s\n", inet_ntoa(addr));
  printf("\nDHCP Enabled: %d\n", lan_config.enable_dhcp);
  if ( lan_config.enable_dhcp == TRUE )
  {
    addr.s_addr = htonl(lan_config.dhcp_config.dhcp_start_ip);
    printf("\nDHCP Start IP: %s\n", inet_ntoa(addr));
    addr.s_addr = htonl(lan_config.dhcp_config.dhcp_end_ip);
    printf("\nDHCP End IP : %s\n", inet_ntoa(addr));
    printf("\nDHCP Lease Time (seconds) : %d\n", lan_config.dhcp_config.lease_time);
  }

  /* Print Staton Configuration. */
  printf("\nStation Configuration.\n");
  if ( station_config.conn_type == QCMAP_MSGR_STA_CONNECTION_DYNAMIC_V01)
    printf("\nConnection Type : DYNAMIC (DHCP)\n");
  else
  {
    printf("\nConnection Type : STATIC\n");
    addr.s_addr = htonl(station_config.static_ip_config.ip_addr);
    printf("\nIP Address: %s\n", inet_ntoa(addr));
    addr.s_addr = htonl(station_config.static_ip_config.gw_ip);
    printf("\nGateway IP : %s\n", inet_ntoa(addr));
    addr.s_addr = htonl(station_config.static_ip_config.netmask);
    printf("\nNetmask: %s\n", inet_ntoa(addr));
    addr.s_addr = htonl(station_config.static_ip_config.dns_addr);
    printf("\nDNS Address : %s\n", inet_ntoa(addr));
  }
  printf("\n STA is configured in %s\n",(station_config.ap_sta_bridge_mode?"Bridge Mode":"Router Mode"));

   if (QcMapClient->GetQCMAPBootupCfg(&mobileap_enable, &wlan_enable, &qmi_err_num))
  {
     printf("\n Mobile AP will be %s on bootup \n",((mobileap_enable == QCMAP_MSGR_ENABLE_ON_BOOT_V01) ?"ENABLED":"DISABLED"));
     printf("\n WLAN will be %s on bootup \n",((wlan_enable == QCMAP_MSGR_ENABLE_ON_BOOT_V01)?"ENABLED":"DISABLED"));
  }
   else
  {
     printf("\n  Get QCMAP Bootup Cfg Fails , Error: 0x%x \n", qmi_err_num);
  }
  memset(dhcp_reserv_record,0,QCMAP_MSGR_MAX_DHCP_RESERVATION_ENTRIES_V01*sizeof(qcmap_msgr_dhcp_reservation_v01));
  if( QcMapClient->GetDHCPReservRecords(dhcp_reserv_record, &num_entries,&qmi_err_num) )
  {
       //display each DHCP reservation records
       if ( !num_entries)
       {
           printf("\nNo DHCP Reservation Records");
       }
       else
       {
           for ( i = 0;i < num_entries; i++)
           {
             printf("\nEntry  %d:",i);
             ds_mac_addr_ntop(dhcp_reserv_record[i].client_mac_addr,mac_addr_str);
             if ( strncmp(mac_addr_str,MAC_NULL_STRING,QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01) != 0 )
             {
                 printf("\nMAC address of the client[%i]: %s",i,mac_addr_str);
             }
             tmpIP.s_addr =ntohl(dhcp_reserv_record[i].client_reserved_ip);
             printf("IP address of the client[%i]: %s",i,inet_ntoa(tmpIP));
             printf("Device Name of the client[%i]: %s",i,dhcp_reserv_record[i].client_device_name);
             printf("DHCP Reservation enabled for the client[%i]: %d",i,dhcp_reserv_record[i].enable_reservation);
           }
       }
   }
   else
   {
        printf("\nFailed to Dsiplay DHCP Reservation record. Error 0x%x.\n ", qmi_err_num);
   }

   /* Display cradle mode */
   qcmap_msgr_cradle_mode_v01 mode;
   if (QcMapClient->GetCradleMode(&mode, &qmi_err_num))
   {
     /* Only  QCMAP_MSGR_CRADLE_WAN_ROUTER_V01 is supported*/
     switch (mode)
     {
        case QCMAP_MSGR_CRADLE_DISABLED_V01:
          printf("\nMobile AP Cradle Mode is Disabled");
          break;
        case QCMAP_MSGR_CRADLE_LAN_BRIDGE_V01:
          printf("\nMobile AP Cradle Mode is LAN BRIDGE");
          break;
        case QCMAP_MSGR_CRADLE_LAN_ROUTER_V01:
          printf("\nMobile AP Cradle Mode is LAN ROUTER");
          break;
        case QCMAP_MSGR_CRADLE_WAN_BRIDGE_V01:
          printf("\nMobile AP Cradle Mode is WAN BRIDGE");
          break;
        case QCMAP_MSGR_CRADLE_WAN_ROUTER_V01:
          printf("\nMobile AP Cradle Mode is WAN ROUTER");
          break;
        default:
          printf("\nIncorrect state returned: 0x%x", mode);
          break;
     }
   }
   else
     printf("  Failed to Get Cradle Mode .Error 0x%x.\n ", qmi_err_num);

  /* Display Ethernet mode */
   qcmap_msgr_ethernet_mode_v01 eth_mode;
   if (QcMapClient->GetEthernetMode(&eth_mode, &qmi_err_num))
   {
     switch (eth_mode)
     {
        case QCMAP_MSGR_ETHERNET_LAN_ROUTER_V01:
          printf("\nMobile AP ETHERNET mode is LAN ROUTER");
          break;
        case QCMAP_MSGR_ETHERNET_WAN_ROUTER_V01:
          printf("\nMobile AP ETHERNET mode is WAN ROUTER");
          break;
        default:
          printf("\nIncorrect state returned: 0x%x", eth_mode);
          break;
     }
   }
   else
     printf("  Failed to Get Ethernet Mode .Error 0x%x.\n ", qmi_err_num);

  if (QcMapClient->GetPrefixDelegationConfig(&pd_mode, &qmi_err_num))
  {
    if (pd_mode)
      printf("Prefix Delegation configuration is enabled.\n");
    else
      printf("Prefix Delegation configuration is disabled.\n");
  }
  else
    printf("  Failed to Get Prefix Delegation config. Error 0x%x.\n ", qmi_err_num);
}

/* CLI Restructuring functions
------------------------------*/

void mobileApConfig(int mobileApOpt)
{
  qmi_error_type_v01 qmi_err_num;
  /* Initialize QMI Error Number. */
  qmi_err_num = QMI_ERR_NONE_V01;
  char scan_string[QCMAP_MSGR_MAX_FILE_PATH_LEN];

  /* MobileAp Configuration options */
  switch(mobileApOpt)
  {
    /* Display the current configuration of the MobileAp. */
    case 1:
      DisplayConfig();
      break;

    /* Enable/Disable MobileAp */
    case 2:
    {
      printf("   Please input MobileAP State(1-Enable/0-Disable) : ");
      fgets(scan_string, sizeof(scan_string), stdin);
      if (atoi(scan_string))
      {
        if (QcMapClient->EnableMobileAP(&qmi_err_num))
          printf("\nMobileAP Enable succeeds.");
        else
          printf("\nMobileAP Enable fails, Error: 0x%x", qmi_err_num);
      }
      else
      {
        if (QcMapClient->DisableMobileAP(&qmi_err_num))
          printf("\nMobileAP Disable in progress.");
        else
          printf("\nMobileAP Disable request fails, Error: 0x%x", qmi_err_num);
      }
      break;
    }

    /* Get Status of MobielAp */
    case 3:
      {
        qcmap_msgr_mobile_ap_status_enum_v01 status;
        if(QcMapClient->GetMobileAPStatus(&status, &qmi_err_num))
        {
         if(status ==QCMAP_MSGR_MOBILE_AP_STATUS_CONNECTED_V01)
         {
           printf("\nMobile AP is Connected");
         }
          else if(status == QCMAP_MSGR_MOBILE_AP_STATUS_DISCONNECTED_V01)
           printf("\nMobile AP is Disconnected");
        }
        break;
      }

    /* Display's Connected Device's information */
    case 4:
      {
        char tmpIPv4[INET_ADDRSTRLEN];
        in6_addr allipv6[QCMAP_MSGR_MAX_IPV6_ADDR_V01];
        in6_addr tmpipv6;
        uint8 zero_buff[QCMAP_MSGR_IPV6_ADDR_LEN_V01] = {0};
        char ip6_addr_buf[INET6_ADDRSTRLEN];
        int connDevCount=0, num_entries=0;
        char mac_addr_str[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01]; /*char array of mac address*/
        qcmap_msgr_connected_device_info_v01
            connected_devices[QCMAP_MSGR_MAX_CONNECTED_DEVICES_V01];
        memset(connected_devices,
               0,
               (QCMAP_MSGR_MAX_CONNECTED_DEVICES_V01
               *sizeof(qcmap_msgr_connected_device_info_v01)));
        memset(tmpIPv4,0,INET_ADDRSTRLEN);

        if (QcMapClient->GetConnectedDevicesInfo(connected_devices,
                                                 &num_entries, &qmi_err_num))
        {
          if(num_entries != 0)
          {
            printf("\n Printing Connected Device Info for this Device \n");
            // Displaying the information in appropriate fashion
            for (connDevCount=0; connDevCount<num_entries; connDevCount++)
            {
              printf("Device No : %d \n",connDevCount+1);
              ds_mac_addr_ntop(connected_devices[connDevCount].client_mac_addr,
                               mac_addr_str);
              printf("MAC Address : %s \n",mac_addr_str);
              if(inet_ntop(AF_INET,
                           (void *)&connected_devices[connDevCount].ipv4_addr,tmpIPv4,
                           INET_ADDRSTRLEN))
              {
                printf("IPv4 Address : %s \n",tmpIPv4);
              }
              memset(&tmpipv6, 0, sizeof(tmpipv6));
              memcpy(&tmpipv6.s6_addr,
                     connected_devices[connDevCount].ll_ipv6_addr,
                     QCMAP_MSGR_IPV6_ADDR_LEN_V01);
              if(inet_ntop(AF_INET6,
                           (void*)&tmpipv6, ip6_addr_buf, sizeof(ip6_addr_buf)))
              {
                printf("Link Local IPv6 Address : %s\n",ip6_addr_buf);
              }

             for (int i = 0; i < QCMAP_MSGR_MAX_IPV6_ADDR_V01; i++)
             {
               memset(&allipv6[i], 0, sizeof(in6_addr));
               memset(ip6_addr_buf, 0, INET6_ADDRSTRLEN);

               memcpy(&allipv6[i].s6_addr,
                      connected_devices[connDevCount].ipv6[i].addr,
                       QCMAP_MSGR_IPV6_ADDR_LEN_V01);
               if (!memcmp(&allipv6[i].s6_addr, zero_buff,QCMAP_MSGR_IPV6_ADDR_LEN_V01))
                 break;
               if(inet_ntop(AF_INET6,
                            (void*)&allipv6[i], ip6_addr_buf, sizeof(ip6_addr_buf)))
               {
                 printf("IPv6 Address %d: %s\n",i, ip6_addr_buf);
               }
             }
             switch (connected_devices[connDevCount].device_type)
             {
               case QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01:
                 printf("Device Type : Primary AP\n");
                 break;
               case QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01:
                 printf("Device Type :Guest AP1\n");
                 break;
               case QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01:
                 printf("Device Type :Guest AP2\n");
                 break;
               case QCMAP_MSGR_DEVICE_TYPE_USB_V01:
                 printf("Device Type :USB\n");
                 break;
               case QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01:
                 printf("Device Type :Ethernet\n");
                 break;
               default:
                 printf("Device Type : Invalid\n");
                 break;
             }
             printf("Host Name : %s\n",
                     connected_devices[connDevCount].host_name);
             printf("rx bytes : %llu\n",
                     connected_devices[connDevCount].bytes_rx);
             printf("tx bytes : %llu\n",
                     connected_devices[connDevCount].bytes_tx);
             printf("Lease Expiry Time (in minutes) : %d\n",
                     connected_devices[connDevCount].lease_expiry_time);
             if (connected_devices[connDevCount].vlan_id != 0)
             {
               printf("VLAN ID :%d\n",connected_devices[connDevCount].vlan_id);
             }
            }
          }
          else
          {
            printf("\n No Connected Device to this Access Point \n");
          }
        }
        else
        {
          printf("\n Error in fetching Connected Device info. Error: 0x%x",
                 qmi_err_num);
        }
      }
      break;

    /* Enable/Disable/Reset Packet Stats */
    case 5:
    {
      printf("   Please input Packet Stats State(1-Enable/0-Disable/5-Reset) : ");
      fgets(scan_string, sizeof(scan_string), stdin);
      if (atoi(scan_string)==PACKET_STATS_ENABLE)
      {
        if (QcMapClient->EnablePacketStats(&qmi_err_num))
          printf("\nPacket Stats Enable succeeds.");
        else
          printf("\nPacket Stats Enable fails, Error: 0x%x", qmi_err_num);
      }
      else if (atoi(scan_string) == PACKET_STATS_DISABLE)
      {
        if (QcMapClient->DisablePacketStats(&qmi_err_num))
          printf("\nPacket Stats Disabled.");
        else
          printf("\nPacket Stats Disable request fails, Error: 0x%x", qmi_err_num);
      }
      else if (atoi(scan_string) == PACKET_STATS_RESET)
      {
        if (QcMapClient->ResetPacketStats(&qmi_err_num))
          printf("\nPacket Stats succeeded");
        else
          printf("\nPacket Stats reset request fails, Error: 0x%x", qmi_err_num);
      }
      break;
    }
     /* Get Packet Stats Status */
    case 6:
      {
        qcmap_msgr_packet_stats_status_enum_v01 status;
        if(QcMapClient->GetPacketStatsStatus(&status, &qmi_err_num))
        {
          if (status == QCMAP_MSGR_PACKET_STATS_STATUS_ENABLED_V01)
         {
           printf("\nPacket Stats is Enabled");
         }
          else if(status == QCMAP_MSGR_PACKET_STATS_STATUS_DISABLED_V01)
           printf("\nPacket Stats is Disabled");
        }
        break;
      }
     /* Will reset the device to  default factory configuration and reboot */
    case 7:
      {
        printf("\n Caution this will Restore Factory configuration and");
        printf("\n reboot the device (1-continue/0-exit this option):");
        fgets(scan_string, sizeof(scan_string), stdin);
        if ( atoi( scan_string ) == 1 )
        {
          if ( QcMapClient->RestoreFactoryConfig( &qmi_err_num ) )
          {
            printf("\nRestore to Factory config success");
            printf("\nSystem will reboot in 5 sec");
          }
          else
          {
            printf("\nRestore Factory config returns Error: 0x%x", qmi_err_num);
          }
        }
        break;
      }

     /* Disconnect BackHaul, disable LAN and exit application. */
    case 8:
      {
        sighandler(SIGTERM);
        exit(1);
      }
      break;

    /*Set Data Path Optimization Flag*/
    case 9:
    {
      boolean data_path_opt_status;
      int input;

      printf("\n Please input 1-[enable]/0-[disable]:");
      fgets(scan_string, sizeof(scan_string), stdin);
      input = atoi(scan_string);
      printf("\n Response: %d",input);
      if (input > 1 || input < 0)
      {
        printf("\nInvalid response\n");
        break;
      }
      else
      {
        data_path_opt_status = input;
        if(QcMapClient->SetDataPathOptStatus(data_path_opt_status , &qmi_err_num))
        {
           printf("\nData path opt status set successfully\n");
        }
        else
        {
           printf("\n Set data path opt  status fails, Error: 0x%x \n", qmi_err_num);
        }
      }
      break;
    }

    /* Get Data Path Optimization Flag */
    case 10:
    {
      boolean data_path_opt_status = 0;
      if (QcMapClient->GetDataPathOptStatus(&data_path_opt_status , &qmi_err_num))
      {
        if(data_path_opt_status)
          printf("\nData optimization handler enabled\n");
        else
          printf("\nData optimization handler not enabled\n");
      }
      else
      {
        printf("\n Get data path opt  status fails, Error: 0x%x \n", qmi_err_num);
      }
      break;
    }
    default:
    {
      printf("Invalid response %d\n", mobileApOpt);
    }
      break;
  }
}

void lanConfig(int lanOpt)
{
  qmi_error_type_v01 qmi_err_num;
  /* Initialize QMI Error Number. */
  qmi_err_num = QMI_ERR_NONE_V01;
  char scan_string[QCMAP_MSGR_MAX_FILE_PATH_LEN];
  qcmap_msgr_dhcp_reservation_v01 dhcp_reserv_record;
  in_addr addr;

  /* LAN Configuration options */
  switch(lanOpt)
  {
  /* setting LAN configuration parameters */
  case 1:
  {
    int p_error=0;
    qcmap_msgr_lan_config_v01 lan_config;
    in_addr addr;
    memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);
    memset(&lan_config,0,sizeof(qcmap_msgr_lan_config_v01));

    printf("   Do you want to set LAN Configuration(1-Yes/0-No) : ");
    fgets(scan_string, sizeof(scan_string), stdin);
    if (atoi(scan_string) == 1)
    {
      while (TRUE)
      {
        printf("   Please input Gateway IP address : ");
        if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
        {
          memset(&addr,0,sizeof(in_addr));
          if ( !(inet_aton(scan_string, &addr) <= 0) )
          {
            lan_config.gw_ip = ntohl(addr.s_addr);
            break;
          }
        }
        printf("      Invalid IPv4 address %s\n", scan_string);
      }
      while (TRUE)
      {
        printf("   Please input AP subnet  : ");
        if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
        {
          memset(&addr,0,sizeof(in_addr));
          if ( !(inet_aton(scan_string, &addr) <= 0) )
          {
            lan_config.netmask = ntohl(addr.s_addr);
            break;
          }
        }
        printf("      Invalid IPv4 address %s\n", scan_string);
      }
      printf("   Please input Enable/Disable DHCP(1-Enable/0-Disable):");
      fgets(scan_string, sizeof(scan_string), stdin);
      if ( atoi(scan_string) == 1)
      {
        lan_config.enable_dhcp = 1;
        printf("   Please input DHCP Configuration\n");
        while (TRUE)
        {
          printf("   Please input starting DHCPD address : ");
          if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
          {
            memset(&addr,0,sizeof(in_addr));
            if ( !(inet_aton(scan_string, &addr) <= 0) )
            {
              lan_config.dhcp_config.dhcp_start_ip = ntohl(addr.s_addr);
              break;
            }
          }
          printf("      Invalid IPv4 address %s\n", scan_string);
        }
        while (TRUE)
        {
          printf("   Please input ending DHCPD address : ");
          if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
          {
            memset(&addr,0,sizeof(in_addr));
            if ( !(inet_aton(scan_string, &addr) <= 0) )
            {
              lan_config.dhcp_config.dhcp_end_ip = ntohl(addr.s_addr);
              // Range check
              if (lan_config.dhcp_config.dhcp_end_ip >= lan_config.dhcp_config.dhcp_start_ip) {
                 break;
              } else {
                 LOG_MSG_ERROR("SSID1 (AP Mode) DHCP Address Range provided is too short",0,0,0);
                 printf(" Please enter a different DHCP End Address. \n");
              }
            }
          }
          printf("      Invalid IPv4 address %s\n", scan_string);
        }
        while(TRUE)
        {
          printf("   Please input DHCP lease time in Seconds:");
          printf("   Minimum DHCP lease time in 120 Seconds:");
          fgets(scan_string, sizeof(scan_string), stdin);
          lan_config.dhcp_config.lease_time = (uint32)atoi(scan_string);
          if(lan_config.dhcp_config.lease_time >= MIN_DHCP_LEASE)
          break;
        }
      }
      else
      {
        lan_config.enable_dhcp = 0;
      }
    }

    if (QcMapClient->SetLANConfig(lan_config, &qmi_err_num))
    {
      if (qmi_err_num ==  QMI_ERR_NONE_V01) {
        printf("\n LAN Config Set Successfully\n");
      } else {
          if (qmi_err_num == QMI_ERR_INVALID_ARG_V01) {
            printf("SSID1 (AP Mode) DHCP Address Range provided is invalid, Minimum range is 7 \n");
            printf("Setting AP DHCP address to default values which are derived from AP Gateway Addr \n");
          } else {
            printf("\n LAN Config set fails, Error: 0x%x", qmi_err_num);
          }
      }
    }
    else
    {
      printf("\n LAN Config set fails, Error: 0x%x", qmi_err_num);
    }
    break;
  }

  /* Getting LAN configuration's */
  case 2:
  {
    qcmap_msgr_lan_config_v01 lan_config;
    in_addr addr;
    int p_error=0;
    memset(&lan_config,0,sizeof(qcmap_msgr_lan_config_v01));
    memset(&addr, 0 ,sizeof(in_addr));
    memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);
    if (QcMapClient->GetLANConfig(&lan_config,&qmi_err_num))
    {
      /* Print AP Configuration. */
      printf("\nAP Configuration.\n");
      addr.s_addr = htonl(lan_config.gw_ip);
      printf("\nGateway IP: %s\n", inet_ntoa(addr));
      addr.s_addr = htonl(lan_config.netmask);
      printf("\nNetmask : %s\n", inet_ntoa(addr));
      printf("\nDHCP Enabled: %d\n", lan_config.enable_dhcp);
      if ( lan_config.enable_dhcp == TRUE )
      {
        addr.s_addr = htonl(lan_config.dhcp_config.dhcp_start_ip);
        printf("\nDHCP Start IP: %s\n", inet_ntoa(addr));
        addr.s_addr = htonl(lan_config.dhcp_config.dhcp_end_ip);
        printf("\nDHCP End IP : %s\n", inet_ntoa(addr));
        printf("\nDHCP Lease Time (seconds) : %d\n", lan_config.dhcp_config.lease_time);
      }
    }
    else
    {
      printf("\nGet LAN Config failed, Error:0x%x", qmi_err_num);
    }
    break;
  }

  /* Activate LAN */
  case 3:
  {
    if(QcMapClient->ActivateLAN(&qmi_err_num))
    {
      printf("\nActivated LAN\n");
    }
    else
      printf("\nFailed to Activate LAN, Error: 0x%x", qmi_err_num);
    break;
  }

  /*Add DHCP Reservation record*/
  case 4:
  {
      int newline_index;
      memset(&dhcp_reserv_record,0,sizeof(qcmap_msgr_dhcp_reservation_v01));
      int i = 0, device_type = -1;
      boolean enable = TRUE;
      dhcp_reserv_record.enable_reservation = enable;
      while(TRUE)
      {
        printf("\nEnter the device type(0-USB/1-Other LAN clients):   ");
        fgets(scan_string,sizeof(scan_string),stdin);
        newline_index = strlen(scan_string)-1;
        scan_string[newline_index] = '\0';
        device_type = (atoi(scan_string));
        /* strcmp is used because atoi("0") and atoi(string representation of an non integral number) both gives 0 */
        if( device_type == 0 && strcmp("0", scan_string) != 0 )
          device_type = -1;
        if (!(device_type == 0 || device_type == 1))
        {
          printf("\nDevice Type not Supported");
          continue;
        }
        break;
      }
      if ( device_type == 1 )
      {
         GET_MAC_ADDR(scan_string,mac_addr_int);
         memcpy(dhcp_reserv_record.client_mac_addr, mac_addr_int,\
                sizeof(dhcp_reserv_record.client_mac_addr));
      }
      GET_IP_ADDR(scan_string,addr);
      dhcp_reserv_record.client_reserved_ip = ntohl(addr.s_addr);
      while(TRUE)
      {
        printf("Enter device name.If client is not USB client,press \"ENTER\" key to skip  :");
        if ( fgets(scan_string,sizeof(scan_string),stdin) != NULL )
        {
          if (*scan_string == '\n')
          {
            if (device_type)
            {
              break;
            }
            else
            {
              printf("Client name is mandatory for USB tethered client\n");
              continue;
            }
          }
          else
          {
            for ( i=0;i < strlen(scan_string)-1;i++)
            {
              dhcp_reserv_record.client_device_name[i] = scan_string[i];
            }
            dhcp_reserv_record.client_device_name[i] ='\0';
            break;
          }
        }
      }
      printf("Enable/disable reservation for this client(1-Enable/0-Disable/Enter-skipped  :");
      fgets(scan_string,sizeof(scan_string),stdin);

      if ( *scan_string != '\n')
      {
          enable = (atoi(scan_string)) ? true:false;
          dhcp_reserv_record.enable_reservation = enable;
      }
      if( QcMapClient->AddDHCPReservRecord(&dhcp_reserv_record, &qmi_err_num) )
      {
          printf("\nDHCP  Reservation Record added successfully");
      }
      else
      {
         printf("\nFailed to add DHCP Reservation record. Error 0x%x.\n ", qmi_err_num);
      }
      break;
  }

  /* Display DHCP Reservation Record */
  case 5:
  {
    uint32_t num_entrees =0, i=0;;
    in_addr tmpIP;
    char mac_addr_str[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01]; /*char array of mac address*/
    qcmap_msgr_dhcp_reservation_v01 dhcp_reserv_record[QCMAP_MSGR_MAX_DHCP_RESERVATION_ENTRIES_V01];
    memset(dhcp_reserv_record,0,\
           QCMAP_MSGR_MAX_DHCP_RESERVATION_ENTRIES_V01*sizeof(qcmap_msgr_dhcp_reservation_v01));

    if( QcMapClient->GetDHCPReservRecords(dhcp_reserv_record, &num_entrees,&qmi_err_num) )
    {
       //display each DHCP reservation records
      if ( num_entrees == 0 )
      {
        printf("\nNo DHCP Reservation Records");
      }
      else
      {
         for ( i = 0;i < num_entrees; i++)
         {
           printf("\nEntry  %d:",i);
           ds_mac_addr_ntop(dhcp_reserv_record[i].client_mac_addr,mac_addr_str);
           if ( strncmp(mac_addr_str,MAC_NULL_STRING,QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01) != 0 )
           {
               printf("\nMAC address of the client[%i]: %s",i,mac_addr_str);
           }
           tmpIP.s_addr =ntohl(dhcp_reserv_record[i].client_reserved_ip);
           printf("\nIP address of the client[%i]: %s",i,inet_ntoa(tmpIP));
           if ( dhcp_reserv_record[i].client_device_name[0] != '\0')
           {
             printf("\nDevice Name of the client[%i]: %s",i,\
                    dhcp_reserv_record[i].client_device_name);
           }
           printf("\nDHCP Reservation enabled for the client[%i]: %d",i,\
                  dhcp_reserv_record[i].enable_reservation);
         }
       }
    }
    else
    {
       printf("\nFailed to Display DHCP Reservation record. Error 0x%x.\n ", qmi_err_num);
    }
    break;
  }

  /*Edit DHCP Reservation record*/
  case 6:
  {
    memset(&dhcp_reserv_record,0,sizeof(qcmap_msgr_dhcp_reservation_v01));
    uint32_t addr_to_edit =0;
    addr.s_addr =0;
    uint8 options=0;
    boolean enable;
    int i;

    GET_IP_ADDR(scan_string,addr);
    addr_to_edit = ntohl(addr.s_addr);
    dhcp_reserv_record.enable_reservation = true;
    while (TRUE )
    {
       printf("Please enter the field to edit:  ");
       printf("\n\t1. MAC Address\n\t2. IP Addr\n\t3. Device Name\n\t4. Enable/Disable\n\t:");
       fgets(scan_string,sizeof(scan_string),stdin);
       options = atoi(scan_string);
       switch (options)
       {
          case 1:
              GET_MAC_ADDR(scan_string,mac_addr_int);
              memcpy(dhcp_reserv_record.client_mac_addr, mac_addr_int,\
                     sizeof(dhcp_reserv_record.client_mac_addr));
              break;

           case 2:
             addr.s_addr =0;
             GET_IP_ADDR(scan_string,addr);
             dhcp_reserv_record.client_reserved_ip = ntohl(addr.s_addr);
             break;

           case 3:
              while(TRUE)
              {
                  printf("Please input the device name  :");
                  if ( fgets(scan_string,sizeof(scan_string),stdin) != NULL )
                  {
                       if ( *scan_string != '\n')
                       {
                          for (i=0;i < strlen(scan_string)-1;i++)
                          {
                              dhcp_reserv_record.client_device_name[i] = scan_string[i];
                          }
                          dhcp_reserv_record.client_device_name[i] ='\0';
                         break;
                       }
                  }
                  printf("\nInvalid Device name entered %s", scan_string);
              }
              break;
           case 4:
              printf("Enable/disable reservation for this client(1-Enable/0-Disable   :");
              fgets(scan_string,sizeof(scan_string),stdin);
              enable = (atoi(scan_string)) ? true:false;
              dhcp_reserv_record.enable_reservation = enable;
              break;
          default:
            printf("Invalid response %d\n",options);
            break;
       }
       printf("Do you wish to Edit more fields(Enter-skipped/other character to continue):");
       fgets(scan_string,sizeof(scan_string),stdin);
       if ( *scan_string == '\n')
       {
          break;
       }
       else
       {
          continue;
       }
    }
    if( QcMapClient->EditDHCPReservRecord(&addr_to_edit,&dhcp_reserv_record,&qmi_err_num) )
    {
      printf("\nDHCP  Reservation Record edited successfully");
    }
    else
   {
      printf("\nFailed to edit DHCP Reservation record. Error 0x%x.\n ", qmi_err_num);
   }
  break;
  }

  /*Delete DHCP Reservation record*/
  case 7:
  {
   uint32_t addr_to_edit =0;
   addr.s_addr =0;

   GET_IP_ADDR(scan_string,addr);

   addr_to_edit = ntohl(addr.s_addr);
   if( QcMapClient->DeleteDHCPReservRecord(&addr_to_edit,&qmi_err_num) )
   {
       printf("\nDHCP  Reservation Record deleted successfully");
   }
   else
   {
      printf("\nFailed to delete DHCP Reservation record. Error 0x%x.\n ", qmi_err_num);
   }
   break;
  }

  /* Set/Get Gateway URL */
  case 8:
  {
   char url[QCMAP_MSGR_MAX_GATEWAY_URL_V01];
   uint32_t url_len = 0;
   uint8 options = 0;
   bzero( url, QCMAP_MSGR_MAX_GATEWAY_URL_V01);
   printf("   Please input Gateway URL (1-SET URL /0-GET URL) : ");
   fgets(scan_string,sizeof(scan_string),stdin);
   options = atoi(scan_string);

   if ( options == 0 )
   {
      if(!QcMapClient->GetGatewayUrl((char *)&url,&url_len,&qmi_err_num))
      {
        printf("\n Failed to get URL . Error 0x%x",qmi_err_num);
      }
      printf("\n Gateway URL configured =%s ", url);
   }
   else if ( options == 1 )
   {
      printf("\n Please enter the URL:");
      fgets( scan_string, sizeof(scan_string), stdin);
      strlcpy( url, scan_string, strlen(scan_string));
      if(!QcMapClient->SetGatewayUrl((char *)&url, strlen(url), &qmi_err_num))
      {
        printf("\n Failed to set URL . Error 0x%x",qmi_err_num);
      }
      else
      {
        printf("\n Gateway URL configured \n");
      }
   }
   else
   {
     printf("\n Invalid input \n");
   }
   break;
  }
  break;
  case 9: //Add VLAN
  {
    qcmap_msgr_vlan_config_v01 vlan_config;
    int phy_option;
    int vlan_id;
    bool is_ipa_offloaded = false;

    memset(&vlan_config, 0, sizeof(vlan_config));


    //tell user what options are available
    printf("   +-----------+\n");
    printf("   |           |\n");
    printf("   |    PHY    |\n");
    printf("   |           |\n");
    printf("   +-----------+\n");
    printf("   | 1. ETH    |\n");
    printf("   | 2. ECM    |\n");
    printf("   | 3. RNDIS  |\n");
    printf("   +-----------+\n");

    //ask for user input
    printf("   Please select PHY iface:");
    memset(&scan_string, 0, sizeof(scan_string));
    if(fgets(scan_string, sizeof(scan_string), stdin) == NULL)
    {
      printf("error with client input: %s\n", strerror(errno));
      goto error_add_vlan;
    }
    phy_option = atoi(scan_string);
    switch(phy_option)
    {
      case(1): //ETH
      {
        snprintf(vlan_config.local_iface, sizeof(vlan_config.local_iface), "eth0");
        break;
      }
      case(2): //ECM
      {
        snprintf(vlan_config.local_iface, sizeof(vlan_config.local_iface), "ecm0");
        break;
      }
      case(3): //RNDIS
      {
        snprintf(vlan_config.local_iface, sizeof(vlan_config.local_iface), "rndis0");
        break;
      }
      default:
      {
        printf("Bad option selected: %d\n", phy_option);
        goto error_add_vlan;
        break;
      }
    }


    printf("\nPlease input VLAN ID(1-%d): ", MAX_VLAN_ID);
    memset(&scan_string, 0, sizeof(scan_string));
    fgets(scan_string, sizeof(scan_string) - 1, stdin);
    vlan_id = atoi(scan_string);

    if((vlan_id <= 0) || (vlan_id > MAX_VLAN_ID))
    {
      printf("\nInvalid value %d\n", vlan_id);
      goto error_add_vlan;
    }
    vlan_config.vlan_id = (int16_t)vlan_id;

    printf("\nDo you want IPA offload for %s.%d (1 - Enable Offload, 0 - Disable Offload)? ",
           vlan_config.local_iface,
           vlan_config.vlan_id);
    memset(&scan_string, 0, sizeof(scan_string));
    fgets(scan_string, sizeof(scan_string) - 1, stdin);
    switch(atoi(scan_string))
    {
      case 1:
      {
        vlan_config.ipa_offload = 1;
        break;
      }
      default:
      {
        vlan_config.ipa_offload = 0;
        break;
      }
    }

    if(QcMapClient->SetVLANConfig(vlan_config, &qmi_err_num, &is_ipa_offloaded))
    {
      if ((vlan_config.ipa_offload == 1) && !(is_ipa_offloaded))
      {
        printf("\nVLAN Interface added without offload as offload count reached max.\n");
      }

      switch(qmi_err_num)
      {
        case(QMI_ERR_OP_IN_PROGRESS_V01):
        {
          printf("\nAuto-reboot triggered for IPA HW offload to take affect...\n");
          break;
        }
        default:
        {
          printf("\nAdd VLAN Interfaces succeeds.\n");
          break;
        }
      }
    } else {
      switch(qmi_err_num)
      {
        case(QMI_ERR_INVALID_ARG_V01):
        {
          printf("\n Add VLAN Interface fails, simultaneous Cradle and Eth not in LAN "
                 "Router mode not supported\n");
          break;
        }
        default:
        {
          printf("\n Add VLAN Interface fails, Error: 0x%x \n", qmi_err_num);
          break;
        }
      }
    }

    break;

    error_add_vlan:
      printf("\nAdd VLAN Interface fails\n");
      break;
  }
  case 10: //Get VLAN
  {
    qcmap_msgr_vlan_conf_t vlan_config;
    memset(&vlan_config, 0, sizeof(qcmap_msgr_vlan_conf_t));
    if(QcMapClient->GetVLANConfig(&vlan_config,&qmi_err_num))
    {
       if(vlan_config.vlan_config_list_len == 0)
       {
         printf("\n NO VLAN Entries Configured\n");
       } else {
          printf("\nVLAN Entries Configured:\n");
         for(int i = 0; i < vlan_config.vlan_config_list_len; i++)
         {
           printf("Physical Interface: %s, VLAN ID: %d, IPA Offload: %u\n",
                   vlan_config.vlan_config_list[i].local_iface,
                   vlan_config.vlan_config_list[i].vlan_id,
                   vlan_config.vlan_config_list[i].ipa_offload);
          }
       }
    } else {
      printf("\n Get VLAN Config fails, Error: 0x%x \n", qmi_err_num);
    }
    break;
  }
  case 11: //Delete VLAN
  {
    qcmap_msgr_vlan_config_v01 vlan_config;
    int phy_option;
    int vlan_id;

    memset(&vlan_config, 0, sizeof(vlan_config));


    //tell user what options are available
    printf("   +-----------+\n");
    printf("   |           |\n");
    printf("   |    PHY    |\n");
    printf("   |           |\n");
    printf("   +-----------+\n");
    printf("   | 1. ETH    |\n");
    printf("   | 2. ECM    |\n");
    printf("   | 3. RNDIS  |\n");
    printf("   +-----------+\n");

    //ask for user input
    printf("   Please select PHY iface:");
    memset(&scan_string, 0, sizeof(scan_string));
    if(fgets(scan_string, sizeof(scan_string), stdin) == NULL)
    {
      printf("error with client input: %s\n", strerror(errno));
      goto error_delete_vlan;
    }
    phy_option = atoi(scan_string);
    switch(phy_option)
    {
      case(1): //ETH
      {
        snprintf(vlan_config.local_iface, sizeof(vlan_config.local_iface), "eth0");
        break;
      }
      case(2): //ECM
      {
        snprintf(vlan_config.local_iface, sizeof(vlan_config.local_iface), "ecm0");
        break;
      }
      case(3): //RNDIS
      {
        snprintf(vlan_config.local_iface, sizeof(vlan_config.local_iface), "rndis0");
        break;
      }
      default:
      {
        printf("Bad option selected: %d\n", phy_option);
        goto error_delete_vlan;
        break;
      }
    }


    printf("\nPlease input VLAN ID(1-%d): ", MAX_VLAN_ID);
    memset(&scan_string, 0, sizeof(scan_string));
    fgets(scan_string, sizeof(scan_string) - 1, stdin);
    vlan_id = atoi(scan_string);

    if((vlan_id <= 0) || (vlan_id > MAX_VLAN_ID))
    {
      printf("\nInvalid value %d\n", vlan_id);
      goto error_delete_vlan;
    }
    vlan_config.vlan_id = (int16_t)vlan_id;

     if(QcMapClient->DeleteVLANConfig(vlan_config, &qmi_err_num))
    {
      printf("\n Delete VLAN Interface succeeds.\n");
    } else {
      printf("\n Delete VLAN Interface fails, Error: 0x%x \n", qmi_err_num);
    }

    break;

    error_delete_vlan:
      printf("\nDelete VLAN Interface fails\n");
      break;
  }
  case 12:
  {
    qcmap_msgr_set_unmanaged_l2tp_state_config_v01 l2tp_enable_config;
    qcmap_msgr_l2tp_TCP_MSS_config_v01 l2tp_MSS_config;
    qcmap_msgr_l2tp_mtu_config_v01 l2tp_MTU_config;
    memset(&l2tp_enable_config,0,
                      sizeof(qcmap_msgr_set_unmanaged_l2tp_state_config_v01));
    memset(&l2tp_MSS_config,0,
                      sizeof(qcmap_msgr_l2tp_TCP_MSS_config_v01));
    memset(&l2tp_MTU_config,0,sizeof(qcmap_msgr_l2tp_mtu_config_v01));

    printf("\n Please input Enable/Disable L2TP for Unmanaged Tunnels:");
    printf("\n0. Disable\n1. Enable:");
    fgets(scan_string, sizeof(scan_string), stdin);

    if (!( 0 == atoi(scan_string) || 1 == atoi(scan_string)))
    {
      printf("\nInvalid valvue %d \n",atoi(scan_string));
      break;
    }
    l2tp_enable_config.enable = atoi(scan_string);

    if (l2tp_enable_config.enable)
    {

      printf("\n Please enter if TCP MSS to be clamped on L2TP interfaces"
           "to avoid Segmentation(1. Enable 0. Disable):");
      fgets(scan_string, sizeof(scan_string), stdin);
      if (!( 0 == atoi(scan_string) || 1 == atoi(scan_string)))
      {
        printf("\nInvalid valvue %d \n",atoi(scan_string));
        break;
      }
      l2tp_MSS_config.enable = atoi(scan_string);

      printf("\n Please enter if MTU size to be set on underlying interfaces"
             "to avoid fragmentation(1. Enable 0. Disable):");
      fgets(scan_string, sizeof(scan_string), stdin);
      if (!( 0 == atoi(scan_string) || 1 == atoi(scan_string)))
      {
        printf("\nInvalid valvue %d \n",atoi(scan_string));
        break;
      }
      l2tp_MTU_config.enable = atoi(scan_string);
    }

    if (QcMapClient->SetUnmanagedL2TPState(l2tp_enable_config,
                                           l2tp_MTU_config,
                                           l2tp_MSS_config,
                                           &qmi_err_num))
    {
      printf("\n Set L2TP Unmanaged state succeeds.");
    }
    else
    {
      if (qmi_err_num == QMI_ERR_NO_EFFECT_V01) {
        printf("\n L2TP Unmanaged state already set \n");
      } else {
     printf("\n Set L2TP Unmanaged state fails, Error: 0x%x \n", qmi_err_num);
      }
    }
  }
  break;
  case 13:
  {
    int temp_uint32 = 0;
    int num_sessions = 0;
    qcmap_msgr_l2tp_config_v01 l2tp_config;
    memset(&l2tp_config,0,sizeof(qcmap_msgr_l2tp_config_v01));

    printf("\n Please input interface name on top of which L2TP Tunnel"
           " has to be created:");
    fgets(scan_string, sizeof(scan_string), stdin);
    if ( strlen(scan_string) == 0
         ||
         strlen( scan_string ) > QCMAP_MAX_IFACE_NAME_SIZE_V01)
    {
      printf("\nInvalid string length \n");
      break;
    }
    strlcpy(l2tp_config.local_iface,scan_string,strlen(scan_string));

    printf("\n Please input local Tunnel ID:");
    fgets(scan_string, sizeof(scan_string), stdin);
    temp_uint32 = atoi(scan_string);
    if ( temp_uint32 > MAX_UINT32_VAL)
    {
      printf("\nInvalid value \n");
      break;
    }
    l2tp_config.local_tunnel_id = temp_uint32;

    printf("\n Please input peer Tunnel ID:");
    fgets(scan_string, sizeof(scan_string), stdin);
    temp_uint32 = atoi(scan_string);
    if ( temp_uint32 > MAX_UINT32_VAL)
    {
      printf("\nInvalid value \n");
      break;
    }
    l2tp_config.peer_tunnel_id = temp_uint32;

    printf("\n Please enter peer IP Version\n (4. v4 \n6. v6):");
    fgets(scan_string, sizeof(scan_string), stdin);
    temp_uint32 = atoi(scan_string);
    if ( temp_uint32 == 4)
    {
      l2tp_config.ip_family = QCMAP_MSGR_IP_FAMILY_V4_V01;
      printf("\nPlease input Peer IPv4 Addr [xxx.xxx.xxx.xxx]:");
      read_addr(AF_INET,(char *)&l2tp_config.peer_ipv4_addr);

    }
    else if(temp_uint32 == 6)
    {
      l2tp_config.ip_family = QCMAP_MSGR_IP_FAMILY_V6_V01;
      printf("\nPlease input Peer IPv6:");
      read_addr(AF_INET6,(char *)&l2tp_config.peer_ipv6_addr[0]);
    }
    else
    {
      printf("\nInvalid value \n");
      break;
    }

    printf("\n Please input Encapsulation Protocol\n (%d. UDP \n%d. IP):",
           PS_IPPROTO_UDP, PS_IPPROTO_IP);
    fgets(scan_string, sizeof(scan_string), stdin);
    temp_uint32 = atoi(scan_string);
    if (temp_uint32 == PS_IPPROTO_IP)
    {
      l2tp_config.proto = QCMAP_MSGR_L2TP_ENCAP_IP_V01;
    }
    else if (temp_uint32 == PS_IPPROTO_UDP)
    {
      l2tp_config.proto = QCMAP_MSGR_L2TP_ENCAP_UDP_V01;
      printf("\nPlease input local UDP Port:");
      temp_uint32 = read_uint32();
      if (0 != check_port(temp_uint32))
      {
        printf("\nInvalid value \n");
        break;
      }
      l2tp_config.local_udp_port = temp_uint32;

      printf("\nPlease input peer UDP Port:");
      temp_uint32 = read_uint32();
      if (0 != check_port(temp_uint32))
      {
        printf("\nInvalid value \n");
        break;
      }
      l2tp_config.peer_udp_port = temp_uint32;
    }
    else
    {
      printf("\nInvalid protocol value entered %d \n",temp_uint32);
      break;
    }

    printf("\n Please input number of session for this "
        "tunnel(Max allowed %d):",QCMAP_MSGR_L2TP_MAX_SESSION_PER_TUNNEL_V01);
    fgets(scan_string, sizeof(scan_string), stdin);
    temp_uint32 = atoi(scan_string);
    if (0 > temp_uint32
         ||
       temp_uint32 > QCMAP_MSGR_L2TP_MAX_SESSION_PER_TUNNEL_V01)
    {
      printf("\nInvalid value \n");
      break;
    }
    num_sessions = temp_uint32;
    for (int i = 0; i < num_sessions; i++)
    {
      printf("\n Please input local Session ID for session %d:",i+1);
      fgets(scan_string, sizeof(scan_string), stdin);
      temp_uint32 = atoi(scan_string);
      if (0 > temp_uint32 || temp_uint32 > MAX_UINT32_VAL)
      {
        printf("\nInvalid value \n");
        break;
      }
      l2tp_config.session_config[i].session_id = temp_uint32;

      printf("\n Please input peer Session ID for session %d:",i+1);
      fgets(scan_string, sizeof(scan_string), stdin);
      temp_uint32 = atoi(scan_string);
      if (0 > temp_uint32 || temp_uint32 > MAX_UINT32_VAL)
      {
        printf("\nInvalid value \n");
        break;
      }
      l2tp_config.session_config[i].peer_session_id = temp_uint32;
    }

    if(QcMapClient->SetL2TPConfig(QCMAP_MSGR_L2TP_ROUTER_MODE_V01, l2tp_config, &qmi_err_num))
    {
      printf("\n Set L2TP Config succeeds.");
      break;
    }

    switch(qmi_err_num)
    {
      case(QMI_ERR_NO_EFFECT_V01):
      {
        printf("\n L2TP Config already set \n");
        break;
      }
      case(QMI_ERR_NOT_SUPPORTED_V01):
      {
        printf("\n L2TP config is not enabled, please enable L2TP config.\n");
        break;
      }
      case(QMI_ERR_INCOMPATIBLE_STATE_V01):
      {
        printf("\nL2TP config can not be enabled...\n"
               "Please map VLAN to default PDN first.\n");
        break;
      }
      default:
      {
        printf("\n Set L2TP Config fails, Error: 0x%x \n", qmi_err_num);
        break;
      }
    }
  }
  break;
  case 14:
  {
    qcmap_msgr_l2tp_conf_t l2tp_conf;
    char v6add_str[INET6_ADDRSTRLEN] = {0};
    char v4add_str[INET_ADDRSTRLEN] = {0};
    qmi_error_type_v01 qmi_err_num;
    memset(&l2tp_conf,0,sizeof(l2tp_conf));
    if (QcMapClient->GetL2TPConfig(&l2tp_conf,&qmi_err_num))
    {
      if (l2tp_conf.l2tp_mtu_config.enable)
      {
        printf("\n L2TP MTU Config is Enabled.\n");
      }
      else
      {
        printf("\n L2TP MTU Config is Disabled.\n");
      }

      if (l2tp_conf.l2tp_mss_config.enable)
      {
        printf("\n L2TP TCP MSS Config is Enabled.\n");
      }
      else
      {
        printf("\n L2TP TCP MSS Config is Disabled.\n");
      }

      if (l2tp_conf.l2tp_config_list_len == 0)
      {
        printf("\n NO L2TP Entries Configured\n");
      }
      else
      {
        printf("\n Current L2TP Tunnel Configuration");
        for (int i = 0; i < l2tp_conf.l2tp_config_list_len; i++)
        {
          printf("\nPhysical interface: %s",
                              l2tp_conf.l2tp_config_list[i].local_iface );
          printf("\nLocal Tunnel ID: %d",
                         l2tp_conf.l2tp_config_list[i].local_tunnel_id);
          printf("\nPeer Tunnel ID: %d",
                          l2tp_conf.l2tp_config_list[i].peer_tunnel_id);

          if (l2tp_conf.l2tp_config_list[i].ip_family ==
                                      QCMAP_MSGR_IP_FAMILY_V4_V01)
          {
            printf("\nIP Version: v4");
            inet_ntop(AF_INET,&l2tp_conf.l2tp_config_list[i].peer_ipv4_addr,
                      v4add_str,INET_ADDRSTRLEN);
             printf("\nPeer IPv4 Address: %s",v4add_str);
          }
          else if (l2tp_conf.l2tp_config_list[i].ip_family ==
                                    QCMAP_MSGR_IP_FAMILY_V6_V01)
          {
            printf("\nIP Version: v6");
            inet_ntop(AF_INET6,
                      (void *)&l2tp_conf.l2tp_config_list[i].peer_ipv6_addr,
                      v6add_str,INET6_ADDRSTRLEN);
            printf("\nPeer IPv6 Address: %s",v6add_str);
          }

          if (l2tp_conf.l2tp_config_list[i].proto ==
                QCMAP_MSGR_L2TP_ENCAP_UDP_V01)
          {
            printf("\nEncapsulation Protocol: UDP");
            if(0 == check_port(l2tp_conf.l2tp_config_list[i].local_udp_port))
            {
              printf("\nLocal UDP Port: %d",
                         l2tp_conf.l2tp_config_list[i].local_udp_port);
            }
            else
            {
              printf("\nInvalid local UDP prot received: %d",
                         l2tp_conf.l2tp_config_list[i].local_udp_port);
              break;
            }
            if(0 == check_port(l2tp_conf.l2tp_config_list[i].peer_udp_port))
            {
              printf("\nPeer UDP Port: %d",
                         l2tp_conf.l2tp_config_list[i].peer_udp_port);
            }
            else
            {
              printf("\nInvalid peer UDP prot received: %d",
                         l2tp_conf.l2tp_config_list[i].peer_udp_port);
              break;
            }
          }
          else if (l2tp_conf.l2tp_config_list[i].proto =
                  QCMAP_MSGR_L2TP_ENCAP_IP_V01)
          {
            printf("\nEncapsulation Protocol: IP");
          }
          else
          {
            printf("\nInvalid IP version received");
            break;
          }

          for (int j = 0; j < QCMAP_MSGR_L2TP_MAX_SESSION_PER_TUNNEL_V01; j++)
          {
            if (l2tp_conf.l2tp_config_list[i].session_config[j].session_id != 0)
            {
              printf("\nSession %d",j+1);
              printf("\n  Session ID: %d",
                   l2tp_conf.l2tp_config_list[i].session_config[j].session_id);
              printf("\n  Peer Session ID: %d\n",
                   l2tp_conf.l2tp_config_list[i].session_config[j].peer_session_id);
            }
          }/* End for int j */
        }/*end for int i*/
      }
    }
    else
    {
      if (qmi_err_num == QMI_ERR_NOT_SUPPORTED_V01)
      {
        printf("\n L2TP Unmanaged tunnel state is not enabled \n");
      }
      else
        printf("\n Get L2TP Config fails, Error: 0x%x \n", qmi_err_num);
    }
  }
  break;
  case 15:
  {
    qcmap_msgr_delete_l2tp_config_v01 l2tp_delete_config;
    int temp32 = 0;
    memset(&l2tp_delete_config,0,sizeof(qcmap_msgr_delete_l2tp_config_v01));

    printf("\n Please input Tunnel ID to delete:");
    fgets(scan_string, sizeof(scan_string), stdin);
    temp32 = atoi(scan_string);
    if ( temp32 ==0 || temp32 > MAX_UINT32_VAL)
    {
      printf("\nInvalid value \n");
      break;
    }

    l2tp_delete_config.tunnel_id = temp32;
    if (QcMapClient->DeleteL2TPTunnelConfig(l2tp_delete_config,&qmi_err_num))
    {
      printf("\n Delete L2TP Config succeeds.");
    }
    else
    {
      if (qmi_err_num == QMI_ERR_NO_EFFECT_V01) {
        printf("\n Tunnel config is not found to delete. \n");
      } else {
      printf("\n Delete L2TP Config fails, Error: 0x%x \n",
                                                           qmi_err_num);
      }
    }
  }
  break;
  case 16: //Select LAN Bridge Context
  {
    int16_t chosen_bridge = -1;

    if((chosen_bridge = ChooseLANBridge()) >= 0)
    {
      if(!QcMapClient->SelectLANBridge(chosen_bridge, &qmi_err_num))
      {
        printf("Failed to select LAN bridge: 0x%x", qmi_err_num);
      }
    } else {
      printf("Invalid LAN Bridge: %d", chosen_bridge);
    }
    break;
  }
  case 17: //Get Bridge-VLAN Context
  {
    qcmap_msgr_bridge_list_v01 bridge_list;
    if(QcMapClient->GetLANBridges(&bridge_list, &qmi_err_num))
    {
      if(bridge_list.num_of_bridges > 0)
      {
        printf("\n Current LAN Bridge is for VLAN ID %d \n",bridge_list.curr_bridge);
      }
      else
      {
        printf("\n No Bridges configured");
      }
    }
    else
      printf("\n Get LAN Bridges fails, Error: 0x%x \n",qmi_err_num);
  }
  break;
  default:
  {
    printf("Invalid response %d\n", lanOpt);
  }
    break;
  }
}

void nat_alg_vpn_config( int natAlgOpt )
{
  qmi_error_type_v01 qmi_err_num;
  /* Initialize QMI Error Number. */
  qmi_err_num = QMI_ERR_NONE_V01;
  char scan_string[QCMAP_MSGR_MAX_FILE_PATH_LEN];
  in_addr addr;
  uint32 tmp_input=0;
  qcmap_msgr_snat_entry_config_v01 snat_entry;
  int i;

  /* NAT/AG/VPN Configuration options */
  switch(natAlgOpt)
  {
    /* Add a static NAT entry and save XML if successful. */
    case 1:
    {
      memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);
      in_addr tmp1, tmp2;

      inet_aton("0.0.0.0",&tmp1);
      inet_aton("255.255.255.255",&tmp2);

      memset(&snat_entry, 0, sizeof(qcmap_msgr_snat_entry_config_v01));

      while(TRUE)
      {
        printf("Please input port_fwding_protocol (TCP=%d, UDP=%d,ICMP=%d,TCP_UDP=%d,ESP=%d): ",\
                                                PS_IPPROTO_TCP, PS_IPPROTO_UDP, PS_IPPROTO_ICMP,\
                                                PS_IPPROTO_TCP_UDP, PS_IPPROTO_ESP);
        fgets(scan_string, sizeof(scan_string), stdin);
        tmp_input = atoi(scan_string);
        if ( check_proto(tmp_input) == 0 )
        break;
      }
      snat_entry.protocol = (uint8)tmp_input;
      while (TRUE)
      {
        printf("   Please input port_fwding_private_ip(xxx.xxx.xxx.xxx)   : ");
        if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
        {
           if ( !( inet_aton(scan_string, &addr) <= 0) &&
                ( addr.s_addr != tmp1.s_addr ) &&
                ( addr.s_addr != tmp2.s_addr ))
            break;
        }
        printf("   Invalid IPv4 address %s",scan_string);
      }
      snat_entry.private_ip_addr = ntohl(addr.s_addr);
      while (TRUE)
      {
        printf("   Please input port_fwding_private_port : ");
        fgets(scan_string, sizeof(scan_string), stdin);
        tmp_input = atoi(scan_string);

        /*No port validation check for ICMP*/
        if(snat_entry.protocol == 1)
          break;
        if(check_port (tmp_input) == 0 )
          break;
      }
      snat_entry.private_port = (uint16)tmp_input;

      while (TRUE)
      {
        printf("   Please input port_fwding_global_port  : ");
        fgets(scan_string, sizeof(scan_string), stdin);
        tmp_input = atoi(scan_string);

        /*No port validation check for ICMP*/
        if(snat_entry.protocol == 1)
          break;
        if(check_port (tmp_input) == 0 )
          break;
      }

      snat_entry.global_port = (uint16)tmp_input;

    if (QcMapClient->AddStaticNatEntry(&snat_entry, &qmi_err_num))
    {
      printf("\nSNAT Entry added successfully");
    }
    else
    {
      if (qmi_err_num == QMI_ERR_INTERFACE_NOT_FOUND_V01)
        printf("\nBackhaul down, SNAT Entry added to xml file");
      else
        printf("\nSNAT Entry add failed, Error: 0x%x", qmi_err_num);
    }
  }
  break;

  /* Delete a static NAT entry and save XML if successful. */
  case 2:
  {
    memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);
    memset(&snat_entry, 0, sizeof(qcmap_msgr_snat_entry_config_v01));
    in_addr tmp1, tmp2;
    inet_aton("0.0.0.0",&tmp1);
    inet_aton("255.255.255.255",&tmp2);

    while(TRUE)
    {
      printf("   Please input port_fwding_protocol   : ");
      fgets(scan_string, sizeof(scan_string), stdin);
      tmp_input = atoi(scan_string);
      if ( check_proto(tmp_input) == 0 )
      break;
    }
    snat_entry.protocol = (uint8)tmp_input;
    while (TRUE)
    {
      printf("   Please input port_fwding_private_ip(xxx.xxx.xxx.xxx)   : ");
      if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
      {
        if ( !( inet_aton(scan_string, &addr) <= 0) &&
      ( addr.s_addr != tmp1.s_addr ) &&
      ( addr.s_addr != tmp2.s_addr ))
      break;
      }
      printf("   Invalid IPv4 address %s",scan_string);
    }
    snat_entry.private_ip_addr = ntohl(addr.s_addr);
    while (TRUE)
    {
      printf("   Please input port_fwding_private_port : ");
      fgets(scan_string, sizeof(scan_string), stdin);
      tmp_input = atoi(scan_string);
      /*No port validation check for ICMP*/
      if(snat_entry.protocol == 1)
        break;
      if(check_port (tmp_input) == 0 )
        break;
    }
    snat_entry.private_port = (uint16)tmp_input;

    while (TRUE)
    {
      printf("   Please input port_fwding_global_port  : ");
      fgets(scan_string, sizeof(scan_string), stdin);
      tmp_input = atoi(scan_string);

      /*No port validation check for ICMP*/
      if(snat_entry.protocol == 1)
        break;
      if(check_port (tmp_input) == 0 )
        break;
    }
    snat_entry.global_port = (uint16)tmp_input;
    if (QcMapClient->DeleteStaticNatEntry(&snat_entry, &qmi_err_num))
    {
      printf("\nSNAT Entry deleted successfully");
    }
    else
    {
      if (qmi_err_num == QMI_ERR_INTERFACE_NOT_FOUND_V01)
      printf("\nBackhaul down, SNAT Entry deleted from xml file.");
      else
      printf("\nSNAT Entry delete failed, Error: 0x%x", qmi_err_num);
    }
  }
  break;

  /* Get SNAT config */
  case 3:
  {
    in_addr tmpIP;
    int i=0, num_entries=0;
    qcmap_msgr_snat_entry_config_v01 snat_config[QCMAP_MSGR_MAX_SNAT_ENTRIES_V01];

    memset(snat_config, 0, QCMAP_MSGR_MAX_SNAT_ENTRIES_V01*sizeof(qcmap_msgr_snat_entry_config_v01));

    if (QcMapClient->GetStaticNatConfig(snat_config, &num_entries, &qmi_err_num))
    {
      if(num_entries > 0)
      {
        for (i=0; i<num_entries; i++)
        {
          printf("\n\nEntry %d:",i);
          tmpIP.s_addr = ntohl(snat_config[i].private_ip_addr);
          printf("\nprivate ip: %s", inet_ntoa(tmpIP));
          printf("\nprivate port: %d", snat_config[i].private_port);
          printf("\nglobal port: %d", snat_config[i].global_port);
          printf("\nprotocol: %d", snat_config[i].protocol);
        }
      }
      else
      {
        printf("\nNo SNAT Entries Configured");
      }
    }
    else
    {
      printf("\nSNAT Entries get failed  Error: 0x%x", qmi_err_num);
    }
  }
  break;

  /* Set NAT type */
  case 4:
  {
    int nat_type;
    memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);

    printf("Select the Type of NAT : \n"
            "0:SYMMETRIC NAT\n1: PORT RESTRICTED CONE NAT\n2: FULL CONE NAT\n"
            "3: ADDRESS RESTRICTED CONE NAT\t");
    fgets(scan_string, sizeof(scan_string), stdin);
    nat_type = atoi(scan_string);
    if ( nat_type < QCMAP_MSGR_NAT_SYMMETRIC_NAT_V01 ||
          nat_type > QCMAP_MSGR_NAT_ADDRESS_RESTRICTED_NAT_V01 )
    {
      printf("\nInvalid NAT Type : %d", nat_type);
      break;
    }
    if (QcMapClient->SetNatType((qcmap_msgr_nat_enum_v01)nat_type, &qmi_err_num))
    {
      printf("\nNAT Type set successfully");
    }
    else
      printf("\nNAT Type set fail, Error: 0x%x", qmi_err_num);
    break;
  }

  /* Get NAT type */
  case 5:
  {
    qcmap_msgr_nat_enum_v01 nat_type;

    if (QcMapClient->GetNatType(&nat_type, &qmi_err_num))
    {
      switch (nat_type)
      {
        case QCMAP_MSGR_NAT_SYMMETRIC_NAT_V01:
          printf("\n Symmetric NAT \n");
          break;
        case QCMAP_MSGR_NAT_PORT_RESTRICTED_CONE_NAT_V01:
          printf("Port Restricted Cone NAT\n");
          break;
        case QCMAP_MSGR_NAT_FULL_CONE_NAT_V01:
          printf("\nFull Cone NAT\n");
          break;
        case QCMAP_MSGR_NAT_ADDRESS_RESTRICTED_NAT_V01:
          printf("Address Restricted Cone NAT\n");
          break;
        default:
          printf("Invalid NAT Type Returned: 0x%d", nat_type);
          break;
      }
    }
    else
      printf("\nGet NAT type failed, Error: 0x%x", qmi_err_num);

    break;
  }

  /* Set NAT Timeout. */
  case 6:
  { /* Timeout values are global, not tied to a PDN */
    qcmap_msgr_nat_timeout_enum_v01 timeout_type;
    uint32 timeout_value = 0;
    int p_error=0;
    memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);
    printf("Select the Type of Timeout : \n"
           "1: GENRIC TIMEOUT\t2: ICMP TIMEOUT\n"
           "3: TCP TIMEOUT ESTABLISHED\t4: UDP TIMEOUT\t:::");
    fgets(scan_string, sizeof(scan_string), stdin);
    timeout_type = (qcmap_msgr_nat_timeout_enum_v01)atoi(scan_string);
    if ( timeout_type < QCMAP_MSGR_NAT_TIMEOUT_GENERIC_V01 ||
        timeout_type > QCMAP_MSGR_NAT_TIMEOUT_UDP_V01 )
    {
      printf("\nInvalid NAT Timeout Type : %d", timeout_type);
      break;
    }
    printf("\nEnter the Timeout Value (should be >= 30):::");
    fgets(scan_string, sizeof(scan_string), stdin);
    timeout_value = atoi(scan_string);
    if (QcMapClient->SetNatTimeout(timeout_type, timeout_value, &qmi_err_num ))
    {
      printf("\nNAT Timeout Set Successfully\n");
    }
    else
      printf("\nNAT timeout set fails, Error: 0x%x", qmi_err_num);
    break;
  }

  /* Get SNAT Timeout */
  case 7:
  { /* Timeout values are global, not tied to a PDN */
    qcmap_msgr_nat_timeout_enum_v01 timeout_type;
    uint32 timeout_value = 0;
    int p_error=0;
    memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);
    printf("Select the Type of Timeout : \n"
           "1: GENRIC TIMEOUT\t2: ICMP TIMEOUT\n"
           "3: TCP TIMEOUT ESTABLISHED\t4: UDP TIMEOUT\t:::");
    fgets(scan_string, sizeof(scan_string), stdin);
    timeout_type = (qcmap_msgr_nat_timeout_enum_v01)atoi(scan_string);
    if ( timeout_type < QCMAP_MSGR_NAT_TIMEOUT_GENERIC_V01 ||
        timeout_type > QCMAP_MSGR_NAT_TIMEOUT_UDP_V01 )
    {
      printf("\n\nInvalid NAT Timeout Type : %d\n", timeout_type);
      break;
    }
    if (QcMapClient->GetNatTimeout(timeout_type, &timeout_value, &qmi_err_num))
    {
      printf("\n\nNAT Timeout for Type %d : %d\n", timeout_type, timeout_value);
    }
    else
      printf("\nNAT timeout get fails, Error: 0x%x", qmi_err_num);
    break;
  }

  /* Add a DMZ IP and save XML if successful. */
  case 8:
  {
    uint32 dmz_ip=0;

    memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);

    while (TRUE)
    {
      printf("   Please input DMZ IP to add(xxx.xxx.xxx.xxx) : ");
      if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
      {
        if ( !(inet_aton(scan_string, &addr) <=0 ))
          break;
      }
      printf("   Invalid IPv4 address %d\n", scan_string);
    }
    dmz_ip = ntohl(addr.s_addr);

    if (QcMapClient->AddDMZ(dmz_ip, &qmi_err_num))
    {
      printf("\nDMZ IP added successfully");
    }
    else if ( qmi_err_num == QMI_ERR_NO_EFFECT_V01 )
    {
      printf(" DMZ is already configured. Delete the current configuration, if DMZ reconfiguration is needed \n");
    }
    else if ( qmi_err_num == QMI_ERR_INTERFACE_NOT_FOUND_V01 )
    {
      printf("\nBackhaul down. DMZ Entry deleted from xml file.");
    }
    else
      printf("\nDMZ add fails. Error: 0x%x", qmi_err_num);
  }
   break;

  /* Get DMZ IP */
  case 9:
  {
    uint32_t dmz_ip=0;
    in_addr tmpIP;

    if (QcMapClient->GetDMZ(&dmz_ip, &qmi_err_num))
    {
      if ( dmz_ip == 0 )
        printf("\nNo DMZ Configured!");
      else
      {
        tmpIP.s_addr = ntohl(dmz_ip);
        printf("\ndmz ip %s",inet_ntoa(tmpIP));
      }
    }
    else
      printf("\nDMZ get fails. Error: 0x%x", qmi_err_num);
  }
  break;

  /* Delete the current DMZ IP and save XML if successful. */
  case 10:
  {
    if (QcMapClient->DeleteDMZ(&qmi_err_num))
    {
      printf("\nDMZ deleted successfully");
    }
    else
    {
      if (qmi_err_num == QMI_ERR_INTERFACE_NOT_FOUND_V01)
        printf("\nBackhaul down. DMZ deleted from xml file.");
      else
        printf("\nDMZ delete fails. Error: 0x%x", qmi_err_num);
    }
  }
  break;

  /* Enable/disable the IPSEC VPN pass through and save XML if successful. */
  case 11:
  {
    boolean enable;
    memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);

    printf("   Please input IPSEC VPN Pass Through(1-Enable/0-Disable) : ");
    fgets(scan_string, sizeof(scan_string), stdin);
    enable = (atoi(scan_string)) ? true : false;

    if (QcMapClient->SetIPSECVpnPassthrough(enable, &qmi_err_num))
    {
      printf("\nIPSEC VPN passthrough set successfully");
    }
    else
    {
      if (qmi_err_num == QMI_ERR_INTERFACE_NOT_FOUND_V01)
        printf("\nBackhaul down. IPSEC VPN passthrough enabled in xml file.");
      else
        printf("\nIPSEC VPN passthrough set fails. Error: 0x%x", qmi_err_num);
    }
  }
  break;

  /* Get IPSEC VPN Passthrough */
  case 12:
  {
    boolean flag;
    int p_error=0;

    if (QcMapClient->GetIPSECVpnPassthrough(&flag, &qmi_err_num))
    {
      printf("\nIPSEC Passthrough Enable Flag : %d", flag);
    }
    else
      printf("\nIPSEC VPN passthrough get fails. Error: 0x%x", qmi_err_num);
  }
   break;

  /* Enable/disable the PPTP VPN pass through and save XML if successful. */
  case 13:
  {
    boolean enable;
    memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);

    printf("   Please input PPTP VPN Pass Through(1-Enable/0-Disable) : ");
    fgets(scan_string, sizeof(scan_string), stdin);
    enable = (atoi(scan_string)) ? true : false;

    if (QcMapClient->SetPPTPVpnPassthrough(enable, &qmi_err_num))
    {
      printf("\nPPTP Passthrough set successfully");
    }
    else
    {
      if (qmi_err_num == QMI_ERR_INTERFACE_NOT_FOUND_V01)
        printf("\nBackhaul down. PPTP VPN passthrough enabled in xml file.");
      else
        printf("\nPPTP VPN passthrough set fails. Error: 0x%x", qmi_err_num);
    }
  }
  break;

  /* Get PPTP passthrough */
  case 14:
  {
    boolean flag;
    int p_error=0;

    if (QcMapClient->GetPPTPVpnPassthrough(&flag, &qmi_err_num))
    {
      printf("\nPPTP Passthrough Enable Flag : %d", flag);
    }
    else
      printf("\nPPTP VPN passthrough get fails. Error: 0x%x", qmi_err_num);
  }
   break;

  /* Enable/disable the L2TP VPN pass through and save XML if successful*/
  case 15:
  {
    boolean enable;
    memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);

    printf("   Please input L2TP VPN Pass Through(1-Enable/0-Disable) : ");
    fgets(scan_string, sizeof(scan_string), stdin);
    enable = (atoi(scan_string))? true : false;

    if (QcMapClient->SetL2TPVpnPassthrough(enable, &qmi_err_num))
    {
      printf("\nL2TP VPN Passthrough set successfully");
    }
    else
    {
      if (qmi_err_num == QMI_ERR_INTERFACE_NOT_FOUND_V01)
        printf("\nBackhaul down. L2TP VPN passthrough enabled in xml file.");
      else
        printf("\nL2TP VPN passthrough set fails. Error: 0x%x", qmi_err_num);
    }
  }
    break;

  /* Get L2TP passthrough */
  case 16:
  {
    boolean flag;
    int p_error=0;

    if (QcMapClient->GetL2TPVpnPassthrough(&flag, &qmi_err_num))
    {
      printf("\nL2TP Passthrough Enable Flag : %d", flag);
    }
    else
      printf("\nL2TP VPN passthrough get fails. Error: 0x%x", qmi_err_num);
  }
  break;

  /* Enable/Disable ALG. */
  case 17:
  {
   int alg_types = 0 , alg_type =0 ;
   memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);
   //  Add more ALG in menu here
   printf("Select the Type of ALG : \n"
           "1: RTSP ALG \n"
           "2: SIP ALG \n"
           "0: EXIT \n");
   fgets(scan_string, sizeof(scan_string), stdin);
   alg_type = atoi(scan_string);
   while(alg_type != 0)
   {
     // Edit if condition to support more ALGs
     if ( alg_type < QCMAP_MSGR_MASK_RTSP_ALG_V01 ||
          alg_type > QCMAP_MSGR_MASK_SIP_ALG_V01 )
     {
       printf("\nInvalid alg Type : 0x%x", alg_type);
     }
     if ( alg_type == 1 )
        alg_types = alg_types | QCMAP_MSGR_MASK_RTSP_ALG_V01;
     else if(alg_type == 2)
        alg_types = alg_types | QCMAP_MSGR_MASK_SIP_ALG_V01;
     printf("Select the Type of ALG : \n"
             "1: RTSP ALG \n"
             "2: SIP ALG \n"
             "0: CONTINUE \n");
     fgets(scan_string, sizeof(scan_string), stdin);
     alg_type = atoi(scan_string);
   }
   printf(" Please input ALG State (1-Enable/0-Disable) : ");
   fgets(scan_string, sizeof(scan_string), stdin);
   if ( atoi(scan_string) == 1 )
   {
     if ( QcMapClient->EnableAlg((qcmap_msgr_alg_type_mask_v01)alg_types,
                                  &qmi_err_num) )
       printf("\n ALGs 0x%x Enable succeeds.",alg_types);
     else
     {
       if (qmi_err_num == QMI_ERR_INTERFACE_NOT_FOUND_V01)
         printf("\nBackhaul down. ALG 0x%x enabled in xml file.",alg_types);
       else if (qmi_err_num == QMI_ERR_OP_PARTIAL_FAILURE_V01)
         printf("\n Only subset of ALGs enabled, Error:0x%x",qmi_err_num);
       else
         printf("\nALGs 0x%x Enable fails,Error:0x%x", alg_types,
                 qmi_err_num);
     }
   }
   else
   {
     if ( QcMapClient->DisableAlg((qcmap_msgr_alg_type_mask_v01)alg_types,
                                   &qmi_err_num) )
       printf("\nALG Disable Succeeds: 0x%x.", alg_types);
     else
     {
       if (qmi_err_num == QMI_ERR_INTERFACE_NOT_FOUND_V01)
         printf("\nBackhaul down. ALG 0x%x disabled in xml file.",alg_types);
       else if (qmi_err_num == QMI_ERR_OP_PARTIAL_FAILURE_V01)
         printf("\n Only subset of ALGs disabled, Error:0x%x",qmi_err_num);
       else
         printf("\nALGs 0x%x Disable fails,Error:0x%x", alg_types,
                 qmi_err_num);
     }
   }
   break;
  }

  /* Set SIP Server Information */
  case 18:
  {
   qcmap_msgr_sip_server_info_v01 sip_server_info;
   printf("Please input 1-Enter PCSCF IP address 2-Enter PCSCF FQDN");
   fgets(scan_string, sizeof(scan_string), stdin);
   if (atoi(scan_string)== 1)
   {
     memset(scan_string,0,QCMAP_MSGR_MAX_FILE_PATH_LEN);
     while(TRUE)
     {
       printf("Please input the PCSCF IP(xxx.xxx.xxx.xxx)  :");
       if ( fgets(scan_string,sizeof(scan_string),stdin) != NULL )
       {
         if ((inet_aton(scan_string, &addr)) &&
             ( addr.s_addr != 0 ) &&
             ( addr.s_addr != 0xffffffff ))
           break;
       }
       printf("Invalid IPv4 Address entered: %s\n", scan_string);
     }

     sip_server_info.pcscf_ip_addr = addr.s_addr;
     sip_server_info.pcscf_info_type = QCMAP_MSGR_PCSCF_IP_ADDRESS_V01;
   }
   else
   {
     memset(scan_string,0,QCMAP_MSGR_MAX_FILE_PATH_LEN);
     printf("Please input the PCSCF FQDN:");
     if (fgets(scan_string,sizeof(scan_string),stdin) != NULL)
     {
       for (i=0;i < strlen(scan_string)-1;i++)
       {
         sip_server_info.pcscf_fqdn[i] = scan_string[i];
       }
       sip_server_info.pcscf_fqdn[i] ='\0';
     }

     sip_server_info.pcscf_info_type = QCMAP_MSGR_PCSCF_FQDN_V01;
   }

   if(QcMapClient->SetSIPServerInfo(&sip_server_info, &qmi_err_num))
     printf("\n Successfully set SIP server information \n");
   else
     printf("\n Failed to set SIP server information. Error:0x%x \n",
            qmi_err_num);

   break;
  }

  /* Get SIP server Information */
  case 19:
  {
   qcmap_msgr_sip_server_info_v01 default_sip_server_info;
   qcmap_msgr_sip_server_info_v01
   network_sip_server_info[QCMAP_MSGR_MAX_SIP_SERVER_ENTRIES_V01];
   qcmap_msgr_ipv6_sip_server_info_v01
   network_ipv6_sip_server_info[QCMAP_MSGR_MAX_SIP_SERVER_ENTRIES_V01];
   qcmap_msgr_ip_family_enum_v01 ip_family;
   int count_network_sip_server_info=0;
   int cnt;
   in_addr addr;
   char ipv6_addr_buf[INET6_ADDRSTRLEN];

   memset(&default_sip_server_info, 0, sizeof(default_sip_server_info));
   memset(network_sip_server_info, 0, sizeof(network_sip_server_info));
   memset(network_ipv6_sip_server_info, 0, sizeof(network_ipv6_sip_server_info));

   printf("Please input IP Family for SIP server info IPV4-4 IPV6-6 : ");
   fgets(scan_string, sizeof(scan_string), stdin);
   ip_family = (qcmap_msgr_ip_family_enum_v01)atoi(scan_string);

   if (ip_family == QCMAP_MSGR_IP_FAMILY_V4_V01)
   {

     if(QcMapClient->GetSIPServerInfo(&default_sip_server_info,
                                      network_sip_server_info,
                                      &count_network_sip_server_info,
                                      &qmi_err_num))
     {
       if (default_sip_server_info.pcscf_info_type != 0)
       {
         if (default_sip_server_info.pcscf_info_type ==
                                        QCMAP_MSGR_PCSCF_IP_ADDRESS_V01)
         {
           addr.s_addr =default_sip_server_info.pcscf_ip_addr;
           printf("\nDefault PCSCF address: %s \n",inet_ntoa(addr));
         }
         else if (default_sip_server_info.pcscf_info_type ==
                                              QCMAP_MSGR_PCSCF_FQDN_V01)
         {
           printf("\n PCSCF FQDN is %s:\n", default_sip_server_info.pcscf_fqdn);
         }
       }

       if (count_network_sip_server_info > 0)
       {
         printf("\n Number of network assigned SIP server info is %d \n",
                count_network_sip_server_info);

         for (cnt=0; cnt<count_network_sip_server_info; cnt++)
         {
           if (network_sip_server_info[cnt].pcscf_info_type ==
               QCMAP_MSGR_PCSCF_IP_ADDRESS_V01)
           {
             addr.s_addr =network_sip_server_info[cnt].pcscf_ip_addr;
             printf("\%d PCSCF address: %s \n",cnt+1, inet_ntoa(addr));
           }
           else if (network_sip_server_info[cnt].pcscf_info_type ==
                    QCMAP_MSGR_PCSCF_FQDN_V01)
           {
             printf("\n %d PCSCF FQDN is %s:\n",
                    cnt+1,
                    network_sip_server_info[cnt].pcscf_fqdn);
           }
         }
       }
     }
     else
     {
       printf("\n Failed to get IPV4 SIP server info. Error:0x%x \n",
              qmi_err_num);
     }
   }

   else if (ip_family == QCMAP_MSGR_IP_FAMILY_V6_V01)
   {
     if(QcMapClient->GetV6SIPServerInfo(network_ipv6_sip_server_info,
                                        &count_network_sip_server_info,
                                        &qmi_err_num))
     {

       if (count_network_sip_server_info > 0)
       {
         printf("\n Number of network assigned SIP server info is %d \n",
                count_network_sip_server_info);

         for (cnt=0; cnt<count_network_sip_server_info; cnt++)
         {
           if (network_ipv6_sip_server_info[cnt].pcscf_info_type ==
               QCMAP_MSGR_PCSCF_IP_ADDRESS_V01)
           {
             char* ipv6_addr;
             if ( (ipv6_addr = inet_ntop(AF_INET6,
                      (in6_addr *)&network_ipv6_sip_server_info[cnt].pcscf_ipv6_addr,
                      ipv6_addr_buf,sizeof(ipv6_addr_buf))) != NULL)
             {
               printf("\%d IPV6 PCSCF address: %s \n", cnt+1, ipv6_addr);
             }
             else
             {
               printf ("Unable to get IPV6 PCSCF address");
             }
           }
           else if (network_ipv6_sip_server_info[cnt].pcscf_info_type ==
                    QCMAP_MSGR_PCSCF_FQDN_V01)
           {
             printf("\n %d PCSCF FQDN is %s:\n",
                    cnt+1,
                    network_ipv6_sip_server_info[cnt].pcscf_fqdn);
           }
         }
       }
     }
     else
     {
       printf("\n Failed to get IPV6 SIP server info. Error:0x%x \n",
              qmi_err_num);
     }
   }

   else
   {
     printf("\n Failed to get SIP server info: Invalid family\n");
   }
   break;
  }

  case 20:
  {
    int pkt_limit =0;
    while(TRUE)
    {
      printf("Please enter the valid initial packet limit: ");
      fgets(scan_string, sizeof(scan_string), stdin);
      if (atoi(scan_string) < SW_PACKET_THRESHOLD)
      {
        printf("Value less than %d not allowed \n",SW_PACKET_THRESHOLD );
        continue;
      }
      break;
    }
    pkt_limit = atoi(scan_string);
    if(QcMapClient->SetInitialPacketLimit(pkt_limit, &qmi_err_num))
      printf("\n Successfully set initial packet threshold \n");
    else
      printf("\n Failed to set initial packet threshold. Error:0x%x \n",qmi_err_num);

    break;
  }
  case 21:
  {
    int pkt_limit = 0;
    if(QcMapClient->GetInitialPacketLimit(&pkt_limit, &qmi_err_num))
    {
      printf("\n The current initial packet threshold is %d \n",pkt_limit );
    }
    else
      printf("\n Failed to set initial packet threshold. Error:0x%x \n", qmi_err_num);

    break;
  }

  /* Enable/Disable the SOCKSv5 Proxy and save XML if successful. */
  case 22:
  {
    if(!(EnableDisableSOCKSv5(&qmi_err_num)))
    {
      printf("\n Failed to EnableDisable SOCKSv5. Error:0x%x\n", qmi_err_num);
    }
    break;
  }

  /* Set the SOCKSv5 Proxy Config params and save config if successful. */
  case 23:
  {
    if(!(SetSOCKSv5Config(&qmi_err_num)))
    {
      printf("\n Failed to Set SOCKSv5 Config. Error:0x%x\n", qmi_err_num);
    }
    break;
  }

  case 24:
  {
    socksv5_configuration configuration;

    memset(&configuration, 0, sizeof(socksv5_configuration));

    if(!(QcMapClient->GetSOCKSv5Config(&configuration, &qmi_err_num)))
    {
      printf("\nFailed to print SOCKSv5 config, Error: 0x%x", qmi_err_num);
    } else
    {
      //print configuration file locations
      printf("Conf File Location: %s\n", configuration.config_file_paths.conf_file);
      printf("Auth File Location: %s\n", configuration.config_file_paths.auth_file);

      //print auth method
      switch(configuration.auth_method)
      {
        case QCMAP_SOCKSV5_NO_AUTHENTICATION_V01:
        {
          printf("SOCKSv5 Authentication Method: No Authentication\n");
          break;
        }
        case QCMAP_SOCKSV5_UNAME_PASSWD_V01:
        {
          printf("SOCKSv5 Authentication Method: Username / Password\n");
          break;
        }
        default:
        {
          printf("SOCKSv5 Authentication Method: %u\n", configuration.auth_method);
          break;
        }
      }

      //print lan_iface
      printf("SOCKSv5 LAN Interface: %s\n", configuration.lan_iface);

      //print wan_ifaces with corresponding profile/service no
      for(int i = 0; i < QCMAP_MAX_NUM_BACKHAULS_V01; i++)
      {
        //if "" string then no wan_iface put into the wan_service struct
        if(strcmp(configuration.wan_service[i].wan_iface, "") != 0)
        {
          printf("SOCKSv5 WAN Interface: %s, Service/Profile#: %u\n",
          configuration.wan_service[i].wan_iface,
          configuration.wan_service[i].service_no);
        }
      }
    }
    break;
  }
  default :
  {
   printf("Invalid response %d\n", natAlgOpt);
  }
   break;
  }
}

/*===========================================================================
  FUNCTION EnableDisableSOCKSv5
  ===========================================================================*/
/*!
  @brief
  Enables or Disables SOCKSv5 Proxy based on user input

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
static boolean EnableDisableSOCKSv5(qmi_error_type_v01 *qmi_err_num)
{
  char scan_string[QCMAP_MSGR_MAX_FILE_PATH_LEN];

  if(NULL == QcMapClient)
  {
    LOG_MSG_INFO1("QCMAP Client object NULL", 0, 0, 0);
    return false;
  }

  memset(scan_string, 0, sizeof(scan_string));
  printf("Please input SOCKSv5 Proxy State(1-Enable/0-Disable) : ");
  fgets(scan_string, sizeof(scan_string) - 1, stdin);

  switch(atoi(scan_string))
  {
    case 0:
    {
      if(QcMapClient->DisableSOCKSv5Proxy(qmi_err_num))
      {
        printf("\nDisabled SOCKSv5 Proxy", 0, 0, 0);
        return true;
      } else
      {
        LOG_MSG_INFO1("Disable SOCKSv5 Proxy fails, Error: 0x%x", *qmi_err_num, 0, 0);
      }
      break;
    }

    case 1:
    {
      if(QcMapClient->EnableSOCKSv5Proxy(qmi_err_num))
      {
        printf("\nEnabled SOCKSv5 Proxy", 0, 0, 0);
        return true;
      } else
      {
        LOG_MSG_INFO1("Enable SOCKSv5 Proxy fails, Error: 0x%x", *qmi_err_num, 0, 0);
      }
      break;
    }

    default:
    {
      printf("Invalid SOCKSv5 Proxy State response: %u\n", scan_string, 0, 0);
    }
  }

  return false;
}

/*===========================================================================
  FUNCTION SetSOCKSv5Config
  ===========================================================================*/
/*!
  @brief
  Sets SOCKSv5 Proxy config based on user input

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
static boolean SetSOCKSv5Config(qmi_error_type_v01 *qmi_err_num)
{
  char scan_string[QCMAP_MSGR_MAX_FILE_PATH_LEN];
  boolean retval = false;
  qcmap_socksv5_config_type_v01 config_type;

  if(NULL == QcMapClient)
  {
    LOG_MSG_INFO1("QCMAP Client object NULL", 0, 0, 0);
    return false;
  }

  memset(scan_string, 0, sizeof(scan_string));
  printf("1. Set SOCKSv5 Proxy Config File Path\n");
  printf("2. Set SOCKSv5 Proxy Authentication Method\n");
  printf("3. Edit LAN Interface\n");
  printf("4. Add/Delete Username/Profile Association\n");
  fgets(scan_string, sizeof(scan_string) - 1, stdin);

  switch(atoi(scan_string))
  {
    case 1:
    {
      config_type = QCMAP_MSGR_SOCKSV5_SET_CONFIG_FILE_PATH_V01;
      qcmap_msgr_socksv5_config_file_paths_v01 config_file_paths;

      memset(&config_file_paths, 0, sizeof(config_file_paths));

      if(!(GetUserInputSOCKSv5ConfigFilePath(config_file_paths.conf_file,
                                             config_file_paths.auth_file)) ||
           !(retval = QcMapClient->SetSOCKSv5Config(&config_file_paths, config_type, qmi_err_num)))
      {
        LOG_MSG_INFO1("Failed to set SOCKSv5 Proxy Config File Path, Error: 0x%x",
                      *qmi_err_num, 0, 0);
      }
      break;
    }

    case 2:
    {
      unsigned char auth_method;
      config_type = QCMAP_MSGR_SOCKSV5_SET_AUTH_METHOD_V01;
      if(!(GetUserInputSOCKSv5AuthMethod(&auth_method)) ||
         !(retval = QcMapClient->SetSOCKSv5Config(&auth_method, config_type, qmi_err_num)))
      {
        LOG_MSG_INFO1("Failed to set SOCKSv5 Proxy Auth, Error: 0x%x", *qmi_err_num, 0, 0);
      }
      break;
    }

    case 3:
    {
      char lan_iface[IFNAMSIZ];
      memset(lan_iface, 0, IFNAMSIZ * sizeof(char));

      config_type = QCMAP_MSGR_SOCKSV5_EDIT_LAN_IFACE_V01;
      if(!(GetUserInputSOCKSv5LANIface(lan_iface)) ||
         !(retval = QcMapClient->SetSOCKSv5Config(lan_iface, config_type, qmi_err_num)))
      {
        LOG_MSG_INFO1("Failed to edit SOCKSv5 LAN iface, Error: 0x%x", *qmi_err_num, 0, 0);
      }
      break;
    }

    case 4:
    {
      retval = PromptUserSOCKSv5UnameAssoc(qmi_err_num);
      break;
    }

    default:
    {
      printf("Invalid set SOCKSv5 response: %u\n", scan_string, 0, 0);
      retval = false;
      break;
    }
  }

  return retval;

}

/*===========================================================================
  FUNCTION GetUserInputSOCKSv5AuthMethod
  ===========================================================================*/
/*!
  @brief
  Gets SOCKSv5 Proxy authentication method based on user input

  @params
  ptr to unsigned char to hold the authentication method

  @return
   true - valid user input
   false - otherwise

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
static boolean GetUserInputSOCKSv5AuthMethod(unsigned char *auth_method)
{
  char scan_string[QCMAP_MSGR_MAX_FILE_PATH_LEN];

  memset(scan_string, 0, sizeof(scan_string));
  printf("Please Enter SOCKSv5 Proxy Authentication Method "
         "(0- No Authentication, 1 - Username/Password): ");
  fgets(scan_string, sizeof(scan_string) - 1, stdin);

  switch(atoi(scan_string))
  {
    case 0:
    {
      *auth_method = QCMAP_SOCKSV5_NO_AUTHENTICATION_V01;
      break;
    }

    case 1:
    {
      *auth_method = QCMAP_SOCKSV5_UNAME_PASSWD_V01;
      break;
    }

    default:
    {
      printf("Invalid set SOCKSv5 auth response: %s\n", scan_string);
      return false;
      break;
    }
  }

  return true;
}

/*===========================================================================
  FUNCTION GetUserInputSOCKSv5LANIface
  ===========================================================================*/
/*!
  @brief
  Gets SOCKSv5 Proxy lan iface based on user input

  @params
  ptr to hold the lan iface name

  @return
  true - valid lan iface name
  false - otherwise

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
static boolean GetUserInputSOCKSv5LANIface(char *lan_iface)
{
  char scan_string[QCMAP_MSGR_MAX_FILE_PATH_LEN];

  memset(scan_string, 0, sizeof(scan_string));
  printf("Please Enter new SOCKSv5 Proxy LAN iface: ");
  fgets(scan_string, sizeof(scan_string) - 1, stdin);

  //bounds check
  if(strlen(scan_string) > IFNAMSIZ)
  {
    printf("Linux Kernel can't allow iface name size > %u\n", IFNAMSIZ);
    return false;
  }

  for(int i = 0; i < strlen(scan_string); i++)
  {
    if(strcmp(&scan_string[i], "\n") != 0)
    {
      lan_iface[i] = scan_string[i];
    }
  }

  //Can't have "" empty string inputs
  if(strcmp(lan_iface, "") == 0)
  {
    printf("Invalid LAN iface name, empty string given\n");
    return false;
  }

  return true;
}

/*===========================================================================
  FUNCTION GetUserInputSOCKSv5ConfigFilePath
  ===========================================================================*/
/*!
  @brief
  Gets SOCKSv5 Proxy config file paths based on user input

  @params
  ptr to hold qti_socksv5_conf.xml file path
  ptr to hold qti_socksv5_auth.xml file path

  @return
  true - valid user input
  false - otherwise

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
static boolean GetUserInputSOCKSv5ConfigFilePath(char* conf_file, char* auth_file)
{
  char scan_string[QCMAP_MSGR_MAX_FILE_PATH_LEN];

  memset(scan_string, 0, sizeof(scan_string));

  //get conf file path
  printf("Please Enter SOCKSv5 Configuration File Path: ");
  fgets(scan_string, sizeof(scan_string) - 1, stdin);

  //bounds check
  if(strlen(scan_string) > QCMAP_MSGR_MAX_FILE_PATH_LEN)
  {
    printf("Can't allow path name size > %u\n", QCMAP_MSGR_MAX_FILE_PATH_LEN);
    return false;
  }

  for(int i = 0; i < strlen(scan_string); i++)
  {
    if(strcmp(&scan_string[i], "\n") != 0)
    {
      conf_file[i] = scan_string[i];
    }
  }

  //get auth file path
  memset(scan_string, 0, sizeof(scan_string));
  printf("Please Enter SOCKSv5 Authentication File Path: ");
  fgets(scan_string, sizeof(scan_string) - 1, stdin);

  //bounds check
  if(strlen(scan_string) > QCMAP_MSGR_MAX_FILE_PATH_LEN)
  {
    printf("Can't allow path name size > %u\n", QCMAP_MSGR_MAX_FILE_PATH_LEN);
    return false;
  }

  for(int i = 0; i < strlen(scan_string); i++)
  {
    if(strcmp(&scan_string[i], "\n") != 0)
    {
      auth_file[i] = scan_string[i];
    }
  }

  //Can't have "" empty string inputs
  if((strcmp(conf_file, "") == 0) || (strcmp(auth_file, "") == 0))
  {
    printf("Invalid confguration file path, empty string given\n");
    return false;
  }

  return true;

}

/*===========================================================================
  FUNCTION PromptUserSOCKSv5UnameAssoc
  ===========================================================================*/
/*!
  @brief
  Prompts user for add/edit/delete configurations for SOCKSv5 Proxy service's
  uname wan service mapping

  @params
  ptr to uname_wan_assoc to store user input

  @return
  true - valid user input
  false - otherwise

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
static boolean PromptUserSOCKSv5UnameAssoc(qmi_error_type_v01 *qmi_err_num)
{
  qcmap_msgr_socksv5_uname_assoc_v01 uname_assoc;
  boolean retval = false;
  char scan_string[QCMAP_MSGR_MAX_FILE_PATH_LEN];
  qcmap_socksv5_config_type_v01 config_type;

  memset(&uname_assoc, 0, sizeof(qcmap_msgr_socksv5_uname_assoc_v01));
  memset(scan_string, 0, sizeof(scan_string));

  printf("(0-Add/1-Delete) : ");
  fgets(scan_string, sizeof(scan_string) - 1, stdin);

  switch(atoi(scan_string))
  {
    case 0:
    {
      config_type = QCMAP_MSGR_SOCKSV5_ADD_UNAME_ASSOC_V01;
      if(!(GetUserInputSOCKSv5Uname(uname_assoc.uname)) ||
         !(GetUserInputSOCKSv5ServiceNo(&(uname_assoc.service_no))) ||
         !(retval = QcMapClient->SetSOCKSv5Config(&uname_assoc, config_type, qmi_err_num)))
      {
        LOG_MSG_INFO1("Failed to add SOCKSv5 uname/assoc, Error: 0x%x", *qmi_err_num, 0, 0);
      }
      break;
    }

    case 1:
    {
      config_type = QCMAP_MSGR_SOCKSV5_DELETE_UNAME_ASSOC_V01;
      if(!(GetUserInputSOCKSv5Uname(uname_assoc.uname)) ||
         !(retval = QcMapClient->SetSOCKSv5Config(uname_assoc.uname, config_type, qmi_err_num)))
      {
        LOG_MSG_INFO1("Failed to delete SOCKSv5 uname/assoc, Error: 0x%x",
                       *qmi_err_num, 0, 0);
      }
      break;
    }

    default:
    {
      printf("Invalid SOCKSv5 uname assoc response: %u\n", scan_string);
      retval = false;
      break;
    }
  }

  return retval;
}

/*===========================================================================
  FUNCTION GetUserInputSOCKSv5Uname
  ===========================================================================*/
/*!
  @brief
  Gets SOCKSv5 Proxy uname based on user input

  @params
  ptr to uname

  @return
  true - valid ulen and plen
  false - otherwise

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
static boolean GetUserInputSOCKSv5Uname(char* uname)
{
  char scan_string[QCMAP_SOCKSV5_MAX_UNAME_PASSWD_LEN_V01 + 2];

  memset(uname, 0, QCMAP_SOCKSV5_MAX_UNAME_PASSWD_LEN_V01);
  memset(scan_string, 0, sizeof(scan_string));

  printf("Please Enter Username: ");
  fgets(scan_string, sizeof(scan_string) - 1, stdin);

  //bounds check
  if(!(CheckSOCKSv5UnameLen(scan_string)))
  {
    return false;
  }
  for(int i = 0; i < strlen(scan_string); i++)
  {
    if(strcmp(&scan_string[i], "\n") != 0)
    {
      uname[i] = scan_string[i];
    }
  }

  //Can't have "" empty string inputs
  if(strcmp(uname, "") == 0)
  {
    printf("Invalid username empty string given\n");
    return false;
  }

  return true;
}

/*===========================================================================
  FUNCTION GetUserInputSOCKSv5ServiceNo
  ===========================================================================*/
/*!
  @brief
  Gets SOCKSv5 Proxy service no based on user input

  @params
  ptr to service no to store user input

  @return
  true - valid user input
  false - otherwise

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
static boolean GetUserInputSOCKSv5ServiceNo(unsigned int* service_no)
{
  char scan_string[QCMAP_MSGR_MAX_FILE_PATH_LEN];

  memset(scan_string, 0, sizeof(scan_string));
  printf("Please Enter Service/Profile#: ");
  fgets(scan_string, sizeof(scan_string) - 1, stdin);

  //no empty string
  if(strcmp(scan_string, "\n") == 0)
  {
    printf("Can't have empty string as service/profile #\n");
    return false;
  }

  if(atoi(scan_string) >= 0)
  {
    *service_no = atoi(scan_string);
    return true;
  }

  printf("Invalid service no: %s\n", scan_string);

  return false;
}

/*===========================================================================
  FUNCTION CheckSOCKSv5UnamePasswdLen
  ===========================================================================*/
/*!
  @brief
  Checks SOCKSv5 Proxy Username and Password Len

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
static boolean CheckSOCKSv5UnameLen(char* str)
{
  //bounds check
  if((strlen(str) > QCMAP_SOCKSV5_MAX_UNAME_PASSWD_LEN_V01 + 1 &&
      str[strlen(str) - 1] == '\n') || (str[strlen(str) - 1] != '\n' &&
      strlen(str) > QCMAP_SOCKSV5_MAX_UNAME_PASSWD_LEN_V01))
  {
    printf("RFC 1929 can't allow uname len > %u\n", QCMAP_SOCKSV5_MAX_UNAME_PASSWD_LEN_V01);
    return false;
  }
  return true;
}

void wlanConfig( int wlanOpt )
{
  qmi_error_type_v01 qmi_err_num;
  /* Initialize QMI Error Number. */
  qmi_err_num = QMI_ERR_NONE_V01;
  char scan_string[QCMAP_MSGR_MAX_FILE_PATH_LEN];
  qcmap_msgr_wlan_if_info_t wlan_info_cfg;
  memset(&wlan_info_cfg, 0, sizeof(qcmap_msgr_wlan_if_info_t));
  in_addr addr;

  /* WLAN configuration options */
  switch(wlanOpt)
  {
   /* Enable/Disable WLAN */
   case 1:
   {
     printf("   Please input WLAN State(1-Enable/0-Disable) : ");
     fgets(scan_string, sizeof(scan_string), stdin);
     if (atoi(scan_string))
     {
       if(QcMapClient->EnableWLAN(&qmi_err_num))
       {
         printf("\nEnabled WLAN");
       }
       else
         printf("\nEnable WLAN fails, Error: 0x%x", qmi_err_num);
     }
     else
     {
       if(QcMapClient->DisableWLAN(&qmi_err_num))
       {
         printf("\nDisabled WLAN");
       }
       else
         printf("\nDisable WLAN fails, Error: 0x%x", qmi_err_num);
     }
     break;
   }

   /* Activate WLAN */
   case 2:
   {
     if(QcMapClient->ActivateWLAN(&qmi_err_num))
     {
       printf("\nActivated WLAN\n");
     }
     else
       printf("\nFailed to Activate WLAN, Error: 0x%x", qmi_err_num);
     break;
   }

   /* Set WLAN config */
   case 3:
   {
     qcmap_msgr_wlan_mode_enum_v01 wlan_mode;
     qcmap_msgr_access_profile_v01 guest_ap_access_profile =
       QCMAP_MSGR_ACCESS_PROFILE_MIN_ENUM_VAL_V01;
     qcmap_msgr_access_profile_v01 guest_ap_2_access_profile =
       QCMAP_MSGR_ACCESS_PROFILE_MIN_ENUM_VAL_V01;
     qcmap_msgr_access_profile_v01 guest_ap_3_access_profile =
       QCMAP_MSGR_ACCESS_PROFILE_MIN_ENUM_VAL_V01;
     qcmap_msgr_guest_profile_config_v01 guest_profile;
     int p_error=0;
     qcmap_msgr_station_mode_config_v01 station_config;
     in_addr addr;
     memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);
     memset(&station_config,0,sizeof(qcmap_msgr_station_mode_config_v01));

     printf("Select the Type of WLAN Mode : \n"
             "1: AP        2: AP-AP\n"
             "3: AP-STA    4: AP-AP-AP\n"
             "5: AP-AP-STA 6: STA-Only \n"
             "7: AP-AP-AP-AP:::\n"
             "11: AP-P2P    12: STA-P2P \n");
     while (TRUE ) {
         fgets(scan_string, sizeof(scan_string), stdin);
         wlan_mode = (qcmap_msgr_wlan_mode_enum_v01)atoi(scan_string);
         if ( wlan_mode < QCMAP_MSGR_WLAN_MODE_AP_V01 ||
              wlan_mode > QCMAP_MSGR_WLAN_MODE_STA_P2P_V01 ){
         printf("\nPlease enter a valid WLAN Mode\t:::");
         continue;
        }
        break;
     }

     printf(" Do you want to change Guest AP 1 Access Profile 1-Yes/0-No\n");
     fgets(scan_string, sizeof(scan_string), stdin);
     if (atoi(scan_string)) {
        printf(" Select guest AP 1 access profile 1-Full Access / 0- Internet Only \n");
        fgets(scan_string, sizeof(scan_string), stdin);
        if (atoi(scan_string) == 1) {
           guest_ap_access_profile = QCMAP_MSGR_PROFILE_FULL_ACCESS_V01;
        } else {
           guest_ap_access_profile = QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01;
        }
     }

     printf(" Do you want to change Guest AP 2 Access Profile 1-Yes/0-No\n");
     fgets(scan_string, sizeof(scan_string), stdin);
     if (atoi(scan_string)) {
        printf(" Select guest AP 2 access profile 1-Full Access / 0- Internet Only \n");
        fgets(scan_string, sizeof(scan_string), stdin);
        if (atoi(scan_string) == 1) {
           guest_ap_2_access_profile = QCMAP_MSGR_PROFILE_FULL_ACCESS_V01;
        } else {
           guest_ap_2_access_profile = QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01;
        }
     }

     if(wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01)
     {
       printf(" Do you want to change Guest AP 3 Access Profile 1-Yes/0-No\n");
       fgets(scan_string, sizeof(scan_string), stdin);
       if (atoi(scan_string)) {
         printf(" Select guest AP 3 access profile 1-Full Access / 0- Internet Only \n");
         fgets(scan_string, sizeof(scan_string), stdin);
         if (atoi(scan_string) == 1) {
           guest_ap_3_access_profile = QCMAP_MSGR_PROFILE_FULL_ACCESS_V01;
         }
         else {
           guest_ap_3_access_profile = QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01;
         }
       }
     }

     guest_profile.guest_ap_profile[QCMAP_MSGR_GUEST_AP_ONE_V01] = guest_ap_access_profile;
     guest_profile.guest_ap_profile[QCMAP_MSGR_GUEST_AP_TWO_V01] = guest_ap_2_access_profile;
     guest_profile.guest_ap_profile[QCMAP_MSGR_GUEST_AP_THREE_V01] = guest_ap_3_access_profile;

     printf("   Do you want to set Station Configuration(1-Yes/0-No) : ");
     fgets(scan_string, sizeof(scan_string), stdin);
     if ( atoi(scan_string) == 1)
     {
       printf("   Please input Connection Type, 1 for DYNAMIC/2 for STATIC:");
       fgets(scan_string, sizeof(scan_string), stdin);
       if (atoi(scan_string) == 2)
       {
         station_config.conn_type = QCMAP_MSGR_STA_CONNECTION_STATIC_V01;
         while (TRUE)
         {
           printf("   Please input a valid Static IP address:");
           if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
           {
             memset(&addr,0,sizeof(in_addr));
             if (inet_aton(scan_string, &addr))
             {
               station_config.static_ip_config.ip_addr = ntohl(addr.s_addr);
               break;
             }
           }
           printf("      Invalid IPv4 address %s", scan_string);
         }
         while (TRUE)
         {
           printf("   Please input a valid Gateway address:");
           if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
           {
             memset(&addr,0,sizeof(in_addr));
             if (inet_aton(scan_string, &addr))
             {
               station_config.static_ip_config.gw_ip = ntohl(addr.s_addr);
               break;
             }
           }
           printf("      Invalid IPv4 address %s", scan_string);
         }
         while (TRUE)
         {
           printf("   Please input a valid Netmask:");
           if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
           {
             memset(&addr,0,sizeof(in_addr));
             if (inet_aton(scan_string, &addr))
             {
               station_config.static_ip_config.netmask = ntohl(addr.s_addr);
               break;
             }
           }
           printf("      Invalid IPv4 address %s", scan_string);
         }
         while (TRUE)
         {
           printf("   Please input a valid DNS Address:");
           if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
           {
             memset(&addr,0,sizeof(in_addr));
             if (inet_aton(scan_string, &addr))
             {
               station_config.static_ip_config.dns_addr = ntohl(addr.s_addr);
               break;
             }
           }
           printf("      Invalid IPv4 address %s", scan_string);
         }
       }
       else
       {
         station_config.conn_type = QCMAP_MSGR_STA_CONNECTION_DYNAMIC_V01;
       }
       printf(" Please Select AP+STA/AP+AP+STA Mode, 0-RouterMode / 1-BridgeMode:");
       fgets(scan_string, sizeof(scan_string), stdin);
       station_config.ap_sta_bridge_mode = (atoi(scan_string) ? 1:0);
   }

     if (QcMapClient->SetWLANConfig(wlan_mode, guest_ap_access_profile, station_config, &qmi_err_num, &guest_profile))
     {
       if (qmi_err_num ==  QMI_ERR_NONE_V01) {
         printf("\n LAN Config Set Successfully\n");
       } else {
           if (qmi_err_num == QMI_ERR_INVALID_ARG_V01) {
             printf("SSID1 (AP Mode) DHCP Address Range provided is invalid, Minimum range is 7 \n");
             printf("Setting AP DHCP address to default values which are derived from AP Gateway Addr \n");
           } else {
             printf("\nWLAN Config set fails, Error: 0x%x", qmi_err_num);
           }
       }
     }
     else
     {
       printf("\nWLAN Config set fails, Error: 0x%x", qmi_err_num);
     }
     break;
   }

   /* Get WLAN Config */
   case 4:
   {
     qcmap_msgr_wlan_mode_enum_v01 wlan_mode;
     qcmap_msgr_access_profile_v01 guest_ap_access_profile;
     qcmap_msgr_guest_profile_config_v01 guest_ap_config;
     qcmap_msgr_station_mode_config_v01 station_config;
     in_addr addr;
     int p_error=0;
     memset(&station_config,0,sizeof(qcmap_msgr_station_mode_config_v01));
     memset(&addr, 0 ,sizeof(in_addr));
     memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);
     if (QcMapClient->GetWLANConfig(&wlan_mode, &guest_ap_access_profile, &station_config,&qmi_err_num, &guest_ap_config))
     {
       switch (wlan_mode)
       {
         case QCMAP_MSGR_WLAN_MODE_AP_V01:
           printf("\nConfigured LAN Mode is AP.\n");
           break;
         case QCMAP_MSGR_WLAN_MODE_AP_AP_V01:
           printf("\nConfigured LAN Mode is AP-AP.\n");
              break;
         case QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01:
           printf("\nConfigured LAN Mode is AP-AP-AP.\n");
           break;
         case QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01:
           printf("\nConfigured LAN Mode is AP-AP-AP-AP.\n");
           break;
         case QCMAP_MSGR_WLAN_MODE_AP_STA_V01:
           printf("\nConfigured LAN Mode is AP-STA.\n");
           break;
         case QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01:
           printf("\nConfigured LAN Mode is AP-AP-STA.\n");
           break;
         case QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01:
           printf("\nConfigured LAN Mode is STA.\n");
           break;
         case QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01:
           printf("\n LAN is not configured.\n");
           break;
         case QCMAP_MSGR_WLAN_MODE_AP_P2P_V01:
           printf("\n Configured LAN Mode is AP-P2P.\n");
           break;
         case QCMAP_MSGR_WLAN_MODE_STA_P2P_V01:
           printf("\n Configured LAN Mode is STA-P2P.\n");
           break;
         default:
           printf("\nUnsupported LAN Mode:- %d.\n", wlan_mode);
           break;
       }

       // Guest AP 1 Access profile
       printf("\n Guest AP 1 is setup to be in '%s' mode \n",
              (guest_ap_config.guest_ap_profile[QCMAP_MSGR_GUEST_AP_ONE_V01] == QCMAP_MSGR_PROFILE_FULL_ACCESS_V01)?
              "Full Access" : "Internet Only Access");

       // Guest AP 2 Access profile
       printf("\n Guest AP 2 is setup to be in '%s' mode \n",
              (guest_ap_config.guest_ap_profile[QCMAP_MSGR_GUEST_AP_TWO_V01] == QCMAP_MSGR_PROFILE_FULL_ACCESS_V01)?
              "Full Access" : "Internet Only Access");

       // Guest AP 3 Access profile
       printf("\n Guest AP 3 is setup to be in '%s' mode \n",
              (guest_ap_config.guest_ap_profile[QCMAP_MSGR_GUEST_AP_THREE_V01] == QCMAP_MSGR_PROFILE_FULL_ACCESS_V01)?
              "Full Access" : "Internet Only Access");

       /* Print Staton Configuration. */
       printf("\nStation Configuration.\n");
       if ( station_config.conn_type == QCMAP_MSGR_STA_CONNECTION_DYNAMIC_V01)
         printf("\nConnection Type : DYNAMIC (DHCP)\n");
       else
       {
         printf("\nConnection Type : STATIC\n");
       }
       printf("\n STATIC STA IP Configuration\n");
       addr.s_addr = htonl(station_config.static_ip_config.ip_addr);
       printf("\nIP Address: %s\n", inet_ntoa(addr));
       addr.s_addr = htonl(station_config.static_ip_config.gw_ip);
       printf("\nGateway IP : %s\n", inet_ntoa(addr));
       addr.s_addr = htonl(station_config.static_ip_config.netmask);
       printf("\nNetmask: %s\n", inet_ntoa(addr));
       addr.s_addr = htonl(station_config.static_ip_config.dns_addr);
       printf("\nDNS Address : %s\n", inet_ntoa(addr));
       printf("\nSTA is configured in %s \n",(station_config.ap_sta_bridge_mode?"Bridge Mode":"Router Mode"));
     }
     else
     {
       printf("\nGet LAN Config failed, Error:0x%x", qmi_err_num);
     }
     break;
   }

   /* Get WLAN Status */
   case 5:
   {
     qcmap_msgr_wlan_mode_enum_v01 wlan_mode;
     int p_error=0;
     memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);
     if (QcMapClient->GetWLANStatus(&wlan_mode, &qmi_err_num))
     {
       switch (wlan_mode)
          {
            case QCMAP_MSGR_WLAN_MODE_AP_V01:
              printf("\nCurrent WLAN Mode is AP.\n");
              break;
            case QCMAP_MSGR_WLAN_MODE_AP_AP_V01:
              printf("\nCurrent WLAN Mode is AP-AP.\n");
              break;
            case QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01:
              printf("\nCurrent WLAN Mode is AP-AP-AP.\n");
              break;
            case QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01:
              printf("\nCurrent WLAN Mode is AP-AP-AP-AP.\n");
              break;
            case QCMAP_MSGR_WLAN_MODE_AP_STA_V01:
              printf("\nCurrent WLAN Mode is AP-STA Router Mode.\n");
              break;
            case QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01:
              printf("\nCurrent WLAN Mode is AP-AP-STA Router Mode.\n");
              break;
            case QCMAP_MSGR_WLAN_MODE_AP_STA_BRIDGE_V01:
              printf("\nCurrent WLAN Mode is AP-STA Bridge Mode.\n");
              break;
            case QCMAP_MSGR_WLAN_MODE_AP_AP_STA_BRIDGE_V01:
              printf("\nCurrent WLAN Mode is AP-AP-STA Bridge Mode.\n");
              break;
            case QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01:
              printf("\nCurrent WLAN Mode is STA Router Mode.\n");
              break;
            case QCMAP_MSGR_WLAN_MODE_STA_ONLY_BRIDGE_V01:
              printf("\nCurrent WLAN Mode is STA Bridge Mode.\n");
              break;
            case QCMAP_MSGR_WLAN_MODE_AP_P2P_V01:
              printf("\n Current WLAN Mode is AP-P2P Mode.\n");
              break;
            case QCMAP_MSGR_WLAN_MODE_STA_P2P_V01:
              printf("\nCurrent WLAN Mode is STA-P2P Mode.\n");
              break;
            case QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01:
              printf("\nWLAN is not UP.\n");
              break;
            default:
              printf("\nUnsupported WLAN Mode:- %d.\n", wlan_mode);
              break;
          }
     }
     else
       printf("\nGet WLAN Status failed, Error: 0x%x", qmi_err_num);
     break;
   }

   /* Set MobileAP/WLAN Bootup Config */
   case 6:
   {
     qcmap_msgr_bootup_flag_v01 mobileap_enable, wlan_enable;
     printf(" Do you want to change MobileAP bootup configuration (1- Change /0- Do not Change) : ");
     fgets(scan_string, sizeof(scan_string), stdin);
     if (atoi(scan_string)) {
        printf(" Do you want to enable MobileAP on bootup (1-Enable/0-Disable) : ");
        fgets(scan_string, sizeof(scan_string), stdin);
        if (atoi(scan_string)) {
           mobileap_enable = QCMAP_MSGR_ENABLE_ON_BOOT_V01;
        } else {
           mobileap_enable = QCMAP_MSGR_DISABLE_ON_BOOT_V01;
        }
     } else {
        mobileap_enable = QCMAP_MSGR_BOOTUP_FLAG_MIN_ENUM_VAL_V01;
     }
     printf(" Do you want to change WLAN bootup configuration (1- Change /0- Do not Change) : ");
     fgets(scan_string, sizeof(scan_string), stdin);
     if (atoi(scan_string)) {
        printf(" Do you want to enable WLAN on bootup (1-Enable/0-Disable) : ");
        fgets(scan_string, sizeof(scan_string), stdin);
        if (atoi(scan_string)) {
           wlan_enable = QCMAP_MSGR_ENABLE_ON_BOOT_V01;
        } else {
           wlan_enable = QCMAP_MSGR_DISABLE_ON_BOOT_V01;
        }
     } else {
        wlan_enable = QCMAP_MSGR_BOOTUP_FLAG_MIN_ENUM_VAL_V01;
     }
     if (QcMapClient->SetQCMAPBootupCfg(mobileap_enable, wlan_enable, &qmi_err_num)){
         printf("\n QCMAP Bootup configuration set successfully set \n");
     } else {
         printf("\n  Set QCMAP Bootup Cfg Fails , Error: 0x%x \n", qmi_err_num);
     }
     break;
   }

   /* Get MobileAP/WLAN Bootup config */
   case 7:
   {
     qcmap_msgr_bootup_flag_v01 mobileap_enable, wlan_enable;
     if (QcMapClient->GetQCMAPBootupCfg(&mobileap_enable, &wlan_enable, &qmi_err_num)){
         printf("\n Mobile AP will be %s on bootup \n",((mobileap_enable == QCMAP_MSGR_ENABLE_ON_BOOT_V01) ?"ENABLED":"DISABLED"));
         printf("\n WLAN will be %s on bootup \n",((wlan_enable == QCMAP_MSGR_ENABLE_ON_BOOT_V01)?"ENABLED":"DISABLED"));
     } else {
         printf("\n  Get QCMAP Bootup Cfg Fails , Error: 0x%x \n", qmi_err_num);
     }
     break;
   }

   /* Get station mode status. */
   case 8:
   {
     qcmap_msgr_station_mode_status_enum_v01 status;
     if (QcMapClient->GetStationModeStatus(&status, &qmi_err_num))
     {
       if (status == QCMAP_MSGR_STATION_MODE_CONNECTED_V01)
         printf("\nMobile AP Station Mode is Connected");
       else if(status == QCMAP_MSGR_STATION_MODE_DISCONNECTED_V01)
         printf("\nMobile AP Station Mode is Disconnected");
       else
         printf("\nIncorrect state returned: 0x%x", status);
     }
     else
       printf("  Failed to Get Station Mode Status .Error 0x%x.\n ", qmi_err_num);
   }
   break;

   /* Activate Hostapd with the new config. */
   case 9:
   {
     qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type;
     qcmap_msgr_activate_hostapd_action_enum_v01 action_type;
     memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);
     printf("Select the AP Type for which settings need to be activated : \n"
               "1: Primary AP   2: Guest AP\n"
               "3: Guest 2 AP  4. Guest 3 AP \n5: All AP's\t:::");
     fgets(scan_string, sizeof(scan_string), stdin);
     ap_type = (qcmap_msgr_activate_hostapd_ap_enum_v01)atoi(scan_string);
     if ( ap_type < QCMAP_MSGR_PRIMARY_AP_V01 ||
          ap_type > QCMAP_MSGR_ALL_AP_V01 )
     {
       printf("\n\nInvalid AP Type : %d\n", ap_type);
       break;
     }
     memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);
     printf("Select the Action Type for hostapd which need to be activated : \n"
            "1: Start\t2: Stop\n"
            "3: Restart\t:::");
     fgets(scan_string, sizeof(scan_string), stdin);
     action_type = (qcmap_msgr_activate_hostapd_action_enum_v01)atoi(scan_string);
     if ( action_type < QCMAP_MSGR_HOSTAPD_START_V01 ||
          action_type > QCMAP_MSGR_HOSTAPD_RESTART_V01 )
     {
       printf("\n\nInvalid Action Type : %d\n", action_type);
       break;
     }
     if(QcMapClient->ActivateHostapdConfig(ap_type, action_type, &qmi_err_num))
     {
       printf("\nActivated Hostapd with the new config\n");
     }
     else
       printf("\nFailed to Activate Hostapd, Error: 0x%x", qmi_err_num);
     break;
   }

   /* Activate Supplicant with the new config. */
   case 10:
   {
     if(QcMapClient->ActivateSupplicantConfig(&qmi_err_num))
     {
       printf("\nActivated Supplicant with the new config\n");
     }
     else
       printf("\nFailed to Activate Supplicant, Error: 0x%x", qmi_err_num);
     break;
   }
   break;
   /* Get WLAN IF information. */
   case 11:
   {
     if (QcMapClient->GetActiveWlanIfInfo(&wlan_info_cfg, &qmi_err_num))
     {
       if (wlan_info_cfg.wlan_if_info_len > 0)
       {
         printf("Total IF active %d\n\n\n",wlan_info_cfg.wlan_if_info_len );
         printf("   +---------+---------+-------------+-----------+\n");
         printf("   |         |         |             |           |\n");
         printf("   | IF Name | AP type |  Card Type  |   State   |\n");
         printf("   |         |         |             |           |\n");
         printf("   +---------+---------+-------------+-----------+\n");

         for (int i = 0; i < QCMAP_MSGR_MAX_WLAN_IFACE_V01; i++)
         {
           if (wlan_info_cfg.wlan_if_info[i].state)
           {
              printf("   |%9s|%9s|%13s|%11s|\n", wlan_info_cfg.wlan_if_info[i].if_name,
                WLAN_AP_TYPE(wlan_info_cfg.wlan_if_info[i].wlan_ap_type),
                    WLAN_CARD_TYPE(wlan_info_cfg.wlan_if_info[i].wlan_dev_type),
                        WLAN_STATE_TYPE(wlan_info_cfg.wlan_if_info[i].state));
              printf("   +---------+---------+-------------+-----------+\n");
           }
         }
       }
       else
       {
         printf("No IF is found active\n");
       }
     }
     else
       printf("Failed to get active wlan information, Error: 0x%x\r\n", qmi_err_num);
     break;
   }

   /*Set Always on WLAN*/
   case 12:
   {
     boolean always_on_wlan_state = false;
     printf("Please input Always on WLAN Flag (1-Enable/2-Disable):");
     fgets(scan_string, sizeof(scan_string), stdin);
     if (atoi(scan_string) == 2 || atoi(scan_string) == 1)
     {
       always_on_wlan_state = (atoi(scan_string) == 1) ? true : false;
       if(QcMapClient->SetAlwaysOnWLAN(always_on_wlan_state,&qmi_err_num))
         printf("\nSet Always on WLAN Succeds\n");
       else
         printf("\nFailed to Set Always on WLAN, Error: 0x%x", qmi_err_num);
     }
     else
     {
       printf("\n Invalid option : %d\n", atoi(scan_string));
     }
     break;
   }

   /*Get Always on WLAN*/
   case 13:
   {
     boolean always_on_wlan_status = false;
     if(QcMapClient->GetAlwaysOnWLAN(&always_on_wlan_status,&qmi_err_num))
     {
       printf("\nAlways on WLAN: %s.",
                (always_on_wlan_status)?"Enabled":"Disabled");
     }
     else
        printf("\nFailed to Get Always on WLAN, Error: 0x%x", qmi_err_num);
     break;
   }
   break;

   /* Set Peer to Peer Role */
   case 14:
   {
     int newline_index;
     int input;
     qcmap_p2p_config         p2p_config;

     memset(&p2p_config, 0, sizeof(qcmap_p2p_config));

     printf("\n Please input Peer to Peer Status (1-Enable/0-Disable) \n");
     fgets(scan_string, sizeof(scan_string), stdin);
     newline_index = strlen(scan_string)-1;
     scan_string[newline_index] = '\0';
     if (atoi(scan_string) == 1)
     {
       printf("\n Please input Peer to Peer Role (1-P2P-GO /2-P2P-CLI) \n");
       fgets(scan_string, sizeof(scan_string), stdin);
       if (atoi(scan_string) == 1)
       {
         LOG_MSG_INFO1("Enabling Peer-to-Peer Mode Group Owner", 0, 0, 0);
         p2p_config.p2p_status = TRUE;
         p2p_config.p2p_role_valid = TRUE;
         p2p_config.p2p_role = QCMAP_P2P_ROLE_GO_V01;
         if(QcMapClient->set_p2p_role(p2p_config, &qmi_err_num))
         {
           printf("\n Set peer-to-peer role P2P-GO succeeds \n");
         }
         else
         {
           printf("\n Set Peer-to-peer role P2P-GO failed   0x%x \n", qmi_err_num);
         }
       }
       else if(atoi(scan_string) == 2)
       {
         LOG_MSG_INFO1("Enabling Peer-to-Peer Mode Client", 0, 0, 0);
         p2p_config.p2p_status = TRUE;
         p2p_config.p2p_role_valid = TRUE;
         p2p_config.p2p_role = QCMAP_P2P_ROLE_CLI_V01;
         if(QcMapClient->set_p2p_role(p2p_config, &qmi_err_num))
         {
           printf("\n Set peer-to-peer role P2P-CLI succeeds \n");
         }
         else
         {
           printf("\n Set Peer-to-peer mode P2P-CLI failed   0x%x \n", qmi_err_num);
         }
      }
    }
    else if( (atoi(scan_string) == 0) && (0 == strcmp(scan_string, "0")) )
    {
      if(QcMapClient->set_p2p_role(p2p_config, &qmi_err_num))
      {
        printf("\n Disable Peer-to-peer mode success");
      }
      else
      {
        printf("\n Disable Peer-to-peer mode failed");
      }
    }
    else
    {
      printf("\n Invalid option : %d Please input a valid option \n", atoi(scan_string));
    }
   }
   break;

   /* Get Peer to Peer Role */
   case 15:
   {
     qcmap_p2p_config p2p_config;

     memset(&p2p_config, 0, sizeof(qcmap_p2p_config));
     if(QcMapClient->get_p2p_role(&p2p_config, &qmi_err_num))
     {
       if(P2P_ROLE_ENABLE == p2p_config.p2p_status)
       {
         printf("\n peer-to-peer mode is enabled \n");
         if(QCMAP_P2P_ROLE_GO_V01 == p2p_config.p2p_role)
         {
           printf("\n peer-to-peer mode set is P2P-Group-Owner \n");
         }
         else if(QCMAP_P2P_ROLE_CLI_V01 == p2p_config.p2p_role)
         {
           printf("\n peer-to-peer mode set is  P2P-Client \n");
         }
       }
       else if(P2P_ROLE_DISABLE == p2p_config.p2p_status)
       {
         printf("\n Peer-to-peer mode is not enabled \n");
       }
     }
     else
     {
         printf("\n Get Peer-to-peer mode failed qmi_err_num: 0x%x,  \
                 \n p2p_status: %d p2p_role_valid: %d p2p_role: %d \n",
                  qmi_err_num, p2p_config.p2p_role_valid, p2p_config.p2p_role);
     }
   }
   break;
   default :
   {
     printf("Invalid response %d\n", wlanOpt);
   }
     break;
  }
}

void firewallConfig( int firewallOpt )
{
  qmi_error_type_v01 qmi_err_num;
  /* Initialize QMI Error Number. */
  qmi_err_num = QMI_ERR_NONE_V01;
  char scan_string[QCMAP_MSGR_MAX_FILE_PATH_LEN];

  /* Firewall Configuration options */
  switch(firewallOpt)
  {
  /* Add Firewall Entry */
  case 1:
  {
    qcmap_msgr_firewall_conf_t     extd_firewall_add;

    /*Default rule is to accept the packets */
    printf("Please Enter Firewall entry:\n");
    if ( read_firewall_conf( &extd_firewall_add ) )
    {
      if ( QcMapClient->AddFireWallEntry(&extd_firewall_add,&qmi_err_num )== true)
      {
        printf("\n Add Firewall Entry success.");
      }
      else if (qmi_err_num == QMI_ERR_INTERFACE_NOT_FOUND_V01)
      {
        printf("\nBackhaul down. Saved firwall entry in configuration file.");
      }
      else
        printf("\nAdd Firewall Entry failed, Error: 0x%x", qmi_err_num);
    }
  }
  break;

  /* Set Firewall Config */
  case 2:
  {
    boolean enable_firewall, pkts_allowed = false;

    printf("   Please input Firewall State (1-Enable/0-Disable) : ");
    fgets(scan_string, sizeof(scan_string), stdin);
    enable_firewall = atoi(scan_string);
    if (enable_firewall)
    {
      printf("   Please input Packets Allowed Setting(1-ACCEPT/0-Drop) : ");
      fgets(scan_string, sizeof(scan_string), stdin);
      pkts_allowed = atoi(scan_string);
    }

    if(QcMapClient->SetFirewall(enable_firewall, pkts_allowed, &qmi_err_num))
    {
      printf("Set Firewall state to:%d success\n", enable_firewall);
    }
    else
    {
      printf("Set Firewall Error: 0x%x", qmi_err_num);
    }

    break;
  }

  /* Get Firewall Config */
  case 3:
  {
    boolean enable_firewall, pkts_allowed;

    if (QcMapClient->GetFirewall(&enable_firewall, &pkts_allowed,&qmi_err_num))
    {
      if(enable_firewall)
        printf("Firewall is Enabled\n");
      else
        printf("Firewall is Disabled \n");

       if(pkts_allowed)
        printf("Firewall is configured to ACCEPT packets\n");
      else
        printf("Firewall is configured to DROP packets\n");
    }
    else
     printf("Get Firewall configuration failed,Error 0x%x", qmi_err_num);
    break;
  }

  /* Display Firewalls */
  case 4:
  {
    qcmap_msgr_firewall_conf_t extd_firewall_handle_list;
    int   handle_list_len;
    int   result;

    memset(&extd_firewall_handle_list, 0, sizeof(qcmap_msgr_firewall_conf_t));
    handle_list_len=0;
    while(1)
    {
      printf("\n Please input IP family type [4-IPv4 6-IPv6]:");
      fgets(scan_string, sizeof(scan_string), stdin);
      result = atoi(scan_string);
      if(result == IP_V4 || result == IP_V6 )
      break;
    }

    if(result == IP_V4)
    {
        extd_firewall_handle_list.extd_firewall_handle_list.ip_family = IP_V4;
    }
     else if(result == IP_V6)
    {
     extd_firewall_handle_list.extd_firewall_handle_list.ip_family = IP_V6;
    }
    if(QcMapClient->GetFireWallHandlesList(&extd_firewall_handle_list.extd_firewall_handle_list, &qmi_err_num))
    {
      handle_list_len = extd_firewall_handle_list.extd_firewall_handle_list.num_of_entries;
    }
    else
    {
      printf("\nFirewall Handle list get failed, Error: 0x%x", qmi_err_num);
      break;
    }
    if( DisplayFirewall( handle_list_len, extd_firewall_handle_list ) )
    {
      printf("\n Firewall Display success \n");
    }
  }
  break;

  /* Delete FireWall Entry */
  case 5:
  {
    int handle=0;
    memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);

    printf(" Please Enter the Firewall Handle : ");
    fgets(scan_string, sizeof(scan_string), stdin);
    handle= atoi(scan_string);
    if(handle <0)
      printf("\n Entered Handle is invalid \n");

    if (!QcMapClient->DeleteFireWallEntry(handle, &qmi_err_num))
    {
      if (qmi_err_num == QMI_ERR_INTERFACE_NOT_FOUND_V01)
      {
        printf("\nBackhaul down. firewall entry deleted from configuration file.");
      }
      else if (qmi_err_num == QMI_ERR_INVALID_HANDLE_V01)
      {
        printf("\nInvalid firewall handle.");
      }
      else
        printf("\nDelete firewall Fails, Error: 0x%x", qmi_err_num);
    }
    else
      printf("\nDelete firewall Successfully");
  }
  break;
  default :
  {
    printf("Invalid response %d\n", firewallOpt);
  }
  break;
  }
}

void backhaulCommConfig( int backhaulCommOpt )
{
  qmi_error_type_v01 qmi_err_num;
  /* Initialize QMI Error Number. */
  qmi_err_num = QMI_ERR_NONE_V01;
  char scan_string[QCMAP_MSGR_MAX_FILE_PATH_LEN];
  uint32 tmp_input=0;

  /* BackhualCOMMON configuration options */
  switch(backhaulCommOpt)
  {
    /* Get WWAN Network Configuration */
    case 1:
    {
      qcmap_msgr_ip_family_enum_v01 ip_family;
      qcmap_nw_params_t qcmap_nw_params;
      char ip6_addr_buf[INET6_ADDRSTRLEN];

      memset(&qcmap_nw_params,0,sizeof(qcmap_nw_params_t));

      printf("\nPlease input IP Family 4:IPv4 6:IPv6 :- ");
      fgets(scan_string, sizeof(scan_string), stdin);
      ip_family = (qcmap_msgr_ip_family_enum_v01)atoi(scan_string);

      if ( QcMapClient->GetNetworkConfiguration(
                                   ip_family, &qcmap_nw_params, &qmi_err_num ))
      {
        if (ip_family == QCMAP_MSGR_IP_FAMILY_V4_V01)
        {
          if (qmi_err_num == QMI_ERR_NONE_V01)
          {
            printf("\n IPv4 configuration \n");
            qcmap_nw_params.v4_conf.public_ip.s_addr =
                        htonl(qcmap_nw_params.v4_conf.public_ip.s_addr);
            qcmap_nw_params.v4_conf.primary_dns.s_addr =
                        htonl(qcmap_nw_params.v4_conf.primary_dns.s_addr);
            qcmap_nw_params.v4_conf.secondary_dns.s_addr =
                        htonl(qcmap_nw_params.v4_conf.secondary_dns.s_addr);

            if (inet_ntoa(qcmap_nw_params.v4_conf.public_ip) != NULL)
            {
              printf("\nPublic IP for WWAN: %s \n",
                     inet_ntoa(qcmap_nw_params.v4_conf.public_ip));
            }
            else
            {
              printf ("Unable to get Public IP for WWAN");
            }

            char* primary_dns_ip;
            char* secondary_dns_ip;
            if ((primary_dns_ip = inet_ntoa(qcmap_nw_params.v4_conf.primary_dns)) != NULL)
            {
              printf("Primary DNS IP address: %s \n", primary_dns_ip);
            }
            else
            {
              printf ("Unable to get Primary DNS IP address");
            }

            if ((secondary_dns_ip = inet_ntoa(qcmap_nw_params.v4_conf.secondary_dns)) !=  NULL)
            {
              printf("Secondary DNS IP address: %s \n", secondary_dns_ip);
            }
            else
            {
              printf ("Unable to get Secondary DNS IP address");
            }
          }
          else
            printf("\nError in IPv4 config - 0x%x\n\n", qmi_err_num);
        }

        if (ip_family == QCMAP_MSGR_IP_FAMILY_V6_V01)
        {
          if (qmi_err_num == QMI_ERR_NONE_V01)
          {
            char* ipv6_addr;
            printf("\n IPv6 configuration \n");
            if ((ipv6_addr = inet_ntop(AF_INET6,
                     &qcmap_nw_params.v6_conf.public_ip_v6,
                     ip6_addr_buf,sizeof(ip6_addr_buf)))!= NULL)
            {
              printf("\nPublic IP for WWAN: %s \n", ipv6_addr);
            }
            else
            {
              printf ("Unable to get Public IP for WWAN");
            }
            if ((ipv6_addr = inet_ntop(AF_INET6,
                              &qcmap_nw_params.v6_conf.primary_dns_v6,
                              ip6_addr_buf,sizeof(ip6_addr_buf))) != NULL)
            {
              printf("Primary DNS IP address: %s \n", ipv6_addr);
            }
            else
            {
              printf ("Unable to get Primary DNS IP address");
            }

            if ((ipv6_addr = inet_ntop(AF_INET6,
                              &qcmap_nw_params.v6_conf.secondary_dns_v6,
                              ip6_addr_buf,sizeof(ip6_addr_buf))) != NULL)
            {
              printf("Secondary DNS IP address: %s \n", ipv6_addr);
            }
            else
            {
              printf ("Unable to get Secondary DNS IP address");
            }
          }
          else
            printf("\nError in IPv6 config - 0x%x\n\n", qmi_err_num);
        }
      }
      else
        printf("\nGet WWAN Network Config failed,0x%x", qmi_err_num);
      break;
    }

    /* Configure Backhaul Active Priority */
    /* Set Backhaul priority*/
    case 2:
    {
      backhaul_pref_t backhaul_pref_req;
      int i = 1;
      bzero(&backhaul_pref_req,sizeof(backhaul_pref_t));
      while (i <= MAX_BACKHAUL_SUPPORTED)
      {
        bzero(&scan_string,sizeof(scan_string));
        printf("Enter backhaul (1. WWAN or 2. USB Cradle or 3. WLAN"
               " or 4. Ethernet or 5. BT-WAN) which has Prioity %d :", i);
        fgets(scan_string, sizeof(scan_string), stdin);
        tmp_input = atoi(scan_string);
        switch (i)
        {
          case 1:
          {
            backhaul_pref_req.first = tmp_input;
            printf(" first priority %d\n",backhaul_pref_req.first);
          }
          break;
          case 2:
          {
            backhaul_pref_req.second = tmp_input;
            printf(" second priority %d\n",backhaul_pref_req.second);
          }
          break;
          case 3:
          {
            backhaul_pref_req.third = tmp_input;
            printf(" thrid priority %d\n",backhaul_pref_req.third);
          }
          break;
          case 4:
          {
            backhaul_pref_req.fourth = tmp_input;
            printf(" fourth priority %d\n",backhaul_pref_req.fourth);
          }
          break;
          case 5:
          {
            backhaul_pref_req.fifth= tmp_input;
            printf(" fifth priority %d\n",backhaul_pref_req.fifth);
          }
          break;
          default:
            printf("Wrong value Entered!!, Please enter ( wwan or cradle "
              "or Ethernet or wlan) \n");
            i--;
          break;
        }
        i++;
      }
      if (QcMapClient->SetActiveBackhaulPref(&backhaul_pref_req, &qmi_err_num))
        printf("\nBackhaul set config succeeds. Please reboot the device for config to take effect");
      else
        printf("\nBackhaul set config fails, Error: 0x%x", qmi_err_num);
    }
      break;

    /* Get Backhual Priority */
    case 3:
    {
      backhaul_pref_t backhaul_pref_resp;
      if (QcMapClient->GetBackhaulPref(&backhaul_pref_resp, &qmi_err_num))
      {
        if (backhaul_pref_resp.first == QCMAP_MSGR_WWAN_BACKHAUL_V01)
          printf("First Priority backhaul is WWAN\n");
        else if (backhaul_pref_resp.first == QCMAP_MSGR_USB_CRADLE_BACKHAUL_V01)
          printf("First Priority backhaul is USB Cradle\n");
        else if (backhaul_pref_resp.first == QCMAP_MSGR_WLAN_BACKHAUL_V01)
          printf("First Priority backhaul is WLAN\n");
        else if (backhaul_pref_resp.first == QCMAP_MSGR_ETHERNET_BACKHAUL_V01)
          printf("First Priority backhaul is Ethernet\n");
        else if (backhaul_pref_resp.first == QCMAP_MSGR_BT_BACKHAUL_V01)
          printf("First Priority backhaul is BT WAN\n");

        if (backhaul_pref_resp.second == QCMAP_MSGR_WWAN_BACKHAUL_V01)
          printf("Second Priority backhaul is WWAN\n");
        else if (backhaul_pref_resp.second == QCMAP_MSGR_USB_CRADLE_BACKHAUL_V01)
          printf("Second Priority backhaul is USB Cradle\n");
        else if (backhaul_pref_resp.second == QCMAP_MSGR_WLAN_BACKHAUL_V01)
          printf("Second Priority backhaul is WLAN\n");
        else if (backhaul_pref_resp.second == QCMAP_MSGR_ETHERNET_BACKHAUL_V01)
          printf("Second Priority backhaul is Ethernet\n");
        else if (backhaul_pref_resp.second == QCMAP_MSGR_BT_BACKHAUL_V01)
          printf("Second Priority backhaul is BT WAN\n");

        if (backhaul_pref_resp.third == QCMAP_MSGR_WWAN_BACKHAUL_V01)
          printf("Third Priority backhaul is WWAN\n");
        else if (backhaul_pref_resp.third == QCMAP_MSGR_USB_CRADLE_BACKHAUL_V01)
          printf("Third Priority backhaul is USB Cradle\n");
        else if (backhaul_pref_resp.third == QCMAP_MSGR_WLAN_BACKHAUL_V01)
          printf("Third Priority backhaul is WLAN\n");
        else if (backhaul_pref_resp.third == QCMAP_MSGR_ETHERNET_BACKHAUL_V01)
          printf("Third Priority backhaul is Ethernet\n");
        else if (backhaul_pref_resp.third == QCMAP_MSGR_BT_BACKHAUL_V01)
          printf("Third Priority backhaul is BT WAN\n");

        if (backhaul_pref_resp.fourth == QCMAP_MSGR_WWAN_BACKHAUL_V01)
          printf("Fourth Priority backhaul is WWAN\n");
        else if (backhaul_pref_resp.fourth == QCMAP_MSGR_USB_CRADLE_BACKHAUL_V01)
          printf("Fourth Priority backhaul is USB Cradle\n");
        else if (backhaul_pref_resp.fourth == QCMAP_MSGR_WLAN_BACKHAUL_V01)
          printf("Fourth Priority backhaul is WLAN\n");
        else if (backhaul_pref_resp.fourth == QCMAP_MSGR_ETHERNET_BACKHAUL_V01)
          printf("Fourth Priority backhaul is Ethernet\n");
        else if (backhaul_pref_resp.fourth == QCMAP_MSGR_BT_BACKHAUL_V01)
          printf("Fourth Priority backhaul is BT WAN\n");

        if (backhaul_pref_resp.fifth == QCMAP_MSGR_WWAN_BACKHAUL_V01)
          printf("Fifth Priority backhaul is WWAN\n");
        else if (backhaul_pref_resp.fifth == QCMAP_MSGR_USB_CRADLE_BACKHAUL_V01)
          printf("Fifth Priority backhaul is USB Cradle\n");
        else if (backhaul_pref_resp.fifth == QCMAP_MSGR_WLAN_BACKHAUL_V01)
          printf("Fifth Priority backhaul is WLAN\n");
        else if (backhaul_pref_resp.fifth == QCMAP_MSGR_ETHERNET_BACKHAUL_V01)
          printf("Fifth Priority backhaul is Ethernet\n");
        else if (backhaul_pref_resp.fifth == QCMAP_MSGR_BT_BACKHAUL_V01)
          printf("Fifth Priority backhaul is BT WAN\n");
      }
      else
        printf("\nGet Backhaul Priority fails, Error: 0x%x", qmi_err_num);
    }
      break;

    /* Get data bitrates */
   case 4:
   {
     qcmap_msgr_data_bitrate_v01 data_rate;
     memset(&data_rate, 0, sizeof(qcmap_msgr_data_bitrate_v01));

     if ( QcMapClient->GetDataRate(&data_rate, &qmi_err_num))
     {
         printf("\n Data Rates:");
         printf("\n Current tx: %llu", data_rate.tx_rate);
         printf("\n Current rx: %llu", data_rate.rx_rate);
         printf("\n Max tx: %llu", data_rate.max_tx_rate);
         printf("\n Max rx: %llu\n", data_rate.max_rx_rate);
     }
     else
     {
         printf("\n  Get Data Bitrate Fails , Error: 0x%x \n", qmi_err_num);
     }
     break;
   }


   /* Enable/Disable IPV4. */
   case 5:
   {
     printf("   Please input IPV4 State (1-Enable/0-Disable) : ");
     fgets(scan_string, sizeof(scan_string), stdin);
     if (atoi(scan_string))
     {
       if (QcMapClient->EnableIPV4(&qmi_err_num))
         printf("\nIPV4 Enable succeeds.");
       else
         printf("\nIPV4 Enable fails, Error: 0x%x", qmi_err_num);
     }
     else
     {
       if (QcMapClient->DisableIPV4(&qmi_err_num))
         printf("\nIPV4 Disable in progress.");
       else
         printf("\nIPV4 Disable request fails, Error: 0x%x", qmi_err_num);
     }
     break;
   }

   /* Enable/Disable IPV6. */
   case 6:
   {
     printf("   Please input IPV6 State (1-Enable/0-Disable) : ");
     fgets(scan_string, sizeof(scan_string), stdin);
     if (atoi(scan_string))
     {
       if (QcMapClient->EnableIPV6(&qmi_err_num))
         printf("\nIPV6 Enable succeeds.");
       else
         printf("\nIPV6 Enable fails, Error: 0x%x", qmi_err_num);
     }
     else
     {
       if (QcMapClient->DisableIPV6(&qmi_err_num))
         printf("\nIPV6 Disable in progress.");
       else
         printf("\nIPV6 Disable request fails, Error: 0x%x", qmi_err_num);
     }
     break;
   }

   /* Get ipv4 state .     */
   case 7:
   {
     boolean ipv4_state;
     memset((void *)&ipv4_state, 0, sizeof(uint8_t));

     if (QcMapClient->GetIPv4State( &ipv4_state, &qmi_err_num))
     {
        printf("\nIPV4 is: %s.\n",(ipv4_state)?"Enabled":"Disabled");
     }
     else
     {
       printf("\nGetIPV4State returns Error: 0x%x", qmi_err_num);
     }
     break;
   }

   /* Get ipv6 state .     */
   case 8:
   {
     DisplayIPv6State();
     break;
   }
   /* Get Current Backhaul Status*/
   case 9:
   {
     qcmap_backhaul_status_info_type backhaul_status_info;
     char buffer[MAX_BACKHAUL_TYPE_LENGTH];
     memset(&backhaul_status_info, 0, sizeof(qcmap_backhaul_status_info_type));
     memset(buffer,0,MAX_BACKHAUL_TYPE_LENGTH);

     if (QcMapClient->GetBackhaulStatus( &backhaul_status_info, &qmi_err_num))
     {
      if ((backhaul_status_info.backhaul_v4_available == false) &&
         (backhaul_status_info.backhaul_v6_available == false))
        printf("No Backhaul \n");
      else
      {
        if (convert_backhaul_enum_to_string(backhaul_status_info.backhaul_type, buffer))
        {
          printf("%s Backhual\n", buffer);
          printf("IPV4 %s.\n",(backhaul_status_info.backhaul_v4_available)?"Connected":"Disconnected");
          printf("IPV6 %s.\n",(backhaul_status_info.backhaul_v6_available)?"Connected":"Disconnected");
        }
        else
        {
          printf("Invaild backhaul type : %d",backhaul_status_info.backhaul_type);
        }
      }
     }
     else
     {
       printf("\nGetBackhaulStatus returns Error: 0x%x", qmi_err_num);
     }
     break;
   }
   break;
   default :
  {
    printf("Invalid response %d\n", backhaulCommOpt);
  }
  break;
  }
}

int16_t ChooseLANBridge(void)
{
  qcmap_msgr_bridge_list_v01 bridge_list;
  qmi_error_type_v01 qmi_err_num;
  char scan_string[QCMAP_MSGR_MAX_FILE_PATH_LEN] = {0};
  int16_t option = 0;
  profile_handle_type_v01 profile_handle = 0;

  if(QcMapClient->GetLANBridges(&bridge_list, &qmi_err_num))
  {
    if(bridge_list.num_of_bridges > 0)
    {
      //tell user what options are available
      printf("   +--------+\n");
      printf("   |        |\n");
      printf("   | Bridge |\n");
      printf("   |        |\n");
      printf("   +--------+\n");

      for(int i = 0; i < bridge_list.num_of_bridges; i++)
      {
        if(bridge_list.bridge_arr[i] == bridge_list.curr_bridge)
        {
          printf("   |%5d(*)|\n", bridge_list.bridge_arr[i]);
        } else {
          printf("   |%8d|\n", bridge_list.bridge_arr[i]);
        }
      }
      printf("   +--------+\n\n");


      //ask for user input
      printf("   Select Bridge #: ");
      if(fgets(scan_string, sizeof(scan_string), stdin) == NULL)
      {
        printf("error with client input: %s\n", strerror(errno));
        return -1;
      }
      option = atoi(scan_string);

      //validate user input
      for(int i = 0; i < bridge_list.num_of_bridges; i++)
      {
        if(option == bridge_list.bridge_arr[i])
        {
          printf ("Selected Bridge# = (%d)\n", option);
          return option;
        }
      }
    }
  }

  printf("Error with choosing LAN Bridge\n");
  return -1;
}

#define WWAN_TECH_TYPE(type) ( (type == QCMAP_MSGR_MASK_TECH_PREF_ANY_V01)? "ANY" : ((type == QCMAP_MSGR_MASK_TECH_PREF_3GPP_V01) ? "3gpp" : "3gpp2"))
#define IP_FAMILY_TYPE(ip_family) ( (ip_family == QCMAP_MSGR_IP_FAMILY_V4_V01)? "V4" : ((ip_family == QCMAP_MSGR_IP_FAMILY_V6_V01) ? "V6" : "V4V6"))

profile_handle_type_v01 ChooseWWANProfileHandle()
{
  qcmap_msgr_wwan_policy_list_resp_msg_v01 wwan_policy_resp_msg;
  qmi_error_type_v01 qmi_err_num;
  char scan_string[QCMAP_MSGR_MAX_FILE_PATH_LEN];
  int option;
  profile_handle_type_v01 profile_handle = 0, current_profile_handle = 0;
  int default_no = 1;

  if (QcMapClient->GetWWANPolicyList(&wwan_policy_resp_msg, &qmi_err_num))
  {
    if (wwan_policy_resp_msg.wwan_policy_len == 1)
      return wwan_policy_resp_msg.wwan_policy[0].profile_handle;

    QcMapClient->GetWWANProfilePreference(&current_profile_handle, &qmi_err_num);

    printf("   +---------+------+------+----------------+----------------+\n");
    printf("   |         |      |      | Profile_Id (V4)| Profile_Id (V6)|\n");
    printf("   | Profile | TECH |  IP  +-------+--------+-------+--------+\n");
    printf("   |         |      |      | 3gpp  | 3gpp2  | 3gpp  | 3gpp2  |\n");
    printf("   +---------+------+------+-------+--------+-------+--------+\n");

    for (int i=0; i < wwan_policy_resp_msg.wwan_policy_len; i++)
    {
      if (wwan_policy_resp_msg.wwan_policy[i].profile_handle == wwan_policy_resp_msg.default_profile_handle)
      {
        default_no = wwan_policy_resp_msg.wwan_policy[i].profile_handle;
        printf("   |%6d(*)|%6s|%6s|%7d|%8d|%7d|%8d|\n", wwan_policy_resp_msg.wwan_policy[i].profile_handle,
                WWAN_TECH_TYPE(wwan_policy_resp_msg.wwan_policy[i].tech_pref),
                IP_FAMILY_TYPE(wwan_policy_resp_msg.wwan_policy[i].ip_family),
                wwan_policy_resp_msg.wwan_policy[i].v4_profile_id_3gpp, wwan_policy_resp_msg.wwan_policy[i].v4_profile_id_3gpp2,
                wwan_policy_resp_msg.wwan_policy[i].v6_profile_id_3gpp, wwan_policy_resp_msg.wwan_policy[i].v6_profile_id_3gpp2);
      }
      else
      {
        printf("   |%9d|%6s|%6s|%7d|%8d|%7d|%8d|\n", wwan_policy_resp_msg.wwan_policy[i].profile_handle,
                WWAN_TECH_TYPE(wwan_policy_resp_msg.wwan_policy[i].tech_pref),
                IP_FAMILY_TYPE(wwan_policy_resp_msg.wwan_policy[i].ip_family),
                wwan_policy_resp_msg.wwan_policy[i].v4_profile_id_3gpp, wwan_policy_resp_msg.wwan_policy[i].v4_profile_id_3gpp2,
                wwan_policy_resp_msg.wwan_policy[i].v6_profile_id_3gpp, wwan_policy_resp_msg.wwan_policy[i].v6_profile_id_3gpp2);
      }
    }
    printf("   +---------+------+------+-------+--------+-------+--------+\n\n");
    printf("   Select Profile Handle # (Default=%d, Current=%d): ", default_no, current_profile_handle);

    fgets(scan_string, sizeof(scan_string), stdin);
    profile_handle = atoi(scan_string);
    printf ("Profile=(%d)\n", profile_handle);
  }

  return profile_handle;
}

void backhaulWWANUpdateConfig( int backhaulWWANUpdateOpt )
{
  qmi_error_type_v01 qmi_err_num;
  char scan_string[QCMAP_MSGR_MAX_FILE_PATH_LEN];
  qcmap_msgr_net_policy_info_v01 net_policy;
  qcmap_msgr_update_profile_enum_v01 update_req = 0;

  /* Initialize QMI Error Number. */
  qmi_err_num = QMI_ERR_NONE_V01;
  memset(&net_policy,0,sizeof(qcmap_msgr_net_policy_info_v01));

  /* BackhaulWWAN Update configuration options */
  switch(backhaulWWANUpdateOpt)
  {
    /* Update Tech Pref */
    case 1:
      {
        printf ("0 -> Any, 1 -> 3gpp, 2 -> 3gpp2 (select one):");
        fgets(scan_string, sizeof(scan_string), stdin);
        net_policy.tech_pref = atoi(scan_string);
        if ( net_policy.tech_pref != 0 && net_policy.tech_pref != 1 &&
            net_policy.tech_pref != 2)
        {
          printf ("\n Invalid tech preference\n");
          return;
        }
        update_req = QCMAP_MSGR_UPDATE_TECH_TYPE_V01;
      }
      break;

    /* Update 3GPP (V4) profile # */
    case 2:
      {
        printf("   Please enter UMTS (V4) Profile Number : ");
        fgets(scan_string, sizeof(scan_string), stdin);
        net_policy.v4_profile_id_3gpp = atoi(scan_string);
        update_req = QCMAP_MSGR_UPDATE_V4_3GPP_PROFILE_INDEX_V01;
      }
      break;

    /* Update 3GPP2 (V4) profile # */
    case 3:
      {
        printf("   Please enter CDMA (V4) Profile Number : ");
        fgets(scan_string, sizeof(scan_string), stdin);
        net_policy.v4_profile_id_3gpp2 = atoi(scan_string);
        update_req = QCMAP_MSGR_UPDATE_V4_3GPP2_PROFILE_INDEX_V01;
      }
      break;

    /* Update 3GPP (V6) profile # */
    case 4:
      {
        printf("   Please enter UMTS (V6) Profile Number : ");
        fgets(scan_string, sizeof(scan_string), stdin);
        net_policy.v6_profile_id_3gpp = atoi(scan_string);
        update_req = QCMAP_MSGR_UPDATE_V6_3GPP_PROFILE_INDEX_V01;
      }
      break;

    /* Update 3GPP2 (V6) profile # */
    case 5:
      {
        printf("   Please enter CDMA (V5) Profile Number : ");
        fgets(scan_string, sizeof(scan_string), stdin);
        net_policy.v6_profile_id_3gpp2 = atoi(scan_string);
        update_req = QCMAP_MSGR_UPDATE_V6_3GPP2_PROFILE_INDEX_V01;
      }
      break;

    /* Update All profile # */
    case 6:
      {
        printf("   Please enter UMTS (V4) Profile Number : ");
        fgets(scan_string, sizeof(scan_string), stdin);
        net_policy.v4_profile_id_3gpp = atoi(scan_string);

        printf("   Please enter CDMA (V4) Profile Number : ");
        fgets(scan_string, sizeof(scan_string), stdin);
        net_policy.v4_profile_id_3gpp2 = atoi(scan_string);

        printf("   Please enter UMTS (V6) Profile Number : ");
        fgets(scan_string, sizeof(scan_string), stdin);
        net_policy.v6_profile_id_3gpp = atoi(scan_string);

        printf("   Please enter CDMA (V5) Profile Number : ");
        fgets(scan_string, sizeof(scan_string), stdin);
        net_policy.v6_profile_id_3gpp2 = atoi(scan_string);

        update_req = QCMAP_MSGR_UPDATE_ALL_PROFILE_INDEX_V01;
      }
      break;

    /* Set concurrent profile as default */
    case 7:
      {
        printf("Switching this current profile as default\n");
        update_req = QCMAP_MSGR_SET_DEFAULT_PROFILE_V01;
      }
      break;
    default:
      printf("Invalid option %d\n", backhaulWWANUpdateOpt);
      break;
  }

  if (update_req && QcMapClient->UpdateWWANPolicy(update_req, net_policy, &qmi_err_num))
    printf("  Update WWAN policy succeeds.\n. ");
  else
  {
    printf("  Failed to Update WWAN policy. Error 0x%x.\n ", qmi_err_num);
  }
}


void backhaulWWANConfig( int backhaulWWANOpt )
{
  qmi_error_type_v01 qmi_err_num;
  /* Initialize QMI Error Number. */
  qmi_err_num = QMI_ERR_NONE_V01;
  char scan_string[QCMAP_MSGR_MAX_FILE_PATH_LEN];
  profile_handle_type_v01 profile_handle = 0;

  /* BackhaulWWAN configuration options */
  switch(backhaulWWANOpt)
  {
  /* Connect/Disconnect Backhaul */
  case 1:
  {
  qcmap_msgr_wwan_call_type_v01 call_type;
  printf("   Please input Backhaul State(1-Connect/0-Disconnect) : ");
  fgets(scan_string, sizeof(scan_string), stdin);
  if (atoi(scan_string))
  {
    memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);
    printf("\n   Please input Call Type (1-IPV4; 2-IPV6) : ");
    fgets(scan_string, sizeof(scan_string), stdin);
    call_type = (qcmap_msgr_wwan_call_type_v01)atoi(scan_string);

    if ( call_type < QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01 ||
         call_type > QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01 )
    {
      printf("\nInvalid Call Type : %d", call_type);
      break;
    }
    if ( QcMapClient->ConnectBackHaul(call_type, &qmi_err_num))
    {
      printf("ConnectBackHaul succeeds.");
    }
    else
      printf("\nConnectBackHaul fails, Error: 0x%x", qmi_err_num);
  }
  else
  {
    memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);
    printf("\n   Please input Call Type (1-IPV4; 2-IPV6) : ");
    fgets(scan_string, sizeof(scan_string), stdin);
    call_type = (qcmap_msgr_wwan_call_type_v01)atoi(scan_string);
    if ( call_type < QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01 ||
         call_type > QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01 )
    {
      printf("\nInvalid Call Type : %d", call_type);
      break;
    }
    if ( QcMapClient->DisconnectBackHaul(call_type, &qmi_err_num))
    {
      printf("\nDisconnect BackHaul succeeds.");
    }
    else
      printf("\nDisconnect BackHaul fails, Error: 0x%x", qmi_err_num);
  }
  break;
  }

  /* Get WAN Status */
  case 2:
  {
  qcmap_msgr_wwan_status_enum_v01 v4_status, v6_status;
  if( QcMapClient->GetWWANStatus(&v4_status, &v6_status, &qmi_err_num))
  {
    switch(v4_status)
    {
      case QCMAP_MSGR_WWAN_STATUS_CONNECTING_V01:
        printf(" IPV4 WWAN is Connecting \n");
        break;

      case QCMAP_MSGR_WWAN_STATUS_CONNECTED_V01:
        printf(" IPV4 WWAN is Connected \n");
        break;

      case QCMAP_MSGR_WWAN_STATUS_DISCONNECTING_V01:
        printf(" IPV4 WWAN is Disconnecting \n");
        break;

      case QCMAP_MSGR_WWAN_STATUS_DISCONNECTED_V01:
        printf(" IPV4 WWAN is Disconnected \n");
        break;

      default:
        printf(" IPV4 WWAN status is unknown \n");
        break;
    }

    switch(v6_status)
    {
      case QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTING_V01:
        printf(" IPV6 WWAN is Connecting \n");
        break;

      case QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTED_V01:
        printf(" IPV6 WWAN is Connected \n");
        break;

      case QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTING_V01:
        printf(" IPV6 WWAN is Disconnecting \n");
        break;

      case QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTED_V01:
        printf(" IPV6 WWAN is Disconnected \n");
        break;

      default:
        printf(" IPV6 WWAN status is unknown \n");
        break;
    }
  }
  else
  printf("\nWWAN status get fails, Error: 0x%x", qmi_err_num);
  }
  break;

  /* Get WWAN Statistics */
  case 3:
  {
  qcmap_msgr_ip_family_enum_v01 ip_family;
  qcmap_msgr_wwan_statistics_type_v01 wwan_stats;

  printf("Please input IP Family IPV4-4 IPV6-6 : ");
  fgets(scan_string, sizeof(scan_string), stdin);
  ip_family = (qcmap_msgr_ip_family_enum_v01)atoi(scan_string);

  memset((void *)&wwan_stats, 0, sizeof(qcmap_msgr_wwan_statistics_type_v01));
  if ( QcMapClient->GetWWANStatistics(ip_family, &wwan_stats, &qmi_err_num) )
  {
   printf("\nWWAN Stats Fetched.\n");
   printf("\nbytes_rx: %llu",wwan_stats.bytes_rx);
   printf("\nbytes_tx: %llu",wwan_stats.bytes_tx);
   printf("\npkts_rx: %lu",wwan_stats.pkts_rx);
   printf("\npkts_tx: %lu",wwan_stats.pkts_tx);
   printf("\npkts_dropped_rx: %lu",wwan_stats.pkts_dropped_rx);
   printf("\npkts_dropped_tx: %lu",wwan_stats.pkts_dropped_tx);
  }
  else
   printf("\nGet WWAN Stats Fails, Error: 0x%x", qmi_err_num);
  }
  break;


  /* RESET WWAN Statistics */
  case 4:
  {
  qcmap_msgr_ip_family_enum_v01 ip_family;

  printf("Please input IP Family IPV4-4 IPV6-6 : ");
  fgets(scan_string, sizeof(scan_string), stdin);
  ip_family = (qcmap_msgr_ip_family_enum_v01)atoi(scan_string);

  if ( QcMapClient->ResetWWANStatistics(ip_family, &qmi_err_num))
  {
     printf("WWAN Stats Reset for IP Family: %d \n", ip_family);
  }
  else
   printf("\nReset WWAN Stats failed, Error: 0x%x", qmi_err_num);
  }
  break;

  /* Set WWAN Webserver Access Flag */
  case 5:
  {
  boolean enable;
  memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);

  printf("   Please input Webserver WWAN access(1-Enable/0-Disable) : ");
  fgets(scan_string, sizeof(scan_string), stdin);
  enable = (atoi(scan_string))? true : false;

  if (QcMapClient->SetWebserverWWANAccess(enable, &qmi_err_num))
  {
    printf("\nWebserver WWAN access set successfully");
  }
  else
  {
    if (qmi_err_num == QMI_ERR_INTERFACE_NOT_FOUND_V01)
      printf("\nBackhaul down.webserver wwan access enabled in xml file.");
    else
      printf("\nset webserver wwan access fails. Error: 0x%x", qmi_err_num);
  }
  }
  break;

  /* Enable/disable the Webserver WWAN access and save XML if successful. */
  case 6:
  {
  boolean flag;
  int p_error=0;

  if (QcMapClient->GetWebserverWWANAccess(&flag, &qmi_err_num))
  {
   printf("\nWebserver WWAN Access Enable Flag : %d", flag);
  }
  else
   printf("\nWebserver WWAN Access fails. Error: 0x%x", qmi_err_num);
  }
  break;


  /* set WWAN Profile */
  case 7:
  {
  qcmap_msgr_ip_family_enum_v01 ip_family ;
  qcmap_msgr_net_policy_info_v01 net_policy;
  memset(&net_policy,0,sizeof(qcmap_msgr_net_policy_info_v01));

  printf("Please select Technology (0-ANY, 1-UMTS, 2-CDMA) : ");
  fgets(scan_string, sizeof(scan_string), stdin);
  net_policy.tech_pref = atoi(scan_string);
  if ( net_policy.tech_pref != 0 && net_policy.tech_pref != 1 &&
      net_policy.tech_pref != 2)
  {
    printf ("\n Invalid tech preference\n");
    break;
  }
  printf("Please input IP Family IPV4-4 IPV6-6 IPV4V6-10 : ");
  fgets(scan_string, sizeof(scan_string), stdin);
  ip_family = (qcmap_msgr_ip_family_enum_v01)atoi(scan_string);
  if ( ip_family == QCMAP_MSGR_IP_FAMILY_V4_V01 )
  {
   printf("   Please enter UMTS Profile Number : ");
   fgets(scan_string, sizeof(scan_string), stdin);
   net_policy.v4_profile_id_3gpp = atoi(scan_string);
   printf("   Please enter CDMA Profile Number : ");
   fgets(scan_string, sizeof(scan_string), stdin);
   net_policy.v4_profile_id_3gpp2 = atoi(scan_string);
  }
  else if ( ip_family == QCMAP_MSGR_IP_FAMILY_V6_V01 )
  {
    printf("   Please enter UMTS Profile Number : ");
    fgets(scan_string, sizeof(scan_string), stdin);
    net_policy.v6_profile_id_3gpp = atoi(scan_string);
    printf("   Please enter CDMA Profile Number : ");
    fgets(scan_string, sizeof(scan_string), stdin);
    net_policy.v6_profile_id_3gpp2 = atoi(scan_string);
  }
  else if ( ip_family == QCMAP_MSGR_IP_FAMILY_V4V6_V01 )
  {
    printf("Please enter V4 UMTS Profile Number : ");
    fgets(scan_string, sizeof(scan_string), stdin);
    net_policy.v4_profile_id_3gpp = atoi(scan_string);
    printf("   Please enter V4 CDMA Profile Number : ");
    fgets(scan_string, sizeof(scan_string), stdin);
    net_policy.v4_profile_id_3gpp2 = atoi(scan_string);
    printf("Please enter V6 UMTS Profile Number : ");
    fgets(scan_string, sizeof(scan_string), stdin);
    net_policy.v6_profile_id_3gpp = atoi(scan_string);
    printf("   Please enter V6 CDMA Profile Number : ");
    fgets(scan_string, sizeof(scan_string), stdin);
    net_policy.v6_profile_id_3gpp2 = atoi(scan_string);
  }
  else
  {
    printf("\nUnsupported ip mode:- %d.\n", ip_family);
    break;
  }
  net_policy.ip_family = ip_family;
  if (QcMapClient->SetWWANPolicy(net_policy, &qmi_err_num))
    printf("  Set Wwan policy succeeds.\n. ");
  else
  {
    printf("  Failed to Set WWAN policy. Error 0x%x.\n ", qmi_err_num);
    if (qmi_err_num == QMI_ERR_DEVICE_IN_USE_V01)
      printf("    Data-call is active for this profile. Disconnect Backhaul and try again.\n");
    else if (qmi_err_num == QMI_ERR_INVALID_HANDLE_V01)
      printf("    Profile already exists, delete this profile and try again.\n");
    else
      printf("    Unknown error.\n");
  }
  break;
  }

  /* get WWAN policy    */
  case 8:
  {
  DisplayWWANPolicy();
  break;
  }

  /* Set Prefix Delegation Mode */
  case 9:
  {
  printf("   Enable(1)/Disable(0) Prefix Delegation mode:");
  fgets(scan_string, sizeof(scan_string), stdin);

  if (atoi(scan_string) >= 0 && atoi(scan_string) <= 1)
  {
   if (QcMapClient->SetPrefixDelegationConfig(atoi(scan_string), &qmi_err_num))
   {
      printf("\nMobile AP Prefix Delegation Config has been set\n");
   }
   else
   {
     if (qmi_err_num == QMI_ERR_DEVICE_IN_USE_V01)
       printf("\nIPv6 WAN call is connected, config saved and will take affect after v6 call is restarted.\n");
     else
       printf("\nFailed to Set Prefix Delegation Config: Error 0x%x.\n ", qmi_err_num);
   }
  }
  else
   printf("\n   %s is invalid, please select a valid option\n", scan_string);

  break;
  }

  /* Get Prefix Delegation Config */
  case 10:
  {
  boolean pd_mode;

  if (QcMapClient->GetPrefixDelegationConfig(&pd_mode, &qmi_err_num))
  {
   if (pd_mode)
     printf("\n   Prefix Delegation Config: mode is enabled.\n");
   else
     printf("\n   Prefix Delegation Config: mode is disabled.\n");
  }
  else
   printf("  Failed to Get Prefix Delegation config. Error 0x%x.\n ", qmi_err_num);
  break;
  }


  /* Get Prefix Delegation Status */
  case 11:
  {
  boolean pd_mode;

  if (QcMapClient->GetPrefixDelegationStatus(&pd_mode, &qmi_err_num))
  {
   if (pd_mode)
     printf("   Prefix Delegation is enabled.\n");
   else
     printf("   Prefix Delegation is disabled.\n");
  }
  else
   printf("  Failed to Get Prefix Delegation mode. Error 0x%x.\n ", qmi_err_num);
  break;
  }

  /* Enable/Disable TinyProxy */
  case 12:
  {
  printf("Please input TinyProxy State(1-Enable/0-Disable) : ");
  fgets(scan_string, sizeof(scan_string), stdin);
  if (atoi(scan_string))
  {
  if(QcMapClient->EnableTinyProxy(&qmi_err_num))
  {
    printf("\nEnabled TinyProxy");
  }
  else
    printf("\nEnable TinyProxy fails, Error: 0x%x", qmi_err_num);
  }
  else
  {
  if(QcMapClient->DisableTinyProxy(&qmi_err_num))
  {
    printf("\nDisabled TinyProxy");
  }
  else
    printf("\nDisable TinyProxy fails, Error: 0x%x", qmi_err_num);
  }

  break;
  }

  /* Get TinyProxy Status */
  case 13:
  {
  qcmap_msgr_tiny_proxy_mode_enum_v01  status;
  if(QcMapClient->GetTinyProxyStatus(&status, &qmi_err_num))
  {
   if(status == QCMAP_MSGR_TINY_PROXY_MODE_UP_V01)
   {
     printf("\nTinyProxy  enabled");
   }
   else if(status == QCMAP_MSGR_TINY_PROXY_MODE_DOWN_V01)
     printf("\nTinyProxy  is disabled");
   else
     printf("\nTinyProxy  mode is not set");
  }
  else
  {
   printf("\nGetTinyProxyStatus returns Error: 0x%x", qmi_err_num);
  }
  break;
  }

  /* Set IP Passthrough Config */
  case 14:
  {
  qcmap_msgr_ip_passthrough_config_v01 ip_passthrough_config;
  boolean device_type = false;
  int i = 0;
  int new_config;
  qcmap_msgr_ip_passthrough_mode_enum_v01 enable_state;
  memset(&ip_passthrough_config, 0, sizeof(qcmap_msgr_ip_passthrough_config_v01));
  printf("\n(Set IP Passthrough Flag: (1-Enable /0-Disable ): ");
  fgets(scan_string, sizeof(scan_string), stdin);
  enable_state = atoi(scan_string);
  if (!(enable_state == 0 || enable_state == 1))
  {
   printf("\n Provide correct input (0/1)");
   break; /* continue */
  }
  if(enable_state == QCMAP_MSGR_IP_PASSTHROUGH_MODE_UP_V01)
  {
   while(TRUE)
   {

     /*
       * If configuring IP Passthrough for the first time
       or User doesn't want to use the existing conf
    */
     printf("\nDo you want to enable passthough with a new config (1-Yes / 0-No ): ");
     fgets(scan_string, sizeof(scan_string), stdin);
     new_config = atoi(scan_string);
     if (!(new_config == 0 || new_config == 1))
     {
       printf("\n Provide correct input (0/1)");
       continue;
     }
     break;
   }
   if (new_config == 0)// Use existing config
   {
     if (QcMapClient->SetIPPassthroughConfig(enable_state, false, NULL, &qmi_err_num))
     {
       printf("Set IP Passthrough status successful\n");
     }
     else
     {
       if (qmi_err_num == QMI_ERR_INVALID_ARG_V01)
         printf("No existing config prevails for IP Passthrough: 0x%x", qmi_err_num);

       printf("Set IP Passthrough status Error: 0x%x", qmi_err_num);
     }
   }
   else if (new_config == 1)// Give a new config
   {
     printf("\nPlease enter the device type (0-USB /1-ETH) : ");
     fgets(scan_string, sizeof(scan_string), stdin);
     device_type = atoi(scan_string);
     if (!(device_type == 0 || device_type == 1))
     {
       printf("\nDevice Type not Supported");
       break; /*continue;*/
     }
     ip_passthrough_config.device_type =
     (device_type == 0) ? QCMAP_MSGR_DEVICE_TYPE_USB_V01
      : QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01;

     if (device_type == 0)
     {
       printf("\nEnter device detail for reservertion\n");
       while(TRUE)
       {
         printf("Enter hostname of the client:");
         if ( fgets(scan_string,sizeof(scan_string),stdin) != NULL )
         {
           if (*scan_string == '\n')
           {
             printf("Client name is mandatory for USB client\n");
             continue;
           }
           else
           {
             for ( i=0;i < strlen(scan_string)-1;i++)
             {
               ip_passthrough_config.client_device_name[i] = scan_string[i];
             }
             ip_passthrough_config.client_device_name[i] ='\0';
             break;
           }
         }
       }
     }
     else if (device_type == 1)
     {
       GET_MAC_ADDR(scan_string,ip_passthrough_config.mac_addr);
     }
     if (QcMapClient->SetIPPassthroughConfig
         (enable_state,true,&ip_passthrough_config,&qmi_err_num))
     {
       printf("Set IP Passthrough status successful\n");
     }
     else
     {
       printf("Set IP Passthrough status Error: 0x%x", qmi_err_num);
     }
   }
  }
  else
  {
   if (QcMapClient->SetIPPassthroughConfig(enable_state,false,NULL,&qmi_err_num))
   {
     printf("Set IP Passthrough status successful\n");
   }
   else
   {
     printf("\nSet IP Passthrough status Error: 0x%x", qmi_err_num);
   }
  }
  }
  break;

  /* Get IP Passthrough Configuration */
  case 15:
  {
  int i=0;
  qcmap_msgr_ip_passthrough_config_v01 ip_passthrough_config;
  qcmap_msgr_ip_passthrough_mode_enum_v01 enable_state;
  char mac_addr_str[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01] = {0}; /*char array of mac address*/
  memset(&ip_passthrough_config, 0, sizeof(qcmap_msgr_ip_passthrough_config_v01));
  if (QcMapClient->GetIPPassthroughConfig(&enable_state,&ip_passthrough_config, &qmi_err_num))
  {
   if(enable_state)
   {
     printf("\nIP Passthrough Flag is SET");
   }
   else
     printf("\nIP Passthrough is NOT SET");

   if (ip_passthrough_config.device_type== QCMAP_MSGR_DEVICE_TYPE_USB_V01)
     printf("\nPassthrough Device: USB ");
   else if (ip_passthrough_config.device_type == QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01)
     printf("\nPassthrough Device: Ethernet ");
   else
     printf("\nDevice Type Not Set");

   ds_mac_addr_ntop(ip_passthrough_config.mac_addr,mac_addr_str);
   if ( strncmp(mac_addr_str,MAC_NULL_STRING,QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01) != 0 )
   {
     printf("\nMAC address of the client: %s",mac_addr_str);
   }

   if ( ip_passthrough_config.client_device_name[0] != '\0')
   {
     printf("\nDevice Name of the client: %s",
     ip_passthrough_config.client_device_name);
   }
  }
  else
   printf("\nGet IP Passthrough status  failed,Error 0x%x", qmi_err_num);
  }
  break;

  /* Get IP Passthrough State */
  case 16:
  {
  boolean active_state;
  if (QcMapClient->GetIPPassthroughState(&active_state,&qmi_err_num))
  {
   if(active_state)
   {
     printf("\nIP Passthrough is ACTIVATED");
   }
   else
     printf("\nIP Passthrough is NOT ACTIVATED");
  }
  else
   printf("Get IP Passthrough state  failed,Error 0x%x", qmi_err_num);
  }
  break;

  /* Set AutoConnect Config */
  case 17:
  {
  boolean enable;
  printf("   Please input Autoconnect Mode Flag (1-Enable/0-Disable) : ");
  fgets(scan_string, sizeof(scan_string), stdin);
  enable = (atoi(scan_string)) ? true : false;
  if ( QcMapClient->SetAutoconnect(enable, &qmi_err_num))
  {
   printf("\nAuto Connect config set succeeds.");
  }
  else
   printf("\nAuto Connect config set fails, Error: 0x%x", qmi_err_num);
   }
  break;

  /* Get AutoConnect Config */
  case 18:
  {
  boolean enable;
  int p_error=0;
  if ( QcMapClient->GetAutoconnect(&enable, &qmi_err_num))
  {
   printf("\nAuto Connect Mode: %s.",(enable)?"Enabled":"Disabled");
  }
  else
   printf("\nAuto Connect config get fails, Error: 0x%x", qmi_err_num);
  }
  break;

  /* Set Roaming */
  case 19:
  {
   boolean enable;
   printf("   Please input Roaming Mode Flag (1-Enable/0-Disable) : ");
   fgets(scan_string, sizeof(scan_string), stdin);
   enable = (atoi(scan_string)) ? true : false;
   if ( QcMapClient->SetRoaming(enable, &qmi_err_num))
   {
     printf("\nRoaming set config succeeds.");
   }
   else
     printf("\nRoaming set config fails, Error: 0x%x", qmi_err_num);
  }
  break;

  /* Get Roaming */
  case 20:
  {
  boolean enable;
  int p_error=0;
  if ( QcMapClient->GetRoaming(&enable, &qmi_err_num))
  {
    printf("\nRoaming Mode: %s.",(enable)? "Enabled" : "Disabled");
  }
  else
    printf("\nRoaming  get fails. Error: 0x%x", qmi_err_num);
  }
  break;

  /* Enable/Disable DDNS */
  case 21:
  {
    printf("   Please input Dynamic DNS State(1-Enable/0-Disable) : ");
    fgets(scan_string, sizeof(scan_string), stdin);
    if (atoi(scan_string))
    {
      if(QcMapClient->EnableDDNS(&qmi_err_num))
      {
        printf("\nEnabled DDNS");
      }
      else
        printf("\nEnable DDNS fails, Error: 0x%x", qmi_err_num);
    }
    else
    {
      if(QcMapClient->DisableDDNS(&qmi_err_num))
      {
        printf("\nDisabled DDNS");
      }
      else
        printf("\nDisable DDNS fails, Error: 0x%x", qmi_err_num);
    }
    break;
  }

  /* Set DDNS Config */
  case 22:
  {
   qcmap_msgr_get_dynamic_dns_config_resp_msg_v01 ddns_server_supported;
   qcmap_msgr_set_dynamic_dns_config_req_msg_v01 ddns_set_config;
   uint32_t value = 0;
   bzero(&ddns_server_supported, sizeof(ddns_server_supported));
   bzero(&ddns_set_config, sizeof(ddns_set_config));
   printf("\nPlease Find the supported Dynamic server: ");
   if(!QcMapClient->GetDDNSConfig(&ddns_server_supported,&qmi_err_num))
   {
     printf("Error getting the supported ddns server 0x%x.\n ", qmi_err_num);
     break;
   }

   if( ddns_server_supported.ddns_config_len ==0 )
   {
     printf("No ddns server configured 0x%x.\n ", qmi_err_num);
     break;
   }
   printf("\nSupported Server : %s ", ddns_server_supported.ddns_config[0].server_url);
   memcpy( ddns_set_config.ddns_server, ddns_server_supported.ddns_config[0].server_url,
   QCMAP_MSGR_DDNS_URL_LENGTH_V01);
   printf("\nPlease enter the login ID :");
   fgets(scan_string,sizeof(scan_string),stdin);
   strlcpy( (char *)&ddns_set_config.login, scan_string, strlen(scan_string));
   printf("\nPlease enter the password :");
   fgets(scan_string,sizeof(scan_string),stdin);
   strlcpy( (char *)&ddns_set_config.password, scan_string, strlen(scan_string));
   printf("\nPlease enter the Hostname:");
   fgets(scan_string,sizeof(scan_string),stdin);
   strlcpy( (char *)&ddns_set_config.hostname, scan_string, strlen(scan_string));
   printf("\nPlease enter the timeout :");
   fgets(scan_string, sizeof(scan_string), stdin);
   ddns_set_config.timeout = atoi(scan_string);
   if(!QcMapClient->SetDDNSConfig(&ddns_set_config,&qmi_err_num))
   {
     printf("Error getting the supported ddns server 0x%x.\n ", qmi_err_num);
     break;
   }
   break;
  }

  /* Get DDNS Config */
  case 23:
  {
   qcmap_msgr_get_dynamic_dns_config_resp_msg_v01 ddns_server_supported;
   uint32_t value = 0;
   bzero(&ddns_server_supported, sizeof(ddns_server_supported));
   printf(" Please Find the supported Dynamic Dns server: \n");
   if(!QcMapClient->GetDDNSConfig(&ddns_server_supported,&qmi_err_num))
   {
     printf("Error getting the supported ddns server 0x%x.\n ", qmi_err_num);
     break;
   }
   if( ddns_server_supported.ddns_config_len ==0 )
   {
     printf("No ddns server configured 0x%x.\n ", qmi_err_num);
     break;
   }
   for(int i =0 ; i < ddns_server_supported.ddns_config_len; i++)
   {
     printf("%d. %s \n",i,ddns_server_supported.ddns_config[i].server_url);
   }
   printf("Configured hostname :%s \n",ddns_server_supported.hostname);
   printf("Please enter the timeout :%d \n",ddns_server_supported.timeout);

   if( ddns_server_supported.enable )
     printf("DDNS enabled \n");
   else
     printf("DDNS Disabled \n");

   break;
  }

  /* Switch Profile */
  case 24:
  {
    profile_handle = ChooseWWANProfileHandle();
    if(QcMapClient->SetWWANProfileHandlePreference(profile_handle, &qmi_err_num))
    {
      printf("Profile-handle updated.");
    }
    else
    {
      printf("Profile-handle Fails!!!");
    }
    break;
  }

  /* Create Profile */
  case 25:
  {
    qcmap_msgr_net_policy_info_v01 net_policy;
    memset(&net_policy,0,sizeof(qcmap_msgr_net_policy_info_v01));

    printf("Please select Technology (0-ANY, 1-UMTS, 2-CDMA) : ");
    fgets(scan_string, sizeof(scan_string), stdin);
    net_policy.tech_pref = atoi(scan_string);
    if ( net_policy.tech_pref != 0 && net_policy.tech_pref != 1 &&
        net_policy.tech_pref != 2)
    {
      printf ("\n Invalid tech preference\n");
      break;
    }

      printf("Please enter V4 UMTS Profile Number : ");
      fgets(scan_string, sizeof(scan_string), stdin);
      net_policy.v4_profile_id_3gpp = atoi(scan_string);
      printf("   Please enter V4 CDMA Profile Number : ");
      fgets(scan_string, sizeof(scan_string), stdin);
      net_policy.v4_profile_id_3gpp2 = atoi(scan_string);
      printf("Please enter V6 UMTS Profile Number : ");
      fgets(scan_string, sizeof(scan_string), stdin);
      net_policy.v6_profile_id_3gpp = atoi(scan_string);
      printf("   Please enter V6 CDMA Profile Number : ");
      fgets(scan_string, sizeof(scan_string), stdin);
      net_policy.v6_profile_id_3gpp2 = atoi(scan_string);
    if (QcMapClient->CreateWWANPolicy(net_policy, &qmi_err_num))
      printf("  Create WWAN policy succeeds.\n. ");
    else
    {
      if (qmi_err_num == QMI_ERR_NO_FREE_PROFILE_V01)
        printf("  Max Profiles reached, Error 0x%x\n", qmi_err_num);
      else if (qmi_err_num == QMI_ERR_INVALID_PROFILE_V01)
        printf ("  Invalid/Duplicate Profile request, Error 0x%x", qmi_err_num);
      else
        printf("  Failed to Create WWAN policy. Error 0x%x\n ", qmi_err_num);
    }
    break;
  }

  /* Update Profile */
  case 26:
  {
    int array_size;
    int backhaulWWANUpdateOpt;
    array_size = sizeof(backhaul_wwan_update_configuration_list)/sizeof(backhaul_wwan_update_configuration_list[0]);
    for (int i=0; i<array_size; i++)
    {
      printf("%s\n",backhaul_wwan_update_configuration_list[i]);
    }

    printf("Please select an option : ");
    fgets(scan_string, sizeof(scan_string), stdin);
    backhaulWWANUpdateOpt = atoi(scan_string);
    backhaulWWANUpdateConfig( backhaulWWANUpdateOpt );
    break;
  }

  /* Delete Profile */
  case 27:
  {
    if(QcMapClient->DeleteWWANPolicy(&qmi_err_num))
    {
      printf("Delete Policy succeeds");
    }
    else
    {
      printf("Delete Policy Fails!!! Error 0x%x.\n", qmi_err_num);
      if (qmi_err_num == QMI_ERR_DEVICE_IN_USE_V01)
        printf("    Data-call is active for this profile. Disconnect Backhaul and try again.\n");
      else if (qmi_err_num == QMI_ERR_INVALID_HANDLE_V01)
        printf("    Profile is default or doesn't exists.\n");
      else
        printf("    Unknown error.\n");
    }
    break;
  }

  /* Add/Delete PDN to VLAN Mapping */
  case 28:
  {
    int16_t chosen_bridge = -1;

    printf("Please input (1-Add/0-Delete) PDN to VLAN Mapping: ");
    memset(scan_string, 0, sizeof(scan_string));
    fgets(scan_string, sizeof(scan_string) - 1, stdin);
    switch(atoi(scan_string))
    {
      case(0): //Delete case
      {
        printf("Please select VLAN/Bridge:\n");
        if((chosen_bridge = ChooseLANBridge()) >= 0)
        {
          if(!QcMapClient->SelectLANBridge(chosen_bridge, &qmi_err_num))
          {
            printf("Failed to select LAN bridge: 0x%x", qmi_err_num);
            return;
          }
        } else {
          printf("Invalid LAN Bridge: %d", chosen_bridge);
          return;
        }

        printf("Please select PDN to disassociate from VLAN/Bridge %d:\n", chosen_bridge);
        profile_handle = ChooseWWANProfileHandle();

        if(!QcMapClient->DeletePDNToVLANMapping(chosen_bridge, profile_handle, &qmi_err_num))
        {
          printf("Failed to Delete VLAN/Bridge %d from profile_handle %d: 0x%x\n", chosen_bridge,
                 profile_handle, qmi_err_num);
          return;
        }
        printf("Succesfully Deleted VLAN/Bridge %d from profile_handle %d\n", chosen_bridge,
                profile_handle);
        break;
      }
      case(1): //Add case
      {
        printf("Please select VLAN/Bridge:\n");
        if((chosen_bridge = ChooseLANBridge()) >= 0)
        {
          if(!QcMapClient->SelectLANBridge(chosen_bridge, &qmi_err_num))
          {
            printf("Failed to select LAN bridge: 0x%x", qmi_err_num);
            return;
          }
        } else {
          printf("Invalid LAN Bridge: %d", chosen_bridge);
          return;
        }

        printf("Please select PDN to map to VLAN/Bridge %d:\n", chosen_bridge);
        profile_handle = ChooseWWANProfileHandle();

        if(!QcMapClient->AddPDNToVLANMapping(chosen_bridge, profile_handle, &qmi_err_num))
        {
          printf("Failed to Add VLAN/Bridge %d to profile_handle %d: 0x%x\n", chosen_bridge,
                 profile_handle, qmi_err_num);
          return;
        }
        printf("Succesfully Added VLAN/Bridge %d to profile_handle %d\n", chosen_bridge,
                profile_handle);

        break;
      }
      default:
      {
        printf("Invalid input: %s\n", scan_string);
        return;
        break;
      }
    }
    break;
  }

  /* Get All PDN to VLAN Mappings */
  case 29:
  {
    int num_entries = 0;
    qcmap_msgr_pdn_to_vlan_mapping_v01 mappings[QCMAP_MAX_NUM_BACKHAULS_V01];
    memset(mappings, 0, QCMAP_MAX_NUM_BACKHAULS_V01*sizeof(qcmap_msgr_pdn_to_vlan_mapping_v01));

    if(QcMapClient->GetPDNtoVLANMappings(mappings, &num_entries, &qmi_err_num))
    {
      printf("|PDN|VLAN-ID|\n");
      printf("-------------\n");
      for (int i=0; i<num_entries; i++)
      {
        printf("  %1d%6d\n",mappings[i].profile_handle, mappings[i].vlan_id);
      }
    }
    else
    {
      printf("No current mappings");
    }
    break;
  }
  /* Set PMIP mode configuration */
  case 30:
  {
    int  pmip_mode_enabled = 0;
    int pmip_mode_type = 0;
    int is_pmip_debug_mode=0;
    int pmip_tunnel_mode = 0;
    int pmipv4_work_mode = 0;
    int pmip_mobile_node_identifier_type = 0;
    int modify_service_selection_string = 0;
    struct ps_in_addr lma_v4_ip = {0};
    struct ps_in_addr dmnp_prefix = {0};
    int dmnp_prefix_len = 0;
    struct ps_in6_addr lma_v6_ip = {0};
    struct in_addr addr = {0};

    qmi_error_type_v01 qmi_err_num;
    /* Initialize QMI Error Number. */
    qmi_err_num = QMI_ERR_NONE_V01;
    char scan_string[QCMAP_MSGR_MAX_FILE_PATH_LEN];
    char id_string[MAX_ID_STRING_LEN] = {0};
    qcmap_msgr_set_pmip_mode_req_msg_v01  set_pmip_mode_req_msg;


    memset( (void*)&set_pmip_mode_req_msg, 0x0, sizeof(qcmap_msgr_set_pmip_mode_req_msg_v01));
    printf("  Please input PMIP mode(1-Enable/0-Disable) : \n");
    fgets(scan_string, sizeof(scan_string), stdin);
    pmip_mode_enabled = atoi(scan_string);
    if (pmip_mode_enabled == 1)
    {
      set_pmip_mode_req_msg.enable_pmip_mode = pmip_mode_enabled;
      printf(" Please input pmip Logging mode : 0-Normal Mode/1-Debug Mode\n");
      printf ("In debug mode Logs will to written in File. Enter choice:");
      fgets(scan_string, sizeof(scan_string), stdin);
      if((atoi(scan_string) != 0) && (atoi(scan_string) != 1))
      {
         printf ("\nInvalid input for debug enable %d. Expecting 0/1\n",atoi(scan_string));
         break;
      }
      is_pmip_debug_mode = atoi(scan_string);
      set_pmip_mode_req_msg.enable_pmip_debug_mode= is_pmip_debug_mode;

      printf ("\nPlease enter LMA ip Address type 0-v4, 1-v6\n");
      printf ("If type v4 then gre Tunnel over v4 address,If v6 then\n");
      printf (" gre tunnel over v6 adress for pmip mode.Enter Choice:");


      fgets(scan_string, sizeof(scan_string), stdin);
      if(atoi(scan_string) == 0)
      {
        printf("\nPlease enter LMA V4 Address : [x:x:x:x]:");
        read_addr(AF_INET,(uint8 *)&lma_v4_ip.ps_s_addr);
        set_pmip_mode_req_msg.lma_ipv4_addr_valid = true;
        set_pmip_mode_req_msg.lma_ipv4_addr = lma_v4_ip.ps_s_addr;
        addr.s_addr= lma_v4_ip.ps_s_addr;
        printf("  \nEnabling PMIP mode with LMA v4 IP=%s... \n",inet_ntoa(addr));
      }
      else
      {
        printf("\nPlease enter LMA V6 Address : [xxxx:xxxx::xxxx:xxxx:xxxx]:");
        read_addr(AF_INET6,(uint8 *)&lma_v6_ip.in6_u.u6_addr8);
        set_pmip_mode_req_msg.lma_ipv6_addr_valid = true;
        memcpy(set_pmip_mode_req_msg.lma_ipv6_addr, &lma_v6_ip, sizeof( struct ps_in6_addr));
      }

      printf("  Please input PMIP Mode: 0-v4/1-v6/2-v4v6\n");
      fgets(scan_string, sizeof(scan_string), stdin);
      pmip_mode_type = atoi(scan_string);
      if (pmip_mode_type == 0)
      {
        set_pmip_mode_req_msg.pmip_mode_type = QCMAP_MSGR_IP_FAMILY_V4_V01;
      }
      else if (pmip_mode_type == 1)
      {
        set_pmip_mode_req_msg.pmip_mode_type = QCMAP_MSGR_IP_FAMILY_V6_V01;
      }
      else if (pmip_mode_type == 2)
      {
        set_pmip_mode_req_msg.pmip_mode_type = QCMAP_MSGR_IP_FAMILY_V4V6_V01;
      }
      else
      {
        printf("\nInvalid pmip mode . setting to v6 pmip moed");
        set_pmip_mode_req_msg.pmip_mode_type = QCMAP_MSGR_IP_FAMILY_V6_V01;
        pmip_mode_type = 1;
      }
      if ((pmip_mode_type == 0) || (pmip_mode_type == 1) || (pmip_mode_type == 2))
      {
        printf("\nPlease Enter PMIP Mobile node identifier type used in PBU[\n1-Mobile Node Identifier String\n2-MAC address\n]:");
        fgets(scan_string, sizeof(scan_string), stdin);
        pmip_mobile_node_identifier_type = atoi(scan_string);
        if (pmip_mobile_node_identifier_type == 1)
        {
          printf("\nPlease Enter Mobile node identifier string:\n");
          fgets(id_string, sizeof(id_string), stdin);
          id_string[strlen(id_string)-1] = '\0';
          set_pmip_mode_req_msg.pmip_mobile_node_identifier_type_valid = true;
          set_pmip_mode_req_msg.pmip_mobile_node_identifier_type = QCMAP_MSGR_PMIP_MOBILE_NODE_IDENTIFIER_STRING_V01;

          memset(set_pmip_mode_req_msg.pmip_mn_id_string,0,QCMAP_MSGR_PMIP_MN_ID_STRING_LENGTH_V01+ 1);
          set_pmip_mode_req_msg.pmip_mn_id_string_valid = true;
          memcpy(set_pmip_mode_req_msg.pmip_mn_id_string,id_string,strlen(id_string));
        }
        else if (pmip_mobile_node_identifier_type == 2)
        {
          set_pmip_mode_req_msg.pmip_mobile_node_identifier_type_valid = true;
          set_pmip_mode_req_msg.pmip_mobile_node_identifier_type = QCMAP_MSGR_PMIP_MOBILE_NODE_IDENTIFIER_MAC_V01;
        }
        else
        {
          printf("\nInvalid Identifier type - %d\n", pmip_mobile_node_identifier_type);
          break;
        }
        printf("\nAdd Service Selection String in PBU [1-Yes/0-NO]:");
        fgets(scan_string, sizeof(scan_string), stdin);
        modify_service_selection_string = atoi(scan_string);
        if (modify_service_selection_string == 0)
        {
          printf("\nService Selection string will not be added in PBU if server has invalid string:\n");
        }
        else if (modify_service_selection_string == 1)
        {
          printf("\nPlease Enter Service Selection string :\n");
          fgets(id_string, sizeof(id_string), stdin);
          id_string[strlen(id_string)-1] = '\0';
          memset(set_pmip_mode_req_msg.pmip_service_selection_string,0,QCMAP_MSGR_PMIP_SERVICE_SELECTION_STRING_LENGTH_V01 + 1);
          set_pmip_mode_req_msg.pmip_service_selection_string_valid= true;
          memcpy(set_pmip_mode_req_msg.pmip_service_selection_string,id_string,strlen(id_string));
        }
        else
        {
          printf("\nInvalid Option - %d\n", modify_service_selection_string);
          break;
        }
      }

      //If pmip type is v4 ask more info
      if((pmip_mode_type == 0) || (pmip_mode_type == 2))
      {
        printf("\nPlease Enter Pmipv4 working mode [0-CPE Mode/1-Secondary Router mode]:");
        fgets(scan_string, sizeof(scan_string), stdin);
        pmipv4_work_mode = atoi(scan_string);
        if (pmipv4_work_mode == 0)
        {
          printf("  Pmip v4 CPE Mode... \n");
          set_pmip_mode_req_msg.pmipv4_mode_type_valid = true;
          set_pmip_mode_req_msg.pmipv4_mode_type = QCMAP_MSGR_PMIPV4_MODE_CPE_V01;
        }
        else if (pmipv4_work_mode == 1)
        {
          printf("  Pmip v4 Secondary Router Mode... \n");
          set_pmip_mode_req_msg.pmipv4_mode_type_valid = true;
          set_pmip_mode_req_msg.pmipv4_mode_type = QCMAP_MSGR_PMIPV4_MODE_SECONDARY_ROUTER_V01;
          printf("\nPlease enter DMNP PREFIX : [x:x:x:x]:");
          read_addr(AF_INET,(uint8 *)&dmnp_prefix.ps_s_addr);
          set_pmip_mode_req_msg.pmipv4_sec_router_param_valid= true;
          set_pmip_mode_req_msg.pmipv4_sec_router_param.dmnp_prefix= dmnp_prefix.ps_s_addr;
          addr.s_addr= dmnp_prefix.ps_s_addr;
          printf(" DMNP prefix=%s... \n",inet_ntoa(addr));

          printf("\nPlease Enter DMNP Prefix length:");
          fgets(scan_string, sizeof(scan_string), stdin);
          dmnp_prefix_len = atoi(scan_string);
          set_pmip_mode_req_msg.pmipv4_sec_router_param.prefix_len = dmnp_prefix_len;
        }
        else
        {
          printf("\nInvalid option - %d\n", pmipv4_work_mode);
          break;
        }
      }
      if (QcMapClient->SetPMIPMode(&set_pmip_mode_req_msg, &qmi_err_num))
        printf("SetPMIPMode to Enable succeeded \n");
      else
        printf("SetPMIPMode fails , Error: 0x%x\n", qmi_err_num);
    }
    else if (pmip_mode_enabled == 0)
    {
      printf("   Disabling PMIP mode ... \n");
      set_pmip_mode_req_msg.enable_pmip_mode = pmip_mode_enabled;
      if (QcMapClient->SetPMIPMode(&set_pmip_mode_req_msg, &qmi_err_num))
        printf("SetPMIPMode to Disable succeeded \n");
      else
        printf("SetPMIPMode fails, Error: 0x%x\n", qmi_err_num);
    }
    else
    {
      printf("\nInvalid option - %d\n", pmip_mode_enabled);
    }
    break;
  }

  /* Get PMIP mode configuration */
  case 31:
  {
    int  pmip_mode_enabled = 0;
    int pmip_mode_type = 0;
    struct ps_in_addr lma_v4_ip;
    struct ps_in6_addr lma_v6_ip;
    qmi_error_type_v01 qmi_err_num;
    /* Initialize QMI Error Number. */
    qmi_err_num = QMI_ERR_NONE_V01;
    char scan_string[QCMAP_MSGR_MAX_FILE_PATH_LEN];
    qcmap_msgr_get_pmip_mode_resp_msg_v01  get_pmip_mode_resp_msg;
    memset( (void*)&get_pmip_mode_resp_msg, 0x0, sizeof(qcmap_msgr_get_pmip_mode_resp_msg_v01));
    printf("  Getting PMIP mode: ");
    if (QcMapClient->GetPMIPMode(&get_pmip_mode_resp_msg, &qmi_err_num))
    {
      char  ip6_addr_str[INET6_ADDRSTRLEN];
      char  ip4_addr_str[INET_ADDRSTRLEN];
      printf("GetPMIPmode succeeds.\n---- Status: %d----\n",get_pmip_mode_resp_msg.pmip_mode);
      printf("---- pmip_mode_type: %d----\n",get_pmip_mode_resp_msg.pmip_mode_type);
      if (get_pmip_mode_resp_msg.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4_V01)
      {
        printf ("Pmip is in v4 Mode\n");
      }
      else if (get_pmip_mode_resp_msg.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V6_V01)
      {
        printf ("Pmip is in v6 Mode\n");
      }
      else if (get_pmip_mode_resp_msg.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
      {
        printf ("Pmip is in V4V6 Mode\n");
      }
      else
      {
        printf ("Pmip is in Invalid Mode\n");
      }
      if(get_pmip_mode_resp_msg.lma_ipv6_addr_valid == 1)
      {
        inet_ntop(AF_INET6, (uint8_t*)get_pmip_mode_resp_msg.lma_ipv6_addr,
                  ip6_addr_str, INET6_ADDRSTRLEN);
        printf("\nTunnel Mode is v6\n");
        printf("---- LMA V6 Addres: %s----\n",ip6_addr_str);
      }
      else if(get_pmip_mode_resp_msg.lma_ipv4_addr_valid == 1)
      {
        inet_ntop(AF_INET, (void *)&get_pmip_mode_resp_msg.lma_ipv4_addr,
                  ip4_addr_str, INET_ADDRSTRLEN);
        printf("\nTunnel Mode is v4\n");
        printf("---- LMA V4 Addres: %s----\n",ip4_addr_str);
      }
      else
      {
        printf("\nEither Tunnel Mode is Invalid Or Tunnel Ip not provided\n");
      }
      if (get_pmip_mode_resp_msg.pmip_mobile_node_identifier_type_valid == true)
      {
        if (get_pmip_mode_resp_msg.pmip_mobile_node_identifier_type == QCMAP_MSGR_PMIP_MOBILE_NODE_IDENTIFIER_STRING_V01)
        {
          printf ("\nPmip Mobile node Identifier type is String.\n");
          if (get_pmip_mode_resp_msg.pmip_mn_id_string_valid)
            printf ("\nPmip Mobile node Identifier type is string %s\n",get_pmip_mode_resp_msg.pmip_mn_id_string);
          else
            printf ("\nget_pmip_mode_resp_msg.pmip_mn_id_string_valid flag is false\n");
        }
        else if (get_pmip_mode_resp_msg.pmip_mobile_node_identifier_type == QCMAP_MSGR_PMIP_MOBILE_NODE_IDENTIFIER_MAC_V01)
        {
          printf ("\nPmip Mobile node Identifier type is MAC address.\n");
        }
        else
        {
          printf("\nInvalid pmip_mobile_node_identifier_type %d\n",get_pmip_mode_resp_msg.pmip_mobile_node_identifier_type);
        }
      }
      else
      {
        printf("\npmip_mobile_node_identifier_type not provided by server\n");
      }

      if (get_pmip_mode_resp_msg.pmip_service_selection_string_valid == true)
      {
        printf ("\nPmip Service Selection string '%s'\n",get_pmip_mode_resp_msg.pmip_service_selection_string);
      }
      else
      {
        printf ("\nPmip Service Selection string not provided by server\n");
      }

      if (get_pmip_mode_resp_msg.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4_V01 ||
          get_pmip_mode_resp_msg.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
      {
        if (get_pmip_mode_resp_msg.pmipv4_mode_type == QCMAP_MSGR_PMIPV4_MODE_CPE_V01)
        {
          printf ("\nPmip v4 is in CPE Mode\n");
        }
        else if (get_pmip_mode_resp_msg.pmipv4_mode_type == QCMAP_MSGR_PMIPV4_MODE_SECONDARY_ROUTER_V01)
        {
          printf ("\nPmip v4 is in Secondary Router Mode\n");
          inet_ntop(AF_INET, (void *)&get_pmip_mode_resp_msg.pmipv4_sec_router_param.dmnp_prefix,
                    ip4_addr_str, INET_ADDRSTRLEN);
          printf("\n---- Secondary Router Mode Prefix: %s----\n",ip4_addr_str);
          printf ("\nSecondary Router Mode prefix Length %d\n",
                  get_pmip_mode_resp_msg.pmipv4_sec_router_param.prefix_len);
        }
        else
        {
          printf ("\nPmip v4  mode getting failed mode type %d \n",get_pmip_mode_resp_msg.pmipv4_mode_type);
        }
      }
    }
    else
      printf("GetPMIPmode fails, Error: 0x%x\n", qmi_err_num);
    break;
  }

  /* Get WWAN Roaming status*/
  case 32:
  {
    uint8_t roam_status = 0;
    if (!QcMapClient->GetWWANRoamStatus(&roam_status,&qmi_err_num))
    {
      printf("Error getting the Roaming status 0x%x.\n ", qmi_err_num);
      break;
    }
    printf("WWAN Roaming status :%d \n",roam_status);
    break;
  }

  /*Get current profile handle*/
  case 33:
  {
    profile_handle_type_v01 current_profile_handle;
    if (!QcMapClient->GetWWANProfilePreference(&current_profile_handle, &qmi_err_num))
    {
      printf("Error getting current profile handle 0x%x.\n ", qmi_err_num);
      break;
    }
    printf("Current Profile Handle :%d \n",current_profile_handle);
    break;
  }
  default :
  {
    printf("Invalid response %d\n", backhaulWWANOpt);
  }
  break;
  }
}

void tetheringConfig( int tetheOpt )
{
  qmi_error_type_v01 qmi_err_num;
  /* Initialize QMI Error Number. */
  qmi_err_num = QMI_ERR_NONE_V01;
  char scan_string[QCMAP_MSGR_MAX_FILE_PATH_LEN];

  /* Tethering Config Options */
  switch(tetheOpt)
  {
  /* Set Cradle Mode */
  case 1:
  {
  /* Set Cradle Mode */
  /* Only  QCMAP_MSGR_CRADLE_WAN_ROUTER_V01 is supported*/
  printf("   Possible Cradle Modes\n");
  printf("   Disabled = 0\n");
  printf("   LAN Bridge = 1\n");
  printf("   LAN Router = 2\n");
  printf("   WAN Bridge = 3\n");
  printf("   WAN Router = 4\n");
  printf("   Please input cradle mode (0-4):");
  fgets(scan_string, sizeof(scan_string), stdin);

  if (atoi(scan_string) >= 0 && atoi(scan_string) <= 4)
  {
   if (QcMapClient->SetCradleMode(atoi(scan_string), &qmi_err_num))
   {
      printf("\nMobile AP Cradle Mode has been set\n");
   }
   else
   {
     switch(qmi_err_num)
     {
       case(QMI_ERR_INCOMPATIBLE_STATE_V01):
       {
         printf("\nCrade mode could not be set...\n"
                "Please delete all VLANs first.\n");
         break;
       }
       default:
       {
         printf("\nFailed to Set Cradle Mode: Error 0x%x.\n ", qmi_err_num);
         break;
       }
     }
   }

  }
  else
   printf("\n   %s is invalid, please select a valid option\n", scan_string);

  break;
  }

  /* Get Cradle Mode */
  case 2:
  {
  /* Get Cradle Mode/Status */
  qcmap_msgr_cradle_mode_v01 mode;
  if (QcMapClient->GetCradleMode(&mode, &qmi_err_num))
  {
   /* Only  QCMAP_MSGR_CRADLE_WAN_ROUTER_V01 is supported*/
   switch (mode)
   {
      case QCMAP_MSGR_CRADLE_DISABLED_V01:
        printf("\nMobile AP Cradle Mode is Disabled");
        break;
      case QCMAP_MSGR_CRADLE_LAN_BRIDGE_V01:
        printf("\nMobile AP Cradle Mode is LAN BRIDGE");
        break;
      case QCMAP_MSGR_CRADLE_LAN_ROUTER_V01:
        printf("\nMobile AP Cradle Mode is LAN ROUTER");
        break;
      case QCMAP_MSGR_CRADLE_WAN_BRIDGE_V01:
        printf("\nMobile AP Cradle Mode is WAN BRIDGE");
        break;
      case QCMAP_MSGR_CRADLE_WAN_ROUTER_V01:
        printf("\nMobile AP Cradle Mode is WAN ROUTER");
        break;
      default:
        printf("\nIncorrect state returned: 0x%x", mode);
        break;
   }
  }
  else
   printf("  Failed to Get Cradle Mode .Error 0x%x.\n ", qmi_err_num);
  break;
  }

  /* Set Ethernet mode. */
  case 3:
  {
  printf("   Possible Ethernet Modes\n");
  printf("   LAN Router = 0\n");
  printf("   WAN Router = 1\n");
  printf("   Please input Ethernet mode (0-1):");
  fgets(scan_string, sizeof(scan_string), stdin);

  if (atoi(scan_string) >= 0 && atoi(scan_string) < 2)
  {
   if (QcMapClient->SetEthernetMode(atoi(scan_string), &qmi_err_num))
   {
      printf("\nMobile AP Ethernet has been set\n");
   } else {
      switch(qmi_err_num)
      {
        case(QMI_ERR_INCOMPATIBLE_STATE_V01):
        {
          printf("\nEth backhaul can not be enabled...\n"
                 "Please delete all VLANs first.\n");
          break;
        }
        default:
        {
          printf("\nFailed to Set Ethernet Mode: Error 0x%x.\n ",
                 qmi_err_num);
          break;
        }
      }
    }
  }
  else
   printf("\n   %s is invalid, please select a valid option\n",
          scan_string);

  break;
  }


  /* Get Ethernet mode. */
  case 4:
  {
  qcmap_msgr_ethernet_mode_v01 mode;
  if (QcMapClient->GetEthernetMode(&mode, &qmi_err_num))
  {
   switch (mode)
   {
      case QCMAP_MSGR_ETHERNET_LAN_ROUTER_V01:
        printf("\nMobile AP Ethernet Mode is LAN ROUTER");
        break;
      case QCMAP_MSGR_ETHERNET_WAN_ROUTER_V01:
        printf("\nMobile AP Ethernet Mode is WAN ROUTER");
        break;
      default:
        printf("\nIncorrect state returned: 0x%x", mode);
        break;
   }
  }
  else
   printf("  Failed to Get Ethernet Mode .Error 0x%x.\n ",
          qmi_err_num);
  break;
  }

  /* Get BT Tethering Status. */
  case 5:
    qcmap_msgr_bt_tethering_status_enum_v01 bt_teth_status;
    qcmap_bt_tethering_mode_enum_v01 bt_teth_mode;
    if (QcMapClient->GetBTTetheringStatus(&bt_teth_status, &qmi_err_num, &bt_teth_mode))
    {
      if (bt_teth_status == QCMAP_MSGR_BT_TETHERING_MODE_UP_V01)
      {
        printf("\n BT Tethering is UP in");
        printf("Mode: %s\n", ((bt_teth_mode == QCMAP_MSGR_BT_MODE_WAN_V01) ? "WAN" : "LAN"));
      }
      else
        printf("BT Tethering is DOWN\n");
    }
    else
      printf("  Failed to Get BT Tethering Status .Error 0x%x.\n ",
          qmi_err_num);
    break;

  /*Set Dun Dongle Mode*/
  case 6:
  {
    boolean dun_dongle_mode_state = false;
    printf("   Please input Dundonglemode Flag (1-Enable/0-Disable):");
    fgets(scan_string, sizeof(scan_string), stdin);
    if (atoi(scan_string) == 0 || atoi(scan_string) == 1)
    {
      dun_dongle_mode_state = (atoi(scan_string)) ? true : false;
      if (QcMapClient->SetDunDongleMode(dun_dongle_mode_state, &qmi_err_num))
      {
        printf("\nDUN Dongle Mode config set succeeds.");
      }
      else
      {
        printf("\nDUN Dongle Mode config set fails, Error: 0x%x",
               qmi_err_num);
      }
    }
    else
    {
      printf("\n   %s is invalid, please select a valid option\n",
             scan_string);
    }
    break;
  }

  /*get Dun Dongle Mode*/
  case 7:
  {
    boolean dun_dongle_mode_status = false;
    if (QcMapClient->GetDunDongleMode(&dun_dongle_mode_status, &qmi_err_num))
    {
      printf("\nDUN Dongle Mode: %s.",
                (dun_dongle_mode_status)?"Enabled":"Disabled");
    }
    else
    {
      printf("\nDUN Dongle Mode config get fails, Error: 0x%x",
             qmi_err_num);
    }
    break;
  }

  default :
    printf("Invalid response %d\n", tetheOpt);
    break;
  }
}

void mediaServiceConfig(int medServOpt)
{
  qmi_error_type_v01 qmi_err_num;
  /* Initialize QMI Error Number. */
  qmi_err_num = QMI_ERR_NONE_V01;
  char scan_string[QCMAP_MSGR_MAX_FILE_PATH_LEN];
  in_addr addr;

  /* MediaService Config Options */
  switch(medServOpt)
  {
    /* Enable/Disable UPNP */
    case 1:
      printf("   Please input UPnP State (1-Enable/0-Disable) : ");
      fgets(scan_string, sizeof(scan_string), stdin);
      if (atoi(scan_string))
      {
      if (QcMapClient->EnableUPNP(&qmi_err_num))
        printf("\nUPNP Enable succeeds.");
      else
        printf("\nUPNP Enable fails, Error: 0x%x", qmi_err_num);
      }
      else
      {
        if (QcMapClient->DisableUPNP(&qmi_err_num))
        printf("\nUPNP Disabled.");
        else
        printf("\nUPNP Disable request fails, Error: 0x%x", qmi_err_num);
      }
    break;

    /* Get UpNp status */
    case 2:
    {
      qcmap_msgr_upnp_mode_enum_v01 upnp_state;
      if(QcMapClient->GetUPNPStatus( &upnp_state, &qmi_err_num))
      {
        if (upnp_state == QCMAP_MSGR_UPNP_MODE_UP_V01)
        {
          printf("\nUPnP is enabled");
        }
        else
        {
          printf("\nUPnP is disabled");
        }
      }
      else
      {
        printf("\nGetUPNPStatus returns Error: 0x%x", qmi_err_num);
      }
    break;
      }

    /* Set UPnP notify interval */
    case 3:
    {
      int upnp_notify_int;
      qcmap_msgr_upnp_mode_enum_v01 upnp_state;

      printf("   Please input UPnP notify interval in seconds (%d-%d):",
      MIN_NOTIFY_INTERVAL, MAX_NOTIFY_INTERVAL);
      fgets(scan_string, sizeof(scan_string), stdin);
      upnp_notify_int = atoi(scan_string);
      if (upnp_notify_int >= MIN_NOTIFY_INTERVAL && upnp_notify_int <= MAX_NOTIFY_INTERVAL)
      {
        printf("\nUPnP notify interval set!\n");
        if(QcMapClient->SetUPNPNotifyInterval(upnp_notify_int, &qmi_err_num))
        {
          if (QcMapClient->GetUPNPStatus( &upnp_state, &qmi_err_num))
          {
            if (upnp_state == QCMAP_MSGR_UPNP_MODE_UP_V01)
            {
              printf("\nThis change will not take effect until restart.\n");
              printf("   Do you want to restart now? (1-yes/0-no) : ");
              fgets(scan_string, sizeof(scan_string), stdin);
              if (atoi(scan_string) == 1)
              {
                if (QcMapClient->DisableUPNP(&qmi_err_num))
                {
                  printf("\nUPNP has been stopped.");
                  if (QcMapClient->EnableUPNP(&qmi_err_num))
                    printf("\nUPNP Restart succeeds.");
                  else
                    printf("\nUPNP Restart fails, Error: 0x%x", qmi_err_num);
                }
              else
                printf("\nUPNP Restart fails, Error: 0x%x", qmi_err_num);
              }
            }
          else
            printf("\nGetUPNPStatus returns Error: 0x%x", qmi_err_num);
          }
        }
        else
        {
          printf("\nUPnP notify interval returns Error: 0x%x", qmi_err_num);
        }
      }
      else
        printf("      Invalid UPnP notify interval, must be in range %d-%d: %s",
        MIN_NOTIFY_INTERVAL, MAX_NOTIFY_INTERVAL, scan_string);
    break;
    }
    /* Get UPnP notify interval */
    case 4:
    {
      int upnp_notify_int = 0;

      if(QcMapClient->GetUPNPNotifyInterval(&upnp_notify_int, &qmi_err_num))
      {
        printf("\nCurrent UPnP notify interval: %d\n", upnp_notify_int);
      }
      else
      {
        printf("\nUPnP notify interval returns Error: 0x%x", qmi_err_num);
      }
    break;
    }

    /* Set UPNPPinhole State */
    case 5:
    {
      boolean enable_firewall, pkts_allowed = false;
      boolean upnp_pinhole_allow =false;
      QcMapClient->GetFirewall(&enable_firewall, &pkts_allowed,&qmi_err_num);
      if(enable_firewall)
      {
        printf("   Please input UPNP Pinhole Allow State (1-Enable/0-Disable) : ");
        fgets(scan_string, sizeof(scan_string), stdin);
        upnp_pinhole_allow = atoi(scan_string);
        if (QcMapClient->SetUPNPState(enable_firewall,upnp_pinhole_allow, &qmi_err_num))
        {
          printf("Set UPNP state success\n");
        }
        else
        {
          printf("Set UPNP state Error: 0x%x", qmi_err_num);
        }
      }
      else
        printf(" Firewall is DISABLED. UPNP NOT ALLOWED " );
      break;
      }

    /* Get UPNPPinhole State */
    case 6:
    {
      boolean upnp_pinhole_flag;
      if (QcMapClient->GetUPNPState(&upnp_pinhole_flag,&qmi_err_num))
      {
        if(upnp_pinhole_flag)
          printf("UPNP Pinhole is allowed\n");
        else
          printf("UPNP Pinhole is NOT allowed \n");
      }
      else
        printf("Get UPNP Pinhole configuration failed,Error 0x%x", qmi_err_num);
    break;
    }

    /* Enable/Disable DLNA */
    case 7:
      printf("   Please input DLNA State (1-Enable/0-Disable) : ");
      fgets(scan_string, sizeof(scan_string), stdin);
      if (atoi(scan_string))
      {
        if (QcMapClient->EnableDLNA(&qmi_err_num))
          printf("\nDLNA Enable succeeds.");
        else
          printf("\nDLNA Enable fails, Error: 0x%x", qmi_err_num);
      }
      else
      {
        if (QcMapClient->DisableDLNA(&qmi_err_num))
          printf("\nDLNA Disabled.");
        else
          printf("\nDLNA Disable request fails, Error: 0x%x", qmi_err_num);
      }
    break;

    /* Set DLNA notify interval */
    case 8:
    {
      int dlna_notify_int;
      qcmap_msgr_dlna_mode_enum_v01 dlna_state;

      printf("   Please input DLNA notify interval in seconds (%d-%d):",
      MIN_NOTIFY_INTERVAL, MAX_NOTIFY_INTERVAL);
      fgets(scan_string, sizeof(scan_string), stdin);
      dlna_notify_int = atoi(scan_string);
      if (dlna_notify_int >= MIN_NOTIFY_INTERVAL && dlna_notify_int <= MAX_NOTIFY_INTERVAL)
      {
        if(QcMapClient->SetDLNANotifyInterval(dlna_notify_int, &qmi_err_num))
        {
          printf("\nDLNA notify interval set!\n");
          if(QcMapClient->GetDLNAStatus( &dlna_state, &qmi_err_num))
          {
            if (dlna_state == QCMAP_MSGR_DLNA_MODE_UP_V01)
            {
              printf("\nThis change will not take effect until restart.\n");
              printf("   Do you want to restart now? (1-yes/0-no) : ");
              fgets(scan_string, sizeof(scan_string), stdin);
              if (atoi(scan_string) == 1)
              {
                if (QcMapClient->DisableDLNA(&qmi_err_num))
                {
                  printf("\nDLNA has been stopped.");
                  if (QcMapClient->EnableDLNA(&qmi_err_num))
                    printf("\nDLNA Restart succeeds.");
                  else
                    printf("\nDLNA Restart fails, Error: 0x%x", qmi_err_num);
                }
                else
                  printf("\nDLNA Restart fails, Error: 0x%x", qmi_err_num);
              }
            }
          }
          else
            printf("\nGetDLNAStatus returns Error: 0x%x", qmi_err_num);
        }
        else
        {
          printf("\nDLNA notify interval returns Error: 0x%x", qmi_err_num);
        }
      }
      else
        printf("      Invalid DLNA notify interval, must be in range %d-%d: %s",
        MIN_NOTIFY_INTERVAL, MAX_NOTIFY_INTERVAL, scan_string);
    break;
    }
    /* Get DLNA notify interval */
    case 9:
    {
      int dlna_notify_int = 0;

      if(QcMapClient->GetDLNANotifyInterval(&dlna_notify_int, &qmi_err_num))
      {
        printf("\nCurrent DLNA notify interval: %d\n", dlna_notify_int);
      }
      else
      {
        printf("\nDLNA notify interval returns Error: 0x%x", qmi_err_num);
      }
    break;
    }

    /* get DLNA status */
    case 10:
    {
      qcmap_msgr_dlna_mode_enum_v01 dlna_state;
      if(QcMapClient->GetDLNAStatus( &dlna_state, &qmi_err_num))
      {
        if (dlna_state == QCMAP_MSGR_DLNA_MODE_UP_V01)
        {
          printf("\nDLNA is enabled");
        }
        else
        {
          printf("\nDLNA is disabled");
        }
      }
      else
      {
      printf("\nGetDLNAStatus returns Error: 0x%x", qmi_err_num);
      }
    break;
    }

    /* set DLNA media directory */
    case 11:
    {
      char media_dir_get[QCMAP_MSGR_MAX_DLNA_DIR_LEN_V01] = "";
      char media_dir_set[QCMAP_MSGR_MAX_DLNA_DIR_LEN_V01] = "";
      char *ptr;

      if(QcMapClient->GetDLNAMediaDir( media_dir_get, &qmi_err_num))
      {
        printf("\nCurrent DLNA Media Dir('s):");
        printf("\n%s\n", media_dir_get);
        printf("   Do you wish to keep these Directories?(1-YES/0-NO) : ");

        fgets(scan_string, sizeof(scan_string), stdin);
        int enable = (atoi(scan_string)) ? true : false;

        if (enable)
        {
          strlcpy(media_dir_set, media_dir_get, sizeof(media_dir_set));

          //replace all newlines with ','
          ptr = strchr(media_dir_set, '\n');
          while (ptr != NULL)
          {
            media_dir_set[ptr-media_dir_set] = ',';
            ptr = strchr(ptr+1, '\n');
          }
          strlcat(media_dir_set, ",", sizeof(media_dir_set));
        }
      }
      printf("   Please input a valid Media Directory (\",\" to seperate multiple):");
      if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
      {
        strlcat(media_dir_set, scan_string, sizeof(media_dir_set));
        if(QcMapClient->SetDLNAMediaDir( media_dir_set, &qmi_err_num))
        {
          printf("\nDLNA Media Dir added!\n");
        }
        else
        {
          if (qmi_err_num != QMI_ERR_NO_EFFECT_V01)
          {
          printf("\nSetDLNAMediaDir returns Error: 0x%x", qmi_err_num);
          }
          else
          {
          printf("\nSetDLNAMediaDir succeeds but restart failed");
          }
        }
      }
      else
        printf("      Invalid Media Directory: %s", scan_string);
    break;
    }

    /* Get DLNA media directory */
    case 12:
    {
      char media_dir[QCMAP_MSGR_MAX_DLNA_DIR_LEN_V01] = "";
      if(QcMapClient->GetDLNAMediaDir( media_dir, &qmi_err_num))
      {
        printf("\nCurrent DLNA Media Dir('s):");
        printf("\n%s\n", media_dir);
      }
      else
      {
        printf("\nGetDLNAMediaDir returns Error: 0x%x", qmi_err_num);
      }
    break;
    }

    /* Set DLNA Whitelisting. */
    case 13:
    {
      boolean dlna_whitelist_ip_flag = false;
      printf("   Please Set DLNA Whitelisting State (1-Enable/0-Disable) : ");
      fgets(scan_string, sizeof(scan_string), stdin);
      dlna_whitelist_ip_flag = atoi(scan_string);
      if (QcMapClient->SetDLNAWhitelisting(dlna_whitelist_ip_flag,&qmi_err_num))
      {
        printf("Set DLNA Whitelisting status successful\n");
      }
      else
      {
        printf("Set DLNA Whitelisting status Error: 0x%x", qmi_err_num);
      }
    break;
    }

    /* Get DLNA Whitelisting */
    case 14:
    {
      boolean dlna_whitelist_ip_flag = false;
      if (QcMapClient->GetDLNAWhitelisting(&dlna_whitelist_ip_flag, &qmi_err_num))
      {
        if(dlna_whitelist_ip_flag == 1)
          printf("DLNA Whitelisting is Enabled\n");
        else
          printf("DLNA Whitelisting is Disabled \n");
      }
      else
        printf("Get DLNA Whitelisting configuration failed,Error 0x%x", qmi_err_num);
    break;
    }

    /* Add DLNAWhitelistingIP */
    case 15:
    {
      uint32 dlna_whitelist_ip;
      memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);
      while (TRUE)
      {
        printf("   Please input DLNA whitelist IP to add(xxx.xxx.xxx.xxx) : ");
        if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
        {
          if ( !(inet_aton(scan_string, &addr) <=0 ))
            break;
        }
        printf("   Invalid IPv4 address %d\n", scan_string);
      }
      dlna_whitelist_ip = ntohl(addr.s_addr);

      if (QcMapClient->AddDLNAWhitelistIP(dlna_whitelist_ip, &qmi_err_num))
      {
        printf("\nDLNA Whitelisting IP added successfully");
      }
      else if ( qmi_err_num == QMI_ERR_NO_EFFECT_V01 )
      {
        printf(" DLNA Whitelisting IP is already present. \n");
      }
      else
        printf("\nDLNA Whitelisting IP add fails. Error: 0x%x", qmi_err_num);
    break;
    }

    /* Delete DLNAWhitelistingIP */
    case 16:
    {
      uint32 dlna_whitelist_ip=0;
      memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);
      memset(&dlna_whitelist_ip, 0, sizeof(uint32));

      while (TRUE)
      {
        printf("   Please input DLNA whitelist IP to delete(xxx.xxx.xxx.xxx) : ");
        if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
        {
          if ( !(inet_aton(scan_string, &addr) <=0 ))
          break;
        }
        printf("   Invalid IPv4 address %d\n", scan_string);
      }
      dlna_whitelist_ip = ntohl(addr.s_addr);

      if (QcMapClient->DeleteDLNAWhitelistIP(dlna_whitelist_ip, &qmi_err_num))
      {
        printf("\nDLNA Whitelisting IP deleted successfully");
      }
      else
        printf("\nDLNA Whitelisting IP add fails. Error: 0x%x", qmi_err_num);
    break;
    }

    /* Enable / Disable M-DNS */
    case 17:
    {
      printf("   Please input M-DNS State (1-Enable/0-Disable) : ");
      fgets(scan_string, sizeof(scan_string), stdin);
      if (atoi(scan_string))
      {
        if (QcMapClient->EnableMDNS(&qmi_err_num)) {
          printf("\n M-DNS Enable succeeds.");
          } else {
          if (qmi_err_num == QMI_ERR_NO_EFFECT_V01) {
            printf("\n M-DNS Already Enabled \n");
          } else {
          printf("\n M-DNS Enable fails, Error: 0x%x \n", qmi_err_num);
          }
          }
        } else {
          if (QcMapClient->DisableMDNS(&qmi_err_num)) {
            printf("\n M-DNS Disable in progress.");
          } else {
          if (qmi_err_num == QMI_ERR_NO_EFFECT_V01) {
            printf("\n M-DNS Already Disabled \n");
          } else {
            printf("\n M-DNS Disable fails, Error: 0x%x \n", qmi_err_num);
          }
          }
       }
    }
    break;

    /* Get MDNS status. */
    case 18:
    {
      qcmap_msgr_mdns_mode_enum_v01 mdns_state;
      if(QcMapClient->GetMDNSStatus( &mdns_state, &qmi_err_num))
      {
        if (mdns_state == QCMAP_MSGR_MDNS_MODE_UP_V01)
        {
          printf("\nMDNS is enabled");
        }
        else
        {
          printf("\nMDNS is disabled");
        }
      }
      else
      {
        printf("\nGetMDNSStatus returns Error: 0x%x", qmi_err_num);
      }
      break;
    }
    default :
    {
      printf("Invalid response %d\n", medServOpt);
    }
    break;
  }
}

void gsbConfig(int gsbOpt)
{
  qmi_error_type_v01 qmi_err_num;
  /* Initialize QMI Error Number. */
  qmi_err_num = QMI_ERR_NONE_V01;
  char scan_string[QCMAP_MSGR_MAX_FILE_PATH_LEN];
  qcmap_msgr_gsb_config_v01 conf;
  uint8 num_of_if = 0;
  qcmap_msgr_gsb_config_v01 conf_arr[QCMAP_MSGR_MAX_IF_SUPPORTED_V01];
  char if_name[QCMAP_MAX_IFACE_NAME_SIZE_V01];
  int ix = 0;
  boolean canSet = true;
  in_addr addr;
  memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);
  memset(&addr, 0, sizeof(in_addr));
  memset(&conf, 0, sizeof(conf));

  /* GSB Configuration options */
  switch(gsbOpt)
  {
  /* Set GSB Config*/
  case 1:
  {
    printf("Enter IF name (example wlan0 etc, please be careful of case): \n");
    fgets(scan_string, sizeof(scan_string), stdin);
    if (strlen(scan_string)-1 >= QCMAP_MAX_IFACE_NAME_SIZE_V01)
    {
      printf("ERROR: Overflow..Supports only 16 char interface size\n");
      break;
    }

    for ( ix=0;ix < strlen(scan_string)-1;ix++)
    {
      if ((scan_string[ix] >='a' && scan_string[ix] <= 'z') ||
          (scan_string[ix] >= '0' && scan_string[ix] <= '9' ))
      {
        conf.if_name[ix] = scan_string[ix];
      }
      else
      {
        printf("ERROR: special char found in interface name\n");
        return;
      }
    }
    printf("you added %s interface\n", conf.if_name);

    printf(" Add bandwidth requirement for IF (Max 900 Mbps)\n");
    while(TRUE)
    {
      memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);
      fgets(scan_string, sizeof(scan_string), stdin);
      conf.bw_reqd_in_mb = atoi(scan_string);
      if (conf.bw_reqd_in_mb > 0  && conf.bw_reqd_in_mb <= 900 ) {
        printf("BW added  %d\n", conf.bw_reqd_in_mb);
        break;
      } else {
        printf("Please add valid bw requirement\n");
      }
    }

    printf(" Add high watermark value(max 600)\n");
    while(TRUE)
    {
      fgets(scan_string, sizeof(scan_string), stdin);
      conf.if_high_watermark = atoi(scan_string);
      if (conf.if_high_watermark > 0 && conf.if_high_watermark <=600) {
        printf("high wm added  %d\n", conf.if_high_watermark);
        break;
      } else {
        printf("Please add valid high wm requirement\n");
      }
    }

    printf(" Add low watermark value\n");
    while(TRUE)
    {
      memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);
      fgets(scan_string, sizeof(scan_string), stdin);
      if ((conf.if_low_watermark = atoi(scan_string)) > 0) {
        printf("low wm added  %d\n", conf.if_low_watermark);
        break;
      } else {
        printf("Please add valid low wm requirement\n");
      }
    }

    printf(" Specify IF type(1-WLAN-AP, 2-WLAN-STA,3-ETH)\n");
    while(TRUE)
    {
      memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);
      fgets(scan_string, sizeof(scan_string), stdin);
      conf.if_type = (qcmap_msgr_gsb_interface_type_enum_v01)atoi(scan_string);
      if ( conf.if_type < QCMAP_MSGR_INTERFACE_TYPE_WLAN_AP_V01 ||
           conf.if_type > QCMAP_MSGR_INTERFACE_TYPE_ETHERNET_V01)
      {
        printf("Please enter a valid IF type\n");
        continue;
      }
      else if (conf.if_type == QCMAP_MSGR_INTERFACE_TYPE_WLAN_AP_V01 ||
               conf.if_type == QCMAP_MSGR_INTERFACE_TYPE_WLAN_STA_V01)
      {
        printf("WLAN IF type is dynamically supported on reference platform.No GSB config required\n");
        canSet = false;
      }
      break;
    }

    /*ap ip for QCMAP is managed by QCMAP server.*/
    while (0)
    {
      printf(" Specify ap_ip\n");
      if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
      {
        if ( !(inet_aton(scan_string, &addr) <= 0) )
        {
          conf.ap_ip = ntohl(addr.s_addr);
          printf("conf Ip set 0x%X, %s\n", conf.ap_ip, inet_ntoa(addr));
          break;
        }
      }
      printf("Invalid IPv4 address %s\n", scan_string);
    }

    if(canSet && QcMapClient->SetGSBConfig(&conf, &qmi_err_num))
    {
      if (qmi_err_num ==  QMI_ERR_NONE_V01) {
        printf("GSB Config Set Successfully\n");
      }
    }
    else
    {
      printf("GSB Config set fails, Error: 0x%x\n", qmi_err_num);
    }
  }
  break;

  /* Enable/Disable GSB*/
  case 2:
  {
    memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);
    printf("Please input GSB State(1-Enable/0-Disable) : ");
    fgets(scan_string, sizeof(scan_string), stdin);
    if (atoi(scan_string))
    {
      if(QcMapClient->EnableGSB(&qmi_err_num))
      {
       printf("Enabled GSB\n");
      }
      else
       printf("Enable GSB fails, Error: 0x%x\n", qmi_err_num);
    }
    else
    {
      if(QcMapClient->DisableGSB(&qmi_err_num))
      {
       printf("Disabled GSB\n");
      }
      else
       printf("Disable GSB fails, Error: 0x%x\n", qmi_err_num);
    }
  }
  break;

  /* Get GSB Config */
  case 3:
  {
    memset(conf_arr, 0, sizeof(qcmap_msgr_gsb_config_v01)*QCMAP_MSGR_MAX_IF_SUPPORTED_V01);
    memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);
    if (QcMapClient->GetGSBConfig(conf_arr,&num_of_if, &qmi_err_num))
    {
      if (num_of_if>0)
      {
        printf("Total IF Configured %d\n\n\n",num_of_if );
        for (int i = 0; i < num_of_if; i++)
        {
          printf("IF Name %s\n", conf_arr[i].if_name);
          switch (conf_arr[i].if_type)
          {
          case QCMAP_MSGR_INTERFACE_TYPE_WLAN_AP_V01:
            printf("WLAN device type(AP mode)\n");
            break;
          case QCMAP_MSGR_INTERFACE_TYPE_WLAN_STA_V01:
            printf("WLAN device type(STA mode)\n");
            break;
          case QCMAP_MSGR_INTERFACE_TYPE_ETHERNET_V01:
            printf("ETH device type\n");
            break;
          default:
            printf("UNKNOWN device type\n");
            break;
          }
          printf("BW_reqd :%d\n", conf_arr[i].bw_reqd_in_mb);
          printf("low_wm :%d\n", conf_arr[i].if_low_watermark);
          printf("high_wm :%d\n", conf_arr[i].if_high_watermark);
          printf("\n\n");
        }
      }
      else
      {
        printf("No IF is configured to work with GSB\n");
      }

    }
    else
    {
      printf("Get GSB Config failed, Error:0x%x\n", qmi_err_num);
    }
  }
  break;

  /* Delete GSB Config*/
  case 4:
  {
    memset(if_name, 0, QCMAP_MAX_IFACE_NAME_SIZE_V01);
    memset(scan_string, 0, QCMAP_MSGR_MAX_FILE_PATH_LEN);

    printf("Enter IF name whose config need to be deleted (example wlan0 etc, please be careful of case):\n");
    fgets(scan_string, sizeof(scan_string), stdin);
    if (strlen(scan_string)-1 >= QCMAP_MAX_IFACE_NAME_SIZE_V01)
    {
      printf("ERROR: Overflow..Supports only 16 char interface size\n");
      break;
    }
    for ( ix=0;ix < strlen(scan_string)-1;ix++)
    {
      if ((scan_string[ix] >='a' && scan_string[ix] <= 'z') ||
          (scan_string[ix] >= '0' && scan_string[ix] <= '9' ))
      {
        if_name[ix] = scan_string[ix];
      }
      else
      {
        printf("ERROR: Unwanted Charaters in interface name\n");
        return;
      }
    }

    printf("The IF you want to delete is %s\n", if_name);
    if((if_name != NULL) && QcMapClient->DeleteGSBConfig(if_name, &qmi_err_num))
    {
      printf("Deleted GSB Conifg\n");
    }
    else
      printf("Delete GSB Config fails, Error: 0x%x\n", qmi_err_num);
  }
  break;

  default :
  {
    printf("Invalid response %d\n", gsbOpt);
  }
  break;
  }
}

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

  int opt = 0, mobileApOpt = 0, lanOpt = 0, natAlgOpt = 0, wlanOpt = 0, firewallOpt = 0,
  backhaulOpt = 0, backhaulCommOpt = 0, backhaulWWANOpt = 0, tetheOpt = 0, medServOpt = 0,
  gsbOpt = 0, p_error;
  char scan_string[QCMAP_MSGR_MAX_FILE_PATH_LEN];
  uint8 mac_addr_int[QCMAP_MSGR_MAC_ADDR_LEN_V01]; /*byte array of mac address*/
  qmi_error_type_v01  qmi_err_num;
  int array_size = 0;

  /* Register the sighandlers, so the app may be shutdown with a
     kill command.*/
  signal(SIGTERM, sighandler);

  QcMapClient = new QCMAP_Client( qcmap_msgr_qmi_qcmap_ind );

  if (QcMapClient->qmi_qcmap_msgr_handle == 0)
  {
    printf("\nCouldn't setup QcMapClient..exiting");
    sighandler(SIGTERM);
    exit(1);
  }


  while (TRUE)
  {

    /* Display menu of options. */
    printf("\nPlease select an option to test from the items listed below.\n\n");
    array_size = sizeof(options_list)/sizeof(options_list[0]);
    for (int i=0; i<array_size; i++)
    {
      printf("%s\n",options_list[i]);
    }
    printf("Option > ");


    /* Initialize QMI Error Number. */
    qmi_err_num = QMI_ERR_NONE_V01;

    /* Read the option from the standard input. */
    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
      continue;

    /* Convert the option to an integer, and switch on the option entered. */
    opt = atoi(scan_string);

    /* Display submenu's according to main menu option selected above. */
    switch (opt)
    {
      /* A. MobileAP CONFIGURATION SUBMENU */
      case 1:
        array_size = sizeof(mobileAp_configuration_list)/sizeof(mobileAp_configuration_list[0]);
        printf("\n");
        for (int i=0; i<array_size; i++)
        {
          printf("%s\n",mobileAp_configuration_list[i]);
        }
        if(fgets(scan_string, sizeof(scan_string), stdin) == NULL)
          continue;

        mobileApOpt = atoi(scan_string);

        /* mobileApConfig() for mobileApConfig options */
        mobileApConfig(mobileApOpt);
      break;

      /* B. LAN CONFIGURATION SUBMENU */
      case 2:
        array_size = sizeof(lan_configuration_list)/sizeof(lan_configuration_list[0]);
        for (int i=0; i<array_size; i++)
        {
          printf("%s\n",lan_configuration_list[i]);
        }

        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
          continue;

          lanOpt = atoi(scan_string);

        /* lanConfig() for LAN options */
        lanConfig(lanOpt);
      break;


      /* C. NAT/ALG/VPN CONFIGURATION SUBMENU */
      case 3:
        array_size = sizeof(nat_alg_vpn_configuration_list)/sizeof(nat_alg_vpn_configuration_list[0]);
        for (int i=0; i<array_size; i++)
        {
          printf("%s\n",nat_alg_vpn_configuration_list[i]);
        }
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
          continue;

        natAlgOpt = atoi(scan_string);

        /* nat_alg_vpn() for respective options */
        nat_alg_vpn_config(natAlgOpt);
      break;


      /* D. WLAN CONFIGURATION SUBMENU */
      case 4:
       array_size = sizeof(wlan_configuration_list)/sizeof(wlan_configuration_list[0]);
       for (int i=0; i<array_size; i++)
       {
         printf("%s\n",wlan_configuration_list[i]);
       }

       if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
         continue;

       wlanOpt = atoi(scan_string);

         /* WLAN Config options*/
         wlanConfig( wlanOpt );
      break;


       /* E. FIREWALL CONFIGURATION SUBMENU */
       case 5:
         array_size = sizeof(firewall_configuration_list)/sizeof(firewall_configuration_list[0]);
         for (int i=0; i<array_size; i++)
         {
           printf("%s\n",firewall_configuration_list[i]);
         }
         if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
           continue;

         firewallOpt = atoi(scan_string);

         /*Firewall Config options */
         firewallConfig( firewallOpt );
       break;


        /* F. BACKHAUL CONFIGURATION SUBMENU */
        case 6:
          array_size = sizeof(backhaul_configuration_list)/sizeof(backhaul_configuration_list[0]);
          for (int i=0; i<array_size; i++)
          {
            printf("%s\n",backhaul_configuration_list[i]);
          }
          if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;

          backhaulOpt = atoi(scan_string);

         /* Backhual configuration options */
         switch(backhaulOpt)
         {
            /* $ BackhualCOMMON Config SUBMENU */
            case 1:
            {
              array_size = sizeof(backhaul_common_configuration_list)/sizeof(backhaul_common_configuration_list[0]);
              for (int i=0; i<array_size; i++)
              {
                printf("%s\n",backhaul_common_configuration_list[i]);
              }
              if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                continue;

              backhaulCommOpt = atoi(scan_string);
              backhaulCommConfig( backhaulCommOpt );
            }
            break;

            /* $ BackhaulWWAN Config SUBMENU */
            case 2:
            {
              array_size = sizeof(backhaul_wwan_configuration_list)/sizeof(backhaul_wwan_configuration_list[0]);
              for (int i=0; i<array_size; i++)
              {
                printf("%s\n",backhaul_wwan_configuration_list[i]);
              }
              if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                continue;

              backhaulWWANOpt = atoi(scan_string);
              backhaulWWANConfig( backhaulWWANOpt );
            }
            break;

            default :
            {
             printf("Invalid response %d\n", backhaulOpt);
            }
            break;
         }
         break;


         /* G. TETHERING CONFIGURATION SUBMENU */
         case 7:
           array_size = sizeof(tethering_configuration_list)/sizeof(tethering_configuration_list[0]);
           for (int i=0; i<array_size; i++)
           {
             printf("%s\n",tethering_configuration_list[i]);
           }
           if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
             continue;

           tetheOpt = atoi(scan_string);

           tetheringConfig(tetheOpt);
         break;

         /* H. MEDIA SERVICE CONFIGURATION SUBMENU */
         case 8:
           array_size = sizeof(media_service_configuration_list)/sizeof(media_service_configuration_list[0]);
           for (int i=0; i<array_size; i++)
           {
             printf("%s\n",media_service_configuration_list[i]);
           }
             if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
               continue;

           medServOpt = atoi(scan_string);

           mediaServiceConfig(medServOpt);
         break;

         /* I. GENERIC SOFTWARE BRIDGE(GSB) SUBMENU */
         case 9:
           array_size = sizeof(software_bridge_config_list)/sizeof(software_bridge_config_list[0]);
           for (int i=0; i<array_size; i++)
           {
             printf("%s\n",software_bridge_config_list[i]);
           }

           if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
             continue;

           gsbOpt = atoi(scan_string);

           gsbConfig(gsbOpt);
         break;

          /* CLI Debug Options */
          case CLI_DEBUG_OPTION:
          {
            printf(" Supported Dev Test Options for QCMAP CLI \n");
            printf(" 501 ------------- EnableSTAOnlyMode \n");
            printf(" 502 ------------- DisableSTAOnlyMode \n");
            printf(" 503 ------------- RegisterWLANStatusIND \n");
          }
          break;

          /* Enable STA Only Mode */
          case ENABLE_STA_ONLY_DEBUG_MODE:
          {
            if (!QcMapClient->EnableSTAMode(&qmi_err_num))
            {
              printf("STA-Only Mode couldnt be enabled. ERR: 0x%x.\n ", qmi_err_num);
            }
            else
            {
              printf("STA-Only Mode Enabled Successufully.\n ");
            }
            break;
          }

          /* Disable STA Only Mode */
          case DISABLE_STA_ONLY_DEBUG_MODE:
          {
            if (!QcMapClient->DisableSTAMode(&qmi_err_num))
            {
              printf("STA-Only Mode couldnt be Disabled. ERR: 0x%x.\n ", qmi_err_num);
            }
            else
            {
              printf("STA-Only Mode Disabled Successufully.\n ");
            }
            break;
          }

          /* Register for WLAN Status IND */
          case REGISTER_FOR_WLAN_STATUS_IND:
          {
            printf("Register/De-register for WLAN Status (1-Regsiter/0-De-register): ");
            fgets(scan_string, sizeof(scan_string), stdin);
            if (!QcMapClient->RegisterForWLANStatusIND(&qmi_err_num, atoi(scan_string)))
            {
              printf("Registeration failed for WLAN Status IND.ERR 0x%x Input %d\n",
                       qmi_err_num, atoi(scan_string));
            }
            else
            {
              printf("Registration Successufully for WLAN Status IND.\n ");
            }
            break;
          }
          /* Invalid integer entered. */
          default :
          {
            printf("Invalid response %d\n", opt);
          }
            break;
        }
      }
      return 0;
}
