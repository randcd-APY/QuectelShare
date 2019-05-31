/*
 * qti-system-daemon: A module to handle commands(diag) that can be processed
 * in native code.
 *
 * Copyright (C) 2013-2014,2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * qti_system_daemon.c : Main implementation of qti-system-daemon
 */
#include <poll.h>
#include "qti_system_daemon.h"

PACK(void *) ffbm_diag_reset_handler(PACK(void *)req_pkt, uint16 pkt_len);
PACK(void *) ffbm_diag_mode_handler(PACK(void *)req_pkt, uint16 pkt_len);
PACK(void *) reboot_to_edl(PACK(void *)req_pkt, uint16 pkt_len);

static int get_mode( ffbm_get_mode_rsq_type *pkt);
/*Table for commands handled only in ffbm mode*/
static const diagpkt_user_table_entry_type ftm_ffbm_mode_table[] =
{
	{DIAG_CONTROL_F, DIAG_CONTROL_F, ffbm_diag_reset_handler}
};

/*Table for commands handled in ffbm and normal mode*/
static const diagpkt_user_table_entry_type ftm_table[] =
{
	{FTM_FFBM_CMD_CODE, FTM_FFBM_CMD_CODE, ffbm_diag_mode_handler}
};

static const diagpkt_user_table_entry_type system_operations_table[] =
{
	{EDL_RESET_CMD_CODE, EDL_RESET_CMD_CODE, reboot_to_edl}
};

int qti_system_daemon_pipe[2];

PACK(void *) ffbm_diag_reset_handler(PACK(void *)req_pkt, uint16 pkt_len)
{
	void *rsp_pkt;
        mode_change_pkt_type *req = (mode_change_pkt_type*)req_pkt;

	rsp_pkt = diagpkt_alloc(DIAG_CONTROL_F, pkt_len);

	if (rsp_pkt)
		memcpy(rsp_pkt, req_pkt, pkt_len);
	else {
		ALOGE("diagpkt_alloc failed");
		return rsp_pkt;
	}

        ALOGI("%s received diag command[%d,%d]",__FUNCTION__,req->cmd_code,req->mode);
	/**Only reboot when reset command received*/
	if (req->mode == MODE_RESET_F){
            ALOGI("%s received diag reset command, start to reboot!",__FUNCTION__);
	    if (write(qti_system_daemon_pipe[1],
				REBOOT_CMD,
				sizeof(REBOOT_CMD)) < 0) {
		ALOGE("Failed to write to pipe: %s", strerror(errno));
		goto error;
	    }
	    close(qti_system_daemon_pipe[1]);
	}
error:
	return rsp_pkt;
}

PACK(void *) ffbm_diag_mode_handler(PACK(void *)req_pkt, uint16 pkt_len)
{
	PACK(void*)rsp = NULL;
	ALOGI("In ffbm_diag_mode_handler");
	rsp = ffbm_dispatch((ffbm_pkt_type *)req_pkt);
	return rsp;
}

void * ffbm_dispatch(ffbm_pkt_type *ffbm_pkt)
{
	ffbm_set_mode_rsq_type *rsp;
	if (ffbm_pkt->ffbm_cmd_code == FTM_FFBM_SET_MODE)
		rsp = (ffbm_set_mode_rsq_type *)ffbm_set_mode((ffbm_set_mode_req_type*)ffbm_pkt);
	else if (ffbm_pkt->ffbm_cmd_code == FTM_FFBM_GET_MODE)
		rsp = (ffbm_set_mode_rsq_type *)ffbm_get_mode();
	else
		rsp = NULL;
	return (void*)rsp;
}

#define MMC_DIR "/sys/block/mmcblk0/"
static int get_partition_by_name(char *name, char *path)
{
	char uevent[256];
	char *dev_name = NULL;
	bool match = false;
	char *p;
	long nr_part = 0;
	int i;
	int j;
	FILE *fp;
	char part_name[64];

	if ((fp = fopen(MMC_DIR"uevent", "r")) == NULL) {
		ALOGE("Fail to open %s: %s", MMC_DIR"uevent",
				strerror(errno));
		return -1;
	}

	while(!feof(fp)) {
		if(fgets(uevent, sizeof(uevent), fp)) {
			if(p = strstr(uevent, "NPARTS=")) {
				nr_part = strtol(p + strlen("NPARTS="), NULL, 0);
				break;
			}
		}
	}
	fclose(fp);

	if (!nr_part) {
		ALOGE("invalid partition number");
		return -1;
	}

	for (i = 1; i <= nr_part; i++) {
		sprintf(path, "%smmcblk0p%d/uevent", MMC_DIR, i);
		if ((fp = fopen(path, "r")) == NULL) {
			ALOGE("Fail to open %s: %s", path,
					strerror(errno));
			continue;
		}

		while(!feof(fp)) {
			if (fgets(uevent, sizeof(uevent), fp)) {
				if (p = strstr(uevent, "PARTNAME=")) {
					p = p + strlen("PARTNAME=");
					p[strlen(p) - 1] = 0;
					if (strcmp(p, name) == 0)
						match = true;
				} else if (p = strstr(uevent, "DEVNAME=")) {
					dev_name = p + strlen("DEVNAME=");
					dev_name[strlen(dev_name) - 1] = 0;
					sprintf(path, "/dev/%s", dev_name);
				}
			}
		}
		fclose(fp);

		if (match) {
			ALOGD("The path of %s is %s", name, path);
			return 0;
		}
	}

	ALOGD("Can't find %s partition", name);
	return -1;
}

static int set_mode(const char* mode)
{
	int num_bytes = 0;
	unsigned int write_count = 0;
	int fd = -1;
	char path[256];

	fd = open(MISC_PARTITION_LOCATION, O_RDWR);
	if (fd < 0) {
		fd = open(MISC_PARTITION_LOCATION_ALT, O_RDWR);
		if (fd < 0) {
			if (!get_partition_by_name("misc", path))
				fd = open(path, O_RDWR);

			if (fd < 0) {
				ALOGE("Error locating/opening misc partion: %s",
					strerror(errno));
				goto error;
			}
		}
	}
	do {
		num_bytes = write(fd, mode + write_count/sizeof(char),
				strlen(mode) - write_count);
		if (num_bytes < 0) {
			ALOGE("Failed to write to partition");
			goto error;
		}
		write_count+=num_bytes;
	} while(write_count < strlen(mode));
	num_bytes = write(fd, '\0', 2);
	close(fd);
	return RET_SUCCESS;
error:
	if (fd >= 0)
		close(fd);
	return RET_FAILED;
}

static int get_mode( ffbm_get_mode_rsq_type *pkt)
{
	int bytes_read = 0;
	int fd = -1;
	int offset = 0;
	char buffer[FFBM_COMMAND_BUFFER_SIZE];
	char cmdline[512];
	FILE *fp;
	char path[256];

	if (!pkt) {
		ALOGE("Invalid argument to get_mode");
		goto error;
	}

	pkt->iCurrentBootMode = BOOT_MODE_HLOS;
	if ((fp = fopen("/proc/cmdline", "r")) == NULL)
		goto error;
	while(!feof(fp)) {
		if(fgets(cmdline, sizeof(cmdline), fp)) {
			if(strstr(cmdline, MODE_FFBM)) {
				pkt->iCurrentBootMode = BOOT_MODE_FFBM;
				ALOGD("Current bootmode is FFBM");
				break;
			}
		}
	}
	fclose(fp);

	fd = open(MISC_PARTITION_LOCATION, O_RDWR);
	if (fd < 0) {
		fd = open(MISC_PARTITION_LOCATION_ALT, O_RDWR);
		if (fd < 0) {
			if (!get_partition_by_name("misc", path))
				fd = open(path, O_RDWR);

			if (fd < 0) {
				ALOGE("Error locating/opening misc partion: %s",
					strerror(errno));
				goto error;
			}
		}
	}
	memset(buffer,'\0', sizeof(buffer));
	do {
		bytes_read = read(fd, buffer + offset,
				(FFBM_COMMAND_BUFFER_SIZE-1) - offset);
		if (bytes_read < 0) {
			ALOGE("Failed to read from misc partition");
			goto error;
		}
		offset += bytes_read;
	} while (bytes_read > 0 && offset < (FFBM_COMMAND_BUFFER_SIZE - 1));

	if (!strncmp(buffer, MODE_FFBM, sizeof(MODE_FFBM))) {
		ALOGI("Next mode: ffbm");
		pkt->iNextBootMode = BOOT_MODE_FFBM;
	} else {
		ALOGI("Next mode: hlos");
		pkt->iNextBootMode = BOOT_MODE_HLOS;
	}
	return 0;
error:
	if (fd >= 0)
		close(fd);
	return -1;
}


void *ffbm_set_mode(ffbm_set_mode_req_type *ffbm_pkt)
{
	ffbm_set_mode_rsq_type *rsp;
	rsp = (ffbm_set_mode_rsq_type*)diagpkt_subsys_alloc( DIAG_SUBSYS_FTM,
		FTM_FFBM_CMD_CODE,
		sizeof(ffbm_set_mode_rsq_type));
	if (!rsp) {
		ALOGE("Failed to allocate response packet");
		return rsp;
	}
	if (ffbm_pkt->iNextBootMode == BOOT_MODE_FFBM) {
		ALOGI("Setting bootmode to FFBM");
		if (set_mode(MODE_FFBM) != RET_SUCCESS) {
			ALOGE("Failed to set bootmode");
			goto error;
		}
	} else if (ffbm_pkt->iNextBootMode == BOOT_MODE_HLOS) {
		ALOGI("Setting bootmode to Normal mode");
		if (set_mode(MODE_NORMAL) != RET_SUCCESS) {
			ALOGE("Failed to set bootmode");
			goto error;
		}
	} else {
		ALOGI("Unknown boot mode recieved");
		goto error;
	}
	rsp->iFTM_Error_Code = FTM_FFBM_SUCCESS;
	return (void*)rsp;
error:
	rsp->iFTM_Error_Code = FTM_FFBM_FAIL;
	return (void*)rsp;
}

void *ffbm_get_mode()
{
	   ffbm_get_mode_rsq_type *rsp;
	   int rcode = -1;
	   ALOGI("In ffbm_get_mode");
	   rsp = (ffbm_get_mode_rsq_type*)diagpkt_subsys_alloc( DIAG_SUBSYS_FTM,
			   FTM_FFBM_CMD_CODE,
			   sizeof(ffbm_get_mode_rsq_type));
	   if (!rsp) {
		   ALOGE("Failed to allocate response packet");
	   } else {
		   rcode = get_mode(rsp);
		   if (rcode) {
			   ALOGE("Failed to get boot mode info");
			   rsp->iFTM_Error_Code = FTM_FFBM_FAIL;
		   } else
			   rsp->iFTM_Error_Code = FTM_FFBM_SUCCESS;
		   /*Boot sub modes are 0 for LA*/
		   rsp->iCurrentBootSubMode = 0;
		   rsp->iNextBootSubMode = 0;
	   }
	   return(void *) rsp;
}

void *reboot_to_edl(PACK(void *)req_pkt, uint16 pkt_len)
{
	void *rsp_pkt = NULL;
	ALOGI("reboot to edl command recieved");
	rsp_pkt = diagpkt_alloc(EDL_RESET_CMD_CODE, pkt_len);
	if (rsp_pkt)
		memcpy(rsp_pkt, req_pkt, pkt_len);
	else {
		ALOGE("diagpkt_alloc failed");
		goto error;
	}
	if (write(qti_system_daemon_pipe[1],
				EDL_REBOOT_CMD,
				sizeof(EDL_REBOOT_CMD)) < 0) {
		ALOGE("Failed to write command to pipe: %s",
				strerror(errno));
		goto error;
	}
	close(qti_system_daemon_pipe[1]);
	ALOGI("returning response packet");
error:
	return rsp_pkt;
}


int main()
{
    int binit_Success = 0;
    int bytes_read = 0;
    int offset = 0;
    char buffer[FFBM_COMMAND_BUFFER_SIZE];
    struct pollfd fds[2];
    char buf[10];
    int ret, nfds = 1;
    int is_factory = 0;
    const char* reboot_arg;

    ALOGI("Starting qti system daemon");
    memset(buffer, '\0', sizeof(buffer));
    if (pipe(qti_system_daemon_pipe) < 0) {
        ALOGE("Failed to create pipe: %s",strerror(errno));
        return -1;
    }
    if(NULL == opendir(SOS_FIFO_FOLDER)){
        mkdir(SOS_FIFO_FOLDER, 0700);
        ALOGE("Failed to create SOS_FIFO_FOLDER: %s",
                strerror(errno));
    }
    mode_t mode = 0600 | S_IFIFO;
    if (mknod(SOS_FIFO, mode, 0) < 0) {
        if (errno != EEXIST) {
            ALOGE("Failed to create node: %s",
                    strerror(errno));
                return -1;
        }
    }

    binit_Success = Diag_LSM_Init(NULL);

    if (!binit_Success) {
        ALOGE(" Diag_LSM_Init failed : %d\n",binit_Success);
        close(qti_system_daemon_pipe[0]);
        close(qti_system_daemon_pipe[1]);
        return -1;
    }

    DIAGPKT_DISPATCH_TABLE_REGISTER(DIAG_SUBSYS_FTM ,
                    ftm_table);
    DIAGPKT_DISPATCH_TABLE_REGISTER(DIAG_SUBSYS_SYSTEM_OPERATIONS,
                    system_operations_table);

    ALOGI("Registering mode reset handler");
    DIAGPKT_DISPATCH_TABLE_REGISTER(DIAGPKT_NO_SUBSYS_ID,
                    ftm_ffbm_mode_table);
    do {
        fds[0].fd = qti_system_daemon_pipe[0];
        fds[0].events = POLLIN;
        fds[0].revents = 0;
        if (is_factory) {
            int fifo;
            if ((fifo = open(SOS_FIFO, O_RDONLY | O_NONBLOCK)) > 0) {
                fds[1].fd = fifo;
                fds[1].events = POLLIN;
                fds[1].revents = 0;
                nfds = 2;
            }
        }
        // start polling on fds
        ret = poll(fds, nfds, -1);
        if (ret < 0) {
            ALOGE("Failed to poll: %s", strerror(errno));
            goto error;
        }
        if ((fds[0].revents & POLLIN)) {
            do {
                bytes_read = read(qti_system_daemon_pipe[0],
                        buffer + offset,
                        (FFBM_COMMAND_BUFFER_SIZE-1) - offset);
                if (bytes_read < 0) {
                    ALOGE("Failed to read command from pipe : %s",
                        strerror(errno));
                    goto error;
                }
                offset += bytes_read;
            } while (bytes_read > 0 && offset <
                (FFBM_COMMAND_BUFFER_SIZE - 1));

            if (!strncmp(buffer, REBOOT_CMD, sizeof(REBOOT_CMD))) { // reboot command
                sync();
                sleep(1);
                reboot_arg = "";
                ret = syscall(__NR_reboot, LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2,
                                LINUX_REBOOT_CMD_RESTART2, reboot_arg);
            } else if (!strncmp(buffer, EDL_REBOOT_CMD,            //  reboot by edl-reboot
                sizeof(EDL_REBOOT_CMD))) {
                sync();
                sleep(1);
                reboot_arg = "edl";
                ret = syscall(__NR_reboot, LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2,
                                LINUX_REBOOT_CMD_RESTART2, reboot_arg);
            }
        }
error:
        ALOGE("Unrecognised command");
        bytes_read = 0;
        offset = 0;
    } while(1);
    return 0;
}
