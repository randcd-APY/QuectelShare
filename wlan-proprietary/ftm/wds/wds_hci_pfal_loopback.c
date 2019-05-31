
/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2012 by Qualcomm Atheros, Inc..
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/select.h>
#include <termios.h>
#include <pthread.h>
#include <stdio.h>
#include "wds_hci_pfal.h"
#include <math.h>
#include <dlfcn.h>
#include <unistd.h>
#include <string.h>
#include <cutils/log.h>
#include "wds_hci_pfal_loopback.h"

#undef LOG_SUCCESS_STATS

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "HCI_VALIDATOR"

/* Extern Function prototypes */
extern int process_pc_data_to_soc(wdsdaemon *wds, unsigned char *buf, int src_fd);

/* Global Data */
hci_loopback_core_struct hci_lb_core;

const char HCI_packets_to_test_menu_options[] = "\n\
================ MENU ================= \n\
\t 1. BT-Only \n\
\t 2. FM-Only \n\
\t 3. BT + FM \n\
======================================= \n\
\t Your option: " ;

const char maximum_packets_menu[] = "\n\
================ MENU ================= \n\
\t 0. Continious \n\
\t 1. Select maximum packets to be loopback each \n\
======================================= \n\
\t Your option: " ;

/* Function Definitions */
void send_loopback_init_command(wdsdaemon *wds)
{
    uint8 buf_in[] = {
                       0x01,
                       0x1C,
                       0xFC,
                       0x07,
                       (hci_lb_core.lb_pkt_type), //HCI PKT types to loopback
                       (hci_lb_core.pkt_type_bt.pkt_delay & 0xFF), //BT Loopback delay: LSB
                       ((hci_lb_core.pkt_type_bt.pkt_delay & 0xFF00) >> 8), //BT Loopback delay: MSB
                       (hci_lb_core.pkt_type_fm.pkt_delay & 0xFF), //FM Loopback delay: LSB
                       ((hci_lb_core.pkt_type_fm.pkt_delay & 0xFF00) >> 8), //BT Loopback delay: MSB
                       (hci_lb_core.pkt_type_ant.pkt_delay & 0xFF), //ANT Loopback delay: LSB
                       ((hci_lb_core.pkt_type_ant.pkt_delay & 0xFF00) >> 8), //ANT Loopback delay: MSB
                      };

    printf("\nEnabling the loopback functionality in the Controller...\n");
    process_pc_data_to_soc(wds, buf_in, wds->soc_if.uart.bt_fd);
}

uint8 lb_is_ssr_in_prog()
{
    uint8 status = 0;

    pthread_mutex_lock(&hci_lb_core.lb_ssr_lock);
    status = hci_lb_core.ssr_in_prog;
    pthread_mutex_unlock(&hci_lb_core.lb_ssr_lock);
    return status;
}

void lb_trigger_ssr(wdsdaemon *wds)
{
    uint8 buf_in[] = {
                      0x01,
                      0x1C,
                      0xFC,
                      0x04,
                      LOOPBACK_MODE_TRIGGER_SSR
                     };

    if (lb_is_ssr_in_prog()) {
        printf("SSR already triggered and in-progress\n");
        return;
    }

    pthread_mutex_lock(&hci_lb_core.lb_ssr_lock);
    hci_lb_core.ssr_in_prog = 1;
    pthread_mutex_unlock(&hci_lb_core.lb_ssr_lock);

    process_pc_data_to_soc(wds, buf_in, wds->soc_if.uart.bt_fd);
    sleep(SSR_COMPLETE_DELAY);
}

struct timespec lb_pkt_timeout()
{
    struct timespec ts;
    struct timeval tp;
    uint32 mSecs = 0, t1, t2, t3;

    t1 = hci_lb_core.pkt_type_bt.pkt_delay;
    t2 = hci_lb_core.pkt_type_fm.pkt_delay;
    t3 = hci_lb_core.pkt_type_ant.pkt_delay;

    mSecs = (t1 > t2) ? t1 : ((t2 > t3) ? t2 : t3);
    mSecs = (mSecs > 1000) ? mSecs : 1000;

    gettimeofday(&tp, NULL);
    ts.tv_sec = tp.tv_sec;
    ts.tv_nsec = tp.tv_usec * 1000;
    ts.tv_sec += (mSecs/1000) + 1;

    return ts;
}

/*===========================================================================
FUNCTION:    lb_functionality_enable

DESCRIPTION:
    Enable the loopback functionality in the Controller by sending the VSC

DEPENDENCIES
NIL

RETURN VALUE
    STATUS_SUCCESS if the loopback functionality is enabled
    STATUS_ERROR if the loopback functionality is not enabled

SIDE EFFECTS
None

===========================================================================*/
int lb_functionality_enable(wdsdaemon *wds)
{
    int retval = STATUS_SUCCESS;

    pthread_mutex_lock(&hci_lb_core.lb_enable_lock);
    if (hci_lb_core.lb_enable_status == LB_STATUS_DISABLED)
    {
        pthread_mutex_lock(&hci_lb_core.pkt_type_bt.tx_q_lock);

        /* Send VSC to enable Loopback functionality in the Controller */
        send_loopback_init_command(wds);

        /* Wait for the response from the Controller */
        hci_lb_core.pkt_type_bt.valid_signal = 0;
        while(hci_lb_core.pkt_type_bt.valid_signal == 0){
            pthread_cond_wait(&hci_lb_core.pkt_type_bt.event_cond,
                &hci_lb_core.pkt_type_bt.tx_q_lock);
        }
        hci_lb_core.pkt_type_bt.valid_signal = 0;
        pthread_mutex_unlock(&hci_lb_core.pkt_type_bt.tx_q_lock);

        printf("\nReveived Loopback complete event from Controller.\n");
        if (hci_lb_core.pkt_type_bt.rx_pkt_buffer[HCI_EVT_PKT_CMD_STATUS_OFFSET] !=
            STATUS_SUCCESS) {
            printf("Loopback enable failed with return value: %d\n",
                hci_lb_core.pkt_type_bt.rx_pkt_buffer[HCI_EVT_PKT_CMD_STATUS_OFFSET]);
            retval = STATUS_ERROR;
        }
        else {
            /* Loopback enable success */
            hci_lb_core.lb_enable_status = LB_STATUS_ENABLED;
        }
    }
    pthread_mutex_unlock(&hci_lb_core.lb_enable_lock);
    return retval;
}

void lb_frame_hci_pkt(uint8 *hciPkt, uint8 hciPktType, uint16 length, uint16 value)
{
    int nIndex = 0;
    uint8 offset = 0;

    if (hciPkt == NULL)
        return;

    hciPkt[HCI_PKT_IND_OFFSET]     = hciPktType;
    hciPkt[HCI_PKT_IND_OFFSET + 1] = (value & 0xFF);
    hciPkt[HCI_PKT_IND_OFFSET + 2] = ((value & 0xFF00) >> 8);

    /* Length is 1 bytes for CMD pkts and 2 bytes for ACL pkts */
    if ((hciPktType == BT_CMD_PKT_ID) || ((hciPktType == FM_CMD_PKT_ID)))
    {
        hciPkt[HCI_CMD_PKT_LEN_OFFSET]    = (uint8) (length & 0xFF);
        offset = HCI_PKT_IND_SIZE + HCI_CMD_PKT_HDR_SIZE;
    }
    else if (hciPktType == BT_ACL_DATA_PKT_ID)
    {
        hciPkt[HCI_ACL_PKT_LENGTH_OFFSET]     = (uint8) (length & 0xFF);
        hciPkt[HCI_ACL_PKT_LENGTH_OFFSET + 1] = (uint8) ((length & 0xFF00) >> 8);
        offset = HCI_PKT_IND_SIZE + HCI_ACL_PKT_HDR_SIZE;
    }

    /* Create random payload for CMD/ACL pkt */
    for(nIndex = 0; nIndex < length ; nIndex++)
        hciPkt[nIndex + HCI_PKT_IND_OFFSET + offset] = (rand() % MAX_PKT_BYTE);
}

void lb_log_txrx_stats()
{
    char filename_with_path[255];
    FILE *error_log_fd;

    /* Protect access to the error log file */
    pthread_mutex_lock(&hci_lb_core.lb_error_log_file_lock);
    error_log_fd = fopen(filename_with_path, "w+");
    if (error_log_fd == NULL)
    {
        printf("%s: Failed to create the log file\n", __func__);
        pthread_mutex_unlock(&hci_lb_core.lb_error_log_file_lock);
        return;
    }
    /* Log the success and failure count of BT/FM/ANT Sub-systems */
    if (hci_lb_core.pkt_type_bt.lb_enabled) {
        fprintf(error_log_fd,"\n\nBT-CMD: SENT : \t%lu\tFAILED : %lu\n",
            hci_lb_core.pkt_type_bt.cmd_pkt_success,
            hci_lb_core.pkt_type_bt.cmd_pkt_failure);
    }

    if (hci_lb_core.pkt_type_fm.lb_enabled) {
        fprintf(error_log_fd,"FM-CMD: SENT : \t%lu\tFAILED : %lu\n",
            hci_lb_core.pkt_type_fm.cmd_pkt_success,
            hci_lb_core.pkt_type_fm.cmd_pkt_failure);
    }
#ifdef HCI_LOOPBACK_ANT
    if (hci_lb_core.pkt_type_ant.lb_enabled) {
        fprintf(error_log_fd,"ANT-CMD: SENT : \t%lu\tFAILED : %lu\n",
            hci_lb_core.pkt_type_ant.cmd_pkt_success,
            hci_lb_core.pkt_type_ant.cmd_pkt_failure);
    }
#endif
    fclose(error_log_fd);
    pthread_mutex_unlock(&hci_lb_core.lb_error_log_file_lock);
}

void lb_log_pkt_stats(uint8 pktType, uint8 *txPktBuffer, uint8 *rx_pkt_buffer)
{
    char buffer[26];
    char filename_with_path[255];
    int pktIndex = -1;
    uint16 txPktLen = 0, rxPktLen = 0;
    FILE *error_log_fd;

    /* Log the timestamp when the mismatch occurred */
    time(&hci_lb_core.time_stamp);
    hci_lb_core.tm_info = localtime(&hci_lb_core.time_stamp);
    strftime(buffer, 26, "%Y:%m:%d %H:%M:%S", hci_lb_core.tm_info);

    /* Protect access to the error log file */
    pthread_mutex_lock(&hci_lb_core.lb_error_log_file_lock);
    error_log_fd = fopen(filename_with_path, "w+");
    if (error_log_fd == NULL) {
        pthread_mutex_unlock(&hci_lb_core.lb_error_log_file_lock);
        return;
    }
    switch (pktType) {
    case BT_CMD_PKT_ID:
    case FM_CMD_PKT_ID:
#ifdef HCI_LOOPBACK_ANT
    case ANT_CMD_PKT_ID:
#endif
        fprintf(error_log_fd,"\n\nHCI CMD PKT TYPE: %d ERROR: \n", pktType);
        fprintf(error_log_fd,"Time Stamp: %s \n",buffer);
        if (txPktBuffer)
            txPktLen = (uint8) (txPktBuffer[HCI_CMD_PKT_LEN_OFFSET] +
                            HCI_PKT_IND_SIZE + HCI_CMD_PKT_HDR_SIZE);
        if (rx_pkt_buffer)
            rxPktLen = (uint8) (rx_pkt_buffer[HCI_EVT_PKT_LEN_OFFSET] +
                            HCI_PKT_IND_SIZE + HCI_EVT_PKT_HDR_SIZE);
        break;
    case BT_ACL_DATA_PKT_ID:
        fprintf(error_log_fd,"\n\nHCI ACL PKT TYPE ERROR: \n");
        fprintf(error_log_fd,"Time Stamp: %s \n",buffer);
        if (txPktBuffer)
            txPktLen = (((txPktBuffer[HCI_ACL_PKT_LENGTH_OFFSET + 1] << 8) |
                     txPktBuffer[HCI_ACL_PKT_LENGTH_OFFSET]) +
                     HCI_PKT_IND_SIZE + HCI_ACL_PKT_HDR_SIZE);
        if (rx_pkt_buffer)
            rxPktLen = (((rx_pkt_buffer[HCI_ACL_PKT_LENGTH_OFFSET + 1] << 8) |
                     rx_pkt_buffer[HCI_ACL_PKT_LENGTH_OFFSET]) +
                     HCI_PKT_IND_SIZE + HCI_ACL_PKT_HDR_SIZE);
        break;
    default:
        printf("Invalid HCI PKT TYPE\n");
    }

    /* Log the complete packet sent and received, including pkt header info */
    fprintf(error_log_fd,"HCI-PKT SENT: \n");
    for(pktIndex = 0; pktIndex < txPktLen; pktIndex++)
        fprintf(error_log_fd, "0x%x ", txPktBuffer[pktIndex]);

    fprintf(error_log_fd,"\n\nLOOPBACK HCI PKT RECVD: \n");
    for(pktIndex = 0; pktIndex < rxPktLen; pktIndex++)
        fprintf(error_log_fd, "0x%x ", rx_pkt_buffer[pktIndex]);

    /* Log the success and failure count of BT/FM/ANT Sub-systems */
    if (hci_lb_core.pkt_type_bt.lb_enabled) {
        fprintf(error_log_fd,"\n\nBT-CMD: SENT : \t%lu\tFAILED : %lu\n",
            hci_lb_core.pkt_type_bt.cmd_pkt_success,
            hci_lb_core.pkt_type_bt.cmd_pkt_failure);
    }

    if (hci_lb_core.pkt_type_fm.lb_enabled) {
        fprintf(error_log_fd,"FM-CMD: SENT : \t%lu\tFAILED : %lu\n",
            hci_lb_core.pkt_type_fm.cmd_pkt_success,
            hci_lb_core.pkt_type_fm.cmd_pkt_failure);
    }
#ifdef HCI_LOOPBACK_ANT
    if (hci_lb_core.pkt_type_ant.lb_enabled) {
        fprintf(error_log_fd,"ANT-CMD: SENT : \t%lu\tFAILED : %lu\n",
            hci_lb_core.pkt_type_ant.cmd_pkt_success,
            hci_lb_core.pkt_type_ant.cmd_pkt_failure);
    }
#endif
    fclose(error_log_fd);
    pthread_mutex_unlock(&hci_lb_core.lb_error_log_file_lock);
}

int lb_pkt_comparator(uint8 pktType, uint8 *txPktBuffer, uint8 *rx_pkt_buffer)
{
    int retval = STATUS_SUCCESS;
    int pktIndex = -1;
    uint16 txPayloadOffset = 0, rxPayloadOffset = 0, rxPktLen = 0;

    if (!txPktBuffer || !rx_pkt_buffer)
        return -1;

    /* Based on PKT ID determine offset from where comparisson has to begin*/
    if ((pktType == BT_CMD_PKT_ID) || (pktType == FM_CMD_PKT_ID)) {
        /* Check if the transmitted HCI CMD LEN matches with the received PKT */
        if ((rx_pkt_buffer[HCI_EVT_PKT_LEN_OFFSET] - HCI_CMD_PKT_HDR_SIZE) !=
            txPktBuffer[HCI_CMD_PKT_LEN_OFFSET]) {
            retval = STATUS_ERROR;
            goto log_packet_stats;
        }

       txPayloadOffset = HCI_CMD_PKT_OPCODE_OFFSET;
       rxPayloadOffset = HCI_LB_EVT_PKT_PARAM_OFFSET;

       /*
        * LB-EVT contains CMD-OPCODE & CMD-PARAM-LEN along with CMD PARAMS
        *     TX-CMD-HDR     : 01 06 04 05
        *     TX-CMD-PAYLOAD : 3e 28 07 9f 70
        *     RX-EVT-HDR     : 04 19 08
        *     RX-EVT-PAYLOAD : 06 04 05 3e 28 07 9f 70
        * Thus, the LB EVT length will be 3 bytes more than the CMD Length
        */
       rxPktLen = rx_pkt_buffer[HCI_EVT_PKT_LEN_OFFSET];
    } else /* if (pktType == BT_ACL_DATA_PKT_ID) */ {
        /* Check if transmitted HCI ACL LEN matches with the received PKT */
        if (((rx_pkt_buffer[HCI_ACL_PKT_LENGTH_OFFSET + 1 ] << 8) |
            rx_pkt_buffer[HCI_ACL_PKT_LENGTH_OFFSET]) !=
            ((txPktBuffer[HCI_ACL_PKT_LENGTH_OFFSET + 1 ] << 8) |
            txPktBuffer[HCI_ACL_PKT_LENGTH_OFFSET])) {
            retval = STATUS_ERROR;
            goto log_packet_stats;
        }

       /*
        * LB-ACL contains ACL-HANDLE & ACL-LEN along with ACL PAYLOAD
        *     TX-ACL-HDR     : 02 01 00 06 00
        *     TX-ACL-PAYLOAD : A1 B2 C3 D4 E5 F6
        *     RX-ACL-HDR     : 02 01 00 06 00
        *     RX-EVT-PAYLOAD : A1 B2 C3 D4 E5 F6
        * Thus, the TX & RX ACL PKTs are the same.
        */
       txPayloadOffset = rxPayloadOffset = HCI_PKT_IND_OFFSET;
       rxPktLen = ((rx_pkt_buffer[HCI_ACL_PKT_LENGTH_OFFSET + 1 ] << 8) |
                    rx_pkt_buffer[HCI_ACL_PKT_LENGTH_OFFSET]);
       rxPktLen += (HCI_PKT_IND_SIZE + HCI_ACL_PKT_HDR_SIZE);
    }

    /* Compare byte-by-byte the transmitted & received packets */
    for(pktIndex = 0; pktIndex < rxPktLen; pktIndex++) {
        if(txPktBuffer[txPayloadOffset + pktIndex] !=
            rx_pkt_buffer[rxPayloadOffset + pktIndex]) {
            ALOGE("ERROR: TX-RX PACKET MISMATCH: txPktBuffer[%d] : "
                "%d---rx_pkt_buffer[%d]: %d",
                pktIndex, txPktBuffer[pktIndex], pktIndex,
                rx_pkt_buffer[pktIndex]);
            retval = STATUS_ERROR;
            break;
        }
    }

log_packet_stats:
    switch(pktType) {
    case BT_CMD_PKT_ID:
    case BT_ACL_DATA_PKT_ID:
        if (retval == STATUS_SUCCESS) {
            hci_lb_core.pkt_type_bt.cmd_pkt_success++;
            ALOGE("BT-HCI-PKT: LOOPBACK SUCCESS COUNT\t: %lu",
                hci_lb_core.pkt_type_bt.cmd_pkt_success);
#ifdef LOG_SUCCESS_STATS
            lb_log_txrx_stats();
#endif
        } else {
            hci_lb_core.pkt_type_bt.cmd_pkt_failure++;
            ALOGE("BT-HCI-PKT: LOOPBACK FAILURE COUNT\t: %lu",
                hci_lb_core.pkt_type_bt.cmd_pkt_failure);
            /* Log the TX and RX packets in case of failure */
            lb_log_pkt_stats(pktType, txPktBuffer, rx_pkt_buffer);
        }
        break;
    case FM_CMD_PKT_ID:
        if (retval == STATUS_SUCCESS) {
            hci_lb_core.pkt_type_fm.cmd_pkt_success++;
            ALOGE("FM-HCI-PKT: LOOPBACK SUCCESS COUNT\t: %lu",
                hci_lb_core.pkt_type_fm.cmd_pkt_success);
#ifdef LOG_SUCCESS_STATS
            lb_log_txrx_stats();
#endif
        } else {
            hci_lb_core.pkt_type_fm.cmd_pkt_failure++;
            ALOGE("FM-HCI-PKT: LOOPBACK FAILURE COUNT\t: %lu",
                hci_lb_core.pkt_type_fm.cmd_pkt_failure);
            /* Log the TX and RX packets in case of failure */
            lb_log_pkt_stats(pktType, txPktBuffer, rx_pkt_buffer);
        }
        break;
#ifdef HCI_LOOPBACK_ANT
    case ANT_CMD_PKT_ID:
        if (retval == STATUS_SUCCESS) {
            hci_lb_core.pkt_type_ant.cmd_pkt_success++;
            ALOGE("ANT-HCI-PKT: LOOPBACK SUCCESS COUNT\t: %lu",
                hci_lb_core.pkt_type_ant.cmd_pkt_success);
#ifdef LOG_SUCCESS_STATS
            lb_log_txrx_stats();
#endif
        } else {
            hci_lb_core.pkt_type_bt.cmd_pkt_failure++;
            ALOGE("ANT-HCI-PKT: LOOPBACK FAILURE COUNT\t: %lu",
                hci_lb_core.pkt_type_ant.cmd_pkt_failure);
            /* Log the TX and RX packets in case of failure */
            lb_log_pkt_stats(pktType, txPktBuffer, rx_pkt_buffer);
        }
        break;
#endif
    default:
        printf("Invalid packet type. Should not be reaching here!!\n");
        retval = STATUS_ERROR;
    }
    return retval;
}

#define UART_BT_BUF_SIZE 1024
static void *process_BT_read_write_to_uart(void *arg)
{
    static uint8 pktType = BT_ACL_DATA;
    static uint8 cmdType = BT_SHORT_CMD ;
    uint16 length = 0;
    int retval = -1;
    uint8 pkt_sequence;
    wdsdaemon *wds = (wdsdaemon *) arg;
    struct timespec ts;

    /*
     * 'pkt_sequence' is a flah used to determine how the validator should send
     * the HCI traffic to Controller.
     * If the number of packets to be transmitted is divisible by 10, then we
     * send 6 ACL packets followed by 4 CMD packeys.
     * Else, we alternate between sending ACL & CMD packet one after another.
     */
    if (hci_lb_core.pkt_type_bt.max_pkt_lb_cnt % 10 == 0)
        pkt_sequence = ACL_6P_CMD_4P;
    else
        pkt_sequence = ACL_1P_CMD_1P;

    /* Enable HCI Loopback functionality */
    if (lb_functionality_enable(wds) != STATUS_SUCCESS)
        goto bt_loopback_exit;

    /*
     * Step1: Send BT: CMD and ACL packets alternately to the Controller.
     * Step2: Wait for the packet to be loop backed by the Controller.
     * Step3: Compare the packet sent and the one that is loop backed
     * Step4: If there is any discrepancy, then save the info to the
     *        error log file and then trigger SSR
     * Step5: If the loopback is successful, then continue if the
     *        loopback mode is continuous, else break if the no. of packets
     *        exceeds the max. loopback packet count.
     */
    do
    {
        if(hci_lb_core.lb_exec_type == LB_TYPE_MAX_PKT_CNT)
            hci_lb_core.pkt_type_bt.max_pkt_lb_cnt --;

        /* Alternate sending between ACL and CMD packets */
        if(pktType == BT_COMMAND) {
            srand(time(NULL));
            if (cmdType == BT_SHORT_CMD)
            {
                lb_frame_hci_pkt(&hci_lb_core.pkt_type_bt.tx_pkt_buffer[HCI_PKT_IND_OFFSET], BT_CMD_PKT_ID,
                    (rand() % BT_SHORT_CMD_SIZE), HCI_INQUIRY);
                cmdType = BT_LONG_CMD;
            }
            else
            {
                lb_frame_hci_pkt(&hci_lb_core.pkt_type_bt.tx_pkt_buffer[HCI_PKT_IND_OFFSET], BT_CMD_PKT_ID,
                    (rand() % BT_LONG_CMD_SIZE), HCI_CHANGE_LOCAL_NAME);
                cmdType = BT_SHORT_CMD;
            }
            printf("\n\nSENDING BT-CMD-PKT: PAYLOAD LENGTH: 0x%x (%d) bytes\n",
                hci_lb_core.pkt_type_bt.tx_pkt_buffer[HCI_CMD_PKT_LEN_OFFSET],
                hci_lb_core.pkt_type_bt.tx_pkt_buffer[HCI_CMD_PKT_LEN_OFFSET]);

        } else /* if (pktType == BT_ACL_DATA) */ {
            srand(time(NULL));
            lb_frame_hci_pkt(&hci_lb_core.pkt_type_bt.tx_pkt_buffer[0], BT_ACL_DATA_PKT_ID,
                (rand() % BT_ACL_PKT_MAX_SIZE), DEFAULT_ACL_HANDLE);

            printf("\n\nSENDING BT-ACL-PKT: PAYLOAD LENGTH: 0x%x (%d) bytes\n",
                ((hci_lb_core.pkt_type_bt.tx_pkt_buffer[HCI_ACL_PKT_LENGTH_OFFSET + 1] << 8) |
                  hci_lb_core.pkt_type_bt.tx_pkt_buffer[HCI_ACL_PKT_LENGTH_OFFSET]),
                ((hci_lb_core.pkt_type_bt.tx_pkt_buffer[HCI_ACL_PKT_LENGTH_OFFSET + 1] << 8) |
                  hci_lb_core.pkt_type_bt.tx_pkt_buffer[HCI_ACL_PKT_LENGTH_OFFSET]));
        }

        /* Wait for the Controller to loop back the sent packet */
        pthread_mutex_lock(&hci_lb_core.pkt_type_bt.tx_q_lock);

        retval = hci_lb_core.pkt_type_bt.valid_signal = 0;

        /* If, SSR is in progress, then abstain from sending further packets */
        if (lb_is_ssr_in_prog()) {
            pthread_mutex_unlock(&hci_lb_core.pkt_type_bt.tx_q_lock);
            ALOGE("SSR in progress: Do not send further BT packets");
            break;
        } else /* Send the command */
            process_pc_data_to_soc(wds, &hci_lb_core.pkt_type_bt.tx_pkt_buffer[0], wds->soc_if.uart.bt_fd);

        ts = lb_pkt_timeout();
        while((hci_lb_core.pkt_type_bt.valid_signal == 0) && (retval != ETIMEDOUT)){
            retval = pthread_cond_timedwait(&hci_lb_core.pkt_type_bt.event_cond,
                &hci_lb_core.pkt_type_bt.tx_q_lock, &ts);
        }
        if (retval == ETIMEDOUT) {
            ALOGE("Timeout waiting for loopback event for below BT CMD");
            ALOGE("BT-CMD-HDR : %2x %2x %2x %2x",
            hci_lb_core.pkt_type_bt.tx_pkt_buffer[HCI_PKT_IND_OFFSET],
            hci_lb_core.pkt_type_bt.tx_pkt_buffer[HCI_CMD_PKT_OPCODE_OFFSET],
            hci_lb_core.pkt_type_bt.tx_pkt_buffer[HCI_CMD_PKT_OPCODE_OFFSET + 1],
            hci_lb_core.pkt_type_bt.tx_pkt_buffer[HCI_CMD_PKT_LEN_OFFSET]);
            lb_log_pkt_stats(hci_lb_core.pkt_type_bt.tx_pkt_buffer[HCI_PKT_IND_OFFSET], hci_lb_core.pkt_type_bt.tx_pkt_buffer, NULL);
            pthread_mutex_unlock(&hci_lb_core.pkt_type_bt.tx_q_lock);
            goto bt_trigger_ssr;
        } else if (retval != 0) {
            printf("Wait for BT Loopback Event failed with return value: %d\n",
                retval);
            pthread_mutex_unlock(&hci_lb_core.pkt_type_bt.tx_q_lock);
            goto bt_loopback_exit;
        }
        pthread_mutex_unlock(&hci_lb_core.pkt_type_bt.tx_q_lock);

        /* Compare byte-by-byte the transmitted and received packets */
        if (lb_pkt_comparator(pktType, &hci_lb_core.pkt_type_bt.tx_pkt_buffer[HCI_PKT_IND_OFFSET],
            &hci_lb_core.pkt_type_bt.rx_pkt_buffer[HCI_PKT_IND_OFFSET]) !=
            STATUS_SUCCESS) {
            ALOGE("###BT HCI PKT MISMATCH###\n");
            goto bt_trigger_ssr;
        }

        if (pkt_sequence == ACL_6P_CMD_4P) {
            if (((hci_lb_core.pkt_type_bt.max_pkt_lb_cnt % 10) > 4) ||
                ((hci_lb_core.pkt_type_bt.max_pkt_lb_cnt % 10) == 0))
                pktType = BT_ACL_DATA;
            else
                pktType = BT_COMMAND;
        } else {
            if(pktType == BT_COMMAND)
                pktType = BT_ACL_DATA;
            else 
                pktType = BT_COMMAND;
        }

        /* If system suspend is enabled then wait for sometime prior to sending
           the next BT HCI PKT */
        if(hci_lb_core.lb_suspend_enable) {
            /* Random sleep: This is to allow the system to enter into
               suspend state */
            if (rand()%5)
                sleep(HCI_INTRA_PKT_DELAY);
        }
        /*
         * In case of sys. suspend disabled, wait for a min. time prior to
         * sending the next BT HCI PKT. This is to ensure the other thread(s)
         * get a chance to run
         */
#ifdef SLEEP_ENABLE
        else
            usleep(CTXT_SWITCH_DELAY);
#endif

    }while(hci_lb_core.pkt_type_bt.max_pkt_lb_cnt != 0);

    if (retval != STATUS_SUCCESS) {
bt_trigger_ssr:
        if (!lb_is_ssr_in_prog()) {
            ALOGE("BT: Triggering SSR");
            lb_trigger_ssr(wds);
        }
    }

bt_loopback_exit:
    if (!lb_is_ssr_in_prog())
        lb_log_txrx_stats();
    printf("BT thread exiting!!!\n");
    pthread_exit(NULL);
    return 0;
}


#define UART_FM_BUF_SIZE 256
static void *process_FM_read_write_to_uart(void *arg)
{
    uint8 pktType = FM_CMD_PKT_ID;
    uint16 length = 0;
    int retval = -1;
    unsigned int nIndex = 0;
    wdsdaemon *wds = (wdsdaemon *) arg;
    struct timespec ts;

    /* Enable HCI Loopback functionality */
    if (lb_functionality_enable(wds) != STATUS_SUCCESS)
        goto fm_loopback_exit;

    /*
     * Step1: Send FM CMD packets to the Controller.
     * Step2: Wait for the packet to be loop backed by the Controller.
     * Step3: Compare the packet sent and the one that is loop backed
     * Step4: If there is any discrepancy, then save the info to the
     *        error log file and then trigger SSR
     * Step5: If the loopback is successful, then continue if the
     *        loopback mode is continuous, else break if the no. of packets
     *        exceeds the max. loopback packet count.
     */

    do
    {
        if(hci_lb_core.lb_exec_type == LB_TYPE_MAX_PKT_CNT)
            hci_lb_core.pkt_type_fm.max_pkt_lb_cnt --;

        srand(time(NULL));
        lb_frame_hci_pkt(&hci_lb_core.pkt_type_fm.tx_pkt_buffer[HCI_PKT_IND_OFFSET], FM_CMD_PKT_ID,
            (rand() % FM_CMD_PKT_MAX_SIZE), HCI_FM_RX_ENABLE);

        printf("\n\nSENDING FM-CMD: PAYLOAD LENGTH: 0x%x (%d) bytes\n",
            hci_lb_core.pkt_type_fm.tx_pkt_buffer[3], hci_lb_core.pkt_type_fm.tx_pkt_buffer[3]);

        /* Wait for the Controller to loop back the sent packet */
        pthread_mutex_lock(&hci_lb_core.pkt_type_fm.tx_q_lock);

        retval = hci_lb_core.pkt_type_fm.valid_signal = 0 ;

        /* If, SSR is in progress, then abstain from sending further packets */
        if (lb_is_ssr_in_prog()) {
            pthread_mutex_unlock(&hci_lb_core.pkt_type_fm.tx_q_lock);
            ALOGE("SSR in progress: Do not send further FM packets");
            break;
        } else /* Send the command */
            process_pc_data_to_soc(wds, &hci_lb_core.pkt_type_fm.tx_pkt_buffer[0], wds->soc_if.uart.fm_fd);

        ts = lb_pkt_timeout();
        while(hci_lb_core.pkt_type_fm.valid_signal == 0){
            retval = pthread_cond_timedwait(&hci_lb_core.pkt_type_fm.event_cond,
                &hci_lb_core.pkt_type_fm.tx_q_lock, &ts);
        }
        if (retval == ETIMEDOUT) {
            ALOGE("Timeout waiting for loopback event for below FM CMD");
            ALOGE("FM-CMD-HDR : %2x %2x %2x %2x",
            hci_lb_core.pkt_type_fm.tx_pkt_buffer[HCI_PKT_IND_OFFSET],
            hci_lb_core.pkt_type_fm.tx_pkt_buffer[HCI_CMD_PKT_OPCODE_OFFSET],
            hci_lb_core.pkt_type_fm.tx_pkt_buffer[HCI_CMD_PKT_OPCODE_OFFSET + 1],
            hci_lb_core.pkt_type_fm.tx_pkt_buffer[HCI_CMD_PKT_LEN_OFFSET]);
            lb_log_pkt_stats(hci_lb_core.pkt_type_fm.tx_pkt_buffer[HCI_PKT_IND_OFFSET], hci_lb_core.pkt_type_fm.tx_pkt_buffer, NULL);
            pthread_mutex_unlock(&hci_lb_core.pkt_type_fm.tx_q_lock);
            goto fm_trigger_ssr;
        } else if (retval != 0) {
            ALOGE("Wait for FM Loopback Event failed with return value: %d",
                retval);
            pthread_mutex_unlock(&hci_lb_core.pkt_type_fm.tx_q_lock);
            goto fm_loopback_exit;
        }
        pthread_mutex_unlock(&hci_lb_core.pkt_type_fm.tx_q_lock);

        /* Compare byte-by-byte the transmitted and received packets */
        if (lb_pkt_comparator(pktType, &hci_lb_core.pkt_type_fm.tx_pkt_buffer[HCI_PKT_IND_OFFSET],
            &hci_lb_core.pkt_type_fm.rx_pkt_buffer[HCI_PKT_IND_OFFSET]) !=
            STATUS_SUCCESS) {
            goto fm_trigger_ssr;
        }

        /* If system suspend is enabled then wait for sometime prior to sending
           the next BT HCI PKT */
        if (hci_lb_core.lb_suspend_enable) {
            /* Random sleep: This is to allow the system to enter into
               suspend state */
            if (rand()%5)
                sleep(HCI_INTRA_PKT_DELAY);
        }
        /*
         * In case of sys. suspend disabled, wait for a min. time prior to
         * sending the next BT HCI PKT. This is to ensure the other thread(s)
         * get a chance to run
         */
#ifdef SLEEP_ENABLE
        else
            usleep(CTXT_SWITCH_DELAY);
#endif

    }while(hci_lb_core.pkt_type_fm.max_pkt_lb_cnt != 0);

    if (retval != STATUS_SUCCESS) {
fm_trigger_ssr:
        if (!lb_is_ssr_in_prog()) {
            ALOGE("FM: Triggering SSR..........");
            lb_trigger_ssr(wds);
        }
    }

fm_loopback_exit:
    if (!lb_is_ssr_in_prog())
        lb_log_txrx_stats();
    printf("FM thread exiting!!\n");
    pthread_exit(NULL);
    return 0;
}

int get_user_input(int read_option)
{
    static int index = 0;
    int option = 0;
    char opt[10];

    if(READ_FROM_CONSOLE == read_option) {
        fgets(opt, sizeof(opt), stdin);
        option = strtol(opt, NULL, 10);
    } else { /* if (READ_FROM_FILE == read_option) */
        fscanf(hci_lb_core.lb_cfg_file_fd,"%d", &option);
        printf("\nOption Read from file: %d\n",option);
    }
    return option;
}

int lb_process_menu_options(uint8 lb_pkt_type)
{
    switch(lb_pkt_type) {
    case LOOPBACK_MODE_BT:
        hci_lb_core.pkt_type_bt.lb_enabled = 1;
        break;
    case LOOPBACK_MODE_FM:
        hci_lb_core.pkt_type_fm.lb_enabled = 1;
        break;
    case LOOPBACK_MODE_BT_FM:
        hci_lb_core.pkt_type_bt.lb_enabled = 1;
        hci_lb_core.pkt_type_fm.lb_enabled = 1;
        break;
#ifdef HCI_LOOPBACK_ANT
    case LOOPBACK_MODE_ANT:
        hci_lb_core.pkt_type_ant.lb_enabled = 1;
        break;
    case LOOPBACK_MODE_BT_ANT:
        hci_lb_core.pkt_type_bt.lb_enabled = 1;
        hci_lb_core.pkt_type_ant.lb_enabled = 1;
        break;
    case LOOPBACK_MODE_FM_ANT:
        hci_lb_core.pkt_type_fm.lb_enabled = 1;
        hci_lb_core.pkt_type_ant.lb_enabled = 1;
        break;
    case LOOPBACK_MODE_BT_FM_ANT:
        hci_lb_core.pkt_type_bt.lb_enabled = 1;
        hci_lb_core.pkt_type_fm.lb_enabled = 1;
        hci_lb_core.pkt_type_ant.lb_enabled = 1;
        break;
#endif /* ifdef HCI_LOOPBACK_ANT */
    default:
        printf("Wrong option selected. Please try again...\n");
        return -1;
        break;
    }
    return 0;
}

void lb_process_suspend_option(uint8 choice)
{
    hci_lb_core.lb_suspend_enable = (choice == 0) ? 0 : 1;
    if (hci_lb_core.lb_suspend_enable)
        printf("Suspend disabled\n");
    else
        printf("Suspend enabled\n");
}

void lb_create_log_file(void)
{
    char buffer[26];
    char filename_with_path[255];
    FILE *error_log_fd;

    time(&hci_lb_core.time_stamp);
    hci_lb_core.tm_info = localtime(&hci_lb_core.time_stamp);
    strftime(buffer, 26, "%Y.%m.%d_%H.%M.%S", hci_lb_core.tm_info);
    snprintf(filename_with_path, sizeof(filename_with_path),
                    "%s_%s.txt", ERROR_LOGING_FILE_PATH, buffer);
    printf("Log file name: %s\n",filename_with_path);

    error_log_fd = fopen(filename_with_path, "w+");
    if (error_log_fd == NULL)
    {
        printf("Failed to create the log file : %s\n",strerror(errno));
        return;
    }
    else
    {
        fprintf(error_log_fd,"File Name: %s\n", filename_with_path);
        fprintf(error_log_fd,"Time Stamp: %s\n", buffer);
    }
    fclose(error_log_fd);
}

void lb_process_max_packets_option(wdsdaemon *wds, int choice)
{
    int option = 0;

    if(choice == 1)
    {
        hci_lb_core.lb_exec_type = 1;
        printf("Enter the Max. No. of packets to be loop backed: ");
        hci_lb_core.pkt_type_bt.max_pkt_lb_cnt = hci_lb_core.pkt_type_fm.max_pkt_lb_cnt = \
           hci_lb_core.pkt_type_ant.max_pkt_lb_cnt = get_user_input(wds->bg_process);
    } else
        hci_lb_core.lb_exec_type = 0;

    if(hci_lb_core.pkt_type_bt.lb_enabled)
    {
        printf("Enter the Firmware loopback delay for BT (mSec): ");
        hci_lb_core.pkt_type_bt.pkt_delay = get_user_input(wds->bg_process);
    }
    if(hci_lb_core.pkt_type_fm.lb_enabled)
    {
        printf("Enter the Firmware loopback delay for FM (mSec): ");
        hci_lb_core.pkt_type_fm.pkt_delay = get_user_input(wds->bg_process);
    }
    if(hci_lb_core.pkt_type_ant.lb_enabled)
    {
        printf("Enter the Firmware loopback delay for ANT (mSec): ");
        hci_lb_core.pkt_type_ant.pkt_delay = get_user_input(wds->bg_process);
    }
}

int lb_start_hci_threads(wdsdaemon *wds)
{
    if (wds->mode == MODE_LOOPBACK_UART) {
        if(hci_lb_core.pkt_type_bt.lb_enabled) {
            if (pthread_create(&hci_lb_core.pkt_type_bt.loopback_rw_thread, NULL,
                process_BT_read_write_to_uart, wds) != 0) {
                ERROR("%s:Unable to create pthread err = %s", __func__,
                    strerror(errno));
                return STATUS_ERROR;
            }
        }

        if(hci_lb_core.pkt_type_fm.lb_enabled) {
            if (pthread_create(&hci_lb_core.pkt_type_fm.loopback_rw_thread, NULL,
                process_FM_read_write_to_uart, wds) != 0) {
                ERROR("%s:Unable to create pthread err = %s", __func__,
                    strerror(errno));
                return STATUS_ERROR;
            }
        }

#ifdef HCI_LOOPBACK_ANT
        if(hci_lb_core.pkt_type_ant.lb_enabled) {
            if (pthread_create(&hci_lb_core.pkt_type_ant.loopback_rw_thread, NULL,
                process_ANT_read_write_to_uart, wds) != 0) {
                ERROR("%s:Unable to create pthread err = %s", __func__,
                    strerror(errno));
                return STATUS_ERROR;
            }
        }
#endif
    } else {
        ERROR("%s: Invalid mode for loopback!", __func__);
        return STATUS_ERROR;
    }

    /* Wait for the threads to complete */
    if(hci_lb_core.pkt_type_bt.lb_enabled)
        pthread_join(hci_lb_core.pkt_type_bt.loopback_rw_thread, NULL);

    if(hci_lb_core.pkt_type_fm.lb_enabled)
        pthread_join(hci_lb_core.pkt_type_fm.loopback_rw_thread, NULL);

#ifdef HCI_LOOPBACK_ANT
    if(hci_lb_core.pkt_type_ant.lb_enabled)
        pthread_join(hci_lb_core.pkt_type_ant.loopback_rw_thread, NULL);
#endif

    return STATUS_SUCCESS;
}

void lbInitResources(hci_loopback_core_struct *ptr_hci_lb_core)
{
    /* Initialize core structure resources*/
    memset(ptr_hci_lb_core, 0, sizeof(hci_loopback_core_struct));
    pthread_mutex_init(&hci_lb_core.lb_enable_lock, NULL);
    pthread_mutex_init(&hci_lb_core.lb_error_log_file_lock, NULL);

    /* BT resources */
    ptr_hci_lb_core->pkt_type_bt.max_pkt_lb_cnt = 1;
    ptr_hci_lb_core->pkt_type_bt.tx_pkt_buffer =
        (uint8 *) (calloc(BT_ACL_DATA_PKT_LEN, sizeof(char)));
    ptr_hci_lb_core->pkt_type_bt.rx_pkt_buffer =
        (uint8 *) (calloc(UART_BUF_SIZE, sizeof(char)));
    pthread_mutex_init(&hci_lb_core.pkt_type_bt.tx_q_lock, NULL);
    pthread_mutex_init(&hci_lb_core.pkt_type_bt.rx_q_lock, NULL);
    pthread_cond_init(&hci_lb_core.pkt_type_bt.event_cond, NULL);

    /* FM resources */
    ptr_hci_lb_core->pkt_type_fm.max_pkt_lb_cnt = 1;
    ptr_hci_lb_core->pkt_type_fm.tx_pkt_buffer =
        (uint8 *) (calloc(FM_HCI_COMMAND_PKT_LEN, sizeof(char)));
    ptr_hci_lb_core->pkt_type_fm.rx_pkt_buffer =
        (uint8 *) (calloc(UART_BUF_SIZE, sizeof(char)));
    pthread_mutex_init(&hci_lb_core.pkt_type_fm.tx_q_lock, NULL);
    pthread_mutex_init(&hci_lb_core.pkt_type_fm.rx_q_lock, NULL);
    pthread_cond_init(&hci_lb_core.pkt_type_fm.event_cond, NULL);

    /* ANT resources */
    ptr_hci_lb_core->pkt_type_ant.max_pkt_lb_cnt = 1;
    ptr_hci_lb_core->pkt_type_ant.tx_pkt_buffer =
        (uint8 *) (calloc(ANT_ACL_DATA_PKT_LEN, sizeof(char)));
    ptr_hci_lb_core->pkt_type_ant.rx_pkt_buffer =
        (uint8 *) (calloc(UART_BUF_SIZE, sizeof(char)));
    pthread_mutex_init(&hci_lb_core.pkt_type_ant.tx_q_lock, NULL);
    pthread_mutex_init(&hci_lb_core.pkt_type_ant.rx_q_lock, NULL);
    pthread_cond_init(&hci_lb_core.pkt_type_ant.event_cond, NULL);
}

void lb_deinit(hci_loopback_core_struct *ptr_hci_lb_core)
{
    printf("%s: Freeing the allocated resources..\n", __func__);

    if (ptr_hci_lb_core->pkt_type_bt.tx_pkt_buffer) {
        free(ptr_hci_lb_core->pkt_type_bt.tx_pkt_buffer);
        ptr_hci_lb_core->pkt_type_bt.tx_pkt_buffer = NULL;
    }
    if (ptr_hci_lb_core->pkt_type_bt.rx_pkt_buffer) {
        free(ptr_hci_lb_core->pkt_type_bt.rx_pkt_buffer);
        ptr_hci_lb_core->pkt_type_bt.rx_pkt_buffer = NULL;
    }
    if (ptr_hci_lb_core->pkt_type_fm.tx_pkt_buffer) {
        free(ptr_hci_lb_core->pkt_type_fm.tx_pkt_buffer);
        ptr_hci_lb_core->pkt_type_fm.tx_pkt_buffer = NULL;
    }
    if (ptr_hci_lb_core->pkt_type_fm.rx_pkt_buffer) {
        free(ptr_hci_lb_core->pkt_type_fm.rx_pkt_buffer);
        ptr_hci_lb_core->pkt_type_fm.rx_pkt_buffer = NULL;
    }
    if (ptr_hci_lb_core->pkt_type_ant.tx_pkt_buffer) {
        free(ptr_hci_lb_core->pkt_type_ant.tx_pkt_buffer);
        ptr_hci_lb_core->pkt_type_ant.tx_pkt_buffer = NULL;
    }
    if (ptr_hci_lb_core->pkt_type_ant.rx_pkt_buffer) {
        free(ptr_hci_lb_core->pkt_type_ant.rx_pkt_buffer);
        ptr_hci_lb_core->pkt_type_ant.rx_pkt_buffer = NULL;
    }

    memset(ptr_hci_lb_core, 0, sizeof(hci_loopback_core_struct));
    pthread_mutex_init(&hci_lb_core.lb_enable_lock, NULL);
    pthread_mutex_init(&hci_lb_core.lb_error_log_file_lock, NULL);
}

int init_loopback_test_mode(wdsdaemon *wds)
{
    int ret = 0;
    int choice = 0;
    char filename_with_path[255];

    /* Initialize the Loopback related data structures */
    lbInitResources(&hci_lb_core);

    /* Check if the loopback has to be done in the background process*/
    if(READ_FROM_FILE == wds->bg_process)
    {
        hci_lb_core.lb_cfg_file_fd = fopen(INPUT_FILE_NAME, "r");
        if (hci_lb_core.lb_cfg_file_fd == NULL) {
            printf("Failed to open the config file : %s\n",strerror(errno));
            return -1;
        }
    }

    /* Preference: packet types to be loop backed */
MENU_OPTIONS:
    printf("%s", HCI_packets_to_test_menu_options);

    hci_lb_core.lb_pkt_type = get_user_input(wds->bg_process);
    if (lb_process_menu_options(hci_lb_core.lb_pkt_type) < 0)
        goto MENU_OPTIONS;

    /* Create a log file to track the packet statistics */
    lb_create_log_file();

    /* Preference: In non-continuous mode no. of packets to loopback */
    printf("%s", maximum_packets_menu);
    choice = get_user_input(wds->bg_process);
    lb_process_max_packets_option(wds, choice);

    if(READ_FROM_FILE == wds->bg_process)
        fclose(hci_lb_core.lb_cfg_file_fd);

    /* Start the BT/FM/ANT threads to pump the traffic */
    ret = lb_start_hci_threads(wds);

    return ret;
}
