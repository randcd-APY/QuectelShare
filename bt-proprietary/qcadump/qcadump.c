/*
 *
 *  qcadump - Captures vendor specific events for firmware crash
 *
 *  Copyright (c) 2015 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <sys/socket.h>
#include <stdint.h>
#include <time.h>
#include "qcadump.h"

static int log_dumped;

static unsigned char *buffer;

static unsigned char *temp_buf;
static unsigned char *q, *pbase;
static long int total_len;

static void dump_event_packet(struct frame *frm, int file_fd)
{
	unsigned char *pkt = frm->ptr;
	int num;
	long int wlen = 0;
	static uint16_t eseq_n;
	uint32_t seq_n = pkt[5] | (pkt[6] << 8);

	num = frm->len;

	/* In case controller dropps packet(s) we would
	 * skip that packet (256 bytes) filling up with
	 * zero. As the buffer has cleared with 0
	 * (with memset()) we just have to increment
	 * the pointer.
	 */
	for (; (seq_n > eseq_n) && (seq_n != LAST_SEQUENCE_NUM); eseq_n++) {
		printf("packet(s) dropped, current sequence %u, expected seq %u\n",
			seq_n, eseq_n);
		temp_buf += VENDOR_PKT_SIZE - VENDOR_OTHER_PKT_HDR_SIZE;
		total_len += VENDOR_PKT_SIZE - VENDOR_OTHER_PKT_HDR_SIZE;
	}
	/*
	 * Subtracting 12 bytes header from first packet
	 * and 8 bytes from second packet onwards, first
	 * packet shall contain 4 bytes extra dump size
	 */
	if (seq_n == 0x0000) {
		memcpy(temp_buf, pkt + VENDOR_FIRST_PKT_HDR_SIZE,
					num - VENDOR_FIRST_PKT_HDR_SIZE);
		temp_buf += num - VENDOR_FIRST_PKT_HDR_SIZE;
		total_len += num - VENDOR_FIRST_PKT_HDR_SIZE;
	} else {
		memcpy(temp_buf, pkt + VENDOR_OTHER_PKT_HDR_SIZE,
					num - VENDOR_OTHER_PKT_HDR_SIZE);
		temp_buf += num - VENDOR_OTHER_PKT_HDR_SIZE;
		total_len += num - VENDOR_OTHER_PKT_HDR_SIZE;
	}

	if (LAST_SEQUENCE_NUM == seq_n) {
		while (total_len != 0 && (wlen = write(file_fd, pbase, total_len)) != 0) {
			if (wlen == -1) {
				if (errno == EINTR)
					continue;
				perror("write() error");
				break;
			}
			total_len -= wlen;
			pbase += wlen;
		}
		log_dumped = 1;
	}
	eseq_n++;
}

static void process_event_packet(struct frame *frm, int file_fd)
{
	unsigned char *pkt = frm->ptr;

	if (!frm->len)
		return;

	switch (pkt[0]) {
	case BT_COMMAND_PKT:
	case BT_ACLDATA_PKT:
	case BT_SCODATA_PKT:
	case BT_VENDOR_PKT:
		break;

	case BT_EVENT_PKT:
		/* we are only interested in vendor specific
		 * event packets (0xff) with controller log
		 * packet (0x01) and memory dump (0x08). So
		 * expected packet byte sequence is:
		 * 04 FF XX 01 08 ...
		 */
		if (pkt[1] == 0xff) {
			if (pkt[3] == 0x01 && pkt[4] == 0x08)
				dump_event_packet(frm, file_fd);
		}
		break;

	default:
		printf("Invalid packet type\n");
	}
}

static int handle_event_packets(int sock_fd, int file_fd)
{
	struct frame frm;
	char *buf = NULL;
	int len;
	fd_set readfds;

	FD_ZERO(&readfds);
	FD_SET(sock_fd, &readfds);

	buf = malloc(MAX_PKT_LEN);
	if (!buf) {
		perror("Can't allocate data buffer");
		return -1;
	}

	memset(buf, 0, MAX_PKT_LEN);

	frm.ptr = buf;

	/* currently this tool handles only one crash dump
	 * at a time. We need to manually run the qcadump
	 * for the next log once crash occurs
	 */
	while (!log_dumped) {

		if (select(sock_fd + 1, &readfds, NULL, NULL, NULL) < 1) {
			perror("select() failed:");
			free(buf);
			return -errno;
		}

		if (FD_ISSET(sock_fd, &readfds)) {

			len = recv(sock_fd, buf, MAX_PKT_LEN, MSG_DONTWAIT);
			if (len < 0) {
				if (errno == EAGAIN || errno == EINTR)
					continue;
				perror("recv() failed:");
				free(buf);
				return -errno;
			}
		}

		frm.len = len;

		process_event_packet(&frm, file_fd);
	}

	free(buf);
	return 0;
}

static int open_crashdump_file(void)
{
	int fd;
	time_t rawtime;
	struct tm *timeinfo;
	char file_path[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(file_path, 80, "bt_fw_crashdump-%b%d-%H%M.bin", timeinfo);

	fd = open(file_path, O_CREAT | O_SYNC | O_WRONLY,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd < 0) {
		perror("Can't open dump file");
		return -errno;
	}

	return fd;
}

static int open_hci_socket(void)
{
	struct sockaddr_hci sa;
	struct hci_filter filter;
	int sk;
	int dev = 0;

	sk = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
	if (sk < 0) {
		perror("socket() error");
		return -errno;
	}

	memset((void *) &filter, 0, sizeof(filter));
	memset((void *) &filter.type_mask, 0xff, sizeof(filter.type_mask));
	memset((void *) filter.event_mask, 0xff, sizeof(filter.event_mask));
	if (setsockopt(sk, SOL_HCI, HCI_FILTER, &filter, sizeof(filter)) < 0) {
		perror("setsockopt() error");
		close(sk);
		return -errno;
	}
	memset(&sa, 0, sizeof(sa));
	sa.hci_family = AF_BLUETOOTH;

	/* dev represents controller ID */
	sa.hci_dev = dev;
	if (bind(sk, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
		perror("bind() error");
		close(sk);
		return -errno;
	}

	return sk;
}

int main(void)
{
	int sock_fd;
	int file_fd;

	buffer = (unsigned char *)malloc(MAX_BUF_SIZE);
	memset(buffer, 0, MAX_BUF_SIZE);
	temp_buf = pbase = q = buffer;

	printf("HCI sniffer: Bluetooth packet analyzer ver 1.0\n");

	sock_fd = open_hci_socket();
	if (sock_fd < 0)
		goto cleanup;

	file_fd = open_crashdump_file();
	if (file_fd < 0)
		goto cleanup;

	handle_event_packets(sock_fd, file_fd);

cleanup:
	if (q != NULL)
		free(q);

	return 0;
}
