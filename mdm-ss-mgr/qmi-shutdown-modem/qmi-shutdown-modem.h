/******************************************************************************

  @file	 qmi-shutdown-modem.h

  ---------------------------------------------------------------------------
  Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
 ******************************************************************************/

#ifndef __QMI_SHUTDOWN_MODEM_H__
#define __QMI_SHUTDOWN_MODEM_H__

#include <subsystem_control.h>
#define MSG_LENGTH 90
#define TIMEOUT_MS 3000

enum command {
    CMD_START,
    CMD_SHUTDOWN = CMD_START,
    CMD_END = CMD_SHUTDOWN,
};

unsigned int qmi_command [] = {
    [CMD_SHUTDOWN] = QMI_SSCTL_SHUTDOWN_REQ_V02,
};

qmi_service_instance qmi_instance_id[] = {
    [PROC_MSM] = 0,
};

struct ssc_req {
    enum procs proc_num;
    enum command cmd;
};

struct ssc_resp {
    int rc;
    int length;
    char msg[MSG_LENGTH];
};

#endif
