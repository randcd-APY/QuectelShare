/*
 * Copyright (C) 2013-2014,2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * qti_system_daemon.h : Header file for qti-system-daemon
 */
#include "msg.h"
#include "diag_lsm.h"
#include "diagpkt.h"
#include "diagcmd.h"
#ifndef ANDROID
#include "common.h"
#endif
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "QTISysDaemon"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/reboot.h>
#include <sys/syscall.h>
#include <linux/reboot.h>

/* FTM MODE ID 75 11 53*/
#define FTM_FFBM_CMD_CODE	53
#define EDL_RESET_CMD_CODE	1

#define MISC_PARTITION_LOCATION_ALT "/dev/block/platform/msm_sdcc.1/by-name/misc"
#define MISC_PARTITION_LOCATION "/dev/block/bootdevice/by-name/misc"

#define MODE_FFBM "ffbm-01"
#define MODE_NORMAL "normal"

#define RET_SUCCESS 0
#define RET_FAILED 1
#define SOS_FIFO "data/misc/qsysdaemon"
#define SOS_FIFO_FOLDER "data/misc"

#define REBOOT_CMD "reboot"
#define EDL_REBOOT_CMD "edl-reboot"
#define FFBM_MODE_CMD "ffbm"

#define FFBM_COMMAND_BUFFER_SIZE 20


typedef enum
{
	FTM_FFBM_SET_MODE   =     0,
	FTM_FFBM_GET_MODE   =     1,
	FTM_FFBM_SET_REGIONAL_PACK =    2,
	FTM_FFBM_GET_REGIONAL_PACK =    3,
	FTM_FFBM_HANDLE_PDC =    4
}FFBM_CMD_CODE;

typedef enum
{
	FTM_FFBM_SUCCESS = 0,
	FTM_FFBM_FAIL = 1
}FTM_ERROR_CODE;

typedef enum
{
	BOOT_MODE_HLOS = 0,
	BOOT_MODE_FFBM = 1
}BOOT_MODE;


typedef enum
{
	MODE_OFFLINE_A_F = 0,         //!<' Go to offline analog
	MODE_OFFLINE_D_F = 1,         //!<' Go to offline digital
	MODE_RESET_F = 2,             //!<' Reset. Only exit from offline
	MODE_FTM_F = 3,               //!<' FTM mode
	MODE_ONLINE_F = 4,            //!<' Go to Online
	MODE_LPM_F = 5,               //!<' Low Power Mode (if supported)
	MODE_POWER_OFF_F = 6,         //!<' Power off (if supported)
	MODE_MAX_F = 7                //!<' Last (and invalid) mode enum value
} mode_enum_type;

typedef PACKED struct
{
	diagpkt_cmd_code_type              cmd_code;
	uint16                             mode;
}__attribute__((packed))mode_change_pkt_type;

typedef PACKED struct
{
	diagpkt_cmd_code_type              cmd_code;
	diagpkt_subsys_id_type             subsys_id;
	diagpkt_subsys_cmd_code_type       subsys_cmd_code;
	uint16                             ffbm_cmd_code;
	uint16                             reserved;
	uint16                             reserved1;
}__attribute__((packed))ffbm_pkt_type;


/* FFBM Set Mode command request packet */
typedef PACKED struct
{
	ffbm_pkt_type        ftm_header;
	uint8                iNextBootMode;
	uint8                iNextBootSubMode;
}__attribute__((packed))ffbm_set_mode_req_type;

/* FFBM Set Mode command respond packet */
typedef PACKED struct
{
	ffbm_pkt_type        ftm_header;
	uint16               iFTM_Error_Code;
}__attribute__((packed))ffbm_set_mode_rsq_type;

/* FFBM Get Mode command request packet */
typedef PACKED struct
{
	ffbm_pkt_type        ftm_header;
	uint16               iFTM_Error_Code;
	uint8                iCurrentBootMode;
	uint8                iCurrentBootSubMode;
	uint8                iNextBootMode;
	uint8                iNextBootSubMode;
}__attribute__((packed))ffbm_get_mode_rsq_type;


void * ffbm_dispatch(ffbm_pkt_type *ffbm_pkt);
void * ffbm_get_mode();
void * ffbm_set_mode(ffbm_set_mode_req_type *ffbm_pkt);
