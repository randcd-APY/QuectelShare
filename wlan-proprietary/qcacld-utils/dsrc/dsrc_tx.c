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
#include <sys/ioctl.h>
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
#include <signal.h>
#include <inttypes.h>
#include "dsrc_util.h"
#include "dsrc_sock.h"
#include "crc.h"   /* crc16 is computed on the payload */

/* The maximum length of the packet is 1500. The maximum length
   for the payload is 1444. The parts of the sample packet packet are:

   1st Ethernet Header (14 bytes)..this is fake
   Tx Ctrl Header (28 bytes)
   2nd Ethernet Header (14 bytes)
      WSMP header (5 byes)
      WAVE ELEMENT Field (3 bytes)
          Seq_num (4 bytes)
          Payload (max1444 bytes)
   CRC16 (2bytes)
*/
#define MAX_PAYLOAD_SIZE 1444

const uint8_t TEST_OUI[] = {0xCF, 0x00, 0x00};

/* Used to track the packet count as well as to place the sequence number
   in the payload */
static int tx_count = 0;

/*******************************************************************************
 * Usage message
 ******************************************************************************/
static
void dsrc_tx_usage(void)
{
    printf("Usage: dsrc_tx [-i <interface>] [-c <channel freq>] [-d]\n");
    printf("    [-h] [-n] [-p <power>] [-m <MCS index>]\n");
    printf("    [-r <retry limit>] [-t <TSF expire high>]\n");
    printf("    [-s <TSF expire low>] [-a <chain mask>]\n");
    printf("    [-o <traffic ID>] [-x <repeat interval msec>]\n");
    printf("    [-p <tx power>] [-k <number of packets>]\n");
    printf("    [-l <payload length>] -f <input file>\n");
    printf("Defaults: interface wlanocb0, channel 5860, no dump\n");
    printf("-h : this message\n");
    printf("-d : dump TX packet\n");
    printf("-n : no TX control header\n");
    printf("-m [MCS index] : (data rates @ 20 MHz BW\n");
    printf("    0: 6 MBPS\n");
    printf("    1: 9 MBPS\n");
    printf("    2: 12 MBPS\n");
    printf("    3: 18 MBPS\n");
    printf("    4: 24 MBPS\n");
    printf("    5: 36 MBPS\n");
    printf("    6: 48 MBPS\n");
    printf("    7: 54 MBPS\n");
    printf("-a : chain mask (antenna). \n"
           "    eg : for chain 0 use \"-a 1\"\n");
    printf("         for chain 1 \"-a 2\"\n");
    printf("         for chain 0 and 1 \"-a 3\"\n");
    printf("-o : Traffic ID (priority 0-15)\n");
    printf("-x : repeat with the interval in milliseconds\n");
    printf("-p : tx power at 0.5dbm step. (eg : value 10 is 5dbm)\n");
    printf("-k : number of packets\n");
    printf("-l : payload length in bytes\n");
    printf("-f : input file with contents to transmit\n");
    printf("-e : 802.3 frame with LLC header\n"
           "     If this flag is not present, an 802.11 frame with EPD header\n"
           "     will be sent (raw mode)\n");
    printf("-v : send a VSA frame instead of data frame. This will always use\n"
           "     an 802.11 frame.\n");
}

/*******************************************************************************
 * Function: dsrc_get_cmdline_opts
 * Description: Parse the command line and get parameters
 * Input Parameters:
 *      argc: Number of commandline arguments
 *      argv: Command line arguments array
 *      interface: Pointer to interface
 *      channel: Pointer for channel
 *      dump_opt: Pointer to bool for packet dump option
 * Return Value: 0 on success. -1 on failure.
 * Side Effects: interface and channel will get filled in
 ******************************************************************************/
static int dsrc_tx_getopts(int argc, char *argv[], char **interface,
                           uint8_t *dump_opt, uint8_t *no_tx_ctrl_opt,
                           struct dsrc_ocb_tx_ctrl_hdr *tx_ctrl_hdr,
                           uint32_t *interval_btw_pkts, uint32_t *num_packets,
                           uint8_t *enable_stream, uint8_t *enable_count,
                           uint32_t *payload_length, char **input_file,
                           uint8_t *raw_mode, uint8_t *vendor_specific_action)
{
    int c;
    int rc = 0;

    /* defaults */
    *interface = "wlanocb0";
    *dump_opt  = 0;
    *no_tx_ctrl_opt = 0;
    *interval_btw_pkts = 0;
    *num_packets = 0;
    *enable_stream = 0;
    *enable_count = 0;
    *payload_length = 0;
    *input_file = 0;
    *raw_mode = 1;
    *vendor_specific_action = 0;

    memset(tx_ctrl_hdr, 0, sizeof(*tx_ctrl_hdr));
    tx_ctrl_hdr->version = 1;
    tx_ctrl_hdr->length = sizeof(*tx_ctrl_hdr);

    while ((c = getopt(argc, argv, "i:dh?c:p:m:r:t:s:na:o:x:k:l:f:ev")) != -1) {
        switch(c) {
        case 'c':
            tx_ctrl_hdr->channel_freq = strtoul(optarg, NULL, 10);
            break;
        case 'i':
            *interface = optarg;
            break;
        case 'd':
            *dump_opt = 1;
            break;
        case 'n':
            *no_tx_ctrl_opt = 1;
            break;
        case 'p':
            tx_ctrl_hdr->flags.b.valid_pwr = 1;
            tx_ctrl_hdr->pwr = strtol(optarg, NULL, 10);
            break;
        case 'm':
            tx_ctrl_hdr->flags.b.valid_datarate = 1;
            tx_ctrl_hdr->datarate = strtoul(optarg, NULL, 10);
            break;
        case 'r':
            tx_ctrl_hdr->flags.b.valid_retries = 1;
            tx_ctrl_hdr->retry_limit = strtoul(optarg, NULL, 10);
            break;
        case 't':
            tx_ctrl_hdr->flags.b.valid_expire_tsf = 1;
            tx_ctrl_hdr->expire_tsf_hi = strtoul(optarg, NULL, 10);
            break;
        case 's':
            tx_ctrl_hdr->flags.b.valid_expire_tsf = 1;
            tx_ctrl_hdr->expire_tsf_lo = strtoul(optarg, NULL, 10);
            break;
        case 'a':
            tx_ctrl_hdr->flags.b.valid_chain_mask = 1;
            tx_ctrl_hdr->chain_mask = strtoul(optarg, NULL, 10);
            break;
        case 'o':
            tx_ctrl_hdr->flags.b.valid_tid = 1;
            tx_ctrl_hdr->ext_tid = strtoul(optarg, NULL, 10);
            break;
        case 'x':
            *interval_btw_pkts = strtoul(optarg, NULL, 10);
            *enable_stream = 1;
            break;
        case 'k':
            *num_packets = strtoul(optarg, NULL, 10);
            *enable_count = 1;
            break;
        case 'l':
            *payload_length = strtoul(optarg, NULL, 10);
            break;
        case 'f':
            *input_file = optarg;
            break;
        case 'e':
            *raw_mode = 0;
            break;
        case 'v':
            *raw_mode = 1;
            *vendor_specific_action = 1;
            break;
        case 'h':
        case '?':
        default:
            rc = -1;
            dsrc_tx_usage();
            goto bail;
        }
    }

  bail:
    return rc;
}

/*******************************************************************************
 * stop the program
 *
 * @param sig_num
 ******************************************************************************/
static
void dsrc_tx_sigint_handler(int sig_num)
{
    printf("sent %d packets, signal %d\n", tx_count, sig_num);
    exit(0);
}

/*******************************************************************************
 * Write the default payload
 *        4                    2
 *    ,--------+------------+-----,
 *    |seq_num |  payload   |crc16|
 *    `--------+------------+-----`
 * @param buf
 * @param seq_num
 ******************************************************************************/
static void dsrc_tx_setup_payload(uint8_t *buf, uint32_t seq_num,
    uint32_t payload_length, uint64_t timestamp, uint8_t *payload)
{
    uint16_t crc16;
    uint8_t *buf_ptr, val;
    uint32_t i;

    buf_ptr = buf;

    /* Copy the Sequence number. */
    memcpy(buf_ptr, &seq_num, SEQ_NUM_LEN);
    buf_ptr += SEQ_NUM_LEN;

    /* Copy the timestamp */
    memcpy(buf_ptr, &timestamp, TIMESTAMP_LEN);
    buf_ptr += TIMESTAMP_LEN;

    /* Generate the payload */
    if (payload) {
        memcpy(buf_ptr, payload, payload_length);
    } else {
        val = 0;

        for (i = 0; i < payload_length; i++) {
            buf_ptr[i] = val;
            /* val goes from 0x00 to 0xFF */
            val = (val + 1) % 0x100;
        }
    }

    /* Calculate crc16 */
    crc16 = crc_16_l_calc(buf, SEQ_NUM_LEN + payload_length);

    memcpy(buf + SEQ_NUM_LEN + payload_length, &crc16, CRC_16_SIZE);
}

int main(int argc, char *argv[])
{
    int raw_socket = -1; /* invalid socket */
    struct sockaddr_ll sockaddr;
    eth_hdr_t eth_hdr;
    ieee80211_qos_hdr_t qos_wifi_hdr;
    ieee80211_hdr_t wifi_hdr;
    wsmp_hdr_t wsmp_hdr;
    wave_element_field_t wsmp_data;
    uint8_t *packet = 0;
    uint8_t *packet_ptr;
    uint8_t *eth_payload;
    int packet_len;
    int current_packet_len;
    int rc = 0;
    char *interface = NULL;
    uint8_t dump_opt = 0;
    uint8_t no_tx_ctrl_opt = 0;
    struct dsrc_ocb_tx_ctrl_hdr tx_ctrl_hdr = {0};
    uint32_t interval_btw_pkts;
    uint32_t num_packets, payload_length;
    uint8_t enable_stream, enable_count;
    uint64_t timestamp;
    int input_fd = -1;
    char *input_file = 0;
    uint8_t raw_mode = 0;
    uint8_t vendor_specific_action = 0;

    signal(SIGINT, dsrc_tx_sigint_handler);

    rc = dsrc_tx_getopts(argc, argv, &interface, &dump_opt,
                         &no_tx_ctrl_opt, &tx_ctrl_hdr, &interval_btw_pkts,
                         &num_packets, &enable_stream, &enable_count,
                         &payload_length, &input_file, &raw_mode,
                         &vendor_specific_action);
    if (rc < 0) {
        goto bail;
    }

    if (payload_length > MAX_PAYLOAD_SIZE) {
            printf("Error. The payload length must be less than 1500\n");
            goto bail;
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
        goto bail;
    }

    if (payload_length == 0) {
        payload_length = PAYLOAD_LEN;
    }

    /* Open the input file */
    if (input_file) {
        if (strcmp(input_file, "stdin") == 0)
            input_fd = STDIN_FILENO;
        else {
            input_fd = open(input_file, O_RDONLY);
            if (input_fd == -1) {
                printf("Error opening the input file\n");
                goto bail;
            }
        }
    }

    /* Allocate memory for complete packet */
    packet_len = SEQ_NUM_LEN + payload_length + CRC_16_SIZE
                 + sizeof(wsmp_hdr_t)
                 + sizeof(wave_element_field_t);

    if (raw_mode) {
        if (vendor_specific_action) {
            packet_len += sizeof(wifi_hdr);
            packet_len += 1 + 3; /* Action category, OUI */
        } else {
            packet_len += sizeof(qos_wifi_hdr);
            packet_len += sizeof(uint16_t); /* EPD header */
        }
    } else {
        packet_len += sizeof(eth_hdr_t);
    }

    if (!no_tx_ctrl_opt)
        packet_len += sizeof(struct dsrc_ocb_tx_ctrl_hdr) + sizeof(eth_hdr_t);
    packet = malloc(packet_len);
    if (0 == packet) {
        goto bail;
    }

    packet_ptr = packet;

    /* If we are including the TX control header,
       copy the TX control header to the packet. */
    if (!no_tx_ctrl_opt)
    {
        /* First the ethernet header to indicate that the TX control header
           follows. */
        memset(&eth_hdr, 0, sizeof(eth_hdr_t));
        eth_hdr.ether_type = htons(LOCAL_ETH_P_QCOM_TX);
        memcpy(packet_ptr, &eth_hdr, sizeof(eth_hdr_t));
        packet_ptr += sizeof(eth_hdr_t);
        /* And then the TX control header. */
        memcpy(packet_ptr, &tx_ctrl_hdr, sizeof(struct dsrc_ocb_tx_ctrl_hdr));
        packet_ptr += sizeof(tx_ctrl_hdr);
    }

    /* Now we need the ethernet header for the packet. */
    if (raw_mode) {
        if (vendor_specific_action) {
            dsrc_populate_action_wifi_hdr(&wifi_hdr, sockaddr.sll_addr);
            memcpy(packet_ptr, &wifi_hdr, sizeof(wifi_hdr));
            packet_ptr += sizeof(wifi_hdr);
            *packet_ptr = 127;  /* Vendor specific action */
            packet_ptr += 1;
            memcpy(packet_ptr, TEST_OUI, 3); /* OUI, Test protocol */
            packet_ptr += 3;
        } else {
            dsrc_populate_qos_wifi_hdr(&qos_wifi_hdr, sockaddr.sll_addr,
                                   tx_ctrl_hdr.ext_tid);
            memcpy(packet_ptr, &qos_wifi_hdr, sizeof(qos_wifi_hdr));
            packet_ptr += sizeof(qos_wifi_hdr);

            /* EPD header */
            *((uint16_t *)packet_ptr) = htons(LOCAL_ETH_P_WSMP);
            packet_ptr += sizeof(uint16_t);
        }
    } else {
        dsrc_populate_eth_hdr(&eth_hdr, sockaddr.sll_addr, LOCAL_ETH_P_WSMP);
        memcpy(packet_ptr, &eth_hdr, sizeof(eth_hdr_t));
        packet_ptr += sizeof(eth_hdr_t);
    }

    eth_payload = packet_ptr;

    do {
        int bytes_sent;
        int current_payload_length = payload_length;
        uint8_t input_buffer[1500];
        tx_count++;
        packet_ptr = eth_payload;

        /* Read the bytes from the input */
        if (input_fd != -1) {
            current_payload_length = read(input_fd, input_buffer,
                                           payload_length - TIMESTAMP_LEN);
            if (current_payload_length <= 0) {
                printf("End of input\n");
                break;
            }
            current_payload_length += TIMESTAMP_LEN;
        }

        current_packet_len = packet_len - payload_length +
                current_payload_length;

        /* Now we need the WSMP header. */
        dsrc_populate_wsmp_hdr(&wsmp_hdr);
        memcpy(packet_ptr, &wsmp_hdr, sizeof(wsmp_hdr_t));
        packet_ptr += sizeof(wsmp_hdr_t);

        /* Now we need the WSMP Data header */
        dsrc_populate_wsmp_data_hdr(&wsmp_data,
                                SEQ_NUM_LEN + current_payload_length + CRC_16_SIZE);
        memcpy(packet_ptr, &wsmp_data, sizeof(wave_element_field_t));
        packet_ptr += sizeof(wave_element_field_t);

        timestamp = timestamp_now();
        dsrc_tx_setup_payload(packet_ptr, tx_count, current_payload_length, timestamp,
                              (input_fd != -1) ? input_buffer : 0);

        bytes_sent = sendto(raw_socket, packet, current_packet_len, 0,
                            (const struct sockaddr *)&sockaddr,
                            sizeof(struct sockaddr_ll));
        if (bytes_sent < 0) {
            printf("Error sending message: %d\n", bytes_sent);
            rc = -1;
            goto bail;
        } else {
            printf("<%" PRIu64 "> Seq: %d, Length = %d\n", timestamp,
                   tx_count, bytes_sent);
        }
        if (dump_opt) {
            print_buffer(packet, current_packet_len);
        }
        if (enable_stream) {
            usleep(interval_btw_pkts * 1000);
        }
        /* Check exit conditions */
        if (enable_count) {
            num_packets--;
            if (num_packets == 0) {
                break;
            }
        } else if (!enable_stream) {
            break;
        }
    } while (1); /* for ever */

    /* Print the number of packets sent */
    dsrc_tx_sigint_handler(0);

bail:

    free(packet);   /* packet == 0 if unallocated */
    (void)dsrc_socket_closeif(&raw_socket);

    return rc;
}

