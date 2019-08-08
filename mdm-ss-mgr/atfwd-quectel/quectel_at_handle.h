/********************************************************************************
 *      Copyright:  (C) 2019 quectel
 *                  All rights reserved.
 *
 *       Filename:  quectel_at_quectel.h
 *    Description:  This is head of quectel_at_quectel.cpp file. 
 *
 *        Version:  1.0.0(2019年03月19日)
 *         Author:  Peeta Chen <peeta.chen@quectel.com>
 *      ChangeLog:  1, Release initial version on "2019年03月19日 14时41分32秒"
 *                 
 ********************************************************************************/
#ifndef __QUECTEL_AT_QUECTEL_H__
#define __QUECTEL_AT_QUECTEL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <poll.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>

#define QUECTEL_QGMR_CMD
#define QUECTEL_QAPSUB_CMD
#define QUECTEL_QDEVINFO_CMD
#define QUECTEL_QAPCMD_CMD

#define LOGI(...)
#define LOGW(...)
#define LOGE(...)

/**
 * RESP_BUF_SIZE msut less than the QMI_ATCOP_AT_RESP_MAX_LEN in the Filename
 * vendor/qcom/proprietary/qmi/inc/qmi_atcop_srvc.h
 */
#define RESP_BUF_SIZE (380 * 2)

typedef struct {
    int opcode;
    char *name;
    int ntokens;
    char **tokens;
} AtCmd;

typedef struct {
    int result;
    char *response;
}AtCmdResponse;

void quec_qgmr_handle(const AtCmd *cmd, AtCmdResponse *response);
void quec_qapsub_handle(const AtCmd *cmd, AtCmdResponse *response);
void quec_qdevinfo_handle(const AtCmd *cmd, AtCmdResponse *response);
void quec_qapcmd_handle(const AtCmd *cmd, AtCmdResponse *response);

#ifdef __cplusplus
}
#endif

#endif /* __QUECTEL_AT_QUECTEL_H__ */

