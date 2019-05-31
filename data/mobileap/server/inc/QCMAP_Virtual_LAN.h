#ifndef _QCMAP_Virtual_LAN_H_
#define _QCMAP_Virtual_LAN_H_

/*======================================================

FILE:  QCMAP_Virtual_LAN.h

SERVICES:
   QCMAP Virtual LAN Class

=======================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

======================================================*/
/*======================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.
    when       who        what, where, why
  --------   ---        -------------------------------------------------------
  03/15/17   jc           Created
======================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unordered_map>
#include <vector>

#include "qualcomm_mobile_access_point_msgr_v01.h"
#include "ds_util.h"
#include "qcmap_cm_api.h"
#include "ds_string.h"
#include "QCMAP_ConnectionManager.h"

#define MAX_VLAN_STR_LEN 5 //4 digits + 1 '\0' character

/* Vconfig flags */
#define VLAN_TAG                     "vlan"
#define VCONFIG_TAG                  "VConfig"
#define VCONFIG_IFACE_TAG            "interface"
#define VCONFIG_ID_TAG               "id"

/*Static IPv6 prefixes*/
#define VLAN_BASE_IPV6_ADDR          "FD53:7CB8:0383"
#define STATIC_IID                   "0123"
#define VLAN_IPV6_PREFIX_LEN_BITS    64
#define VLAN_IPV6_PREFIX_LEN_BYTES   8

/*Static IPv4 addresses*/
#define VLAN_BASE_IPV4_ADDR          "192.169.0.0"
#define VLAN_SUBNET_MASK             "255.255.255.240"

#define MIN_VLAN_ID                  1  /*vlan 0 is reserved as per RFC*/
#define MAX_VLAN_ID                  4094/*vlan 4095 is max and it is reserved*/

#define VLAN_SLEEP_INTERVAL          3
#define VLAN_NTN_IOCTL_SLEEP_INTERVAL 3
#define VLAN_NTN_MAX_RETRIES         2
#define VLAN_NTN_CONFIG_FAIL_ERRNO     11 /*EAGAIN 11*/

//maximum payload for netlink message
#define MAX_NL_PAYLOAD 4096

#if !defined(FEATURE_DATA_TARGET_MDM9607) && !defined(FEATURE_MOBILEAP_LV_PLATFORM) && \
    !defined(FEATURE_QCMAP_OFFTARGET) && !defined(FEATURE_MOBILEAP_APQ_PLATFORM)

/* MAX_IPA_OFFLOAD total supported is 4, of which
   bridge0 is always assumed to be IPA offload (unconditional).
   So, left with 3 on-demand bridges which can support IPA offload.
 */
  #define MAX_IPA_OFFLOAD                   3
  #define IPA_DEVICE_NAME                   "/dev/ipa"
  #define NTN_TX_DMA_CH_2                   2
  #define NTN_RX_DMA_CH_0                   0
  #define NEUTRINO_IOCTL_BUFFER_SIZE_BYTES  14
  #define NEUTRINO_IPA_VLAN_DISABLE         0
  #define NEUTRINO_IPA_VLAN_ENABLE          1

  #define NEUTRION_IOCTL_TX_IPA_DMA_INDEX   0
  #define NEUTRION_IOCTL_RX_IPA_DMA_INDEX   4
  #define NEUTRION_IOCTL_COMMAND_INDEX      8
  #define NEUTRION_IOCTL_VLAN_ID_INDEX      12
#endif

/* Is vlan_id in valid range */
#define IS_VLAN_ID_VALID(vlan_id) ((vlan_id >= MIN_VLAN_ID) && (vlan_id <= MAX_VLAN_ID))

typedef enum {
  QCMAP_TETH_MIN = -1,
  QCMAP_TETH_ECM = 0,
  QCMAP_TETH_RNDIS = 1,
  QCMAP_TETH_ETH = 2,
  QCMAP_TETH_BRIDGE0 = 3,
  QCMAP_MAX_PHY_LAN_IFACE,
} QCMAP_Virtual_LAN_phy_iface;

typedef struct
{
  int16_t                     vlan_id;
  bool                        ipa_offload;
  bool                        is_up;
  QCMAP_Virtual_LAN_phy_iface phy_iface_type;
  char                        iface_name[QCMAP_MAX_IFACE_NAME_SIZE_V01];
} qcmap_vlan_item;

class QCMAP_Virtual_LAN
{
  #if !defined(FEATURE_DATA_TARGET_MDM9607) && !defined(FEATURE_MOBILEAP_LV_PLATFORM) && \
      !defined(FEATURE_QCMAP_OFFTARGET) && !defined(FEATURE_MOBILEAP_APQ_PLATFORM)

  #define ECM_STR " ecm"
  #define RNDIS_STR " rndis"
  #define ETH_STR " eth"
  //' ' + strlen(ECM_STR) + ' ' + strlen(RNDIS_STR) + ' ' + strlen(ETH_STR) + '\0'
  #define MAX_PHY_IFACE_LIST_STRLEN 15

  private:
    //Private class for memoizing ipa offload requirements, can automatically trigger reboots for
    //IPA hw configuration
    class QCMAP_VLAN_IPA_Offloader
    {
      private:
        bool auto_reboot_flag;
        std::unordered_map<QCMAP_Virtual_LAN_phy_iface, std::vector<int16_t>, std::hash<int>>
                          phy_iface_use_ht;

        //count for how many current VLANS configured for offload, it is expected that this is
        //strictly monotonically increasing
        unsigned char offload_count;
        qcmap_msgr_vlan_config_v01 vlan_iface[MAX_IPA_OFFLOAD];

        void PrintPhyIfaceUseHashTable(void) const;
        bool IsVLANIfacePreviouslyOffloaded(const char* iface_name, int16_t vlan_id) const;
        unsigned char GetOffloadCount(void) const;
        void UpdateIPACfgFile(void) const;

      public:
        QCMAP_VLAN_IPA_Offloader();
        ~QCMAP_VLAN_IPA_Offloader(){};
        bool GetAutoRebootFlag(void);
        bool IsVLANIfaceOffloadAllowed(const char* iface_name, int16_t vlan_id) const;
        void UpdateDesiredOffloadInfo(const char* iface_name, int16_t vlan_id, bool offload_flag);
        bool UpdateIPAWithVlanIOCTL(const char *iface_name, int16_t vlan_id, bool is_up);
        bool UpdateNeutrinoWithVlanIoctl(int16_t vlan_id, bool is_up) const;
    };
  #endif

  private:
    #if !defined(FEATURE_DATA_TARGET_MDM9607) && !defined(FEATURE_MOBILEAP_LV_PLATFORM) && \
        !defined(FEATURE_QCMAP_OFFTARGET) && !defined(FEATURE_MOBILEAP_APQ_PLATFORM)
    QCMAP_VLAN_IPA_Offloader* ipa_offloader;
    #endif
    static bool flag;
    static QCMAP_Virtual_LAN *object;
    QCMAP_Virtual_LAN();

    void ConfigureVLAN(qcmap_vlan_item vlan_config_node) const;
    void DeleteVLAN(qcmap_vlan_item vlan_config_node, bool deleteConfig) const;

    bool ConstructVLANNode(qcmap_msgr_vlan_config_v01 vconfig,
                           qcmap_vlan_item *vlan_node) const;

    bool SetVLANConfigToXML(qcmap_msgr_vlan_config_v01 vconfig) const;

    bool DeleteVLANConfigFromXML(qcmap_msgr_vlan_config_v01 vconfig) const;


  public:
    ~QCMAP_Virtual_LAN();
    static QCMAP_Virtual_LAN *Get_Instance(bool obj_create=false);

    void AddDeleteVLANOnIface(char *iface_name, bool link_up);

    bool IsPhyLinkUp(QCMAP_Virtual_LAN_phy_iface iface_type) const;

    bool GetIPAddrOfPhyLink(QCMAP_Virtual_LAN_phy_iface iface_type, uint32 *ipv4_addr,
                            struct in6_addr *ipv6_addr) const;

    bool SetVLANConfig(qcmap_msgr_vlan_config_v01 vlan_config, bool createConfig,
                       qmi_error_type_v01* qmi_err_num, bool* is_ipa_offloaded);

    bool GetVLANConfig(qcmap_msgr_vlan_config_v01* vlan_config_list, uint32_t* vlan_config_list_len,
                       qmi_error_type_v01* qmi_err_num);

    bool DeleteVLANConfig(qcmap_msgr_vlan_config_v01 vlan_config, qmi_error_type_v01* qmi_err_num);

    static bool GetPhyIfaceVLANIDFromIface(char *iface_name,
                                           QCMAP_Virtual_LAN_phy_iface *phy_type,
                                           int16_t *vlan_id);

    static bool IsVLANIDUp(int16_t vlan_id, char *iface_name);

    static bool GetIPAddrForVLAN(int16_t vlan_id, qcmap_ip4_addr_subnet_mask_v01 *ipv4_addr,
                                 qcmap_ip6_addr_prefix_len_v01 *ipv6_addr);
};
#endif
