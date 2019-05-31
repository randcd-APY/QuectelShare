/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <getopt.h>
#include <signal.h>

#include "malloc.h"
#include "qapi_firmware_upgrade_ext.h"
#include "qapi_timer.h"
#include "qapi_types.h"
#include "qapi_status.h"
#include "qapi_qsHost.h"
#include "qsPack.h"

#define SWAP16(a) ((uint16_t)((((a)>>8) & 0xFF) | (((a)&0xFF)<<8)))
#define SWAP32(a) (SWAP16((((a)>>16))&0xFFFF) | (SWAP16((a)&0xFFFF) << 16))

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define LE2CPU32(a)   (a)
#else
#define LE2CPU32(a)   SWAP32(a)
#endif

#ifndef ANDROID
#define IOTD_NAME    "iotd"
#else
#define IOTD_NAME    "iotd_qz"
#endif
#define IOTD_INI     "/etc/iotd_config.ini"

#define MSGQ_MSG_SIZE                                   (2000)
#define MSGQ_MAX_NUM_MSG                                (15)
#define PATH_MAX                                        (256)

/*-------------------------------------------------------------------------
 * Function Definitions
 *-----------------------------------------------------------------------*/
static int Command_Display_FWD(void *arg)
{
	int32_t Index;
	uint32_t boot_type, fwd_present;

	/* get active FWD */
	Index = qapi_Fw_Upgrade_Get_Active_FWD(&boot_type, &fwd_present);
	fprintf(stderr, "Active FWD: %s  index:%d, present:%d\n",
		(boot_type == QAPI_FW_UPGRADE_FWD_BOOT_TYPE_TRIAL) ?
		"Trial" : (boot_type == QAPI_FW_UPGRADE_FWD_BOOT_TYPE_CURRENT) ?
		"Current" : "Golden", Index, fwd_present);


	return 0;
}

static int Command_Done_Trial(void *arg)
{
	int ret = 0;
	int trail = *(int *) arg;

	if (trail > 1) {
		fprintf(stderr, "Invalid trail value %d\n", trail);
		return -1;
	}

	if (trail == 1) {
		if (qapi_Fw_Upgrade_Done(1, 0) != QAPI_FW_UPGRADE_OK_E) {
			fprintf(stderr, "Fail to Accept Trial FWD\n");
			ret = -1;
		} else
			fprintf(stderr, "Success to Accept Trial FWD\n");
	} else {
		if (qapi_Fw_Upgrade_Done(0, 0) != QAPI_FW_UPGRADE_OK_E) {
			fprintf(stderr, "Fail to Reject Trial FWD\n");
			ret = -1;
		} else
			fprintf(stderr, "Success to Reject Trial FWD\n");
	}

	return ret;
}

static int Command_Fw_Upgrade_Fwupgrade(void *arg)
{
#define BUF_SIZE 892		//note: need check qcli buf size (1024 now)
	qapi_Fw_Upgrade_Status_Code_t fwup_status;
	qapi_Fw_Upgrade_State_t fwup_state;
	uint8_t *data = NULL;
	int fd = -1, data_size, len;
	uint32_t stage, bytes_written;
	uint32_t flags = 0;

	const char *file_name = (const char *) arg;
	int32_t Index;
	uint32_t boot_type, fwd_present;
	stage = 0;

	Index = qapi_Fw_Upgrade_Get_Active_FWD(&boot_type, &fwd_present);

	/* Reject old trail image if any. */
        if (boot_type == QAPI_FW_UPGRADE_FWD_BOOT_TYPE_TRIAL) {
		fprintf(stderr, "There's old Trial image, accept or reject it first\n");
		return -1;
        }

	/* open image file */
	if ((fd = open(file_name, O_RDONLY)) == -1) {
		fprintf(stderr, "fail to open %s\n", file_name);
		goto fw_start_on_error;
	}

	/* allocate memory */
	data_size = BUF_SIZE;
	if ((data = (uint8_t *) malloc(data_size)) == NULL) {
		fprintf(stderr, "ERROR: Memory allocation failed\n");
		goto fw_start_on_error;
	}

//  if (!firmware_version_check(version))
//      goto err;

	stage = 1;
	/* Init firmware upgrade session */
	fwup_status = qapi_Fw_Upgrade_Host_Init(flags);
	if (fwup_status != QAPI_FW_UPGRADE_OK_E) {
		fprintf(stderr, "fail to init firmware upgrade: %d\n", fwup_status);
		goto fw_start_on_error;
	}

	stage = 1;

	bytes_written = 0;
	fprintf(stderr, "\nstart");
	while ((len = read(fd, data, data_size)) != 0) {	/* read data from image file */
		bytes_written += len;

		fprintf(stderr, ".");

		/* send image data to quartz */
		fwup_status = qapi_Fw_Upgrade_Host_Write(data, len);
		if (fwup_status != QAPI_FW_UPGRADE_OK_E) {
			fprintf(stderr, "\nfail to write: %d\n", fwup_status);
			goto fw_start_on_error;
		}
	}

	fprintf(stderr, "\nsend %s with %d bytes\n", file_name, (int) bytes_written);

	/* check firmware upgrade status */
	while (qapi_Fw_Upgrade_Get_Status() == QAPI_FW_UPGRADE_OK_E) {
		/* check firmware upgrade state */
		fwup_state = qapi_Fw_Upgrade_Get_State();
		if (fwup_state == QAPI_FW_UPGRADE_STATE_FINISH_E) {
			fprintf(stderr, "Firmware Upgrade is done.\n");
			goto fw_start_on_error;
		}
		sleep(1);
	}

	fprintf(stderr, "fail to do firmware upgrade: (state:%d, status:%d)\n", fwup_state,
		fwup_status);

      fw_start_on_error:
	if (stage == 1)
		qapi_Fw_Upgrade_Host_Deinit();

	if (data != NULL)
		free(data);

	if (fd >= 0)
		close(fd);

	if (QAPI_FW_UPGRADE_STATE_FINISH_E != fwup_state)
		return -1;

	return 0;
}

static int Command_Fw_Upgrade_Info(void *arg)
{

	fprintf(stderr, "state: %d, status:%d\n",
		qapi_Fw_Upgrade_Get_State(), qapi_Fw_Upgrade_Get_Status());

	return 0;
}

static int firmware_version_check(uint32_t version)
{
	qapi_FW_Ver_t info;

	if (qapi_Get_FW_Ver(&info) == QAPI_OK) {
		if (version > info.crm_Build_Number)
			return 1;

		fprintf(stderr, "The image version is old, current version is %d\n",
			info.crm_Build_Number);
	} else {
		fprintf(stderr, "ERROR, can't get current image version.\n");
	}

	return 0;
}

typedef int (*fw_op_t) (void *);

int run_command(fw_op_t op, void *arg)
{
	int ret = 0;

	pid_t parent = getpid();
	pid_t pid = fork();
	if (pid == -1) {
		return -1;
	} else if (pid > 0) {
		int status;
		sleep(2);
		if (qapi_Qs_Init("/iotdq", MSGQ_MSG_SIZE, MSGQ_MAX_NUM_MSG)) {
			fprintf(stderr, "qapi_Qs_Init failed\n");
			return -1;
		}
		ret = (*op) (arg);
		qapi_Qs_DeInit();
		kill(pid, SIGKILL);
		waitpid(pid, &status, 0);
	} else {
		// we are the child
		char *newargv[] = { NULL, IOTD_INI, NULL };
		char *newenviron[] = { NULL };
		newargv[0] = IOTD_NAME;
		execve(IOTD_NAME, newargv, newenviron);
	}
	return ret;
}

static void usage(void)
{
	fprintf(stderr,
		"\nusage: qca_mgr_daemon [-hsd] [-p <image path>] [-v <version>] [-t <1/0>]\n"
		"options:\n" "   -h --help        show this usage\n"
		"   -s --status      show state and status\n"
		"   -d --display     display active FWD\n"
		"   -t --trail [1/0] accept/reject the trail image \n"
		"   -p --path path   specify image path\n"
		"   -v --ver version specify image version\n");
	exit(1);
}

const struct option options_long[] = {
	{"help", 0, NULL, 'h'},
	{"status", 0, NULL, 's'},
	{"display", 0, NULL, 'd'},
	{"path", 1, NULL, 'p'},
	{"trail", 1, NULL, 't'},
	{"path", 1, NULL, 'p'},
	{"ver", 1, NULL, 'v'},
	{NULL, 0, NULL, 0}
};

int main(int argc, char *argv[])
{
	int opt;
	const char *image = NULL;
	int version = 0;
	int trail_flag;

	if (argc < 2)
		usage();

	while (1) {
		opt = getopt_long(argc, argv, "hsdp:t:v:", options_long, NULL);
		if (opt < 0)
			break;

		switch (opt) {
		case 'h':
			usage();
			break;
		case 's':
			return run_command(Command_Fw_Upgrade_Info, NULL);
		case 'd':
			return run_command(Command_Display_FWD, NULL);
		case 'p':
			image = optarg;
			break;
		case 't':
			trail_flag = atoi(optarg);
			return run_command(Command_Done_Trial, (void *) &trail_flag);
		case 'v':
			version = atoi(optarg);
			break;
		default:
			usage();
		}
	}

	if (image == NULL || !version) {
		fprintf(stderr, "Please specify correct image path and version!\n");
		usage();
	}

	if (access(image, F_OK)) {
		fprintf(stderr, "Invalid image path %s\n", image);
		exit(1);
	}

	if (run_command(Command_Fw_Upgrade_Fwupgrade, (void *) image)) {
		fprintf(stderr, "fwupgrade failed\n");
		return -1;
	}

	unlink(image);

	return 0;
}
