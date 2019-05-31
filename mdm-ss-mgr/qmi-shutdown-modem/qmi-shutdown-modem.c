/*
 * ---------------------------------------------------------------------------
 *  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
 *  Qualcomm Technologies Proprietary and Confidential.
 * ---------------------------------------------------------------------------
 *
 */

#include <stdio.h>
#include <linux/input.h>
#include <fcntl.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <qmi/qmi.h>
#include "qmi_client.h"
#include "qmi_idl_lib.h"
#include "subsystem_control_v02.h"
#include "device_management_service_v01.h"
#include "qmi-shutdown-modem.h"

#define PWR_KEY_DEVICE "/dev/input/event0"
#define USEC_IN_SEC 1000000
#define SHUTDOWN_TIME_US 2000000
#define POWER_NODE "/sys/power/state"
#define AUTOSLEEP_NODE "/sys/power/autosleep"
#define BUFFER_SIZE 32
#define SUSPEND_STRING "mem"
#define AUTOSLEEP_DISABLE_STRING "off"
#define NO_ERROR 0
#define ERROR -1

#define LOGI(...) fprintf(stderr, "I:" __VA_ARGS__)

qmi_idl_service_object_type ssc_service_object;
qmi_client_type ssc_clnt, ssc_notifier;
qmi_client_type dms_clnt;
int ssc_active = 0;

static volatile int ssc_in_progress = 0;
static pthread_mutex_t ssc_wait = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t lpm_mutex;
pthread_cond_t  lpm_cond_var;

static void dms_indication(qmi_client_type handle,
                                    unsigned long msg_id,
                                    unsigned char *buffer,
                                    int buffer_len,
                                    void *indication_data)
{
    void                         *decoded_payload = NULL;
    uint32_t                      decoded_payload_len;
    qmi_client_error_type         qmi_err;
    dms_event_report_ind_msg_v01 *ind_info;

    LOGI("DMS indication id - %d\n", msg_id);

    qmi_err = qmi_idl_get_message_c_struct_len( dms_get_service_object_v01(),
                                                QMI_IDL_INDICATION,
                                                msg_id,
                                                &decoded_payload_len);

    decoded_payload = malloc( decoded_payload_len );
    if ( decoded_payload != NULL )
    {
        memset(decoded_payload, 0, decoded_payload_len);
        qmi_err = qmi_client_message_decode(
                      dms_clnt,
                      QMI_IDL_INDICATION,
                      msg_id,
                      buffer,
                      buffer_len,
                      decoded_payload,
                      (int)decoded_payload_len );

        switch ( msg_id )
        {
           case QMI_DMS_EVENT_REPORT_IND_V01:
                ind_info = (dms_event_report_ind_msg_v01*)decoded_payload;
                if ( ind_info->operating_mode_valid )
                {
                    if ( ind_info->operating_mode == DMS_OP_MODE_LOW_POWER_V01 )
                    {
                        LOGI("Modem in LPM state.. proceed with rest of shutdown procedure\n");
                        pthread_mutex_lock(&lpm_mutex);
                        pthread_cond_signal(&lpm_cond_var);
                        pthread_mutex_unlock(&lpm_mutex);
                    }
                }
                break;

          default:
            break;
        }
    }

}


/*
 * Change modem mode to LPM
 */
int process_modem_lpm()
{
    int rc = ERROR;
    int wait_for_status = 0;
    qmi_service_info info;
    qmi_cci_os_signal_type os_params;
    qmi_client_error_type  client_err = 0;
    dms_set_operating_mode_req_msg_v01   qmi_request;
    dms_set_operating_mode_resp_msg_v01  qmi_response;
    dms_get_operating_mode_resp_msg_v01  qmi_status_resp;
    dms_set_event_report_req_msg_v01     qmi_event_request;
    dms_set_event_report_resp_msg_v01    qmi_event_response;

    memset(&qmi_request, 0, sizeof(qmi_request));
    memset(&qmi_response, 0, sizeof(qmi_response));
    memset(&qmi_status_resp, 0, sizeof(qmi_status_resp));
    memset(&qmi_event_request, 0, sizeof(qmi_event_request));
    memset(&qmi_event_response, 0, sizeof(qmi_event_response));

    LOGI("Initialize QMI DMS Client\n");
    client_err = qmi_client_init_instance(dms_get_service_object_v01(),
                                          QMI_CLIENT_INSTANCE_ANY,
                                          dms_indication,
                                          NULL,
                                          &os_params,
                                          TIMEOUT_MS,
                                          &dms_clnt);

    if ( client_err != QMI_NO_ERR )
    {
        return -EINVAL;
    }

    memset(&qmi_status_resp,0,sizeof(qmi_status_resp));
    client_err = qmi_client_send_msg_sync( dms_clnt,
                                           QMI_DMS_GET_OPERATING_MODE_REQ_V01,
                                           NULL,
                                           0,
                                           (void*) &qmi_status_resp,
                                           sizeof( qmi_status_resp ),
                                           TIMEOUT_MS );

    if ( client_err != QMI_NO_ERR )
    {
        return -EINVAL;
    }

    LOGI("Current Modem status - %d\n", qmi_status_resp.operating_mode);

    // If modem online, move it to LPM state.
    if ( qmi_status_resp.operating_mode == DMS_OP_MODE_ONLINE_V01 )
    {
        qmi_event_request.report_oprt_mode_state_valid = 1;
        qmi_event_request.report_oprt_mode_state = 1;

        client_err = qmi_client_send_msg_sync ( dms_clnt,
                                                QMI_DMS_SET_EVENT_REPORT_REQ_V01,
                                                (void*) &qmi_event_request,
                                                sizeof( qmi_event_request ),
                                                (void*) &qmi_event_response,
                                                sizeof( qmi_event_response ),
                                                TIMEOUT_MS);

        if ( client_err != QMI_NO_ERR )
        {
            return -EINVAL;
        }

        LOGI("Move modem status to LPM\n");
        qmi_request.operating_mode = DMS_OP_MODE_LOW_POWER_V01;
        client_err = qmi_client_send_msg_sync( dms_clnt,
                                               QMI_DMS_SET_OPERATING_MODE_REQ_V01,
                                               &qmi_request,
                                               sizeof( qmi_request ),
                                               &qmi_response,
                                               sizeof( qmi_response ),
                                               TIMEOUT_MS );

        if ( client_err != QMI_NO_ERR )
        {
            return -EINVAL;
        }

        wait_for_status = 1;
    }

    if ( wait_for_status )
    {
        pthread_mutex_lock(&lpm_mutex);
        LOGI("Wait for modem LPM complete\n");
        pthread_cond_wait(&lpm_cond_var, &lpm_mutex);
        pthread_mutex_unlock(&lpm_mutex);
    }

    qmi_client_release(dms_clnt);
    dms_clnt = 0;

    return 0;
}

int time_diff_us(struct timeval *press, struct timeval *release)
{
    int time;
    if (release->tv_usec > press->tv_usec) {
      release->tv_usec += USEC_IN_SEC;
      release->tv_sec--;
    }
    time = (int) (release->tv_sec - press->tv_sec) * USEC_IN_SEC +
                release->tv_usec - press->tv_usec;
    return time;
}

void suspend_or_resume(void)
{
    int fd = -1;
    char buf[BUFFER_SIZE];
    static int suspend = 1;

    /* Open socket to /sys/power/state */
    fd = open(POWER_NODE, O_WRONLY);
    if (fd < 0)
    {
        LOGI("Failed to open a socket to POWER_NODE\n");
        goto close;
    }

    if (suspend == 1)
    {
        strlcpy (buf, SUSPEND_STRING,sizeof(buf));
        if (write(fd, buf, strlen(buf)) == -1)
        {
            LOGI("Suspend failed %d (%s)\n", errno, strerror(errno));
        }
        else
        {
            suspend = 0;
        }
    }
    else
    {
        suspend = 1;
    }

close:
    close(fd);
}

static int qmi_send_msg_sync(int cmd, void *req, int req_len, void *resp,
        int resp_len)
{
    int rc;
    rc = qmi_client_send_msg_sync(ssc_clnt, cmd, req, req_len, resp,
            resp_len, TIMEOUT_MS);
    return rc;
}

static void ssc_qmi_send_cmd(struct ssc_req *client_req,
        struct ssc_resp *client_resp)
{
    int rc, req_len, resp_len, cmd, retry = 0;
    void *req, *resp;
    struct timespec ts;
    qmi_ssctl_restart_resp_msg_v02 restart_resp;
    qmi_ssctl_shutdown_resp_msg_v02 shutdown_resp;
    qmi_ssctl_get_failure_reason_resp_msg_v02 failure_reason_resp;

    if (client_req->cmd < 0 ||
        client_req->cmd >= (sizeof(qmi_command)/sizeof(qmi_service_instance))) {
        client_resp->rc = -1;
        return;
    }

    cmd = qmi_command[client_req->cmd];
    client_resp->length = 0;

    switch (client_req->cmd) {
        case CMD_SHUTDOWN:
            pthread_mutex_lock(&ssc_wait);
            req = NULL;
            req_len = 0;
            ts.tv_sec = 0;
            ts.tv_nsec = 100000000L;
            resp = &shutdown_resp;
            resp_len = sizeof(shutdown_resp);
            rc = qmi_send_msg_sync(cmd, req, req_len, resp, resp_len);
            if (rc == QMI_NO_ERR) {
                client_resp->rc = shutdown_resp.resp.error;
                for (retry = 0; retry <= 100; retry++) {
                    rc = pthread_mutex_trylock(&ssc_wait);
                    if (rc == 0)
                        break;
                    nanosleep(&ts, NULL);
                }
                if (rc == 0)
                    LOGI("Received subsystem shutdown complete in %d second(s)", retry);
                else {
                    LOGI("Timed out waiting for shutdown completion; continuing anyways");
                    client_resp->rc = -1;
                }
            } else
                client_resp->rc = -1;
            pthread_mutex_unlock(&ssc_wait);
            break;

        default:
            client_resp->rc = -1;
    };
}

static void ssc_indication(qmi_client_type handle, unsigned long msg_id,
                unsigned char *buffer, int buffer_len, void *indication_data)
{
    if (msg_id == QMI_SSCTL_SHUTDOWN_READY_IND_V02)
        pthread_mutex_unlock(&ssc_wait);
    else
        LOGI("Got unknown subsystem control indicator 0x%08lx", msg_id);
}

static int ssc_qmi_init(int proc_num)
{
    qmi_service_info info;
    qmi_cci_os_signal_type os_params;
    qmi_client_error_type  client_err = 0;

    client_err = qmi_client_init_instance(ssctl_get_service_object_v02(),
                                          QMI_CLIENT_INSTANCE_ANY,
                                          ssc_indication,
                                          NULL,
                                          &os_params,
                                          TIMEOUT_MS,
                                          &ssc_clnt);

    if ( client_err != QMI_NO_ERR )
    {
        return -EINVAL;
    }

    ssc_active = 1;
    return 0;
}

static void ssc_qmi_release(void)
{
    if(ssc_active)
    {
        qmi_client_release(ssc_clnt);
        // qmi_client_release(ssc_notifier);
        ssc_active = 0;
    }
}

static int ssc_process_command(struct ssc_req *req, struct ssc_resp *resp)
{
    int rc;

    if((req->cmd < CMD_START) || (req->cmd > CMD_END))
    {
        LOGI("CMD[%u] is invalid.", req->cmd);
        return -EINVAL;
    }

    rc = ssc_qmi_init(req->proc_num);

    if(rc)
    {
        LOGI("Failed to do QMI init.\n");
        return -EINVAL;
    }

    process_modem_lpm();

    LOGI("In the shutdown path");
    ssc_qmi_send_cmd(req, resp);
    ssc_qmi_release();

    return 0;
}

int subsystem_control_shutdown(unsigned proc_num)
{
    int rc;
    struct ssc_req req;
    struct ssc_resp resp;

    LOGI("subsystem_control_shutdown\n");

    if((proc_num < PROC_START) || (proc_num > PROC_END))
    {
        LOGI("PROC ID[%u] is invalid.\n", proc_num);
        return -EINVAL;
    }

    if (!__sync_bool_compare_and_swap(&ssc_in_progress, 0, 1))
    {
        LOGI("Shutdown request already in progress");
        return -EINVAL;
    }

    req.proc_num = proc_num;
    req.cmd = CMD_SHUTDOWN;
    resp.rc = -1;

    rc = ssc_process_command(&req, &resp);
    __sync_bool_compare_and_swap(&ssc_in_progress, 1, 0);
    if(rc)
    {
        LOGI("Failed to process shutdown command.\n");
        return rc;
    }

    LOGI("Success.\n");

    return resp.rc;
}

/*
 * if argc == 1, program runs as a daemon waiting for power key event
 * if argc == 2 (called by "modem-shutdown.sh"), shut down the modem immediately
 */
int main(int argc, char* argv[])
{
    int rc, fd, read_size, keypress=0;
    struct input_event event;
    struct timeval press, release;
    char buf[BUFFER_SIZE];

    pthread_mutex_init(&lpm_mutex, NULL);
    pthread_cond_init (&lpm_cond_var, NULL);

    if(argc == 1){
        /* wait for power key event */
        fd = open(PWR_KEY_DEVICE, O_RDONLY);
        if (fd < 0) {
            LOGI("open pwr key device failed: %s\n", PWR_KEY_DEVICE);
            qmi_client_release(ssc_clnt);
        }

        memset(&press, 0, sizeof(struct timeval));
        memset(&release, 0, sizeof(struct timeval));

        while (1) {
            read_size = read(fd, &event, sizeof(struct input_event));
            if (read_size < sizeof(struct input_event)) {
                qmi_client_release(ssc_clnt);
            }
            if (event.type == EV_KEY && event.code == KEY_POWER) {
                if (event.value == 1){
                    memcpy(&press, &event.time, sizeof(struct timeval));
                    keypress=1;
                }
                else {
                    memcpy(&release, &event.time, sizeof(struct timeval));
                    /* Sometimes timeval is negative when reading the
                     * event time from pmic. Add a check to process suspend or
                     * shutdown request only when the values are non-negative.
                     */
                    if((press.tv_sec >= 0) && (release.tv_sec >= 0) && (keypress==1)) {
                            keypress=0;
                            if (time_diff_us(&press, &release) < SHUTDOWN_TIME_US) {
                                suspend_or_resume();
                            } else {
                                fd = open(AUTOSLEEP_NODE, O_WRONLY);
                                strlcpy (buf, AUTOSLEEP_DISABLE_STRING,sizeof(buf));
                                errno = 0;
                                if (write(fd, buf, strlen(buf)) == -1)
                                    printf("autosleep disable failed %d (%s)\n", errno, strerror(errno));
                                else
                                    close(fd);
                                /*shutdown*/
                                LOGI("Going to shutdown.\n");
                                break;
                            }
                        }
                }
            }
        }
    }

#ifndef ENABLE_TARGET_APQ
    rc = subsystem_control_shutdown((unsigned)PROC_MSM);

    if(!rc)
        LOGI("Modem shutdown complete\n");
    else
        LOGI("Modem shutdown command failed\n");
#endif

    /* halt APPS only if modem is shut down by power key */
    if(argc == 1) {
        system("halt");
    }

    pthread_mutex_destroy(&lpm_mutex);
    pthread_cond_destroy(&lpm_cond_var);

    return 0;
}
