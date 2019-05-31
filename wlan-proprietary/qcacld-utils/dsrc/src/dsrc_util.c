/*
 * Copyright (c) 2014-2015 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <linux/wireless.h>
#include "dsrc_util.h"

#ifndef _ANDROID_
size_t
strlcpy(char *dst, const char *src, size_t siz);
#endif

/*******************************************************************************
 * Function: dsrc_get_if_index
 * Description: Get interface index
 * Input Parameters:
 *      socket: Socket descriptor
 *      interface: Interface name
 *      if_idx: Pointer to ifreq struct
 * Return Value:
 *      0 on success. Negative value on failure
 * Side Effects: ifreq struct will contain interface index value on success
 ******************************************************************************/
int dsrc_get_if_index(int socket, char *interface, struct ifreq *if_idx)
{
    memset(if_idx, 0, sizeof(struct ifreq));
    strlcpy(if_idx->ifr_name, interface, IFNAMSIZ-1);
    if (ioctl(socket, SIOCGIFINDEX, if_idx) < 0) {
        printf("Error getting interface index\n");
        perror("SIOCGIFINDEX");
        return -1;
    }

    return 0;
}

/*******************************************************************************
 * Function: dsrc_get_if_mac
 * Description: Get MAC address
 * Input Parameters:
 *      socket: Socket descriptor
 *      interface: Interface name
 *      if_mac: Pointer to ifreq struct
 * Return Value:
 *      0 on success. Negative value on failure
 * Side Effects: ifreq struct will contain MAC address on success
 ******************************************************************************/
int dsrc_get_if_mac(int socket, char *interface, struct ifreq *if_mac)
{
    memset(if_mac, 0, sizeof(struct ifreq));
    strlcpy(if_mac->ifr_name, interface, IFNAMSIZ-1);
    if (ioctl(socket, SIOCGIFHWADDR, if_mac) < 0) {
        printf("Error getting source MAC address\n");
        perror("SIOCGIFHWADDR");
        return -1;
    }

    return 0;
}

/*******************************************************************************
 * Function: dsrc_set_channel_sched
 * Description: Set the channel schedule
 * Input Parameters:
 *      socket: Socket descriptor
 *      interface: Interface name
 *      num_channels: Number of channels in the schedule
 *      channel: Array of channel frequencies
 *      duration: Array of channel durations
 * Return Value:
 *      0 on success. Negative value on failure
 * Side Effects: None
 ******************************************************************************/
int dsrc_set_channel_sched(int socket, char *interface,
                           uint32_t num_channels, uint32_t channel[],
                           uint32_t duration[])
{
    struct dsrc_ocb_sched schedule;
    struct ifreq priv;
    uint32_t i;

    if (channel == NULL)
        return -1;

    /* Set the schedule */
    memset(&schedule, 0, sizeof(struct dsrc_ocb_sched));
    schedule.num_channels = num_channels;

    for (i = 0; i < num_channels; i++) {
        schedule.channels[i].channel_freq = channel[i];
        schedule.channels[i].duration = duration[i];

        /* Use default values for all other parameters */
        schedule.channels[i].tx_power = 23;
        schedule.channels[i].tx_rate = 12;
        schedule.channels[i].qos_params[DSRC_OCB_AC_BE].aifsn = 6;
        schedule.channels[i].qos_params[DSRC_OCB_AC_BE].cwmin = 4;
        schedule.channels[i].qos_params[DSRC_OCB_AC_BE].cwmax = 10;
        schedule.channels[i].qos_params[DSRC_OCB_AC_BK].aifsn = 9;
        schedule.channels[i].qos_params[DSRC_OCB_AC_BK].cwmin = 4;
        schedule.channels[i].qos_params[DSRC_OCB_AC_BK].cwmax = 10;
        schedule.channels[i].qos_params[DSRC_OCB_AC_VI].aifsn = 3;
        schedule.channels[i].qos_params[DSRC_OCB_AC_VI].cwmin = 3;
        schedule.channels[i].qos_params[DSRC_OCB_AC_VI].cwmax = 4;
        schedule.channels[i].qos_params[DSRC_OCB_AC_VO].aifsn = 2;
        schedule.channels[i].qos_params[DSRC_OCB_AC_VO].cwmin = 2;
        schedule.channels[i].qos_params[DSRC_OCB_AC_VO].cwmax = 3;
    }

    priv.ifr_data = (void*) &schedule;

    strlcpy(priv.ifr_name, interface, IFNAMSIZ-1);
    if (ioctl(socket, SIOCDEVPRIVATE + 4, &priv) < 0) {
        printf("Error setting channel schedule\n");
        return -1;
    }

    return 0;
}

/*******************************************************************************
 * Function: dsrc_get_channel_sched
 * Description: Get the channel schedule
 * Input Parameters:
 *      socket: Socket descriptor
 *      interface: Interface name
 *      sched_info: Pointer to buffer to receive schedule info
 * Return Value:
 *      0 on success. Negative value on failure
 * Side Effects: None
 ******************************************************************************/
int dsrc_get_channel_sched(int socket, char *interface, ocb_get_sched_t *sched_info)
{
    struct ifreq priv;

    memset(sched_info, 0, sizeof(ocb_get_sched_t));
    sched_info->length = sizeof (ocb_get_sched_t);
    sched_info->version = GET_OCB_SCHED_VERSION;

    priv.ifr_data = (void*) sched_info;

    strlcpy(priv.ifr_name, interface, IFNAMSIZ-1);
    if (ioctl(socket, SIOCDEVPRIVATE + 5, &priv) < 0) {
        printf("Error getting channel schedule\n");
        return -1;
    }

    return 0;
}


/*******************************************************************************
 * Function: dsrc_sched_info_print
 * Description: Set the channel schedule
 * Input Parameters:
 *      interface: Interface name
 *      sched_info: Pointer to buffer with schedule info
 * Return Value:
 *      0 on success. Negative value on failure
 * Side Effects: None
 ******************************************************************************/
int dsrc_sched_info_print(char *interface, ocb_get_sched_t *sched_info)
{
    uint32_t i;

    /* Print the schedule */

    printf ("Interface: %s, Info version: %d, Info length: %d\n",
            interface, sched_info->version, sched_info->length);
    printf ("Number of channels: %d\n", sched_info->num_channels);
    printf ("\n");

    for (i = 0; i < sched_info->num_channels; i++) {
        printf ("\n");
        printf ("Channel %d, Frequency %d, ", i, sched_info->channels[i].channel_freq);
        printf ("Duration %d \n", sched_info->channels[i].duration);

        /* Use default values for all other parameters */
        printf ("TX Power %d ",sched_info->channels[i].tx_power);
        printf ("TX Rate %d ",sched_info->channels[i].tx_rate);
        printf ("\nDSRC_OCB_AC_BE ");
        printf ("aifsn %d ",sched_info->channels[i].qos_params[DSRC_OCB_AC_BE].aifsn);
        printf ("cwmin %d ",sched_info->channels[i].qos_params[DSRC_OCB_AC_BE].cwmin);
        printf ("cwmax %d ",sched_info->channels[i].qos_params[DSRC_OCB_AC_BE].cwmax);
        printf ("\nDSRC_OCB_AC_BK ");
        printf ("aifsn %d ",sched_info->channels[i].qos_params[DSRC_OCB_AC_BK].aifsn);
        printf ("cwmin %d ",sched_info->channels[i].qos_params[DSRC_OCB_AC_BK].cwmin);
        printf ("cwmax %d ",sched_info->channels[i].qos_params[DSRC_OCB_AC_BK].cwmax);
        printf ("\nDSRC_OCB_AC_VI ");
        printf ("aifsn %d ",sched_info->channels[i].qos_params[DSRC_OCB_AC_VI].aifsn);
        printf ("cwmin %d ",sched_info->channels[i].qos_params[DSRC_OCB_AC_VI].cwmin);
        printf ("cwmax %d ",sched_info->channels[i].qos_params[DSRC_OCB_AC_VI].cwmax);
        printf ("\nDSRC_OCB_AC_VO ");
        printf ("aifsn %d ",sched_info->channels[i].qos_params[DSRC_OCB_AC_VO].aifsn);
        printf ("cwmin %d ",sched_info->channels[i].qos_params[DSRC_OCB_AC_VO].cwmin);
        printf ("cwmax %d ",sched_info->channels[i].qos_params[DSRC_OCB_AC_VO].cwmax);
        printf ("\n");
    }

    return 0;
}



/*******************************************************************************
 * Function: dsrc_set_channel
 * Description: Set the channel
 * Input Parameters:
 *      socket: Socket descriptor
 *      interface: Interface name
 *      channel: Channel Number
 * Return Value:
 *      0 on success. Negative value on failure
 * Side Effects: None
 ******************************************************************************/
int dsrc_set_channel(int socket, char *interface, uint32_t channel)
{
    struct dsrc_ocb_sched schedule;
    struct iwreq req;

    /* Set the schedule */
    memset(&schedule, 0, sizeof(struct dsrc_ocb_sched));
    schedule.num_channels = 1;
    schedule.channels[0].channel_freq = channel;
    schedule.channels[0].tx_power = 17;

    memset(&req, 0, sizeof(req));
    req.u.data.pointer = &schedule;
    req.u.data.length = sizeof(struct dsrc_ocb_sched);

    strlcpy(req.ifr_ifrn.ifrn_name, interface, IFNAMSIZ-1);
    if (ioctl(socket, SIOCIWFIRSTPRIV + 30, &req) < 0) {
        printf("Error setting channel\n");
        return -1;
    }

    return 0;
}

/*******************************************************************************
 * Function: dsrc_set_schedule
 * Description: Set the dsrc channel scheduler parameters.
 * Input Parameters:
 *      interface: Interface name
 *      schedule: schedule parameters
 * Return Value:
 *      0 on success. Negative value on failure
 * Side Effects: None
 ******************************************************************************/
int dsrc_ocb_set_schedule(char *interface, struct dsrc_ocb_sched *schedule)
{
    struct iwreq req;
    int sock;
    int rc = 0;

    /* Open a raw socket */
    sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock < 0) {
        return -errno;
    }
    memset(&req, 0, sizeof(req));
    req.u.data.pointer = schedule;
    req.u.data.length = sizeof(struct dsrc_ocb_sched);

    strlcpy(req.ifr_ifrn.ifrn_name, interface, IFNAMSIZ-1);
    /* issue the command to qcacld kernel module */
    rc = ioctl(sock, SIOCIWFIRSTPRIV + 30, &req);

    close(sock);

    return rc;
}

/*******************************************************************************
 * Function: dsrc_get_sched
 * Description: Get the channel schedule
 * Input Parameters:
 *      interface: Interface name
 *      num_channels: Number of channels in the schedule
 *      channel: Array of channel frequencies
 *      duration: Array of channel durations
 * Return Value:
 *      0 on success. Negative value on failure
 * Side Effects: None
 ******************************************************************************/
int dsrc_get_sched(char interface[], uint32_t *num_channels,
                   uint32_t channel[], uint32_t duration[])
{
    uint32_t i;

    if ((channel == NULL) || (duration == NULL))
        return -1;

    printf("Enter interface: ");
    scanf("%20s", interface);

    printf("Enter number of channels: ");
    scanf("%u", num_channels);

    if ((*num_channels) && (*num_channels <= DSRC_OCB_CHANNEL_MAX)) {
        for (i = 0; i < *num_channels; i++) {
            printf("Channel & Duration %d: ", i + 1);
            scanf("%u %u", &channel[i], &duration[i]);
        }
    } else {
        printf("Invalid number of channels specified!\n");
        return -1;
    }

    return 0;
}

/*******************************************************************************
 * Function: dsrc_populate_sockaddr
 * Description: Populate the sockaddr struct
 * Input Parameters:
 *      sockaddr: Pointer to sockaddr struct
 *      if_idx: Pointer to interface index struct
 *      if_mac: Pointer to MAC address struct
 * Side Effects: None
 ******************************************************************************/
void dsrc_populate_sockaddr(struct sockaddr_ll *sockaddr,
                            struct ifreq *if_idx,
                            struct ifreq *if_mac)
{
    memset(sockaddr, 0, sizeof(struct sockaddr_ll));
    sockaddr->sll_family = AF_PACKET;
    sockaddr->sll_protocol = htons(ETH_P_ALL);
    sockaddr->sll_ifindex = if_idx->ifr_ifindex;
    sockaddr->sll_hatype = 0;  /* ARPHRD_IEEE80211 */
    sockaddr->sll_pkttype = 0; /* PACKET_BROADCAST */
    sockaddr->sll_halen = IFHWADDRLEN;
    sockaddr->sll_addr[0] = if_mac->ifr_hwaddr.sa_data[0];
    sockaddr->sll_addr[1] = if_mac->ifr_hwaddr.sa_data[1];
    sockaddr->sll_addr[2] = if_mac->ifr_hwaddr.sa_data[2];
    sockaddr->sll_addr[3] = if_mac->ifr_hwaddr.sa_data[3];
    sockaddr->sll_addr[4] = if_mac->ifr_hwaddr.sa_data[4];
    sockaddr->sll_addr[5] = if_mac->ifr_hwaddr.sa_data[5];
}

/*******************************************************************************
 * Function: dsrc_populate_wsmp_tx_ctrl_hdr
 * Description: Populate the WSMP TX Control header
 * Input Parameters:
 *      hdr: Pointer to header
 *      channel: Channel Number
 * Side Effects: WSMP TX control header will get populated with default values
 ******************************************************************************/
void dsrc_populate_wsmp_tx_ctrl_hdr(struct dsrc_ocb_tx_ctrl_hdr *hdr, uint16_t channel_freq)
{
    memset(hdr, 0, sizeof(struct dsrc_ocb_tx_ctrl_hdr));
    hdr->version = 1;
    hdr->length = sizeof(struct dsrc_ocb_tx_ctrl_hdr);
    hdr->channel_freq = channel_freq;
    hdr->flags.b.valid_pwr = 1;
    hdr->pwr = 23;
}

/*******************************************************************************
 * Function: dsrc_populate_eth_hdr
 * Description: Populate the Ethernet header
 * Input Parameters:
 *      hdr: Pointer to header
 *      my_addr: Pointer to my MAC address
 *      ether_type: Ethernet type to use in header
 * Side Effects: Ethernet header will get populated
 ******************************************************************************/
void dsrc_populate_eth_hdr(eth_hdr_t *hdr, uint8_t my_addr[],
                           uint16_t ether_type)
{
    memset(hdr, 0, sizeof(eth_hdr_t));
    hdr->ether_shost[0] = my_addr[0];
    hdr->ether_shost[1] = my_addr[1];
    hdr->ether_shost[2] = my_addr[2];
    hdr->ether_shost[3] = my_addr[3];
    hdr->ether_shost[4] = my_addr[4];
    hdr->ether_shost[5] = my_addr[5];

    /* Use broadcast destination address */
    hdr->ether_dhost[0] = 0xFF;
    hdr->ether_dhost[1] = 0xFF;
    hdr->ether_dhost[2] = 0xFF;
    hdr->ether_dhost[3] = 0xFF;
    hdr->ether_dhost[4] = 0xFF;
    hdr->ether_dhost[5] = 0xFF;
    hdr->ether_type = htons(ether_type);
}

/*******************************************************************************
 * Function: dsrc_populate_qos_wifi_hdr
 * Description: Populate the Ethernet header
 * Input Parameters:
 *      hdr: Pointer to header
 *      my_addr: Pointer to my MAC address
 *      tid: Traffic ID
 * Side Effects: Ethernet header will get populated
 ******************************************************************************/
uint8_t broadcast_mac[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
void dsrc_populate_qos_wifi_hdr(ieee80211_qos_hdr_t *hdr, uint8_t my_addr[],
                           uint8_t tid)
{
    static uint8_t seq = 0;

    memset(hdr, 0, sizeof(*hdr));
    /* Set version = 0, the type = data, and the subtype = qos */
    hdr->frame_control = IEEE80211_FCTL_VERSION_0 | IEEE80211_FCTL_TYPE_DATA |
        IEEE80211_FCTL_SUBTYPE_QOS_DATA;
    memcpy(hdr->address_1, broadcast_mac, sizeof(hdr->address_1));
    memcpy(hdr->address_2, my_addr, sizeof(hdr->address_2));
    memcpy(hdr->address_3, broadcast_mac, sizeof(hdr->address_3));
    hdr->sequence = seq++;
    hdr->qos = 0x0f & tid;
}

/*******************************************************************************
 * Function: dsrc_populate_action_wifi_hdr
 * Description: Populate an action Ethernet header
 * Input Parameters:
 *      hdr: Pointer to header
 *      my_addr: Pointer to my MAC address
 * Side Effects: Ethernet header will get populated
 ******************************************************************************/
void dsrc_populate_action_wifi_hdr(ieee80211_hdr_t *hdr, uint8_t my_addr[])
{
    static uint8_t seq = 0;

    memset(hdr, 0, sizeof(*hdr));
    /* Set version = 0, the type = data, and the subtype = qos */
    hdr->frame_control = IEEE80211_FCTL_VERSION_0 |
        IEEE80211_FCTL_TYPE_MANAGEMENT |
        IEEE80211_FCTL_SUBTYPE_ACTION;
    memcpy(hdr->address_1, broadcast_mac, sizeof(hdr->address_1));
    memcpy(hdr->address_2, my_addr, sizeof(hdr->address_2));
    memcpy(hdr->address_3, broadcast_mac, sizeof(hdr->address_3));
    hdr->sequence = seq++;
}

/*******************************************************************************
 * Function: dsrc_populate_llc_hdr
 * Description: Populate the Ethernet header
 * Input Parameters:
 *      hdr: Pointer to header
 *      ether_type: Ethernet type to use in header
 * Side Effects: Ethernet header will get populated
 ******************************************************************************/
void dsrc_populate_llc_hdr(llc_snap_hdr_t *hdr, uint16_t ether_type)
{
    memset(hdr, 0, sizeof(*hdr));
    hdr->dsap = 0xAA;
    hdr->ssap = 0xAA;
    hdr->cntl = 0x03;
    hdr->org_code[0] = 0;
    hdr->org_code[1] = 0;
    hdr->org_code[2] = 0;
    hdr->ether_type = htons(ether_type);
}

/*******************************************************************************
 * Function: dsrc_populate_unicast_eth_hdr
 * Description: Populate the Ethernet header for a unicast packet
 * Input Parameters:
 *      hdr: Pointer to header
 *      my_addr: Pointer to my MAC address
 *      dest: Destination MAC address
 *      ether_type: Ethernet type to use in header
 * Side Effects: Ethernet header will get populated
 ******************************************************************************/
void dsrc_populate_unicast_eth_hdr(eth_hdr_t *hdr, uint8_t my_addr[],
                                   uint8_t dest[], uint16_t ether_type)
{
    memset(hdr, 0, sizeof(eth_hdr_t));
    hdr->ether_shost[0] = my_addr[0];
    hdr->ether_shost[1] = my_addr[1];
    hdr->ether_shost[2] = my_addr[2];
    hdr->ether_shost[3] = my_addr[3];
    hdr->ether_shost[4] = my_addr[4];
    hdr->ether_shost[5] = my_addr[5];

    /* Use given destination address */
    hdr->ether_dhost[0] = dest[0];
    hdr->ether_dhost[1] = dest[1];
    hdr->ether_dhost[2] = dest[2];
    hdr->ether_dhost[3] = dest[3];
    hdr->ether_dhost[4] = dest[4];
    hdr->ether_dhost[5] = dest[5];
    hdr->ether_type = htons(ether_type);
}

/*******************************************************************************
 * Function: dsrc_populate_dot3_hdr
 * Description: Populate the 802.3 header
 * Input Parameters:
 *      hdr: Pointer to header
 *      if_mac: Pointer to MAC address struct
 *      length: Length of the packet
 * Side Effects: 802.3 header will get populated
 ******************************************************************************/
void dsrc_populate_dot3_hdr(dot3_hdr_t *hdr, struct ifreq *if_mac,
                           uint16_t length)
{
    memset(hdr, 0, sizeof(eth_hdr_t));
    hdr->ether_shost[0] = if_mac->ifr_hwaddr.sa_data[0];
    hdr->ether_shost[1] = if_mac->ifr_hwaddr.sa_data[1];
    hdr->ether_shost[2] = if_mac->ifr_hwaddr.sa_data[2];
    hdr->ether_shost[3] = if_mac->ifr_hwaddr.sa_data[3];
    hdr->ether_shost[4] = if_mac->ifr_hwaddr.sa_data[4];
    hdr->ether_shost[5] = if_mac->ifr_hwaddr.sa_data[5];
    /* Use broadcast destination address */
    hdr->ether_dhost[0] = 0xFF;
    hdr->ether_dhost[1] = 0xFF;
    hdr->ether_dhost[2] = 0xFF;
    hdr->ether_dhost[3] = 0xFF;
    hdr->ether_dhost[4] = 0xFF;
    hdr->ether_dhost[5] = 0xFF;
    hdr->length = htons(length);
}

/*******************************************************************************
 * Function: dsrc_populate_unicast_dot3_hdr
 * Description: Populate the 802.3 header for a unicast packet
 * Input Parameters:
 *      hdr: Pointer to header
 *      if_mac: Pointer to MAC address struct
 *      dest: Destination MAC address
 *      length: Length of the packet
 * Side Effects: 802.3 header will get populated
 ******************************************************************************/
void dsrc_populate_unicast_dot3_hdr(dot3_hdr_t *hdr, struct ifreq *if_mac,
                                    uint8_t dest[], uint16_t length)
{
    memset(hdr, 0, sizeof(eth_hdr_t));
    hdr->ether_shost[0] = if_mac->ifr_hwaddr.sa_data[0];
    hdr->ether_shost[1] = if_mac->ifr_hwaddr.sa_data[1];
    hdr->ether_shost[2] = if_mac->ifr_hwaddr.sa_data[2];
    hdr->ether_shost[3] = if_mac->ifr_hwaddr.sa_data[3];
    hdr->ether_shost[4] = if_mac->ifr_hwaddr.sa_data[4];
    hdr->ether_shost[5] = if_mac->ifr_hwaddr.sa_data[5];
    /* Use broadcast destination address */
    hdr->ether_dhost[0] = dest[0];
    hdr->ether_dhost[1] = dest[1];
    hdr->ether_dhost[2] = dest[2];
    hdr->ether_dhost[3] = dest[3];
    hdr->ether_dhost[4] = dest[4];
    hdr->ether_dhost[5] = dest[5];
    hdr->length = htons(length);
}

/*******************************************************************************
 * Function: dsrc_populate_llc_snap_hdr
 * Description: Populate the LLC SNAP header
 * Input Parameters:
 *      hdr: Pointer to header
 *      ether_type: Ethernet type to use in header
 * Side Effects: LLC SNAP header will get populated
 ******************************************************************************/
void dsrc_populate_llc_snap_hdr(llc_snap_hdr_t *hdr, uint16_t ether_type)
{
    memset(hdr, 0, sizeof(llc_snap_hdr_t));
    hdr->dsap = 0xAA;
    hdr->ssap = 0xAA;
    hdr->cntl = 0x03;
    hdr->ether_type = htons(ether_type);
}

/*******************************************************************************
 * Function: dsrc_populate_wsmp_hdr
 * Description: Populate the WSMP header
 * Input Parameters:
 *      hdr: Pointer to header
 * Side Effects: WSMP header will get populated
 ******************************************************************************/
void dsrc_populate_wsmp_hdr(wsmp_hdr_t *hdr)
{
    memset(hdr, 0, sizeof(wsmp_hdr_t));
    hdr->version = 0x02;

    // the header we create is hard-coded to 4-bytes, so ensure a 4-byte PSID
    // is used (MSB = 111x xxxx)
    hdr->psid[0] = 0xEF;
    hdr->psid[1] = 0xFF;
    hdr->psid[2] = 0xFF;
    hdr->psid[3] = 0xFF;
}

/*******************************************************************************
 * Function: dsrc_populate_wsmp_data_hdr
 * Description: Populate the WSMP data header
 * Input Parameters:
 *      hdr: Pointer to header
 *      len: Length of WSMP data
 * Side Effects: WSMP data header will get populated
 ******************************************************************************/
void dsrc_populate_wsmp_data_hdr(wave_element_field_t *hdr, uint16_t len)
{
    memset(hdr, 0, sizeof(wave_element_field_t));
    hdr->wave_element_id = WAVE_ELEM_ID_DATA;
    hdr->data_len = htons(len);
}

/*******************************************************************************
 * Function: dsrc_populate_ext_field
 * Description: Populate a WSMP extension field
 * Input Parameters:
 *      ext_field: Pointer to extension field
 *      type: Type of extension field
 *      value: Value of extension field
 * Side Effects: WSMP Extension field will get populated
 ******************************************************************************/
void dsrc_populate_ext_field(wsmp_ext_field_t *ext_field, ext_field_e type,
                             uint8_t value)
{

    if (ext_field == NULL || type >= EXT_FIELD_MAX) {
        return;
    }


    switch (type){
    // these three items are all 8-bit fields
    case EXT_FIELD_RATE:
        ext_field->wave_element_id = WAVE_ELEM_ID_RATE;
        ext_field->data.data_rate = value;
        ext_field->length = 1;
        break;

    case EXT_FIELD_CHAN:
        ext_field->wave_element_id = WAVE_ELEM_ID_CHAN;
        ext_field->data.channel = value;
        ext_field->length = 1;
        break;

    case EXT_FIELD_PWR:
        ext_field->wave_element_id = WAVE_ELEM_ID_PWR;
        ext_field->data.tx_power = value;
        ext_field->length = 1;
        break;

    case EXT_FIELD_MAX:
        break;
    }


}

/*******************************************************************************
 * Function: dsrc_create_payload
 * Description: Populate the payload
 * Input Parameters:
 *      len: Length of payload
 *      seq: Sequence Number of payload
 * Return Value: Pointer to new payload on success. NULL on failure.
 * Side Effects: Creates payload of specified length with sequence number
 *      repeated through the payload
 ******************************************************************************/
uint8_t *dsrc_create_payload(uint16_t len, uint8_t seq)
{
    uint16_t i;
    uint8_t *payload;
    uint8_t *payload_ptr;

    payload = calloc(len, 1);
    if (payload == NULL) {
        goto exit;
    }

    payload_ptr = payload;
    for (i = 0; i < len; i++) {
        *payload_ptr = seq;
        payload_ptr++;
    }

exit:
    return payload;
}

/*******************************************************************************
 * Function: dsrc_create_init_req_payload
 * Description: Populate the init request payload
 * Return Value: Pointer to new payload on success. NULL on failure.
 * Side Effects: Creates the init request payload containing the special string
 ******************************************************************************/
uint8_t *dsrc_create_init_req_payload(void)
{
    int len;
    uint8_t *payload;

    len = STR_INIT_REQ_LEN + 1;
    payload = calloc(len, 1);
    if (payload == NULL) {
        goto exit;
    }

    strlcpy((char *)payload, STR_INIT_REQ, STR_INIT_REQ_LEN);

exit:
    return payload;
}

/*******************************************************************************
 * Function: dsrc_create_init_resp_payload
 * Description: Populate the init request payload
 * Return Value: Pointer to new payload on success. NULL on failure.
 * Side Effects: Creates the init request payload containing the special string
 ******************************************************************************/
uint8_t *dsrc_create_init_resp_payload(void)
{
    int len;
    uint8_t *payload;

    len = STR_INIT_RESP_LEN + 1;
    payload = calloc(len, 1);
    if (payload == NULL) {
        goto exit;
    }

    strlcpy((char *)payload, STR_INIT_RESP, STR_INIT_RESP_LEN);

exit:
    return payload;
}

/*******************************************************************************
 * Function: dsrc_create_chan_req_payload
 * Description: Populate the chan request payload
 * Return Value: Pointer to new payload on success. NULL on failure.
 * Side Effects: Creates the chan request payload containing the special string
 ******************************************************************************/
uint8_t *dsrc_create_chan_req_payload(uint16_t channel)
{
    int len;
    uint8_t *payload;

    len = STR_CHANGE_CHAN_REQ_LEN + 4 + 1;
    payload = calloc(len, 1);
    if (payload == NULL) {
        goto exit;
    }

    snprintf((char *)payload, len, "%s%04u", STR_CHANGE_CHAN_REQ, channel);

exit:
    return payload;
}

/*******************************************************************************
 * Function: dsrc_create_chan_resp_payload
 * Description: Populate the chan response payload
 * Return Value: Pointer to new payload on success. NULL on failure.
 * Side Effects: Creates the chan response payload containing the special string
 ******************************************************************************/
uint8_t *dsrc_create_chan_resp_payload(uint16_t channel)
{
    int len;
    uint8_t *payload;

    len = STR_CHANGE_CHAN_RESP_LEN + 4 + 1;
    payload = calloc(len, 1);
    if (payload == NULL) {
        goto exit;
    }

    snprintf((char *)payload, len, "%s%04u", STR_CHANGE_CHAN_RESP, channel);

exit:
    return payload;
}

/*******************************************************************************
 * Function: dsrc_create_packet
 * Description: Create a complete packet
 * Input Parameters:
 *      pkt_type: Packet type
 *      if_mac: MAC Address struct
 *      len: Length of payload
 *      seq: Sequence number of payload
 *      channel: Channel number
 *      pkt_len: Pointer to total packet length
 *      ext_rate: Rate to be used in WSMP extension field (optional)
 *      ext_chan: Channel to be used in WSMP extension field (optional)
 *      ext_pwr: Power to be used in WSMP extension field (optional)
 * Return Value: Pointer to new packet on success. NULL on failure.
 * Side Effects: Creates a new packet of specified type. pkt_len will contain
 *      actual length of created packet
 ******************************************************************************/
uint8_t *dsrc_create_packet(tx_pkt_e pkt_type, struct ifreq *if_mac,
                            int len, int seq, uint16_t channel,
                            int *pkt_len, uint8_t ext_rate,
                            uint8_t ext_chan, uint8_t ext_pwr)
{
    eth_hdr_t eth_hdr;
    llc_snap_hdr_t llc_snap_hdr;
    struct dsrc_ocb_tx_ctrl_hdr wsmp_tx_ctrl_hdr;
    wsmp_hdr_t wsmp_hdr;
    wsmp_ext_field_t wsmp_ext_rate;
    wsmp_ext_field_t wsmp_ext_chan;
    wsmp_ext_field_t wsmp_ext_pwr;
    wave_element_field_t wsmp_data;
    uint8_t *payload = NULL;
    int payload_len = 0;
    uint8_t *packet = NULL;
    uint8_t *packet_ptr = NULL;
    int packet_len = 0;

    if ((if_mac == NULL)
        || (len == 0)) {
        return NULL;
    }

    switch (pkt_type) {
    case TX_PKT_INIT_REQ:
        payload = dsrc_create_init_req_payload();
        if (payload == NULL) {
            goto exit;
        }
        payload_len = STR_INIT_REQ_LEN + 1;
        dsrc_populate_wsmp_hdr(&wsmp_hdr);
        dsrc_populate_wsmp_data_hdr(&wsmp_data, payload_len);
        dsrc_populate_eth_hdr(&eth_hdr, (uint8_t*)if_mac->ifr_hwaddr.sa_data,
                              LOCAL_ETH_P_WSMP);

        packet_len = sizeof(eth_hdr_t) + sizeof(wsmp_hdr_t)
                     + sizeof(wave_element_field_t) + payload_len;

        packet = calloc(packet_len, 1);
        if (packet == NULL) {
            free(payload);
            goto exit;
        }

        packet_ptr = packet;

        memcpy(packet_ptr, &eth_hdr, sizeof(eth_hdr_t));
        packet_ptr += sizeof(eth_hdr_t);

        memcpy(packet_ptr, &wsmp_hdr, sizeof(wsmp_hdr_t));
        packet_ptr += sizeof(wsmp_hdr_t);

        memcpy(packet_ptr, &wsmp_data, sizeof(wave_element_field_t));
        packet_ptr += sizeof(wave_element_field_t);

        memcpy(packet_ptr, payload, payload_len);
        free(payload);

        break;

    case TX_PKT_INIT_RESP:
        payload = dsrc_create_init_resp_payload();
        if (payload == NULL) {
            goto exit;
        }
        payload_len = STR_INIT_RESP_LEN + 1;
        dsrc_populate_wsmp_hdr(&wsmp_hdr);
        dsrc_populate_wsmp_data_hdr(&wsmp_data, payload_len);
        dsrc_populate_eth_hdr(&eth_hdr, (uint8_t*)if_mac->ifr_hwaddr.sa_data,
                              LOCAL_ETH_P_WSMP);

        packet_len = sizeof(eth_hdr_t) + sizeof(wsmp_hdr_t)
                     + sizeof(wave_element_field_t) + payload_len;

        packet = calloc(packet_len, 1);
        if (packet == NULL) {
            free(payload);
            goto exit;
        }

        packet_ptr = packet;

        memcpy(packet_ptr, &eth_hdr, sizeof(eth_hdr_t));
        packet_ptr += sizeof(eth_hdr_t);

        memcpy(packet_ptr, &wsmp_hdr, sizeof(wsmp_hdr_t));
        packet_ptr += sizeof(wsmp_hdr_t);

        memcpy(packet_ptr, &wsmp_data, sizeof(wave_element_field_t));
        packet_ptr += sizeof(wave_element_field_t);

        memcpy(packet_ptr, payload, payload_len);

        free(payload);

        break;

    case TX_PKT_WSMP_ONLY:
        payload = dsrc_create_payload(len, seq);
        if (payload == NULL) {
            goto exit;
        }
        payload_len = len;
        dsrc_populate_wsmp_hdr(&wsmp_hdr);
        dsrc_populate_wsmp_data_hdr(&wsmp_data, payload_len);
        dsrc_populate_eth_hdr(&eth_hdr, (uint8_t*)if_mac->ifr_hwaddr.sa_data,
                              LOCAL_ETH_P_WSMP);

        packet_len = sizeof(eth_hdr_t) + sizeof(wsmp_hdr_t)
                     + sizeof(wave_element_field_t) + payload_len;

        if (ext_rate) {
            dsrc_populate_ext_field(&wsmp_ext_rate, EXT_FIELD_RATE, ext_rate);
            packet_len += sizeof(wsmp_ext_field_t);
        }

        if (ext_chan) {
            dsrc_populate_ext_field(&wsmp_ext_chan, EXT_FIELD_CHAN, ext_chan);
            packet_len += sizeof(wsmp_ext_field_t);
        }

        if (ext_pwr) {
            dsrc_populate_ext_field(&wsmp_ext_pwr, EXT_FIELD_PWR, ext_pwr);
            packet_len += sizeof(wsmp_ext_field_t);
        }

        packet = calloc(packet_len, 1);
        if (packet == NULL) {
            free(payload);
            goto exit;
        }

        packet_ptr = packet;

        memcpy(packet_ptr, &eth_hdr, sizeof(eth_hdr_t));
        packet_ptr += sizeof(eth_hdr_t);

        memcpy(packet_ptr, &wsmp_hdr, sizeof(wsmp_hdr_t));
        packet_ptr += sizeof(wsmp_hdr_t);

        if (ext_rate) {
            memcpy(packet_ptr, &wsmp_ext_rate, sizeof(wsmp_ext_field_t));
            packet_ptr += sizeof(wsmp_ext_field_t);
        }

        if (ext_chan) {
            memcpy(packet_ptr, &wsmp_ext_chan, sizeof(wsmp_ext_field_t));
            packet_ptr += sizeof(wsmp_ext_field_t);
        }

        if (ext_pwr) {
            memcpy(packet_ptr, &wsmp_ext_pwr, sizeof(wsmp_ext_field_t));
            packet_ptr += sizeof(wsmp_ext_field_t);
        }

        memcpy(packet_ptr, &wsmp_data, sizeof(wave_element_field_t));
        packet_ptr += sizeof(wave_element_field_t);

        memcpy(packet_ptr, payload, payload_len);

        free(payload);

        break;

    case TX_PKT_WSMP_CTRL:
        payload = dsrc_create_payload(len, seq);
        if (payload == NULL) {
            goto exit;
        }
        payload_len = len;

        dsrc_populate_wsmp_hdr(&wsmp_hdr);
        dsrc_populate_wsmp_data_hdr(&wsmp_data, payload_len);
        dsrc_populate_llc_snap_hdr(&llc_snap_hdr, LOCAL_ETH_P_WSMP);
        dsrc_populate_wsmp_tx_ctrl_hdr(&wsmp_tx_ctrl_hdr, channel);
        dsrc_populate_eth_hdr(&eth_hdr, (uint8_t*)if_mac->ifr_hwaddr.sa_data,
                              LOCAL_ETH_P_QCOM_TX);

        /* This is a special case where we add a LLC SNAP header after the
         * proprietary TX control header. The TX control header gets stripped
         * off in the firmware and the LLC SNAP header will get sent over the
         * air as is.
         */
        packet_len = sizeof(eth_hdr_t)
                     + sizeof(struct dsrc_ocb_tx_ctrl_hdr)
                     + sizeof(llc_snap_hdr_t)
                     + sizeof(wsmp_hdr_t)
                     + sizeof(wave_element_field_t) + payload_len;

        packet = calloc(packet_len, 1);
        if (packet == NULL) {
            free(payload);
            goto exit;
        }

        packet_ptr = packet;

        memcpy(packet_ptr, &eth_hdr, sizeof(eth_hdr_t));
        packet_ptr += sizeof(eth_hdr_t);

        memcpy(packet_ptr, &wsmp_tx_ctrl_hdr, sizeof(struct dsrc_ocb_tx_ctrl_hdr));
        packet_ptr += sizeof(struct dsrc_ocb_tx_ctrl_hdr);

        memcpy(packet_ptr, &llc_snap_hdr, sizeof(llc_snap_hdr_t));
        packet_ptr += sizeof(llc_snap_hdr_t);

        memcpy(packet_ptr, &wsmp_hdr, sizeof(wsmp_hdr_t));
        packet_ptr += sizeof(wsmp_hdr_t);

        memcpy(packet_ptr, &wsmp_data, sizeof(wave_element_field_t));
        packet_ptr += sizeof(wave_element_field_t);

        memcpy(packet_ptr, payload, payload_len);
        free(payload);

        break;

    case TX_PKT_CHANGE_CHAN_REQ:
        payload = dsrc_create_chan_req_payload(channel);
        if (payload == NULL) {
            goto exit;
        }
        payload_len = STR_CHANGE_CHAN_REQ_LEN + 5;
        dsrc_populate_wsmp_hdr(&wsmp_hdr);
        dsrc_populate_wsmp_data_hdr(&wsmp_data, payload_len);
        dsrc_populate_eth_hdr(&eth_hdr, (uint8_t*)if_mac->ifr_hwaddr.sa_data,
                              LOCAL_ETH_P_WSMP);

        packet_len = sizeof(eth_hdr_t) + sizeof(wsmp_hdr_t)
                     + sizeof(wave_element_field_t) + payload_len;

        packet = calloc(packet_len, 1);
        if (packet == NULL) {
            free(payload);
            goto exit;
        }

        packet_ptr = packet;

        memcpy(packet_ptr, &eth_hdr, sizeof(eth_hdr_t));
        packet_ptr += sizeof(eth_hdr_t);

        memcpy(packet_ptr, &wsmp_hdr, sizeof(wsmp_hdr_t));
        packet_ptr += sizeof(wsmp_hdr_t);

        memcpy(packet_ptr, &wsmp_data, sizeof(wave_element_field_t));
        packet_ptr += sizeof(wave_element_field_t);

        memcpy(packet_ptr, payload, payload_len);
        free(payload);

        break;

    case TX_PKT_CHANGE_CHAN_RESP:
        payload = dsrc_create_chan_resp_payload(channel);
        if (payload == NULL) {
            goto exit;
        }
        payload_len = STR_CHANGE_CHAN_RESP_LEN + 5;
        dsrc_populate_wsmp_hdr(&wsmp_hdr);
        dsrc_populate_wsmp_data_hdr(&wsmp_data, payload_len);
        dsrc_populate_eth_hdr(&eth_hdr, (uint8_t*)if_mac->ifr_hwaddr.sa_data,
                              LOCAL_ETH_P_WSMP);

        packet_len = sizeof(eth_hdr_t) + sizeof(wsmp_hdr_t)
                     + sizeof(wave_element_field_t) + payload_len;

        packet = calloc(packet_len, 1);
        if (packet == NULL) {
            free(payload);
            goto exit;
        }

        packet_ptr = packet;

        memcpy(packet_ptr, &eth_hdr, sizeof(eth_hdr_t));
        packet_ptr += sizeof(eth_hdr_t);

        memcpy(packet_ptr, &wsmp_hdr, sizeof(wsmp_hdr_t));
        packet_ptr += sizeof(wsmp_hdr_t);

        memcpy(packet_ptr, &wsmp_data, sizeof(wave_element_field_t));
        packet_ptr += sizeof(wave_element_field_t);

        memcpy(packet_ptr, payload, payload_len);
        free(payload);

        break;

    default:
        printf("Invalid packet type specified!\n");
        goto exit;
    }

    *pkt_len = packet_len;

exit:
    return packet;
}

/*******************************************************************************
 * Function: dsrc_receive_packet
 * Description: Receive a packet
 * Input Parameters:
 *      socket: Socket descriptor
 *      packet_buffer: Pointer to packet buffer
 *      packet_len: Length of packet buffer
 *      if_max: MAC address struct
 * Return Value: Number of bytes received on success. Negative on failure.
 * Side Effects: Receives a pakcet and puts it in packet_buffer
 ******************************************************************************/
int dsrc_receive_packet(int socket, uint8_t *packet_buffer, int packet_len,
                        struct ifreq *if_mac, int timeout)
{
    int bytes_recd;
    eth_hdr_t *eh;
    struct timeval tv;
    uint8_t br_addr[ETH_ALEN] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    if (timeout) {
        tv.tv_sec = timeout;
        tv.tv_usec = 0;

        setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,
                   sizeof(struct timeval));
    }

    while (1) {
        memset(packet_buffer, 0, packet_len);
        bytes_recd = recvfrom(socket, packet_buffer, packet_len,
                              0, NULL, NULL);
        if (bytes_recd < 0) {
            printf("Error receiving message!\n");
            perror(NULL);
            if (errno == EAGAIN) {
                bytes_recd = 0;
            }
            goto exit;
        }

        /* Check if this is an incoming packet
           Listening to a raw socket also directs outgoing packets here.
           Hence, this check is necessary to filter out outgoing traffic */
        eh = (eth_hdr_t *)packet_buffer;
        if (memcmp((const void *)eh->ether_shost,
                   (const void *)if_mac->ifr_hwaddr.sa_data,
                   ETH_ALEN) == 0) {
            /* This is an outgoing packet */
            continue;
        }

        if ((memcmp((const void *)eh->ether_dhost,
                   (const void *)if_mac->ifr_hwaddr.sa_data,
                   ETH_ALEN)) &&
            (memcmp((const void *)eh->ether_dhost,
                   (const void *)br_addr,
                   ETH_ALEN))) {
            /* This packet is not a broadcast packet or addressed to us */
            continue;
        }
        printf("Message received: Len = %d\n", bytes_recd);
        break;
    }

exit:
    return bytes_recd;
}

/*******************************************************************************
 * Function: dsrc_decode_wsmp_packet
 * Description:
 *  Finds the key fields in an incoming WSMP packet.   This
 *  function will not actually copy the data to any of the incoming pointers,
 *  it will just set the pointers to the appropriate pointer offsets in the
 *  incoming packet buffer.
 *
 * Input Parameters:
 *      wsmp_packet: [in] Pointer to the packet buffer to decode
 *      payload: [out] first byte of the payload buffer
 *      payload_len: [out] length of the payload from the packet.
 *      rate: [out] data rate from the incoming packet (if present)
 *      chan: [out] channel from the packet (if present)
 *      pwr: [out] transmit power from the packet (if present)
 *
 * Side Effects: payload, payload_len, rate, chan and pwr will get filled in
 *      as necessary.
 ******************************************************************************/
void dsrc_decode_wsmp_packet(uint8_t *wsmp_packet,
                             uint8_t **payload, uint16_t *payload_len,
                             uint8_t *rate, uint8_t *chan, uint8_t *pwr)
{
    struct wave_element_field *wsmp_data_hdr = NULL;
    struct wsmp_ext_field *ext_field;
    uint8_t *ptr;


    if (wsmp_packet == NULL) {
        return;
    }

    // the order of bytes in the WSMP header:
    // Version, PSID (variable length), extension fields (optional), Wave Element ID, length, data
    // PSID length is determined via the value of the PSID itself, see section 8.1.3 of 1609.3
    // Length is determined by 4 MSB's of the first value
    // < 7f   = 1 byte
    // 80-BF  = 2 bytes
    // C0-DF  = 3 bytes
    // E0-EF  = 4 bytes
    //

    // start the ptr just past the PSID and version fields.  THis is either
    // the start of the extension fields (if they are present), or the start of the
    // Wave element ID of the data (what we call a wsmp_data_hdr)
    ptr = WSMP_EXT_FIELD_START(wsmp_packet);

    ext_field = (struct wsmp_ext_field*)ptr;

    // loop over all the extension fields first
    while(1) {
        switch (ext_field->wave_element_id) {
        case WAVE_ELEM_ID_RATE:
            if (rate) {
                *rate = ext_field->data.data_rate;
                ptr = ptr + WSMP_EXT_FIELD_SIZE(ext_field);
            }
            break;

        case WAVE_ELEM_ID_PWR:
            if (pwr){
                *pwr = ext_field->data.tx_power;
                ptr = ptr + WSMP_EXT_FIELD_SIZE(ext_field);
            }
            break;

        case WAVE_ELEM_ID_CHAN:
            if (chan) {
                *chan = ext_field->data.channel;
                ptr = ptr + WSMP_EXT_FIELD_SIZE(ext_field);
            }
            break;

        }

        break;
    }

    // after the extension fields comes the payload.   For WSMP, it'll be a
    // element ID, length, followed by payload
    wsmp_data_hdr = (struct wave_element_field*) ptr;

    if (wsmp_data_hdr && payload) {
        *payload = (uint8_t *)wsmp_data_hdr + sizeof(wave_element_field_t);
        if (payload_len) {
            *payload_len = ntohs(wsmp_data_hdr->data_len);
        }
    }
}

/*******************************************************************************
 * Function: dsrc_get_packet_type
 * Description: Get the packet type
 * Input Parameters:
 *      packet: Pointer to the packet buffer
 * Return Value: Type of packet
 ******************************************************************************/
rx_pkt_e dsrc_get_packet_type(uint8_t *packet)
{
    eth_hdr_t *eh;
    uint16_t ether_type;
    uint8_t *wsmp_packet = NULL;
    rx_pkt_e pkt_type = RX_PKT_OTHER;
    uint8_t *payload = NULL;
    uint16_t payload_len;

    eh = (eth_hdr_t *)packet;

    ether_type = ntohs(eh->ether_type);

    if ((ether_type != LOCAL_ETH_P_QCOM_RX) &&
        (ether_type != LOCAL_ETH_P_WSMP)) {
        pkt_type = RX_PKT_OTHER;
        goto exit;
    }

    if (ether_type == LOCAL_ETH_P_QCOM_RX) {
        pkt_type = RX_PKT_WSMP_STATS;
        wsmp_packet = packet + sizeof(eth_hdr_t) + sizeof(struct dsrc_ocb_rx_stats_hdr);
    } else if (ether_type == LOCAL_ETH_P_WSMP) {
        pkt_type = RX_PKT_WSMP_ONLY;
        wsmp_packet = packet + sizeof(eth_hdr_t);
    }

    dsrc_decode_wsmp_packet(wsmp_packet, &payload, &payload_len,
                            NULL, NULL, NULL);
    if (payload_len == (STR_INIT_REQ_LEN + 1)) {
        if (!strncmp((char *)payload, STR_INIT_REQ, STR_INIT_REQ_LEN)) {
            pkt_type = RX_PKT_INIT_REQ;
        }
    } else if (payload_len == (STR_INIT_RESP_LEN + 1)) {
        if (!strncmp((char *)payload, STR_INIT_RESP, STR_INIT_RESP_LEN)) {
            pkt_type = RX_PKT_INIT_RESP;
        }
    } else if (payload_len == (STR_CHANGE_CHAN_REQ_LEN + 5)) {
        if (!strncmp((char *)payload, STR_CHANGE_CHAN_REQ,
                     STR_CHANGE_CHAN_REQ_LEN)) {
            pkt_type = RX_PKT_CHANGE_CHAN_REQ;
        }
    } else if (payload_len == (STR_CHANGE_CHAN_RESP_LEN + 5)) {
        if (!strncmp((char *)payload, STR_CHANGE_CHAN_RESP,
                     STR_CHANGE_CHAN_RESP_LEN)) {
            pkt_type = RX_PKT_CHANGE_CHAN_RESP;
        }
    }

exit:
    return pkt_type;
}

/*******************************************************************************
 * Function: dsrc_create_echo_packet
 * Description: Creates an echo packet to return to sender
 *      with identical payload
 * Input Parameters:
 *      packet_in: Pointer to the incoming packet buffer
 *      pkt_in_len: Length of incoming packet
 *      if_mac: MAC address struct
 *      pkt_len: Pointer to length of created packet
 * Return Value: Pointer to new packet on success. NULL on failure.
 * Side Effects: pkt_len will contain length of newly created packet
 ******************************************************************************/
uint8_t *dsrc_create_echo_packet(uint8_t *packet_in, int pkt_in_len,
                                 struct ifreq *if_mac, int *pkt_out_len)
{
    eth_hdr_t *eh;
    uint16_t ether_type;
    int packet_len;
    eth_hdr_t eth_hdr;
    uint8_t *wsmp_pkt = NULL;
    int wsmp_pkt_len = 0;
    uint8_t *packet_out = NULL;
    uint8_t *packet_ptr;

    if ((packet_in == NULL) || (pkt_out_len == NULL) || (pkt_in_len <= 0)) {
        return NULL;
    }

    eh = (eth_hdr_t *)packet_in;
    ether_type = ntohs(eh->ether_type);

    if (ether_type == LOCAL_ETH_P_QCOM_RX) {
        wsmp_pkt = packet_in + sizeof(eth_hdr_t)
                   + sizeof(struct dsrc_ocb_rx_stats_hdr);
        wsmp_pkt_len = pkt_in_len - sizeof(eth_hdr_t)
                       - sizeof(struct dsrc_ocb_rx_stats_hdr);
    } else if (ether_type == LOCAL_ETH_P_WSMP) {
        wsmp_pkt = packet_in + sizeof(eth_hdr_t);
        wsmp_pkt_len = pkt_in_len - sizeof(eth_hdr_t);
    }

    /* Create a new ethernet header */
    memset(&eth_hdr, 0, sizeof(eth_hdr_t));
    eth_hdr.ether_shost[0] = if_mac->ifr_hwaddr.sa_data[0];
    eth_hdr.ether_shost[1] = if_mac->ifr_hwaddr.sa_data[1];
    eth_hdr.ether_shost[2] = if_mac->ifr_hwaddr.sa_data[2];
    eth_hdr.ether_shost[3] = if_mac->ifr_hwaddr.sa_data[3];
    eth_hdr.ether_shost[4] = if_mac->ifr_hwaddr.sa_data[4];
    eth_hdr.ether_shost[5] = if_mac->ifr_hwaddr.sa_data[5];
    /* Use incoming packets source address as destination address */
    eth_hdr.ether_dhost[0] = eh->ether_shost[0];
    eth_hdr.ether_dhost[1] = eh->ether_shost[1];
    eth_hdr.ether_dhost[2] = eh->ether_shost[2];
    eth_hdr.ether_dhost[3] = eh->ether_shost[3];
    eth_hdr.ether_dhost[4] = eh->ether_shost[4];
    eth_hdr.ether_dhost[5] = eh->ether_shost[5];
    eth_hdr.ether_type = htons(LOCAL_ETH_P_WSMP);

    packet_len = sizeof(eth_hdr_t) + wsmp_pkt_len;
    packet_out = calloc(packet_len, 1);
    if (packet_out == NULL) {
        goto exit;
    }

    packet_ptr = packet_out;

    memcpy(packet_ptr, &eth_hdr, sizeof(eth_hdr_t));
    packet_ptr += sizeof(eth_hdr_t);

    memcpy(packet_ptr, wsmp_pkt, wsmp_pkt_len);

    *pkt_out_len = packet_len;

exit:
    return packet_out;
}

/*******************************************************************************
 * Function: dsrc_verify_echo_packet
 * Description: Verify a echoed packet
 * Input Parameters:
 *      packet: Pointer to the packet buffer
 *      len: Expected length of payload
 *      seq: Expected sequence number in payload
 * Return Value: 1 on successful verification. 0 on failure.
 ******************************************************************************/
int dsrc_verify_echo_packet(uint8_t *packet, int len, int seq)
{
    eth_hdr_t *eh;
    uint8_t *wsmp_packet;
    uint16_t ether_type;
    uint8_t *payload = NULL;
    uint16_t payload_len;
    int i;
    int rc = 0;

    if (packet == NULL) {
        goto exit;;
    }

    eh = (eth_hdr_t *)packet;
    ether_type = ntohs(eh->ether_type);

    if (ether_type == LOCAL_ETH_P_QCOM_RX) {
        wsmp_packet = packet + sizeof(eth_hdr_t) + sizeof(struct dsrc_ocb_rx_stats_hdr);
    } else if (ether_type == LOCAL_ETH_P_WSMP) {
        wsmp_packet = packet + sizeof(eth_hdr_t);
    } else {
        goto exit;
    }

    dsrc_decode_wsmp_packet(wsmp_packet, &payload, &payload_len,
                            NULL, NULL, NULL);

    if (payload_len != len) {
        goto exit;
    }

    for (i = 0; i < len; i++) {
        if (*payload != seq) {
            goto exit;
        }
        payload++;
    }

    rc = 1;

exit:
    return rc;
}

/*******************************************************************************
 * Function: print_buffer
 * Description: Print out a buffer in hex
 * Input Parameters:
 *      buffer: buffer to print
 *      buffer_len: number of bytes in buffer
 ******************************************************************************/
void print_buffer (uint8_t *buffer, int buffer_len)
{
    uint8_t *pkt_buf_ptr;
    int items_printed = 0;

    pkt_buf_ptr = buffer;

    while (items_printed < buffer_len) {
        if (items_printed % 16 == 0)
            printf ("\n");
        printf ("%02x ", *pkt_buf_ptr);
        pkt_buf_ptr++;
        items_printed++;
    }
    printf ("\n");
}
