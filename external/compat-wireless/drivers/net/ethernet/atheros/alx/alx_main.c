/* Copyright (c) 2014-2017, The Linux Foundation. All rights reserved. */
/*
 * Copyright (c) 2012 Qualcomm Atheros, Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "alx.h"
#include "alx_hwcom.h"

#include <linux/moduleparam.h>
#ifdef MDM_PLATFORM
#include <linux/debugfs.h>
#include <linux/pm_wakeup.h>
#endif

#ifdef APQ_PLATFORM
#include <linux/platform_device.h>
#include <asm/dma-iommu.h>
#include <linux/iommu.h>
#define ALX_SMMU_BASE       0x10000000 /* Device address range base */
#define ALX_SMMU_SIZE       ((SZ_1G * 4ULL) - ALX_SMMU_BASE)
struct dma_iommu_mapping *alx_mapping;
#endif

char alx_drv_name[] = "alx";
static const char alx_drv_description[] =
	"Qualcomm Atheros(R) "
	"AR813x/AR815x/AR816x PCI-E Ethernet Network Driver";

static bool ipa_enable = 1;
bool module_ipa_enable = 1;
module_param(module_ipa_enable, bool, S_IRUGO);
MODULE_PARM_DESC(module_ipa_enable, "PSB enable/disable");

// ProjE change
u32 mac_addr_hi16=0xFFFFFFFFUL;
module_param(mac_addr_hi16, uint, 0);
MODULE_PARM_DESC(mac_addr_hi16,"Specify the high 16 bits (most significant) of the mac address");

u32 mac_addr_lo32=0xFFFFFFFFUL;
module_param(mac_addr_lo32, uint, 0);
MODULE_PARM_DESC(mac_addr_lo32,"Specify the low 32 bits of the mac address");
// END of ProjE change


/* alx_pci_tbl - PCI Device ID Table
 *
 * Wildcard entries (PCI_ANY_ID) should come last
 * Last entry must be all 0s
 *
 * { Vendor ID, Device ID, SubVendor ID, SubDevice ID,
 *   Class, Class Mask, private data (not used) }
 */
#define ALX_ETHER_DEVICE(device_id) {\
	PCI_DEVICE(ALX_VENDOR_ID, device_id)}
static DEFINE_PCI_DEVICE_TABLE(alx_pci_tbl) = {
	ALX_ETHER_DEVICE(ALX_DEV_ID_AR8131),
	ALX_ETHER_DEVICE(ALX_DEV_ID_AR8132),
	ALX_ETHER_DEVICE(ALX_DEV_ID_AR8151_V1),
	ALX_ETHER_DEVICE(ALX_DEV_ID_AR8151_V2),
	ALX_ETHER_DEVICE(ALX_DEV_ID_AR8152_V1),
	ALX_ETHER_DEVICE(ALX_DEV_ID_AR8152_V2),
	ALX_ETHER_DEVICE(ALX_DEV_ID_AR8161),
	ALX_ETHER_DEVICE(ALX_DEV_ID_AR8162),
	{0,}
};
MODULE_DEVICE_TABLE(pci, alx_pci_tbl);
MODULE_AUTHOR("Qualcomm Corporation, <nic-devel@qualcomm.com>");
MODULE_DESCRIPTION("Qualcomm Atheros Gigabit Ethernet Driver");
MODULE_LICENSE("Dual BSD/GPL");

static void alx_init_ring_ptrs(struct alx_adapter *adpt);

#ifdef MDM_PLATFORM
static int alx_ipa_rm_try_release(struct alx_adapter *adpt);
static int alx_ipa_setup_rm(struct alx_adapter *adpt);

/* Global CTX PTR which can be used for debugging */
static struct alx_adapter *galx_adapter_ptr = NULL;
const mode_t read_write_mode = S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP;

static inline char *alx_ipa_rm_state_to_str(enum alx_ipa_rm_state state)
{
	switch (state) {
	case ALX_IPA_RM_RELEASED: return "RELEASED";
	case ALX_IPA_RM_REQUESTED: return "REQUESTED";
	case ALX_IPA_RM_GRANT_PENDING: return "GRANT PENDING";
	case ALX_IPA_RM_GRANTED: return "GRANTED";
	}

	return "UNKNOWN";
}

#ifdef ALX_IPA_DEBUG
/**
 * alx_dump_buff() - dumps buffer for debug purposes
 * @base: buffer base address
 * @phy_base: buffer physical base address
 * @size: size of the buffer
 */
static void alx_dump_buff(void *base, dma_addr_t phy_base, u32 size)
{
        int i;
        u32 *cur = (u32 *)base;
        u8 *byt;
        pr_info("system phys addr=%pa len=%u\n", &phy_base, size);
        for (i = 0; i < size / 4; i++) {
                byt = (u8 *)(cur + i);
                pr_info("%2d %08x   %02x %02x %02x %02x\n", i, *(cur + i),
                                byt[0], byt[1], byt[2], byt[3]);
        }
        pr_info("END\n");
}
#define ALX_DUMP_BUFF(base, phy_base, size) alx_dump_buff(base, phy_base, size)

static bool alx_is_packet_dhcp(struct sk_buff *skb)
{
	uint16_t sport = 0;
	uint16_t dport = 0;

	sport = be16_to_cpu((uint16_t)(*(uint16_t *)(skb->data + ALX_IP_OFFSET + ALX_IP_HEADER_SIZE)));
	dport = be16_to_cpu((uint16_t)(*(uint16_t *)(skb->data + ALX_IP_OFFSET + ALX_IP_HEADER_SIZE + sizeof(uint16_t))));

	if (((sport == ALX_DHCP_SRV_PORT) && (dport == ALX_DHCP_CLI_PORT)) ||
	    ((dport == ALX_DHCP_SRV_PORT) && (sport == ALX_DHCP_CLI_PORT)))
	{
		return true;
	}
	return false;
}
#endif

static int alx_ipa_rm_request(struct alx_adapter *adpt);

#endif

int alx_cfg_r16(const struct alx_hw *hw, int reg, u16 *pval)
{
	if (!(hw && hw->adpt && hw->adpt->pdev))
		return -EINVAL;
	return pci_read_config_word(hw->adpt->pdev, reg, pval);
}


int alx_cfg_w16(const struct alx_hw *hw, int reg, u16 val)
{
	if (!(hw && hw->adpt && hw->adpt->pdev))
		return -EINVAL;
	return pci_write_config_word(hw->adpt->pdev, reg, val);
}


void alx_mem_flush(const struct alx_hw *hw)
{
	readl(hw->hw_addr);
}


void alx_mem_r32(const struct alx_hw *hw, int reg, u32 *val)
{
	if (unlikely(!hw->link_up))
		readl(hw->hw_addr + reg);
	*(u32 *)val = readl(hw->hw_addr + reg);
}


void alx_mem_w32(const struct alx_hw *hw, int reg, u32 val)
{
	if (hw->mac_type == alx_mac_l2cb_v20 && reg < 0x1400)
		readl(hw->hw_addr + reg);
	writel(val, hw->hw_addr + reg);
}


static void alx_mem_r16(const struct alx_hw *hw, int reg, u16 *val)
{
	if (unlikely(!hw->link_up))
		readl(hw->hw_addr + reg);
	*(u16 *)val = readw(hw->hw_addr + reg);
}


static void alx_mem_w16(const struct alx_hw *hw, int reg, u16 val)
{
	if (hw->mac_type == alx_mac_l2cb_v20 && reg < 0x1400)
		readl(hw->hw_addr + reg);
	writew(val, hw->hw_addr + reg);
}


void alx_mem_w8(const struct alx_hw *hw, int reg, u8 val)
{
	if (hw->mac_type == alx_mac_l2cb_v20 && reg < 0x1400)
		readl(hw->hw_addr + reg);
	writeb(val, hw->hw_addr + reg);
}


/*
 * alx_hw_printk
 */
void alx_hw_printk(const char *level, const struct alx_hw *hw,
		   const char *fmt, ...)
{
	struct va_format vaf;
	va_list args;

	va_start(args, fmt);
	vaf.fmt = fmt;
	vaf.va = &args;
/* warning : __netdev_printk if no compat.o
	if (hw && hw->adpt && hw->adpt->netdev)
		__netdev_printk(level, hw->adpt->netdev, &vaf);
	else
*/
		printk("%salx_hw: %pV", level, &vaf);

	va_end(args);
}


/*
 *  alx_validate_mac_addr - Validate MAC address
 */
static int alx_validate_mac_addr(u8 *mac_addr)
{
	int retval = 0;

	if (mac_addr[0] & 0x01) {
		printk(KERN_DEBUG "MAC address is multicast\n");
		retval = -EADDRNOTAVAIL;
	} else if (mac_addr[0] == 0xff && mac_addr[1] == 0xff) {
		printk(KERN_DEBUG "MAC address is broadcast\n");
		retval = -EADDRNOTAVAIL;
	} else if (mac_addr[0] == 0 && mac_addr[1] == 0 &&
		   mac_addr[2] == 0 && mac_addr[3] == 0 &&
		   mac_addr[4] == 0 && mac_addr[5] == 0) {
		printk(KERN_DEBUG "MAC address is all zeros\n");
		retval = -EADDRNOTAVAIL;
	}
	return retval;
}


/*
 *  alx_set_mac_type - Sets MAC type
 */
static int alx_set_mac_type(struct alx_adapter *adpt)
{
	struct alx_hw *hw = &adpt->hw;
	int retval = 0;

	if (hw->pci_venid == ALX_VENDOR_ID) {
		switch (hw->pci_devid) {
		case ALX_DEV_ID_AR8131:
			hw->mac_type = alx_mac_l1c;
			break;
		case ALX_DEV_ID_AR8132:
			hw->mac_type = alx_mac_l2c;
			break;
		case ALX_DEV_ID_AR8151_V1:
			hw->mac_type = alx_mac_l1d_v1;
			break;
		case ALX_DEV_ID_AR8151_V2:
			/* just use l1d configure */
			hw->mac_type = alx_mac_l1d_v2;
			break;
		case ALX_DEV_ID_AR8152_V1:
			hw->mac_type = alx_mac_l2cb_v1;
			break;
		case ALX_DEV_ID_AR8152_V2:
			if (hw->pci_revid == ALX_REV_ID_AR8152_V2_0)
				hw->mac_type = alx_mac_l2cb_v20;
			else
				hw->mac_type = alx_mac_l2cb_v21;
			break;
		case ALX_DEV_ID_AR8161:
			hw->mac_type = alx_mac_l1f;
			break;
		case ALX_DEV_ID_AR8162:
			hw->mac_type = alx_mac_l2f;
			break;
		default:
			retval = -EINVAL;
			break;
		}
	} else {
		retval = -EINVAL;
	}

	netif_dbg(adpt, hw, adpt->netdev,
		   "found mac: %d, returns: %d\n", hw->mac_type, retval);
	return retval;
}


/*
 *  alx_init_hw_callbacks
 */
static int alx_init_hw_callbacks(struct alx_adapter *adpt)
{
	struct alx_hw *hw = &adpt->hw;
	int retval = 0;

	alx_set_mac_type(adpt);

	switch (hw->mac_type) {
	case alx_mac_l1f:
	case alx_mac_l2f:
		retval = alf_init_hw_callbacks(hw);
		break;
	case alx_mac_l1c:
	case alx_mac_l2c:
	case alx_mac_l2cb_v1:
	case alx_mac_l2cb_v20:
	case alx_mac_l2cb_v21:
	case alx_mac_l1d_v1:
	case alx_mac_l1d_v2:
		retval = alc_init_hw_callbacks(hw);
		break;
	default:
		retval = -EINVAL;
		break;
	}
	return retval;
}


void alx_reinit_locked(struct alx_adapter *adpt)
{
	WARN_ON(in_interrupt());

	while (CHK_ADPT_FLAG(1, STATE_RESETTING))
		msleep(20);
	SET_ADPT_FLAG(1, STATE_RESETTING);

	alx_stop_internal(adpt, ALX_OPEN_CTRL_RESET_MAC);
	alx_open_internal(adpt, ALX_OPEN_CTRL_RESET_MAC);

	CLI_ADPT_FLAG(1, STATE_RESETTING);
}


static void alx_task_schedule(struct alx_adapter *adpt)
{
	if (!CHK_ADPT_FLAG(1, STATE_DOWN) &&
	    !CHK_ADPT_FLAG(1, STATE_WATCH_DOG)) {
		SET_ADPT_FLAG(1, STATE_WATCH_DOG);
		schedule_work(&adpt->alx_task);
	}
}


static void alx_check_lsc(struct alx_adapter *adpt)
{
	SET_ADPT_FLAG(0, TASK_LSC_REQ);
	adpt->link_jiffies = jiffies + ALX_TRY_LINK_TIMEOUT;

	if (!CHK_ADPT_FLAG(1, STATE_DOWN))
		alx_task_schedule(adpt);
}


/*
 * alx_tx_timeout - Respond to a Tx Hang
 */
static void alx_tx_timeout(struct net_device *netdev)
{
	struct alx_adapter *adpt = netdev_priv(netdev);

	/* Do the reset outside of interrupt context */
	if (!CHK_ADPT_FLAG(1, STATE_DOWN)) {
		SET_ADPT_FLAG(0, TASK_REINIT_REQ);
		alx_task_schedule(adpt);
	}
}


/*
 * alx_set_multicase_list - Multicast and Promiscuous mode set
 */
static void alx_set_multicase_list(struct net_device *netdev)
{
	struct alx_adapter *adpt = netdev_priv(netdev);
	struct alx_hw *hw = &adpt->hw;
	struct netdev_hw_addr *ha;

	/* Check for Promiscuous and All Multicast modes */
	if (netdev->flags & IFF_PROMISC) {
		SET_HW_FLAG(PROMISC_EN);
	} else if (netdev->flags & IFF_ALLMULTI) {
		SET_HW_FLAG(MULTIALL_EN);
		CLI_HW_FLAG(PROMISC_EN);
	} else {
		CLI_HW_FLAG(MULTIALL_EN);
		CLI_HW_FLAG(PROMISC_EN);
	}
	hw->cbs.config_mac_ctrl(hw);

	/* clear the old settings from the multicast hash table */
	hw->cbs.clear_mc_addr(hw);

	/* comoute mc addresses' hash value ,and put it into hash table */
	netdev_for_each_mc_addr(ha, netdev)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35))
		hw->cbs.set_mc_addr(hw, ha->addr);
#else
		hw->cbs.set_mc_addr(hw, ha->dmi_addr);
#endif
}


/*
 * alx_set_mac - Change the Ethernet Address of the NIC
 */
static int alx_set_mac_address(struct net_device *netdev, void *data)
{
	struct alx_adapter *adpt = netdev_priv(netdev);
	struct alx_hw *hw = &adpt->hw;
	struct sockaddr *addr = data;

	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;

	if (netif_running(netdev))
		return -EBUSY;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36))
	if (netdev->addr_assign_type & NET_ADDR_RANDOM)
		netdev->addr_assign_type ^= NET_ADDR_RANDOM;
#endif

	memcpy(netdev->dev_addr, addr->sa_data, netdev->addr_len);
	memcpy(hw->mac_addr, addr->sa_data, netdev->addr_len);

	if (hw->cbs.set_mac_addr)
		hw->cbs.set_mac_addr(hw, hw->mac_addr);
	return 0;
}


/*
 * Read / Write Ptr Initialize:
 */
static void alx_init_ring_ptrs(struct alx_adapter *adpt)
{
	int i, j;

	for (i = 0; i < adpt->num_txques; i++) {
		struct alx_tx_queue *txque = adpt->tx_queue[i];
		struct alx_buffer *tpbuf = txque->tpq.tpbuff;
		txque->tpq.produce_idx = 0;
		txque->tpq.consume_idx = 0;
		for (j = 0; j < txque->tpq.count; j++)
			tpbuf[j].dma = 0;
	}

	for (i = 0; i < adpt->num_hw_rxques; i++) {
		struct alx_rx_queue *rxque = adpt->rx_queue[i];
		struct alx_buffer *rfbuf = rxque->rfq.rfbuff;
		rxque->rrq.produce_idx = 0;
		rxque->rrq.consume_idx = 0;
		rxque->rfq.produce_idx = 0;
		rxque->rfq.consume_idx = 0;
		for (j = 0; j < rxque->rfq.count; j++)
			rfbuf[j].dma = 0;
	}

	if (CHK_ADPT_FLAG(0, SRSS_EN))
		goto srrs_enable;

	return;

srrs_enable:
	for (i = 0; i < adpt->num_sw_rxques; i++) {
		struct alx_rx_queue *rxque = adpt->rx_queue[i];
		rxque->swq.produce_idx = 0;
		rxque->swq.consume_idx = 0;
	}
}


static void alx_config_rss(struct alx_adapter *adpt)
{
	static const u8 key[40] = {
		0xE2, 0x91, 0xD7, 0x3D, 0x18, 0x05, 0xEC, 0x6C,
		0x2A, 0x94, 0xB3, 0x0D, 0xA5, 0x4F, 0x2B, 0xEC,
		0xEA, 0x49, 0xAF, 0x7C, 0xE2, 0x14, 0xAD, 0x3D,
		0xB8, 0x55, 0xAA, 0xBE, 0x6A, 0x3E, 0x67, 0xEA,
		0x14, 0x36, 0x4D, 0x17, 0x3B, 0xED, 0x20, 0x0D};

	struct alx_hw *hw = &adpt->hw;
	u32 reta = 0;
	int i, j;

	/* initialize rss hash type and idt table size */
	hw->rss_hstype = ALX_RSS_HSTYP_ALL_EN;
	hw->rss_idt_size = 0x100;

	/* Fill out redirection table */
	memcpy(hw->rss_key, key, sizeof(hw->rss_key));

	/* Fill out redirection table */
	memset(hw->rss_idt, 0x0, sizeof(hw->rss_idt));
	for (i = 0, j = 0; i < 256; i++, j++) {
		if (j == adpt->max_rxques)
			j = 0;
		reta |= (j << ((i & 7) * 4));
		if ((i & 7) == 7) {
			hw->rss_idt[i>>3] = reta;
			reta = 0;
		}
	}

	if (hw->cbs.config_rss)
		hw->cbs.config_rss(hw, CHK_ADPT_FLAG(0, SRSS_EN));
}

#ifdef MDM_PLATFORM
static bool alx_ipa_is_non_ip_pkt(uint8_t proto)
{
	if ((proto == RRD_PROTO_NON_IP) ||
		(proto == RRD_PROTO_LLDP))
		return true;
	return false;
}

static void alx_ipa_remove_padding(struct sk_buff *skb, bool ipv4, bool ipv6)
{
	if (ipv4) {
		struct iphdr *ip_hdr = NULL;
		ip_hdr = (struct iphdr *)(skb_mac_header(skb) + ETH_HLEN);
		skb_trim(skb, ntohs(ip_hdr->tot_len) + ETH_HLEN);
	} else if (ipv6) {
		struct ipv6hdr *ip6_hdr = NULL;
		ip6_hdr = (struct ipv6hdr *)(skb_mac_header(skb) + ETH_HLEN);
		skb_trim(skb, ntohs(ip6_hdr->payload_len) + sizeof(struct ipv6hdr) + ETH_HLEN);
	}
}

static bool alx_ipa_is_ip_frag_pkt(struct sk_buff *skb)
{
	struct iphdr *ip_hdr = NULL;
	ip_hdr = (struct iphdr *)(skb_mac_header(skb) + ETH_HLEN);

	/* Return true if: 'More_Frag bit is set' OR
			'Fragmentation Offset is set' */
	if (ip_hdr->frag_off & htons(IP_MF | IP_OFFSET))
		return true;
	return false;
}

static bool alx_ipa_is_ipv4_pkt(uint8_t proto)
{
	if ((proto == RRD_PROTO_IPv4) ||
		(proto == RRD_PROTO_IPv4_TCP) ||
		(proto == RRD_PROTO_IPv4_UDP))
		return true;
	else
		return false;
}

static bool alx_ipa_is_ipv6_pkt(uint8_t proto)
{
	if ((proto == RRD_PROTO_IPv6) ||
		(proto == RRD_PROTO_IPv6_TCP) ||
		(proto == RRD_PROTO_IPv6_UDP))
		return true;
	else
		return false;
}
#endif

/*
 * alx_receive_skb
 */
static void alx_receive_skb(struct alx_adapter *adpt,
                            struct sk_buff *skb,
                            u16 vlan_tag, bool vlan_flag)
{
	if (vlan_flag) {
		u16 vlan;
		ALX_TAG_TO_VLAN(vlan_tag, vlan);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0))
		/*alx MAC only recognize ETH_P_8021Q*/
                __vlan_hwaccel_put_tag(skb, htons(ETH_P_8021Q), vlan);
#else
                __vlan_hwaccel_put_tag(skb, vlan);
#endif
	}
	netif_receive_skb(skb);
}

#ifdef MDM_PLATFORM
/*
 * alx_receive_skb_ipa
 */
static void alx_receive_skb_ipa(struct alx_adapter *adpt,
				struct sk_buff *skb,
				u16 vlan_tag, bool vlan_flag, uint8_t proto)
{
	struct ipa_tx_meta ipa_meta = {0x0};
	int ret =0;
	struct alx_ipa_rx_desc_node *node = NULL;
	bool schedule_ipa_work = false;
	bool is_pkt_ipv4 = alx_ipa_is_ipv4_pkt(proto);
	bool is_pkt_ipv6 = alx_ipa_is_ipv6_pkt(proto);
	struct alx_ipa_ctx *alx_ipa = adpt->palx_ipa;
	skb_reset_mac_header(skb);
	if (vlan_flag) {
		u16 vlan;
		ALX_TAG_TO_VLAN(vlan_tag, vlan);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0))
                __vlan_hwaccel_put_tag(skb, htons(ETH_P_8021Q), vlan);
#else
		__vlan_hwaccel_put_tag(skb, vlan);
#endif
	}

	/* If Packet is an IP Packet and non-fragmented then Send it to
		IPA/ODU Bridge Driver*/
	if (alx_ipa_is_non_ip_pkt(proto) ||
	   (is_pkt_ipv4 && alx_ipa_is_ip_frag_pkt(skb)) ||
	   (!CHK_ADPT_FLAG(2, ODU_CONNECT))) {
		/* Send packet to network stack */
		skb->protocol = eth_type_trans(skb, adpt->netdev);
		adpt->palx_ipa->stats.non_ip_frag_pkt++;
		/* Prevent device to goto suspend for 200 msec; to provide enough
		time for packet to be processed by network stack */
		pm_wakeup_event(&adpt->pdev->dev, 200);
		netif_receive_skb(skb);
		return;
	} else {
		/* Send Packet to ODU bridge Driver */
		spin_lock(&adpt->flow_ctrl_lock);

		/* Drop Packets if we cannot handle them */
		if (adpt->freeq_cnt == 0) {
			adpt->palx_ipa->stats.flow_control_pkt_drop++;
                        pr_debug("ALX-IPA Flow Control - Drop Pkt"
				" IPA pending packets = %d",
					adpt->ipa_free_desc_cnt);
			dev_kfree_skb(skb);
			/* Schedule ipa_work task if IPA has desc left. This is
			done since priority of softirq > IPA_WRITE_DONE Event.
			Which implies ipa_send_task will not be scheduled in
			a timely manner */
			if ((adpt->ipa_free_desc_cnt > 0) && !CHK_ADPT_FLAG(2, WQ_SCHED))
				schedule_ipa_work = true;
			goto unlock_and_schedule;
		}

		/* Remove extra padding if the rcv_pkt_len == 64 */
		if (skb->len == ETH_ZLEN)
			alx_ipa_remove_padding(skb, is_pkt_ipv4, is_pkt_ipv6);

		/* Send Packet to IPA; if there are no pending packets
		   and ipa has available descriptors */
		if ((adpt->pendq_cnt == 0) && (adpt->ipa_free_desc_cnt > 0) &&
			(alx_ipa_rm_request(adpt) == 0)) {
			ipa_meta.dma_address_valid = false;
			/* Send Packet to ODU bridge Driver */
			ret = odu_bridge_tx_dp(skb, &ipa_meta);
			if (ret) {
				pr_err("odu_bridge_tx_dp() Failed!!"
					" ret %d--Free SKB\n", ret);
				kfree(skb);
				adpt->palx_ipa->stats.rx_ipa_send_fail++;
			} else {
				adpt->ipa_free_desc_cnt--;
				adpt->palx_ipa->stats.rx_ipa_send++;
				/* Increment the ipa_rx_completion Counter */
				spin_lock(&alx_ipa->rm_ipa_lock);
				if (alx_ipa->acquire_wake_src == false) {
					__pm_stay_awake(&alx_ipa->rm_ipa_wait);
					alx_ipa->acquire_wake_src = true;
				}
				alx_ipa->ipa_rx_completion++;
				spin_unlock(&alx_ipa->rm_ipa_lock);
			}
		} else if (adpt->pendq_cnt <= ALX_IPA_SYS_PIPE_DNE_PKTS) {
			/* If we have pending packets to send,
			add the current packet to the end of the queue. */
			node = list_first_entry(&adpt->free_queue_head,
				struct alx_ipa_rx_desc_node, link);
			list_del(&node->link);
			node->skb_ptr = skb;
			list_add_tail(&node->link, &adpt->pend_queue_head);
			adpt->freeq_cnt--;
			adpt->pendq_cnt++;
			if (!CHK_ADPT_FLAG(2, WQ_SCHED))
				schedule_ipa_work = true;
		}
unlock_and_schedule:
		spin_unlock(&adpt->flow_ctrl_lock);
		if (schedule_ipa_work) {
			SET_ADPT_FLAG(2, WQ_SCHED);
			schedule_work(&adpt->ipa_send_task);
		}
	}
}
#endif


static bool alx_get_rrdesc(struct alx_adapter *adpt, struct alx_rx_queue *rxque,
			   union alx_sw_rrdesc *srrd)
{
	u32 cnt = 0;
	union alx_hw_rrdesc *hrrd =
			ALX_RRD(rxque, rxque->rrq.consume_idx);

	srrd->dfmt.dw0 = le32_to_cpu(hrrd->dfmt.dw0);
	srrd->dfmt.dw1 = le32_to_cpu(hrrd->dfmt.dw1);
	srrd->dfmt.dw2 = le32_to_cpu(hrrd->dfmt.dw2);
	srrd->dfmt.dw3 = le32_to_cpu(hrrd->dfmt.dw3);

	if (!srrd->genr.update)
		return false;

	/* workaround for the PCIe DMA write issue */
	/* Please make sure hrrd->dmt.dw0 is set to 0 after handling, please refer to the later line in the same function alx_get_rrdesc: hrrd->dfmt.dw0 = 0;*/
	if (srrd->dfmt.dw0 == 0) {
		volatile u32 *flag = (volatile u32 *)&hrrd->dfmt.dw0;
		while (*flag == 0) { /* while the dword0 of the rrd is still 0 which means it's not updated yet, read it again */
						if (++cnt >= 10) { /* 10 more times should be enough, actually should NOT get here */
							alx_err(adpt, "ERROR, RRD update timeout\n");
							return false;
						}
		}
		/* read the rrd descriptor from hardware again after all four dwords of the rrd are synced by DMA engine */
		srrd->dfmt.dw0 = le32_to_cpu(hrrd->dfmt.dw0);
		srrd->dfmt.dw1 = le32_to_cpu(hrrd->dfmt.dw1);
		srrd->dfmt.dw2 = le32_to_cpu(hrrd->dfmt.dw2);
		srrd->dfmt.dw3 = le32_to_cpu(hrrd->dfmt.dw3);
	}

	if (unlikely(srrd->genr.nor != 1)) {
		/* TODO support mul rfd*/
		printk(KERN_EMERG "Multi rfd not support yet!\n");
		alx_err(adpt, "Please make sure PCIe DMA Write workaroud has been applied\n");
		alx_err(adpt, "SRRD 0/1/2/3:0x%08x/0x%08x/0x%08x/0x%08x\n",
			srrd->dfmt.dw0, srrd->dfmt.dw1, srrd->dfmt.dw2, srrd->dfmt.dw3);
		alx_err(adpt, "HRRD 0/1/2/3:0x%08x/0x%08x/0x%08x/0x%08x\n",
			hrrd->dfmt.dw0, hrrd->dfmt.dw1, hrrd->dfmt.dw2, hrrd->dfmt.dw3);
	}

	srrd->genr.update = 0;
	hrrd->dfmt.dw3 = cpu_to_le32(srrd->dfmt.dw3); /* clear the update bit in hardware */

	/* Workaround for the PCIe DMA write issue */
	/* Note: for intensive verification to set it 0 */
	hrrd->dfmt.dw0 = 0;

	if (++rxque->rrq.consume_idx == rxque->rrq.count)
		rxque->rrq.consume_idx = 0;

	return true;
}


static bool alx_set_rfdesc(struct alx_rx_queue *rxque,
			   union alx_sw_rfdesc *srfd)
{
	union alx_hw_rfdesc *hrfd =
			ALX_RFD(rxque, rxque->rfq.produce_idx);

	hrfd->qfmt.qw0 = cpu_to_le64(srfd->qfmt.qw0);

	if (++rxque->rfq.produce_idx == rxque->rfq.count)
		rxque->rfq.produce_idx = 0;

	return true;
}


static bool alx_set_tpdesc(struct alx_tx_queue *txque,
			   union alx_sw_tpdesc *stpd)
{
	union alx_hw_tpdesc *htpd;

	txque->tpq.last_produce_idx = txque->tpq.produce_idx;
	htpd = ALX_TPD(txque, txque->tpq.produce_idx);

	if (++txque->tpq.produce_idx == txque->tpq.count)
		txque->tpq.produce_idx = 0;

	htpd->dfmt.dw0 = cpu_to_le32(stpd->dfmt.dw0);
	htpd->dfmt.dw1 = cpu_to_le32(stpd->dfmt.dw1);
	htpd->qfmt.qw1 = cpu_to_le64(stpd->qfmt.qw1);

	return true;
}


static void alx_set_tpdesc_lastfrag(struct alx_tx_queue *txque)
{
	union alx_hw_tpdesc *htpd;
#define ALX_TPD_LAST_FLAGMENT  0x80000000
	htpd = ALX_TPD(txque, txque->tpq.last_produce_idx);
	htpd->dfmt.dw1 |= cpu_to_le32(ALX_TPD_LAST_FLAGMENT);
}


static int alx_refresh_rx_buffer(struct alx_rx_queue *rxque)
{
	struct alx_adapter *adpt = netdev_priv(rxque->netdev);
	struct alx_hw *hw = &adpt->hw;
	struct alx_buffer *curr_rxbuf;
	struct alx_buffer *next_rxbuf;
	union alx_sw_rfdesc srfd;
	struct sk_buff *skb;
	void *skb_data = NULL;
	u16 count = 0;
	u16 next_produce_idx;

	next_produce_idx = rxque->rfq.produce_idx;
	if (++next_produce_idx == rxque->rfq.count)
		next_produce_idx = 0;
	curr_rxbuf = GET_RF_BUFFER(rxque, rxque->rfq.produce_idx);
	next_rxbuf = GET_RF_BUFFER(rxque, next_produce_idx);

	/* this always has a blank rx_buffer*/
	while (next_rxbuf->dma == 0) {
		skb = dev_alloc_skb(adpt->rxbuf_size);
		if (unlikely(!skb)) {
			alx_err(adpt, "alloc rx buffer failed\n");
			break;
		}

		/*
		 * Make buffer alignment 2 beyond a 16 byte boundary
		 * this will result in a 16 byte aligned IP header after
		 * the 14 byte MAC header is removed
		 */
		skb_data = skb->data;
		/*skb_reserve(skb, NET_IP_ALIGN);*/
		curr_rxbuf->skb = skb;
		curr_rxbuf->length = adpt->rxbuf_size;
		curr_rxbuf->dma = dma_map_single(rxque->dev,
						 skb_data,
						 curr_rxbuf->length,
						 DMA_FROM_DEVICE);
		srfd.genr.addr = curr_rxbuf->dma;
		alx_set_rfdesc(rxque, &srfd);
		/*alx_err(adpt, "rx-buffer-addr=%llx\n",
			(u64)curr_rxbuf->dma);*/

		next_produce_idx = rxque->rfq.produce_idx;
		if (++next_produce_idx == rxque->rfq.count)
			next_produce_idx = 0;
		curr_rxbuf = GET_RF_BUFFER(rxque, rxque->rfq.produce_idx);
		next_rxbuf = GET_RF_BUFFER(rxque, next_produce_idx);
		count++;
	}

	if (count) {
		wmb();
		alx_mem_w16(hw, rxque->produce_reg, rxque->rfq.produce_idx);
		netif_dbg(adpt, rx_err, adpt->netdev,
			   "RX[%d]: prod_reg[%x] = 0x%x, rfq.prod_idx = 0x%x\n",
			   rxque->que_idx, rxque->produce_reg,
			   rxque->rfq.produce_idx, rxque->rfq.produce_idx);
	}
	return count;
}


static void alx_clean_rfdesc(struct alx_rx_queue *rxque,
			     union alx_sw_rrdesc *srrd)
{
	struct alx_buffer *rfbuf = rxque->rfq.rfbuff;
	u32 consume_idx = srrd->genr.si;
	u32 i;

	for (i = 0; i < srrd->genr.nor; i++) {
		rfbuf[consume_idx].skb = NULL;
		if (++consume_idx == rxque->rfq.count)
			consume_idx = 0;
	}
	rxque->rfq.consume_idx = consume_idx;
}

#if 0
#define ROLL_BK_NUM 16

static void alx_dump_rrd(struct alx_adapter *adpt, struct alx_rx_queue *rxque)
{
	union alx_sw_rrdesc srrd;
	union alx_hw_rrdesc *hrrd;

	u16 begin, end;


	alx_err(adpt, "PATCH v5, dumpping RRD .... consumer idx=%x\n",
			rxque->rrq.consume_idx);
	begin = (rxque->rrq.consume_idx - ROLL_BK_NUM) % rxque->rrq.count;
	end = (rxque->rrq.consume_idx + ROLL_BK_NUM) % rxque->rrq.count;

  while (begin != end) {
		hrrd = ALX_RRD(rxque, begin);
		srrd.dfmt.dw0 = le32_to_cpu(hrrd->dfmt.dw0);
		srrd.dfmt.dw1 = le32_to_cpu(hrrd->dfmt.dw1);
		srrd.dfmt.dw2 = le32_to_cpu(hrrd->dfmt.dw2);
		srrd.dfmt.dw3 = le32_to_cpu(hrrd->dfmt.dw3);
		alx_err(adpt, "Index:%x\n", begin);
		alx_err(adpt, "rrd->word0/1/2/3:0x%08x/0x%08x/0x%08x/0x%08x\n",
			srrd.dfmt.dw0, srrd.dfmt.dw1, srrd.dfmt.dw2, srrd.dfmt.dw3);

		if (++begin == rxque->rrq.count)
			begin = 0;
	}
}


static void alx_dump_rfd(struct alx_adapter *adpt, struct alx_rx_queue *rxque, u16 idx)
{
	struct alx_buffer *rfbuf;
	u16 begin, end;


	alx_err(adpt, "RFD:%x\n", idx);

	//begin = (idx - 3) % rxque->rfq.count;
	//end = (idx + 3) % rxque->rfq.count;
	begin = 0;
	end = rxque->rfq.count - 1;

	while (begin != end) {
		rfbuf = GET_RF_BUFFER(rxque, begin);
		alx_err(adpt, "IDX(%x): addr=0x%llx\n", begin, (u64)rfbuf->dma);
        if (++begin == rxque->rfq.count)
		begin = 0;
	}
}

static void alx_dump_register(struct alx_adapter *adpt)
{
	struct alx_hw *hw = &adpt->hw;
	u16 reg, count;
	u32 val;

	for (reg = 0x40, count = 0; count < 0x20; count++, reg++) {
		val = alx_read_dbg_reg(hw, reg);
		alx_err(adpt, "DBG-reg(%x)=%08X\n", reg, val);
	}

}

#endif

u32 alx_read_dbg_reg(struct alx_hw *hw, u16 reg)
{
	u32 val;

	alx_mem_w32(hw, 0x1900, reg);
	alx_mem_r32(hw, 0x1904, &val);

	return val;
}

static bool alx_dispatch_rx_irq(struct alx_msix_param *msix,
				struct alx_rx_queue *rxque)
{
	struct alx_adapter *adpt = msix->adpt;
	struct pci_dev *pdev = adpt->pdev;
	struct net_device *netdev  = adpt->netdev;

	union alx_sw_rrdesc srrd;
	struct alx_buffer *rfbuf;
	struct sk_buff *skb;
	struct alx_rx_queue *swque;
	struct alx_sw_buffer *curr_swbuf;
	struct alx_sw_buffer *next_swbuf;

	u16 next_produce_idx;
	u16 count = 0;

	while (1) {
		if (!alx_get_rrdesc(adpt, rxque, &srrd))
			break;

		if (srrd.genr.res || srrd.genr.lene) {
			alx_clean_rfdesc(rxque, &srrd);
			netif_warn(adpt, rx_err, adpt->netdev,
				   "wrong packet! rrd->word3 is 0x%08x\n",
				   srrd.dfmt.dw3);
			continue;
		}

		/* Good Receive */
		if (likely(srrd.genr.nor == 1)) {
			rfbuf = GET_RF_BUFFER(rxque, srrd.genr.si);
			pci_unmap_single(pdev, rfbuf->dma,
					 rfbuf->length, DMA_FROM_DEVICE);
			rfbuf->dma = 0;
			skb = rfbuf->skb;
			netif_dbg(adpt, rx_err, adpt->netdev,
				   "skb addr = %p, rxbuf_len = %x\n",
				   skb->data, rfbuf->length);
		} else {
			/* TODO */
			alx_err(adpt, "alx_dispatch_rx_irq: Multi rfd not support yet!\n");
			break;
		}
		alx_clean_rfdesc(rxque, &srrd);

		skb_put(skb, srrd.genr.pkt_len - ETH_FCS_LEN);
		skb->protocol = eth_type_trans(skb, netdev);
		skb_checksum_none_assert(skb);

		/* start to dispatch */
		swque = adpt->rx_queue[srrd.genr.rss_cpu];
		next_produce_idx = swque->swq.produce_idx;
		if (++next_produce_idx == swque->swq.count)
			next_produce_idx = 0;

		curr_swbuf = GET_SW_BUFFER(swque, swque->swq.produce_idx);
		next_swbuf = GET_SW_BUFFER(swque, next_produce_idx);

		/*
		 * if full, will discard the packet,
		 * and at lease has a blank sw_buffer.
		 */
		if (!next_swbuf->skb) {
			curr_swbuf->skb = skb;
			curr_swbuf->vlan_tag = srrd.genr.vlan_tag;
			curr_swbuf->vlan_flag = srrd.genr.vlan_flag;
			if (++swque->swq.produce_idx == swque->swq.count)
				swque->swq.produce_idx = 0;
		}

		count++;
		if (count == 32)
			break;
	}
	if (count)
		alx_refresh_rx_buffer(rxque);
	return true;
}


static bool alx_handle_srx_irq(struct alx_msix_param *msix,
			       struct alx_rx_queue *rxque,
			       int *num_pkts, int max_pkts)
{
	struct alx_adapter *adpt = msix->adpt;
	struct alx_sw_buffer *swbuf;
	bool retval = true;

	while (rxque->swq.consume_idx != rxque->swq.produce_idx) {
		swbuf = GET_SW_BUFFER(rxque, rxque->swq.consume_idx);

		alx_receive_skb(adpt, swbuf->skb, (u16)swbuf->vlan_tag,
				(bool)swbuf->vlan_flag);
		swbuf->skb = NULL;

		if (++rxque->swq.consume_idx == rxque->swq.count)
			rxque->swq.consume_idx = 0;

		(*num_pkts)++;
		if (*num_pkts >= max_pkts) {
			retval = false;
			break;
		}
	}
	return retval;
}


static bool alx_handle_rx_irq(struct alx_msix_param *msix,
			      struct alx_rx_queue *rxque,
			      int *num_pkts, int max_pkts)
{
	struct alx_adapter *adpt = msix->adpt;
	struct alx_hw *hw = &adpt->hw;
	struct pci_dev *pdev = adpt->pdev;

	union alx_sw_rrdesc srrd;
	struct alx_buffer *rfbuf;
	struct sk_buff *skb;

	u16 hw_consume_idx, num_consume_pkts;
	u16 count = 0;

	alx_mem_r16(hw, rxque->consume_reg, &hw_consume_idx);
	num_consume_pkts = (hw_consume_idx > rxque->rrq.consume_idx) ?
		(hw_consume_idx -  rxque->rrq.consume_idx) :
		(hw_consume_idx + rxque->rrq.count - rxque->rrq.consume_idx);

	while (1) {
		if (!num_consume_pkts)
			break;

		if (!alx_get_rrdesc(adpt, rxque, &srrd))
			break;

		if (srrd.genr.res || srrd.genr.lene) {
			alx_clean_rfdesc(rxque, &srrd);
			netif_warn(adpt, rx_err, adpt->netdev,
				   "wrong packet! rrd->word3 is 0x%08x\n",
				   srrd.dfmt.dw3);
			continue;
		}

		/* TODO: Good Receive */
		if (likely(srrd.genr.nor == 1)) {
			rfbuf = GET_RF_BUFFER(rxque, srrd.genr.si);
			pci_unmap_single(pdev, rfbuf->dma, rfbuf->length,
					 DMA_FROM_DEVICE);
			rfbuf->dma = 0;
			skb = rfbuf->skb;
		} else {
			/* TODO */
			alx_err(adpt, "alx_hande_rx_irq: Multi rfd not support yet!\n");
			break;
		}
		alx_clean_rfdesc(rxque, &srrd);
		skb_put(skb, srrd.genr.pkt_len - ETH_FCS_LEN);
#ifdef MDM_PLATFORM
		/*Don't strip off MAC Header if sending through IPA*/
		if (!ipa_enable) {
			skb->protocol = eth_type_trans(skb, adpt->netdev);
		}
#else
		skb->protocol = eth_type_trans(skb, adpt->netdev);
#endif

		skb_checksum_none_assert(skb);

		/*the HW rrd->vlan_flag is still true when vlan stripping disabled,
		correct it here */
		if (!CHK_HW_FLAG(VLANSTRIP_EN))
			srrd.genr.vlan_flag = false;

#ifdef MDM_PLATFORM
		/* Use IPA path only if VLAN stripping is not done and IPA is
		 * enabled */
		if (ipa_enable && !srrd.genr.vlan_flag)
			alx_receive_skb_ipa(adpt, skb, (u16)srrd.genr.vlan_tag,
					(bool)srrd.genr.vlan_flag,
					(uint8_t)srrd.genr.proto);
		else
#endif
			alx_receive_skb(adpt, skb, (u16)srrd.genr.vlan_tag,
					(bool)srrd.genr.vlan_flag);

		num_consume_pkts--;
		count++;
		(*num_pkts)++;
		if (*num_pkts >= max_pkts)
			break;
	}
	if (count)
		alx_refresh_rx_buffer(rxque);

	return true;
}


static bool alx_handle_tx_irq(struct alx_msix_param *msix,
			      struct alx_tx_queue *txque)
{
	struct alx_adapter *adpt = msix->adpt;
	struct alx_hw *hw = &adpt->hw;
	struct alx_buffer *tpbuf;
	struct alx_ipa_ctx *alx_ipa;
	int num_tx_comp;
	u16 consume_data;

#ifdef MDM_PLATFORM
	if (ipa_enable) {
		alx_ipa = adpt->palx_ipa;
		num_tx_comp = 0;
	}
#endif

	alx_mem_r16(hw, txque->consume_reg, &consume_data);
	netif_dbg(adpt, tx_err, adpt->netdev,
		   "TX[%d]: consume_reg[0x%x] = 0x%x, tpq.consume_idx = 0x%x\n",
		   txque->que_idx, txque->consume_reg, consume_data,
		   txque->tpq.consume_idx);


	while (txque->tpq.consume_idx != consume_data) {
		tpbuf = GET_TP_BUFFER(txque, txque->tpq.consume_idx);
		if (tpbuf->dma) {
			pci_unmap_page(adpt->pdev, tpbuf->dma, tpbuf->length,
				       DMA_TO_DEVICE);
			tpbuf->dma = 0;
		}

		if (tpbuf->skb) {
			/* Since its called by NAPI; we are already in bh
			context; so its safe to free the skb here*/
			dev_kfree_skb(tpbuf->skb);
			tpbuf->skb = NULL;
		}

		if (++txque->tpq.consume_idx == txque->tpq.count)
			txque->tpq.consume_idx = 0;

#ifdef MDM_PLATFORM
		if (ipa_enable)
			/* Update TX Completetion Recieved */
			num_tx_comp++;
#endif
	}

#ifdef MDM_PLATFORM
	if (ipa_enable) {
		/* Release Wakelock if all TX Completion is done */
		spin_lock_bh(&alx_ipa->rm_ipa_lock);
		alx_ipa->alx_tx_completion -= num_tx_comp;
		if (!alx_ipa->ipa_rx_completion && !alx_ipa->alx_tx_completion &&
			(alx_ipa->acquire_wake_src == true)) {
			__pm_relax(&alx_ipa->rm_ipa_wait);
			alx_ipa->acquire_wake_src = false;
		}
		spin_unlock_bh(&alx_ipa->rm_ipa_lock);
	}
#endif

	if (netif_queue_stopped(adpt->netdev) &&
		netif_carrier_ok(adpt->netdev)) {
		netif_wake_queue(adpt->netdev);
	}
	return true;
}


static irqreturn_t alx_msix_timer(int irq, void *data)
{
	struct alx_msix_param *msix = data;
	struct alx_adapter *adpt = msix->adpt;
	struct alx_hw *hw = &adpt->hw;
	u32 isr;

	hw->cbs.disable_msix_intr(hw, msix->vec_idx);

	alx_mem_r32(hw, ALX_ISR, &isr);
	isr = isr & (ALX_ISR_TIMER | ALX_ISR_MANU);


	if (isr == 0) {
		hw->cbs.enable_msix_intr(hw, msix->vec_idx);
		return IRQ_NONE;
	}

	/* Ack ISR */
	alx_mem_w32(hw, ALX_ISR, isr);

	if (isr & ALX_ISR_MANU) {
		adpt->net_stats.tx_carrier_errors++;
		alx_check_lsc(adpt);
	}

	hw->cbs.enable_msix_intr(hw, msix->vec_idx);

	return IRQ_HANDLED;
}


static irqreturn_t alx_msix_alert(int irq, void *data)
{
	struct alx_msix_param *msix = data;
	struct alx_adapter *adpt = msix->adpt;
	struct alx_hw *hw = &adpt->hw;
	u32 isr;

	hw->cbs.disable_msix_intr(hw, msix->vec_idx);

	alx_mem_r32(hw, ALX_ISR, &isr);
	isr = isr & ALX_ISR_ALERT_MASK;

	if (isr == 0) {
		hw->cbs.enable_msix_intr(hw, msix->vec_idx);
		return IRQ_NONE;
	}
	alx_mem_w32(hw, ALX_ISR, isr);

	hw->cbs.enable_msix_intr(hw, msix->vec_idx);

	return IRQ_HANDLED;
}


static irqreturn_t alx_msix_smb(int irq, void *data)
{
	struct alx_msix_param *msix = data;
	struct alx_adapter *adpt = msix->adpt;
	struct alx_hw *hw = &adpt->hw;

	hw->cbs.disable_msix_intr(hw, msix->vec_idx);

	hw->cbs.enable_msix_intr(hw, msix->vec_idx);

	return IRQ_HANDLED;
}


static irqreturn_t alx_msix_phy(int irq, void *data)
{
	struct alx_msix_param *msix = data;
	struct alx_adapter *adpt = msix->adpt;
	struct alx_hw *hw = &adpt->hw;

	hw->cbs.disable_msix_intr(hw, msix->vec_idx);

	if (hw->cbs.ack_phy_intr)
		hw->cbs.ack_phy_intr(hw);

	adpt->net_stats.tx_carrier_errors++;
	alx_check_lsc(adpt);

	hw->cbs.enable_msix_intr(hw, msix->vec_idx);

	return IRQ_HANDLED;
}


/*
 * alx_msix_rtx
 */
static irqreturn_t alx_msix_rtx(int irq, void *data)
{
	struct alx_msix_param *msix = data;
	struct alx_adapter  *adpt = msix->adpt;
	struct alx_hw *hw = &adpt->hw;

	netif_dbg(adpt, intr, adpt->netdev,
		   "msix vec_idx = %d\n", msix->vec_idx);

	hw->cbs.disable_msix_intr(hw, msix->vec_idx);
	if (!msix->rx_count && !msix->tx_count) {
		hw->cbs.enable_msix_intr(hw, msix->vec_idx);
		return IRQ_HANDLED;
	}

	napi_schedule(&msix->napi);
	return IRQ_HANDLED;
}


/*
 * alx_napi_msix_rtx
 */
static int alx_napi_msix_rtx(struct napi_struct *napi, int max_pkts)
{
	struct alx_msix_param *msix =
			       container_of(napi, struct alx_msix_param, napi);
	struct alx_adapter *adpt = msix->adpt;
	struct alx_hw *hw = &adpt->hw;
	struct alx_rx_queue *rxque;
	struct alx_rx_queue *swque;
	struct alx_tx_queue *txque;
	unsigned long flags = 0;
	bool complete = true;
	int num_pkts = 0;
	int rque_idx, tque_idx;
	int i, j;

	netif_dbg(adpt, intr, adpt->netdev,
		   "NAPI: msix vec_idx = %d\n", msix->vec_idx);

	pr_info("NAPI: msix vec_idx = %d\n", msix->vec_idx);

	/* RX */
	for (i = 0; i < msix->rx_count; i++) {
		rque_idx = msix->rx_map[i];
		num_pkts = 0;
		if (CHK_ADPT_FLAG(0, SRSS_EN)) {
			if (!spin_trylock_irqsave(&adpt->rx_lock, flags))
				goto clean_sw_irq;

			for (j = 0; j < adpt->num_hw_rxques; j++)
				alx_dispatch_rx_irq(msix, adpt->rx_queue[j]);

			spin_unlock_irqrestore(&adpt->rx_lock, flags);
clean_sw_irq:
			swque = adpt->rx_queue[rque_idx];
			complete &= alx_handle_srx_irq(msix, swque, &num_pkts,
						       max_pkts);

		} else {
			rxque = adpt->rx_queue[rque_idx];
			complete &= alx_handle_rx_irq(msix, rxque, &num_pkts,
						      max_pkts);
		}
	}


	/* Handle TX */
	for (i = 0; i < msix->tx_count; i++) {
		tque_idx = msix->tx_map[i];
		txque = adpt->tx_queue[tque_idx];
		complete &= alx_handle_tx_irq(msix, txque);
	}

	if (!complete) {
		netif_dbg(adpt, intr, adpt->netdev,
			   "Some packets in the queue are not handled!\n");
		num_pkts = max_pkts;
	}

	netif_dbg(adpt, intr, adpt->netdev,
		   "num_pkts = %d, max_pkts = %d\n", num_pkts, max_pkts);
	/* If all work done, exit the polling mode */
	if (num_pkts < max_pkts) {
		napi_complete(napi);
		if (!CHK_ADPT_FLAG(1, STATE_DOWN))
			hw->cbs.enable_msix_intr(hw, msix->vec_idx);
	}

	return num_pkts;
}



/*
 * alx_napi_legacy_rtx - NAPI Rx polling callback
 */
static int alx_napi_legacy_rtx(struct napi_struct *napi, int max_pkts)
{
	struct alx_msix_param *msix =
				container_of(napi, struct alx_msix_param, napi);
	struct alx_adapter *adpt = msix->adpt;
	struct alx_hw *hw = &adpt->hw;
	int complete = true;
	int num_pkts = 0;
	int que_idx;

	netif_dbg(adpt, intr, adpt->netdev,
		   "NAPI: msix vec_idx = %d\n", msix->vec_idx);

	/* Keep link state information with original netdev */
	if (!netif_carrier_ok(adpt->netdev))
		goto enable_rtx_irq;

	for (que_idx = 0; que_idx < adpt->num_txques; que_idx++)
		complete &= alx_handle_tx_irq(msix, adpt->tx_queue[que_idx]);

	for (que_idx = 0; que_idx < adpt->num_hw_rxques; que_idx++) {
		num_pkts = 0;
		complete &= alx_handle_rx_irq(msix, adpt->rx_queue[que_idx],
					      &num_pkts, max_pkts);
	}

	if (!complete)
		num_pkts = max_pkts;

	if (num_pkts < max_pkts) {
enable_rtx_irq:
		napi_complete(napi);
		hw->intr_mask |= (ALX_ISR_RXQ | ALX_ISR_TXQ);
		alx_mem_w32(hw, ALX_IMR, hw->intr_mask);
	}
	return num_pkts;
}


static void alx_set_msix_flags(struct alx_msix_param *msix,
			       enum alx_msix_type type, int index)
{
	if (type == alx_msix_type_rx) {
		switch (index) {
		case 0:
			SET_MSIX_FLAG(RX0);
			break;
		case 1:
			SET_MSIX_FLAG(RX1);
			break;
		case 2:
			SET_MSIX_FLAG(RX2);
			break;
		case 3:
			SET_MSIX_FLAG(RX3);
			break;
		case 4:
			SET_MSIX_FLAG(RX4);
			break;
		case 5:
			SET_MSIX_FLAG(RX5);
			break;
		case 6:
			SET_MSIX_FLAG(RX6);
			break;
		case 7:
			SET_MSIX_FLAG(RX7);
			break;
		default:
			printk(KERN_ERR "alx_set_msix_flags: rx error.");
			break;
		}
	} else if (type == alx_msix_type_tx) {
		switch (index) {
		case 0:
			SET_MSIX_FLAG(TX0);
			break;
		case 1:
			SET_MSIX_FLAG(TX1);
			break;
		case 2:
			SET_MSIX_FLAG(TX2);
			break;
		case 3:
			SET_MSIX_FLAG(TX3);
			break;
		default:
			printk(KERN_ERR "alx_set_msix_flags: tx error.");
			break;
		}
	} else if (type == alx_msix_type_other) {
		switch (index) {
		case ALX_MSIX_TYPE_OTH_TIMER:
			SET_MSIX_FLAG(TIMER);
			break;
		case ALX_MSIX_TYPE_OTH_ALERT:
			SET_MSIX_FLAG(ALERT);
			break;
		case ALX_MSIX_TYPE_OTH_SMB:
			SET_MSIX_FLAG(SMB);
			break;
		case ALX_MSIX_TYPE_OTH_PHY:
			SET_MSIX_FLAG(PHY);
			break;
		default:
			printk(KERN_ERR "alx_set_msix_flags: other error.");
			break;
		}
	}
}


/* alx_setup_msix_maps */
static int alx_setup_msix_maps(struct alx_adapter *adpt)
{
	int msix_idx = 0;
	int que_idx = 0;
	int num_rxques = adpt->num_rxques;
	int num_txques = adpt->num_txques;
	int num_msix_rxques = adpt->num_msix_rxques;
	int num_msix_txques = adpt->num_msix_txques;
	int num_msix_noques = adpt->num_msix_noques;

	if (CHK_ADPT_FLAG(0, FIXED_MSIX))
		goto fixed_msix_map;

	netif_warn(adpt, ifup, adpt->netdev,
		   "don't support non-fixed msix map\n");
	return -EINVAL;

fixed_msix_map:
	/*
	 * For RX queue msix map
	 */
	msix_idx = 0;
	for (que_idx = 0; que_idx < num_msix_rxques; que_idx++, msix_idx++) {
		struct alx_msix_param *msix = adpt->msix[msix_idx];
		if (que_idx < num_rxques) {
			adpt->rx_queue[que_idx]->msix = msix;
			msix->rx_map[msix->rx_count] = que_idx;
			msix->rx_count++;
			alx_set_msix_flags(msix, alx_msix_type_rx, que_idx);
		}
	}
	if (msix_idx != num_msix_rxques)
		netif_warn(adpt, ifup, adpt->netdev, "msix_idx is wrong\n");

	/*
	 * For TX queue msix map
	 */
	for (que_idx = 0; que_idx < num_msix_txques; que_idx++, msix_idx++) {
		struct alx_msix_param *msix = adpt->msix[msix_idx];
		if (que_idx < num_txques) {
			adpt->tx_queue[que_idx]->msix = msix;
			msix->tx_map[msix->tx_count] = que_idx;
			msix->tx_count++;
			alx_set_msix_flags(msix, alx_msix_type_tx, que_idx);
		}
	}
	if (msix_idx != (num_msix_rxques + num_msix_txques))
		netif_warn(adpt, ifup, adpt->netdev, "msix_idx is wrong\n");


	/*
	 * For NON queue msix map
	 */
	for (que_idx = 0; que_idx < num_msix_noques; que_idx++, msix_idx++) {
		struct alx_msix_param *msix = adpt->msix[msix_idx];
		alx_set_msix_flags(msix, alx_msix_type_other, que_idx);
	}
	return 0;
}


static inline void alx_reset_msix_maps(struct alx_adapter *adpt)
{
	int que_idx, msix_idx;

	for (que_idx = 0; que_idx < adpt->num_rxques; que_idx++)
		adpt->rx_queue[que_idx]->msix = NULL;
	for (que_idx = 0; que_idx < adpt->num_txques; que_idx++)
		adpt->tx_queue[que_idx]->msix = NULL;

	for (msix_idx = 0; msix_idx < adpt->num_msix_intrs; msix_idx++) {
		struct alx_msix_param *msix = adpt->msix[msix_idx];
		memset(msix->rx_map, 0, sizeof(msix->rx_map));
		memset(msix->tx_map, 0, sizeof(msix->tx_map));
		msix->rx_count = 0;
		msix->tx_count = 0;
		CLI_MSIX_FLAG(ALL);
	}
}


/*
 * alx_enable_intr - Enable default interrupt generation settings
 */
static inline void alx_enable_intr(struct alx_adapter *adpt)
{
	struct alx_hw *hw = &adpt->hw;
	int i;

	if (!atomic_dec_and_test(&adpt->irq_sem))
		return;

	if (hw->cbs.enable_legacy_intr)
		hw->cbs.enable_legacy_intr(hw);

	/* enable all MSIX IRQs */
	for (i = 0; i < adpt->num_msix_intrs; i++) {
		if (hw->cbs.disable_msix_intr)
			hw->cbs.disable_msix_intr(hw, i);
		if (hw->cbs.enable_msix_intr)
			hw->cbs.enable_msix_intr(hw, i);
	}
}


/* alx_disable_intr - Mask off interrupt generation on the NIC */
static inline void alx_disable_intr(struct alx_adapter *adpt)
{
	struct alx_hw *hw = &adpt->hw;
	atomic_inc(&adpt->irq_sem);

	if (hw->cbs.disable_legacy_intr)
		hw->cbs.disable_legacy_intr(hw);

	if (CHK_ADPT_FLAG(0, MSIX_EN)) {
		int i;
		for (i = 0; i < adpt->num_msix_intrs; i++) {
			synchronize_irq(adpt->msix_entries[i].vector);
			hw->cbs.disable_msix_intr(hw, i);
		}
	} else {
		synchronize_irq(adpt->pdev->irq);
	}


}


/*
 * alx_interrupt - Interrupt Handler
 */
static irqreturn_t alx_interrupt(int irq, void *data)
{
	struct net_device *netdev  = data;
	struct alx_adapter *adpt = netdev_priv(netdev);
	struct alx_hw *hw = &adpt->hw;
	struct alx_msix_param *msix = adpt->msix[0];
	int max_intrs = ALX_MAX_HANDLED_INTRS;
	u32 isr, status;

	do {
		alx_mem_r32(hw, ALX_ISR, &isr);
		status = isr & hw->intr_mask;

		if (status == 0) {
			alx_mem_w32(hw, ALX_ISR, 0);
			if (max_intrs != ALX_MAX_HANDLED_INTRS)
				return IRQ_HANDLED;
			return IRQ_NONE;
		}

                /* GPHY_INT is received when cabled is plugged in */
                /* Connect to ODU Bridge Driver */
		/* ack ISR to PHY register */
		if (status & ALX_ISR_PHY) {
			hw->cbs.ack_phy_intr(hw);
		}
		/* ack ISR to MAC register */
		alx_mem_w32(hw, ALX_ISR, status | ALX_ISR_DIS);

		if (status & ALX_ISR_ERROR) {
			netif_warn(adpt, intr, adpt->netdev,
				   "isr error (status = 0x%x)\n",
				   status & ALX_ISR_ERROR);
			if (status & ALX_ISR_PCIE_FERR) {
				alx_mem_w16(hw, ALX_DEV_STAT,
					    ALX_DEV_STAT_FERR |
					    ALX_DEV_STAT_NFERR |
					    ALX_DEV_STAT_CERR);
			}
			/* reset MAC */
			SET_ADPT_FLAG(0, TASK_REINIT_REQ);
			alx_task_schedule(adpt);
			return IRQ_HANDLED;
		}

		if (status & (ALX_ISR_RXQ | ALX_ISR_TXQ)) {
			if (napi_schedule_prep(&(msix->napi))) {
				hw->intr_mask &= ~(ALX_ISR_RXQ | ALX_ISR_TXQ);
				alx_mem_w32(hw, ALX_IMR, hw->intr_mask);
				__napi_schedule(&(msix->napi));
			}
		}

		if (status & ALX_ISR_OVER) {
			netif_warn(adpt, intr, adpt->netdev,
				   "TX/RX overflow (status = 0x%x)\n",
				   status & ALX_ISR_OVER);
		}

		/* link event */
		if (status & (ALX_ISR_PHY | ALX_ISR_MANU)) {
			netdev->stats.tx_carrier_errors++;
			alx_check_lsc(adpt);
			break;
		}

	} while (--max_intrs > 0);
	/* re-enable Interrupt*/
	alx_mem_w32(hw, ALX_ISR, 0);
	return IRQ_HANDLED;
}


/*
 * alx_request_msix_irqs - Initialize MSI-X interrupts
 */
static int alx_request_msix_irq(struct alx_adapter *adpt)
{
	struct net_device *netdev = adpt->netdev;
	irqreturn_t (*handler)(int, void *);
	int msix_idx;
	int num_msix_intrs = adpt->num_msix_intrs;
	int rx_idx = 0, tx_idx = 0;
	int i;
	int retval;

	retval = alx_setup_msix_maps(adpt);
	if (retval)
		return retval;

	for (msix_idx = 0; msix_idx < num_msix_intrs; msix_idx++) {
		struct alx_msix_param *msix = adpt->msix[msix_idx];

		if (CHK_MSIX_FLAG(RXS) && CHK_MSIX_FLAG(TXS)) {
			handler = alx_msix_rtx;
			snprintf(msix->name, sizeof(msix->name), "%s:%s%d",
					    netdev->name, "rtx", rx_idx);
			rx_idx++;
			tx_idx++;
		} else if (CHK_MSIX_FLAG(RXS)) {
			handler = alx_msix_rtx;
			snprintf(msix->name, sizeof(msix->name), "%s:%s%d",
					    netdev->name, "rx", rx_idx);
			rx_idx++;
		} else if (CHK_MSIX_FLAG(TXS)) {
			handler = alx_msix_rtx;
			snprintf(msix->name, sizeof(msix->name), "%s:%s%d",
					    netdev->name, "tx", tx_idx);
			tx_idx++;
		} else if (CHK_MSIX_FLAG(TIMER)) {
			handler = alx_msix_timer;
			snprintf(msix->name, sizeof(msix->name), "%s:%s", netdev->name, "timer");
		} else if (CHK_MSIX_FLAG(ALERT)) {
			handler = alx_msix_alert;
			snprintf(msix->name, sizeof(msix->name), "%s:%s", netdev->name, "alert");
		} else if (CHK_MSIX_FLAG(SMB)) {
			handler = alx_msix_smb;
			snprintf(msix->name, sizeof(msix->name), "%s:%s", netdev->name, "smb");
		} else if (CHK_MSIX_FLAG(PHY)) {
			handler = alx_msix_phy;
			snprintf(msix->name, sizeof(msix->name), "%s:%s", netdev->name, "phy");
		} else {
			netif_dbg(adpt, ifup, adpt->netdev,
				   "MSIX entry [%d] is blank\n",
				   msix->vec_idx);
			continue;
		}
		netif_dbg(adpt, ifup, adpt->netdev,
			   "MSIX entry [%d] is %s\n",
			   msix->vec_idx, msix->name);
		retval = request_irq(adpt->msix_entries[msix_idx].vector,
				     handler, 0, msix->name, msix);
		if (retval)
			goto free_msix_irq;

		/* assign the mask for this irq */
		irq_set_affinity_hint(adpt->msix_entries[msix_idx].vector,
				      msix->affinity_mask);
	}
	return retval;


free_msix_irq:
	for (i = 0; i < msix_idx; i++) {
		irq_set_affinity_hint(adpt->msix_entries[i].vector, NULL);
		free_irq(adpt->msix_entries[i].vector, adpt->msix[i]);
	}
	CLI_ADPT_FLAG(0, MSIX_EN);
	pci_disable_msix(adpt->pdev);
	kfree(adpt->msix_entries);
	adpt->msix_entries = NULL;
	return retval;
}


/*
 * alx_request_irq - initialize interrupts
 */
static int alx_request_irq(struct alx_adapter *adpt)
{
	struct net_device *netdev = adpt->netdev;
	int retval;

	/* request MSIX irq */
	if (CHK_ADPT_FLAG(0, MSIX_EN)) {
		retval = alx_request_msix_irq(adpt);
		if (retval) {
			alx_err(adpt, "request msix irq failed, error = %d\n",
				retval);
		}
		goto out;
	}

	/* request MSI irq */
	if (CHK_ADPT_FLAG(0, MSI_EN)) {
		retval = request_irq(adpt->pdev->irq, alx_interrupt, 0,
			netdev->name, netdev);
		if (retval) {
			alx_err(adpt, "request msix irq failed, error = %d\n",
				retval);
		}
		pr_info("%s -- request_irq() suceeded for irq %d\n", __func__, adpt->pdev->irq);
		goto out;
	}

	/* request shared irq */
	retval = request_irq(adpt->pdev->irq, alx_interrupt, IRQF_SHARED,
			netdev->name, netdev);
	if (retval) {
		alx_err(adpt, "request shared irq failed, error = %d\n",
			retval);
	}
out:
	return retval;
}


static void alx_free_irq(struct alx_adapter *adpt)
{
	struct net_device *netdev = adpt->netdev;
	int i;

	if (CHK_ADPT_FLAG(0, MSIX_EN)) {
		for (i = 0; i < adpt->num_msix_intrs; i++) {
			struct alx_msix_param *msix = adpt->msix[i];
			netif_dbg(adpt, ifdown, adpt->netdev,
				   "msix entry = %d\n", i);
			if (!CHK_MSIX_FLAG(ALL))
				continue;
			if (CHK_MSIX_FLAG(RXS) || CHK_MSIX_FLAG(TXS)) {
				irq_set_affinity_hint(
					adpt->msix_entries[i].vector, NULL);
			}
			free_irq(adpt->msix_entries[i].vector, msix);
		}
		alx_reset_msix_maps(adpt);
	} else {
		free_irq(adpt->pdev->irq, netdev);
	}
}

static void alx_vlan_mode(struct net_device *netdev,
			  netdev_features_t features)
{
	struct alx_adapter *adpt = netdev_priv(netdev);
	struct alx_hw *hw = &adpt->hw;

	if (!CHK_ADPT_FLAG(1, STATE_DOWN))
		alx_disable_intr(adpt);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
	if (features & NETIF_F_HW_VLAN_CTAG_RX) {
#else
	if (features & NETIF_F_HW_VLAN_RX) {
#endif
		/* enable VLAN tag insert/strip */
		SET_HW_FLAG(VLANSTRIP_EN);
	} else {
		/* disable VLAN tag insert/strip */
		CLI_HW_FLAG(VLANSTRIP_EN);
	}

	hw->cbs.config_mac_ctrl(hw);

	if (!CHK_ADPT_FLAG(1, STATE_DOWN))
		alx_enable_intr(adpt);
}


static void alx_restore_vlan(struct alx_adapter *adpt)
{
	alx_vlan_mode(adpt->netdev, adpt->netdev->features);
}


static void alx_napi_enable_all(struct alx_adapter *adpt)
{
	struct alx_msix_param *msix;
	int num_msix_intrs = adpt->num_msix_intrs;
	int msix_idx;

	if (!CHK_ADPT_FLAG(0, MSIX_EN))
		num_msix_intrs = 1;

	for (msix_idx = 0; msix_idx < num_msix_intrs; msix_idx++) {
		struct napi_struct *napi;
		msix = adpt->msix[msix_idx];
		napi = &msix->napi;
		napi_enable(napi);
	}
}


static void alx_napi_disable_all(struct alx_adapter *adpt)
{
	struct alx_msix_param *msix;
	int num_msix_intrs = adpt->num_msix_intrs;
	int msix_idx;

	if (!CHK_ADPT_FLAG(0, MSIX_EN))
		num_msix_intrs = 1;

	for (msix_idx = 0; msix_idx < num_msix_intrs; msix_idx++) {
		msix = adpt->msix[msix_idx];
		napi_disable(&msix->napi);
	}
}


static void alx_clean_tx_queue(struct alx_tx_queue *txque)
{
	struct device *dev = txque->dev;
	unsigned long size;
	u16 i;

	/* ring already cleared, nothing to do */
	if (!txque->tpq.tpbuff)
		return;

	for (i = 0; i < txque->tpq.count; i++) {
		struct alx_buffer *tpbuf;
		tpbuf = GET_TP_BUFFER(txque, i);
		if (tpbuf->dma) {
			pci_unmap_single(to_pci_dev(dev),
					tpbuf->dma,
					tpbuf->length,
					DMA_TO_DEVICE);
			tpbuf->dma = 0;
		}
		if (tpbuf->skb) {
			dev_kfree_skb_any(tpbuf->skb);
			tpbuf->skb = NULL;
		}
	}

	size = sizeof(struct alx_buffer) * txque->tpq.count;
	memset(txque->tpq.tpbuff, 0, size);

	/* Zero out Tx-buffers */
	memset(txque->tpq.tpdesc, 0, txque->tpq.size);

	txque->tpq.consume_idx = 0;
	txque->tpq.produce_idx = 0;
}


/*
 * alx_clean_all_tx_queues
 */
static void alx_clean_all_tx_queues(struct alx_adapter *adpt)
{
	int i;

	for (i = 0; i < adpt->num_txques; i++)
		alx_clean_tx_queue(adpt->tx_queue[i]);
}


static void alx_clean_rx_queue(struct alx_rx_queue *rxque)
{
	struct device *dev = rxque->dev;
	unsigned long size;
	int i;

	if (CHK_RX_FLAG(HW_QUE)) {
		/* ring already cleared, nothing to do */
		if (!rxque->rfq.rfbuff)
			goto clean_sw_queue;

		for (i = 0; i < rxque->rfq.count; i++) {
			struct alx_buffer *rfbuf;
			rfbuf = GET_RF_BUFFER(rxque, i);

			if (rfbuf->dma) {
				pci_unmap_single(to_pci_dev(dev),
						rfbuf->dma,
						rfbuf->length,
						DMA_FROM_DEVICE);
				rfbuf->dma = 0;
			}
			if (rfbuf->skb) {
				dev_kfree_skb(rfbuf->skb);
				rfbuf->skb = NULL;
			}
		}
		size =  sizeof(struct alx_buffer) * rxque->rfq.count;
		memset(rxque->rfq.rfbuff, 0, size);

		/* zero out the descriptor ring */
		memset(rxque->rrq.rrdesc, 0, rxque->rrq.size);
		rxque->rrq.produce_idx = 0;
		rxque->rrq.consume_idx = 0;

		memset(rxque->rfq.rfdesc, 0, rxque->rfq.size);
		rxque->rfq.produce_idx = 0;
		rxque->rfq.consume_idx = 0;
	}
clean_sw_queue:
	if (CHK_RX_FLAG(SW_QUE)) {
		/* ring already cleared, nothing to do */
		if (!rxque->swq.swbuff)
			return;

		for (i = 0; i < rxque->swq.count; i++) {
			struct alx_sw_buffer *swbuf;
			swbuf = GET_SW_BUFFER(rxque, i);

			/* swq doesn't map DMA*/

			if (swbuf->skb) {
				dev_kfree_skb(swbuf->skb);
				swbuf->skb = NULL;
			}
		}
		size =  sizeof(struct alx_buffer) * rxque->swq.count;
		memset(rxque->swq.swbuff, 0, size);

		/* swq doesn't have any descripter rings */
		rxque->swq.produce_idx = 0;
		rxque->swq.consume_idx = 0;
	}
}


/*
 * alx_clean_all_rx_queues
 */
static void alx_clean_all_rx_queues(struct alx_adapter *adpt)
{
	int i;
	for (i = 0; i < adpt->num_rxques; i++)
		alx_clean_rx_queue(adpt->rx_queue[i]);
}


/*
 * alx_set_rss_queues: Allocate queues for RSS
 */
static inline void alx_set_num_txques(struct alx_adapter *adpt)
{
	struct alx_hw *hw = &adpt->hw;

	if (hw->mac_type == alx_mac_l1f || hw->mac_type == alx_mac_l2f)
		adpt->num_txques = 4;
	else
		adpt->num_txques = 2;
}


/*
 * alx_set_rss_queues: Allocate queues for RSS
 */
static inline void alx_set_num_rxques(struct alx_adapter *adpt)
{
	if (CHK_ADPT_FLAG(0, SRSS_CAP)) {
		adpt->num_hw_rxques = 1;
		adpt->num_sw_rxques = adpt->max_rxques;
		adpt->num_rxques =
			max_t(u16, adpt->num_hw_rxques, adpt->num_sw_rxques);
	}
}


/*
 * alx_set_num_queues: Allocate queues for device, feature dependant
 */
static void alx_set_num_queues(struct alx_adapter *adpt)
{
	/* Start with default case */
	adpt->num_txques = 1;
	adpt->num_rxques = 1;
	adpt->num_hw_rxques = 1;
	adpt->num_sw_rxques = 0;

	alx_set_num_rxques(adpt);
	alx_set_num_txques(adpt);
}


/* alx_alloc_all_rtx_queue - allocate all queues */
static int alx_alloc_all_rtx_queue(struct alx_adapter *adpt)
{
	int que_idx;

	for (que_idx = 0; que_idx < adpt->num_txques; que_idx++) {
		struct alx_tx_queue *txque = adpt->tx_queue[que_idx];

		txque = kzalloc(sizeof(struct alx_tx_queue), GFP_KERNEL);
		if (!txque)
			goto err_alloc_tx_queue;
		txque->tpq.count = adpt->num_txdescs;
		txque->que_idx = que_idx;
		txque->dev = &adpt->pdev->dev;
		txque->netdev = adpt->netdev;

		adpt->tx_queue[que_idx] = txque;
	}

	for (que_idx = 0; que_idx < adpt->num_rxques; que_idx++) {
		struct alx_rx_queue *rxque = adpt->rx_queue[que_idx];

		rxque = kzalloc(sizeof(struct alx_rx_queue), GFP_KERNEL);
		if (!rxque)
			goto err_alloc_rx_queue;
		rxque->rrq.count = adpt->num_rxdescs;
		rxque->rfq.count = adpt->num_rxdescs;
		rxque->swq.count = adpt->num_rxdescs;
		rxque->que_idx = que_idx;
		rxque->dev = &adpt->pdev->dev;
		rxque->netdev = adpt->netdev;

		if (CHK_ADPT_FLAG(0, SRSS_EN)) {
			if (que_idx < adpt->num_hw_rxques)
				SET_RX_FLAG(HW_QUE);
			if (que_idx < adpt->num_sw_rxques)
				SET_RX_FLAG(SW_QUE);
		} else {
			SET_RX_FLAG(HW_QUE);
		}
		adpt->rx_queue[que_idx] = rxque;
	}
	netif_dbg(adpt, probe, adpt->netdev,
		  "num_tx_descs = %d, num_rx_descs = %d\n",
		  adpt->num_txdescs, adpt->num_rxdescs);
	return 0;

err_alloc_rx_queue:
	alx_err(adpt, "goto err_alloc_rx_queue");
	for (que_idx = 0; que_idx < adpt->num_rxques; que_idx++)
		kfree(adpt->rx_queue[que_idx]);
err_alloc_tx_queue:
	alx_err(adpt, "goto err_alloc_tx_queue");
	for (que_idx = 0; que_idx < adpt->num_txques; que_idx++)
		kfree(adpt->tx_queue[que_idx]);
	return -ENOMEM;
}


/* alx_free_all_rtx_queue */
static void alx_free_all_rtx_queue(struct alx_adapter *adpt)
{
	int que_idx;

	for (que_idx = 0; que_idx < adpt->num_txques; que_idx++) {
		kfree(adpt->tx_queue[que_idx]);
		adpt->tx_queue[que_idx] = NULL;
	}
	for (que_idx = 0; que_idx < adpt->num_rxques; que_idx++) {
		kfree(adpt->rx_queue[que_idx]);
		adpt->rx_queue[que_idx] = NULL;
	}
}


/* alx_set_interrupt_param - set interrupt parameter */
static int alx_set_interrupt_param(struct alx_adapter *adpt)
{
	struct alx_msix_param *msix;
	int (*poll)(struct napi_struct *, int);
	int msix_idx;

	if (CHK_ADPT_FLAG(0, MSIX_EN)) {
		poll = &alx_napi_msix_rtx;
	} else {
		adpt->num_msix_intrs = 1;
		poll = &alx_napi_legacy_rtx;
	}

	for (msix_idx = 0; msix_idx < adpt->num_msix_intrs; msix_idx++) {
		msix = kzalloc(sizeof(struct alx_msix_param),
					   GFP_KERNEL);
		if (!msix)
			goto err_alloc_msix;
		msix->adpt = adpt;
		msix->vec_idx = msix_idx;
		/* Allocate the affinity_hint cpumask, configure the mask */
		if (!alloc_cpumask_var(&msix->affinity_mask, GFP_KERNEL))
			goto err_alloc_cpumask;

		cpumask_set_cpu((msix_idx % num_online_cpus()),
				msix->affinity_mask);

		netif_napi_add(adpt->netdev, &msix->napi, (*poll), 64);
		adpt->msix[msix_idx] = msix;
	}
	return 0;

err_alloc_cpumask:
	kfree(msix);
	adpt->msix[msix_idx] = NULL;
err_alloc_msix:
	for (msix_idx--; msix_idx >= 0; msix_idx--) {
		msix = adpt->msix[msix_idx];
		netif_napi_del(&msix->napi);
		free_cpumask_var(msix->affinity_mask);
		kfree(msix);
		adpt->msix[msix_idx] = NULL;
	}
	alx_err(adpt, "can't allocate memory\n");
	return -ENOMEM;
}


/*
 * alx_reset_interrupt_param - Free memory allocated for interrupt vectors
 */
static void alx_reset_interrupt_param(struct alx_adapter *adpt)
{
	int msix_idx;

	for (msix_idx = 0; msix_idx < adpt->num_msix_intrs; msix_idx++) {
		struct alx_msix_param *msix = adpt->msix[msix_idx];
		netif_napi_del(&msix->napi);
		free_cpumask_var(msix->affinity_mask);
		kfree(msix);
		adpt->msix[msix_idx] = NULL;
	}
}


/* set msix interrupt mode */
static int alx_set_msix_interrupt_mode(struct alx_adapter *adpt)
{
	int msix_intrs, msix_idx;
	int retval = 0;

	adpt->msix_entries = kcalloc(adpt->max_msix_intrs,
				sizeof(struct msix_entry), GFP_KERNEL);
	if (!adpt->msix_entries) {
		netif_dbg(adpt, probe, adpt->netdev,
			   "can't allocate msix entry\n");
		CLI_ADPT_FLAG(0, MSIX_EN);
		goto try_msi_mode;
	}

	for (msix_idx = 0; msix_idx < adpt->max_msix_intrs; msix_idx++)
		adpt->msix_entries[msix_idx].entry = msix_idx;


	msix_intrs = adpt->max_msix_intrs;
	while (msix_intrs >= adpt->min_msix_intrs) {
		retval = pci_enable_msix(adpt->pdev, adpt->msix_entries,
				      msix_intrs);
		if (!retval) /* Success in acquiring all requested vectors. */
			break;
		else if (retval < 0)
			msix_intrs = 0; /* Nasty failure, quit now */
		else /* error == number of vectors we should try again with */
			msix_intrs = retval;
	}
	if (msix_intrs < adpt->min_msix_intrs) {
		netif_dbg(adpt, probe, adpt->netdev,
			   "can't enable MSI-X interrupts\n");
		CLI_ADPT_FLAG(0, MSIX_EN);
		kfree(adpt->msix_entries);
		adpt->msix_entries = NULL;
		goto try_msi_mode;
	}

	netif_dbg(adpt, probe, adpt->netdev,
		   "enable MSI-X interrupts, num_msix_intrs = %d\n",
		   msix_intrs);
	SET_ADPT_FLAG(0, MSIX_EN);
	if (CHK_ADPT_FLAG(0, SRSS_CAP))
		SET_ADPT_FLAG(0, SRSS_EN);

	adpt->num_msix_intrs = min_t(int, msix_intrs, adpt->max_msix_intrs);
	retval = 0;
	return retval;

try_msi_mode:
	CLI_ADPT_FLAG(0, SRSS_CAP);
	CLI_ADPT_FLAG(0, SRSS_EN);
	alx_set_num_queues(adpt);
	retval = -1;
	return retval;
}


/* set msi interrupt mode */
static int alx_set_msi_interrupt_mode(struct alx_adapter *adpt)
{
	int retval;

	retval = pci_enable_msi(adpt->pdev);
	adpt->netdev->irq = adpt->pdev->irq;
	if (retval) {
		netif_dbg(adpt, probe, adpt->netdev,
			   "can't enable MSI interrupt, error = %d\n", retval);
		return retval;
	}
	SET_ADPT_FLAG(0, MSI_EN);
	return retval;
}


/* set interrupt mode */
static int alx_set_interrupt_mode(struct alx_adapter *adpt)
{
	int retval = 0;

	if (CHK_ADPT_FLAG(0, MSIX_CAP)) {
		netif_dbg(adpt, probe, adpt->netdev,
			   "try to set MSIX interrupt\n");
		retval = alx_set_msix_interrupt_mode(adpt);
		if (!retval)
			return retval;
	}

	if (CHK_ADPT_FLAG(0, MSI_CAP)) {
		netif_dbg(adpt, probe, adpt->netdev,
			   "try to set MSI interrupt\n");
		retval = alx_set_msi_interrupt_mode(adpt);
		if (!retval)
			return retval;
	}

	netif_dbg(adpt, probe, adpt->netdev,
		   "can't enable MSIX and MSI, will enable shared interrupt\n");
	retval = 0;
	return retval;
}


static void alx_reset_interrupt_mode(struct alx_adapter *adpt)
{
	if (CHK_ADPT_FLAG(0, MSIX_EN)) {
		CLI_ADPT_FLAG(0, MSIX_EN);
		pci_disable_msix(adpt->pdev);
		kfree(adpt->msix_entries);
		adpt->msix_entries = NULL;
	} else if (CHK_ADPT_FLAG(0, MSI_EN)) {
		CLI_ADPT_FLAG(0, MSI_EN);
		pci_disable_msi(adpt->pdev);
	}
}


static int __devinit alx_init_adapter_special(struct alx_adapter *adpt)
{
	switch (adpt->hw.mac_type) {
	case alx_mac_l1f:
	case alx_mac_l2f:
		goto init_alf_adapter;
		break;
	case alx_mac_l1c:
	case alx_mac_l2c:
		goto init_alc_adapter;
		break;
	case alx_mac_l1d_v1:
	case alx_mac_l1d_v2:
	case alx_mac_l2cb_v1:
	case alx_mac_l2cb_v20:
	case alx_mac_l2cb_v21:
		adpt->hw.bHibBug = true;
		goto init_alc_adapter;
		break;
	default:
		break;
	}
	return -1;

init_alc_adapter:
	if (CHK_ADPT_FLAG(0, MSIX_CAP))
		alx_err(adpt, "ALC doesn't support MSIX\n");

	/* msi for tx, rx and none queues */
	adpt->num_msix_txques = 0;
	adpt->num_msix_rxques = 0;
	adpt->num_msix_noques = 0;
	return 0;

init_alf_adapter:
	if (CHK_ADPT_FLAG(0, MSIX_CAP)) {
		/* msix for tx, rx and none queues */
		adpt->num_msix_txques = 4;
		adpt->num_msix_rxques = 8;
		adpt->num_msix_noques = ALF_MAX_MSIX_NOQUE_INTRS;

		/* msix vector range */
		adpt->max_msix_intrs = ALF_MAX_MSIX_INTRS;
		adpt->min_msix_intrs = ALF_MIN_MSIX_INTRS;
	} else {
		/* msi for tx, rx and none queues */
		adpt->num_msix_txques = 0;
		adpt->num_msix_rxques = 0;
		adpt->num_msix_noques = 0;
	}
	return 0;

}


/*
 * alx_init_adapter
 */
static int __devinit alx_init_adapter(struct alx_adapter *adpt)
{
	struct alx_hw *hw   = &adpt->hw;
	struct pci_dev	*pdev = adpt->pdev;
	u16 revision;
	int max_frame;

	/* PCI config space info */
	hw->pci_venid = pdev->vendor;
	hw->pci_devid = pdev->device;
	alx_cfg_r16(hw, PCI_CLASS_REVISION, &revision);
	hw->pci_revid = revision & 0xFF;
	hw->pci_sub_venid = pdev->subsystem_vendor;
	hw->pci_sub_devid = pdev->subsystem_device;

	if (alx_init_hw_callbacks(adpt) != 0) {
		alx_err(adpt, "set HW function pointers failed\n");
		return -1;
	}

	if (hw->cbs.identify_nic(hw) != 0) {
		alx_err(adpt, "HW is disabled\n");
		return -1;
	}

	/* Set adapter flags */
	switch (hw->mac_type) {
	case alx_mac_l1f:
	case alx_mac_l2f:
#ifdef CONFIG_ALX_MSI
		SET_ADPT_FLAG(0, MSI_CAP);
#endif
#ifdef CONFIG_ALX_MSIX
		SET_ADPT_FLAG(0, MSIX_CAP);
#endif
		if (CHK_ADPT_FLAG(0, MSIX_CAP)) {
			SET_ADPT_FLAG(0, FIXED_MSIX);
			SET_ADPT_FLAG(0, MRQ_CAP);
#ifdef CONFIG_ALX_RSS
			SET_ADPT_FLAG(0, SRSS_CAP);
#endif
		}
		pdev->dev_flags |= PCI_DEV_FLAGS_MSI_INTX_DISABLE_BUG;
		break;
	case alx_mac_l1c:
	case alx_mac_l1d_v1:
	case alx_mac_l1d_v2:
	case alx_mac_l2c:
	case alx_mac_l2cb_v1:
	case alx_mac_l2cb_v20:
	case alx_mac_l2cb_v21:
#ifdef CONFIG_ALX_MSI
		SET_ADPT_FLAG(0, MSI_CAP);
#endif
		break;
	default:
		break;
	}

	/* set default for alx_adapter */
	adpt->max_msix_intrs = 1;
	adpt->min_msix_intrs = 1;
	max_frame = adpt->netdev->mtu + ETH_HLEN + ETH_FCS_LEN + VLAN_HLEN;
	adpt->rxbuf_size = adpt->netdev->mtu > ALX_DEF_RX_BUF_SIZE ?
			   ALIGN(max_frame, 8) : ALX_DEF_RX_BUF_SIZE;
	adpt->wol = 0;
	device_set_wakeup_enable(&pdev->dev, false);

	/* set default for alx_hw */
	hw->link_up = false;
	hw->link_speed = ALX_LINK_SPEED_UNKNOWN;
	hw->preamble = 7;
	hw->intr_mask = ALX_IMR_NORMAL_MASK;
	hw->smb_timer = 400; /* 400ms */
	hw->mtu = adpt->netdev->mtu;
#ifdef MDM_PLATFORM
	if (ipa_enable)
		hw->imt = 500;       /* For MDM set it to 1ms */
	else
#endif
		hw->imt = 100;       /* set to 200us */

	/* set default for wrr */
	hw->wrr_prio0 = 4;
	hw->wrr_prio1 = 4;
	hw->wrr_prio2 = 4;
	hw->wrr_prio3 = 4;
	hw->wrr_mode = alx_wrr_mode_none;

	/* set default flow control settings */
	hw->req_fc_mode = alx_fc_full;
	hw->cur_fc_mode = alx_fc_full;	/* init for ethtool output */
	hw->disable_fc_autoneg = false;
	hw->fc_was_autonegged = false;
	hw->fc_single_pause = true;

	/* set default for rss info*/
	hw->rss_hstype = 0;
	hw->rss_mode = alx_rss_mode_disable;
	hw->rss_idt_size = 0;
	hw->rss_base_cpu = 0;
	memset(hw->rss_idt, 0x0, sizeof(hw->rss_idt));
	memset(hw->rss_key, 0x0, sizeof(hw->rss_key));

	atomic_set(&adpt->irq_sem, 1);
	spin_lock_init(&adpt->tx_lock);
	spin_lock_init(&adpt->rx_lock);

	alx_init_adapter_special(adpt);

	if (hw->cbs.init_phy) {
		if (hw->cbs.init_phy(hw))
			return -EINVAL;
	}

	SET_ADPT_FLAG(1, STATE_DOWN);
	return 0;
}


static int alx_set_register_info_special(struct alx_adapter *adpt)
{
	struct alx_hw *hw = &adpt->hw;
	int num_txques = adpt->num_txques;

	switch (adpt->hw.mac_type) {
	case alx_mac_l1f:
	case alx_mac_l2f:
		goto cache_alf_register;
		break;
	case alx_mac_l1c:
	case alx_mac_l1d_v1:
	case alx_mac_l1d_v2:
	case alx_mac_l2c:
	case alx_mac_l2cb_v1:
	case alx_mac_l2cb_v20:
	case alx_mac_l2cb_v21:
		goto cache_alc_register;
		break;
	default:
		break;
	}
	return -1;

cache_alc_register:
	/* setting for Produce Index and Consume Index */
	adpt->rx_queue[0]->produce_reg = hw->rx_prod_reg[0];
	adpt->rx_queue[0]->consume_reg = hw->rx_cons_reg[0];

	switch (num_txques) {
	case 2:
		adpt->tx_queue[1]->produce_reg = hw->tx_prod_reg[1];
		adpt->tx_queue[1]->consume_reg = hw->tx_cons_reg[1];
	case 1:
		adpt->tx_queue[0]->produce_reg = hw->tx_prod_reg[0];
		adpt->tx_queue[0]->consume_reg = hw->tx_cons_reg[0];
		break;
	}
	return 0;

cache_alf_register:
	/* setting for Produce Index and Consume Index */
	adpt->rx_queue[0]->produce_reg = hw->rx_prod_reg[0];
	adpt->rx_queue[0]->consume_reg = hw->rx_cons_reg[0];

	switch (num_txques) {
	case 4:
		adpt->tx_queue[3]->produce_reg = hw->tx_prod_reg[3];
		adpt->tx_queue[3]->consume_reg = hw->tx_cons_reg[3];
	case 3:
		adpt->tx_queue[2]->produce_reg = hw->tx_prod_reg[2];
		adpt->tx_queue[2]->consume_reg = hw->tx_cons_reg[2];
	case 2:
		adpt->tx_queue[1]->produce_reg = hw->tx_prod_reg[1];
		adpt->tx_queue[1]->consume_reg = hw->tx_cons_reg[1];
	case 1:
		adpt->tx_queue[0]->produce_reg = hw->tx_prod_reg[0];
		adpt->tx_queue[0]->consume_reg = hw->tx_cons_reg[0];
	}
	return 0;
}


/* alx_alloc_tx_descriptor - allocate Tx Descriptors */
static int alx_alloc_tx_descriptor(struct alx_adapter *adpt,
				   struct alx_tx_queue *txque)
{
	struct alx_ring_header *ring_header = &adpt->ring_header;
	int size;

	netif_dbg(adpt, ifup, adpt->netdev,
		   "tpq.count = %d\n", txque->tpq.count);

	size = sizeof(struct alx_buffer) * txque->tpq.count;
	txque->tpq.tpbuff = kzalloc(size, GFP_KERNEL);
	if (!txque->tpq.tpbuff)
		goto err_alloc_tpq_buffer;

	/* round up to nearest 4K */
	txque->tpq.size = txque->tpq.count * sizeof(union alx_hw_tpdesc);

	txque->tpq.tpdma = ring_header->dma + ring_header->used;
	txque->tpq.tpdesc = ring_header->desc + ring_header->used;
	adpt->hw.tpdma[txque->que_idx] = (u64)txque->tpq.tpdma;
	ring_header->used += ALIGN(txque->tpq.size, 8);

	txque->tpq.produce_idx = 0;
	txque->tpq.consume_idx = 0;
	txque->max_packets = txque->tpq.count;
	return 0;

err_alloc_tpq_buffer:
	alx_err(adpt, "Unable to allocate memory for the Tx descriptor\n");
	return -ENOMEM;
}


/* alx_alloc_all_tx_descriptor - allocate all Tx Descriptors */
static int alx_alloc_all_tx_descriptor(struct alx_adapter *adpt)
{
	int i, retval = 0;
	netif_dbg(adpt, ifup, adpt->netdev,
		   "num_tques = %d\n", adpt->num_txques);

	for (i = 0; i < adpt->num_txques; i++) {
		retval = alx_alloc_tx_descriptor(adpt, adpt->tx_queue[i]);
		if (!retval)
			continue;

		alx_err(adpt, "Allocation for Tx Queue %u failed\n", i);
		break;
	}

	return retval;
}


/* alx_alloc_rx_descriptor - allocate Rx Descriptors */
static int alx_alloc_rx_descriptor(struct alx_adapter *adpt,
				   struct alx_rx_queue *rxque)
{
	struct alx_ring_header *ring_header = &adpt->ring_header;
	int size;

	netif_dbg(adpt, ifup, adpt->netdev,
		   "RRD.count = %d, RFD.count = %d, SWD.count = %d\n",
		   rxque->rrq.count, rxque->rfq.count, rxque->swq.count);

	if (CHK_RX_FLAG(HW_QUE)) {
		/* alloc buffer info */
		size = sizeof(struct alx_buffer) * rxque->rfq.count;
		rxque->rfq.rfbuff = kzalloc(size, GFP_KERNEL);
		if (!rxque->rfq.rfbuff)
			goto err_alloc_rfq_buffer;
		/*
		 * set dma's point of rrq and rfq
		 */

		/* Round up to nearest 4K */
		rxque->rrq.size =
			rxque->rrq.count * sizeof(union alx_hw_rrdesc);
		rxque->rfq.size =
			rxque->rfq.count * sizeof(union alx_hw_rfdesc);

		rxque->rrq.rrdma = ring_header->dma + ring_header->used;
		rxque->rrq.rrdesc = ring_header->desc + ring_header->used;
		adpt->hw.rrdma[rxque->que_idx] = (u64)rxque->rrq.rrdma;
		ring_header->used += ALIGN(rxque->rrq.size, 8);

		rxque->rfq.rfdma = ring_header->dma + ring_header->used;
		rxque->rfq.rfdesc = ring_header->desc + ring_header->used;
		adpt->hw.rfdma[rxque->que_idx] = (u64)rxque->rfq.rfdma;
		ring_header->used += ALIGN(rxque->rfq.size, 8);

		/* clean all counts within rxque */
		rxque->rrq.produce_idx = 0;
		rxque->rrq.consume_idx = 0;

		rxque->rfq.produce_idx = 0;
		rxque->rfq.consume_idx = 0;
	}

	if (CHK_RX_FLAG(SW_QUE)) {
		size = sizeof(struct alx_sw_buffer) * rxque->swq.count;
		rxque->swq.swbuff = kzalloc(size, GFP_KERNEL);
		if (!rxque->swq.swbuff)
			goto err_alloc_swq_buffer;

		rxque->swq.consume_idx = 0;
		rxque->swq.produce_idx = 0;
	}

	rxque->max_packets = rxque->rrq.count / 2;
	return 0;

err_alloc_swq_buffer:
	kfree(rxque->rfq.rfbuff);
	rxque->rfq.rfbuff = NULL;
err_alloc_rfq_buffer:
	alx_err(adpt, "Unable to allocate memory for the Rx descriptor\n");
	return -ENOMEM;
}


/* alx_alloc_all_rx_descriptor - allocate all Rx Descriptors */
static int alx_alloc_all_rx_descriptor(struct alx_adapter *adpt)
{
	int i, error = 0;

	for (i = 0; i < adpt->num_rxques; i++) {
		error = alx_alloc_rx_descriptor(adpt, adpt->rx_queue[i]);
		if (!error)
			continue;
		alx_err(adpt, "Allocation for Rx Queue %u failed\n", i);
		break;
	}

	return error;
}


/* alx_free_tx_descriptor - Free Tx Descriptor */
static void alx_free_tx_descriptor(struct alx_tx_queue *txque)
{
	alx_clean_tx_queue(txque);

	kfree(txque->tpq.tpbuff);
	txque->tpq.tpbuff = NULL;

	/* if not set, then don't free */
	if (!txque->tpq.tpdesc)
		return;
	txque->tpq.tpdesc = NULL;
}


/* alx_free_all_tx_descriptor - Free all Tx Descriptor */
static void alx_free_all_tx_descriptor(struct alx_adapter *adpt)
{
	int i;

	for (i = 0; i < adpt->num_txques; i++)
		alx_free_tx_descriptor(adpt->tx_queue[i]);
}


/* alx_free_all_rx_descriptor - Free all Rx Descriptor */
static void alx_free_rx_descriptor(struct alx_rx_queue *rxque)
{
	alx_clean_rx_queue(rxque);

	if (CHK_RX_FLAG(HW_QUE)) {
		kfree(rxque->rfq.rfbuff);
		rxque->rfq.rfbuff = NULL;

		/* if not set, then don't free */
		if (!rxque->rrq.rrdesc)
			return;
		rxque->rrq.rrdesc = NULL;

		if (!rxque->rfq.rfdesc)
			return;
		rxque->rfq.rfdesc = NULL;
	}

	if (CHK_RX_FLAG(SW_QUE)) {
		kfree(rxque->swq.swbuff);
		rxque->swq.swbuff = NULL;
	}
}

#ifdef MDM_PLATFORM
static int alx_alloc_flow_ctrl_desc(struct alx_adapter *adpt)
{
	int i;
	struct alx_ipa_rx_desc_node *node = NULL;

	for (i = 0; i < ALX_IPA_SYS_PIPE_DNE_PKTS; i++) {
		node = (struct alx_ipa_rx_desc_node *)
				kzalloc(sizeof(struct alx_ipa_rx_desc_node),
					GFP_KERNEL);
		if (!node) {
			pr_err("%s -- Only able to allocate %d nodes \n"
						,__func__, adpt->freeq_cnt);
			return -ENOMEM;
		}
		spin_lock(&adpt->flow_ctrl_lock);
		adpt->freeq_cnt++;
		list_add_tail(&node->link, &adpt->free_queue_head);
		spin_unlock(&adpt->flow_ctrl_lock);
	}
	return 0;
}

static void alx_free_flow_ctrl_desc(struct alx_adapter *adpt)
{
	struct alx_ipa_rx_desc_node *node, *tmp;

	spin_lock_bh(&adpt->flow_ctrl_lock);
	list_for_each_entry_safe(node, tmp, &adpt->free_queue_head, link) {
		list_del(&node->link);
		kfree(node);
		adpt->freeq_cnt--;
	}
	spin_unlock_bh(&adpt->flow_ctrl_lock);
	if (adpt->freeq_cnt != 0) {
		pr_err("%s - Memory Leak Detected \n",__func__);
		BUG();
	}
}
#endif

/* alx_free_all_rx_descriptor - Free all Rx Descriptor */
static void alx_free_all_rx_descriptor(struct alx_adapter *adpt)
{
	int i;
	for (i = 0; i < adpt->num_rxques; i++)
		alx_free_rx_descriptor(adpt->rx_queue[i]);
}


/*
 * alx_alloc_all_rtx_descriptor - allocate Tx / RX descriptor queues
 */
static int alx_alloc_all_rtx_descriptor(struct alx_adapter *adpt)
{
	struct device *dev = &adpt->pdev->dev;
	struct alx_ring_header *ring_header = &adpt->ring_header;
	int num_tques = adpt->num_txques;
	int num_rques = adpt->num_hw_rxques;
	unsigned int num_tx_descs = adpt->num_txdescs;
	unsigned int num_rx_descs = adpt->num_rxdescs;
	int retval;

	/*
	 * real ring DMA buffer
	 * each ring/block may need up to 8 bytes for alignment, hence the
	 * additional bytes tacked onto the end.
	 */
	ring_header->size =
		num_tques * num_tx_descs * sizeof(union alx_hw_tpdesc) +
		num_rques * num_rx_descs * sizeof(union alx_hw_rfdesc) +
		num_rques * num_rx_descs * sizeof(union alx_hw_rrdesc) +
		sizeof(struct coals_msg_block) +
		sizeof(struct alx_hw_stats) +
		num_tques * 8 + num_rques * 2 * 8 + 8 * 2;
	netif_dbg(adpt, ifup, adpt->netdev,
		   "num_tques = %d, num_tx_descs = %d\n",
		   num_tques, num_tx_descs);
	netif_dbg(adpt, ifup, adpt->netdev,
		   "num_rques = %d, num_rx_descs = %d\n",
		   num_rques, num_rx_descs);

	ring_header->used = 0;
	ring_header->desc = dma_alloc_coherent(dev, ring_header->size,
				&ring_header->dma, GFP_KERNEL);

	if (!ring_header->desc) {
		alx_err(adpt, "dma_alloc_coherent failed\n");
		retval = -ENOMEM;
		goto err_alloc_dma;
	}
	memset(ring_header->desc, 0, ring_header->size);
	ring_header->used = ALIGN(ring_header->dma, 8) - ring_header->dma;

	netif_dbg(adpt, ifup, adpt->netdev,
		   "ring header: size = %d, used= %d\n",
		   ring_header->size, ring_header->used);

	/* allocate receive descriptors */
	retval = alx_alloc_all_tx_descriptor(adpt);
	if (retval)
		goto err_alloc_tx;

	/* allocate receive descriptors */
	retval = alx_alloc_all_rx_descriptor(adpt);
	if (retval)
		goto err_alloc_rx;

	/* Init CMB dma address */
	adpt->cmb.dma = ring_header->dma + ring_header->used;
	adpt->cmb.cmb = (u8 *) ring_header->desc + ring_header->used;
	ring_header->used += ALIGN(sizeof(struct coals_msg_block), 8);

	adpt->smb.dma = ring_header->dma + ring_header->used;
	adpt->smb.smb = (u8 *)ring_header->desc + ring_header->used;
	ring_header->used += ALIGN(sizeof(struct alx_hw_stats), 8);

	return 0;

err_alloc_rx:
	alx_free_all_rx_descriptor(adpt);
err_alloc_tx:
	alx_free_all_tx_descriptor(adpt);
err_alloc_dma:
	return retval;
}


/*
 * alx_alloc_all_rtx_descriptor - allocate Tx / RX descriptor queues
 */
static void alx_free_all_rtx_descriptor(struct alx_adapter *adpt)
{
	struct pci_dev *pdev = adpt->pdev;
	struct alx_ring_header *ring_header = &adpt->ring_header;

	alx_free_all_tx_descriptor(adpt);
	alx_free_all_rx_descriptor(adpt);

	adpt->cmb.dma = 0;
	adpt->cmb.cmb = NULL;
	adpt->smb.dma = 0;
	adpt->smb.smb = NULL;

	if (ring_header->desc) {
	pci_free_consistent(pdev, ring_header->size, ring_header->desc,
					ring_header->dma);
	ring_header->desc = NULL;
	}

	ring_header->size = ring_header->used = 0;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,39))
static netdev_features_t alx_fix_features(struct net_device *netdev,
					  netdev_features_t features)
{
	struct alx_adapter *adpt = netdev_priv(netdev);
	/*
	 * Since there is no support for separate rx/tx vlan accel
	 * enable/disable make sure tx flag is always in same state as rx.
	 */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
	if (features & NETIF_F_HW_VLAN_CTAG_RX)
		features |= NETIF_F_HW_VLAN_CTAG_TX;
	else
		features &= ~NETIF_F_HW_VLAN_CTAG_TX;
#else
        if (features & NETIF_F_HW_VLAN_RX)
                features |= NETIF_F_HW_VLAN_TX;
        else
                features &= ~NETIF_F_HW_VLAN_TX;
#endif /*(LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))*/

	if (netdev->mtu > ALX_MAX_TSO_PKT_SIZE ||
	    adpt->hw.mac_type == alx_mac_l1c ||
	    adpt->hw.mac_type == alx_mac_l2c)
		features &= ~(NETIF_F_TSO | NETIF_F_TSO6);

	return features;
}


static int alx_set_features(struct net_device *netdev,
			    netdev_features_t features)
{
	netdev_features_t changed = netdev->features ^ features;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
        if (changed & NETIF_F_HW_VLAN_CTAG_RX)
#else
	if (changed & NETIF_F_HW_VLAN_RX)
#endif /*(LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))*/
		alx_vlan_mode(netdev, features);

	return 0;
}
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,39)) */

/*
 * alx_change_mtu - Change the Maximum Transfer Unit
 */
static int alx_change_mtu(struct net_device *netdev, int new_mtu)
{
	struct alx_adapter *adpt = netdev_priv(netdev);
	int old_mtu   = netdev->mtu;
	int max_frame = new_mtu + ETH_HLEN + ETH_FCS_LEN + VLAN_HLEN;

	if ((max_frame < ALX_MIN_ETH_FRAME_SIZE) ||
	    (max_frame > ALX_MAX_ETH_FRAME_SIZE)) {
		alx_err(adpt, "invalid MTU setting\n");
		return -EINVAL;
	}
	/* set MTU */
	if (old_mtu != new_mtu && netif_running(netdev)) {
		netif_dbg(adpt, hw, adpt->netdev,
			   "changing MTU from %d to %d\n",
			   netdev->mtu, new_mtu);
		netdev->mtu = new_mtu;
		adpt->hw.mtu = new_mtu;
		adpt->rxbuf_size = new_mtu > ALX_DEF_RX_BUF_SIZE ?
				   ALIGN(max_frame, 8) : ALX_DEF_RX_BUF_SIZE;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,39))
		if (new_mtu > (7*1024)) {
			netdev->features &= ~NETIF_F_TSO;
			netdev->features &= ~NETIF_F_TSO6;
		} else {
			netdev->features |= NETIF_F_TSO;
			netdev->features |= NETIF_F_TSO6;
		}
#else
		netdev_update_features(netdev);
#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,39)) */
		alx_reinit_locked(adpt);
	}

	return 0;
}


int alx_open_internal(struct alx_adapter *adpt, u32 ctrl)
{
	struct alx_hw *hw = &adpt->hw;
	int retval = 0;
	int i;

	alx_init_ring_ptrs(adpt);

	alx_set_multicase_list(adpt->netdev);
	alx_restore_vlan(adpt);

	if (hw->cbs.config_mac)
		retval = hw->cbs.config_mac(hw, adpt->rxbuf_size,
				adpt->num_hw_rxques, adpt->num_rxdescs,
				adpt->num_txques, adpt->num_txdescs);

	if (hw->cbs.config_tx)
		retval = hw->cbs.config_tx(hw);

	if (hw->cbs.config_rx)
		retval = hw->cbs.config_rx(hw);

	alx_config_rss(adpt);

	for (i = 0; i < adpt->num_hw_rxques; i++)
		alx_refresh_rx_buffer(adpt->rx_queue[i]);

	/* configure HW regsiters of MSIX */
	if (hw->cbs.config_msix)
		retval = hw->cbs.config_msix(hw, adpt->num_msix_intrs,
					CHK_ADPT_FLAG(0, MSIX_EN),
					CHK_ADPT_FLAG(0, MSI_EN));

	if (ctrl & ALX_OPEN_CTRL_IRQ_EN) {
		retval = alx_request_irq(adpt);
		if (retval)
			goto err_request_irq;
	}

	/* enable NAPI, INTR and TX */
	alx_napi_enable_all(adpt);

	alx_enable_intr(adpt);

	netif_tx_start_all_queues(adpt->netdev);

	CLI_ADPT_FLAG(1, STATE_DOWN);

	/* check link status */
	SET_ADPT_FLAG(0, TASK_LSC_REQ);
	adpt->link_jiffies = jiffies + ALX_TRY_LINK_TIMEOUT;
#ifdef ALX_HIB_TIMER_CONFIG
	mod_timer(&adpt->alx_timer, jiffies);
#endif
	return retval;

err_request_irq:
	alx_clean_all_rx_queues(adpt);
	return retval;
}


void alx_stop_internal(struct alx_adapter *adpt, u32 ctrl)
{
	struct net_device *netdev = adpt->netdev;
	struct alx_hw *hw = &adpt->hw;

	SET_ADPT_FLAG(1, STATE_DOWN);

	netif_tx_stop_all_queues(netdev);
	/* call carrier off first to avoid false dev_watchdog timeouts */
	netif_carrier_off(netdev);
	netif_tx_disable(netdev);

	alx_disable_intr(adpt);

	alx_napi_disable_all(adpt);

	if (ctrl & ALX_OPEN_CTRL_IRQ_EN)
		alx_free_irq(adpt);

	CLI_ADPT_FLAG(0, TASK_LSC_REQ);
	CLI_ADPT_FLAG(0, TASK_REINIT_REQ);
#ifdef ALX_HIB_TIMER_CONFIG
	del_timer_sync(&adpt->alx_timer);
#endif
	if (ctrl & ALX_OPEN_CTRL_RESET_PHY)
		hw->cbs.reset_phy(hw);

	if (ctrl & ALX_OPEN_CTRL_RESET_MAC)
		hw->cbs.reset_mac(hw);

	adpt->hw.link_speed = ALX_LINK_SPEED_UNKNOWN;

	alx_clean_all_tx_queues(adpt);
	alx_clean_all_rx_queues(adpt);
}


/*
 * alx_open - Called when a network interface is made active
 */
static int alx_open(struct net_device *netdev)
{
	struct alx_adapter *adpt = netdev_priv(netdev);
	struct alx_hw *hw = &adpt->hw;
	int retval;

	/* disallow open during test */
	if (CHK_ADPT_FLAG(1, STATE_TESTING) ||
	    CHK_ADPT_FLAG(1, STATE_DIAG_RUNNING))
		return -EBUSY;

	netif_carrier_off(netdev);

	/* allocate rx/tx dma buffer & descriptors */
	retval = alx_alloc_all_rtx_descriptor(adpt);
	if (retval) {
		alx_err(adpt, "error in alx_alloc_all_rtx_descriptor\n");
		goto err_alloc_rtx;
	}

#ifdef  MDM_PLATFORM
	if (ipa_enable) {
		/* Allocate Nodes and List for storing flow control packets*/
		retval = alx_alloc_flow_ctrl_desc(adpt);
		if (retval) {
			alx_err(adpt, "Error in allocating Flow Control Buffers \n");
			goto err_alloc_flow_ctrl;
		}
		pr_info("%s -- %d Flow Control Buffer Allocated \n",
						__func__, adpt->freeq_cnt);
	}
#endif

	retval = alx_open_internal(adpt, ALX_OPEN_CTRL_IRQ_EN);
	if (retval)
		goto err_open_internal;

	return retval;

err_open_internal:
	alx_stop_internal(adpt, ALX_OPEN_CTRL_IRQ_EN);
#ifdef MDM_PLATFORM
err_alloc_flow_ctrl:
	if (ipa_enable)
		alx_free_flow_ctrl_desc(adpt);
#endif
err_alloc_rtx:
	alx_free_all_rtx_descriptor(adpt);
	hw->cbs.reset_mac(hw);
	return retval;
}


/*
 * alx_stop - Disables a network interface
 */
static int alx_stop(struct net_device *netdev)
{
	struct alx_adapter *adpt = netdev_priv(netdev);
	struct alx_ipa_rx_desc_node *node = NULL;

	if (CHK_ADPT_FLAG(1, STATE_RESETTING))
		netif_warn(adpt, ifdown, adpt->netdev,
			   "flag STATE_RESETTING has already set\n");

	alx_stop_internal(adpt, ALX_OPEN_CTRL_IRQ_EN |
				ALX_OPEN_CTRL_RESET_MAC);
	alx_free_all_rtx_descriptor(adpt);
#ifdef  MDM_PLATFORM
	if (ipa_enable) {
		/* Flush any pending packets */
		pr_info("ALX - Flush %d Pending Packets \n", adpt->pendq_cnt);
		spin_lock_bh(&adpt->flow_ctrl_lock);
		while (adpt->pendq_cnt) {
			node = list_first_entry(&adpt->pend_queue_head,
				struct alx_ipa_rx_desc_node, link);
			list_del(&node->link);
			list_add_tail(&node->link, &adpt->free_queue_head);
			adpt->pendq_cnt--;
			adpt->freeq_cnt++;
		}
		spin_unlock_bh(&adpt->flow_ctrl_lock);
		if ((adpt->freeq_cnt != ALX_IPA_SYS_PIPE_DNE_PKTS) ||
			(adpt->pendq_cnt != 0)) {
			pr_err("%s -- Memory leak detected freeq_cnt %d, pendq_cnt %d",
				__func__, adpt->freeq_cnt, adpt->pendq_cnt);
			BUG();
		}
		alx_free_flow_ctrl_desc(adpt);
	}
#endif
	return 0;
}


static int alx_shutdown_internal(struct pci_dev *pdev, bool *wakeup)
{
	struct alx_adapter *adpt = pci_get_drvdata(pdev);
	struct net_device *netdev = adpt->netdev;
	struct alx_hw *hw = &adpt->hw;
	u32 wufc = adpt->wol;
	u16 lpa;
	u32 speed, adv_speed, misc;
	bool link_up;
	int i;
	int retval = 0;

	hw->cbs.config_aspm(hw, false, false);

	netif_device_detach(netdev);
	if (netif_running(netdev))
		alx_stop_internal(adpt, 0);

#ifdef CONFIG_PM_SLEEP
	retval = pci_save_state(pdev);
	if (retval)
		return retval;
#endif
	hw->cbs.check_phy_link(hw, &speed, &link_up);

	if (link_up) {
		if (hw->mac_type == alx_mac_l1f ||
		    hw->mac_type == alx_mac_l2f) {
			alx_mem_r32(hw, ALX_MISC, &misc);
			misc |= ALX_MISC_INTNLOSC_OPEN;
			alx_mem_w32(hw, ALX_MISC, misc);
		}

		retval = hw->cbs.read_phy_reg(hw, MII_LPA, &lpa);
		if (retval)
			return retval;

		adv_speed = ALX_LINK_SPEED_10_HALF;
		if (lpa & LPA_10FULL)
			adv_speed = ALX_LINK_SPEED_10_FULL;
		else if (lpa & LPA_10HALF)
			adv_speed = ALX_LINK_SPEED_10_HALF;
		else if (lpa & LPA_100FULL)
			adv_speed = ALX_LINK_SPEED_100_FULL;
		else if (lpa & LPA_100HALF)
			adv_speed = ALX_LINK_SPEED_100_HALF;

		retval = hw->cbs.setup_phy_link(hw, adv_speed, true,
				!hw->disable_fc_autoneg);
		if (retval)
			return retval;

		for (i = 0; i < ALX_MAX_SETUP_LNK_CYCLE; i++) {
			mdelay(100);
			retval = hw->cbs.check_phy_link(hw, &speed, &link_up);
			if (retval)
				continue;
			if (link_up)
				break;
		}
	} else {
		speed = ALX_LINK_SPEED_10_HALF;
		link_up = false;
		/* When link is  disabled and PHY/MAC not programmed at all
		* Due to a suspected bug in HW, we dont get PHY UP interrupt
		* As a solution, program the MAC/PHY with 10Mbps HD link speed
		* even the there is no LINK Detected */
		retval = hw->cbs.setup_phy_link(hw, speed, true,
				!hw->disable_fc_autoneg);
		if (retval)
			return retval;
	}
	hw->link_speed = speed;
	hw->link_up = link_up;

	retval = hw->cbs.config_wol(hw, wufc);
	if (retval)
		return retval;

	/* clear phy interrupt */
	retval = hw->cbs.ack_phy_intr(hw);
	if (retval)
		return retval;

	if (wufc) {
		/* pcie patch */
		device_set_wakeup_enable(&pdev->dev, 1);
	}

	retval = hw->cbs.config_pow_save(hw, adpt->hw.link_speed,
			(wufc ? true : false), false,
			(wufc & ALX_WOL_MAGIC ? true : false), true);
	if (retval)
		return retval;

	*wakeup = wufc ? true : false;
	pci_clear_master(pdev);
	return 0;
}


static void alx_shutdown(struct pci_dev *pdev)
{
	bool wakeup;
	alx_shutdown_internal(pdev, &wakeup);

	pci_wake_from_d3(pdev, wakeup);
	pci_set_power_state(pdev, PCI_D3hot);
}


#ifdef CONFIG_PM_SLEEP

static int alx_suspend(struct device *dev)
{
	struct pci_dev *pdev = to_pci_dev(dev);
	struct alx_adapter *adpt = pci_get_drvdata(pdev);
	int retval;
	bool wakeup;

	if (!pdev || !adpt) {
		pr_err("%s NULL Pointers pdev %p adpt %p",__func__,pdev,adpt);
		return -1;
	}

	retval = alx_shutdown_internal(pdev, &wakeup);
	if (retval)
		return retval;

#ifndef APQ_PLATFORM
	if (ipa_enable) {
		if(alx_ipa_rm_try_release(adpt))
			pr_err("%s -- ODU PROD Release unsuccessful \n",__func__);
	}
#endif

	if (wakeup) {
		pci_prepare_to_sleep(pdev);
	} else {
		pci_wake_from_d3(pdev, false);
		pci_set_power_state(pdev, PCI_D3hot);
	}

	return 0;
}


static int alx_resume(struct device *dev)
{
	struct pci_dev *pdev = to_pci_dev(dev);
	struct alx_adapter *adpt = pci_get_drvdata(pdev);
	struct net_device *netdev = adpt->netdev;
	struct alx_hw *hw = &adpt->hw;
	u32 retval;

	pci_set_power_state(pdev, PCI_D0);
	pci_restore_state(pdev);
	/*
	 * pci_restore_state clears dev->state_saved so call
	 * pci_save_state to restore it.
	 */
	pci_save_state(pdev);
	pci_enable_pcie_error_reporting(pdev);
	pci_set_master(pdev);
	pci_enable_wake(pdev, PCI_D3hot, 0);
	pci_enable_wake(pdev, PCI_D3cold, 0);

	retval = hw->cbs.reset_pcie(hw, true, true);
	retval = hw->cbs.reset_phy(hw);
	retval = hw->cbs.reset_mac(hw);
	retval = hw->cbs.setup_phy_link(hw, hw->autoneg_advertised, true,
			!hw->disable_fc_autoneg);

	retval = hw->cbs.config_wol(hw, 0);

	if (netif_running(netdev)) {
		retval = alx_open_internal(adpt, 0);
		if (retval)
			return retval;
	}


	netif_device_attach(netdev);
	/* Hold the wake lock for 5sec to ensure any traffic*/
	pm_wakeup_event(dev, 5000);
	return 0;
}
#endif


/*
 * alx_update_hw_stats - Update the board statistics counters.
 */
void alx_update_hw_stats(struct alx_adapter *adpt)
{
	struct net_device_stats *net_stats;
	struct alx_hw *hw = &adpt->hw;
	struct alx_hw_stats *hwstats = &adpt->hw_stats;
	unsigned long *hwstat_item = NULL;
	u32 hwstat_reg;
	u32 hwstat_data;

	if (CHK_ADPT_FLAG(1, STATE_DOWN) || CHK_ADPT_FLAG(1, STATE_RESETTING))
		return;

	/* update RX status */
	hwstat_reg  = hw->rxstat_reg;
	hwstat_item = &hwstats->rx_ok;
	while (hwstat_reg < hw->rxstat_reg + hw->rxstat_sz) {
		alx_mem_r32(hw, hwstat_reg, &hwstat_data);
		*hwstat_item += hwstat_data;
		hwstat_reg += 4;
		hwstat_item++;
	}

	/* update TX status */
	hwstat_reg  = hw->txstat_reg;
	hwstat_item = &hwstats->tx_ok;
	while (hwstat_reg < hw->txstat_reg + hw->txstat_sz) {
		alx_mem_r32(hw, hwstat_reg, &hwstat_data);
		*hwstat_item += hwstat_data;
		hwstat_reg += 4;
		hwstat_item++;
	}

	net_stats = &adpt->netdev->stats;
	net_stats->rx_packets = hwstats->rx_ok;
	net_stats->tx_packets = hwstats->tx_ok;
	net_stats->rx_bytes   = hwstats->rx_byte_cnt;
	net_stats->tx_bytes   = hwstats->tx_byte_cnt;
	net_stats->multicast  = hwstats->rx_mcast;
	net_stats->collisions = hwstats->tx_single_col +
		hwstats->tx_multi_col * 2 +
		hwstats->tx_late_col + hwstats->tx_abort_col;

	net_stats->rx_errors  = hwstats->rx_frag + hwstats->rx_fcs_err +
		hwstats->rx_len_err + hwstats->rx_ov_sz +
		hwstats->rx_ov_rrd + hwstats->rx_align_err;

	net_stats->rx_fifo_errors   = hwstats->rx_ov_rxf;
	net_stats->rx_length_errors = hwstats->rx_len_err;
	net_stats->rx_crc_errors    = hwstats->rx_fcs_err;
	net_stats->rx_frame_errors  = hwstats->rx_align_err;
	net_stats->rx_over_errors   = hwstats->rx_ov_rrd + hwstats->rx_ov_rxf;

	net_stats->rx_missed_errors = hwstats->rx_ov_rrd + hwstats->rx_ov_rxf;

	net_stats->tx_errors = hwstats->tx_late_col + hwstats->tx_abort_col +
		hwstats->tx_underrun + hwstats->tx_trunc;
	net_stats->tx_fifo_errors    = hwstats->tx_underrun;
	net_stats->tx_aborted_errors = hwstats->tx_abort_col;
	net_stats->tx_window_errors  = hwstats->tx_late_col;
}


/*
 * alx_get_stats - Get System Network Statistics
 *
 * Returns the address of the device statistics structure.
 * The statistics are actually updated from the timer callback.
 */
static struct net_device_stats *alx_get_stats(struct net_device *netdev)
{
	struct alx_adapter *adpt = netdev_priv(netdev);

	alx_update_hw_stats(adpt);
	return &netdev->stats;
}

/* Resize the descriptor rings */
int alx_resize_rings(struct net_device *netdev)
{
	/* close and then re-open interface */
	alx_stop(netdev);
	return alx_open(netdev);
}


#ifdef ALX_LINK_DOWN_CONFIG
static int alx_link_mac_restore(struct alx_adapter *adpt)
{
	struct alx_hw *hw = &adpt->hw;
	int retval = 0;
	int i;

	printk(KERN_INFO "alx: into NEW alx_link_mac_restore\n");
	alx_init_ring_ptrs(adpt);

	alx_set_multicase_list(adpt->netdev);
	alx_restore_vlan(adpt);

	if (hw->cbs.config_mac)
		retval = hw->cbs.config_mac(hw, adpt->rxbuf_size,
				adpt->num_hw_rxques, adpt->num_rxdescs,
				adpt->num_txques, adpt->num_txdescs);

	if (hw->cbs.config_tx)
		retval = hw->cbs.config_tx(hw);

	if (hw->cbs.config_rx)
		retval = hw->cbs.config_rx(hw);

	alx_config_rss(adpt);

	for (i = 0; i < adpt->num_hw_rxques; i++)
		alx_refresh_rx_buffer(adpt->rx_queue[i]);

	/* configure HW regsiters of MSIX */
	if (hw->cbs.config_msix)
		retval = hw->cbs.config_msix(hw, adpt->num_msix_intrs,
					CHK_ADPT_FLAG(0, MSIX_EN),
					CHK_ADPT_FLAG(0, MSI_EN));

	return retval;
}
#endif

#ifndef APQ_PLATFORM
static int alx_ipa_set_perf_level(void)
{
	struct ipa_rm_perf_profile profile;
	struct alx_ipa_ctx *alx_ipa = galx_adapter_ptr->palx_ipa;
	int ret = 0;

	if (!alx_ipa) {
		pr_err("%s alx_ipa cts NULL ctx:%p\n",__func__,alx_ipa);
		return -1;
	}

	memset(&profile, 0, sizeof(profile));
	profile.max_supported_bandwidth_mbps = MAX_AR8151_BW;

	ret = ipa_rm_set_perf_profile (IPA_RM_RESOURCE_ODU_ADAPT_PROD,
					&profile);
	if (ret) {
		pr_err("Err to set BW: IPA_RM_RESOURCE_ODU_ADAPT_PROD err:%d\n",
			ret);
		return ret;
	}

	ret = ipa_rm_set_perf_profile (IPA_RM_RESOURCE_ODU_ADAPT_CONS,
					&profile);
	if (ret) {
		pr_err("Err to set BW: IPA_RM_RESOURCE_ODU_ADAPT_CONS err:%d\n",
			ret);
		return ret;
	}

	alx_ipa->alx_ipa_perf_requested = true;
	return ret;
}
#endif

static void alx_link_task_routine(struct alx_adapter *adpt)
{
	struct net_device *netdev = adpt->netdev;
	struct alx_hw *hw = &adpt->hw;
	char *link_desc;
	int ret = 0;
#ifndef APQ_PLATFORM
	struct alx_ipa_ctx *alx_ipa = adpt->palx_ipa;
#endif

	if (!CHK_ADPT_FLAG(0, TASK_LSC_REQ))
		return;
	CLI_ADPT_FLAG(0, TASK_LSC_REQ);

	if (CHK_ADPT_FLAG(1, STATE_DOWN))
		return;

	if (hw->cbs.check_phy_link) {
		hw->cbs.check_phy_link(hw,
			&hw->link_speed, &hw->link_up);
	} else {
		/* always assume link is up, if no check link function */
		hw->link_speed = ALX_LINK_SPEED_1GB_FULL;
		hw->link_up = true;
	}
	netif_dbg(adpt, timer, adpt->netdev,
		   "link_speed = %d, link_up = %d\n",
		   hw->link_speed, hw->link_up);

	if (!hw->link_up && time_after(adpt->link_jiffies, jiffies))
		SET_ADPT_FLAG(0, TASK_LSC_REQ);

	if (hw->link_up) {
		if (netif_carrier_ok(netdev))
			return;

		link_desc = (hw->link_speed == ALX_LINK_SPEED_1GB_FULL) ?
			"1 Gbps Duplex Full" :
			(hw->link_speed == ALX_LINK_SPEED_100_FULL ?
			 "100 Mbps Duplex Full" :
			 (hw->link_speed == ALX_LINK_SPEED_100_HALF ?
			  "100 Mbps Duplex Half" :
			  (hw->link_speed == ALX_LINK_SPEED_10_FULL ?
			   "10 Mbps Duplex Full" :
			   (hw->link_speed == ALX_LINK_SPEED_10_HALF ?
			    "10 Mbps Duplex HALF" :
			    "unknown speed"))));
		netif_dbg(adpt, timer, adpt->netdev,
			   "NIC Link is Up %s\n", link_desc);

		hw->cbs.config_aspm(hw, true, true);
		hw->cbs.start_mac(hw);
		netif_carrier_on(netdev);
		netif_tx_wake_all_queues(netdev);

#ifdef MDM_PLATFORM
		if (ipa_enable) {
			/* Enable ODU Bridge */
			if (alx_ipa->ipa_ready == true && CHK_ADPT_FLAG(2, ODU_INIT)) {
				ret = odu_bridge_connect();
				if (ret)
					pr_err("Could not connect to ODU bridge %d \n",
						ret);
				else
					SET_ADPT_FLAG(2, ODU_CONNECT);
				/* Request for IPA Resources */
				spin_lock_bh(&alx_ipa->ipa_rm_state_lock);
				if (alx_ipa->ipa_prod_rm_state == ALX_IPA_RM_RELEASED) {
					spin_unlock_bh(&alx_ipa->ipa_rm_state_lock);
					alx_ipa_rm_request(adpt);
				} else {
					spin_unlock_bh(&alx_ipa->ipa_rm_state_lock);
				}
			}
		}
#endif
	} else {
		/* only continue if link was up previously */
		if (!netif_carrier_ok(netdev))
			return;

		hw->link_speed = 0;
		netif_dbg(adpt, timer, adpt->netdev, "NIC Link is Down\n");
		netif_carrier_off(netdev);
		netif_tx_stop_all_queues(netdev);

#ifdef ALX_LINK_DOWN_CONFIG
		hw->cbs.reset_mac(hw);
#else
		hw->cbs.stop_mac(hw);
#endif
		hw->cbs.config_aspm(hw, false, true);
		hw->cbs.setup_phy_link(hw, hw->autoneg_advertised, true,
				!hw->disable_fc_autoneg);
#ifdef ALX_LINK_DOWN_CONFIG
		alx_link_mac_restore(adpt);
#endif
#ifdef MDM_PLATFORM
		if (ipa_enable) {
			/* Disable ODU Bridge */
			ret = odu_bridge_disconnect();
			if (ret) {
				pr_err("Could not connect to ODU bridge %d \n", ret);
			} else {
				CLI_ADPT_FLAG(2, ODU_CONNECT);
				adpt->palx_ipa->alx_ipa_perf_requested = false;
				if(alx_ipa_rm_try_release(adpt))
					pr_err("%s -- ODU PROD Release unsuccessful \n", __func__);
			}
		}
#endif
	}
}


static void alx_reinit_task_routine(struct alx_adapter *adpt)
{
	if (!CHK_ADPT_FLAG(0, TASK_REINIT_REQ))
		return;
	CLI_ADPT_FLAG(0, TASK_REINIT_REQ);

	if (CHK_ADPT_FLAG(1, STATE_DOWN) || CHK_ADPT_FLAG(1, STATE_RESETTING))
		return;

	alx_reinit_locked(adpt);
}


/*
 * alx_timer_routine - Timer Call-back
 */
static void alx_timer_routine(unsigned long data)
{
	struct alx_adapter *adpt = (struct alx_adapter *)data;
	unsigned long delay;

#ifdef ALX_HIB_TASK_CONFIG
	struct alx_hw *hw = &adpt->hw;
	if (hw->bHibBug)
		hw->cbs.apply_phy_hib_patch(hw);
#endif
	/* poll faster when waiting for link */
	if (CHK_ADPT_FLAG(0, TASK_LSC_REQ))
		delay = HZ / 10;
	else
		delay = HZ * 2;

	/* Reset the timer */
	mod_timer(&adpt->alx_timer, delay + jiffies);

	alx_task_schedule(adpt);
}


/*
 * alx_task_routine - manages and runs subtasks
 */
static void alx_task_routine(struct work_struct *work)
{
	struct alx_adapter *adpt = container_of(work,
				struct alx_adapter, alx_task);
	/* test state of adapter */
	if (!CHK_ADPT_FLAG(1, STATE_WATCH_DOG))
		netif_warn(adpt, timer, adpt->netdev,
			   "flag STATE_WATCH_DOG doesn't set\n");

	/* reinit task */
	alx_reinit_task_routine(adpt);

	/* link task */
	alx_link_task_routine(adpt);

	/* flush memory to make sure state is correct before next watchog */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,39))
	smp_mb();
#else
	smp_mb__before_clear_bit();
#endif

	CLI_ADPT_FLAG(1, STATE_WATCH_DOG);
}

#ifndef APQ_PLATFORM
/*
 * alx_ipa_send_routine - Sends packets to IPA/ODU bridge Driver
 * Scheduled on RX of IPA_WRITE_DONE Event
 */
static void alx_ipa_send_routine(struct work_struct *work)
{
	struct alx_adapter *adpt = container_of(work,
				struct alx_adapter, ipa_send_task);
	struct alx_ipa_rx_desc_node *node = NULL;
	struct ipa_tx_meta ipa_meta = {0x0};
	int ret =0;
	struct alx_ipa_ctx *alx_ipa = adpt->palx_ipa;

	/* Set the Perf level when the Request is granted */
	if (!alx_ipa->alx_ipa_perf_requested) {
		spin_lock_bh(&alx_ipa->ipa_rm_state_lock);
		if (alx_ipa->ipa_prod_rm_state == ALX_IPA_RM_GRANTED) {
			spin_unlock_bh(&alx_ipa->ipa_rm_state_lock);
			alx_ipa_set_perf_level();
			alx_ipa->alx_ipa_perf_requested = true;
		} else {
			spin_unlock_bh(&alx_ipa->ipa_rm_state_lock);
		}
	}

	/* Send all pending packets to IPA.
          Compute the number of desc left for HW and send packets accordingly*/
	spin_lock_bh(&adpt->flow_ctrl_lock);
	CLI_ADPT_FLAG(2, WQ_SCHED);
	if (unlikely(!adpt->pendq_cnt)) {
		pr_err("%s - Error no pending packets in Queue %d\n",
						__func__, adpt->pendq_cnt);
		spin_unlock_bh(&adpt->flow_ctrl_lock);
		return;
	}
	if (adpt->ipa_free_desc_cnt < adpt->ipa_low_watermark) {
		adpt->palx_ipa->stats.ipa_low_watermark_cnt++;
		spin_unlock_bh(&adpt->flow_ctrl_lock);
		return;
	}

        spin_lock_bh(&alx_ipa->ipa_rm_state_lock);
	if (alx_ipa->ipa_prod_rm_state != ALX_IPA_RM_GRANTED) {
          spin_unlock_bh(&alx_ipa->ipa_rm_state_lock);
          if (adpt->pendq_cnt > 0 && alx_ipa_rm_request(adpt) != 0 ) {
	    if(printk_ratelimit())
	      pr_info("%s IPA RM resource not granted,return \n", __func__);
            spin_unlock_bh(&adpt->flow_ctrl_lock);
            return;
          }
        }
        else
          spin_unlock_bh(&alx_ipa->ipa_rm_state_lock);

	while (adpt->ipa_free_desc_cnt && adpt->pendq_cnt) {
		node = list_first_entry(&adpt->pend_queue_head,
				struct alx_ipa_rx_desc_node, link);
		list_del(&node->link);
		list_add_tail(&node->link, &adpt->free_queue_head);
		adpt->freeq_cnt++;
		adpt->pendq_cnt--;
		ipa_meta.dma_address_valid = false;
		/* Send Packet to ODU bridge Driver */
		ret = odu_bridge_tx_dp(node->skb_ptr, &ipa_meta);
		if (ret) {
			pr_err("odu_bridge_tx_dp() Failed in %s!!"
				" ret %d--Free SKB\n", __func__, ret);
			kfree(node->skb_ptr);
			adpt->palx_ipa->stats.rx_ipa_send_fail++;
		} else {
			adpt->palx_ipa->stats.rx_ipa_send++;
			adpt->ipa_free_desc_cnt--;
			/* Increment the ipa_rx_completion Counter */
			spin_lock(&alx_ipa->rm_ipa_lock);
			if (alx_ipa->acquire_wake_src == false) {
				__pm_stay_awake(&alx_ipa->rm_ipa_wait);
				alx_ipa->acquire_wake_src = true;
			}
			alx_ipa->ipa_rx_completion++;
			spin_unlock(&alx_ipa->rm_ipa_lock);
		}
	}
	/* Release PROD if we dont have any more data to send*/
	spin_unlock_bh(&adpt->flow_ctrl_lock);
}
#endif

/* Calculate the transmit packet descript needed*/
static bool alx_check_num_tpdescs(struct alx_tx_queue *txque,
				  const struct sk_buff *skb)
{
	u16 num_required = 1;
	u16 num_available = 0;
	u16 produce_idx = txque->tpq.produce_idx;
	u16 consume_idx = txque->tpq.consume_idx;
	int i = 0;

	u16 proto_hdr_len = 0;
	if (skb_is_gso(skb)) {
		proto_hdr_len = skb_transport_offset(skb) + tcp_hdrlen(skb);
		if (proto_hdr_len < skb_headlen(skb))
			num_required++;
		if (skb_shinfo(skb)->gso_type & SKB_GSO_TCPV6)
			num_required++;
	}
	for (i = 0; i < skb_shinfo(skb)->nr_frags; i++)
		num_required++;
	num_available = (u16)(consume_idx > produce_idx) ?
		(consume_idx - produce_idx - 1) :
		(txque->tpq.count + consume_idx - produce_idx - 1);

	return num_required < num_available;
}


static int alx_tso_csum(struct alx_adapter *adpt,
			struct alx_tx_queue *txque,
			struct sk_buff *skb,
			union alx_sw_tpdesc *stpd)
{
	struct pci_dev *pdev = adpt->pdev;
	u8  hdr_len;
	int retval;

	if (skb_is_gso(skb)) {
		if (skb_header_cloned(skb)) {
			retval = pskb_expand_head(skb, 0, 0, GFP_ATOMIC);
			if (unlikely(retval))
				return retval;
		}

		if (skb->protocol == htons(ETH_P_IP)) {
			u32 pkt_len =
				((unsigned char *)ip_hdr(skb) - skb->data) +
				ntohs(ip_hdr(skb)->tot_len);
			if (skb->len > pkt_len)
				pskb_trim(skb, pkt_len);
		}

		hdr_len = skb_transport_offset(skb) + tcp_hdrlen(skb);
		if (unlikely(skb->len == hdr_len)) {
			/* we only need to do csum */
			dev_warn(&pdev->dev,
				 "tso doesn't need, if packet with 0 data\n");
			goto do_csum;
		}

		if (skb_shinfo(skb)->gso_type & SKB_GSO_TCPV4) {
			ip_hdr(skb)->check = 0;
			tcp_hdr(skb)->check = ~csum_tcpudp_magic(
						ip_hdr(skb)->saddr,
						ip_hdr(skb)->daddr,
						0, IPPROTO_TCP, 0);
			stpd->genr.ipv4 = 1;
		}

		if (skb_shinfo(skb)->gso_type & SKB_GSO_TCPV6) {
			/* ipv6 tso need an extra tpd */
			union alx_sw_tpdesc extra_tpd;

			memset(stpd, 0, sizeof(union alx_sw_tpdesc));
			memset(&extra_tpd, 0, sizeof(union alx_sw_tpdesc));

			ipv6_hdr(skb)->payload_len = 0;
			tcp_hdr(skb)->check = ~csum_ipv6_magic(
						&ipv6_hdr(skb)->saddr,
						&ipv6_hdr(skb)->daddr,
						0, IPPROTO_TCP, 0);
			extra_tpd.tso.pkt_len = skb->len;
			extra_tpd.tso.lso = 0x1;
			extra_tpd.tso.lso_v2 = 0x1;
			alx_set_tpdesc(txque, &extra_tpd);
			stpd->tso.lso_v2 = 0x1;
		}

		stpd->tso.lso = 0x1;
		stpd->tso.tcphdr_offset = skb_transport_offset(skb);
		stpd->tso.mss = skb_shinfo(skb)->gso_size;
		return 0;
	}

do_csum:
	if (likely(skb->ip_summed == CHECKSUM_PARTIAL)) {
		u8 css, cso;
		cso = skb_checksum_start_offset(skb);

		if (unlikely(cso & 0x1)) {
			dev_err(&pdev->dev, "pay load offset should not be an "
				"event number\n");
			return -1;
		} else {
			css = cso + skb->csum_offset;

			stpd->csum.payld_offset = cso >> 1;
			stpd->csum.cxsum_offset = css >> 1;
			stpd->csum.c_csum = 0x1;
		}
	}
	return 0;
}


static void alx_tx_map(struct alx_adapter *adpt,
		       struct alx_tx_queue *txque,
		       struct sk_buff *skb,
		       union alx_sw_tpdesc *stpd)
{
	struct alx_buffer *tpbuf = NULL;

	unsigned int nr_frags = skb_shinfo(skb)->nr_frags;

	unsigned int len = skb_headlen(skb);

	u16 map_len = 0;
	u16 mapped_len = 0;
	u16 hdr_len = 0;
	u16 f;
	u32 tso = stpd->tso.lso;

	if (tso) {
		/* TSO */
		map_len = hdr_len = skb_transport_offset(skb) + tcp_hdrlen(skb);

		tpbuf = GET_TP_BUFFER(txque, txque->tpq.produce_idx);
		tpbuf->length = map_len;
		tpbuf->dma = dma_map_single(txque->dev,
					skb->data, hdr_len, DMA_TO_DEVICE);
		mapped_len += map_len;
		stpd->genr.addr = tpbuf->dma;
		stpd->genr.buffer_len = tpbuf->length;

		alx_set_tpdesc(txque, stpd);
	}

	if (mapped_len < len) {
		tpbuf = GET_TP_BUFFER(txque, txque->tpq.produce_idx);
		tpbuf->length = len - mapped_len;
		tpbuf->dma =
			dma_map_single(txque->dev, skb->data + mapped_len,
					tpbuf->length, DMA_TO_DEVICE);
		stpd->genr.addr = tpbuf->dma;
		stpd->genr.buffer_len  = tpbuf->length;
		alx_set_tpdesc(txque, stpd);
	}

	for (f = 0; f < nr_frags; f++) {
		struct skb_frag_struct *frag;

		frag = &skb_shinfo(skb)->frags[f];

		tpbuf = GET_TP_BUFFER(txque, txque->tpq.produce_idx);
		tpbuf->length = skb_frag_size(frag);
		tpbuf->dma = skb_frag_dma_map(txque->dev, frag, 0,
					      tpbuf->length, DMA_TO_DEVICE);
		stpd->genr.addr = tpbuf->dma;
		stpd->genr.buffer_len  = tpbuf->length;
		alx_set_tpdesc(txque, stpd);
	}


	/* The last tpd */
	alx_set_tpdesc_lastfrag(txque);
	/*
	 * The last buffer info contain the skb address,
	 * so it will be free after unmap
	 */
	if (tpbuf)
		tpbuf->skb = skb;
}


static netdev_tx_t alx_start_xmit_frame(struct alx_adapter *adpt,
					struct alx_tx_queue *txque,
					struct sk_buff *skb)
{
	struct alx_hw     *hw = &adpt->hw;
	unsigned long     flags = 0;
	union alx_sw_tpdesc stpd; /* normal*/
	struct alx_ipa_ctx *alx_ipa;
#ifdef MDM_PLATFORM
	if (ipa_enable)
		alx_ipa = adpt->palx_ipa;
#endif

	if (CHK_ADPT_FLAG(1, STATE_DOWN) ||
	    CHK_ADPT_FLAG(1, STATE_DIAG_RUNNING)) {
		dev_kfree_skb_any(skb);
		return NETDEV_TX_OK;
	}

	if (!spin_trylock_irqsave(&adpt->tx_lock, flags)) {
		alx_err(adpt, "tx locked!\n");
		return NETDEV_TX_LOCKED;
	}

	if (!alx_check_num_tpdescs(txque, skb)) {
		/* no enough descriptor, just stop queue */
		netif_stop_queue(adpt->netdev);
		spin_unlock_irqrestore(&adpt->tx_lock, flags);
		alx_err(adpt, "No TX Desc to send packet\n");
		return NETDEV_TX_BUSY;
	}

	memset(&stpd, 0, sizeof(union alx_sw_tpdesc));
	/* do TSO and check sum */
	if (alx_tso_csum(adpt, txque, skb, &stpd) != 0) {
		spin_unlock_irqrestore(&adpt->tx_lock, flags);
		dev_kfree_skb_any(skb);
		return NETDEV_TX_OK;
	}
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
	if (unlikely(skb_vlan_tag_present(skb))) {
		u16 vlan = skb_vlan_tag_get(skb);
#else
	if (unlikely(vlan_tx_tag_present(skb))) {
		u16 vlan = vlan_tx_tag_get(skb);
#endif
		u16 tag;
		ALX_VLAN_TO_TAG(vlan, tag);
		stpd.genr.vlan_tag = tag;
		stpd.genr.instag = 0x1;
	}
	if (skb_network_offset(skb) != ETH_HLEN)
		stpd.genr.type = 0x1; /* Ethernet frame */

	alx_tx_map(adpt, txque, skb, &stpd);


	/* update produce idx */
	wmb();
	alx_mem_w16(hw, txque->produce_reg, txque->tpq.produce_idx);
	netif_dbg(adpt, tx_err, adpt->netdev,
		   "TX[%d]: tpq.consume_idx = 0x%x, tpq.produce_idx = 0x%x\n",
		   txque->que_idx, txque->tpq.consume_idx,
		   txque->tpq.produce_idx);
	netif_dbg(adpt, tx_err, adpt->netdev,
		   "TX[%d]: Produce Reg[%x] = 0x%x\n",
		   txque->que_idx, txque->produce_reg, txque->tpq.produce_idx);

	spin_unlock_irqrestore(&adpt->tx_lock, flags);

#ifdef MDM_PLATFORM
	if (ipa_enable) {
		/* Hold on to the wake lock for TX Completion Event */
		spin_lock_bh(&alx_ipa->rm_ipa_lock);
		if (alx_ipa->acquire_wake_src == false) {
			 __pm_stay_awake(&alx_ipa->rm_ipa_wait);
			alx_ipa->acquire_wake_src = true;
		}
		alx_ipa->alx_tx_completion++;
		spin_unlock_bh(&alx_ipa->rm_ipa_lock);
	}
#endif

	return NETDEV_TX_OK;
}


static netdev_tx_t alx_start_xmit(struct sk_buff *skb,
				  struct net_device *netdev)
{
	struct alx_adapter *adpt = netdev_priv(netdev);
	struct alx_tx_queue *txque;

	txque = adpt->tx_queue[0];
	return alx_start_xmit_frame(adpt, txque, skb);
}


/*
 * alx_mii_ioctl
 */
static int alx_mii_ioctl(struct net_device *netdev,
			 struct ifreq *ifr, int cmd)
{
	struct alx_adapter *adpt = netdev_priv(netdev);
	struct alx_hw *hw = &adpt->hw;
	struct mii_ioctl_data *data = if_mii(ifr);
	int retval = 0;

	if (!netif_running(netdev))
		return -EINVAL;

	switch (cmd) {
	case SIOCGMIIPHY:
		data->phy_id = 0;
		break;

	case SIOCGMIIREG:
		if (data->reg_num & ~(0x1F)) {
			retval = -EFAULT;
			goto out;
		}

		retval = hw->cbs.read_phy_reg(hw, data->reg_num,
					      &data->val_out);
		netif_dbg(adpt, hw, adpt->netdev, "read phy %02x %04x\n",
			  data->reg_num, data->val_out);
		if (retval) {
			retval = -EIO;
			goto out;
		}
		break;

	case SIOCSMIIREG:
		if (data->reg_num & ~(0x1F)) {
			retval = -EFAULT;
			goto out;
		}

		retval = hw->cbs.write_phy_reg(hw, data->reg_num, data->val_in);
		netif_dbg(adpt, hw, adpt->netdev, "write phy %02x %04x\n",
			  data->reg_num, data->val_in);
		if (retval) {
			retval = -EIO;
			goto out;
		}
		break;
	default:
		retval = -EOPNOTSUPP;
		break;
	}
out:
	return retval;

}


static int alx_ioctl(struct net_device *netdev, struct ifreq *ifr, int cmd)
{
	switch (cmd) {
	case SIOCGMIIPHY:
	case SIOCGMIIREG:
	case SIOCSMIIREG:
		return alx_mii_ioctl(netdev, ifr, cmd);
	default:
		return -EOPNOTSUPP;
	}
}


#ifdef CONFIG_NET_POLL_CONTROLLER
static void alx_poll_controller(struct net_device *netdev)
{
	struct alx_adapter *adpt = netdev_priv(netdev);
	int num_msix_intrs = adpt->num_msix_intrs;
	int msix_idx;

	/* if interface is down do nothing */
	if (CHK_ADPT_FLAG(1, STATE_DOWN))
		return;

	if (CHK_ADPT_FLAG(0, MSIX_EN)) {
		for (msix_idx = 0; msix_idx < num_msix_intrs; msix_idx++) {
			struct alx_msix_param *msix = adpt->msix[msix_idx];
			if (CHK_MSIX_FLAG(RXS) || CHK_MSIX_FLAG(TXS))
				alx_msix_rtx(0, msix);
			else if (CHK_MSIX_FLAG(TIMER))
				alx_msix_timer(0, msix);
			else if (CHK_MSIX_FLAG(ALERT))
				alx_msix_alert(0, msix);
			else if (CHK_MSIX_FLAG(SMB))
				alx_msix_smb(0, msix);
			else if (CHK_MSIX_FLAG(PHY))
				alx_msix_phy(0, msix);
		}
	} else {
		alx_interrupt(adpt->pdev->irq, netdev);
	}
}
#endif


static const struct net_device_ops alx_netdev_ops = {
	.ndo_open               = alx_open,
	.ndo_stop               = alx_stop,
	.ndo_start_xmit         = alx_start_xmit,
	.ndo_get_stats          = alx_get_stats,
	.ndo_set_rx_mode        = alx_set_multicase_list,
	.ndo_validate_addr      = eth_validate_addr,
	.ndo_set_mac_address    = alx_set_mac_address,
	.ndo_change_mtu         = alx_change_mtu,
	.ndo_do_ioctl           = alx_ioctl,
	.ndo_tx_timeout         = alx_tx_timeout,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,39))
	.ndo_fix_features	= alx_fix_features,
	.ndo_set_features	= alx_set_features,
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,39)) */
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller    = alx_poll_controller,
#endif
};

#ifdef MDM_PLATFORM
static void alx_ipa_tx_dp_cb(void *priv, enum ipa_dp_evt_type evt,
                unsigned long data)
{
	struct alx_adapter *adpt = priv;
	struct alx_ipa_ctx *alx_ipa = adpt->palx_ipa;
	struct sk_buff *skb = (struct sk_buff *)data;
	bool schedule_ipa_work = false;

	if (!CHK_ADPT_FLAG(2, ODU_CONNECT)) {
		pr_err("%s called before ODU_CONNECT was called with evt %d \n",
				__func__, evt);
		return;
	}

	pr_debug("%s %d EVT Rcvd %d \n", __func__, __LINE__, evt);
	if (evt == IPA_RECEIVE) {
		/* Deliver SKB to network adapter */
		alx_ipa->stats.rx_ipa_excep++;
		skb->dev = adpt->netdev;
		skb->protocol = eth_type_trans(skb, skb->dev);
		/* Prevent device to goto suspend for 200 msec; to provide enough
		time for packet to be processed by network stack */
		pm_wakeup_event(&adpt->pdev->dev, 200);
		netif_rx_ni(skb);
        } else if (evt == IPA_WRITE_DONE) {
		/* SKB send to IPA, safe to free */
		alx_ipa->stats.rx_ipa_write_done++;
		dev_kfree_skb(skb);
	        spin_lock_bh(&adpt->flow_ctrl_lock);
		adpt->ipa_free_desc_cnt++;
		/* Decrement the ipa_rx_completion Counter */
		spin_lock_bh(&alx_ipa->rm_ipa_lock);
		alx_ipa->ipa_rx_completion--;
		if (!alx_ipa->ipa_rx_completion &&
			!alx_ipa->alx_tx_completion &&
			(alx_ipa->acquire_wake_src == true)) {
			__pm_relax(&alx_ipa->rm_ipa_wait);
			alx_ipa->acquire_wake_src = false;
		}
		spin_unlock_bh(&alx_ipa->rm_ipa_lock);
		if ((adpt->pendq_cnt > 0) &&
			(adpt->ipa_free_desc_cnt < adpt->ipa_low_watermark)) {
			alx_ipa->stats.ipa_low_watermark_cnt++;
		} else if ((adpt->pendq_cnt > 0) &&
			(adpt->ipa_free_desc_cnt >= adpt->ipa_low_watermark) &&
			!CHK_ADPT_FLAG(2, WQ_SCHED)) {
			schedule_ipa_work = true;
		}

                spin_lock_bh(&alx_ipa->rm_ipa_lock);
                if (alx_ipa->ipa_rx_completion == 0 && adpt->pendq_cnt == 0) {
                  spin_unlock_bh(&alx_ipa->rm_ipa_lock);
                  alx_ipa_rm_try_release(adpt);
                  /*Holding wakelock for 200 msec here will allow enough time for
                  IPA Inactivity timer expiry(100msec) + IPA TAG process to complete
                  before ALX tries to suspend. Starting Inactivity timer also helps
                  improving UL throughput scenario as the resource will not be actually
                  released till 100msec before which inactivity timer will be reset. This
                  will resuce overhead caused by resource request and release during
                  continous data transfer*/
                  pm_wakeup_event(&adpt->pdev->dev, 200);
                }
                else
                   spin_unlock_bh(&alx_ipa->rm_ipa_lock);

		spin_unlock_bh(&adpt->flow_ctrl_lock);
		if (schedule_ipa_work) {
			SET_ADPT_FLAG(2, WQ_SCHED);
			schedule_work(&adpt->ipa_send_task);
		}
	}
}

static void alx_ipa_tx_dl(void *priv, struct sk_buff *skb)
{
	struct alx_adapter *adpt = priv;
	struct alx_ipa_ctx *alx_ipa = adpt->palx_ipa;
	netdev_tx_t ret = __NETDEV_TX_MIN;

	if (!CHK_ADPT_FLAG(2, ODU_CONNECT)) {
                pr_err("%s called before ODU_CONNECT was called! \n",__func__);
                return;
        }

        spin_lock_bh(&alx_ipa->ipa_rm_state_lock);
        if (alx_ipa->ipa_prod_rm_state != ALX_IPA_RM_GRANTED) {
          spin_unlock_bh(&alx_ipa->ipa_rm_state_lock);
          alx_ipa_rm_request(adpt);
        }
        else
          spin_unlock_bh(&alx_ipa->ipa_rm_state_lock);

	pr_debug("%s %d SKB Send to line \n",__func__,__LINE__);
	if ((ret = alx_start_xmit(skb, adpt->netdev)) != NETDEV_TX_OK)
	{
		pr_err("%s alx_ipa_tx_dl() failed xmit returned %d \n",
					__func__, ret);
		alx_ipa->stats.tx_ipa_send_err++;
		dev_kfree_skb_any(skb);
	} else {
		/* Deliver SKB to HW */
		alx_ipa->stats.tx_ipa_send++;
	}
        spin_lock_bh(&alx_ipa->rm_ipa_lock);
        spin_lock_bh(&alx_ipa->ipa_rm_state_lock);
        if (alx_ipa->ipa_rx_completion == 0 && adpt->pendq_cnt == 0
            && alx_ipa->ipa_prod_rm_state != ALX_IPA_RM_RELEASED) {
          spin_unlock_bh(&alx_ipa->ipa_rm_state_lock);
          spin_unlock_bh(&alx_ipa->rm_ipa_lock);
          alx_ipa_rm_try_release(adpt);
          /*Holding wakelock for 200 msec here will allow enough time for
            IPA Inactivity timer expiry(100msec) + IPA TAG process to complete
            before ALX tries to suspend. Starting Inactivity timer also helps
            improving UL throughput scenario as the resource will not be actually
            released till 100msec before which inactivity timer will be reset. This
            will resuce overhead caused by resource request and release during
           continous data transfer*/
           pm_wakeup_event(&adpt->pdev->dev, 200);
         }
         else{
           spin_unlock_bh(&alx_ipa->ipa_rm_state_lock);
           spin_unlock_bh(&alx_ipa->rm_ipa_lock);
         }
}

static void alx_ipa_ready_work(struct work_struct *work)
{
	struct alx_adapter *adpt = container_of(work, struct alx_adapter, ipa_ready_task);
	struct alx_ipa_ctx *alx_ipa = adpt->palx_ipa;
	struct odu_bridge_params *params_ptr, params;
	int retval = 0;
	struct alx_hw *hw = &adpt->hw;
	params_ptr = &params;

	pr_info("%s:%d --- IPA is ready --- \n",__func__,__LINE__);
	alx_ipa->ipa_ready = true;

	/* Init IPA Resources */
	if (alx_ipa_setup_rm(adpt)) {
		pr_err("ALX: IPA Setup RM Failed \n");
		return;
	} else {
		SET_ADPT_FLAG(2, IPA_RM);
	}

	/* Initialize the ODU bridge driver now: odu_bridge_init()*/
	params_ptr->netdev_name = adpt->netdev->name;
	params_ptr->priv = adpt;
	params.tx_dp_notify = alx_ipa_tx_dp_cb;
	params_ptr->send_dl_skb = (void *)&alx_ipa_tx_dl;
	memcpy(params_ptr->device_ethaddr, adpt->netdev->dev_addr, ETH_ALEN);
        /* The maximum number of descriptors that can be provided to a BAM at
	* once is one less than the total number of descriptors that the buffer
	* can contain. */
	params_ptr->ipa_desc_size = (adpt->ipa_high_watermark + 1) *
					sizeof(struct sps_iovec);
	retval = odu_bridge_init(params_ptr);
	if (retval) {
		pr_err("Couldnt initialize ODU_Bridge Driver \n");
		return;
	} else {
		SET_ADPT_FLAG(2, ODU_INIT);
	}

	/* Check for link phy state */
	if (hw->link_up) {
		/* Enable ODU Bridge */
		if (CHK_ADPT_FLAG(2, ODU_INIT)) {
			retval = odu_bridge_connect();
			if (retval) {
				pr_err("Could not connect to ODU bridge %d \n",
					retval);
				return;
			} else {
				SET_ADPT_FLAG(2, ODU_CONNECT);
			}
			spin_lock_bh(&alx_ipa->ipa_rm_state_lock);
			if (alx_ipa->ipa_prod_rm_state == ALX_IPA_RM_RELEASED) {
				spin_unlock_bh(&alx_ipa->ipa_rm_state_lock);
				alx_ipa_rm_request(adpt);
			} else {
				spin_unlock_bh(&alx_ipa->ipa_rm_state_lock);
			}
		}
	}
}

static void alx_ipa_ready_cb(void *padpt)
{
	struct alx_adapter *adpt = (struct alx_adapter *)padpt;

	/* Adding to work queue */
	schedule_work(&adpt->ipa_ready_task);
}


static ssize_t alx_ipa_debugfs_read_ipa_stats(struct file *file,
                char __user *user_buf, size_t count, loff_t *ppos)
{
	struct alx_adapter *adpt = file->private_data;
	struct alx_ipa_ctx *alx_ipa = adpt->palx_ipa;
	char *buf;
	unsigned int len = 0, buf_len = 2000;
	ssize_t ret_cnt;
	u16 pendq_cnt, freeq_cnt, ipa_free_desc_cnt;
	u16 max_pkts_allowed, min_pkts_allowed;

	if (unlikely(!alx_ipa)) {
		pr_err(" %s NULL Pointer \n",__func__);
		return -EINVAL;
	}

	if (!CHK_ADPT_FLAG(2, DEBUGFS_INIT))
		return 0;

	buf = kzalloc(buf_len, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

        len += scnprintf(buf + len, buf_len - len, "\n \n");
        len += scnprintf(buf + len, buf_len - len, "%25s\n",
         "ALX IPA stats");
        len += scnprintf(buf + len, buf_len - len, "%25s\n\n",
         "==================================================");

        len += scnprintf(buf + len, buf_len - len, "%25s %10llu\n",
        "IPA RX Pkt Send: ", alx_ipa->stats.rx_ipa_send);
        len += scnprintf(buf + len, buf_len - len, "%25s %10llu\n",
        "IPA RX IPA Send Fail: ", alx_ipa->stats.rx_ipa_send_fail);
        len += scnprintf(buf + len, buf_len - len, "%25s %10llu\n",
        "IPA RX Write done: ", alx_ipa->stats.rx_ipa_write_done);
        len += scnprintf(buf + len, buf_len - len, "%25s %10llu\n",
        "IPA RX Exception: ", alx_ipa->stats.rx_ipa_excep);
        len += scnprintf(buf + len, buf_len - len, "%25s %10llu\n",
        "IPA TX Send: ", alx_ipa->stats.tx_ipa_send);
        len += scnprintf(buf + len, buf_len - len, "%25s %10llu\n",
        "IPA TX Send Err: ", alx_ipa->stats.tx_ipa_send_err);
        len += scnprintf(buf + len, buf_len - len, "%25s %10llu\n",
        "Non-IP or Frag RX Pkt: ", alx_ipa->stats.non_ip_frag_pkt);

	spin_lock_bh(&adpt->flow_ctrl_lock);
	pendq_cnt = adpt->pendq_cnt;
	freeq_cnt = adpt->freeq_cnt;
	ipa_free_desc_cnt = adpt->ipa_free_desc_cnt;
	max_pkts_allowed = adpt->ipa_high_watermark;
	min_pkts_allowed = adpt->ipa_low_watermark;
	spin_unlock_bh(&adpt->flow_ctrl_lock);

        len += scnprintf(buf + len, buf_len - len, "%25s %10u\n",
        "ALX Pending Queue Count: ", pendq_cnt);
        len += scnprintf(buf + len, buf_len - len, "%25s %10u\n",
        "ALX Free Queue Count: ", freeq_cnt);
        len += scnprintf(buf + len, buf_len - len, "%25s %10u\n",
        "IPA Free Queue Count: ", ipa_free_desc_cnt);
        len += scnprintf(buf + len, buf_len - len, "%25s %10u\n",
        "IPA High Watermark: ", max_pkts_allowed);
        len += scnprintf(buf + len, buf_len - len, "%25s %10u\n",
        "IPA Low Watermark: ", min_pkts_allowed);
        len += scnprintf(buf + len, buf_len - len, "%25s %10llu\n",
        "IPA Low Watermark Count: ", alx_ipa->stats.ipa_low_watermark_cnt);
	len += scnprintf(buf + len, buf_len - len, "%25s %10llu\n",
	"IPA Flow Ctrl Pkt Drop: ", alx_ipa->stats.flow_control_pkt_drop);

        len += scnprintf(buf + len, buf_len - len, "\n \n");
	len += scnprintf(buf + len, buf_len - len, "%25s %s\n",
	"Data Path IPA Enabled: ",CHK_ADPT_FLAG(2, ODU_CONNECT)?"True":"False");

        len += scnprintf(buf + len, buf_len - len,
		"<------------------ ALX RM STATS ------------------>\n");
	len += scnprintf(buf + len, buf_len - len, "%25s %s\n",
	"IPA PROD RM State: ",
	alx_ipa_rm_state_to_str(alx_ipa->ipa_prod_rm_state));
	len += scnprintf(buf + len, buf_len - len, "%25s %s\n",
	"IPA CONS RM State: ",
	alx_ipa_rm_state_to_str(alx_ipa->ipa_cons_rm_state));
	len += scnprintf(buf + len, buf_len - len, "%25s %s\n",
	"IPA Perf Requested:",alx_ipa->alx_ipa_perf_requested?"True":"False");
	len += scnprintf(buf + len, buf_len - len, "%25s %10llu\n",
	"IPA RX Completions Events Remaining: ", alx_ipa->ipa_rx_completion);
	len += scnprintf(buf + len, buf_len - len, "%25s %10llu\n",
	"ALX TX Completions Events Remaining: ", alx_ipa->alx_tx_completion);


        if (len > buf_len)
	        len = buf_len;

        ret_cnt = simple_read_from_buffer(user_buf, count, ppos, buf, len);
        kfree(buf);
        return ret_cnt;
}

static const struct file_operations fops_ipa_stats = {
                .read = alx_ipa_debugfs_read_ipa_stats,
                .open = simple_open,
                .owner = THIS_MODULE,
                .llseek = default_llseek,
};

static int alx_debugfs_init(struct alx_adapter *adpt)
{
	adpt->palx_ipa->debugfs_dir = debugfs_create_dir("alx", 0);
	if (!adpt->palx_ipa->debugfs_dir)
		return -ENOMEM;

	debugfs_create_file("stats", S_IRUSR, adpt->palx_ipa->debugfs_dir,
					adpt, &fops_ipa_stats);

	return 0;
}

void alx_debugfs_exit(struct alx_adapter *adpt)
{
    if (adpt->palx_ipa->debugfs_dir)
	debugfs_remove_recursive(adpt->palx_ipa->debugfs_dir);
}

/*
static void alx_ipa_process_evt(int evt, void *priv)
{
  *** When the RM Request is granted then send packet to IPA
  *** But we might not use this since we send the packet to odu_bridge
  *** and may be ODU bridge will request for IPA_RM_GRANT
  *** Need to confirm with ady
}
*/

static void alx_ipa_rm_notify(void *user_data, enum ipa_rm_event event,
                                                        unsigned long data)
{
	struct alx_adapter *adpt = user_data;
        struct alx_ipa_ctx *alx_ipa = adpt->palx_ipa;


	spin_lock_bh(&alx_ipa->ipa_rm_state_lock);
        pr_info(" %s IPA RM Evt: %d alx_ipa->ipa_prod_rm_state  %s\n",__func__,
		event, alx_ipa_rm_state_to_str(alx_ipa->ipa_prod_rm_state));

	spin_unlock_bh(&alx_ipa->ipa_rm_state_lock);

        switch(event) {
        case IPA_RM_RESOURCE_GRANTED:
		pr_debug("%s:%d IPA_RM_RESOURCE_GRANTED \n",__func__,__LINE__);
		spin_lock_bh(&alx_ipa->ipa_rm_state_lock);
		if (alx_ipa->ipa_prod_rm_state == ALX_IPA_RM_GRANTED) {
			spin_unlock_bh(&alx_ipa->ipa_rm_state_lock);
			alx_err(adpt,"%s ERR:RM_GRANTED RCVD but rm_state already Granted\n",
					__func__);
			break;
		}
                alx_ipa->ipa_prod_rm_state = ALX_IPA_RM_GRANTED;
		spin_unlock_bh(&alx_ipa->ipa_rm_state_lock);
		/* Use Send task as a deffered way to request for IPA RM */
		if (!alx_ipa->alx_ipa_perf_requested) {
			schedule_work(&adpt->ipa_send_task);
			break;
		}
		spin_lock_bh(&adpt->flow_ctrl_lock);
		if (adpt->pendq_cnt && !CHK_ADPT_FLAG(2, WQ_SCHED)) {
			SET_ADPT_FLAG(2, WQ_SCHED);
			spin_unlock_bh(&adpt->flow_ctrl_lock);
			schedule_work(&adpt->ipa_send_task);
		} else {
			spin_unlock_bh(&adpt->flow_ctrl_lock);
			alx_err(adpt,"%s -- ERR RM_GRANTED RCVD but pendq_cnt %d, WQ_SCHED:%s\n",
				__func__,adpt->pendq_cnt,CHK_ADPT_FLAG(2, WQ_SCHED)?"true":"false");
		}
                break;
        case IPA_RM_RESOURCE_RELEASED:
		spin_lock_bh(&alx_ipa->ipa_rm_state_lock);
		alx_ipa->ipa_prod_rm_state = ALX_IPA_RM_RELEASED;
		spin_unlock_bh(&alx_ipa->ipa_rm_state_lock);
		pr_debug("%s -- IPA RM Release \n",__func__);
                break;
        default:
                pr_err("Unknown RM Evt: %d", event);
                break;
        }
}

static int alx_ipa_rm_cons_request(void)
{
	struct alx_adapter *adpt = galx_adapter_ptr;
	struct alx_ipa_ctx *alx_ipa = NULL;

	pr_info("-- %s:%d -- \n",__func__,__LINE__);
	if (!adpt) {
		pr_err("%s --- adpt NULL pointer\n",__func__);
		return 0;
	}
	alx_ipa = adpt->palx_ipa;

	spin_lock_bh(&alx_ipa->ipa_rm_state_lock);
	if (alx_ipa->ipa_cons_rm_state != ALX_IPA_RM_REQUESTED) {
		alx_ipa->ipa_cons_rm_state = ALX_IPA_RM_REQUESTED;
		spin_unlock_bh(&alx_ipa->ipa_rm_state_lock);
	} else {
		spin_unlock_bh(&alx_ipa->ipa_rm_state_lock);
		pr_err("%s -- IPA RM CONS state = Requested %d."
			"Missed a release request\n"
			,__func__, alx_ipa->ipa_cons_rm_state);
	}

	/* Prevent device to goto suspend for 200 msec; to provide enough
	time for IPA to send packets to us */
	pm_wakeup_event(&adpt->pdev->dev, 200);
	return 0;
}

static int alx_ipa_rm_cons_release(void)
{
	struct alx_adapter *adpt = galx_adapter_ptr;
        struct alx_ipa_ctx *alx_ipa = NULL;

	pr_info("-- %s:%d -- \n",__func__,__LINE__);
        if (!adpt) {
                pr_err("%s --- adpt NULL pointer\n",__func__);
                return 0;
        }
	alx_ipa = adpt->palx_ipa;

	spin_lock_bh(&alx_ipa->ipa_rm_state_lock);
	if (alx_ipa->ipa_cons_rm_state != ALX_IPA_RM_RELEASED) {
		alx_ipa->ipa_cons_rm_state = ALX_IPA_RM_RELEASED;
		spin_unlock_bh(&alx_ipa->ipa_rm_state_lock);
	} else {
		spin_unlock_bh(&alx_ipa->ipa_rm_state_lock);
		pr_err("%s -- IPA RM CONS state = Requested %d."
				"Missed a release request\n"
				,__func__, alx_ipa->ipa_cons_rm_state);
	}

	return 0;
}

static int alx_ipa_setup_rm(struct alx_adapter *adpt)
{
	struct ipa_rm_create_params create_params = {0};
	int ret;
	struct alx_ipa_ctx *alx_ipa = adpt->palx_ipa;

	memset(&create_params, 0, sizeof(create_params));
	create_params.name = IPA_RM_RESOURCE_ODU_ADAPT_PROD;
	create_params.reg_params.user_data = adpt;
	create_params.reg_params.notify_cb = alx_ipa_rm_notify;
	create_params.floor_voltage = IPA_VOLTAGE_SVS;

        ret = ipa_rm_create_resource(&create_params);
        if (ret) {
                pr_err("Create ODU PROD RM resource failed: %d \n", ret);
		goto prod_fail;
        }

        ipa_rm_add_dependency(IPA_RM_RESOURCE_ODU_ADAPT_PROD,
					IPA_RM_RESOURCE_APPS_CONS);

	ret = ipa_rm_inactivity_timer_init(IPA_RM_RESOURCE_ODU_ADAPT_PROD,
					ALX_IPA_INACTIVITY_DELAY_MS);
	if (ret) {
		pr_err("Create ODU PROD RM inactivity timer failed: %d \n", ret);
		goto delete_prod;
	}

        memset(&create_params, 0, sizeof(create_params));
        create_params.name = IPA_RM_RESOURCE_ODU_ADAPT_CONS;
        create_params.request_resource= alx_ipa_rm_cons_request;
        create_params.release_resource= alx_ipa_rm_cons_release;
        create_params.floor_voltage = IPA_VOLTAGE_SVS;

        ret = ipa_rm_create_resource(&create_params);
        if (ret) {
		pr_err("Create ODU CONS RM resource failed: %d \n", ret);
                goto delete_prod;
        }

	alx_ipa_set_perf_level();

	/* Initialize IPA RM State variables */
	spin_lock_init(&alx_ipa->ipa_rm_state_lock);
	spin_lock_init(&alx_ipa->rm_ipa_lock);
	alx_ipa->ipa_rx_completion = 0;
	alx_ipa->alx_tx_completion = 0;
	alx_ipa->acquire_wake_src = 0;
	alx_ipa->ipa_prod_rm_state = ALX_IPA_RM_RELEASED;
	alx_ipa->ipa_cons_rm_state = ALX_IPA_RM_RELEASED;
	wakeup_source_init(&alx_ipa->rm_ipa_wait, "alx_ipa_completion_wake_source");
	return ret;

delete_prod:
	ipa_rm_delete_resource(IPA_RM_RESOURCE_ODU_ADAPT_PROD);

prod_fail:
        return ret;
}

static void alx_ipa_cleanup_rm(struct alx_adapter *adpt)
{
        int ret;
	struct alx_ipa_ctx *alx_ipa = adpt->palx_ipa;

        ret = ipa_rm_delete_resource(IPA_RM_RESOURCE_ODU_ADAPT_PROD);
        if (ret)
                pr_err("Resource:IPA_RM_RESOURCE_ODU_ADAPT_PROD del fail %d\n",
					ret);

        ret =  ipa_rm_inactivity_timer_destroy(IPA_RM_RESOURCE_ODU_ADAPT_PROD);
        if (ret)
                pr_err("Resource:IPA RM inactivity timer destroy fail %d\n",
					ret);

        ret = ipa_rm_delete_resource(IPA_RM_RESOURCE_ODU_ADAPT_CONS);
        if (ret)
                pr_err("Resource:IPA_RM_RESOURCE_ODU_ADAPT_CONS del fail %d\n",
					ret);
	if (!ret) {
		/* ReInitialize IPA RM State to be Released */
		alx_ipa->ipa_prod_rm_state = ALX_IPA_RM_RELEASED;
		alx_ipa->ipa_cons_rm_state = ALX_IPA_RM_RELEASED;

		wakeup_source_trash(&alx_ipa->rm_ipa_wait);
		alx_ipa->ipa_rx_completion = 0;
		alx_ipa->alx_tx_completion = 0;
	}
}

/* Request IPA RM Resource for ODU_PROD */
static int alx_ipa_rm_request(struct alx_adapter *adpt)
{
	int ret = 0;
	struct alx_ipa_ctx *alx_ipa = adpt->palx_ipa;
        spin_lock_bh(&alx_ipa->ipa_rm_state_lock);
	pr_debug("%s - IPA RM PROD State state %s\n",__func__,
			alx_ipa_rm_state_to_str(alx_ipa->ipa_prod_rm_state));
	switch (alx_ipa->ipa_prod_rm_state) {
	case ALX_IPA_RM_GRANTED:
		spin_unlock_bh(&alx_ipa->ipa_rm_state_lock);
		return 0;
	case ALX_IPA_RM_GRANT_PENDING:
		spin_unlock_bh(&alx_ipa->ipa_rm_state_lock);
		return -EINPROGRESS;
	case ALX_IPA_RM_RELEASED:
		alx_ipa->ipa_prod_rm_state = ALX_IPA_RM_GRANT_PENDING;
		break;
	default:
		spin_unlock_bh(&alx_ipa->ipa_rm_state_lock);
		pr_err("%s - IPA RM PROD State state %s\n",__func__,
			alx_ipa_rm_state_to_str(alx_ipa->ipa_prod_rm_state));
		return -EINVAL;
	}

	ret = ipa_rm_inactivity_timer_request_resource(
					IPA_RM_RESOURCE_ODU_ADAPT_PROD);
	if (ret == -EINPROGRESS) {
		pr_debug("%s - IPA RM PROD State state %s, wake %d \n",__func__,
			alx_ipa_rm_state_to_str(alx_ipa->ipa_prod_rm_state),
			alx_ipa->acquire_wake_src);
		/* Acquire the IPA TX Complete wait lock;
		Since we will only request for grant if we want to send packets*/
		spin_lock_bh(&alx_ipa->rm_ipa_lock);
		if (alx_ipa->acquire_wake_src == false) {
			__pm_stay_awake(&alx_ipa->rm_ipa_wait);
			alx_ipa->acquire_wake_src = true;
		}
		spin_unlock_bh(&alx_ipa->rm_ipa_lock);
		ret = -EINPROGRESS;
	} else if (ret == 0) {
		alx_ipa->ipa_prod_rm_state = ALX_IPA_RM_GRANTED;
		pr_debug("%s - IPA RM PROD State state %s\n",__func__,
			alx_ipa_rm_state_to_str(alx_ipa->ipa_prod_rm_state));
	} else {
		pr_err("%s -- IPA RM Request failed ret=%d\n",__func__, ret);
                ret = -EINVAL;
	}
        spin_unlock_bh(&alx_ipa->ipa_rm_state_lock);
	return ret;
}

/* Release IPA RM Resource for ODU_PROD if not needed*/
static int alx_ipa_rm_try_release(struct alx_adapter *adpt)
{
        int ret = -1;
        struct alx_ipa_ctx *alx_ipa = adpt->palx_ipa;

	pr_debug("%s:%d \n",__func__,__LINE__);
        spin_lock_bh(&alx_ipa->ipa_rm_state_lock);
	ret = ipa_rm_inactivity_timer_release_resource(
                                       IPA_RM_RESOURCE_ODU_ADAPT_PROD);
        if (ret == 0){
          alx_ipa->ipa_prod_rm_state = ALX_IPA_RM_RELEASED;
        }
        spin_unlock_bh(&alx_ipa->ipa_rm_state_lock);
        return ret;
}
#endif


#ifdef APQ_PLATFORM
int alx_smmu_init(struct device *dev)
{
	int retval = 0;
	int alx_atomic_ctx = 1;
	int alx_bypass = 1;

	pr_info("[APQ] alx_smmu_init\n");
	alx_mapping = arm_iommu_create_mapping(&platform_bus_type, ALX_SMMU_BASE, ALX_SMMU_SIZE);

	if (IS_ERR_OR_NULL(alx_mapping)) {
		retval = PTR_ERR(alx_mapping) ?: -ENODEV;
		pr_err("Failed to create ALX SMMU mapping (%d)\n", retval);
		return retval;
	}

	retval = iommu_domain_set_attr(alx_mapping->domain,
				   DOMAIN_ATTR_ATOMIC,
				   &alx_atomic_ctx);
	if (retval) {
		pr_err("Set atomic attribute for ALX SMMU failed (%d)\n", retval);
		goto release_alx_mapping;
	}

	retval = iommu_domain_set_attr(alx_mapping->domain,
				   DOMAIN_ATTR_S1_BYPASS,
				   &alx_bypass);
	if (retval) {
		pr_err("Set bypass attribute for ALX SMMU failed (%d)\n", retval);
		goto release_alx_mapping;
	}

	retval = arm_iommu_attach_device(dev, alx_mapping);
	if (retval) {
		pr_err("arm_iommu_attach_device for ALX failed (%d)\n", retval);
		goto release_alx_mapping;
	}

	pr_info("attached to SMMU successful\n");

	return retval;

release_alx_mapping:
	arm_iommu_release_mapping(alx_mapping);
	alx_mapping = NULL;

	return retval;
};

void alx_smmu_remove(struct device *dev)
{
    if (alx_mapping)
    {
	pr_info("[APQ] alx_smmu_remove\n");
	arm_iommu_detach_device(dev);
	arm_iommu_release_mapping(alx_mapping);
	alx_mapping = NULL;
    }
}
#endif

/*
 * alx_init - Device Initialization Routine
 */
static int __devinit alx_init(struct pci_dev *pdev,
		       const struct pci_device_id *ent)
{
	struct net_device *netdev;
	struct alx_adapter *adpt = NULL;
	struct alx_hw *hw = NULL;
#ifdef MDM_PLATFORM
	struct alx_ipa_ctx *alx_ipa = NULL;
#endif
	static int cards_found;
	int retval;

#ifdef APQ_PLATFORM
	retval = alx_smmu_init(&pdev->dev);
	if (retval) {
	    pr_err("%s: ALX SMMU init failed, err = %d\n", __func__, retval);
	    goto alx_smmu_init_fail;
	}
#endif

#ifndef APQ_PLATFORM
        struct odu_bridge_params *params_ptr, params;
        params_ptr = &params;
#endif

#ifdef MDM_PLATFORM
	retval = msm_pcie_pm_control(MSM_PCIE_RESUME, pdev->bus->number,
					pdev, NULL, 0);
	if (retval) {
		pr_err("Couldnt perform PCIe MSM Link resume %d\n",
			retval);
		return retval;
	}
#endif

	/* enable device (incl. PCI PM wakeup and hotplug setup) */
	retval = pci_enable_device_mem(pdev);
	if (retval) {
		dev_err(&pdev->dev, "cannot enable PCI device\n");
		goto err_alloc_device;
	}

	/*
	 * The alx chip can DMA to 64-bit addresses, but it uses a single
	 * shared register for the high 32 bits, so only a single, aligned,
	 * 4 GB physical address range can be used at a time.
	 */
	if (!dma_set_mask(&pdev->dev, DMA_BIT_MASK(64)) &&
	    !dma_set_coherent_mask(&pdev->dev, DMA_BIT_MASK(64))) {
		dev_info(&pdev->dev, "DMA to 64-BIT addresses\n");
	} else {
		retval = dma_set_mask(&pdev->dev, DMA_BIT_MASK(32));
		if (retval) {
			retval = dma_set_coherent_mask(&pdev->dev,
						       DMA_BIT_MASK(32));
			if (retval) {
				dev_err(&pdev->dev,
					"No usable DMA config, aborting\n");
				goto err_alloc_pci_res_mem;
			}
		}
	}

	retval = pci_request_selected_regions(pdev, pci_select_bars(pdev,
					IORESOURCE_MEM), alx_drv_name);
	if (retval) {
		dev_err(&pdev->dev,
			"pci_request_selected_regions failed 0x%x\n", retval);
		goto err_alloc_pci_res_mem;
	}


	pci_enable_pcie_error_reporting(pdev);
	pci_set_master(pdev);

	netdev = alloc_etherdev(sizeof(struct alx_adapter));
	if (netdev == NULL) {
		dev_err(&pdev->dev, "etherdev alloc failed\n");
		retval = -ENOMEM;
		goto err_alloc_netdev;
	}

	SET_NETDEV_DEV(netdev, &pdev->dev);
	netdev->irq  = pdev->irq;
	adpt = netdev_priv(netdev);

#ifdef MDM_PLATFORM
	if (ipa_enable) {
		/* Initialize the work for the first time */
		INIT_WORK(&adpt->ipa_ready_task, alx_ipa_ready_work);
		/* Init IPA Context */
		alx_ipa = kzalloc(sizeof(struct alx_ipa_ctx), GFP_KERNEL);
		if (!alx_ipa) {
			pr_err("kzalloc err.\n");
			return -ENOMEM;
		} else {
			adpt->palx_ipa = alx_ipa;
		}
	    /* Reset all the flags */
	    CLI_ADPT_FLAG(2, ODU_CONNECT);
	    CLI_ADPT_FLAG(2, ODU_INIT);
	    CLI_ADPT_FLAG(2, IPA_RM);
	    CLI_ADPT_FLAG(2, DEBUGFS_INIT);
		CLI_ADPT_FLAG(2, WQ_SCHED);

		galx_adapter_ptr = adpt;

		/* Initialize all the flow control variables */
		adpt->pendq_cnt = 0;
		adpt->freeq_cnt = 0;
		adpt->ipa_free_desc_cnt = ALX_IPA_SYS_PIPE_MAX_PKTS_DESC;
		adpt->ipa_high_watermark = ALX_IPA_SYS_PIPE_MAX_PKTS_DESC;
		adpt->ipa_low_watermark = ALX_IPA_SYS_PIPE_MIN_PKTS_DESC;
		alx_ipa->ipa_ready = false;
		spin_lock_init(&adpt->flow_ctrl_lock);
		INIT_LIST_HEAD(&adpt->pend_queue_head);
		INIT_LIST_HEAD(&adpt->free_queue_head);
	}
#endif

	pci_set_drvdata(pdev, adpt);
	adpt->netdev = netdev;
	adpt->pdev = pdev;
	hw = &adpt->hw;
	hw->adpt = adpt;
	adpt->msg_enable = ALX_MSG_DEFAULT;

#ifdef MDM_PLATFORM
	if (ipa_enable) {
		retval = ipa_register_ipa_ready_cb(alx_ipa_ready_cb, (void *)adpt);
		if (retval < 0) {
			if (retval == -EEXIST) {
				pr_info("%s:%d -- IPA is Ready retval %d \n",
						__func__,__LINE__,retval);
				alx_ipa->ipa_ready = true;
			} else {
				pr_info("%s:%d -- IPA is Not Ready retval %d \n",
						__func__,__LINE__,retval);
				alx_ipa->ipa_ready = false;
			}
		}

		if (alx_ipa->ipa_ready == true)
		{
			if (alx_ipa_setup_rm(adpt)) {
				pr_err("ALX: IPA Setup RM Failed \n");
	                        goto err_ipa_rm;
			} else {
				SET_ADPT_FLAG(2, IPA_RM);
			}
		}
		if (alx_debugfs_init(adpt)) {
			pr_err("ALX: Debugfs Init failed \n");
		} else {
			SET_ADPT_FLAG(2, DEBUGFS_INIT);
		}
		alx_ipa->alx_ipa_perf_requested = false;
	}
#endif

	adpt->hw.hw_addr = ioremap(pci_resource_start(pdev, BAR_0),
				   pci_resource_len(pdev, BAR_0));
	if (!adpt->hw.hw_addr) {
		alx_err(adpt, "cannot map device registers\n");
		retval = -EIO;
		goto err_iomap;
	}
	netdev->base_addr = (unsigned long)adpt->hw.hw_addr;

	/* set cb member of netdev structure*/
#ifdef MDM_PLATFORM
	netdev->netdev_ops = &alx_netdev_ops;
#else
#ifndef APQ_PLATFORM
	netdev_attach_ops(netdev, &alx_netdev_ops);
#else
	netdev->netdev_ops = &alx_netdev_ops;
#endif
#endif
	alx_set_ethtool_ops(netdev);
	netdev->watchdog_timeo = ALX_WATCHDOG_TIME;
	strlcpy(netdev->name, pci_name(pdev), sizeof(netdev->name) - 1);

	/* init alx_adapte structure */
	retval = alx_init_adapter(adpt);
	if (retval) {
		alx_err(adpt, "net device private data init failed\n");
		goto err_init_adapter;
	}

	/* reset pcie */
	retval = hw->cbs.reset_pcie(hw, true, true);
	if (retval) {
		alx_err(adpt, "PCIE Reset failed, error = %d\n", retval);
		retval = -EIO;
		goto err_init_adapter;
	}

	/* Init GPHY as early as possible due to power saving issue  */
	retval = hw->cbs.reset_phy(hw);
	if (retval) {
		alx_err(adpt, "PHY Reset failed, error = %d\n", retval);
		retval = -EIO;
		goto err_init_adapter;
	}

	/* reset mac */
	retval = hw->cbs.reset_mac(hw);
	if (retval) {
		alx_err(adpt, "MAC Reset failed, error = %d\n", retval);
		retval = -EIO;
		goto err_init_adapter;
	}

	/* setup link to put it in a known good starting state */
	retval = hw->cbs.setup_phy_link(hw, hw->autoneg_advertised, true,
					!hw->disable_fc_autoneg);

	/* get user settings */
	adpt->num_txdescs = 1024;
	adpt->num_rxdescs = 512;
	adpt->max_rxques = min_t(int, ALX_MAX_RX_QUEUES, num_online_cpus());
	adpt->max_txques = min_t(int, ALX_MAX_TX_QUEUES, num_online_cpus());

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,39))
	netdev->hw_features = NETIF_F_SG	 |
			      NETIF_F_HW_CSUM	 |
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
                              NETIF_F_HW_VLAN_CTAG_RX;
#else
			      NETIF_F_HW_VLAN_RX;
#endif /*(LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))*/
	if (adpt->hw.mac_type != alx_mac_l1c &&
	    adpt->hw.mac_type != alx_mac_l2c) {
		netdev->hw_features = netdev->hw_features |
				      NETIF_F_TSO |
				      NETIF_F_TSO6;
	}
	netdev->features = netdev->hw_features |
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
                           NETIF_F_HW_VLAN_CTAG_TX;
#else
                           NETIF_F_HW_VLAN_TX;
#endif /*(LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))*/
#else
	netdev->features = NETIF_F_SG	 |
			   NETIF_F_HW_CSUM	 |
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
                           NETIF_F_HW_VLAN_CTAG_RX;
#else
			   NETIF_F_HW_VLAN_RX;
#endif /*(LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))*/
	if (adpt->hw.mac_type != alx_mac_l1c &&
	    adpt->hw.mac_type != alx_mac_l2c) {
		netdev->features = netdev->features |
				   NETIF_F_TSO |
				   NETIF_F_TSO6;
	}
	netdev->features = netdev->features |
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
                           NETIF_F_HW_VLAN_CTAG_TX;
#else
			   NETIF_F_HW_VLAN_TX;
#endif /*(LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))*/
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,39)) */

	/* get mac addr and perm mac addr, set to register */
	if (hw->cbs.get_mac_addr)
		retval = hw->cbs.get_mac_addr(hw, hw->mac_perm_addr);
	else {
		retval = -EINVAL;
	}

/* original QC code */
#if 0
	if (retval) {
		eth_hw_addr_random(netdev);
		memcpy(hw->mac_perm_addr, netdev->dev_addr, netdev->addr_len);
	}
#endif

	/* ProjE change */
        /* Fill the mac address from input parameters
         * Always use the mac address from the input parameters
         */
        netdev->dev_addr[0] = ((mac_addr_hi16 >> 8) & 0xFF);
        netdev->dev_addr[1] = ((mac_addr_hi16) & 0xFF);

        netdev->dev_addr[2] = ((mac_addr_lo32 >> 24) & 0xFF);
        netdev->dev_addr[3] = ((mac_addr_lo32 >> 16) & 0xFF);
        netdev->dev_addr[4] = ((mac_addr_lo32 >> 8)  & 0xFF);
        netdev->dev_addr[5] = ((mac_addr_lo32) & 0xFF);
        printk(KERN_INFO "alx: Input mac: %X:%X:%X:%X:%X:%X\n",
               netdev->dev_addr[0], netdev->dev_addr[1], netdev->dev_addr[2],
               netdev->dev_addr[3], netdev->dev_addr[4], netdev->dev_addr[5]);

        if ((mac_addr_hi16 == 0xFFFFFFFF) ||
             (mac_addr_lo32 == 0xFFFFFFFF))  {
                printk(KERN_INFO "alx: Use random generate the mac address \n");
                eth_hw_addr_random(netdev);
        }

        printk(KERN_INFO "alx: Use mac addr: %X:%X:%X:%X:%X:%X\n",
               netdev->dev_addr[0], netdev->dev_addr[1], netdev->dev_addr[2],
               netdev->dev_addr[3], netdev->dev_addr[4], netdev->dev_addr[5]);
        memcpy(hw->mac_perm_addr, netdev->dev_addr, netdev->addr_len);
        // END ProjE change


	memcpy(hw->mac_addr, hw->mac_perm_addr, netdev->addr_len);
	if (hw->cbs.set_mac_addr)
		hw->cbs.set_mac_addr(hw, hw->mac_addr);

	memcpy(netdev->dev_addr, hw->mac_perm_addr, netdev->addr_len);
	memcpy(netdev->perm_addr, hw->mac_perm_addr, netdev->addr_len);
	retval = alx_validate_mac_addr(netdev->perm_addr);
	if (retval) {
		alx_err(adpt, "invalid MAC address\n");
		goto err_init_adapter;
	}

	setup_timer(&adpt->alx_timer, &alx_timer_routine,
		    (unsigned long)adpt);
	INIT_WORK(&adpt->alx_task, alx_task_routine);

	/* Number of supported queues */
	alx_set_num_queues(adpt);
	retval = alx_set_interrupt_mode(adpt);
	if (retval) {
		alx_err(adpt, "can't set interrupt mode\n");
		goto err_set_interrupt_mode;
	}

	retval = alx_set_interrupt_param(adpt);
	if (retval) {
		alx_err(adpt, "can't set interrupt parameter\n");
		goto err_set_interrupt_param;
	}

	retval = alx_alloc_all_rtx_queue(adpt);
	if (retval) {
		alx_err(adpt, "can't allocate memory for queues\n");
		goto err_alloc_rtx_queue;
	}

	alx_set_register_info_special(adpt);

	netif_dbg(adpt, probe, adpt->netdev,
		  "num_msix_noque_intrs = %d, num_msix_rxque_intrs = %d, "
		  "num_msix_txque_intrs = %d\n",
		  adpt->num_msix_noques, adpt->num_msix_rxques,
		  adpt->num_msix_txques);
	netif_dbg(adpt, probe, adpt->netdev, "num_msix_all_intrs = %d\n",
		  adpt->num_msix_intrs);

	netif_dbg(adpt, probe, adpt->netdev,
		  "RX Queue Count = %u, HRX Queue Count = %u, "
		  "SRX Queue Count = %u, TX Queue Count = %u\n",
		  adpt->num_rxques, adpt->num_hw_rxques, adpt->num_sw_rxques,
		  adpt->num_txques);

	/* WOL not supported for all but the following */
	switch (hw->pci_devid) {
	case ALX_DEV_ID_AR8131:
	case ALX_DEV_ID_AR8132:
	case ALX_DEV_ID_AR8151_V1:
	case ALX_DEV_ID_AR8151_V2:
	case ALX_DEV_ID_AR8152_V1:
	case ALX_DEV_ID_AR8152_V2:
		adpt->wol = (ALX_WOL_MAGIC | ALX_WOL_PHY);
		break;
	case ALX_DEV_ID_AR8161:
	case ALX_DEV_ID_AR8162:
		adpt->wol = (ALX_WOL_MAGIC | ALX_WOL_PHY);
		break;
	default:
		adpt->wol = 0;
		break;
	}
	device_set_wakeup_enable(&adpt->pdev->dev, adpt->wol);

	SET_ADPT_FLAG(1, STATE_DOWN);
	strlcpy(netdev->name, "eth%d", sizeof(netdev->name) - 1);
	retval = register_netdev(netdev);
	if (retval) {
		alx_err(adpt, "register netdevice failed\n");
		goto err_register_netdev;
	}
	adpt->netdev_registered = true;

#ifdef MDM_PLATFORM
	if (ipa_enable) {
		/* Initialize the ODU bridge driver now: odu_bridge_init()*/
		if (alx_ipa->ipa_ready == true) {
			params_ptr->netdev_name = netdev->name;
			params_ptr->priv = adpt;
			params.tx_dp_notify = alx_ipa_tx_dp_cb;
			params_ptr->send_dl_skb = (void *)&alx_ipa_tx_dl;
			memcpy(params_ptr->device_ethaddr, netdev->dev_addr, ETH_ALEN);
			/* The maximum number of descriptors that can be provided to a
			 * BAM at once is one less than the total number of descriptors
			 * that the buffer can contain. */
			params_ptr->ipa_desc_size = (adpt->ipa_high_watermark + 1) *
						sizeof(struct sps_iovec);
			retval = odu_bridge_init(params_ptr);
			if (retval) {
				pr_err("Couldnt initialize ODU_Bridge Driver \n");
				goto err_init_odu_bridge;
			} else {
				SET_ADPT_FLAG(2, ODU_INIT);
			}
		}

		/* Initialize IPA Flow Control Work Task */
		INIT_WORK(&adpt->ipa_send_task, alx_ipa_send_routine);
	}

	/* Register with MSM PCIe PM Framework */
	adpt->msm_pcie_event.events = MSM_PCIE_EVENT_LINKDOWN;
	adpt->msm_pcie_event.user = pdev;
	adpt->msm_pcie_event.mode = MSM_PCIE_TRIGGER_CALLBACK;
	adpt->msm_pcie_event.callback = NULL;
	adpt->msm_pcie_event.options = MSM_PCIE_CONFIG_NO_RECOVERY;
	retval = msm_pcie_register_event(&adpt->msm_pcie_event);
	if (retval) {
		pr_err("%s: PCI link down detect register failed %d\n",
				__func__, retval);
		goto msm_pcie_register_fail;
	}
#endif

	/* carrier off reporting is important to ethtool even BEFORE open */
	netif_carrier_off(netdev);
	/* keep stopping all the transmit queues for older kernels */
	netif_tx_stop_all_queues(netdev);

	/* print the MAC address */
	netif_dbg(adpt, probe, adpt->netdev, "%pM\n", netdev->dev_addr);

	/* print the adapter capability */
	if (CHK_ADPT_FLAG(0, MSI_CAP)) {
		netif_dbg(adpt, probe, adpt->netdev,
			   "MSI Capable: %s\n",
			   CHK_ADPT_FLAG(0, MSI_EN) ? "Enable" : "Disable");
	}
	if (CHK_ADPT_FLAG(0, MSIX_CAP)) {
		netif_dbg(adpt, probe, adpt->netdev,
			   "MSIX Capable: %s\n",
			   CHK_ADPT_FLAG(0, MSIX_EN) ? "Enable" : "Disable");
	}
	if (CHK_ADPT_FLAG(0, MRQ_CAP)) {
		netif_dbg(adpt, probe, adpt->netdev,
			   "MRQ Capable: %s\n",
			   CHK_ADPT_FLAG(0, MRQ_EN) ? "Enable" : "Disable");
	}
	if (CHK_ADPT_FLAG(0, MRQ_CAP)) {
		netif_dbg(adpt, probe, adpt->netdev,
			   "MTQ Capable: %s\n",
			   CHK_ADPT_FLAG(0, MTQ_EN) ? "Enable" : "Disable");
	}
	if (CHK_ADPT_FLAG(0, SRSS_CAP)) {
		netif_dbg(adpt, probe, adpt->netdev,
			   "RSS(SW) Capable: %s\n",
			   CHK_ADPT_FLAG(0, SRSS_EN) ? "Enable" : "Disable");
	}
#ifdef ALX_HIB_TIMER_CONFIG
	mod_timer(&adpt->alx_timer, jiffies);
#endif

	printk(KERN_INFO "alx: Atheros Gigabit Network Connection\n");
	cards_found++;
	return 0;

msm_pcie_register_fail:
err_init_odu_bridge:
	unregister_netdev(netdev);
	adpt->netdev_registered = false;
err_register_netdev:
	alx_free_all_rtx_queue(adpt);
err_alloc_rtx_queue:
	alx_reset_interrupt_param(adpt);
err_set_interrupt_param:
	alx_reset_interrupt_mode(adpt);
err_ipa_rm:
err_set_interrupt_mode:
err_init_adapter:
	iounmap(adpt->hw.hw_addr);
err_iomap:
	free_netdev(netdev);
err_alloc_netdev:
	pci_release_selected_regions(pdev,
				     pci_select_bars(pdev, IORESOURCE_MEM));
err_alloc_pci_res_mem:
	pci_disable_device(pdev);
err_alloc_device:
	dev_err(&pdev->dev,
		"error when probe device, error = %d\n", retval);
#ifdef APQ_PLATFORM
alx_smmu_init_fail:
#endif
	return retval;
}


/*
 * alx_remove - Device Removal Routine
 */
static void __devexit alx_remove(struct pci_dev *pdev)
{
	struct alx_adapter *adpt = pci_get_drvdata(pdev);
	struct alx_hw *hw = &adpt->hw;
	struct net_device *netdev = adpt->netdev;
	int retval = 0;

#ifdef ALX_HIB_TIMER_CONFIG
	del_timer_sync(&adpt->alx_timer);
#endif
	SET_ADPT_FLAG(1, STATE_DOWN);
	cancel_work_sync(&adpt->alx_task);

	hw->cbs.config_pow_save(hw, ALX_LINK_SPEED_UNKNOWN,
				false, false, false, false);

	/* resume permanent mac address */
	hw->cbs.set_mac_addr(hw, hw->mac_perm_addr);

	if (adpt->netdev_registered) {
		unregister_netdev(netdev);
		adpt->netdev_registered = false;
	}

	alx_free_all_rtx_queue(adpt);
	alx_reset_interrupt_param(adpt);
	alx_reset_interrupt_mode(adpt);

	iounmap(adpt->hw.hw_addr);
	pci_release_selected_regions(pdev,
				     pci_select_bars(pdev, IORESOURCE_MEM));

	netif_dbg(adpt, probe, adpt->netdev, "complete\n");
	free_netdev(netdev);

#ifdef MDM_PLATFORM
	if (ipa_enable) {
		if (CHK_ADPT_FLAG(2, ODU_CONNECT)) {
			retval = odu_bridge_disconnect();
			if (retval)
				pr_err("Could not Disconnect to ODU bridge"
					"or ODU Bridge already disconnected %d \n",
					retval);
		}

		if (CHK_ADPT_FLAG(2, ODU_INIT)) {
			retval = odu_bridge_cleanup();
			if (retval)
				pr_err("Couldnt cleanup ODU_Bridge Driver %d \n",
					retval);
		}

		/* ALX IPA Specific Cleanup */
		if (CHK_ADPT_FLAG(2, IPA_RM))
			alx_ipa_cleanup_rm(adpt);

		if (CHK_ADPT_FLAG(2, DEBUGFS_INIT))
			alx_debugfs_exit(adpt);

		/* Reset all the flags */
		CLI_ADPT_FLAG(2, ODU_CONNECT);
		CLI_ADPT_FLAG(2, ODU_INIT);
		CLI_ADPT_FLAG(2, IPA_RM);
		CLI_ADPT_FLAG(2, DEBUGFS_INIT);
		CLI_ADPT_FLAG(2, WQ_SCHED);
		kfree(adpt->palx_ipa);

		/* Cancel ALX IPA Flow Control Work */
		cancel_work_sync(&adpt->ipa_send_task);
		/* Cancel ALX IPA Ready Callback */
		cancel_work_sync(&adpt->ipa_ready_task);
	}
#endif
	/* Release wakelock to ensure that system can goto power collapse*/
	pm_relax(&pdev->dev);

	pci_disable_pcie_error_reporting(pdev);

	pci_disable_device(pdev);

#ifdef MDM_PLATFORM
	/* De-register with MSM PCIe PM framework */
	msm_pcie_deregister_event(&adpt->msm_pcie_event);

	retval = msm_pcie_pm_control(MSM_PCIE_SUSPEND, pdev->bus->number,
					  pdev, NULL, 0);
	if (retval)
		 pr_err("Couldnt Suspend PCIe MSM Link %d \n",
				retval);
#endif

#ifdef APQ_PLATFORM
	alx_smmu_remove(&pdev->dev);
#endif
}


/*
 * alx_pci_error_detected
 */
static pci_ers_result_t alx_pci_error_detected(struct pci_dev *pdev,
					       pci_channel_state_t state)
{
	struct alx_adapter *adpt = pci_get_drvdata(pdev);
	struct net_device *netdev = adpt->netdev;
	pci_ers_result_t retval = PCI_ERS_RESULT_NEED_RESET;

	netif_device_detach(netdev);

	if (state == pci_channel_io_perm_failure) {
		retval = PCI_ERS_RESULT_DISCONNECT;
		goto out;
	}

	if (netif_running(netdev))
		alx_stop_internal(adpt, ALX_OPEN_CTRL_RESET_MAC);
	pci_disable_device(pdev);
out:
	return retval;
}


/*
 * alx_pci_error_slot_reset
 */
static pci_ers_result_t alx_pci_error_slot_reset(struct pci_dev *pdev)
{
	struct alx_adapter *adpt = pci_get_drvdata(pdev);
	pci_ers_result_t retval = PCI_ERS_RESULT_DISCONNECT;

	if (pci_enable_device(pdev)) {
		alx_err(adpt, "cannot re-enable PCI device after reset\n");
		goto out;
	}

	pci_set_master(pdev);
	pci_enable_wake(pdev, PCI_D3hot, 0);
	pci_enable_wake(pdev, PCI_D3cold, 0);
	adpt->hw.cbs.reset_mac(&adpt->hw);
	retval = PCI_ERS_RESULT_RECOVERED;
out:
	pci_cleanup_aer_uncorrect_error_status(pdev);
	return retval;
}


/*
 * alx_pci_error_resume
 */
static void alx_pci_error_resume(struct pci_dev *pdev)
{
	struct alx_adapter *adpt = pci_get_drvdata(pdev);
	struct net_device *netdev = adpt->netdev;

	if (netif_running(netdev)) {
		if (alx_open_internal(adpt, 0))
			return;
	}

	netif_device_attach(netdev);
}


static struct pci_error_handlers alx_err_handler = {
	.error_detected = alx_pci_error_detected,
	.slot_reset     = alx_pci_error_slot_reset,
	.resume         = alx_pci_error_resume,
};


#ifdef CONFIG_PM_SLEEP
static SIMPLE_DEV_PM_OPS(alx_pm_ops, alx_suspend, alx_resume);
#define ALX_PM_OPS (&alx_pm_ops)
#ifndef MDM_PLATFORM

#ifndef APQ_PLATFORM
compat_pci_suspend(alx_suspend)
compat_pci_resume(alx_resume)
#endif

#endif
#else
#define ALX_PM_OPS      NULL
#endif


static struct pci_driver alx_driver = {
	.name        = alx_drv_name,
	.id_table    = alx_pci_tbl,
	.probe       = alx_init,
	.remove      = __devexit_p(alx_remove),
	.shutdown    = alx_shutdown,
	.err_handler = &alx_err_handler,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,29))
	.driver.pm   = ALX_PM_OPS,
#elif defined(CONFIG_PM_SLEEP)
	.suspend        = alx_suspend_compat,
	.resume         = alx_resume_compat,
#endif
};


static int __init alx_init_module(void)
{
	int retval;

	printk(KERN_INFO "%s\n", alx_drv_description);
	/* printk(KERN_INFO "%s\n", "-----ALX_V1.0.0.2-----"); */
	ipa_enable = module_ipa_enable;
	ipa_enable? printk("ALX: Software Bridge is Enabled\n"):
				printk("ALX: Software Bridge is Disabled\n");

	retval = pci_register_driver(&alx_driver);

	return retval;
}
module_init(alx_init_module);


static void __exit alx_exit_module(void)
{
	pci_unregister_driver(&alx_driver);
}


module_exit(alx_exit_module);
