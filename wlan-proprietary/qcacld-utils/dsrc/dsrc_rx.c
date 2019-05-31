/*
 * Copyright (c) 2014-2015 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <linux/if_ether.h>
#include <linux/if.h>
#include <arpa/inet.h>
#include <errno.h>
#include <inttypes.h>
#include "dsrc_util.h"
#include "dsrc_sock.h"
#include "crc.h"   /* crc16 is computed on the payload */

#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KNRM  "\x1B[0m"

int rx_error_count = 0;
int rx_count = 0;

/* last sequence number recieved from the tx */
uint32_t rx_seq_num;
/* last timestamp received from the tx */
uint64_t rx_timestamp;

#define PAYLOAD_BUFFER_SIZE 1500
uint8_t payload_contents[PAYLOAD_BUFFER_SIZE];
uint16_t payload_contents_len = 0;

/*******************************************************************************
 * Usage message
 ******************************************************************************/
static
void dsrc_rx_usage(void)
{
    printf("Usage: dsrc_rx [-i <interface>] [-d] [-f <output file>\n");
    printf("Defaults: interface wlanocb0, no dump\n");
    printf("-d : dump raw packet\n");
    printf("-i <interface> : interface name as found in listing by ifconfig\n");
    printf("-f : output file\n");
    printf("-e : expect to receive 802.3 frames. If not present expect to receive 802.11 frames.\n");
}

/*******************************************************************************
 * break the command arguments in to params.
 *
 * @param argc
 * @param argv
 * @param interface
 * @param dump_opt
 *
 * @return int 0 on success; -1 on error
 ******************************************************************************/
static
int dsrc_rx_getopts(int argc, char *argv[],
                    char **interface, uint8_t *dump_opt, char **output_file,
                    uint8_t *raw_mode)
{
    int c;
    int rc = 0;

    /* defaults */
    *interface = "wlanocb0";
    *dump_opt  = 0;
    *output_file = NULL;
    *raw_mode = 1;

    while ((c = getopt(argc, argv, "i:dhf:e?")) != -1) {
        switch(c) {
        case 'i':
            *interface = optarg;
            break;
        case 'd':
            *dump_opt = 1;
            break;
        case 'f':
            *output_file = optarg;
            break;
        case 'e':
            *raw_mode = 0;
            break;
        case '?':
        case 'h':
        default:
            rc = -1;
            dsrc_rx_usage();
            goto bail;
        }
    }
  bail:
    return rc;
}

void print_rx_status(void) {
    printf("<%" PRIu64 "> Seq: %d, timestamp: %" PRIu64 ", %s%d errors%s out of %d packets\n",
           timestamp_now(),
           rx_seq_num,
           rx_timestamp,
           rx_error_count == 0 ? KGRN : KRED,
           rx_error_count,
           KNRM,
           rx_count);
    printf("\n\n");
}

static
void print_eth_header(const eth_hdr_t *eth)
{
    printf("Ethernet Header (802.3)\n");
    printf("Destination address = %02x:%02x:%02x:%02x:%02x:%02x\n",
           eth->ether_dhost[0], eth->ether_dhost[1], eth->ether_dhost[2],
           eth->ether_dhost[3], eth->ether_dhost[4], eth->ether_dhost[5]);
    printf("Source address = %02x:%02x:%02x:%02x:%02x:%02x\n",
           eth->ether_shost[0], eth->ether_shost[1], eth->ether_shost[2],
           eth->ether_shost[3], eth->ether_shost[4], eth->ether_shost[5]);
    printf("Ether type/Length: 0x%04x\n", ntohs(eth->ether_type));
}

static
void print_epd_header(const ieee80211_epd_hdr_t *epd_header)
{
    printf("EPD Header:\n");
    printf("  Length/Ethertype: %d (0x%04x)\n", epd_header->ethertype,
           epd_header->ethertype);
}

static
void print_rx_stats(const struct dsrc_ocb_rx_stats_hdr* rx_stats)
{
    printf("RX Stats Header:\n");
    printf("  Version = %d\n", rx_stats->version);
    printf("  Length = %d\n", rx_stats->length);
    printf("  Channel = %u\n", rx_stats->channel_freq);
    printf("  RSSI Cmb = %d\n", rx_stats->rssi_cmb);
    printf("  RSSI Chain 0 = %d\n", rx_stats->rssi[0]);
    printf("  RSSI Chain 1 = %d\n", rx_stats->rssi[1]);
    printf("  RSSI Chain 2 = %d\n", rx_stats->rssi[2]);
    printf("  RSSI Chain 3 = %d\n", rx_stats->rssi[3]);
    printf("  Data Rate = %d\n", rx_stats->datarate);
    printf("  Timestamp (microsecond) = %u\n", rx_stats->timestamp_microsec);
    printf("  Timestamp (submicrosecond) = %u\n", rx_stats->timestamp_submicrosec);
    printf("  TSF = %d\n", rx_stats->tsf32);
    printf("  TID = %d\n", rx_stats->ext_tid);
}

/*******************************************************************************
 * Verify the snap header is WSMP ether type
 *
 * @param llc_snap
 *
 * @return bool : true if wsmp ether type
 ******************************************************************************/
static
bool llc_snap_is_wsmp(const llc_snap_hdr_t *llc_snap)
{
    printf("LLC SNAP: DSAP: %02x, SSAP: %02x, Control: %02x, "
           "OUI: %02x %02x %02x, Type: %04x",
           llc_snap->dsap, llc_snap->ssap, llc_snap->cntl,
           llc_snap->org_code[0], llc_snap->org_code[1],
           llc_snap->org_code[2], ntohs(llc_snap->ether_type));

    if (ntohs(llc_snap->ether_type) != LOCAL_ETH_P_WSMP ||
        llc_snap->dsap != 0xAA || llc_snap->ssap != 0xAA ||
        llc_snap->cntl != 0x03) {
        return false;
    }

    return true;
}

static
void signal_handler(int sig_num)
{

    if(rx_error_count == 0) {
        printf("********** %sPASS%s **********\n", KGRN, KNRM);
    }
    else {
        printf("********** %sFAIL%s **********\n", KRED, KNRM);
    }
    print_rx_status();
    exit(sig_num);
}

/*******************************************************************************
 * Verify the wsmp payload matches the one sent by dsrc_tx application.
 *        4                    2
 *    ,--------+------------+-----,
 *    |seq_num |  payload   |crc16|
 *    `--------+------------+-----`
 * @param pkt
 * @return int
 ******************************************************************************/
static int verify_wsmp_packet(uint8_t *pkt)
{
    uint8_t *payload;
    uint16_t payload_len;
    uint8_t rate;
    uint8_t chan;
    uint8_t pwr;

    rx_seq_num = 0;
    rx_timestamp = 0;
    payload_contents_len = 0;

    if (pkt == NULL) {
        printf("Invalid pointer to WSMP packet\n");
        return -1;
    }

    /* extract the wsmp data payload in addition to the rate, channel
       and power info */
    dsrc_decode_wsmp_packet(pkt, &payload, &payload_len, &rate, &chan, &pwr);

    /* verify the checksum on the payload */
    if (CRC_16_L_OK != crc_16_l_calc(payload, payload_len)) {
        printf("WSMP payload mismatch!\n");
        return -1;
    }

    /* valid payload contain a 4 byte sequence number */
    memcpy(&rx_seq_num, payload, SEQ_NUM_LEN);
    memcpy(&rx_timestamp, payload + SEQ_NUM_LEN, TIMESTAMP_LEN);
    payload_contents_len = payload_len - SEQ_NUM_LEN - TIMESTAMP_LEN - 2;
    memcpy(payload_contents, payload + SEQ_NUM_LEN + TIMESTAMP_LEN,
           payload_contents_len);

    return 0;
}

/*******************************************************************************
 * Function: print_wifi_qos_header
 * Description: Print out the contents of an 802.11 QOS header
 * Input Parameters:
 *      wifi_header: the 802.11 header
 ******************************************************************************/
static void print_wifi_qos_header(const ieee80211_qos_hdr_t *wifi_header)
{
    printf("Wi-Fi header (802.11)\n");
    printf("  Frame Control:\n");
    printf("    Protocol Version: %d\n", (wifi_header->frame_control) & 0x03);
    printf("    Type: %d\n", (wifi_header->frame_control >> 2) & 0x03);
    printf("    Subtype: %d\n", (wifi_header->frame_control >> 4) & 0x0f);
    printf("    To DS: %d\n", (wifi_header->frame_control >> 8) & 0x01);
    printf("    From DS: %d\n", (wifi_header->frame_control >> 9) & 0x1);
    printf("    More Flag: %d\n", (wifi_header->frame_control >> 10) & 0x01);
    printf("    Retry: %d\n", (wifi_header->frame_control >> 11) & 0x01);
    printf("    Pwr Mgr: %d\n", (wifi_header->frame_control >> 12) & 0x01);
    printf("    More Data: %d\n", (wifi_header->frame_control >> 13) & 0x01);
    printf("    WEP: %d\n", (wifi_header->frame_control >> 14) & 0x01);
    printf("    Order: %d\n", (wifi_header->frame_control >> 15) & 0x01);
    printf("  Duration/ID: %d\n", wifi_header->duration);
    printf("  Address 1 (dest): %02x:%02x:%02x:%02x:%02x:%02x\n",
           wifi_header->address_1[0], wifi_header->address_1[1],
           wifi_header->address_1[2], wifi_header->address_1[3],
           wifi_header->address_1[4], wifi_header->address_1[5]);
    printf("  Address 2 (source): %02x:%02x:%02x:%02x:%02x:%02x\n",
           wifi_header->address_2[0], wifi_header->address_2[1],
           wifi_header->address_2[2], wifi_header->address_2[3],
           wifi_header->address_2[4], wifi_header->address_2[5]);
    printf("  Address 3 (BSSID): %02x:%02x:%02x:%02x:%02x:%02x\n",
           wifi_header->address_3[0], wifi_header->address_3[1],
           wifi_header->address_3[2], wifi_header->address_3[3],
           wifi_header->address_3[4], wifi_header->address_3[5]);
    printf("  Sequence/Control: %d\n", wifi_header->sequence);
    printf("  QOS: 0x%x\n", wifi_header->qos);
}

/*******************************************************************************
 * parse the 802.3 headers from the packets,
 * print the contents of the headers, parse the payload, and verify
 * the contents of the payload match what was expected.
 *
 * @param pkt : 802.3 pkt
 *
 * @return bool : true on success, false on bad frame
 ******************************************************************************/
static
bool dsrc_rx_verify_frame(uint8_t *pkt, uint8_t raw_mode)
{
    if (raw_mode)
    {
        ieee80211_epd_hdr_t *epd_hdr;

        print_wifi_qos_header((ieee80211_qos_hdr_t *)pkt);
        pkt += sizeof(ieee80211_qos_hdr_t);
        epd_hdr = (ieee80211_epd_hdr_t *)pkt;
        pkt += sizeof(ieee80211_epd_hdr_t);
        print_epd_header(epd_hdr);

        if (ntohs(epd_hdr->ethertype) != LOCAL_ETH_P_WSMP) {
            printf("Not a WSMP packet\n");
            goto print_error;
        }
    }
    else
    {
        eth_hdr_t *eh;

        /* The first header is the ethernet header. */
        eh = (eth_hdr_t *)pkt;
        print_eth_header(eh);
        pkt += sizeof(eth_hdr_t);

        /* if the ether type is less than 0x600, that indicates that the
           ethernet header is followed immediatedly by the LLC SNAP header. */
        if (ntohs(eh->ether_type) < 0x600) {
            llc_snap_hdr_t *llc_snap = (llc_snap_hdr_t*) pkt;
            pkt += sizeof(llc_snap_hdr_t);

            if (!llc_snap_is_wsmp(llc_snap)) {
                printf("Not a WSMP packet\n");
                goto print_error;
            }
        } else if (ntohs(eh->ether_type) != LOCAL_ETH_P_WSMP) {
            printf("Not a WSMP packet\n");
            goto print_error;
        }
    }

    /* Now we have the WSMP packet. */
    if (verify_wsmp_packet(pkt)) {
        printf("WSMP packet verification failed!\n");
        goto print_error;
    } else {
        printf("WSMP packet verification successful!\n");
    }

    return true;

  print_error:
    printf("%s Failed\n", KRED);
    printf("%s", KNRM);
    return false;
}

int main(int argc, char *argv[])
{
    int raw_socket = -1;   /* invalid socket */
    struct sockaddr_ll sockaddr;
    int bytes_recd;
    uint8_t packet_buffer[ETH_FRAME_LEN];
    int rc = 0;
    char *interface;
    uint8_t dump_opt = 0;
    int output_fd = -1;
    char *output_file = 0;
    uint8_t raw_mode = 1;

    signal(SIGINT, signal_handler);

    rc = dsrc_rx_getopts(argc, argv, &interface, &dump_opt, &output_file,
                         &raw_mode);
    if (rc < 0) {
        goto exit;
    }

    if (output_file) {
        if (strcmp(output_file, "stdout") == 0)
            output_fd = STDOUT_FILENO;
        else if (strcmp(output_file, "stderr") == 0)
            output_fd = STDERR_FILENO;
        else {
            output_fd = open(output_file, O_WRONLY | O_CREAT);
            if (output_fd == -1) {
                printf("Error opening the output file.\n");
                goto exit;
            }
        }
    }

    /* Open a raw socket */
    rc = dsrc_socket_open(interface, &raw_socket, &sockaddr);
    if (rc < 0) {
        switch (-rc) {
        case EAFNOSUPPORT:
            printf("Error getting interface index\n");
            break;
        case EACCES:
            printf("Error getting source MAC address\n");
            break;
        case 1:
        default:
            perror("socket()");
            printf ("Need sudo?\n");
        }
        goto exit;
    }

    /* Bind to the socket */
    rc = bind(raw_socket,
              (struct sockaddr *)&sockaddr,
              sizeof(struct sockaddr_ll));
    if (rc < 0) {
        printf("Error binding to socket!\n");
        rc = -1;
        goto exit;
    }

    printf("Successfully bound to interface: %s, "
           "MAC address = %02x:%02x:%02x:%02x:%02x:%02x\n",
           interface, sockaddr.sll_addr[0], sockaddr.sll_addr[1],
           sockaddr.sll_addr[2], sockaddr.sll_addr[3], sockaddr.sll_addr[4],
           sockaddr.sll_addr[5]);

    memset(packet_buffer, 0, sizeof(packet_buffer));

    /* Listen for incoming packets */
    while (1) {
        struct dsrc_ocb_rx_stats_hdr *rx_stats;
        uint8_t* rx_frame;
        ssize_t  rx_frame_len;

        bytes_recd = dsrc_socket_recv_eth_frame_and_stats(
            raw_socket, packet_buffer, sizeof(packet_buffer), 0,
            &rx_stats, &rx_frame, &rx_frame_len);

        if (bytes_recd < 0) {
            rc = -1;
            goto exit;
        }

        if (dump_opt) {
            print_buffer(packet_buffer, bytes_recd);
        }

        if (0 != rx_stats) {
            print_rx_stats(rx_stats);
        }

        if(!dsrc_rx_verify_frame(rx_frame, raw_mode)) {
            rx_error_count++;
        } else {
            if (output_fd != -1) {
                write(output_fd, payload_contents, payload_contents_len);
            }
        }
        rx_count++;

        print_rx_status();
    }

exit:
    if (output_fd != -1)
        close(output_fd);
    (void)dsrc_socket_closeif(&raw_socket);
    return rc;
}
