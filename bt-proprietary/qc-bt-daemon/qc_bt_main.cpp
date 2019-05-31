/*******************************************************************************
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 *
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *******************************************************************************/

/******************************************************************************

  @file    qc_bt_main.cpp
  @brief   Main QC_BT Module

  DESCRIPTION
  Implementation of the Main Process thread to interact proprietaries module
  (eg. QCMAP)
 ******************************************************************************/

/*Including necessary header files*/
#include <stdio.h>
#include <unistd.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "QCMAP_Client.h"

#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#endif

#define QC_BT_SOCK_FILE "/data/misc/bluetooth/btappsocket";

#define LOGTAG "qc_bt_daemon"
#define TAG "qc_bt_daemon"

#ifdef USE_ANDROID_LOGGING
#include <utils/Log.h>
#define LOG_TAG "qc_bt_daemon"
#else
#include <syslog.h>
#define ALOGV(fmt, arg...) syslog (LOG_WARNING, fmt, ##arg)
#define ALOGD(fmt, arg...) syslog (LOG_NOTICE, fmt, ##arg)
#define ALOGI(fmt, arg...) syslog (LOG_INFO, fmt, ##arg)
#define ALOGW(fmt, arg...) syslog (LOG_WARNING, fmt, ##arg)
#define ALOGE(fmt, arg...) syslog (LOG_ERR, fmt, ##arg)
#endif

/**
  * @brief BT IPC message type
  */
typedef enum{
    /**
     * ipc message to enable tethering
     */
    BT_IPC_ENABLE_TETHERING = 0x01,
    /**
     * ipc message to disable tethering
     */
    BT_IPC_DISABLE_TETHERING,
    /**
     * ipc message to enable reverse tethering
     */
    BT_IPC_ENABLE_REVERSE_TETHERING,
    /**
     * ipc message to disable reverse tethering
     */
    BT_IPC_DISABLE_REVERSE_TETHERING,
    /**
     * ipc message to start WLAN
     */
    BT_IPC_REMOTE_START_WLAN,
    /**
     * invalid ipc message
     */
    BT_IPC_INAVALID = 0xFF
}bt_ipc_type;

/**
 * @brief BT IPC message status
 */
typedef enum {
    SUCCESS = 0x00,
    FAILED,
    INITIATED,
    INVALID = 0xFF
}bt_ipc_status;

/**
  * @brief BT IPC message between qcbtdaemon & btapp
  */
typedef struct{
    /**
     * It can be any value of bt_ipc_type
     */
    uint8_t type;
    /**
     * It can be any value of bt_ipc_status
     */
    uint8_t status;
}BtIpcMsg;

typedef enum
{
    TETHERED,
    REVERSE_TETHERED,
    UNTETHERED
} bt_tethering_mode_type;

static pthread_t read_thread;
static pthread_mutex_t sock_write_mutex;
static pthread_cond_t sock_write_signal;

static QCMAP_Client * qcmap_client = NULL;
static qmi_error_type_v01 qmi_err_num;

static bool is_interuppted = false;
static bool is_bt_tethering_enabled = false;
static bool is_wlan_enabled = false;
static bt_tethering_mode_type bt_tethering_mode = UNTETHERED;

static bool is_mobileap_enabled = false;
static bool is_backhaul_connected = false;
static bool is_auto_connect_backhaul = false;


static BtIpcMsg write_ipc_msg, *write_ipc_msg_ptr =NULL;
static int conn_sk;

#define BT_IPC_MSG_SIZE 2

static void QcBtStartReadThread();
static void QcBtStopReadThread();
static void QcBtMainExitHandler(int sig);
static bool QCMAPClient();


/*=====================================================================
    FUNCTION main
    ======================================================================*/
    /*!
    @brief
    This function is  main module of qcbtdaemon & does following functionality
    - It's used to create/destroy the socket connection.
    - It's used to create/destroy qcmap instance.
    - It's used to connect/Disconnect backhaul using qcmap client instance.

    @return
    void

    @note

    - Dependencies
    - None

    */
/*===================================================================*/
int main(int argc, char *argv[])
{
    struct sockaddr_un addr;
    struct sigaction actions;
    int fd, len;
    bool status;
    char *socket_path = QC_BT_SOCK_FILE;

    ALOGV("%s: QC BT daemon started", __func__);
    memset(&actions, 0, sizeof(actions));
    sigemptyset(&actions.sa_mask);
    actions.sa_flags = 0;
    actions.sa_handler = QcBtMainExitHandler;

    if (sigaction(SIGTERM,&actions,NULL) < 0) {
       ALOGE("%s: Error in sigaction in %s", __func__, strerror(errno));
    }

    if ((fd = socket(AF_LOCAL, SOCK_STREAM, 0)) == -1) {
        ALOGE("%s: socket error", __func__);
        exit(-1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_LOCAL;
    strlcpy(addr.sun_path, socket_path, sizeof(addr.sun_path));

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        ALOGE("%s: connect error", __func__);
        close(fd);
        exit(-1);
    }
    ALOGV("%s: BT app socket interface successfully connected", __func__);
    conn_sk = fd;

    if (QCMAPClient()) {
        ALOGV("%s: QCMAP client successfully initialized", __func__);

        status = qcmap_client->EnableMobileAP(&qmi_err_num);
        ALOGV("%s, EnableMobileAP : status: %d qmi_err_num: %d", __func__, status, qmi_err_num);

        if (status) {
            is_mobileap_enabled = true;
        }

        if (is_mobileap_enabled) {
            is_backhaul_connected = qcmap_client->ConnectBackHaul(QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01,
                                                                  &qmi_err_num);
            ALOGV("%s: is_backhaul_connected : %d, qmi_err_num: %d",
                  __func__, is_backhaul_connected, qmi_err_num);

            if (is_backhaul_connected == false) {
                // Enable auto connect, to retry to connect to the backhaul on failure.
                status = qcmap_client->SetAutoconnect(true, &qmi_err_num);
                ALOGV("%s: enable auto connect backhaul : status: %d, qmi_err_num: %d",
                      __func__, status, qmi_err_num);

                if (status) {
                    is_auto_connect_backhaul = true;
                }
            }
        } else {
            ALOGE("%s: Enable Mobile AP failed, closing the Socket", __func__);
            delete qcmap_client;
            close(fd);
            exit(-1);
        }
    } else {
        ALOGE("%s: QCMAP client is null", __func__);
        close(fd);
        exit(-1);
    }

    pthread_mutex_init(&sock_write_mutex, NULL);
    pthread_cond_init(&sock_write_signal, NULL);
    QcBtStartReadThread();

    while (1) {
        pthread_mutex_lock(&sock_write_mutex);

        // predicate for spurious wakeups,would also
        // make it robust if there were several consumers
        if (write_ipc_msg_ptr == NULL) {
            //atomically lock/unlock mutex
            pthread_cond_wait(&sock_write_signal, &sock_write_mutex);
        }

        //when daemon receive SIGTERM, is_interuppted is set to true
        if (is_interuppted) {
            pthread_mutex_unlock(&sock_write_mutex);
            break;
        } else {
            len = send (conn_sk, write_ipc_msg_ptr, BT_IPC_MSG_SIZE, 0);

            if (len < 0) {
                ALOGE("%s: Not able to send mesg to remote dev: %s", __func__, strerror(errno));
                is_interuppted = true;
                break;
            } else {
                ALOGV("%s: IPC msg type %d writen  to socket", __func__, write_ipc_msg_ptr->type);
            }

            write_ipc_msg_ptr = NULL;
            pthread_mutex_unlock(&sock_write_mutex);
        }
    }

    QcBtStopReadThread();

    if (qcmap_client) {
        //Cleanup
        if (is_bt_tethering_enabled) {
            status = qcmap_client->BringdownBTTethering(&qmi_err_num);
            ALOGV("%s: Bringdown BT tethering : status: %d, qmi_err_num: %d",
                  __func__, status, qmi_err_num);

            if (status) {
                is_bt_tethering_enabled = false;
                bt_tethering_mode = UNTETHERED;
            }
        }

        if (is_auto_connect_backhaul) {
            status = qcmap_client->SetAutoconnect(false, &qmi_err_num);
            ALOGV("%s: Disable auto connect backhaul : status: %d, qmi_err_num: %d",
                  __func__, status, qmi_err_num);

            if (status) {
                is_auto_connect_backhaul = false;
            }
        }

        if (is_backhaul_connected) {
            status = qcmap_client->DisconnectBackHaul(QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01,
                                                      &qmi_err_num);
            ALOGV("%s: Disconnect backhaul : status: %d, qmi_err_num: %d",
                  __func__, status, qmi_err_num);

            if (status) {
                is_backhaul_connected = false;
            }
        }

        if (is_mobileap_enabled) {
            status = qcmap_client->DisableMobileAP(&qmi_err_num);
            ALOGV("%s: Disable Mobile AP : status: %d, qmi_err_num: %d",
                  __func__, status, qmi_err_num);

            if (status) {
                is_mobileap_enabled = false;
            }
        }

        delete qcmap_client;
    }
    pthread_mutex_destroy(&sock_write_mutex);
    pthread_cond_destroy(&sock_write_signal);
    close(fd);
    ALOGV("%s: QC BT daemon stopped", __func__);
    exit(0);
}

/*=====================================================================
    FUNCTION QcBtReadThreadExitHandler
    ======================================================================*/
    /*!
    @brief
    This function does read thread exit handling

    @return
    void

    @note

    - Dependencies
    - None

    */
/*===================================================================*/
static void QcBtReadThreadExitHandler(int sig)
{
    ALOGV("%s: read exit handler sig: %d", __func__, sig);

    if (sig == SIGUSR1) {
        pthread_exit(0);
    } else {
        ALOGE("%s: Error in read thread exit handler! Sig value not SIGUSR1", __func__);
    }
}

/*=====================================================================
    FUNCTION QcBtMainExitHandler
======================================================================*/
    /*!
    @brief
    This function does main exit handling

    @return
    void

    @note

    - Dependencies
    - None

    */
/*===================================================================*/
static void QcBtMainExitHandler(int sig)
{
    ALOGV("%s: main exit handler sig: %d", __func__, sig);

    if (sig == SIGTERM) {
        is_interuppted =  true;
        pthread_cond_signal(&sock_write_signal);
    } else {
        ALOGE("%s: Error in main exit handler! Sig value not SIGTERM", __func__);
    }
}

/*=====================================================================
FUNCTION QcBtReadThread
======================================================================*/
/*!
@brief
It is used to recieve ipc message from bt app socket
 - enable/disble tetheing
 - enable wlan

@return
void *

@note

- Dependencies
- None

*/
/*===================================================================*/
static void* QcBtReadThread(void *arg)
{

    struct sigaction actions;
    BtIpcMsg ipcMsg;
    int len;
    bool status;

    ALOGV("%s: Started", __func__);
    memset(&actions, 0, sizeof(actions));
    sigemptyset(&actions.sa_mask);
    actions.sa_flags = 0;
    actions.sa_handler = QcBtReadThreadExitHandler;

    if (sigaction(SIGUSR1,&actions,NULL) < 0) {
        ALOGE("%s: Error in sigaction in %s: ", __func__, strerror(errno));
    }

    while(is_interuppted == false) {
        len = recv(conn_sk, &ipcMsg, BT_IPC_MSG_SIZE, 0);

        if (len <= 0) {
            ALOGE("%s: Not able to receive msg to remote dev: %s", __func__, strerror(errno));
            is_interuppted = true;
            pthread_cond_signal(&sock_write_signal);
            break;
        } else if(len == BT_IPC_MSG_SIZE) {
            ALOGV("%s: IPC msg type : %d, IPC msg status: %d",
                  __func__, ipcMsg.type, ipcMsg.status);

            switch(ipcMsg.type) {
                case BT_IPC_ENABLE_TETHERING:
                    ALOGV("%s: BT_IPC_ENABLE_TETHERING", __func__);

                    if (qcmap_client && ipcMsg.status == INITIATED) {
                        status = qcmap_client->BringupBTTethering(&qmi_err_num,
                                                                  QCMAP_MSGR_BT_MODE_LAN_V01);
                        ALOGV("%s: Bringup BTTethering status: %d, qmi_err_num: %d",
                              __func__, status, qmi_err_num);

                        if (write_ipc_msg_ptr == NULL) {
                            write_ipc_msg.type = BT_IPC_ENABLE_TETHERING;
                            write_ipc_msg.status = status ? SUCCESS : FAILED;
                            write_ipc_msg_ptr = &write_ipc_msg;
                            pthread_cond_signal(&sock_write_signal);
                        }

                        if (status) {
                            is_bt_tethering_enabled = true;
                            bt_tethering_mode = TETHERED;
                        }
                    }
                    break;

                case BT_IPC_DISABLE_TETHERING:
                    ALOGV("%s: BT_IPC_DISABLE_TETHERING", __func__);

                    if (qcmap_client && ipcMsg.status == INITIATED) {
                        status = qcmap_client->BringdownBTTethering(&qmi_err_num);
                        ALOGV("%s: Bringdown BTTethering status: %d, qmi_err_num: %d",
                              __func__, status, qmi_err_num);

                        if (write_ipc_msg_ptr == NULL) {
                            write_ipc_msg.type = BT_IPC_DISABLE_TETHERING;
                            write_ipc_msg.status = status ? SUCCESS : FAILED;
                            write_ipc_msg_ptr = &write_ipc_msg;
                            pthread_cond_signal(&sock_write_signal);
                        }

                        if (status) {
                            is_bt_tethering_enabled = false;
                            bt_tethering_mode = UNTETHERED;
                        }
                    }
                    break;

                case BT_IPC_ENABLE_REVERSE_TETHERING:
                    ALOGV("%s: BT_IPC_ENABLE_REVERSE_TETHERING", __func__);

                    if (qcmap_client && ipcMsg.status == INITIATED) {
                        status = qcmap_client->BringupBTTethering(&qmi_err_num,
                                                                  QCMAP_MSGR_BT_MODE_WAN_V01);
                        ALOGV("%s: Bringup BTTethering status: %d, qmi_err_num: %d",
                              __func__, status, qmi_err_num);

                        if (write_ipc_msg_ptr == NULL) {
                            write_ipc_msg.type = BT_IPC_ENABLE_REVERSE_TETHERING;
                            write_ipc_msg.status = status ? SUCCESS : FAILED;
                            write_ipc_msg_ptr = &write_ipc_msg;
                            pthread_cond_signal(&sock_write_signal);
                        }

                        if (status) {
                            is_bt_tethering_enabled = true;
                            bt_tethering_mode = REVERSE_TETHERED;
                        }
                    }
                    break;

                case BT_IPC_DISABLE_REVERSE_TETHERING:
                    ALOGV("%s: BT_IPC_DISABLE_REVERSE_TETHERING", __func__);

                    if (qcmap_client && ipcMsg.status == INITIATED) {
                        status = qcmap_client->BringdownBTTethering(&qmi_err_num);
                        ALOGV("%s: Bringdown BTTethering status: %d, qmi_err_num: %d",
                              __func__, status, qmi_err_num);

                        if (write_ipc_msg_ptr == NULL) {
                            write_ipc_msg.type = BT_IPC_DISABLE_REVERSE_TETHERING;
                            write_ipc_msg.status = status ? SUCCESS : FAILED;
                            write_ipc_msg_ptr = &write_ipc_msg;
                            pthread_cond_signal(&sock_write_signal);
                        }

                        if (status) {
                            is_bt_tethering_enabled = false;
                            bt_tethering_mode = UNTETHERED;
                        }
                    }
                    break;

                case BT_IPC_REMOTE_START_WLAN:
                    ALOGV("%s: BT_IPC_REMOTE_START_WLAN", __func__);

                    if (qcmap_client && ipcMsg.status == INITIATED) {
                        is_wlan_enabled = qcmap_client->EnableWLAN(&qmi_err_num);
                        ALOGV("%s: Enable WLAN status : %d, qmi_err_num: %d",
                              __func__, is_wlan_enabled, qmi_err_num);

                        if (write_ipc_msg_ptr == NULL) {
                            write_ipc_msg.type = BT_IPC_REMOTE_START_WLAN;
                            write_ipc_msg.status = is_wlan_enabled ? SUCCESS : FAILED;
                            write_ipc_msg_ptr = &write_ipc_msg;
                            pthread_cond_signal(&sock_write_signal);
                        }
                    }
                    break;

                default:
                    ALOGV("%s: unknown msg type: %d ", __func__, ipcMsg.type);
                    break;
            }
        }
    }
    ALOGV("%s: Stopped", __func__);
    return (void *)0;
}

/*=====================================================================
FUNCTION QcBtStartReadThread
======================================================================*/
/*!
@brief
It is used to start read thread.

@return
None

@note

- Dependencies
- None

*/
/*===================================================================*/
void QcBtStartReadThread(void)
{

    if (pthread_create(&(read_thread), NULL,
                QcBtReadThread, (void *)NULL) < 0) {
        ALOGE("%s: Unable to create read thread : %s", __func__, strerror(errno));
    }
}

/*=====================================================================
FUNCTION QcBtStopReadThread
======================================================================*/
/*!
@brief
It is used to stop read thread.

@return
None

@note

- Dependencies
- None

*/
/*===================================================================*/
void QcBtStopReadThread(void)
{
    int status;

    if ((status = pthread_kill(read_thread,
                    SIGTERM)) < 0) {
        ALOGE("%s: Error cancelling thread %d, error = %d (%s)",
              __func__, (int)read_thread, status, strerror(status));
    }

    if ((status = pthread_join(read_thread, NULL)) < 0) {
        ALOGE("%s: Error joining thread %d, error = %d (%s)",
              __func__, (int)read_thread, status, strerror(status));
    }

}

/*===========================================================================
  FUNCTION  QcBtQmiQcmapInd
  ===========================================================================*/
/*!
  @brief
  Processes an incoming QMI QCMAP Indication.

  @return
  void

  @note

  - Dependencies
  - None

 */
/*=========================================================================*/
void QcBtQmiQcmapInd
(
 qmi_client_type user_handle,                    /* QMI user handle       */
 unsigned int    msg_id,                         /* Indicator message ID  */
 void           *ind_buf,                        /* Raw indication data   */
 unsigned int    ind_buf_len,                    /* Raw data length       */
 void           *ind_cb_data                     /* User call back handle */
 )
{
    qmi_client_error_type qmi_error;

    ALOGV("%s: user_handle 0x%x msg_id %d ind_buf_len %d.",
          __func__, user_handle, msg_id, ind_buf_len);

    switch (msg_id)
    {
        case QMI_QCMAP_MSGR_BT_TETHERING_STATUS_IND_V01:
        {
            qcmap_msgr_bt_tethering_status_ind_msg_v01 ind_data;

            ALOGV("%s: QMI_QCMAP_MSGR_BT_TETHERING_STATUS_IND_V01", __func__);
            qmi_error =
                    qmi_client_message_decode(user_handle,
                                              QMI_IDL_INDICATION,
                                              msg_id,
                                              ind_buf,
                                              ind_buf_len,
                                              &ind_data,
                                              sizeof(qcmap_msgr_bt_tethering_status_ind_msg_v01));
            if (qmi_error != QMI_NO_ERR) {
                ALOGV("%s:  qmi_client_message_decode error %d", __func__, qmi_error);
                break;
            }

            switch (ind_data.bt_tethering_status)
            {
                case QCMAP_MSGR_BT_TETHERING_MODE_UP_V01:
                    ALOGV("%s:  BT Tethering UP...", __func__);
                    break;

                case QCMAP_MSGR_BT_TETHERING_MODE_DOWN_V01:
                    ALOGV("%s:  BT Tethering DOWN...", __func__);

                    if (qcmap_client && write_ipc_msg_ptr == NULL) {
                        write_ipc_msg.type = BT_IPC_DISABLE_TETHERING;
                        write_ipc_msg.status = SUCCESS;
                        write_ipc_msg_ptr = &write_ipc_msg;
                        pthread_cond_signal(&sock_write_signal);
                    }

                    is_bt_tethering_enabled = false;
                    bt_tethering_mode = UNTETHERED;
                    break;

                default:
                    ALOGV("%s: Unhandled bt tethering status %d",
                          __func__, ind_data.bt_tethering_status);
                    break;

            }
            break;
        }

        case QMI_QCMAP_MSGR_BT_TETHERING_WAN_IND_V01:
        {
            qcmap_msgr_bt_tethering_wan_ind_msg_v01 ind_data;

            ALOGV("%s: QMI_QCMAP_MSGR_BT_TETHERING_WAN_IND_V01", __func__);
            qmi_error =
                    qmi_client_message_decode(user_handle,
                                              QMI_IDL_INDICATION,
                                              msg_id,
                                              ind_buf,
                                              ind_buf_len,
                                              &ind_data,
                                              sizeof(qcmap_msgr_bt_tethering_wan_ind_msg_v01));

            if (qmi_error != QMI_NO_ERR) {
                ALOGV("%s:  qmi_client_message_decode error %d", __func__, qmi_error);
                break;
            }

            switch (ind_data.bt_tethering_wan) {
                case QCMAP_MSGR_BT_WAN_MODE_CONNECTED_V01:
                    ALOGV("%s: QCMAP_MSGR_BT_WAN_MODE_CONNECTED_V01", __func__);
                    break;

                case QCMAP_MSGR_BT_WAN_MODE_DISCONNECTED_V01:
                    ALOGV("%s: QCMAP_MSGR_BT_WAN_MODE_DISCONNECTED_V01", __func__);

                    if (qcmap_client && write_ipc_msg_ptr == NULL)
                    {
                        write_ipc_msg.type = BT_IPC_DISABLE_REVERSE_TETHERING;
                        write_ipc_msg.status = SUCCESS;
                        write_ipc_msg_ptr = &write_ipc_msg;
                        pthread_cond_signal(&sock_write_signal);
                    }

                    is_bt_tethering_enabled = false;
                    bt_tethering_mode = UNTETHERED;
                    break;

                default:
                    ALOGV("%s: Unhandled bt reverse tethering staus: %d",
                          __func__, ind_data.bt_tethering_wan);
            }
            break;
        }

        case QMI_QCMAP_MSGR_STATION_MODE_STATUS_IND_V01:
        {
            ALOGV("%s: QMI_QCMAP_MSGR_STATION_MODE_STATUS_IND_V01", __func__);
            qcmap_msgr_station_mode_status_ind_msg_v01 ind_data;

            qmi_error =
                    qmi_client_message_decode(user_handle,
                                              QMI_IDL_INDICATION,
                                              msg_id,
                                              ind_buf,
                                              ind_buf_len,
                                              &ind_data,
                                              sizeof(qcmap_msgr_station_mode_status_ind_msg_v01));
            if (qmi_error != QMI_NO_ERR) {
                ALOGV("%s: qmi_client_message_decode error %d", __func__,qmi_error);
                break;
            }

            switch (ind_data.station_mode_status) {
                case QCMAP_MSGR_STATION_MODE_CONNECTED_V01:
                    ALOGV("%s: Station mode Connected...", __func__);
                    break;

                case QCMAP_MSGR_STATION_MODE_DISCONNECTED_V01:
                    ALOGV("%s: Station mode Disconnected...", __func__);
                    break;

                case QCMAP_MSGR_STATION_MODE_ASSOCIATION_FAIL_V01:
                    ALOGV("%s: Station mode Association Failed. Going back to AP+STA Router Mode",
                          __func__);
                    break;

                case QCMAP_MSGR_STATION_MODE_DHCP_IP_ASSIGNMENT_FAIL_V01:
                    ALOGV("%s:Station mode IP Assignment via DHCP Failed. switch back to Static IP",
                          __func__);
                    break;

                default:
                    ALOGV("%s: Unhandled station mode status: %d",
                          __func__, ind_data.station_mode_status);
                    break;
            }
            break;
        }

        case QMI_QCMAP_MSGR_WLAN_STATUS_IND_V01:
        {
            ALOGV("%s: QMI_QCMAP_MSGR_WLAN_STATUS_IND_V01", __func__);
            qcmap_msgr_wlan_status_ind_msg_v01 ind_data;
            int i = 0;
            in_addr ip4_addr;
            char ip6_addr[INET6_ADDRSTRLEN];

            qmi_error = qmi_client_message_decode(user_handle,
                                                  QMI_IDL_INDICATION,
                                                  msg_id,
                                                  ind_buf,
                                                  ind_buf_len,
                                                  &ind_data,
                                                  sizeof(qcmap_msgr_wlan_status_ind_msg_v01));
            if (qmi_error != QMI_NO_ERR) {
                ALOGV("%s: qmi_client_message_decode error %d", __func__, qmi_error);
                break;
            }

            switch (ind_data.wlan_status) {
                case QCMAP_MSGR_WLAN_ENABLED_V01:
                    ALOGV("%s: WLAN is ENABLED...", __func__);
                    break;

                case QCMAP_MSGR_WLAN_DISABLED_V01:
                    ALOGV("%s:  WLAN is DISABLED...", __func__);
                    break;

                default:
                    ALOGV("%s:  Invalid wlan status %d ", __func__, ind_data.wlan_status);
                    break;
            }

            switch (ind_data.wlan_mode) {
                case QCMAP_MSGR_WLAN_MODE_AP_V01:
                    ALOGV("%s: WLAN is in AP Mode...", __func__);
                    break;

                case QCMAP_MSGR_WLAN_MODE_AP_AP_V01:
                    ALOGV("%s: WLAN is AP+AP Mode...", __func__);
                    break;

                case QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01:
                    ALOGV("%s: WLAN is AP+AP+AP Mode...", __func__);
                    break;

                case QCMAP_MSGR_WLAN_MODE_AP_STA_V01:
                    ALOGV("%s: WLAN is AP+STA Mode...", __func__);
                    ALOGV("%s: STA is in %s mode...",
                          __func__, (ind_data.bridge_mode ? "Bridge" : "Router"));
                    break;

                case QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01:
                    ALOGV("%s: WLAN is AP+AP+STA Mode...", __func__);
                    ALOGV("%s: STA is in %s mode...",
                          __func__, (ind_data.bridge_mode ? "Bridge" : "Router"));
                    break;

                default:
                    ALOGV("%s:  Invalid wlan mode %d ...", __func__, ind_data.wlan_mode);
                    break;
            }

            for ( i = 0; i < ind_data.wlan_state_len; i++) {
                ALOGV("%s:  WLAN State for Iface %s ",
                      __func__, ind_data.wlan_state[i].wlan_iface_name);
                ALOGV("%s: IP type %d ", __func__, ind_data.wlan_state[i].ip_type);
                ALOGV("%s: Iface type %d ", __func__, ind_data.wlan_state[i].wlan_iface_type);

                switch (ind_data.wlan_state[i].wlan_iface_state) {
                    case QCMAP_MSGR_WLAN_CONNECTED_V01:
                        ALOGV("%s: Iface state is connected...", __func__);
                        break;

                    case QCMAP_MSGR_WLAN_DISCONNECTED_V01:
                        ALOGV("%s: Iface state is disconnected...", __func__);
                        break;

                    case QCMAP_MSGR_WLAN_CONNECTING_V01:
                        ALOGV("%s: Iface state is connecting...", __func__);
                        break;

                   default:
                        ALOGV("%s: Incorrect Iface state %d ",
                              __func__, ind_data.wlan_state[i].wlan_iface_state);
                        break;
                }

                ip4_addr.s_addr = ind_data.wlan_state[i].ip4_addr;
                ALOGV("%s: IP4 address of the iface: %s", __func__, inet_ntoa(ip4_addr));

                inet_ntop(AF_INET6,(void*)&ind_data.wlan_state[i].ip6_addr,
                          ip6_addr, sizeof(ip6_addr));
                ALOGV("%s: IPv6 Address of the iface: %s", __func__, ip6_addr);
            }
            break;
        }

        default:
            ALOGV("%s: Unhandled msg ind: %d", __func__, msg_id);
            break;
    }
}

/*=====================================================================
FUNCTION QCMAPClient
======================================================================*/
/*!
@brief
It is used to create qcmap client instance.

@return
true  - on success
false - on failure

@note

- Dependencies
- None

*/
/*===================================================================*/
bool QCMAPClient()
{
    if (!qcmap_client) {
        qcmap_client = new QCMAP_Client(QcBtQmiQcmapInd);

        if (!qcmap_client) {
            ALOGE("%s: Could not create QCMAP_Client", __func__);
            return false;
        }
    }
    return true;
}
